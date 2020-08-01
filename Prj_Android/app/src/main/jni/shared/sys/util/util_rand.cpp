/*+----------------------------------------------------------------+
  |	Title:		util_rand.cpp　[共通環境]
  |	Comment:	乱数：メルセンヌツイスター
  |				http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/mt.html
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "util_rand.hpp"

/*+----------------------------------------------------------------+
  |	Define		定義
  +----------------------------------------------------------------+*/
/* Period parameters */
#define MT_N			624
#define MT_M			397
#define MT_MATRIX_A		0x9908b0dfUL	/* constant vector a */
#define MT_UPPER_MASK	0x80000000UL	/* most significant w-r bits */
#define MT_LOWER_MASK	0x7fffffffUL	/* least significant r bits */

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// ワーク
static unsigned long __mt[MAX_RAND_CHANNEL][MT_N];	// ベクトル配列
static int __imt[MAX_RAND_CHANNEL];					// インデックス
static unsigned long __mag01[2] = { 0x0UL, MT_MATRIX_A };

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
static void mt_init_genrand( int ch, unsigned long s );
static unsigned long mt_genrand_int32( int ch );
static double mt_genrand_res53( int ch );

/*+----------------------------------------------------------------+
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//----------------------------------
// 乱数環境の初期化
//----------------------------------
void util_rand_init( void ){
	for( int i=0; i<MAX_RAND_CHANNEL; i++ ){
		__imt[i] = MT_N + 1;		// 未初期化値の設定
	}
}

//----------------------------------
// 乱数の種を設定
//----------------------------------
void util_rand_set_seed( int ch, uint32 seed ){
	ch %= MAX_RAND_CHANNEL;			// 用心
	mt_init_genrand( ch, seed );
}

//--------------------------------------
// 乱数取得： [0] <= val <= [0x7FFFFFFF]
//--------------------------------------
int util_rand_get_next_int( int ch ){
	ch %= MAX_RAND_CHANNEL;			// 用心
	return( (int)(mt_genrand_int32( ch ) & 0x7FFFFFFF) );
}

//--------------------------------
// 乱数取得 : [0.0] <= val < [1.0]
//--------------------------------
double util_rand_get_next_double( int ch ){
	ch %= MAX_RAND_CHANNEL;			// 用心
	return( mt_genrand_res53( ch ) );
}

/****************/
/* 乱数の初期化 */
/****************/
static void mt_init_genrand( int ch, unsigned long s ){
    __mt[ch][0]= s & 0xffffffffUL;
    for( __imt[ch]=1; __imt[ch]<MT_N; __imt[ch]++ ){
        __mt[ch][__imt[ch]] = (1812433253UL * (__mt[ch][__imt[ch]-1] ^ (__mt[ch][__imt[ch]-1] >> 30)) + __imt[ch]);
        __mt[ch][__imt[ch]] &= 0xffffffffUL;	// 32bit調整
    }
}

/**********************************/
/* [0,0xffffffff]の範囲で乱数生成 */
/**********************************/
static unsigned long mt_genrand_int32( int ch ){
    unsigned long y;

    // 終端に達していたら補充
    if( __imt[ch] >= MT_N ){
        int kk;

        // 未初期化なら初期化
        if( __imt[ch] == MT_N+1 ){
            mt_init_genrand( ch, 5489UL );	// デフォルトシード
        }

        for( kk=0; kk<MT_N-MT_M; kk++ ){
            y = (__mt[ch][kk]&MT_UPPER_MASK) | (__mt[ch][kk+1]&MT_LOWER_MASK);
            __mt[ch][kk] = __mt[ch][kk+MT_M] ^ (y >> 1) ^ __mag01[y & 0x1UL];
        }

        for( ; kk<MT_N-1 ; kk++ ){
            y = (__mt[ch][kk]&MT_UPPER_MASK) | (__mt[ch][kk+1]&MT_LOWER_MASK);
            __mt[ch][kk] = __mt[ch][kk+(MT_M-MT_N)] ^ (y >> 1) ^ __mag01[y & 0x1UL];
        }

        y = (__mt[ch][MT_N-1]&MT_UPPER_MASK) | (__mt[ch][0]&MT_LOWER_MASK);
        __mt[ch][MT_N-1] = __mt[ch][MT_M-1] ^ (y >> 1) ^ __mag01[y & 0x1UL];

        __imt[ch] = 0;
    }

    // 枠から一つとりだす
    y = __mt[ch][__imt[ch]++];

    // 細工して返す
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return( y );
}

/****************************************/
/* [0,1)の範囲で乱数の生成（53bit精度） */
/*****************************************/
static double mt_genrand_res53( int ch ){
    unsigned long a = mt_genrand_int32( ch ) >> 5;	// 27bit
    unsigned long b = mt_genrand_int32( ch ) >> 6;	// 26bit
    return( (a*67108864.0+b)*(1.0/9007199254740992.0) );
}
