/*+----------------------------------------------------------------+
  |	Title:		ImgMgr.hpp [共通環境]
  |	Comment:	画像管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __IMG_MGR_HPP__
#define __IMG_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Img.hpp"

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
// 画像管理
//----------------------------------------
class CImgMgr{
private:
    static CImg* s_oArrImg;

public:
    static bool OnCreate( void );
    static void OnDestroy( void );

    // 画像取得
	static CImg* GetImg( eIMG imgId );
    static CImg* GetImgWithPixelRate( eIMG imgId, int rate=0 );

	// テクスチャ取得
	static CTex* GetImgDot( eIMG imgId, int at=0 );
	static CTex* GetImgPal( eIMG imgId, int at=0 );

private:
	static bool CreateAll( void );

	// インスタンスの作成は不可
	CImgMgr( void ){}
	virtual ~CImgMgr( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __IMG_MGR_HPP__
