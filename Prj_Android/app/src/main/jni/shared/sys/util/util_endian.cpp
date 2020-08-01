/*+----------------------------------------------------------------+
  |	Title:		util_endian.cpp　[共通環境]
  |	Comment:	エンディアン関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// エンディアンチェック用データ
#define ENDIAN_CHECK_HINT	                (0xFEFF)

// リトルエンディアンチェック用データ
#define ENDIAN_CHECK_HINT_B0_FOR_LE     (0xFF)
#define ENDIAN_CHECK_HINT_B1_FOR_LE     (0xFE)

// ビッグエンディアンチェック用データ
#define ENDIAN_CHECK_HINT_B0_FOR_BE     (0xFE)
#define ENDIAN_CHECK_HINT_B1_FOR_BE     (0xFF)

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//----------------------------------
// リトルエンディアンか？
//----------------------------------
bool util_endian_is_little( void ){
	WORD data = ENDIAN_CHECK_HINT;
	BYTE* pArrByte = (BYTE*) (&data);

	if( pArrByte[0] != ENDIAN_CHECK_HINT_B0_FOR_LE ){ return( false ); }
	if( pArrByte[1] != ENDIAN_CHECK_HINT_B1_FOR_LE ){ return( false ); }
	return( true );
}

//----------------------------------
// ビッグエンディアンか？
//----------------------------------
bool util_endian_is_big( void ){
	WORD data = ENDIAN_CHECK_HINT;
	BYTE* pArrByte = (BYTE*) (&data);

	if( pArrByte[0] != ENDIAN_CHECK_HINT_B0_FOR_BE ){ return( false ); }
	if( pArrByte[1] != ENDIAN_CHECK_HINT_B1_FOR_BE ){ return( false ); }
	return( true );
}

//----------------------------------------------
// エンディアン調整（※バイトの並びを入れ替える）
//----------------------------------------------
void util_endian_adjust( void* pAddr, int size ){
	BYTE* pArr = (BYTE*) pAddr;

	// 並び替え（※左右の一番目、左右の二番目、三番目...）
	int num = size/2;
	for( int i=0; i<num; i++ ){
		BYTE temp = pArr[i];
		pArr[i] = pArr[size-(i+1)];
		pArr[size-(i+1)] = temp;
	}
}
