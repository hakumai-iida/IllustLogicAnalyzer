/*+----------------------------------------------------------------+
  |	Title:		Win.cpp [共通環境]
  |	Comment:	窓（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Win.hpp"

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
CWin::CWin( eGRP grpId, float cW, float cH ){
    m_eGrpId = grpId;
	m_ePalId = eIMG_INVALID;
	setTitle( "UNDEFINED" );

	// フリック係数
	m_fFlickAdjX = 1.0f;
	m_fFlickAdjY = 1.0f;

	setCenterWH( cW, cH );
}

//------------------
// 名前の設定
//------------------
void CWin::setTitle( const char* pTitle ){
	// バッファにコピー（※収まらない場合は切り捨てられる）
	util_str_cpy( m_cArrTitle, pTitle, COMMON_NAME_LEN );
}

//-----------------------------------------------
// 表示領域の設定（※[eSPR_WINDOW]の素材に合わせた設定）
//-----------------------------------------------
void CWin::setCenterWH( float w, float h ){
	m_fInternalW = m_fCenterW = w;
	m_fInternalH = m_fCenterH = h;

    CGrp* pGrp = CGrpMgr::GetGrp( m_eGrpId );

    // グループ指定が有効なら窓枠の調整
	if( pGrp != NULL ){
        CSpr* pSprLT = pGrp->getSprAt(0);
        CSpr* pSprRB = pGrp->getSprAt(8);
		m_fCenterX = pSprLT->getWidth();
		m_fCenterY = pSprLT->getHeight();
		m_fWinW = m_fCenterX + m_fCenterW + pSprRB->getWidth();
		m_fWinH = m_fCenterY + m_fCenterH + pSprRB->getHeight();
	}
    // 無効なら一律で余白の設定
    else{
		m_fCenterX = 8;
		m_fCenterY = 21;
		m_fWinW = m_fCenterX + m_fInternalW + 8;
		m_fWinH = m_fCenterY + m_fInternalH + 4;
	}

	// タップ領域
	setTouchRect( m_fCenterX, m_fCenterY, m_fCenterW, m_fCenterH );
}

//------------------
// 内部領域の設定
//------------------
void CWin::setInternalWH( float w, float h ){
	// 表示領域よりも大きければ設定
	if( w > m_fCenterW ){
		m_fInternalW = w;
	}

	if( h > m_fCenterH ){
		m_fInternalH = h;
	}

	// 用心にクリアしておく
	m_fInternalOfsX = 0.0f;
	m_fInternalOfsY = 0.0f;
}

//------------------
// オフセット設定
//------------------
void CWin::setInternalOfsX( float x ){
	m_fInternalOfsX = x;
	limitInternalOfsX();
}

void CWin::setInternalOfsY( float y ){
	m_fInternalOfsY = y;
	limitInternalOfsY();
}

//------------------
// 更新
//------------------
void CWin::onUpdate0( void ){
	// フリックはスクロール値として適用
	if( isFlicked() ){
		CTouchStatusCell* pCell = getCellFlicked();
		setFlick( pCell->getFlickX(), pCell->getFlickY() );
	}
	// ドラッグは即座に適用
	else if( isMoved() ){
		CTouchStatusCell* pCell = getCellMoved();
		m_fInternalOfsX -= pCell->getMoveX();
		m_fInternalOfsY -= pCell->getMoveY();
	}

	// フリックスクロール中であれば
	if( m_nFlickCount > 0 ){
		// 再タッチされたらクリア
		if( isPressed() ){
			m_nFlickCount = 0;
		}
		// 速度／加速度の適用
		else{
			m_nFlickCount--;
			if( m_nFlickCount > 0 ){
				m_fInternalOfsX -= m_fFlickVX;
				m_fInternalOfsY -= m_fFlickVY;
				m_fFlickVX += m_fFlickAX;
				m_fFlickVY += m_fFlickAY;
			}else{
				m_fInternalOfsX -= m_fFlickVX;
				m_fInternalOfsY -= m_fFlickVY;
				m_fFlickVX += m_fFlickAX;
				m_fFlickVY += m_fFlickAY;
			}
		}
	}

	// リミッタ
	limitInternalOfsX();
	limitInternalOfsY();
}

//-----------------
// リミッタ：X
//-----------------
void CWin::limitInternalOfsX( void ){
	if( m_fInternalOfsX < 0.0f ){
		m_fInternalOfsX = 0.0f;
	}else if( m_fInternalOfsX > (m_fInternalW-m_fCenterW) ){
		m_fInternalOfsX = m_fInternalW - m_fCenterW;
	}
}

//-----------------
// リミッタ：Y
//-----------------
void CWin::limitInternalOfsY( void ){
	if( m_fInternalOfsY < 0.0f ){
		m_fInternalOfsY = 0.0f;
	}else if( m_fInternalOfsY > (m_fInternalH-m_fCenterH) ){
		m_fInternalOfsY = m_fInternalH - m_fCenterH;
	}
}

//-----------------
// 移動速度
//-----------------
void CWin::setFlick( float flickX, float flickY ){
	// 調整
	flickX /= 1.2f;
	flickY /= 1.2f;

	float pow = (float)sqrt( flickX*flickX + flickY*flickY );
	m_nFlickCount = (int)(pow/6.0f);

	// スクロール時間
	if( m_nFlickCount < 8 ){
		m_nFlickCount = 8;
	}else if( m_nFlickCount > 32 ){
		m_nFlickCount = 32;
	}

	// 速度／加速度の設定
	m_fFlickVX = m_fFlickAdjX * flickX / m_nFlickCount;
	m_fFlickVY = m_fFlickAdjY * flickY / m_nFlickCount;
	m_fFlickAX = -m_fFlickVX / m_nFlickCount;
	m_fFlickAY = -m_fFlickVY / m_nFlickCount;
}

//------------------
// 描画
//------------------
void CWin::onDraw0( void ){
	// 窓の描画
	CDrawCommon::RenderWindow( m_eGrpId, m_fX, m_fY, m_fCenterW, m_fCenterH, 1.0f, 1.0f, (isDark())? 0x808080FF: 0xFFFFFFFF, m_ePalId );

	// タイトル表示
	CDrawCell* pDC = CDrawCell::GetFreeCell();
    pDC->clear();
	pDC->setStr( eFONT_DEBUG, m_cArrTitle );
    pDC->setScaleXY( 1.0f, 2.0f );
    if( isDark() ){ pDC->setRGBA( 0xA0A0A0FF ); }
	pDC->draw( m_fX+8, m_fY-2 );
}
