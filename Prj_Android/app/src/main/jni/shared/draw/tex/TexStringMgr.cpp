/*+----------------------------------------------------------------+
  |	Title:		TexStringMgr.cpp [共通環境]
  |	Comment:	テクスチャ文字列管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TexStringMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保枠
#define MAX_TEX_STRING		256

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
CTexString* CTexStringMgr::s_oArrTexString;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CTexStringMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexStringMgr::OnCreate()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
	s_oArrTexString = new CTexString[MAX_TEX_STRING];
	CMemMgr::PopTargetField();

	return( true );
}

//------------------------
// 終了
//------------------------
void CTexStringMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexStringMgr::OnDestroy()\n" );
#endif

	// 解放
	SAFE_DELETE_ARRAY( s_oArrTexString );
}

//--------------------------
// 更新
//--------------------------
void CTexStringMgr::Update( void ){
	for( int i=0; i<MAX_TEX_STRING; i++ ){
		s_oArrTexString[i].update();
	}
}

//--------------------------
// 最大枠数取得
//--------------------------
int CTexStringMgr::GetTexStringNumMax( void ){
	return( MAX_TEX_STRING );
}

//--------------------------
// 使用数取得
//--------------------------
int CTexStringMgr::GetTexStringNumUse( void ){
	int num = 0;
	for( int i=0; i<MAX_TEX_STRING; i++ ){
		if( s_oArrTexString[i].isValid() ){ num++; }
	}
	return( num );
}

//--------------------------
// テクスチャ取得
//--------------------------
CTex* CTexStringMgr::GetTex( const char* pStr, int type, int size ){
#if 1
	// 空文字調整：空文字だとテクスチャの生成がうまくいかない(？)ので半角スペースに置き換える
	if( pStr == NULL || pStr[0] == '\0' ){
		pStr = " ";
	}
#endif

	// キャッシュ確認
	for( int i=0; i<MAX_TEX_STRING; i++ ){
		// ヒットするキャッシュがあればテクスチャを返す
		if( s_oArrTexString[i].isCacheHit( pStr, type, size ) ){
			return( s_oArrTexString[i].getCachedTex() );
		}
	}

	// 利用可能なキャッシュを探す
	int target = -1;
	for( int i=0; i<MAX_TEX_STRING; i++ ){
		if( !s_oArrTexString[i].isValid() ){
			target = i;
			break;
		}
	}

	// 空きがなければ一番利用されていないキャッシュを探す
	if( target < 0 ){
		target = 0;
		for( int i=1; i<MAX_TEX_STRING; i++ ){
			if( s_oArrTexString[target].getUnusedCount() < s_oArrTexString[i].getUnusedCount() ){
				target = i;
			}
		}
	}

	// キャッシュして返す
	s_oArrTexString[target].cacheTex( pStr, type, size );
	return( s_oArrTexString[target].getCachedTex() );
}
