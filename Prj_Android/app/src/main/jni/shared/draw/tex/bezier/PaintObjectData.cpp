/*+----------------------------------------------------------------+
  |	Title:		PaintObjectData.cpp [共通環境]
  |	Comment:	塗りデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "PaintObjectData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_PAINT_OBJECT_DATA   (4)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CPaintObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_PAINT_OBJECT_DATA )

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------------------------
// 確保メモリセル数（※管理内で[new]するセル数）
//----------------------------------------------
int CPaintObjectData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CPaintObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_PAINT_OBJECT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CPaintObjectData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CPaintObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_PAINT_OBJECT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CPaintObjectData) * MAX_ALLOC_PAINT_OBJECT_DATA;
    
    return( allocSize );
}

//-------------------------
// コンストラクタ
//-------------------------
CPaintObjectData::CPaintObjectData( void ): CLayerObject(eLAYER_OBJECT_TYPE_PAINT), CDataList(true){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CPaintObjectData::~CPaintObjectData( void ){
    clear();    // リスト解放
}

//--------------------
// クリア（※データの破棄）
//--------------------
void CPaintObjectData::clear( void ){
    clearTempAdjust();
    m_nFlag = 0;
    
    // リスト解放
    CFillPointData* pFPD = getDataHead();
    while( pFPD != NULL ){
        CFillPointData* pNext = pFPD->getNext();
        CFillPointData::Free( pFPD );
        pFPD = pNext;
    }
    m_oDataList.disconnectAll();
    
    // 各種値
    m_eBucketId = eBUCKET_INVALID;
    m_eDefaultPalOfsId = ePAL_OFS_INVALID;
    m_eTestPalOfsId = ePAL_OFS_INVALID;
}

//--------------------
// コピー
//--------------------
void CPaintObjectData::copy( CPaintObjectData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    // クリア（※リストの解放）
    clear();

    m_nFlag = pData->m_nFlag;

    // リストのコピー
    CFillPointData* pFPD = pData->getDataHead();
    while( pFPD != NULL ){
        CFillPointData* pNew = CFillPointData::Alloc();
        pNew->copy( pFPD );
        addData( pNew );
        pFPD = pFPD->getNext();
    }

    // 各種値
    m_eBucketId = pData->m_eBucketId;
    m_eDefaultPalOfsId = pData->m_eDefaultPalOfsId;
    m_eTestPalOfsId = pData->m_eTestPalOfsId;
}

//---------------------------
// 読み込み
//---------------------------
void CPaintObjectData::read( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();

    // 要素数
    m_nFlag = pIB->readInt16();
    
    // 要素読み込み
    int n = pIB->readInt16();
    for( int i=0; i<n; i++ ){
        CFillPointData* pFPD = CFillPointData::Alloc();
        pFPD->read( pIB );
        addData( pFPD );
    }
    
    // 各種値
    m_eBucketId = (eBUCKET)CEnum::ReadEnum( pIB, g_pArrLabelBucket );
    m_eDefaultPalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
    m_eTestPalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
}

//---------------------------
// 書き込み
//---------------------------
void CPaintObjectData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    
    // 要素数
    int num = getDataSize();
    pOB->writeInt16( (int16)num );

    // 要素書き込み
    CFillPointData* pFPD = getDataHead();
    while( pFPD != NULL ){
        pFPD->write( pOB );
        pFPD = pFPD->getNext();
    }

    // 各種値
    CEnum::WriteEnum( pOB, m_eBucketId, eBUCKET_MAX, g_pArrLabelBucket );
    CEnum::WriteEnum( pOB, m_eDefaultPalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
    CEnum::WriteEnum( pOB, m_eTestPalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
}

//---------------------------
// 一時調整の値を適用する
//---------------------------
// 基本
void CPaintObjectData::applyTempAdjust( void ){
    CFillPointData* pFPD = getDataHead();
    while( pFPD != NULL ){
        CAdjustablePoint* pXY = pFPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfs( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pFPD = pFPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｈ最大
void CPaintObjectData::applyTempAdjustForMaxH( void ){
    CFillPointData* pFPD = getDataHead();
    while( pFPD != NULL ){
        CAdjustablePoint* pXY = pFPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMaxH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pFPD = pFPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｈ最小
void CPaintObjectData::applyTempAdjustForMinH( void ){
    CFillPointData* pFPD = getDataHead();
    while( pFPD != NULL ){
        CAdjustablePoint* pXY = pFPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMinH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pFPD = pFPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｖ最大
void CPaintObjectData::applyTempAdjustForMaxV( void ){
    CFillPointData* pFPD = getDataHead();
    while( pFPD != NULL ){
        CAdjustablePoint* pXY = pFPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMaxV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pFPD = pFPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｖ最小
void CPaintObjectData::applyTempAdjustForMinV( void ){
    CFillPointData* pFPD = getDataHead();
    while( pFPD != NULL ){
        CAdjustablePoint* pXY = pFPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMinV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pFPD = pFPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

//------------
// スケール適用
//------------
void CPaintObjectData::applyRateScale( int rateScale ){
    CFillPointData* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // [m_eBucketId]は無視
    // [m_eDefaultPalOfsId]は無視
    // [m_eTestPalOfsId]は無視
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CPaintObjectData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 3 + 4 + ePOD_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "BUCKET_ID", &m_eBucketId, eEDIT_VALUE_TYPE_INT32,
                             eBUCKET_INVALID, eBUCKET_MAX-1, g_pArrLabelBucket );
    pMenu->setItemAtAsLabel( id++, "DEF_PAL_F_ID", &m_eDefaultPalOfsId, eEDIT_VALUE_TYPE_INT32,
                             ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );
    pMenu->setItemAtAsLabel( id++, "TEST_PAL_F_ID", &m_eTestPalOfsId, eEDIT_VALUE_TYPE_INT32,
                             ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );
    
    pMenu->setSeparatorAt( id, true );

    // 一時調整値（※この値は保存されない＝修正後に[applyTempAdjust]で調整を適用する想定）
    pMenu->setItemAt( id++, "TEMP: X", &m_nTempAdjustRateOfsX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: Y", &m_nTempAdjustRateOfsY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_X", &m_nTempAdjustRateScaleX, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_Y", &m_nTempAdjustRateScaleY, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, ePOD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_GUIDE_AFTER_FILL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, ePOD_FLAG_STAY_GUIDE_AFTER_FILL );
    pMenu->setItemAtAsBit( id++, "FLAG: GUIDE_RESET_FULL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, ePOD_FLAG_GUIDE_RESET_FULL );
    
    // 確定
    pMenu->fixMenu();
}
