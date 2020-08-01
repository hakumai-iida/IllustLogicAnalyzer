/*+----------------------------------------------------------------+
  |	Title:		LineObjectData.cpp [共通環境]
  |	Comment:	線データ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LineObjectData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_LINE_OBJECT_DATA   (16)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLineObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LINE_OBJECT_DATA )

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
int CLineObjectData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLineObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LINE_OBJECT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CLineObjectData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLineObjectData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LINE_OBJECT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CLineObjectData) * MAX_ALLOC_LINE_OBJECT_DATA;
    
    return( allocSize );
}

//-------------------------
// コンストラクタ
//-------------------------
CLineObjectData::CLineObjectData( void ): CLayerObject(eLAYER_OBJECT_TYPE_LINE), CDataList(true){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CLineObjectData::~CLineObjectData( void ){
    clear();    // リストの解放
}

//--------------------
// クリア（※データの破棄）
//--------------------
void CLineObjectData::clear( void ){
    clearTempAdjust();
    m_nFlag = 0;
    
    // リスト解放
    CAnchorPointData* pAPD = getDataHead();
    while( pAPD != NULL ){
        CAnchorPointData* pNext = pAPD->getNext();
        CAnchorPointData::Free( pAPD );
        pAPD = pNext;
    }
    m_oDataList.disconnectAll();
    
    // 各種値
    m_eBrushId = eBRUSH_INVALID;
    m_eTestPalOfsId = ePAL_OFS_INVALID;
    m_eTestPalOfsIdForRepair = ePAL_OFS_INVALID;
}

//--------------------
// コピー
//--------------------
void CLineObjectData::copy( CLineObjectData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }

    // クリア（※リストの解放）
    clear();
    
    m_nFlag = pData->m_nFlag;

    // リストのコピー
    CAnchorPointData* pAPD = pData->getDataHead();
    while( pAPD != NULL ){
        CAnchorPointData* pNew = CAnchorPointData::Alloc();
        pNew->copy( pAPD );
        addData( pNew );
        pAPD = pAPD->getNext();
    }

    // 各種値
    m_eBrushId = pData->m_eBrushId;
    m_eTestPalOfsId = pData->m_eTestPalOfsId;
    m_eTestPalOfsIdForRepair = pData->m_eTestPalOfsIdForRepair;
}

//---------------------------
// 読み込み
//---------------------------
void CLineObjectData::read( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();

    // 要素数
    m_nFlag = pIB->readInt16();

    // 要素読み込み
    int n = pIB->readInt16();
    for( int i=0; i<n; i++ ){
        CAnchorPointData* pAPD = CAnchorPointData::Alloc();
        pAPD->read( pIB );
        addData( pAPD );
    }

    // 各種値
    m_eBrushId = (eBRUSH)CEnum::ReadEnum( pIB, g_pArrLabelBrush );
    m_eTestPalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
    m_eTestPalOfsIdForRepair = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
}

//---------------------------
// 書き込み
//---------------------------
void CLineObjectData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );

    // 要素数
    int num = getDataSize();
    pOB->writeInt16( (int16)num );
    
    // 要素書き込み
    CAnchorPointData* pAPD = getDataHead();
    while( pAPD != NULL ){
        pAPD->write( pOB );
        pAPD = pAPD->getNext();
    }

    // 各種値
    CEnum::WriteEnum( pOB, m_eBrushId, eBRUSH_MAX, g_pArrLabelBrush );
    CEnum::WriteEnum( pOB, m_eTestPalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
    CEnum::WriteEnum( pOB, m_eTestPalOfsIdForRepair, ePAL_OFS_MAX, g_pArrLabelPalOfs );
}

//---------------------------
// 一時調整の値を適用する
//---------------------------
// 調整値適用：基本
void CLineObjectData::applyTempAdjust( void ){
    CAnchorPointData* pAPD = getDataHead();
    while( pAPD != NULL ){
        CAdjustablePoint* pXY = pAPD->getAdjXY();
        CAdjustablePoint* pInXY = pAPD->getAdjInXY();
        CAdjustablePoint* pOutXY = pAPD->getAdjOutXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfs( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        // 方向線にはスケールのみを適用（※オフセットは不要＝方向線は[pXY]に対する相対座標なので）
        pInXY->applyScale( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pOutXY->applyScale( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );

        pAPD = pAPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// 調整値適用：Ｈ最大
void CLineObjectData::applyTempAdjustForMaxH( void ){
    CAnchorPointData* pAPD = getDataHead();
    while( pAPD != NULL ){
        CAdjustablePoint* pXY = pAPD->getAdjXY();
        CAdjustablePoint* pInXY = pAPD->getAdjInXY();
        CAdjustablePoint* pOutXY = pAPD->getAdjOutXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMaxH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        // 方向線にはスケールのみを適用（※オフセットは不要＝方向線は[pXY]に対する相対座標なので）
        pInXY->applyScaleForMaxH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pOutXY->applyScaleForMaxH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        
        pAPD = pAPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// 調整値適用：Ｈ最小
void CLineObjectData::applyTempAdjustForMinH( void ){
    CAnchorPointData* pAPD = getDataHead();
    while( pAPD != NULL ){
        CAdjustablePoint* pXY = pAPD->getAdjXY();
        CAdjustablePoint* pInXY = pAPD->getAdjInXY();
        CAdjustablePoint* pOutXY = pAPD->getAdjOutXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMinH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        // 方向線にはスケールのみを適用（※オフセットは不要＝方向線は[pXY]に対する相対座標なので）
        pInXY->applyScaleForMinH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pOutXY->applyScaleForMinH( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        
        pAPD = pAPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// 調整値適用：Ｖ最大
void CLineObjectData::applyTempAdjustForMaxV( void ){
    CAnchorPointData* pAPD = getDataHead();
    while( pAPD != NULL ){
        CAdjustablePoint* pXY = pAPD->getAdjXY();
        CAdjustablePoint* pInXY = pAPD->getAdjInXY();
        CAdjustablePoint* pOutXY = pAPD->getAdjOutXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMaxV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        // 方向線にはスケールのみを適用（※オフセットは不要＝方向線は[pXY]に対する相対座標なので）
        pInXY->applyScaleForMaxV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pOutXY->applyScaleForMaxV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        
        pAPD = pAPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

// 調整値適用：Ｖ最小
void CLineObjectData::applyTempAdjustForMinV( void ){
    CAnchorPointData* pAPD = getDataHead();
    while( pAPD != NULL ){
        CAdjustablePoint* pXY = pAPD->getAdjXY();
        CAdjustablePoint* pInXY = pAPD->getAdjInXY();
        CAdjustablePoint* pOutXY = pAPD->getAdjOutXY();
        
        // 座標にはスケール＆オフセットを適用
        pXY->applyScaleThenOfsForMinV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY, m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY );
        
        // 方向線にはスケールのみを適用（※オフセットは不要＝方向線は[pXY]に対する相対座標なので）
        pInXY->applyScaleForMinV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pOutXY->applyScaleForMinV( m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        
        pAPD = pAPD->getNext();
    }
    
    // 調整値クリア
    clearTempAdjust();
}

//-----------------------------------------------
// アンカーポイントの追加（※現在選択中の点と次の点の中間）
//-----------------------------------------------
CAnchorPointData* CLineObjectData::addDataAtIntermediate( void ){
    CAnchorPointData* pNew = addData();
    
    // 要素が３つ以上であれば（※上で追加したデータ込みで３以上＝追加前は２点以上）
    int num = getDataSize();
    if( num >= 3 ){
        int select = getSelect();
        CAnchorPointData* pPrev;
        CAnchorPointData* pNext;

        // 先頭であれば末端を取得
        if( select == 0 ){
            pPrev = getDataTail();
        }
        // 前の要素があれば取得
        else{
            pPrev = pNew->getPrev();
        }

        // 末端であれば先頭を取得
        if( select >= (num-1) ){
            pNext = getDataHead();
        }
        // 次の要素があれば取得
        else{
            pNext = pNew->getNext();
        }
        
        // 新規要素を２点間の中央に配置
        pNew->setAtIntermediate( pPrev, pNext );
    }
    
    return( pNew );
}

//------------------------
// 鏡面コピー
//------------------------
bool CLineObjectData::mirrorCopy0( bool isVertical ){
    // 無効は無視（※要素数が３に満たない場合はコピーできない）
    int num = getDataSize();
    if( num < 3 ){
        LOGE( "@ CLineObjectData::mirrorCopy0: FAILED: num=%d, isV=%d\n", num, isVertical );
        return( false );
    }
    
    // 処理対象の抽出（※[末尾２番目の要素]から[先頭２番目の要素]までを[末尾の後ろ]から追加）
    CList* pList = getDataList();
    CListNode* pHead = pList->getHead();
    CListNode* pTail = pList->getTail();
    CListNode* pNode = pTail->getPrev0();

    // リストの操作対象は末尾要素
    setSelect( num - 1 );

    // 追加開始
    while( pNode != pHead ){
        CAnchorPointData* pSrc = (CAnchorPointData*) pNode;
        CAnchorPointData* pNew = CAnchorPointData::Alloc();
        pNew->copy( pSrc );

        // コピー方向に応じて調整
        if( isVertical ){
            pNew->fixForMirrorCopyV();
        }else{
            pNew->fixForMirrorCopyH();
        }

        addData( pNew );
        
        pNode = pNode->getPrev0();
    }
    
    // 先頭と末尾(元)の調整
    if( isVertical ){
        ((CAnchorPointData*)pHead)->fixAsHeadForMirrorCopyV();
        ((CAnchorPointData*)pTail)->fixAsTailForMirrorCopyV();
    }else{
        ((CAnchorPointData*)pHead)->fixAsHeadForMirrorCopyH();
        ((CAnchorPointData*)pTail)->fixAsTailForMirrorCopyH();
    }

    return( true );
}

//----------------
// スケール適用
//----------------
void CLineObjectData::applyRateScale( int rateScale ){
    // 対象のデータを割合で取り込む
    CAnchorPointData* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // [m_eBrushId]は無視
    // [m_eTestPalOfsId]は無視
    // [m_eTestPalOfsIdForRepair]は無視
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CLineObjectData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 3 + 4 + eLOD_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "BRUSH_ID", &m_eBrushId, eEDIT_VALUE_TYPE_INT32,
                             eBRUSH_INVALID, eBRUSH_MAX-1, g_pArrLabelBrush );
    pMenu->setItemAtAsLabel( id++, "TEST_PAL_F_ID", &m_eTestPalOfsId, eEDIT_VALUE_TYPE_INT32,
                             ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );
    pMenu->setItemAtAsLabel( id++, "REPAIR_TEST_PAL_F_ID", &m_eTestPalOfsIdForRepair, eEDIT_VALUE_TYPE_INT32,
                             ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );
    
    pMenu->setSeparatorAt( id, true );
    
    // 一時調整値（※この値は保存されない＝修正後に[applyTempAdjust]で調整を適用する想定）
    pMenu->setItemAt( id++, "TEMP: X", &m_nTempAdjustRateOfsX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: Y", &m_nTempAdjustRateOfsY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_X", &m_nTempAdjustRateScaleX, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_Y", &m_nTempAdjustRateScaleY, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: CLOSE_PATH", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_CLOSE_PATH );
    pMenu->setItemAtAsBit( id++, "FLAG: DOT", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_DOT );
    pMenu->setItemAtAsBit( id++, "FLAG: TOUCH", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_TOUCH );
    pMenu->setItemAtAsBit( id++, "FLAG: TOUCH_IS_STRIPE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_TOUCH_IS_STRIPE );
    pMenu->setItemAtAsBit( id++, "FLAG: TOUCH_IS_FRILL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_TOUCH_IS_FRILL );
    pMenu->setItemAtAsBit( id++, "FLAG: GUIDE_DRAW_BEFORE_STROKE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_GUIDE_DRAW_BEFORE_STROKE );
    pMenu->setItemAtAsBit( id++, "FLAG: GUIDE_RESET_AFTER_STROKE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_GUIDE_RESET_AFTER_STROKE );
    pMenu->setItemAtAsBit( id++, "FLAG: GUIDE_RESET_FULL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLOD_FLAG_GUIDE_RESET_FULL );

    
    // 確定
    pMenu->fixMenu();
}
