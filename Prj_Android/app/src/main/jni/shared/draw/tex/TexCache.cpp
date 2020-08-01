/*+----------------------------------------------------------------+
  |	Title:		TexCache.cpp [GLES2.0環境]
  |	Comment:	テクスチャキャッシュ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TexCache.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// チェック指定
#define CHECK_TEX_CACHE_SETTING

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
//---------------------------
// コンストラクタ
//---------------------------
CTexCache::CTexCache( void ): CTexCacheBase(){}

//---------------------------
// 無効化
//---------------------------
void CTexCache	::invalidate( void ){
	m_eDrawShader = eDRAW_SHADER_INVALID;
	m_pTexSrcDot = NULL;
	m_pTexSrcPal = NULL;
	m_nId = -1;
	clear();
}

//---------------------------
// 汎用：キャッシュ判定
//---------------------------
bool CTexCache::isCacheHit( eDRAW_SHADER shader, CTex* pTexDot, CTex* pTexPal ){
	if( isValid() ){
		if( m_eDrawShader != shader ){ return( false ); }
		if( m_pTexSrcDot != pTexDot ){ return( false ); }
		if( m_pTexSrcPal != pTexPal ){ return( false ); }
		return( true );
	}

	return( false );
}

//-----------------------------------------------------------------------------------
// 汎用：テクスチャ設定
//-----------------------------------------------------------------------------------
#define START_TEX_CACHE( _r, _g, _b, _a )										    \
	/* ソーステクスチャのサイズを取得（※実寸） */									        \
	int w = (int)pTexDot->getWidth();												\
	int h = (int)pTexDot->getHeight();												\
																					\
	/* 作業バッファを作成し直す（※テクスチャを確保）*/								        \
	CFrameBuffer* pWorkTex;															\
	pWorkTex = CFrameBufferMgr::GetFrameBuffer( eFRAME_BUFFER_WORK_CHARA );			\
																					\
	/* 作業バッファ解放 */															    \
	/* テクスチャ領域はキャッシュインスタンスの管理下にあるため解放しない */		                \
	pWorkTex->release( true );														\
	pWorkTex->create( w, h );														\
																					\
	/* 描画先を作業バッファへ */													        \
	CMainRenderer::SetRenderTarget( eFRAME_BUFFER_WORK_CHARA, true );				\
																					\
	/* ワーク確保 */																    \
	CDrawCell* pDC = &s_oDC
//-----------------------------------------------------------------------------------
#define END_TEX_CACHE()															    \
	/* 描画先を戻す */																    \
	CMainRenderer::ResetRenderTarget( false );										\
																					\
	/* キャッシュにテクスチャ情報をコピー */										        \
	m_oTex.copyForCache( pWorkTex->getGlTexture(), pTexDot );						\
																					\
	resetUnusedCount()
//-----------------------------------------------------------------------------------
CTex* CTexCache::cacheTex( eDRAW_SHADER shader, CTex* pTexDot, CTex* pTexPal ){
#ifdef CHECK_TEX_CACHE_SETTING
	// シェーダー無効は無視
	if( shader < (eDRAW_SHADER)0 || shader >= eDRAW_SHADER_MAX ){
		LOGE( "@ CTexCache::cacheTex: INVALID SHADER:%d\n", shader );
		return( NULL );
	}

	// αシェーダーは無視（※パレット画像のジャギ対策以外に意味はない）
	if( shader == eDRAW_SHADER_ALPHA ){
		LOGE( "@ CTexCache::cacheTex: INVALID SHADER: %d\n", shader );
		return( NULL );
	}

	// パレット画像でなければ無視（※パレット画像のジャギ対策以外に意味はない）
	if( pTexDot == NULL || pTexPal == NULL ){
		LOGE( "@ CTexCache::cacheTex: INVALID TEXTURE: dot=%p, pal=%p\n", pTexDot, pTexPal );
		return( NULL );
	}
#endif

	// キャッシュ開始
	START_TEX_CACHE( pTexPal->getR(), pTexPal->getG(), pTexPal->getB(), 0x00 );

	// 作業バッファに書き込む
	pDC->clear();
	pDC->setTex( pTexDot, pTexPal );
	switch( shader ){
	case eDRAW_SHADER_ALPHA:		pDC->setShaderAlpha();		break;

	default:
	case eDRAW_SHADER_NORMAL:
		break;
	}

	// 指定があればαを有効化
	if( pTexDot->isUseAlphaForTexCache() ){
		pDC->setBlendAlpha();
	}

	// GL座標系は下から上が正なので上下反転
	pDC->setFlipY( true );

	//-----------------------------------------------------------------------------
	// 表示：ここではサイズと色見を100%で描画する
	//-----------------------------------------------------------------------------
	// 画像の実寸に収まるようにオフセット分ずらす（※余白部分を省いて描画する）
	// （※[CDrawCell::draw]ではテクスチャのオフセットを加味して描画するため）
	//-----------------------------------------------------------------------------
	pDC->draw( -pTexDot->getOfsL(), -pTexDot->getOfsB() );

	// キャッシュ終了
	END_TEX_CACHE();

	// キャッシュ情報を更新
	m_eDrawShader = shader;
	m_pTexSrcDot  = pTexDot;
	m_pTexSrcPal  = pTexPal;
	return( &m_oTex );
}

//---------------------------
// キャッシュ判定（※ID）
//---------------------------
bool CTexCache::isCacheHitForId( int id ){
	if( isValid() ){
		if( m_nId != id ){ return( false ); }
		return( true );
	}

	return( false );
}
