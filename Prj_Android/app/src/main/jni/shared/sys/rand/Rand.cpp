/*+----------------------------------------------------------------+
  |	Title:		Rand.cpp [共通環境]
  |	Comment:	乱数関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Rand.hpp"

#include "sys/util/util_rand.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
const char* CRand::s_pArrLabelForRandId[] = {
	"COMMON",
    
	"LOGIC_ANALYZE_SEARCH_FEELING",

	NULL,
};

// 設定値を保持しておく（※復旧用）
int CRand::s_nArrSeed[eRAND_ID_MAX];
int CRand::s_nArrUse[eRAND_ID_MAX];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------
// 初期化
//--------------------
bool CRand::Init( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CRand::Init\n" );
#endif

	// 初期化
	util_rand_init();

	// コモンの初期化
	int seed;
	seed = (int)util_sys_time_sec();

#ifdef MGR_INIT_LOG
	//LOGD( "@ RAND_ID[0]: seed=%d\n", seed );
#endif
	SetSeedAt( eRAND_ID_COMMON, seed, 0 );

	// 個別の初期化
	for( int i=0; i<eRAND_ID_MAX; i++ ){
		if( i != eRAND_ID_COMMON ){
			seed = GetRandAt( eRAND_ID_COMMON, 0x7FFFFFFF );
#ifdef MGR_INIT_LOG
			//LOGD( "@ RAND_ID[%d]: seed=%d\n", i, seed );
#endif
			SetSeedAt( (eRAND_ID)i, seed, 0 );
		}
	}
    
    return( true );
}

//--------------------
// 終了
//--------------------
void CRand::Exit( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CRand::Exit\n" );
#endif
}

//---------------------
// リセット
//---------------------
void CRand::Reset( void ){
	for( int i=0; i<eRAND_ID_MAX; i++ ){
		int seed = GetRand();
		SetSeedAt( (eRAND_ID)i, seed, 0 );
	}
}


//--------------------
// 乱数初期化
//--------------------
void CRand::SetSeedAt( eRAND_ID id, uint32 seed, int use ){
#ifdef MGR_INIT_LOG
	//LOGD( "@ CRand::SetSeedAt( id=%d, seed=%d, use=%d )\n", id, seed, use );
#endif

	// シードの設定
	util_rand_set_seed( id, seed );
	s_nArrSeed[id] = seed;

	// 指定があれば消費させる
	for( int i=0; i<use; i++ ){
		util_rand_get_next_double( id );
	}
	s_nArrUse[id] = use;
}

//--------------------
// 乱数取得
//--------------------
// 整数：最大値指定
int32 CRand::GetRandAt( eRAND_ID id, int32 maxI ){
	if( maxI <= 0 ){ return( 0 ); }	// 用心

	if( maxI < 0x7FFFFFFF ){
		s_nArrUse[id]++;	// １つ消費
		return( util_rand_get_next_int(id) % maxI );
	}else{
		s_nArrUse[id]++;	// １つ消費
		return( util_rand_get_next_int(id) );
	}
}

// 浮動小数：最大値指定
float CRand::GetRandAtF( eRAND_ID id ){
	s_nArrUse[id]++;	// １つ消費
	return( (float)util_rand_get_next_double(id) );
}

// 真偽：指定の分子分母で発生するか？
bool CRand::IsRandHappenAt( eRAND_ID id, int numerator, int denominator ){
	if( numerator <= 0 ){ return( false ); }
	if( denominator <= 0 ){ return( true ); }
	if( numerator >= denominator ){ return( true ); }
	return( IsRandHappenAtF( id, ((float)numerator)/denominator ) );
}

// 真偽：指定率で発生するか？
bool CRand::IsRandHappenAtF( eRAND_ID id, float rate ){
	if( rate <= 0.0f ){ return( false ); }
	if( rate >= 1.0f ){ return( true ); }
	return( rate > GetRandAtF( id ) );
}
