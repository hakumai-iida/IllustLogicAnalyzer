/*+----------------------------------------------------------------+
  |	Title:		TexCacheMgr.hpp [共通環境]
  |	Comment:	テクスチャキャッシュ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TEX_CACHE_MGR_HPP__
#define __TEX_CACHE_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TexCache.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// テクスチャキャッシュ管理
//----------------------------------------
class CTexCacheMgr{
private:
	// 管理データ
	static CTexCache* s_oArrTexCache;
	static CTexCache* s_pTexCacheTemporary;

public:
	// 開始＆終了
    static bool OnCreate( void );
    static void OnDestroy( void );

    // 更新
    static void Update( void );

    //--------------------------------------
    // 汎用キャッシュ
    //--------------------------------------
    static int GetTexCacheNumMax( void );
    static int GetTexCacheNumUse( void );

    // キャッシュの無効化
    static void InvalidateTex( CTex* pTexDot );

    // テクスチャ取得（※キャッシュにあればキャッシュから、なければ作成）
    // [isTemporary]を指定するとキャッシュしない＝キャッシュに意味のない不安定な画像用
    static CTex* GetTex( eDRAW_SHADER shader, CTex* pTexDot, CTex* pTexPal,  bool isTemporary=false );

private:
	// インスタンス作成は不可
	CTexCacheMgr( void ){}
	virtual ~CTexCacheMgr( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TEX_CACHE_MGR_HPP__
