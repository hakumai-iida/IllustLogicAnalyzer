/*+----------------------------------------------------------------+
  |	Title:		TexMgr.hpp [共通環境]
  |	Comment:	テクスチャ管理（※ベジェで作成したテクスチャの管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TEX_MGR_HPP__
#define __TEX_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Tex.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#if 1
//--------------------------------
// とりあえず
//--------------------------------
enum eBANK{
    eBANK_INVALID = -1,
    
    eBANK_DUMMY,
    
    eBANK_MAX,
};

//--------------------------------
// とりあえず
//--------------------------------
enum eREGIST_TEX_BANK{
    eREGIST_TEX_BANK_INVALID = -1,
    
    eREGIST_TEX_BANK_DUMMY,
    
    eREGIST_TEX_BANK_MAX,
};

//--------------------------------
// とりあえず
//--------------------------------
enum eREGIST_TEX{
    eREGIST_TEX_INVALID = -1,
    
    eREGIST_TEX_DUMMY,
    
    eREGIST_TEX_MAX,
};

//--------------------------------
// とりあえず
//--------------------------------
enum eCHARA_COLOR{
    eCHARA_COLOR_INVALID = -1,
    
    eCHARA_COLOR_DUMMY,
    
    eCHARA_COLOR_MAX,
};
#endif

//---------------------------------------------------------------
// BMPタイプ
// 画像は[色／線／枠]の３つに分割される
//---------------------------------------------------------------
enum eBMP_TYPE{
    eBMP_TYPE_INVALID = -1,
    
	eBMP_TYPE_COLOR,		// 無印：色
	eBMP_TYPE_LINE,			// [_l]：線
	eBMP_TYPE_OUTLINE,		// [_ol]：枠

	eBMP_TYPE_MAX,
};

//---------------------------------------------------------------
// BMP名調整文字
//---------------------------------------------------------------
// ワイルドカード（※BMP中に指定することで置換される）
#define BMP_NAME_WILD_CARD			'@'

// 線BMPオプション
#define BMP_NAME_PART_LINE			"_l"
#define BMP_NAME_PART_LINE_LEN		2

// アウトラインBMPオプション
#define BMP_NAME_PART_OUTLINE		"_ol"
#define BMP_NAME_PART_OUTLINE_LEN	3

// ファイル末尾からみた拡張子分割点
#define BMP_NAME_EXTENSION_PIVOT	4	// [.bdd/.bdp]

// BMP画像の基本色
#define BMP_COLOR_BASE_RGBA			0xFFFFFFFF
#define BMP_LINE_BASE_RGBA			0x808080FF
#define BMP_OUTLINE_BASE_RGBA		0x000000FF

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// テクスチャ管理
//----------------------------------------
class CTexMgr{
private:
    // バンクテクスチャ：ACT内のBMP名で指定するデータ
    static CTex** s_oArrArrBankTex;
    static char*** s_pArrArrBankTexName;
    static char** s_pArrBankTexNameBuf;
    static int** s_nArrArrBankTexType;
    static int* s_nArrBankTexNum;

    // 登録型テクスチャ：[eREGIST_BANK_TEX_***]で指定する実行時に読み直される前提のデータ
    static CTex** s_oArrArrRegistTex;

    // キャラパレット
    static CTex** s_pArrCharaColorPalColor;
    static CTex** s_pArrCharaColorPalLine;

    // キャラフキダシ文字色
	static uint32* s_nArrCharaColorForWordTop;
	static uint32* s_nArrCharaColorForWordTopOL;
	static uint32* s_nArrCharaColorForWord;
	static uint32* s_nArrCharaColorForWordOL;

public:
    static bool OnCreate( void );
    static void OnDestroy( void );

	// バンクテクスチャ取得
	static CTex* GetBankTexWithType( eBANK bank, const char* pName, eBMP_TYPE type, eREGIST_TEX_BANK registBank );
	static CTex* GetBankTexAt( eBANK bank, int at );
	static int GetBankTexTypeAt( eBANK bank, int at );
	static const char** GetBankTexNameArray( eBANK bank );
	static const char* GetBankTexNameAt( eBANK bank, int at );

	// 登録テクスチャ取得
	static CTex* GetRegistTexWithType( eREGIST_TEX_BANK registBank, eREGIST_TEX registTexId, eBMP_TYPE type );

	// パレット設定／取得
	static void SetCharaColorPalColor( eCHARA_COLOR id, CTex* pTexPal );
	static void SetCharaColorPalLine( eCHARA_COLOR id, CTex* pTexPal );
	static CTex* GetCharaColorPalColor( eCHARA_COLOR id );
	static CTex* GetCharaColorPalLine( eCHARA_COLOR id );

    // フキダシ文字色設定／取得
    static void SetCharaColorForWordTop( eCHARA_COLOR id, uint32 rgba );
    static void SetCharaColorForWordTopOL( eCHARA_COLOR id, uint32 rgba );
    static void SetCharaColorForWord( eCHARA_COLOR id, uint32 rgba );
    static void SetCharaColorForWordOL( eCHARA_COLOR id, uint32 rgba );
    static uint32 GetCharaColorForWordTop( eCHARA_COLOR id );
    static uint32 GetCharaColorForWordTopOL( eCHARA_COLOR id );
    static uint32 GetCharaColorForWord( eCHARA_COLOR id );
    static uint32 GetCharaColorForWordOL( eCHARA_COLOR id );
    
    // ID変換
    static eCHARA_COLOR ConvCharaColorId( eBANK bank, eREGIST_TEX_BANK registBank, bool isBurning );

private:
	static bool CreateAllForBank( void );
	static bool ReadBankTexAt( eBANK bank, const char* pFileName );
	static bool CreateAllForRegist( void );
	static bool CreateAllForCharaColor( void );

	// インスタンスの作成は不可
	CTexMgr( void ){}
	virtual ~CTexMgr( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TEX_MGR_HPP__
