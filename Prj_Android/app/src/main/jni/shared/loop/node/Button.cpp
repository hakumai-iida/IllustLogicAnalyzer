/*+----------------------------------------------------------------+
  |	Title:		Button.cpp [共通環境]
  |	Comment:	ボタン
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Button.hpp"

// 文字色設定を参照
#include "StrMenu.hpp"

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
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------
// コンストラクタ
//------------------
CButton::CButton( void ){
	clear();
}

//------------------
// コンストラクタ
//------------------
CButton::CButton( eGRP grpId ){
	clear();
	setGrp( grpId );
}

//------------------
// コンストラクタ
//------------------
CButton::CButton( eGRP grpId, const char* pName ){
	clear();
	setGrp( grpId );
	setName( pName );
}

//------------------
// コンストラクタ
//------------------
CButton::CButton( eGRP grpId, eSPR sprId ){
	clear();
	setGrp( grpId );
	setSpr( sprId );
}

//------------------
// コンストラクタ
//------------------
CButton::CButton( eGRP grpId, eSPR sprId, eIMG sprPalId ){
	clear();
	setGrp( grpId );
	setSpr( sprId );
	setSprPal( sprPalId );
}

//------------------
// クリア
//------------------
void CButton::clear( void ){
	// 設定無効化
	m_eGrpId = eGRP_INVALID;
	m_eGrpPalId = eIMG_INVALID;
	m_eSprId = eSPR_INVALID;
	m_eSprPalId = eIMG_INVALID;
	m_cArrName[0] = '\0';

	// フラグ
	m_bFlipX = m_bFlipY = false;

	// 初期値
	m_nRGBABase		        = RGBA_NODE_ACTIVE;
	m_nRGBABaseDark	        = RGBA_NODE_DARK;
	m_nRGBASpr		        = RGBA_NODE_ACTIVE;
	m_nRGBASprDark	        = RGBA_NODE_DARK;
	m_nRGBAStr		        = RGBA_NODE_ACTIVE;
	m_nRGBAStrDark	        = RGBA_NODE_DARK;
    m_nRGBAStrFocused       = SM_ITEM_SELECTED_RGBA;
    m_nRGBAStrDarkFocused   = SM_ITEM_SELECTED_RGBA_DARK;
    
    m_bCheckArea = false;
    m_bFocused = false;
}

//------------------
// グループ設定
//------------------
void CButton::setGrp( eGRP grpId ){
	m_eGrpId = grpId;

	// デフォルトのタップエリアは指定画像の幅＆高さ
    CGrp* pGrp = CGrpMgr::GetGrp( m_eGrpId );
    CSpr* pSpr = pGrp->getSprAt( 0 );
	float w = pSpr->getWidth();
	float h = pSpr->getHeight();
	setTouchRect( 0.0f, 0.0f, w, h );
}

//----------------------
// 名前設定
//----------------------
void CButton::setName( const char* pName ){
	// 文字列のコピー（※バッファに収まらない部分は切り捨て）
	util_str_cpy( m_cArrName, pName, COMMON_NAME_LEN );
}

//------------------
// 更新
//------------------
void CButton::onUpdate0( void ){}

//------------------
// 描画
//------------------
void CButton::onDraw0( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();
	bool bDark = isDark();

	int target = 0;
	if( isTapped() ){ target = 2; }
	else if( isPressed() ){ target = 1; }

	// ボタン表示
	CGrp *pGrp = CGrpMgr::GetGrp( m_eGrpId );
    CSpr* pButtonSpr = pGrp->getSprAt( target );
    
	pDC->clear();
	pDC->setSpr( pButtonSpr );
	pDC->setBlendAlpha();
	pDC->setRGBA( (bDark)? m_nRGBABaseDark: m_nRGBABase );
	if( m_eGrpPalId != eIMG_INVALID ){
		pDC->changePal( CImgMgr::GetImgPal( m_eGrpPalId ) );
	}
	pDC->setFlipX( m_bFlipX );
	pDC->setFlipY( m_bFlipY );
	pDC->draw( m_fX, m_fY );

	// ボタンにかぶせるスプライト
	CSpr* pCoverSpr = CSprMgr::GetSpr( m_eSprId );
	if( pCoverSpr != NULL ){
		pDC->clear();
		pDC->setSpr( pCoverSpr );
		if( m_eSprPalId != eIMG_INVALID ){
			pDC->changePal( CImgMgr::GetImgPal( m_eSprPalId ) );
		}

		pDC->setBlendAlpha();
		pDC->setRGBA( (bDark)? m_nRGBASprDark: m_nRGBASpr );
		pDC->setFlipX( m_bFlipX );
		pDC->setFlipY( m_bFlipY );
		pDC->draw( m_fX, m_fY + target );
	}

	// ボタンにかぶせる文字列
	if( m_cArrName[0] != '\0' ){
		float cX = pDC->getWidth()/2.0f;
		float cY = pDC->getHeight()/2.0f;

		pDC->clear();
		pDC->setStr( eFONT_DEBUG, m_cArrName );
        pDC->setScaleXY( 1.0f, 2.0f );
		pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
        if( m_bFocused ){
            pDC->setRGBA( (bDark)? m_nRGBAStrDarkFocused: m_nRGBAStrFocused );
        }else{
            pDC->setRGBA( (bDark)? m_nRGBAStrDark: m_nRGBAStr );
        }
		pDC->draw( m_fX+cX, m_fY+cY + target );
	}

	// タッチ領域確認
	if( m_bCheckArea ){
		pDC->clear();

		if( m_nTouchArea == eTOUCH_AREA_OVAL ){
			pDC->setImg( eIMG_DEBUG_CIRCLE );
			pDC->setWidth( m_fTouchW );
            pDC->setHeight( m_fTouchH );
		}else{
			pDC->setRect( m_fTouchW, m_fTouchH );
		}

		pDC->setRGBA( 0x8080FF80 );
		pDC->setBlendAlpha();
		pDC->draw( m_fX+m_fTouchX, m_fY+m_fTouchY );
	}
}
