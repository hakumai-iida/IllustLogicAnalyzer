/*+----------------------------------------------------------------+
  |	Title:		ScrollBar.cpp [共通環境]
  |	Comment:	スクロールバー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ScrollBar.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// デフォルト値
#define SCROLL_BAR_RGBA_BASE		0x404040FF
#define SCROLL_BAR_RGBA_FRAME		0x808080FF
#define SCROLL_BAR_RGBA_BAR		    0xC0C0C0FF
#define SCROLL_BAR_ALPHA            0xFF

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
CScrollBar::CScrollBar( void ){
	reset();
}

//------------------
// リセット
//------------------
void CScrollBar::reset( void ){
	m_bVertical  = false;
	m_nWidth     = 0;
	m_nHeight    = 0;
	m_nFrameSize = 0;

	m_fScrollAreaSize = 0.0f;
	m_fScrollBarSize  = 0.0f;
	m_fScrollOfs      = 0.0f;

	m_nBaseRGBA  = SCROLL_BAR_RGBA_BASE;
	m_nFrameRGBA = SCROLL_BAR_RGBA_FRAME;
	m_nBarRGBA   = SCROLL_BAR_RGBA_BAR;
    m_nAlpha = SCROLL_BAR_ALPHA;
}

//------------------
// 表示設定
//------------------
void CScrollBar::setForDisp( bool isV, int w, int h, int b ){
	m_bVertical  = isV;
	m_nWidth     = w;
	m_nHeight    = h;
	m_nFrameSize = b;
}

//------------------
// スクロール設定
//------------------
void CScrollBar::setForScroll( float area, float bar ){
	m_fScrollAreaSize = area;
	m_fScrollBarSize  = bar;
}

//------------------
// オフセット設定
//------------------
void CScrollBar::setScrollOfs( float ofs ){
	m_fScrollOfs = ofs;

	// 用心
	if( m_fScrollOfs < 0.0f ){
		m_fScrollOfs = 0.0f;
	}else if( (m_fScrollOfs + m_fScrollBarSize) > m_fScrollAreaSize ){
		m_fScrollOfs = m_fScrollAreaSize - m_fScrollBarSize;
	}
}

//------------------
// 更新
//------------------
void CScrollBar::onUpdate0( void ){}

//--------------------
// 描画
//--------------------
void CScrollBar::onDraw0( void ){
	// 無効は無視
	if( m_nAlpha <= 0 || m_nWidth <= 0 || m_nHeight <= 0 ){ return; }

	bool bDark = isDark();
	DWORD rgba0 = GET_NODE_RGB_WITH_A( bDark, false, m_nAlpha );
	DWORD rgbaBase = CConst::CalcMultiForRGBA( m_nBaseRGBA, rgba0 );
	DWORD rgbaFrame = CConst::CalcMultiForRGBA( m_nFrameRGBA, rgba0 );
	DWORD rgbaBar = CConst::CalcMultiForRGBA( m_nBarRGBA, rgba0 );

	CDrawCell* pDC = CDrawCell::GetFreeCell();
	float x = m_fX;
	float y = m_fY;

	// フレーム表示
	if( m_nFrameSize > 0 ){
		pDC->clear();
		pDC->setRect( m_nWidth+2*m_nFrameSize, m_nHeight+2*m_nFrameSize );
		pDC->setRGBA( rgbaFrame );
		pDC->setBlendAlpha();
		pDC->draw( x-m_nFrameSize, y-m_nFrameSize );
	}

	// ベース表示
	pDC->clear();
	pDC->setRect( m_nWidth, m_nHeight );
	pDC->setRGBA( rgbaBase );
	pDC->setBlendAlpha();
	pDC->draw( x, y );

	// バーの表示
	float w = m_nWidth;
	float h = m_nHeight;
	if( m_bVertical ){
		h = m_nHeight*m_fScrollBarSize/m_fScrollAreaSize;
		y += m_nHeight*m_fScrollOfs/m_fScrollAreaSize;
	}else{
		w = m_nWidth*m_fScrollBarSize/m_fScrollAreaSize;
		x += m_nWidth*m_fScrollOfs/m_fScrollAreaSize;
	}
	pDC->clear();
	pDC->setRect( w, h );
	pDC->setRGBA( rgbaBar );
	pDC->setBlendAlpha();
	pDC->draw( x, y );
}
