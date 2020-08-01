/*+----------------------------------------------------------------+
  |	Title:		sdarc_pak3.cpp [共通環境]
  |	Comment:	pak3圧縮処理:スライド辞書[WORD単位]
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sdarc_def.hpp"
#include "sdarc_pak3.hpp"

#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// パターンバッファ
static DWORD *ptnBuf;
static int sizePtnBuf;
static int cntPtnBuf;

// 入力バッファ
static WORD* srcBuf;

// 出力バッファ
static WORD* outBuf;
static int cntOutBuf;

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// バッファリセット
static void reset_work( BYTE *in, BYTE *out );

// 圧縮（※パターンバッファ作成）
static void pak3_1( WORD *pBuf, int len );

// パターン取得
static int  ptn_get( WORD *p, int at, int len );

// 出力
static void output( DWORD *pBuf, int len );
static void outputWithEndianAdjust( DWORD *pBuf, int len );

//-----------------------------------------------
// 作業領域のリセット
//-----------------------------------------------
static void reset_work( BYTE *in, BYTE *out ){
	cntPtnBuf  = 0;
	cntOutBuf  = 0;
	srcBuf = (WORD *)in;
	outBuf = (WORD *)out;
	memset( ptnBuf, 0, (size_t)sizePtnBuf );
}

//-----------------------------------------------
// 圧縮（※パターンバッファ作成）
//-----------------------------------------------
static void pak3_1( WORD *pBuf, int len ){
	int d;
	int i = 0;

	while( i < len ){
		d = ptn_get( pBuf, i, len );

		if( d == 0 ){
			ptnBuf[cntPtnBuf++] = pBuf[i];
			i++;
		}else{
			ptnBuf[cntPtnBuf++] = (WORD)d;
			i +=PTN1_GET_LENGTH( d ) + PTN_LENGTH_OFS;
		}
	}
}

//-----------------------------------------------
// データの並びからパターンを検出
//-----------------------------------------------
static int ptn_get( WORD *p, int at, int len ){
	int at0, i, n;
	int target = -1;
	int length =  1;
	int num = PTN1_MAX_TARGET;
	if( num > at ){ num = at; }

	// 指定文字数内でのマッチング検出
	for( i=0; i<num; i++ ){
		at0 = at - ( i + 1 );

		for( n=0; n<(PTN1_MAX_LENGTH+PTN_LENGTH_OFS); n++ ) {
			if( p[at + n] != p[at0 + n] || (at+n) >= len ) { break; }
		}

		// マッチしている文字が多ければ差し替え
		if( n > length ) {
			target = i;
			length = n;
		}
	}

	// パターンにマッチしていたら
	if( length >= PTN_LENGTH_OFS ){
		int d = PTN1_PACK( length, target );
		if( d >= 0x10000 ){
			LOGE( "エラー：length=%d, target=%d\n", length, target );
			return( 0 );
		}
		return( d + 0x10000 );
	}
	return( 0 );
}

//--------------------------
// 出力（※未調整）
//--------------------------
static void output( DWORD *pBuf, int len ){
	int c, d;

	for( int i=0; i<len; i++ ){
		// パターン指定フラグの作成
		if( (i%16) == 0 ){
			d = 0;
			for( int j=0; j<16; j++ ){
				if( (i+j) < len ){ c = pBuf[i+j]; }
				else{ c = 0; }
				d = d << 1;
				d += ((c >> 16) & 1);
			}
			outBuf[cntOutBuf++] = (WORD)(d & 0xffff);
		}

		c = pBuf[i];
		outBuf[cntOutBuf++] = (WORD)(c & 0xffff);
	}
}

//---------------------------------------------------------
// 出力（※エンディアンを調整して出力）
//---------------------------------------------------------
static void outputWithEndianAdjust( DWORD *pBuf, int len ){
	int c, d, temp;

	for( int i=0; i<len; i++ ){
		// パターン指定フラグの作成
		if( (i%16) == 0 ){
			d = 0;
			for( int j=0; j<16; j++ ){
				if( (i+j) < len ){ c = pBuf[i+j]; }
				else{ c = 0; }
				d = d << 1;
				d += ((c >> 16) & 1);
			}

			// エンディアン調整をして出力
			temp = d & 0xffff;
			outBuf[cntOutBuf++] = (WORD)(((temp<<8)&0xFF00)|((temp>>8)&0xFF));
		}

		c = pBuf[i];

		// エンディアン調整をして出力
		temp = c & 0xffff;
		outBuf[cntOutBuf++] = (WORD)(((temp<<8)&0xFF00)|((temp>>8)&0xFF));
	}
}

//-----------------------------------------------
// 作業環境設定
//-----------------------------------------------
void sdarc_pak3_set_env( int size, void* pBuf ){
	sizePtnBuf = size;
	ptnBuf = (DWORD*) pBuf;
}

//---------------------------------------------------------------------
// pak3圧縮（※結果としてリトルエンディアンのデータが出力されるように）
//---------------------------------------------------------------------
int sdarc_pak3( BYTE *in, int len, BYTE *out ){
	reset_work( in, out );

	// パターンバッファに圧縮タイプの出力をしておく（※[PTN_1=1]固定）
	outBuf[cntOutBuf++] = 1;

	// 入力は[WORD]配列なので入力バイト長を調整
	pak3_1( srcBuf, (len+1)/2 );

	// ビッグエンディアン環境
	if( util_endian_is_big() ){
		outputWithEndianAdjust( ptnBuf, cntPtnBuf );
	}
	// リトルエンディアン環境
	else{
		output( ptnBuf, cntPtnBuf );
	}

	// 圧縮後のサイズをバイト長で返す
	return( 2*cntOutBuf );
}
