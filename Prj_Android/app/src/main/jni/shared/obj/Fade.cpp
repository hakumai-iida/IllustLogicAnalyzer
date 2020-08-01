/*+----------------------------------------------------------------+
  |	Title:		Fade.cpp [共通環境]
  |	Comment:	フェード
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Fade.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------
// コンストラクタ
//-----------------------
CFade::CFade( void ): CObj(){
    setDrawLayer( eDP_LAYER_FADE );

    clearFade();
}

//-----------------------
// クリア
//-----------------------
void CFade::clearFade( void ){
    // デフォルト設定
    m_nRGBA = 0x000000FF;
    m_nCountDefault = 16;
    
    m_bFadeActive = false;
    m_bFadeIn = false;
}

//-----------------------
// 設定：フェード（実体）
//-----------------------
void CFade::setFade0( int frame, bool isFadeIn ){
	// 負の指定であればデフォルト値
	if( frame < 0 ){
		frame = m_nCountDefault;
	}

	m_bFadeIn = isFadeIn;
	setCountMax( frame );

	// フレームが有効であれば活動中
	m_bFadeActive = ( frame > 0 );

	// ブランク設定（※活動中／０フレフェードアウトなら表示）
	if( m_bFadeActive || !m_bFadeIn ){
		setBlank( false );
	}else{
		setBlank( true );
	}
}

//-----------------------
// 更新：フェード
//-----------------------
void CFade::update0( void ){
	if( countUp() ){
		m_bFadeActive = false;

		// フェードインの終了であればブランク
		if( m_bFadeIn ){
			setBlank( true );
		}
	}
}

//-----------------------
// 描画：フェード
//-----------------------
void CFade::onDraw0( void ){
	float rate = getCountRate();
	BYTE alpha;
	if( m_bFadeIn ){
		if( m_bFadeActive ){
			alpha = (BYTE)(0xFF*(1.0f-rate));
		}else{
			alpha = 0x00;
		}
	}else{
		if( m_bFadeActive ){
			alpha = (BYTE)(0xFF*rate);
		}else{
			alpha = 0xFF;
		}
	}

	// 全領域に対して描画
	CDrawCell* pDC = CDrawCell::GetFreeCell();
	pDC->clear();
	pDC->setRectFull();
	pDC->setRGBA( m_nRGBA );
	pDC->setAlpha( alpha );
	pDC->drawFB_I( 0.0f, 0.0f );
}