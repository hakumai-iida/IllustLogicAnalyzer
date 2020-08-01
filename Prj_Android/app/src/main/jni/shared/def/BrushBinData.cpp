/*+----------------------------------------------------------------+
  |	Title:		BrushBinData.cpp [共通環境]
  |	Comment:	定義定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BrushBinData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロト宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------------------------
// データ編集（※ブラシチェックループでのエディット用）
//------------------------------------------
void CBrushBinData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 11 );

    int id = 0;

    // 値
    pMenu->setItemAtAsLabel( id++, "TYPE", &m_eStrokeType, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TYPE_INVALID, eSTROKE_TYPE_MAX-1, g_pArrLabelStrokeType );

    pMenu->setItemAt( id++, "SIZE1000", &m_nStrokeSize1000, eEDIT_VALUE_TYPE_INT16, 0, STROKE_DOT_BASE_SIZE_MAX*1000 );
    pMenu->setItemAt( id++, "RANGE1000", &m_nStrokeRange1000, eEDIT_VALUE_TYPE_INT16, 0, 30*1000 );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "DOT PUT: MAX", &m_nDotPutMax, eEDIT_VALUE_TYPE_UINT8, 0, 255 );
    pMenu->setItemAt( id++, "DOT PUT: BASE", &m_nDotPutRateBase, eEDIT_VALUE_TYPE_UINT8, 0, 100 );
    pMenu->setItemAt( id++, "DOT PUT: STRONG", &m_nDotPutRateStrong, eEDIT_VALUE_TYPE_UINT8, 0, 100 );
    pMenu->setItemAt( id++, "DOT PUT: WEAK", &m_nDotPutRateWeak, eEDIT_VALUE_TYPE_UINT8, 0, 100 );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "SHAKE: POS1000", &m_nShakePosScale1000, eEDIT_VALUE_TYPE_INT16, 0, 30*1000 );
    pMenu->setItemAt( id++, "SHAKE: SIZE1000", &m_nShakeSizeScale1000, eEDIT_VALUE_TYPE_INT16, 0, 30*1000 );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAtAsBool( id++, "FLAG: PUT_FORCE", &m_bDotPutForce );
    pMenu->setItemAtAsBool( id++, "FLAG: PUT_WHETHER_MAX", &m_bDotPutWhetherMax );

    // 確定
    pMenu->fixMenu();
}
