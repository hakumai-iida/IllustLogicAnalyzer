/*+----------------------------------------------------------------+
  |	Title:		TexCacheMgr.cpp [共通環境]
  |	Comment:	テクスチャキャッシュ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TexCacheMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保枠
#define MAX_TEX_CACHE		256

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 管理データ
CTexCache* CTexCacheMgr::s_oArrTexCache;
CTexCache* CTexCacheMgr::s_pTexCacheTemporary;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CTexCacheMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexCacheMgr::OnCreate()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
	s_oArrTexCache = new CTexCache[MAX_TEX_CACHE];
	s_pTexCacheTemporary = new CTexCache();
	CMemMgr::PopTargetField();

	return( true );
}

//------------------------
// 終了
//------------------------
void CTexCacheMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexCacheMgr::OnDestroy()\n" );
#endif

	// 解放
	SAFE_DELETE_ARRAY( s_oArrTexCache );
	SAFE_DELETE( s_pTexCacheTemporary );
}

//--------------------------
// 更新
//--------------------------
void CTexCacheMgr::Update( void ){
	for( int i=0; i<MAX_TEX_CACHE; i++ ){
		s_oArrTexCache[i].update();
	}
}

//--------------------------
// 最大枠数取得
//--------------------------
int CTexCacheMgr::GetTexCacheNumMax( void ){
	return( MAX_TEX_CACHE );
}

//--------------------------
// 使用数取得
//--------------------------
int CTexCacheMgr::GetTexCacheNumUse( void ){
	int num = 0;
	for( int i=0; i<MAX_TEX_CACHE; i++ ){
		if( s_oArrTexCache[i].isValid() ){ num++; }
	}
	return( num );
}

//--------------------------
// キャッシュの無効化
//--------------------------
void CTexCacheMgr::InvalidateTex( CTex* pTexDot ){
	// 用心
	if( pTexDot == NULL ){ return; }

	// キャッシュ確認
	for( int i=0; i<MAX_TEX_CACHE; i++ ){
		// ＩＤが一致するキャッシュはすべて無効化
		if( s_oArrTexCache[i].getTexDot() == pTexDot ){
			s_oArrTexCache[i].invalidate();
		}
	}
}

//--------------------------
// テクスチャ取得
//--------------------------
CTex* CTexCacheMgr::GetTex( eDRAW_SHADER shader, CTex* pTexDot, CTex* pTexPal,  bool isTemporary ){
	CTexCache* pTarget = s_pTexCacheTemporary;

	// 指定があればキャッシュ確認
	if( ! isTemporary ){
		// キャッシュにヒットしたらそのテクスチャを返す
		for( int i=0; i<MAX_TEX_CACHE; i++ ){
			if( s_oArrTexCache[i].isCacheHit( shader, pTexDot, pTexPal ) ){
				return( s_oArrTexCache[i].getCachedTex() );
			}
		}

		// 利用可能なキャッシュを探す
		int target = -1;
		for( int i=0; i<MAX_TEX_CACHE; i++ ){
			if( !s_oArrTexCache[i].isValid() ){
				target = i;
				break;
			}
		}

		// 空きがなければ一番利用されていないキャッシュを探す
		if( target < 0 ){
			target = 0;
			for( int i=1; i<MAX_TEX_CACHE; i++ ){
				if( s_oArrTexCache[target].getUnusedCount() < s_oArrTexCache[i].getUnusedCount() ){
					target = i;
				}
			}
		}

		// 対象取り出し
		pTarget = &s_oArrTexCache[target];
	}

	// 指定枠にキャッシュしてテクスチャを返す
	pTarget->cacheTex( shader, pTexDot, pTexPal );
	return( pTarget->getCachedTex() );
}
