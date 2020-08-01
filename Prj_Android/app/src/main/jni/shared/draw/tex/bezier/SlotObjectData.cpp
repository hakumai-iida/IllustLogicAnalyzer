/*+----------------------------------------------------------------+
  |	Title:		SlotObjectData.cpp [共通環境]
  |	Comment:	スロットデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "SlotObjectData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_SLOT_OBJECT_DATA   (4)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_OBJECT_DATA )

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
int CSlotObjectData::GetAllocCellNum( void ){
    int num = 0;
    
    //-----------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_OBJECT_DATA )
    //-----------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CSlotObjectData::GetAllocSize( void ){
    int allocSize = 0;
    
    //-----------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_OBJECT_DATA )
    //-----------------------------------------------------------------------------------------------------
    allocSize += sizeof(CSlotObjectData) * MAX_ALLOC_SLOT_OBJECT_DATA;
    
    return( allocSize );
}

//-------------------------
// コンストラクタ
//-------------------------
CSlotObjectData::CSlotObjectData( void ): CLayerObject(eLAYER_OBJECT_TYPE_SLOT), CDataList(true){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CSlotObjectData::~CSlotObjectData( void ){
    clear();    // リスト解放
}

//--------------------
// クリア（※データの破棄）
//--------------------
void CSlotObjectData::clear( void ){
    clearTempAdjust();
    m_nFlag = 0;
    
    // リスト解放
    CSlotPointData* pSPD = getDataHead();
    while( pSPD != NULL ){
        CSlotPointData* pNext = pSPD->getNext();
        CSlotPointData::Free( pSPD );
        pSPD = pNext;
    }
    m_oDataList.disconnectAll();
    
    // 各種値
}

//--------------------
// コピー
//--------------------
void CSlotObjectData::copy( CSlotObjectData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    // クリア（※リストの解放）
    clear();

    m_nFlag = pData->m_nFlag;

    // リストのコピー
    CSlotPointData* pSPD = pData->getDataHead();
    while( pSPD != NULL ){
        CSlotPointData* pNew = CSlotPointData::Alloc();
        pNew->copy( pSPD );
        addData( pNew );
        pSPD = pSPD->getNext();
    }

    // 各種値
}

//---------------------------
// 読み込み
//---------------------------
void CSlotObjectData::read( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();

    // 要素数
    m_nFlag = pIB->readInt16();
    
    // 要素読み込み
    int n = pIB->readInt16();
    for( int i=0; i<n; i++ ){
        CSlotPointData* pSPD = CSlotPointData::Alloc();
        pSPD->read( pIB );
        addData( pSPD );
    }
    
    // 各種値
}

//---------------------------
// 書き込み
//---------------------------
void CSlotObjectData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    
    // 要素数
    int num = getDataSize();
    pOB->writeInt16( (int16)num );
    
    // 要素書き込み
    CSlotPointData* pSPD = getDataHead();
    while( pSPD != NULL ){
        pSPD->write( pOB );
        pSPD = pSPD->getNext();
    }

    // 各種値
}

//---------------------------
// 一時調整の値を適用する
//---------------------------
// 基本
void CSlotObjectData::applyTempAdjust( void ){
    CSlotPointData* pSPD = getDataHead();
    while( pSPD != NULL ){
        CAdjustablePoint* pXY = pSPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfs( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pSPD = pSPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｈ最大
void CSlotObjectData::applyTempAdjustForMaxH( void ){
    CSlotPointData* pSPD = getDataHead();
    while( pSPD != NULL ){
        CAdjustablePoint* pXY = pSPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMaxH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pSPD = pSPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｈ最小
void CSlotObjectData::applyTempAdjustForMinH( void ){
    CSlotPointData* pSPD = getDataHead();
    while( pSPD != NULL ){
        CAdjustablePoint* pXY = pSPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMinH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pSPD = pSPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｖ最大
void CSlotObjectData::applyTempAdjustForMaxV( void ){
    CSlotPointData* pSPD = getDataHead();
    while( pSPD != NULL ){
        CAdjustablePoint* pXY = pSPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMaxV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pSPD = pSPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// Ｖ最小
void CSlotObjectData::applyTempAdjustForMinV( void ){
    CSlotPointData* pSPD = getDataHead();
    while( pSPD != NULL ){
        CAdjustablePoint* pXY = pSPD->getAdjXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMinV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        pSPD = pSPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

//------------
// スケール適用
//------------
void CSlotObjectData::applyRateScale( int rateScale ){
    CSlotPointData* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CSlotObjectData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 0 + 4 + eSOD_FLAG_MAX );
    
    int id = 0;
    
    // 値
    
    // 一時調整値（※この値は保存されない＝修正後に[applyTempAdjust]で調整を適用する想定）
    pMenu->setItemAt( id++, "TEMP: X", &m_nTempAdjustRateOfsX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: Y", &m_nTempAdjustRateOfsY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_X", &m_nTempAdjustRateScaleX, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_Y", &m_nTempAdjustRateScaleY, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, ePOD_FLAG_DISABLE );
    
    // 確定
    pMenu->fixMenu();
}
