/*+----------------------------------------------------------------+
  | Title:      LayerObject.cpp [共通環境]
  | Comment:    レイヤーオブジェクト（※レイヤーに配置する要素の基底）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerObject.hpp"

#include "LineObjectData.hpp"
#include "PaintObjectData.hpp"
#include "SlotObjectData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//----------------------
// ラベル：レイヤー種別
//----------------------
const char* g_pArrLabelLayerObjectType[] = {
    "LINE",
    "PAINT",
    "SLOT",
    
    NULL,
};

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------------------------
// コンストラクタ
//-----------------------------------------
CLayerObject::CLayerObject( eLAYER_OBJECT_TYPE type ): CListNode(){
    m_eObjectType = type;
}

//-----------------------------------------
// 解放（※対応するアロケーターに受け渡す）
//-----------------------------------------
void CLayerObject::Free( CLayerObject* pObj ){
    if( pObj != NULL ){
        if( pObj->isLineObject() ){
            CLineObjectData::Free( (CLineObjectData*)pObj );
        }else if( pObj->isPaintObject() ){
            CPaintObjectData::Free( (CPaintObjectData*)pObj );
        }else if( pObj->isSlotObject() ){
            CSlotObjectData::Free( (CSlotObjectData*)pObj );
        }else{
            LOGE( "@ CLayerObject::Free: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
    }
}

//------------
// スケール適用
//------------
void CLayerObject::applyRateScale( int rateScale ){
    // 線であれば
    if( isLineObject() ){
        (getLineObject())->applyRateScale( rateScale );
    }
    // 塗りであれば
    else if( isPaintObject() ){
        (getPaintObject())->applyRateScale( rateScale );
    }
    // スロットであれば
    else if( isSlotObject() ){
        (getSlotObject())->applyRateScale( rateScale );
    }
}
