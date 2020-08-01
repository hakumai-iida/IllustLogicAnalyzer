/*+----------------------------------------------------------------+
  |	Title:		DrawCell.cpp [共通環境]
  |	Comment:	描画要素
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DrawCell.hpp"

#include "sys/util/util_utf8.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// テクスチャキャッシュを利用する
//#define USE_TEX_CACHE

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
CDrawCell CDrawCell::s_oFreeCell;

eDRAW_ALIGN_X CDrawCell::s_eLogAlignX = eDRAW_ALIGN_X_L;
eDRAW_ALIGN_Y CDrawCell::s_eLogAlignY = eDRAW_ALIGN_Y_T;
float CDrawCell::s_fLogX = 0.0f;
float CDrawCell::s_fLogY = 0.0f;
float CDrawCell::s_fLogScale = 1.0f;

float CDrawCell::s_fDepthOfs = 0.0f;

float CDrawCell::s_fOfsX = 0.0f;
float CDrawCell::s_fOfsY = 0.0f;
BYTE  CDrawCell::s_nAlpha = 0xFF;

bool  CDrawCell::s_bIgnoreApplyRate = false;
float CDrawCell::s_fNegaRate = 0.0f;
float CDrawCell::s_fMonoRate = 0.0f;
float CDrawCell::s_fSepiaRate = 0.0f;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------
// ログ表示
//----------------
void CDrawCell::DrawLog( const char* pStr, float x, float y, uint32 rgba, float scale ){
    CDrawCell* pDC = &s_oFreeCell;
    
    pDC->clear();
    pDC->setStr( eFONT_DEBUG, pStr );
    pDC->setRGBA( rgba );
    pDC->setScaleXY( scale, scale );
    pDC->draw( x, y );
}

//--------------------
// レイアウト付きログ表示
//--------------------
void CDrawCell::DrawLogAlign( const char* pStr, uint32 rgba, float scale, float ofsX, float ofsY ){
    CDrawCell* pDC = &s_oFreeCell;
    
    pDC->clear();
    pDC->setStr( eFONT_DEBUG, pStr );
    pDC->setRGBA( rgba );
    if( scale > 0.0f ){
        pDC->setScaleXY( scale, scale );
    }else{
        pDC->setScaleXY( s_fLogScale, s_fLogScale );
    }
    
    pDC->setAlignXY( s_eLogAlignX, s_eLogAlignY );
    pDC->draw( s_fLogX+ofsX, s_fLogY+ofsY );

    // 次回描画位置の調整
    switch( s_eLogAlignY ){
        case eDRAW_ALIGN_Y_B:
            s_fLogY -= pDC->getHeight();
            break;
            
        default:
        case eDRAW_ALIGN_Y_T:
        case eDRAW_ALIGN_Y_M:    // どっちに足したらよいかわからないので下へ
            s_fLogY += pDC->getHeight();
            break;
    }
    
    s_fLogY += ofsY;
}

//-----------------------------
// ログ表示のレイアウト設定
//-----------------------------
 void CDrawCell::SetLogAlign( eDRAW_ALIGN_X alignX, eDRAW_ALIGN_Y alignY, float baseScale, float ofsX, float ofsY ){
     //------------------------
     // X
     //------------------------
	 switch( alignX ){
	 case eDRAW_ALIGN_X_C:
        s_eLogAlignX = eDRAW_ALIGN_X_C;
        s_fLogX = LAYOUT_X_C;
        break;
             
	 case eDRAW_ALIGN_X_R:
        s_eLogAlignX = eDRAW_ALIGN_X_R;
        s_fLogX = LAYOUT_X_R;
        break;

	 default:
        s_eLogAlignX = eDRAW_ALIGN_X_L;
        s_fLogX = LAYOUT_X_L;
        break;
	 }

     //------------------------
     // Y
     //------------------------
	 switch( alignY ){
	 case eDRAW_ALIGN_Y_M:
        s_eLogAlignY = eDRAW_ALIGN_Y_M;
        s_fLogY = LAYOUT_Y_M;
        break;
             
	 case eDRAW_ALIGN_Y_B:
        s_eLogAlignY = eDRAW_ALIGN_Y_B;
        s_fLogY = LAYOUT_Y_B;
        break;

	 default:
        s_eLogAlignY = eDRAW_ALIGN_Y_T;
        s_fLogY = LAYOUT_Y_T;
        break;
	 }

     //------------------------
     // オフセット
     //------------------------
	 s_fLogX += ofsX;
	 s_fLogY += ofsY;
     
     s_fLogScale = baseScale;
 }

//------------------------
// コンストラクタ
//------------------------
CDrawCell::CDrawCell( void ){
	clear();
}

//---------------------
// クリア
//---------------------
void CDrawCell::clear( void ){
	m_eType   = eDRAW_TYPE_INVALID;
	m_eShader = eDRAW_SHADER_NORMAL;
	m_eBlend  = eDRAW_BLEND_OFF;
	m_eAlignX = eDRAW_ALIGN_X_L;
	m_eAlignY = eDRAW_ALIGN_Y_T;

	m_fScaleX = m_fScaleY = 1.0f;
	m_nRGBA = 0xFFFFFFFF;
	m_nAlpha = 0xFF;
	m_bFlipX = m_bFlipY = false;
	m_fRotation = 0.0f;
	m_fRotX = m_fRotY = 0.0f;
	m_bDepthTest = false;
	m_fDepth = 0.0f;
	m_bUseTexCache = false;
	m_bTexCacheTemporay = false;
	m_fMonoRate = 0.0f;
	m_fNegaRate = 0.0f;
	m_fSepiaRate = 0.0f;

    // for eDRAW_TYPE_RECT
    m_fRectW = m_fRectH = 1.0f;

    // for eDRAW_TYPE_TEX
    m_pTex = m_pTexP = NULL;
    m_fDrawTexCutL = m_fDrawTexCutR = m_fDrawTexCutT = m_fDrawTexCutB = 0;

    // for eDRAW_TYPE_SPR
    m_pSpr = NULL;
    m_bSprWithUVScale = false;

    // for eDRAW_TYPE_STR
    m_eFontId = eFONT_INVALID;
    m_pStr = NULL;
	m_fStrMarginX = 0.0f;
	m_nDispLen = -1;
    
    // for eDRAW_TYPE_NUM
    m_eNumId  = eNUM_INVALID;
	m_nNum = 0;
	m_nForceFigureNum = 0;
	m_fFigureMarginX = 0;
	m_bNumWithPlus = false;
	m_nArrFigureRGBA = NULL;
    
    // for eDRAW_TYPE_POLY
	m_nNumVertex = 0;
	m_pReffArrX = m_pReffArrY = m_pReffArrU = m_pReffArrV = NULL;
}

//---------------------
// 矩形設定
//---------------------
void CDrawCell::setRect( float w, float h ){
	m_eType = eDRAW_TYPE_RECT;
	m_fRectW = w;
	m_fRectH = h;
}

// 画面全体
void CDrawCell::setRectFull( void ){
    m_eType = eDRAW_TYPE_RECT;
    m_fRectW = CFB_W;
    m_fRectH = CFB_H;
}

//---------------------------
// テクスチャ設定
//---------------------------
void CDrawCell::setTex( CTex* pTex, CTex* pTexP ){
    m_eType = eDRAW_TYPE_TEX;
    m_pTex = pTex;
    m_pTexP = pTexP;
}

//-----------------------------
// スプライト設定
//-----------------------------
void CDrawCell::setSpr( CSpr* pSpr ){
    m_eType = eDRAW_TYPE_SPR;
    m_pSpr = pSpr;
    m_pTex = m_pSpr->getTexDot();
    m_pTexP = m_pSpr->getTexPal();
}

//---------------------
// 文字列設定
//---------------------
void CDrawCell::setStr( eFONT fontId, const char* pStr, int dispLen ){
	m_eType = eDRAW_TYPE_STR;
	m_eFontId = fontId;
	changeStr( pStr, dispLen );

	// 描画調整
	CFont *pFont = CFontMgr::GetFont(fontId);
	setDrawSetType( pFont->getDrawSetType() );
}

//---------------------
// 数値設定
//---------------------
void CDrawCell::setNum( eNUM numId, int numVal ){
	m_eType = eDRAW_TYPE_NUM;
	m_eNumId = numId;
	changeNum( numVal );

	CNum* pNum = CNumMgr::GetNum( m_eNumId );
	setDrawSetType( pNum->getDrawSetType() );
}

//-----------------------
// ポリゴン設定
//-----------------------
void CDrawCell::setPoly( CTex* pTex, CTex*pTexP, int numVert, const float* pArrX, const float* pArrY, const float* pArrU, const float* pArrV ){
    m_eType = eDRAW_TYPE_POLY;
    m_pTex = pTex;
    m_pTexP = pTexP;
    m_nNumVertex = numVert;
    m_pReffArrX = pArrX;
    m_pReffArrY = pArrY;
    m_pReffArrU = pArrU;
    m_pReffArrV = pArrV;
}

//-----------------------
// 描画タイプ設定
//-----------------------
void CDrawCell::setDrawSetType( eDRAW_SET_TYPE type ){
    switch( type ){
    // パレット
    case eDRAW_SET_TYPE_P8:
        setShaderNormal();
        setBlendOff();
        break;
        
    // アルファ付きパレット
    case eDRAW_SET_TYPE_P8_ALPHA:
        setShaderNormal();
        setBlendAlpha();
        break;
        
    // フルカラー
    case eDRAW_SET_TYPE_RGBA:
        setShaderRGBA();
        break;
        
    // その他：アルファ画像
    default:
    case eDRAW_SET_TYPE_ALPHA:
        setShaderAlpha();
        break;
    }
}

//---------------------
// デプス設定
//---------------------
void CDrawCell::setDepth( float depth ){
	m_bDepthTest = true;
	m_fDepth = depth;
}

//---------------------
// テクスチャキャッシュ使用
//---------------------
void CDrawCell::setUseTexCache( bool flag, bool isTemporary ){
	m_bUseTexCache = flag;
	m_bTexCacheTemporay = isTemporary;
}

//---------------------
// 幅の取得（※１００％）
//---------------------
float CDrawCell::getOriginalWidth( void ){
	switch( m_eType ){
	case eDRAW_TYPE_RECT:	return( m_fRectW );
	case eDRAW_TYPE_TEX:	return( m_pTex->getWidthOrig() );
	case eDRAW_TYPE_SPR:	return( m_pSpr->getWidth() );
	case eDRAW_TYPE_STR:	return( getStrOriginalWidth() );
	case eDRAW_TYPE_NUM:	return( getNumOriginalWidth() );
	case eDRAW_TYPE_POLY:	return( getPolyOriginalWidth() );

	default:
		break;
	}

	// ここまできたら未対応
	return( 0.0f );
}

//---------------------
// 幅の取得（※実幅）
//---------------------
float CDrawCell::getWidth( void ){
    return( m_fScaleX * getOriginalWidth() );
    
}

//-------------------------
// 高さの取得（※１００％）
//-------------------------
float CDrawCell::getOriginalHeight( void ){
	switch( m_eType ){
	case eDRAW_TYPE_RECT:	return( m_fRectH );
	case eDRAW_TYPE_TEX:	return( m_pTex->getHeightOrig() );
	case eDRAW_TYPE_SPR:	return( m_pSpr->getHeight() );
	case eDRAW_TYPE_STR:	return( getStrOriginalHeight() );
	case eDRAW_TYPE_NUM:	return( getNumOriginalHeight() );
	case eDRAW_TYPE_POLY:	return( getPolyOriginalWidth() );

	default:
		break;
	}

	// ここまで来たら未対応
	return( 0.0f );
}

//---------------------
// 高さの取得（※実高）
//---------------------
float CDrawCell::getHeight( void ){
    return( m_fScaleY * getOriginalHeight() );
}

//-----------------------------
// 幅の設定（※内部的にはスケール変更）
//-----------------------------
void CDrawCell::setWidth( float w ){
	float baseW = getOriginalWidth();
	if( baseW > 0.0f ){ m_fScaleX = w/baseW; }
	else{ m_fScaleX = 1.0f; }
}

//-------------------------------
// 高さの設定（※内部的にはスケール変更）
//-------------------------------
void CDrawCell::setHeight( float h ){
	float baseH = getOriginalHeight();
	if( baseH > 0.0f ){ m_fScaleY = h/baseH; }
	else{ m_fScaleY = 1.0f; }
}

//-----------------------------
// シェーダー指定
//-----------------------------
// 通常
void CDrawCell::setShaderNormal( void ){
	m_eShader = eDRAW_SHADER_NORMAL;
}

// RGBA
void CDrawCell::setShaderRGBA( void ){
	m_eShader = eDRAW_SHADER_RGBA;
	m_pTexP = NULL;
	setBlendAlpha();
}

// 合成：α
void CDrawCell::setShaderAlpha( void ){
	m_eShader = eDRAW_SHADER_ALPHA;
	m_pTexP = NULL;
	setBlendAlpha();
}

//---------------------------------------------------------
// シェーダー取得（※描画時は、必ずこのメソッドを経由して取り出すこと）
//---------------------------------------------------------
CShader* CDrawCell::getShaderObject( void ){
	eSHADER shader = eSHADER_COLOR;
	float applyRate = 0.0f;

	switch( m_eType ){
	case eDRAW_TYPE_RECT:
		shader = eSHADER_COLOR;
		break;

	case eDRAW_TYPE_TEX:
	case eDRAW_TYPE_SPR:
		if( m_eShader == eDRAW_SHADER_ALPHA ){ shader = eSHADER_ALPHA; }
		else if( m_pTexP != NULL ){ shader = eSHADER_PAL8; }
		else{ shader = eSHADER_RGBA; }
		break;

	case eDRAW_TYPE_NUM:
	case eDRAW_TYPE_STR:
		if( m_eShader == eDRAW_SHADER_ALPHA ){ shader = eSHADER_ALPHA; }
		else if( m_eShader == eDRAW_SHADER_RGBA ){ shader = eSHADER_RGBA; }
		else{ shader = eSHADER_PAL8; }
		break;

	case eDRAW_TYPE_POLY:
		if( m_pTex == NULL ){ shader = eSHADER_COLOR; }
		else if( m_eShader == eDRAW_SHADER_ALPHA ){ shader = eSHADER_ALPHA; }
		else if( m_pTexP != NULL ){ shader = eSHADER_PAL8; }
		else{ shader = eSHADER_RGBA; }
		break;

	default:
		break;
	}
    
	// モノクロ差し替え
	if( m_fMonoRate > 0.0f || (!s_bIgnoreApplyRate && s_fMonoRate > 0.0f ) ){
		shader = (eSHADER)(shader+1);
		if( !s_bIgnoreApplyRate && s_fMonoRate > 0.0f ){
			applyRate = s_fMonoRate;
		}else{
			applyRate = m_fMonoRate;
		}
	}
	// ネガポジ差し替え
	else if( m_fNegaRate > 0.0f || (!s_bIgnoreApplyRate && s_fNegaRate > 0.0f ) ){
		// 加算／乗算指定はモノクロにしておく
		if( m_eBlend == eDRAW_BLEND_ADD || m_eBlend == eDRAW_BLEND_MULTI ){
			shader = (eSHADER)(shader+1);
		}else{
			shader = (eSHADER)(shader+2);
		}
		if( !s_bIgnoreApplyRate && s_fNegaRate > 0.0f ){
			applyRate = s_fNegaRate;
		}else{
			applyRate = m_fNegaRate;
		}
	}
	// セピア差し替え
	else if( m_fSepiaRate > 0.0f || (!s_bIgnoreApplyRate && s_fSepiaRate > 0.0f ) ){
		shader = (eSHADER)(shader+3);
		if( !s_bIgnoreApplyRate && s_fSepiaRate > 0.0f ){
			applyRate = s_fSepiaRate;
		}else{
			applyRate = m_fSepiaRate;
		}
	}

	//-------------------------------------------------
	// シェーダー取得取得
	//-------------------------------------------------
	CShader* pPrg = CShaderMgr::GetShader( shader );
    pPrg->setRenderForGame( m_bRenderForGame );
    pPrg->setRenderForSafeArea( m_bRenderForSafeArea );
    pPrg->setRenderWithIntPosition( m_bRenderWithIntPosition );
	pPrg->setApplyRate( applyRate );

	// 指定ブレンド値
	eDRAW_BLEND blend = m_eBlend;

	//----------------------------
	// シェーダー事前設定
	//----------------------------
	BYTE alpha = (BYTE)((s_nAlpha * m_nAlpha)/0xFF);

	// 色
	if( m_nRGBA == 0xFFFFFFFF && alpha >= 0xFF ){
		pPrg->setRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	}else{
		float r = ((m_nRGBA >> 24)&0xFF) / 255.0f;
		float g = ((m_nRGBA >> 16)&0xFF) / 255.0f;
		float b = ((m_nRGBA >>  8)&0xFF) / 255.0f;
		float a = ((m_nRGBA&0xFF)/255.0f) * (alpha/255.0f);

		// 加算の場合：アルファの反映（アルファが小さいほど加算の効果を小さく）
		if( blend == eDRAW_BLEND_ADD ){
			pPrg->setRGBA( r*a, g*a, b*a, 1.0f );
		}
		// 乗算の場合：アルファの反映（アルファが小さいほど乗算の効果を小さく）
		else if( blend == eDRAW_BLEND_MULTI ){
			pPrg->setRGBA( (1.0f-a)+r*a, (1.0f-a)+g*a, (1.0f-a)+b*a, 1.0f );
		}
		// その他
		else{
			pPrg->setRGBA( r, g, b, a );

			// アルファが有効であればブレンドオン
			if( a < 1.0f ){
				if( blend == eDRAW_BLEND_OFF ){
					blend = eDRAW_BLEND_ALPHA;
				}
			}
		}
	}

	// フレームバッファはデフォルトで縦反転（※Ｙ軸反転しているせいで描画毎に反転の必要がある）
	bool isFlipY = m_bFlipY;
	if( m_eType == eDRAW_TYPE_TEX ){
		if( m_pTex->isFrameBuffer() ){ isFlipY = !isFlipY; }
	}

	// フリップ
	pPrg->setFlipX( m_bFlipX );
	pPrg->setFlipY( isFlipY );

	// 回転
	pPrg->setRotation( m_fRotation );
	pPrg->setRotXY( m_fRotX, m_fRotY );

	// ブレンド
	switch( blend ){
	case eDRAW_BLEND_OFF:		CMainRenderer::SetBlendOff();		break;
	case eDRAW_BLEND_ALPHA:		CMainRenderer::SetBlendAlpha();		break;
	case eDRAW_BLEND_ALPHA_ONE:	CMainRenderer::SetBlendAlphaOne();	break;
	case eDRAW_BLEND_ADD:		CMainRenderer::SetBlendAdd();		break;
	case eDRAW_BLEND_MULTI:		CMainRenderer::SetBlendMulti();		break;
	default:
		break;
	}

	// デプステスト
	if( m_bDepthTest ){
		pPrg->setDepth( m_fDepth - s_fDepthOfs );
		CMainRenderer::SetDepthTestOn();
		s_fDepthOfs += 0.0001f;
	}else{
		pPrg->setDepth( 0.0f );
		CMainRenderer::SetDepthTestOff();
	}

	return( pPrg );
}

//-----------------------------
// 描画
//-----------------------------
void CDrawCell::draw0( float x, float y, bool isGame, bool isSafeArea, bool isInt ){
	// 無効確認
	if( s_nAlpha <= 0 || m_nAlpha <= 0 ){ return; }
	if( (m_nRGBA & 0xFF) <= 0 ){ return; }
	if( m_fScaleX <= 0.0f || m_fScaleY <= 0.0f ){ return; }

    // フラグの保持
    m_bRenderForGame = isGame;
    m_bRenderForSafeArea = isSafeArea;
    m_bRenderWithIntPosition = isInt;

	// Ｘ位置調整（※数字の左右は描画時に調整）
	if( m_eType != eDRAW_TYPE_NUM ){
		if( m_eAlignX == eDRAW_ALIGN_X_R ){ x -= getWidth(); }
		else if( m_eAlignX == eDRAW_ALIGN_X_C ){ x -= getWidth()/2.0f; }
	}

	// Ｙ位置調整
	if( m_eAlignY == eDRAW_ALIGN_Y_B ){ y -= getHeight(); }
	else if( m_eAlignY == eDRAW_ALIGN_Y_M ){ y -= getHeight()/2.0f; }
  
    // 全体補正
    x += s_fOfsX;
    y += s_fOfsY;

#ifdef USE_TEX_CACHE
	// 復旧用
	eDRAW_SHADER shader0 = m_eShader;
	eDRAW_BLEND blend0 = m_eBlend;
	CTex* pTex0 = m_pTex;
	CTex* pTexP0 = m_pTexP;

	// 指定があればキャッシュ確認
	CTex* pTexCache = NULL;
	if( m_bUseTexCache ){
		// 有効な設定であれば描画結果が[RGBA]のテクスチャで返る
		pTexCache = CTexCacheMgr::GetTex( m_eShader, m_pTex, m_pTexP, m_bTexCacheTemporay );

		// 結果が有効であれば
		if( pTexCache != NULL ){
			// [RGBA]描画設定に差し替え
			m_eShader = eDRAW_SHADER_NORMAL;
			m_pTex    = pTexCache;
			m_pTexP   = NULL;

			// ブレンドオフ指定の場合はαを有効に
			if( m_eBlend == eDRAW_BLEND_OFF ){
				m_eBlend = eDRAW_BLEND_ALPHA;
			}
		}
	}
#endif

	// 表示割り振り
	switch( m_eType ){
	case eDRAW_TYPE_RECT:	drawRect( x, y );		break;
	case eDRAW_TYPE_TEX:	drawTex( x, y );		break;
	case eDRAW_TYPE_SPR:	drawSpr( x, y );		break;
	case eDRAW_TYPE_STR:	drawStr( x, y );		break;
	case eDRAW_TYPE_NUM:	drawNum( x, y );		break;
	case eDRAW_TYPE_POLY:	drawPoly( x, y );		break;

	default:
		break;
	}

#ifdef USE_TEX_CACHE
	// 復旧
	if( pTexCache != NULL ){
		m_eShader = shader0;
		m_eBlend = blend0;
		m_pTex = pTex0;
		m_pTexP = pTexP0;
	}
#endif
}

//---------------------
// 矩形の描画
//---------------------
void CDrawCell::drawRect( float x, float y ){
	CShader* pPrg = getShaderObject();
    
	// 描画
	pPrg->pushRect( x, y, m_fScaleX*m_fRectW, m_fScaleY*m_fRectH );
	pPrg->render( NULL, NULL );
}

//---------------------
// テクスチャの描画
//---------------------
void CDrawCell::drawTex( float x, float y ){
	CShader* pPrg = getShaderObject();

	// フレームバッファはデフォルトで縦反転（※Ｙ軸反転しているせいで描画毎に反転の必要がある）
	bool isFlipY = (m_bFlipY != m_pTex->isFrameBuffer());

	// 基本スケール調整
	float sX = m_fScaleX;
	float sY = m_fScaleY;

	// 座標調整
	if( m_bFlipX ){ x += sX*(m_pTex->getOfsR()+m_fDrawTexCutR); }
	else{ x += sX*(m_pTex->getOfsL()+m_fDrawTexCutL); }

	if( isFlipY ){ y += sY*(m_pTex->getOfsB()+m_fDrawTexCutB); }
	else{ y += sY*(m_pTex->getOfsT()+m_fDrawTexCutT); }

	float texW = m_pTex->getWidth() - m_fDrawTexCutL - m_fDrawTexCutR;
	float texH = m_pTex->getHeight() - m_fDrawTexCutT - m_fDrawTexCutB;
	float w = sX * texW;
	float h = sY * texH;
	float u = 0.0f + m_fDrawTexCutL;
	float v = 0.0f + m_fDrawTexCutT;

	// 描画
	pPrg->pushTex( m_pTex, x, y, w, h, u, v, texW, texH );
	pPrg->render( m_pTex, m_pTexP );
}

//-----------------------------
// 描画：スプライト
//-----------------------------
void CDrawCell::drawSpr( float x, float y ){
	CShader* pPrg = getShaderObject();

	// 描画
	if( m_bSprWithUVScale ){
		pPrg->pushSprWithUVScale( m_pSpr, x, y, m_fScaleX, m_fScaleY );
	}else{
		pPrg->pushSpr( m_pSpr, x, y, m_fScaleX, m_fScaleY );
	}
    
	pPrg->render( m_pTex, m_pTexP );
}

//--------------------------
// 描画：文字列
//--------------------------
void CDrawCell::drawStr( float x, float y ){
	// 無効は無視
	if( m_pStr == NULL ){ return; }

	// フォント取得
	CFont* pFont = CFontMgr::GetFont( m_eFontId );
	CShader* pPrg = getShaderObject();

	// UTF8の配列の取得
	int len;
	int bufUtf8[DRAW_CELL_STR_DISP_LEN];
	if( !util_utf8_conv_to_int_array( bufUtf8, DRAW_CELL_STR_DISP_LEN, m_pStr, &len ) ){
		LOGE( "@ CDrawCell::drawStr() FAILED, m_pStr=%s\n", m_pStr );
		return;
	}

	// 表示サイズが有効なら差し替え
	if( m_nDispLen >= 0 && m_nDispLen < len ){
		len = m_nDispLen;
	}

	// 一回で描画するか？
	bool isRenderAtOnce = pFont->isRenderAtOnce();

	// テクスチャを取り出しておく
	CSpr* pS = pFont->getSprChar( 0 );	// [UNKNOWN]が返る
	CTex* pTexFont = pS->getTexDot();
	CTex* pPalFont = pS->getTexPal();

	int i, c;
	float x0 = x;
	for( i=0; i<len; i++ ){
		c = util_utf8_decode( bufUtf8[i] );
		if( c == '\n' ){
			x = x0;
			y += m_fScaleY * pFont->getLineHeight();
			continue;
		}

		pS = pFont->getSprChar( c );

		// 座標データ設定
		pPrg->pushSpr( pS, x+pS->getCharOfsX()*m_fScaleX, y+pS->getCharOfsY()*m_fScaleY, m_fScaleX, m_fScaleY );
		if( !isRenderAtOnce ){
			pPrg->render( pTexFont, pPalFont );
		}

		x += m_fScaleX*pS->getCharAdvX();

#if 1
		// 微調整（※拡大すると文字間が空くので）
		if( m_fScaleX > 1.0f ){
			x -= (m_fScaleX-1.0f);
		}
		x += m_fScaleX*m_fStrMarginX;
#endif
	}

	//----------------------------------------------------------
	// 描画（※おなじテクスチャを参照するものとして一回で登録）
	//----------------------------------------------------------
	// 文字列は右下へ向かって配置されるので、文字と文字の間に出来る
	// 不要な矩形の頂点は左回りになるので、裏向きポリゴンとなり表示されない
	//（※基本的には１行でＹ座標の上下がないフォントデータでのみ利用可能）
	//----------------------------------------------------------
	if( isRenderAtOnce ){
		pPrg->render( pTexFont, pPalFont );
	}
}

//--------------------------
// 文字列の横幅（※１００％）
//--------------------------
float CDrawCell::getStrOriginalWidth( void ){
	// 無効は無視
	if( m_pStr == NULL ){ return( 0.0f ); }

	// フォント取得
	CFont* pFont = CFontMgr::GetFont( m_eFontId );

	// UTF8の配列の取得
	int len;
	int bufUtf8[DRAW_CELL_STR_DISP_LEN];
	if( !util_utf8_conv_to_int_array( bufUtf8, DRAW_CELL_STR_DISP_LEN, m_pStr, &len ) ){
		LOGE( "@ CDrawCell::drawStr() FAILED, m_pStr=%s\n", m_pStr );
		return( 0.0f );
	}

	float w = 0.0f;
	float maxW = 0.0f;
	CSpr* pS = NULL;
	for( int i=0; i<len; i++ ){
		int c = util_utf8_decode( bufUtf8[i] );
		if( c == '\r' ){ continue; }
		if( c == '\n' ){
			if( maxW < w ){ maxW = w; }
			w = 0.0f;
			continue;
		}
		pS = pFont->getSprChar( c );
		w += pS->getCharAdvX();

#if 1
		// 微調整（※拡大すると文字間が空くので）
		if( m_fScaleX > 1.0f ){
			w -= (m_fScaleX-1.0f)/m_fScaleX;
		}
		w += m_fStrMarginX;	// （※こいつは倍率に影響される）
#endif
	}

	if( maxW < w ){ maxW = w; }

	return( maxW );
}

//--------------------------
// 文字列の縦幅（※１００％）
//--------------------------
float CDrawCell::getStrOriginalHeight(){
	// 無効は無視（※空文字）
	if( m_pStr == NULL ){ return( 0.0f ); }

	// フォント取得
	CFont* pFont = CFontMgr::GetFont( m_eFontId );

	// UTF8の配列の取得
	int len;
	int bufUtf8[DRAW_CELL_STR_DISP_LEN];
	if( !util_utf8_conv_to_int_array( bufUtf8, DRAW_CELL_STR_DISP_LEN, m_pStr, &len ) ){
		LOGE( "@ CDrawCell::drawStr() FAILED, m_pStr=%s\n", m_pStr );
		return( 0.0f );
	}

	int numLine = 1;
	for( int i=0; i<len; i++ ){
		int c = util_utf8_decode( bufUtf8[i] );
		if( c == '\n' ){
			numLine++;
		}
	}

	return( numLine * pFont->getLineHeight() );
}

//--------------------------
// 描画：数字
//--------------------------
void CDrawCell::drawNum( float x, float y ){
	CShader* pPrg = getShaderObject();
    CNum* pNum = CNumMgr::GetNum( m_eNumId );
    CGrp* pGrp = CGrpMgr::GetGrp( pNum->getGrpId() );
	float marginX = pNum->getMarginX();

	// 位置調整：Ｘ
	if( m_eAlignX == eDRAW_ALIGN_X_L ){
		x += getWidth();
	}else if( m_eAlignX == eDRAW_ALIGN_X_C ){
		x += getWidth()/2.0f;
	}

	int v = m_nNum;
	bool isMinus = false;

	if( v < 0 ){
		v = -v;
		isMinus = true;
	}

	int length = getNumFigure( v );

	// 符号例外：力ずく
	if( isMinus || m_bNumWithPlus ){
		length++;
	}

	// 一回で描画するか？
	bool isRenderAtOnce = pNum->isRenderAtOnce();

    CSpr* pSpr = pGrp->getSprAt( 0 );
	CTex* pTex = pSpr->getTexDot();
	CTex* pPal = pSpr->getTexPal();
	for( int i=0; i<length; i++ ){
		// 表示対象確認
		int target = v%10;
		if( i == (length-1) ){	// 最後の１桁は符号の可能性
			if( isMinus ){ target = 10; }				// -
			else if( m_bNumWithPlus ){ target = 11; }	// +
		}

        pSpr = pGrp->getSprAt( target );

		// 数字は右揃えが基本なので描画前に調整
		x -= m_fScaleX*pSpr->getWidth();

		// 座標データ設定
		pPrg->pushSpr( pSpr, x, y, m_fScaleX, m_fScaleY );

		// 桁の色が指定されていたら色の再設定
		if( m_nArrFigureRGBA != NULL ){
			int rgba = m_nArrFigureRGBA[i];
			float r = ((rgba >> 24)&0xFF) / 255.0f;
			float g = ((rgba >> 16)&0xFF) / 255.0f;
			float b = ((rgba >>  8)&0xFF) / 255.0f;
			float a = (rgba&0xFF)/255.0f;
			pPrg->setRGBA( r, g, b, a );
		}

		if( ! isRenderAtOnce ){
			pPrg->render( pTex, pPal );
		}

		v /= 10;
		x -= m_fScaleX*(marginX + m_fFigureMarginX);
	}

	//----------------------------------------------------------
	// スプライト描画（※同じテクスチャを参照しているものとして一回で登録）
	//----------------------------------------------------------
	// 文字列は右下へ向かって配置されるので、文字と文字の間に出来る
	// 不要な矩形の頂点は左回りになるので、裏向きポリゴンとなり表示されない
	//（※基本的には１行でＹ座標の上下がないフォントデータでのみ利用可能）
	//----------------------------------------------------------
	if( isRenderAtOnce ){
		pPrg->render( pTex, pPal );
	}
}

//-----------------------------------------
// 数字の桁を調べる（※符号は無視）
//-----------------------------------------
int CDrawCell::getNumFigure( int v ){
	if( v < 0 ){
		v = -v;
	}

	int figure = 1;
	while( v >= 10 ){
		figure++;
		v /= 10;
	}

	if( m_nForceFigureNum > figure ){
		figure = m_nForceFigureNum;
	}

	return( figure );
}

//-------------------------------
// 数字の横幅を調べる（※１００％）
//-------------------------------
float CDrawCell::getNumOriginalWidth( void ){
	CNum* pNum = CNumMgr::GetNum( m_eNumId );
    CGrp* pGrp = CGrpMgr::GetGrp( pNum->getGrpId() );
	int marginX = pNum->getMarginX();
	int v = m_nNum;
	bool isMinus = false;

	if( v < 0 ){
		v = -v;
		isMinus = true;
	}
	int length = getNumFigure( v );

    CSpr* pSpr;
	float w = 0.0f;
	for( int i=0; i<length; i++ ){
        pSpr = pGrp->getSprAt( v%10 );
		w += pSpr->getWidth();
		v /= 10;
	}

	// 符号
	if( isMinus ){
        pSpr = pGrp->getSprAt( 10 );
		w += pSpr->getWidth();	// -
		length += 1;
	}else if( m_bNumWithPlus ){
        pSpr = pGrp->getSprAt( 11 );
        w += pSpr->getWidth();  // +
		length += 1;
	}

	// マージン調整
	w += (marginX + m_fFigureMarginX) * (length-1);
	return( w );
}

//-------------------------------------------------------------------------
// 数字の縦幅を調べる（※１００％）（※全ての数字は同じ高さの想定＝[０]の高さを返しておく）
//-------------------------------------------------------------------------
float CDrawCell::getNumOriginalHeight( void ){
	CNum* pNum = CNumMgr::GetNum( m_eNumId );
    CGrp* pGrp = CGrpMgr::GetGrp( pNum->getGrpId() );
    CSpr* pSpr = pGrp->getSprAt( 0 );
	return( pSpr->getHeight() );
}

//---------------------
// ポリゴンの描画
//---------------------
void CDrawCell::drawPoly( float x, float y ){
	CShader* pPrg = getShaderObject();

	// 頂点の登録
	if( m_pTex != NULL ){
		for( int i=0; i<m_nNumVertex; i++ ){
			pPrg->pushVertex( x + m_fScaleX*m_pReffArrX[i], y + m_fScaleY*m_pReffArrY[i], m_pReffArrU[i], m_pReffArrV[i] );
		}
	}else{
		for( int i=0; i<m_nNumVertex; i++ ){
			pPrg->pushVertex( x + m_fScaleX*m_pReffArrX[i], y + m_fScaleY*m_pReffArrY[i], 0.0f, 0.0f );
		}
	}

	pPrg->render( m_pTex, m_pTexP );
}

//------------------------------
// ポリゴンの横幅を調べる（※１００％）
//------------------------------
float CDrawCell::getPolyOriginalWidth( void ){
	float xL = m_pReffArrX[0];
	float xR = m_pReffArrX[0];
	for( int i=1; i<m_nNumVertex; i++ ){
		if( xL > m_pReffArrX[i] ){ xL = m_pReffArrX[i]; }
		if( xR < m_pReffArrX[i] ){ xR = m_pReffArrX[i]; }
	}

	return( xR - xL );
}

//------------------------------
// ポリゴンの縦幅を調べる（※１００％）
//------------------------------
float CDrawCell::getPolyOriginalHeight( void ){
	float yT = m_pReffArrY[0];
	float yB = m_pReffArrY[0];
	for( int i=1; i<m_nNumVertex; i++ ){
		if( yT > m_pReffArrY[i] ){ yT = m_pReffArrY[i]; }
		if( yB < m_pReffArrY[i] ){ yB = m_pReffArrY[i]; }
	}

	return( yB - yT );
}
