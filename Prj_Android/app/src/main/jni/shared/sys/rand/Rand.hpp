/*+----------------------------------------------------------------+
  |	Title:		Rand.hpp [共通環境]
  |	Comment:	乱数関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __RAND_HPP__
#define __RAND_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 中断復帰用にランダムの枠を設ける
//-----------------------------------------------------------------
enum eRAND_ID{
	// 共通：生成順に意味のないもの
	eRAND_ID_COMMON,

	// 個別：ゲームの流れに関わりのあるもの
	eRAND_ID_LOGIC_ANALYZE_SEARCH_ARRAY_SET,    // 検索配列設定用

	eRAND_ID_MAX,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
class CRand{
protected:
	// ラベル
	static const char* s_pArrLabelForRandId[];

	// 設定値を保持しておく（※復旧用）
	static int s_nArrSeed[eRAND_ID_MAX];
	static int s_nArrUse[eRAND_ID_MAX];

public:
	// 取得
	static const char** GetRandIdLabelArray( void ){ return( s_pArrLabelForRandId ); }

	// 初期化／終了
	static bool Init( void );
    static void Exit( void );

	// リセット
	static void Reset( void );

	// シードの設定
	static void SetSeedAt( eRAND_ID id, uint32 seed, int use );

	// シード／消費数の取得
	static int GetSeedAt( eRAND_ID id ){ return( s_nArrSeed[id] ); }
	static int GetUseAt( eRAND_ID id ){ return( s_nArrUse[id] ); }

	// 整数取得
	static int GetRandAt( eRAND_ID id, int maxI );
	static inline int GetRand( void ){ return( GetRandAt( eRAND_ID_COMMON, 0x7FFFFFFF ) ); }
	static inline int GetRand( int maxI ){ return( GetRandAt( eRAND_ID_COMMON, maxI ) ); }

	// 浮動小数取得
	static float GetRandAtF( eRAND_ID id );
	static inline float GetRandAtF( eRAND_ID id, float maxF ){ return( maxF*GetRandAtF(id) ); }
	static inline float GetRandF( void ){ return( GetRandAtF( eRAND_ID_COMMON ) ); }
	static inline float GetRandF( float maxF ){ return( GetRandAtF( eRAND_ID_COMMON, maxF ) ); }

	// 判定：整数
	static bool IsRandHappenAt( eRAND_ID id, int numerator, int denominator );
	static inline bool IsRandHappen( int numerator, int denominator ){ return( IsRandHappenAt( eRAND_ID_COMMON, numerator, denominator ) ); }

	// 判定：浮動小数
	static bool IsRandHappenAtF( eRAND_ID id, float rate );
	static inline bool IsRandHappenF( float rate ){ return( IsRandHappenAtF( eRAND_ID_COMMON, rate ) ); }

private:
	// インスタンス生成は不可
	CRand( void ){}
	virtual ~CRand( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __RAND_HPP__ */
