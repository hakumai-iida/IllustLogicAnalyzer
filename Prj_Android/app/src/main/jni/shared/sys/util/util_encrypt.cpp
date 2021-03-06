/*+----------------------------------------------------------------+
  |	Title:		util_encrypt.cpp　[共通環境]
  |	Comment:	暗号化（※生データを見せない程度のシンプルなもの）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "util_encrypt.hpp"

/*+----------------------------------------------------------------+
  |	Define		定義
  +----------------------------------------------------------------+*/
// 鍵
#define ENC_KEY_L   0x59
#define ENC_KEY_R   0xD3

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
// 暗号化
//----------------------------------
bool util_encrypt_encode( void* pBuf, uint32 len ){
    BYTE* pArr = (BYTE*)pBuf;
    uint32 num = len/2;

    // 左右を並び替えてXORをかけておく（※奇数の真ん中は無加工となるが気にしない）
	for( uint32 i=0; i<num; i++ ){
		BYTE temp = pArr[i];
		pArr[i] = (BYTE)(pArr[len-(i+1)] ^ ENC_KEY_L);
		pArr[len-(i+1)] = (BYTE)(temp ^ ENC_KEY_R);
	}

    return( true );
}

//----------------------------------
// 復号化
//----------------------------------
bool util_encrypt_decode( void* pBuf, uint32 len ){
    BYTE* pArr = (BYTE*)pBuf;
    uint32 num = len/2;
    
    // 並びを戻してXOR
	for( uint32 i=0; i<num; i++ ){
		BYTE temp = pArr[i];
		pArr[i] = (BYTE)(pArr[len-(i+1)] ^ ENC_KEY_R);
		pArr[len-(i+1)] = (BYTE)(temp ^ ENC_KEY_L);
	}
    
    return( true );
}
