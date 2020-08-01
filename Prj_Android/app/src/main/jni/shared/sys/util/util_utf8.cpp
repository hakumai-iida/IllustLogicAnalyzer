/*+----------------------------------------------------------------+
  |	Title:		util_utf8.cpp [共通環境]
  |	Comment:	utf8関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "util_utf8.hpp"

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
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//--------------------------------------
// utf8エンコードの[char]配列から文字数の取得
//--------------------------------------
int util_utf8_get_len( const char* pSrc0 ){
	// 無効は無視
	if( pSrc0 == NULL ){
		LOGE( "@ util_utf8_get_len: INVALID SRC\n" );
		return( -1 );
	}

	//----------------------
	// 変換後の文字列長確認
	//----------------------
    // 用心にBYTE(unsigned char)で取り扱う
    BYTE* pSrc = (BYTE*) pSrc0;

	int len = 0;
	while( *pSrc != '\0' ){
		// 中間文字[10XX XXXX]でなければ１文字と判定
		if( (*pSrc&0xC0) != 0x80 ){
			len++;
		}
		pSrc++;
	}

	return( len );
}

//--------------------------------------
// utf8エンコードの[char]配列を[int]配列へ変換
//--------------------------------------
bool util_utf8_conv_to_int_array( int* pArrI, int sizeArrI, const char* pSrc0, int* pLen ){
	// 無効は無視
	if( pSrc0 == NULL ){
		LOGE( "@ util_utf8_conv_to_int_array: INVALID SRC\n" );
		return( false );
	}

	// 無効は無視
	if( pArrI == NULL ){
		LOGE( "@ util_utf8_conv_to_int_array: INVALID BUF for %s\n", pSrc0 );
		return( false );
	}

	//----------------------
	// 変換後の文字列長確認
	//----------------------
	int len = util_utf8_get_len( pSrc0 );

	// バッファが不足していたら無視
	if( (len+1) > sizeArrI ){
		LOGE( "@ util_utf8_conv_to_int_array: BUF SHORTAGE: (%d > %d) for %s\n", len+1, sizeArrI, pSrc0 );
		return( false );
	}

	//----------------------
	// 変換
	//----------------------
    // 用心にBYTE(unsigned char)で取り扱う
    BYTE* pSrc = (BYTE*) pSrc0;

	int c = 0;
	len = 0;
	while( *pSrc != '\0' ){
		// 中間文字[10XX XXXX]であれば直前の値をシフトして結合
		if( (*pSrc&0xC0) == 0x80 ){
			c = (c<<8) | *pSrc++;
			continue;
		}

		// ここまできたら文字の区切り
		if( c != 0 ){
			pArrI[len++] = c;
			c = 0;
		}

		// 値の格納
		c = *pSrc++;
	}

	// 最後の１文字
	if( c != 0 ){
		pArrI[len++] = c;
	}

	// 終端文字の指定
	pArrI[len] = '\0';

	if( pLen != NULL ){
		*pLen = len;
	}

	return( true );
}

//--------------------------------------
// 上記関数で得た[utf8]の値を、文字コードに変換
//--------------------------------------
int util_utf8_decode( int data ){
	BYTE* pDat = (BYTE*) &data;

	int c = 0;

	// １バイト文字 : [0aaa bbb]
	if( (pDat[0]&0x80) == 0 ){
		c = pDat[0];
	}
	// ２バイト文字 : [110a aabb][10bb ccc]
	else if( (pDat[1]&0xC0) ==  0xC0 ){
		c = (pDat[1]&0x1F);
		c = (c<<6) | (pDat[0]&0x3F);
	}
	// ３バイト文字 : [1110 aaaa][10bb bbcc][10cc dddd]
	else if( (pDat[2]&0xE0) ==  0xE0 ){
		c = (pDat[2]&0x0F);
		c = (c<<6) | (pDat[1]&0x3F);
		c = (c<<6) | (pDat[0]&0x3F);
	}

	return( c );
}

//---------------------------------------------
// [Unicode]でエンコードされた[char]配列を[utf8]に変換
//---------------------------------------------
bool util_utf8_conv_from_unicode( char* pDest, int sizeDest, const char* pSrc0, int* pLen ){
    // 用心にBYTE(unsigned char)で取り扱う
    BYTE* pSrc = (BYTE*) pSrc0;

	bool isReverse = false;

	// UTF16-LE（※基本的にこれしかこないはず）
	if( pSrc[0] == 0xFF && pSrc[1] == 0xFE ){ isReverse = true; }
	// UTF16-BE
	else if( pSrc[0] == 0xFE && pSrc[1] == 0xFF ){ isReverse = false; }
	// それ以外は無視
	else{ return( false ); }

	// 文字列長検出（※２バイトで１文字）
	int len = 0;
	while( pSrc[len] != 0 || pSrc[len+1] != 0 ){
		len += 2;
	}

	// [utf8]に変換
	int cur = 0;
	BYTE aaaabbbb, ccccdddd;
	for( int i=2; i<len; i+=2 ){
		if( isReverse ){
			aaaabbbb = pSrc[i+1];
			ccccdddd = pSrc[i];
		}else{
			aaaabbbb = pSrc[i];
			ccccdddd = pSrc[i+1];
		}

		// 終端判定
		if( ccccdddd == 0 ){
			break;
		}

		// ３バイト文字 : [1110 aaaa][10bb bbcc][10cc dddd]
		if( aaaabbbb > 0 ){
			if( sizeDest <= (cur+3) ){
				LOGE( "@ util_utf8_conv_from_unicode: BUF SHORTAGE:%s\n", pSrc );
				return( false );
			}

			pDest[cur++] = 0xE0 | ((aaaabbbb>>4)&0x0F);
			pDest[cur++] = 0x80 | ((aaaabbbb<<2)&0x3C) | ((ccccdddd>>6)&0x03);
			pDest[cur++] = 0x80 | (ccccdddd&0x3F);
		}
		// １バイト文字（※とりあえず）
		else{
			if( sizeDest <= (cur+1) ){
				LOGE( "@ util_utf8_conv_from_unicode: BUF SHORTAGE:%s\n", pSrc );
				return( false );
			}

			pDest[cur++] = ccccdddd;
		}
	}

	// 終端文字の指定
	pDest[cur] = '\0';

	if( pLen != NULL ){
		*pLen = cur;
	}

	return( true );
}
