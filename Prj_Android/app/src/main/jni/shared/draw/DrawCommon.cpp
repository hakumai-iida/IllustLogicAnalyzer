/*+----------------------------------------------------------------+
  |	Title:		DrawCommon.cpp [共通環境]
  |	Comment:	表示共通
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DrawCommon.hpp"

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
//-------------------------------------------------
// ウィンドウの表示（※[eSPR_WINDOW]の素材に合わせた設定）
// 構成要素としてSprite[9]の描画グループを想定
//-------------------------------------------------
void CDrawCommon::RenderWindow( eGRP grpId, float x, float y, float wC, float hC, float sX, float sY, int rgba,
                                eIMG palId, float* pW, float* pH, bool isCalcOnly ){
	CGrp* pGrp = CGrpMgr::GetGrp( grpId );
    
    // データがなければ矩形で代用
    if( pGrp == NULL ){
        CDrawCell* pDC = CDrawCell::GetFreeCell();
        
        pDC->clear();
        pDC->setRect( 8+wC+8, 21+hC+4 ); // 上のみ幅広で、左右下は狭い想定
        pDC->setRGBA( rgba );
        pDC->setBlendAlpha();
        pDC->draw( x, y );
        return;
    }
    
    CSpr* pSpr;

	// 左上の横／縦幅
    pSpr = pGrp->getSprAt( 0 );
	float wL = pSpr->getWidth();
	float hT = pSpr->getHeight();

	// サイズ検出（右と下から判定）
    pSpr = pGrp->getSprAt( 2 );
	float w = wL + wC + pSpr->getWidth();

    pSpr = pGrp->getSprAt( 8 );
	float h = hT + hC + pSpr->getHeight();
    
	if( pW != NULL ){ *pW = w; }
	if( pH != NULL ){ *pH = h; }

	// 描画
	if( ! isCalcOnly ){
		CDrawCell* pDC = CDrawCell::GetFreeCell();
        pDC->clear();
        
        pSpr = pGrp->getSprAt( 0 );
        pDC->setSpr( pSpr );

		CShader* pPrg = pDC->getShaderObject();
        pPrg->setRenderForGame( true );
        pPrg->setRenderForSafeArea( true );
        pPrg->setRenderWithIntPosition( true );

		float x0 = x;
        pSpr = pGrp->getSprAt( 4 );
		float scaleWC = wC/pSpr->getWidth();
		float scaleHC = hC/pSpr->getHeight();

		// 上段：左
        pSpr = pGrp->getSprAt( 0 );
		pPrg->pushSpr( pSpr, x, y, sX, sY );
		x += wL * sX;

		// 上段：中
		if( wC > 0 ){
            pSpr = pGrp->getSprAt( 1 );
			pPrg->pushSpr( pSpr, x, y, sX*scaleWC, sY );
			x += wC * sX;
		}

		// 上段：右
        pSpr = pGrp->getSprAt( 2 );
		pPrg->pushSpr( pSpr, x, y, sX, sY );
		y += hT * sY;

		// 中段断：左
		if( hC > 0 ){
			x = x0;
            pSpr = pGrp->getSprAt( 3 );
			pPrg->pushSpr( pSpr, x, y, sX, sY*scaleHC );
			x += wL * sX;

			// 中段：中
			if( wC > 0 ){
                pSpr = pGrp->getSprAt( 4 );
				pPrg->pushSpr( pSpr, x, y, sX*scaleWC, sY*scaleHC );
				x += wC * sX;
			}

			// 中段：右
            pSpr = pGrp->getSprAt( 5 );
			pPrg->pushSpr( pSpr, x, y, sX, sY*scaleHC );
			y += hC * sY;
		}

		// 下段：左
		x = x0;
        pSpr = pGrp->getSprAt( 6 );
		pPrg->pushSpr( pSpr, x, y, sX, sY );
		x += wL * sX;

		// 下段：中
		if( wC > 0 ){
            pSpr = pGrp->getSprAt( 7 );
			pPrg->pushSpr( pSpr, x, y, sX*scaleWC, sY );
			x += wC * sX;
		}

		// 下段：右
        pSpr = pGrp->getSprAt( 8 );
		pPrg->pushSpr( pSpr, x, y, sX, sY );

		CMainRenderer::SetBlendAlpha();

		float r = ((rgba>>24)&0xFF)/255.0f;
		float g = ((rgba>>16)&0xFF)/255.0f;
		float b = ((rgba>>8)&0xFF)/255.0f;
		float a = (rgba&0xFF)/255.0f;
		pPrg->setRGBA( r, g, b, a );

		CTex* pTexPal;
        pSpr = pGrp->getSprAt( 0 );
		if( palId != eIMG_INVALID ){ pTexPal = CImgMgr::GetImgPal( palId ); }
		else{ pTexPal = pSpr->getTexPal(); }
		pPrg->render( pSpr->getTexDot(), pTexPal );
	}
}

//-----------------------------------------------------------
// 矩形による窓
// 横幅[mX + wC + mX]、縦幅[mY + hC + mY]の土台を描いた後
// 上下左右から１ピクセル喰い込んだ[border]サイズの枠線を描く
//-----------------------------------------------------------
void CDrawCommon::RenderWindowRect( float x, float y, float wC, float hC, DWORD rgbaBase, DWORD rgbaBorder,
                                    float mX, float mY, float border ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();

	// 土台表示
    pDC->clear();
	pDC->setRect( wC + 2*mX, hC + 2*mY );
	pDC->setRGBA( rgbaBase );
	pDC->setBlendAlpha();
	pDC->draw( x - mX, y - mY );

	// 枠線の表示
    pDC->clear();
	pDC->setRect( wC + 2*(mX-1), border );
	pDC->setRGBA( rgbaBorder );
	pDC->draw( x - mX + 1, y - mY + 1 );
	pDC->draw( x - mX + 1, y + hC + mY - (border+1) );

    pDC->clear();
	pDC->setRect( border, hC + 2*(mY-1) );
    pDC->setRGBA( rgbaBorder );
	pDC->draw( x - mX + 1, y - mY + 1 );
	pDC->draw( x + wC + mX - (border+1), y - mY + 1 );
}

