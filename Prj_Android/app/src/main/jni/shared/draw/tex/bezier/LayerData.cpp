/*+----------------------------------------------------------------+
  |	Title:		LayerData.cpp [共通環境]
  |	Comment:	レイヤーデータ（※描画における処理単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerData.hpp"
#include "BezierDataConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_LAYER_DATA   (4)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLayerData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LAYER_DATA )

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
int CLayerData::GetAllocCellNum( void ){
    int num = 0;

    //---------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLayerData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LAYER_DATA )
    //---------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CLayerData::GetAllocSize( void ){
    int allocSize = 0;
    
    //---------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLayerData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LAYER_DATA )
    //---------------------------------------------------------------------------------------------
    allocSize += sizeof(CLayerData) * MAX_ALLOC_LAYER_DATA;
    
    return( allocSize );
}

//-------------------------
// コンストラクタ
//-------------------------
CLayerData::CLayerData( void ): CDataList(true), CListNode(){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CLayerData::~CLayerData( void ){
    clear();    // リストの解放
}

//--------------------
// クリア（※データの破棄）
//--------------------
void CLayerData::clear( void ){
    m_nFlag = 0;
    
    // リスト解放
    CLayerObject* pObj =  getDataHead();
    while( pObj != NULL ){
        CLayerObject* pNext = pObj->getNext();
        
        if( pObj->isLineObject() ){
            CLineObjectData::Free( (CLineObjectData*) pObj );
        }else if( pObj->isPaintObject() ){
            CPaintObjectData::Free( (CPaintObjectData*) pObj );
        }else if( pObj->isSlotObject() ){
            CSlotObjectData::Free( (CSlotObjectData*) pObj );
        }else{
            LOGE( "@ CLayerData::clear: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
        
        pObj = pNext;
    }
    m_oDataList.disconnectAll();
    
    // 各種値
    m_cArrName[0] = '\0';
    
    m_eDelayType = eDELAY_LOG_INVALID;
    m_nDelayDepth = 0;
    m_nDelayPowerRateX = m_nDelayPowerRateY = 0;
}

//--------------------
// コピー
//--------------------
void CLayerData::copy( CLayerData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    // クリア（※リストの解放）
    clear();

    m_nFlag = pData->m_nFlag;

    // リストのコピー
    CLayerObject* pObj = pData->getDataHead();
    while( pObj != NULL ){
        if( pObj->isLineObject() ){
            CLineObjectData* pLOD = CLineObjectData::Alloc();
            pLOD->copy( (CLineObjectData*)pObj);
            addData( pLOD );
        }else if( pObj->isPaintObject() ){
            CPaintObjectData* pPOD = CPaintObjectData::Alloc();
            pPOD->copy( (CPaintObjectData*)pObj);
            addData( pPOD );
        }else if( pObj->isSlotObject() ){
            CSlotObjectData* pSOD = CSlotObjectData::Alloc();
            pSOD->copy( (CSlotObjectData*)pObj);
            addData( pSOD );
        }else{
            LOGE( "@ CLayerData::copy: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
        
        pObj = pObj->getNext();
    }

    // 各種値
    setName( pData->m_cArrName );

    m_eDelayType = pData->m_eDelayType;
    m_nDelayDepth = pData->m_eDelayType;
    m_nDelayPowerRateX = pData->m_nDelayPowerRateX;
    m_nDelayPowerRateY = pData->m_nDelayPowerRateY;
}

//---------------------------
// 読み込み
//---------------------------
void CLayerData::read( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();

    m_nFlag = pIB->readInt16();

    // 要素読み込み
    int n = pIB->readInt16();
    for( int i=0; i<n; i++ ){
        // タイプ読み込み
        eLAYER_OBJECT_TYPE type = (eLAYER_OBJECT_TYPE) CEnum::ReadEnum( pIB, g_pArrLabelLayerObjectType );
        
        // 割り振り
        if( type == eLAYER_OBJECT_TYPE_LINE ){
            CLineObjectData* pLOD = CLineObjectData::Alloc();
            pLOD->read( pIB );
            addData( pLOD );
        }else if( type == eLAYER_OBJECT_TYPE_PAINT ){
            CPaintObjectData* pPOD = CPaintObjectData::Alloc();
            pPOD->read( pIB );
            addData( pPOD );
        }else if( type == eLAYER_OBJECT_TYPE_SLOT ){
            CSlotObjectData* pSOD = CSlotObjectData::Alloc();
            pSOD->read( pIB );
            addData( pSOD );
        }else{
            LOGE( "@ CLayerData::read: UNKNOWN OBJECT: type=%d\n", type );
        }
    }

    // 各種値
    pIB->readBlock( m_cArrName, LAYER_NAME_LEN );
    
    m_eDelayType = (eDELAY_LOG)CEnum::ReadEnum( pIB, g_pArrLabelDelayLog );
    m_nDelayDepth = pIB->readInt16();
    m_nDelayPowerRateX = pIB->readInt16();
    m_nDelayPowerRateY = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CLayerData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );

    // 要素出力
    int num = getDataSize();
    pOB->writeInt16( (int16)num );
    
    CLayerObject* pObj = getDataHead();
    while( pObj != NULL ){
        // タイプ別に出力
        if( pObj->isLineObject() ){
            CEnum::WriteEnum( pOB, eLAYER_OBJECT_TYPE_LINE, eLAYER_OBJECT_TYPE_MAX, g_pArrLabelLayerObjectType );
            ((CLineObjectData*)pObj)->write( pOB );
        }else if( pObj->isPaintObject() ){
            CEnum::WriteEnum( pOB, eLAYER_OBJECT_TYPE_PAINT, eLAYER_OBJECT_TYPE_MAX, g_pArrLabelLayerObjectType );
            ((CPaintObjectData*)pObj)->write( pOB );
        }else if( pObj->isSlotObject() ){
            CEnum::WriteEnum( pOB, eLAYER_OBJECT_TYPE_SLOT, eLAYER_OBJECT_TYPE_MAX, g_pArrLabelLayerObjectType );
            ((CSlotObjectData*)pObj)->write( pOB );
        }else{
            LOGE( "@ CLayerData::write: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
        
        pObj = pObj->getNext();
    }

    // 各種値
    pOB->writeBlock( m_cArrName, LAYER_NAME_LEN );
    
    CEnum::WriteEnum( pOB, m_eDelayType, eDELAY_LOG_MAX, g_pArrLabelDelayLog );
    pOB->writeInt16( (int16)m_nDelayDepth );
    pOB->writeInt16( (int16)m_nDelayPowerRateX );
    pOB->writeInt16( (int16)m_nDelayPowerRateY );
}

//--------------------------------
// 追加（※基本）
//--------------------------------
CLayerObject* CLayerData::addData( void ){
    // 追加対象の作成（※選択中の要素が有効であれば追加要素にコピーするのが[CDataList]での仕様）
    CLayerObject* pNew = copyData();

    //  コピーに失敗していたら無視
    if( pNew == NULL ){
        return( NULL );
    }
    
    return( addData( pNew ) );
}

//--------------------------------
// 追加: LineObjectData
//--------------------------------
CLayerObject* CLayerData::addLineObjectData( void ){
    CLayerObject* pNew = (CLayerObject*) CLineObjectData::Alloc();
    return( addData( pNew ) );
}

//--------------------------------
// 追加: PaintObjectData
//--------------------------------
CLayerObject* CLayerData::addPaintObjectData( void ){
    CLayerObject* pNew = (CLayerObject*) CPaintObjectData::Alloc();
    return( addData( pNew ) );
}

//--------------------------------
// 追加: SlotObjectData
//--------------------------------
CLayerObject* CLayerData::addSlotObjectData( void ){
    CLayerObject* pNew = (CLayerObject*) CSlotObjectData::Alloc();
    return( addData( pNew ) );
}

//--------------------------------
// 削除
//--------------------------------
void CLayerData::deleteData( void ){
    CLayerObject* pObj = (CLayerObject*) deleteData0();
    CLayerObject::Free( pObj );
}

//--------------------------------
// コピー
//--------------------------------
CLayerObject* CLayerData::copyData( void ){
    CLayerObject* pObj = getSelectedData();
    
    if( pObj != NULL ){
        if(  pObj->isLineObject() ){
            CLineObjectData* pLineObj = CLineObjectData::Alloc();
            pLineObj->copy( (CLineObjectData*)pObj );
            return( (CLayerObject*) pLineObj );
        }
        
        if( pObj->isPaintObject() ){
            CPaintObjectData* pPaintObj = CPaintObjectData::Alloc();
            pPaintObj->copy( (CPaintObjectData*)pObj );
            return( (CLayerObject*) pPaintObj );
        }
        
        if( pObj->isSlotObject() ){
            CSlotObjectData* pSlotObj = CSlotObjectData::Alloc();
            pSlotObj->copy( (CSlotObjectData*)pObj );
            return( (CLayerObject*) pSlotObj );
        }
    }
    
    return( NULL );
}

//--------------------------------
// ペースト
//--------------------------------
CLayerObject* CLayerData::pasteData( CLayerObject* pObjCopy ){
    CLayerObject* pObj = getSelectedData();
    CLayerObject* pNew = NULL;
    if( pObj != NULL && pObjCopy != NULL ){
        if( pObjCopy->isLineObject() ){
            CLineObjectData* pLineObj = CLineObjectData::Alloc();
            pLineObj->copy( (CLineObjectData*)pObjCopy );
            pNew = (CLayerObject*) pLineObj;
        }else if( pObjCopy->isPaintObject() ){
            CPaintObjectData* pPaintObj = CPaintObjectData::Alloc();
            pPaintObj->copy( (CPaintObjectData*)pObjCopy );
            pNew = (CLayerObject*) pPaintObj;
        }else if( pObjCopy->isSlotObject() ){
            CSlotObjectData* pSlotObj = CSlotObjectData::Alloc();
            pSlotObj->copy( (CSlotObjectData*)pObjCopy );
            pNew = (CLayerObject*) pSlotObj;
        }

        // 追加した上で対象を削除（※タイプが違うデータには上書きできないので）
        if( pNew != NULL ){
            int select = getSelect();
            addData( pNew );
            setSelect( select );    // 選択を戻す（※追加により１つ下へ移動するので）
            deleteData();
            setSelect( select );    // 選択を戻す（※削除により１つ下へ移動するので）
        }
    }
    
    return( pNew );
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
// [CBezier:Stroke]に対する処理では[pFillGuide]の指定を固定化し切り替え等できなくしている
// 同様に、[CBezier::Stroke/Dot]へのテストバッファとして[pBufColor]を固定化している
//（※[pBufColor]で画素が出力されていたら以降のストロークの書き込みは不要とみなすルール）
//（※フラグ等で色々と切り替えたいところではあるがシンプルさを優先してみる）
//-----------------------------------------------------------------------------
int CLayerData::draw( stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath ){
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL---[START:%s] CLayerData::draw\n", m_cArrName );
    }
#endif
    
    int timeForLine = 0;
    int timeForPaint = 0;

    // フックのリセット（※デフォルトではレイヤー単位でテンポラリをリセット）
    if( ! checkFlag( eLAYER_FLAG_STAY_HOOK ) ){
        CBezier::ResetHookPoint( checkFlag( eLAYER_FLAG_RESET_HOOK_ALL ) );
    }

    // タッチのリセット（※デフォルトではレイヤー単位でテンポラリをリセット）
    if( ! checkFlag( eLAYER_FLAG_STAY_TOUCH ) ){
        CBezier::ResetTouchPoint( checkFlag( eLAYER_FLAG_RESET_TOUCH_ALL ) );
    }

    //------------
    // 描画
    //------------
    CLayerObject* pObj = getDataHead();
    while( pObj != NULL ){
        //-----------------------------
        // ライン：CLineObjectData
        //-----------------------------
        if( pObj->isLineObject() ){
            CLineObjectData* pLOD = pObj->getLineObject();
            // オブジェクトが有効であれば描画
            if( ! pLOD->checkFlag( eLOD_FLAG_DISABLE ) ){
                // アンカーポイントを描画形式データに変換
                stBEZIER_ANCHOR_POINT* pHead = CBezierDataConv::CreateBezierAnchorPoint( pLOD, pBaseParam, pLayerParam );

                // 指定があればタッチ情報でガイドを構築
                if( pLOD->checkFlag( eLOD_FLAG_GUIDE_DRAW_BEFORE_STROKE ) ){
                    CBezier::DrawFillGuideForTouch( pBaseParam );
                }
                
                // タッチの描画
                if( pLOD->checkFlag( eLOD_FLAG_TOUCH ) ){
                    // フリルであれば分岐
                    if( pLOD->checkFlag( eLOD_FLAG_TOUCH_IS_FRILL ) ){
                        timeForLine += CBezier::TouchForFrill( pHead, pBaseParam, pLayerParam, pLOD->getTestPalGrp(), isWorkPath );
                    }else{
                        bool isStripe = pLOD->checkFlag( eLOD_FLAG_TOUCH_IS_STRIPE );
                        timeForLine += CBezier::Touch( pHead, pBaseParam, pLayerParam, pLOD->getTestPalGrp(), isStripe, isWorkPath );
                    }
                }
                // ストロークの描画
                else{
                    // ドットフラグ（※指定があれば線ではなく点を描く）
                    bool isDot = pLOD->checkFlag( eLOD_FLAG_DOT );

                    timeForLine += CBezier::Stroke( pHead, pBaseParam, pLayerParam,
                                                    pLOD->getTestPalGrp(), pLOD->getTestPalGrpForRepair(), isDot, isWorkPath );
                }
                
                // 描画形式データを解放
                CBezierDataConv::ReleaseBezierAnchorPoint( pHead );
                
                // 指定があればガイドのクリア
                if( pLOD->checkFlag( eLOD_FLAG_GUIDE_RESET_AFTER_STROKE ) ){
                    bool isFull = pLOD->checkFlag( eLOD_FLAG_GUIDE_RESET_FULL );
                    CBmpGenerator::ResetGuide( isFull );
                }
            }
        }
        //-----------------------------
        // ペイント：CPaintObjectData
        //-----------------------------
        else if( pObj->isPaintObject() ){
            CPaintObjectData* pPOD = pObj->getPaintObject();
            
            // オブジェクトが有効であれば描画
            if( ! pPOD->checkFlag( ePOD_FLAG_DISABLE ) ){
                stBEZIER_FILL_POINT* pHead = CBezierDataConv::CreateBezierFillPoint( pPOD, pBaseParam, pLayerParam );
                timeForPaint += CBezier::Fill( pHead, pBaseParam, pLayerParam, pPOD->getTestPalGrp(), isWorkPath );
                CBezierDataConv::ReleaseBezierFillPoint( pHead );

                // ガイドは塗るたびにクリア（※指定があれば持ち越す）
                if( ! pPOD->checkFlag( ePOD_FLAG_STAY_GUIDE_AFTER_FILL ) ){
                    bool isFull = pPOD->checkFlag( ePOD_FLAG_GUIDE_RESET_FULL );
                    CBmpGenerator::ResetGuide( isFull );
                }
            }
        }
        //---------------------------------------------------------------
        // スロット：CSlotObjectData（※開発用＝作業パス指定がある場合のみ処理する）
        //---------------------------------------------------------------
        else if( pObj->isSlotObject() ){
            // ワークパス指定があれば開発用に表示
            if( isWorkPath ){
                CSlotObjectData* pSOD = pObj->getSlotObject();
                if( ! pSOD->checkFlag( eSOD_FLAG_DISABLE ) ){
                    stBEZIER_SLOT_POINT* pHead = CBezierDataConv::CreateBezierSlotPoint( pSOD, pBaseParam, pLayerParam );
                    CBezier::SlotForDev( pHead, pBaseParam );
                    CBezierDataConv::ReleaseBezierSlotPoint( pHead );
                }
            }
        }
        // ここまできたら困る
        else{
            LOGE( "@ CLayerData::draw: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }

        pObj = pObj->getNext();
    }
    
    int total = timeForLine + timeForPaint;

#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL---[FINISH:%s] CLayerData::draw: %d(%d+%d)us\n", m_cArrName, total, timeForLine, timeForPaint );
    }
#endif
    
    return( total );
}

//-----------------------------
// スケール適用
//-----------------------------
void CLayerData::applyRateScale( int rateScale ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // [m_cArrName]は無視

    // [m_eDelayType]は無視
    // [m_eDelayDepth]は無視
    m_nDelayPowerRateX = (m_nDelayPowerRateX * rateScale)/BEZIER_SCALE_RATE;
    m_nDelayPowerRateY = (m_nDelayPowerRateY * rateScale)/BEZIER_SCALE_RATE;
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CLayerData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 4 + eLAYER_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "DLY: TYPE", &m_eDelayType, eEDIT_VALUE_TYPE_INT32,
                             eDELAY_LOG_INVALID, eDELAY_LOG_MAX-1, g_pArrLabelDelayLog );
    pMenu->setItemAt( id++, "DLY: DEPTH", &m_nDelayDepth, eEDIT_VALUE_TYPE_INT32, 0, DELAY_LOG_DEPTH );
    pMenu->setItemAt( id++, "DLY: POW_RATE_X", &m_nDelayPowerRateX, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "DLY: POW_RATE_Y", &m_nDelayPowerRateY, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_HOOK", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_HOOK );
    pMenu->setItemAtAsBit( id++, "FLAG: RESET_HOOK_ALL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_RESET_HOOK_ALL );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_TOUCH", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_TOUCH );
    pMenu->setItemAtAsBit( id++, "FLAG: RESET_TOUCH_ALL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_RESET_TOUCH_ALL );
    pMenu->setItemAtAsBit( id++, "FLAG: OPEN_COVER", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_OPEN_COVER );
    pMenu->setItemAtAsBit( id++, "FLAG: CLOSE_COVER", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_CLOSE_COVER );

    
    // 確定
    pMenu->fixMenu();
}
