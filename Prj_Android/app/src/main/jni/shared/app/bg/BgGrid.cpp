/*+----------------------------------------------------------------+
  |	Title:		BgGrid.cpp [共通環境]
  |	Comment:	グリッド背景
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BgGrid.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// グリッド値（※デフォルト）
#define DEBUG_DA_RGBA_0		            0x606080FF
#define DEBUG_SAFE_AREA_LINE_RGBA_0	    0xDD8833FF
#define DEBUG_MIN_AREA_LINE_RGBA_0      0x66DD66FF
#define DEBUG_GRID_C0_RGBA_0	        0x333333FF
#define DEBUG_GRID_C1_RGBA_0            0xCCCCCCFF
#define DEBUG_GRID_RGBA_0		        0xAAAAAAFF
#define DEBUG_GRID_W_0			        32
#define DEBUG_GRID_H_0			        32

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
//-------------------------
// コンストラクタ：遠景
//-------------------------
CBgGridB::CBgGridB( void ): CObj(){
    setDrawLayer( eDP_LAYER_BG_B );

    clear();
}

//-------------------------
// クリア：遠景
//-------------------------
void CBgGridB::clear( void ){
    setGridType( eBG_GRID_TYPE_DEFAULT );
}

//-------------------------
// タイプ設定：遠景
//-------------------------
void CBgGridB::setGridType( eBG_GRID_TYPE type ){
    m_eGridType = type;

    // デフォルト値
    m_nDEBUG_DA_RGBA = DEBUG_DA_RGBA_0;
    m_nDEBUG_SAFE_AREA_LINE_RGBA = DEBUG_SAFE_AREA_LINE_RGBA_0;
    m_nDEBUG_MIN_AREA_LINE_RGBA = DEBUG_MIN_AREA_LINE_RGBA_0;
    m_nDEBUG_GRID_C0_RGBA = DEBUG_GRID_C0_RGBA_0;
    m_nDEBUG_GRID_C1_RGBA = DEBUG_GRID_C1_RGBA_0;
    m_nDEBUG_GRID_RGBA = DEBUG_GRID_RGBA_0;
    m_nDEBUG_GRID_W = DEBUG_GRID_W_0;
    m_nDEBUG_GRID_H = DEBUG_GRID_H_0;

    // タイプ毎の差分の設定
    switch( m_eGridType ){
    // ベジェ編集：クリティカルボタン有効
    case eBD_GRID_TYPE_BEZIER_CRITICAL:
        m_nDEBUG_DA_RGBA = 0x806060FF;
        m_nDEBUG_GRID_C0_RGBA = 0xFF3333FF;
        m_nDEBUG_GRID_C1_RGBA = 0xFFDDDDFF;
        m_nDEBUG_GRID_RGBA = 0xFFAAAAFF;
        break;
            
    // ベジェ編集：XY
    case eBG_GRID_TYPE_BEZIER_XY:
        m_nDEBUG_DA_RGBA = 0x808080FF;
        m_nDEBUG_GRID_C0_RGBA = 0x222222FF;
        m_nDEBUG_GRID_C1_RGBA = 0xDDDDDDFF;
        m_nDEBUG_GRID_RGBA = 0xCCCCCCFF;
        break;

    // ベジェ編集：XY（調整値）
    case eBG_GRID_TYPE_BEZIER_XY_ADJ:
        m_nDEBUG_DA_RGBA = 0x404040FF;
        m_nDEBUG_GRID_C0_RGBA = 0x111111FF;
        m_nDEBUG_GRID_C1_RGBA = 0xEEEEEEFF;
        m_nDEBUG_GRID_RGBA = 0xDDDDDDFF;
        break;

    // ベジェ編集：進入XY
    case eBG_GRID_TYPE_BEZIER_IN_XY:
        m_nDEBUG_DA_RGBA = 0xAA6666FF;
        m_nDEBUG_GRID_C0_RGBA = 0x222222FF;
        m_nDEBUG_GRID_C1_RGBA = 0xDDDDDDFF;
        m_nDEBUG_GRID_RGBA = 0xCCCCCCFF;
        break;
            
    // ベジェ編集：進入XY（調整値）
    case eBG_GRID_TYPE_BEZIER_IN_XY_ADJ:
        m_nDEBUG_DA_RGBA = 0x802222FF;
        m_nDEBUG_GRID_C0_RGBA = 0x111111FF;
        m_nDEBUG_GRID_C1_RGBA = 0xEEEEEEFF;
        m_nDEBUG_GRID_RGBA = 0xDDDDDDFF;
        break;
            
    // ベジェ編集：出立XY
    case eBG_GRID_TYPE_BEZIER_OUT_XY:
        m_nDEBUG_DA_RGBA = 0x6666AAFF;
        m_nDEBUG_GRID_C0_RGBA = 0x222222FF;
        m_nDEBUG_GRID_C1_RGBA = 0xDDDDDDFF;
        m_nDEBUG_GRID_RGBA = 0xCCCCCCFF;
        break;
            
    // ベジェ編集：出立XY（調整値）
    case eBG_GRID_TYPE_BEZIER_OUT_XY_ADJ:
        m_nDEBUG_DA_RGBA = 0x222280FF;
        m_nDEBUG_GRID_C0_RGBA = 0x111111FF;
        m_nDEBUG_GRID_C1_RGBA = 0xEEEEEEFF;
        m_nDEBUG_GRID_RGBA = 0xDDDDDDFF;
        break;

    default:
    case eBG_GRID_TYPE_DEFAULT:
        break;
    }
}

//-------------------------
// 描画：グリッド背景：遠景
//-------------------------
void CBgGridB::onDraw0( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();
    
    // 画面中央を基準に描画
    float x = SA_XC + getX();
    float y = SA_YM + getY();

	// クリア
	pDC->clear();
	pDC->setRectFull();
	pDC->setRGBA( m_nDEBUG_DA_RGBA );
	pDC->drawFB_I( 0.0f, 0.0f );

	// 格子の表示
	int gW = m_nDEBUG_GRID_W;
	int gH = m_nDEBUG_GRID_H;
	int numBlockW = (SA_W/gW)/2 + 1;
	int numBlockH = (SA_H/gH)/2 + 1;

    // 横線（座標の中央に線がくるように）
    for( int i=0; i<numBlockH; i++ ){
        pDC->clear();
        pDC->setRect( SA_W, 1.0f );
        pDC->setRGBA( m_nDEBUG_GRID_RGBA );
        pDC->setAlignX( eDRAW_ALIGN_X_C );
        pDC->drawSA_I( x, y - i*gH - 0.5f );
        pDC->drawSA_I( x, y + i*gH - 0.5f );
    }

    // 縦線（座標の中央に線がくるように）
	for( int i=0; i<numBlockW; i++ ){
		pDC->clear();
		pDC->setRect( 1.0f, SA_H );
		pDC->setRGBA( m_nDEBUG_GRID_RGBA );
        pDC->setAlignY( eDRAW_ALIGN_Y_M );
        pDC->drawSA_I( x - i*gW - 0.5f, y );
        pDC->drawSA_I( x + i*gW - 0.5f, y );
	}
    
	// セーフエリア枠（エリアの内側に線がくるように）
	pDC->clear();
	pDC->setRect( SA_W, 1.0f );
	pDC->setRGBA( m_nDEBUG_SAFE_AREA_LINE_RGBA );
    pDC->setAlignX( eDRAW_ALIGN_X_C );
	pDC->drawSA_I( x, y - SA_H/2.0f );
    pDC->setAlignY( eDRAW_ALIGN_Y_B );
	pDC->drawSA_I( x, y + SA_H/2.0f );

	pDC->clear();
	pDC->setRect( 1.0f, SA_H );
	pDC->setRGBA( m_nDEBUG_SAFE_AREA_LINE_RGBA );
    pDC->setAlignY( eDRAW_ALIGN_Y_M );
	pDC->drawSA_I( x - SA_W/2.0f, y );
    pDC->setAlignX( eDRAW_ALIGN_X_R );
	pDC->drawSA_I( x + SA_W/2.0f, y );
    
    // 最低保証領域枠（エリアの内側に線がくるように）
    pDC->clear();
    pDC->setRect( REQUIRED_W, 1.0f );
    pDC->setRGBA( m_nDEBUG_MIN_AREA_LINE_RGBA );
    pDC->setAlignX( eDRAW_ALIGN_X_C );
    pDC->drawSA_I( x, y - REQUIRED_H/2.0f );
    pDC->setAlignY( eDRAW_ALIGN_Y_B );
    pDC->drawSA_I( x, y + REQUIRED_H/2.0f );
    
    pDC->clear();
    pDC->setRect( 1.0f, REQUIRED_H );
    pDC->setRGBA( m_nDEBUG_MIN_AREA_LINE_RGBA );
    pDC->setAlignY( eDRAW_ALIGN_Y_M );
    pDC->drawSA_I( x - REQUIRED_W/2.0f, y );
    pDC->setAlignX( eDRAW_ALIGN_X_R );
    pDC->drawSA_I( x + REQUIRED_W/2.0f, y );

	// 中心線（※ゲームエリアに対して描画）
    x = SA_OFS_X + SA_W/2.0f + getX();
    y = SA_OFS_Y + SA_H/2.0f + getY();
	pDC->clear();
	pDC->setRect( 3.0f, GA_H );
	pDC->setRGBA( m_nDEBUG_GRID_C1_RGBA );
    pDC->setAlignX( eDRAW_ALIGN_X_C );
    pDC->drawGA_I( x, 0.0f );

	pDC->clear();
	pDC->setRect( GA_W, 3.0f );
	pDC->setRGBA( m_nDEBUG_GRID_C1_RGBA );
    pDC->setAlignY( eDRAW_ALIGN_Y_M );
	pDC->drawGA_I( 0.0f, y );
    
    pDC->clear();
    pDC->setRect( 1.0f, GA_H );
    pDC->setRGBA( m_nDEBUG_GRID_C0_RGBA );
    pDC->setAlignX( eDRAW_ALIGN_X_C );
    pDC->drawGA_I( x, 0.0f );
    
    pDC->clear();
    pDC->setRect( GA_W, 1.0f );
    pDC->setRGBA( m_nDEBUG_GRID_C0_RGBA );
    pDC->setAlignY( eDRAW_ALIGN_Y_M );
    pDC->drawGA_I( 0.0f, y );
}

//-------------------------
// コンストラクタ：近景
//-------------------------
CBgGridF::CBgGridF( void ): CObj(){
    setDrawLayer( eDP_LAYER_BG_F );

    clear();
}

//-------------------------
// クリア：近景
//-------------------------
void CBgGridF::clear( void ){
    setGridType( eBG_GRID_TYPE_DEFAULT );
}

//-------------------------
// タイプ設定：近景
//-------------------------
void CBgGridF::setGridType( eBG_GRID_TYPE type ){
    // 特に無し
}

//-------------------------
// 描画：グリッド背景：近景
//-------------------------
void CBgGridF::onDraw0( void ){
    // 特に無し
}

