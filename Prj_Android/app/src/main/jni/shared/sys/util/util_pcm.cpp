/*+----------------------------------------------------------------+
  |	Title:		util_pcm.cpp [共通環境]
  |	Comment:	PCM関連(※[pcmconv.exe]の実装に依存)
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "util_pcm.hpp"

#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// [16bit モノラル]用 PCMデコード
static int _decode_16bit_mono_LE( short* pDest, BYTE* pBuff, int len );
static int _decode_16bit_mono_BE( short* pDest, BYTE* pBuff, int len );

// [16bit モノラル]用 ADPCMデコード
static int _decode_adpcm_16bit_mono_LE( short* pDest, BYTE* pBuff, int len, int outOfs, int outSkip );
static int _decode_adpcm_16bit_mono_BE( short* pDest, BYTE* pBuff, int len, int outOfs, int outSkip );

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// インデックステーブル
static const int _adpcm_index_table[] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

// データテーブル
static const int _adpcm_step_size_table[] = {
	7, 8, 9, 10, 11, 12, 13, 14,
	16, 17, 19, 21, 23, 25, 28, 31,
	34, 37, 41, 45, 50, 55, 60, 66,
	73, 80, 88, 97, 107, 118, 130, 143,
	157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658,
	724, 796, 876, 963, 1060, 1166, 1282, 1411,
	1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
	3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
	7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
	32767
};

/*+----------------------------------------------------------------+
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//----------------------------------------------------------------
// PCM 読み込み:16bit モノラル（※入力はリトルエンディアンデータを想定）
//----------------------------------------------------------------
int util_pcm_decode_16bit_mono( short* pDest, BYTE* pBuff, int len ){
	if( util_endian_is_little() ){
		return( _decode_16bit_mono_LE( pDest, pBuff, len ) );
	}else{
		return( _decode_16bit_mono_BE( pDest, pBuff, len ) );
	}
}

//----------------------------------------------------------------
// PCM 読み込み:16bit モノラル：リトルエンディアン
//----------------------------------------------------------------
static int _decode_16bit_mono_LE( short* pDest, BYTE* pBuff, int len ){
	// そのままコピー
	memcpy( pDest, pBuff, len );
	return( len );
}

//----------------------------------------------------------------
// PCM 読み込み:16bit モノラル：ビッグエンディアン
//----------------------------------------------------------------
static int _decode_16bit_mono_BE( short* pDest, BYTE* pBuff, int len ){
	// ショートに変換して格納
	int size = len/2;
	for( int i=0; i<size; i++ ){
		pDest[i] = (short)(pBuff[2*i+1] | (pBuff[2*i+0]<<8));
	}
	return( len );
}

//----------------------------------------------------------------------------------
// ADPCM 読み込み:16bit モノラル（※入力はリトルエンディアンデータを想定）
//----------------------------------------------------------------------------------
int util_pcm_decode_adpcm_16bit_mono( short* pDest, BYTE* pBuff, int len ){
	if( util_endian_is_little() ){
		return( _decode_adpcm_16bit_mono_LE( pDest, pBuff, len, 0, 0 ) );
	}else{
		return( _decode_adpcm_16bit_mono_BE( pDest, pBuff, len, 0, 0 ) );
	}
}

//----------------------------------------------------------------------------------
// ADPCM 読み込み:16bit ステレオＬ（※入力はリトルエンディアンデータを想定）
//----------------------------------------------------------------------------------
int util_pcm_decode_adpcm_16bit_stereoL( short* pDest, BYTE* pBuff, int len ){
	if( util_endian_is_little() ){
		return( _decode_adpcm_16bit_mono_LE( pDest, pBuff, len, 0, 1 ) );
	}else{
		return( _decode_adpcm_16bit_mono_BE( pDest, pBuff, len, 0, 1 ) );
	}
}

//----------------------------------------------------------------------------------
// ADPCM 読み込み:16bit ステレオＲ（※入力はリトルエンディアンデータを想定）
//----------------------------------------------------------------------------------
int util_pcm_decode_adpcm_16bit_stereoR( short* pDest, BYTE* pBuff, int len ){
	if( util_endian_is_little() ){
		return( _decode_adpcm_16bit_mono_LE( pDest, pBuff, len, 1, 1 ) );
	}else{
		return( _decode_adpcm_16bit_mono_BE( pDest, pBuff, len, 1, 1 ) );
	}
}

//----------------------------------------------------------------
// ADPCM 読み込み:16bit モノラル：リトルエンディアン
//----------------------------------------------------------------
static int _decode_adpcm_16bit_mono_LE( short* pDest, BYTE* pBuff, int len, int outOfs, int outSkip ){
	//------------------------
	// ヘッダ取得
	//------------------------
	int p = 0;

	// ブロックサイズ読飛ばし:[2byte]
	p += 2;

	// １ブロック中のサンプル数読み込み:[2byte]
	int samples_per_block = *((short*)&pBuff[p]);
	p += 2;

	// ブロック数読み込み:[4byte]
	int number_of_block = *((int*)&pBuff[p]);
	p += 4;

	//------------------------
	// 解凍
	//------------------------
	int c, s, dp, step_size;
	int o=outOfs, data=0, sp=0, index=0;
	for( int block=0; block<number_of_block; block++ ){
		for( int n=0; n<samples_per_block; n++ ){
			if( n == 0 ){
				// 初期値読み込み:[2byte]
				s = sp = *((short*)&pBuff[p]);
				p += 2;

				// インデックス読み込み:[1byte]
				index = pBuff[p];
				p += 1;

				// パディング読み捨て:[1byte]
				p += 1;
			}else{
				if( (n % 2) == 1){
					// １バイト読み込み:[1byte]
					data = pBuff[p];
					p += 1;

					// dataの下位4bit
					c = data & 0x0F;
				}else{
					// dataの上位4bit
					c = (data>>4) & 0x0F;
				}

				step_size = _adpcm_step_size_table[index];

				// 伸張
				dp = step_size >> 3;
				if( (c & 0x1) == 0x1 ){ dp += (step_size >> 2 ); }
				if( (c & 0x2) == 0x2 ){ dp += (step_size >> 1 ); }

				if( (c & 0x4) == 0x4 ){ dp += step_size; }

				if( (c & 0x8) == 0x8 ){ sp -= dp; }
				else{ sp += dp; }

				// リミッタ
				if( sp > 32767 ){ sp = 32767; }
				else if( sp < -32768 ){ sp = -32768; }

				index += _adpcm_index_table[c];
				if( index < 0 ){ index = 0; }
				else if( index > 88 ){ index = 88; }

				s = sp;
			}

			// 出力
			pDest[o++] = (short) s;
			o += outSkip;
		}
	}

	// データ長を返す
	o -= outOfs;
	return( 2*o );
}

//----------------------------------------------------------------
// ADPCM 読み込み:16bit モノラル：ビッグエンディアン
//----------------------------------------------------------------
static int _decode_adpcm_16bit_mono_BE( short* pDest, BYTE* pBuff, int len, int outOfs, int outSkip ){

	//------------------------
	// ヘッダ取得
	//------------------------
	int p = 0;

	// ブロックサイズ読み飛ばし:[2byte]
	p += 2;

	// １ブロック中のサンプル数読み込み:[2byte]
	int samples_per_block = pBuff[p+1] | (pBuff[p+0]<<8);
	p += 2;

	// ブロック数読み込み:[4byte]
	int number_of_block = pBuff[p+3] | (pBuff[p+2]<<8) | (pBuff[p+1]<<16) | (pBuff[p+0]<<24);
	p += 4;

	//------------------------
	// 解凍
	//------------------------
	int c, s, dp, step_size;
	int o=outOfs, data=0, sp=0, index=0;
	for( int block=0; block<number_of_block; block++ ){
		for( int n=0; n<samples_per_block; n++ ){
			if( n == 0 ){
				// 初期値読み込み:[2byte]
				s = sp = (short)(pBuff[p+1] | (pBuff[p+0]<<8));
				p += 2;

				// インデックス読み込み:[1byte]
				index = pBuff[p];
				p += 1;

				// パディング読み捨て:[1byte]
				p += 1;
			}else{
				if( (n % 2) == 1){
					// １バイト読み込み:[1byte]
					data = pBuff[p];
					p += 1;

					// dataの下位4bit
					c = data & 0x0F;
				}else{
					// dataの上位4bit
					c = (data>>4) & 0x0F;
				}

				step_size = _adpcm_step_size_table[index];

				// 伸張
				dp = step_size >> 3;
				if( (c & 0x1) == 0x1 ){ dp += (step_size >> 2 ); }
				if( (c & 0x2) == 0x2 ){ dp += (step_size >> 1 ); }

				if( (c & 0x4) == 0x4 ){ dp += step_size; }

				if( (c & 0x8) == 0x8 ){ sp -= dp; }
				else{ sp += dp; }

				// リミッタ
				if( sp > 32767 ){ sp = 32767; }
				else if( sp < -32768 ){ sp = -32768; }

				index += _adpcm_index_table[c];
				if( index < 0 ){ index = 0; }
				else if( index > 88 ){ index = 88; }

				s = sp;
			}

			// 出力
			pDest[o++] = (short) s;
			o += outSkip;
		}
	}

	// データ長を返す
	o -= outOfs;
	return( 2*o );
}
