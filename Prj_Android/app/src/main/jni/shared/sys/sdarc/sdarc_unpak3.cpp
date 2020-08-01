/*+----------------------------------------------------------------+
  |	Title:		sdarc_unpak3.cpp [共通環境]
  |	Comment:	pak3解凍処理:スライド辞書[WORD単位]
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sdarc_def.hpp"
#include "sdarc_unpak3.hpp"

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
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
static int unpak3_1( WORD* out_buf, WORD* inp_buf, int inp_size );
static int unpak3_1_withEndianAdjust( WORD* out_buf, WORD* inp_buf, int inp_size );

/*+----------------------------------------------------------------+
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//------------------------------------------------------------------
// 解凍処理（※入力はリトルエンディアンで保存したデータを想定）
//------------------------------------------------------------------
int sdarc_unpak3( void* out_buf, void* inp_buf, int inp_size ){
	WORD* pSrc = (WORD*)inp_buf;
	WORD* pDest = (WORD*) out_buf;
	WORD type = 0;

	// ビッグエンディアン環境（※エンディアン調整）
	if( util_endian_is_big() ){
		// 先頭のWORDに圧縮タイプが指定されている
		type = *pSrc++;
		type = (WORD)(((type<<8)&0xFF00)|((type>>8)&0xFF));
		inp_size -= 2;

		// [PTN_1=1]のみ対応
		if( type == 1 ){
			return( unpak3_1_withEndianAdjust( pDest, pSrc, inp_size/2 ) );
		}
	}
	// リトルエンディアン環境（※そのまま処理）
	else{
		// 先頭のWORDに圧縮タイプが指定されている
		type = *pSrc++;
		inp_size -= 2;

		// [PTN_1=1]のみ対応
		if( type == 1 ){
		  	return( unpak3_1( pDest, pSrc, inp_size/2 ) );
		}
	}

	// ここまで来たらエラー
	LOGE( "@sdarc_unpak3: UN SUPPORTED: type=%d\n", type );
	return( -1 );
}

//------------------------------------------------------------------
// スライド辞書デコード：１７文字（※調整なし）
//------------------------------------------------------------------
static int unpak3_1( WORD* out_buf, WORD* inp_buf, int len ){
	int shift = 0;
	int out_cnt = 0;
	int dat = 0, target = 0, flag = 0;
	WORD *end_buf = &inp_buf[len];

	while( inp_buf < end_buf ){
		// シフト指定の取り出し
		if( shift == 0 ){
			flag = *inp_buf++;
			shift = 16;
		}

		// データ取り出し
		dat = *inp_buf++;

		// フラグシフト
		flag <<= 1;
		shift--;

		// スライド指定がなければそのまま
		if( (flag & 0x10000) == 0 ){
			out_buf[out_cnt++] = (WORD)dat;
			continue;
		}

		// 指定があれば既出のデータで上書き（※forループではなくswitch展開）
		target = out_cnt - PTN1_GET_TARGET(dat) + PTN1_GET_LENGTH(dat);
		out_cnt += PTN1_GET_LENGTH(dat) + PTN_LENGTH_OFS;
		switch( PTN1_GET_LENGTH( dat ) ){
            case 15:  out_buf[out_cnt-17] = out_buf[target-16];
            case 14:  out_buf[out_cnt-16] = out_buf[target-15];
            case 13:  out_buf[out_cnt-15] = out_buf[target-14];
            case 12:  out_buf[out_cnt-14] = out_buf[target-13];
            case 11:  out_buf[out_cnt-13] = out_buf[target-12];
            case 10:  out_buf[out_cnt-12] = out_buf[target-11];
            case  9:  out_buf[out_cnt-11] = out_buf[target-10];
            case  8:  out_buf[out_cnt-10] = out_buf[target- 9];
            case  7:  out_buf[out_cnt- 9] = out_buf[target- 8];
            case  6:  out_buf[out_cnt- 8] = out_buf[target- 7];
            case  5:  out_buf[out_cnt- 7] = out_buf[target- 6];
            case  4:  out_buf[out_cnt- 6] = out_buf[target- 5];
            case  3:  out_buf[out_cnt- 5] = out_buf[target- 4];
            case  2:  out_buf[out_cnt- 4] = out_buf[target- 3];
            case  1:  out_buf[out_cnt- 3] = out_buf[target- 2];
            case  0:  out_buf[out_cnt- 2] = out_buf[target- 1];
                out_buf[out_cnt- 1] = out_buf[target- 0];
                break;

            default:
                break;
		}
	}

	return( 2*out_cnt );
}

//------------------------------------------------------------------
// スライド辞書デコード：１７文字（※エンディアン調整）
//------------------------------------------------------------------
static int unpak3_1_withEndianAdjust( WORD* out_buf, WORD* inp_buf, int len ){
	int shift = 0;
	int out_cnt = 0;
	int dat = 0, target = 0, flag = 0;
	WORD *end_buf = &inp_buf[len];

	while( inp_buf < end_buf ){
		// シフト指定の取り出し＆エンディアン調整
		if( shift == 0 ){
			flag = *inp_buf++;
			flag = (WORD)(((flag<<8)&0xFF00)|((flag>>8)&0xFF));
			shift = 16;
		}

		// データ取り出し＆エンディアン調整
		dat = *inp_buf++;
		dat = (WORD)(((dat<<8)&0xFF00)|((dat>>8)&0xFF));

		// フラグシフト
		flag <<= 1;
		shift--;

		// スライド指定がなければそのまま
		if( (flag & 0x10000) == 0 ){
			out_buf[out_cnt++] = (WORD)dat;
			continue;
		}

		// 指定があれば既出のデータで上書き（※forループではなくswitch展開）
		target = out_cnt - PTN1_GET_TARGET(dat) + PTN1_GET_LENGTH(dat);
		out_cnt += PTN1_GET_LENGTH(dat) + PTN_LENGTH_OFS;
		switch( PTN1_GET_LENGTH( dat ) ){
            case 15:  out_buf[out_cnt-17] = out_buf[target-16];
            case 14:  out_buf[out_cnt-16] = out_buf[target-15];
            case 13:  out_buf[out_cnt-15] = out_buf[target-14];
            case 12:  out_buf[out_cnt-14] = out_buf[target-13];
            case 11:  out_buf[out_cnt-13] = out_buf[target-12];
            case 10:  out_buf[out_cnt-12] = out_buf[target-11];
            case  9:  out_buf[out_cnt-11] = out_buf[target-10];
            case  8:  out_buf[out_cnt-10] = out_buf[target- 9];
            case  7:  out_buf[out_cnt- 9] = out_buf[target- 8];
            case  6:  out_buf[out_cnt- 8] = out_buf[target- 7];
            case  5:  out_buf[out_cnt- 7] = out_buf[target- 6];
            case  4:  out_buf[out_cnt- 6] = out_buf[target- 5];
            case  3:  out_buf[out_cnt- 5] = out_buf[target- 4];
            case  2:  out_buf[out_cnt- 4] = out_buf[target- 3];
            case  1:  out_buf[out_cnt- 3] = out_buf[target- 2];
            case  0:  out_buf[out_cnt- 2] = out_buf[target- 1];
                out_buf[out_cnt- 1] = out_buf[target- 0];
                break;

            default:
                break;
		}
	}

	return( 2*out_cnt );
}
