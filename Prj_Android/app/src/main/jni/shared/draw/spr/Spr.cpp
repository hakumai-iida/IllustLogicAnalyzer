/*+----------------------------------------------------------------+
  |	Title:		Spr.cpp [共通環境]
  |	Comment:	スプライト
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Spr.hpp"

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
//------------------------
// コンストラクタ
//------------------------
CSpr::CSpr( void ){
	clear();
}

//------------------------
// クリア
//------------------------
void CSpr::clear( void ){
	m_eFrameBufferId = eFRAME_BUFFER_INVALID;
	m_eImgId = eIMG_INVALID;
	m_nImgDotNo = 0;
	m_fWidth = m_fHeight = 0;
	m_fU0 = m_fV0 = m_fU1 = m_fV1 = 0.0f;

	m_nCharCode = 0;
	m_fCharOfsX = m_fCharOfsY = 0;
	m_fCharAdvX = 0;
}

//-------------------
// 判定
//-------------------
bool CSpr::isValid( void ){
	if( m_eFrameBufferId != eFRAME_BUFFER_INVALID || m_eImgId != eIMG_INVALID  ){
		return( true );
	}

	return( false );
}

//------------------------
// 画素テクスチャ取得
//------------------------
CTex* CSpr::getTexDot( void ){
	if( m_eFrameBufferId != eFRAME_BUFFER_INVALID ){
		return( CFrameBufferMgr::GetFrameBuffer( m_eFrameBufferId ) );
	}

	if( m_eImgId != eIMG_INVALID ){
		return( CImgMgr::GetImgDot( m_eImgId, m_nImgDotNo ) );
	}

	return( NULL );
}

//------------------------
// パレットテクスチャ取得
//------------------------
CTex* CSpr::getTexPal( void ){
	if( m_eImgId != eIMG_INVALID ){
		return( CImgMgr::GetImgPal( m_eImgId ) );
	}

	// フレームバッファにはパレットがない
	return( NULL );
}

//------------------------
// バイナリ適用
//------------------------
bool CSpr::applyBin( CSprBinData* pBin ){
    if( pBin == NULL ){
        return( false );
    }
    
    return( setWithImg( pBin->getImg(), pBin->getNo(), pBin->getU0(), pBin->getV0(), pBin->getU1(), pBin->getV1() ) );
}

//------------------------
// バイナリ適用（※フォント用）
//------------------------
bool CSpr::applyBinForChar( CFontBinData* pBin, CFontSprBinData* pSprBin ){
    if( pBin == NULL || pSprBin == NULL ){
        return( false );
    }

    return( setWithImgForChar( pBin->getImg(), pSprBin->getNo(),
                               pSprBin->getU0(), pSprBin->getV0(), pSprBin->getU1(), pSprBin->getV1(), pSprBin->getCode(),
                               pSprBin->getOfsX(), pSprBin->getOfsY(), pSprBin->getAdvX(), pBin->getAdjX() ) );
}

//------------------------
// 設定（※画像経由）
//------------------------
bool CSpr::setWithImg( eIMG imgId, int no, int u0, int v0, int u1, int v1 ){
	if( set0( CImgMgr::GetImgDot( imgId, no ), u0, v0, u1, v1 ) ){
		m_eFrameBufferId = eFRAME_BUFFER_INVALID;
		m_eImgId = imgId;
		m_nImgDotNo = no;
		return( true );
	}

	return( false );
}

//-------------------------------
// 設定（※フレームバッファ経由）
//-------------------------------
bool CSpr::setWithFrameBuffer( eFRAME_BUFFER fbId, int u0, int v0, int u1, int v1 ){
	CFrameBuffer* pFB = CFrameBufferMgr::GetFrameBuffer( fbId );
	if( pFB == NULL ){
		return( false );
	}

	// フレームバッファの内容は上下が反転している
	int h = (int)pFB->getHeight();
	int _v0 = h-v0;
	int _v1 = h-v1;
	if( set0( pFB, u0, _v0, u1, _v1 ) ){
		m_eFrameBufferId = fbId;
		m_eImgId = eIMG_INVALID;
		return( true );
	}

	return( false );
}

//------------------------
// フォントの１文字用設定
//------------------------
bool CSpr::setWithImgForChar( eIMG imgId, int no, int u0, int v0, int u1, int v1, uint32 c, int oX, int oY, int advX, int adjAdvX ){
	// 基本設定
	if( ! setWithImg( imgId, no, u0, v0, u1, v1 ) ){
		return( false );
	}

	m_nCharCode = c;
	m_fCharOfsX = oX;
	m_fCharOfsY = oY;

	// 文字送りが有効であれば（※BmpFontで変換したフォントデータ）
	if( advX > 0 ){
		m_fCharAdvX = advX;

		// 横のずらしがおかしなフォント用に調整
		if( adjAdvX < 0 ){
			float tempX = (u1-u0+oX) - advX;
			if( adjAdvX < tempX ){
				m_fCharAdvX += tempX - adjAdvX;
			}
		}
	}
	// 文字送りが無効であれば(※手作業で作ったフォントデータ)
	else{
		m_fCharAdvX = m_fWidth + m_fCharOfsX;
	}

	return( true );
}

//------------------------
// 設定（実体）
//------------------------
bool CSpr::set0( CTex* pTex, int u0, int v0, int u1, int v1 ){
	// テクスチャが無効であれば失敗
	if( pTex == NULL ){
		return( false );
	}

	// 右から左への設定にも対応してみる
	m_fWidth  = u1 - u0;
	if( m_fWidth < 0 ){
		m_fWidth *= -1;
	}

	// 下から上への設定にも対応してみる
	m_fHeight = v1 - v0;
	if( m_fHeight < 0 ){
		m_fHeight *= -1;
	}

	m_fU0 = u0/pTex->getWidth();
	m_fV0 = v0/pTex->getHeight();
	m_fU1 = u1/pTex->getWidth();
	m_fV1 = v1/pTex->getHeight();

	return( true );
}
