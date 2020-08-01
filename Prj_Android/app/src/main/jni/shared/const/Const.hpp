/*+----------------------------------------------------------------+
  |	Title:		Const.hpp [共通環境]
  |	Comment:	定型処理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __CONST_HPP__
#define __CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 同期タイプ
//-----------------------------------------------------------------
enum eASYNC_TYPE{
	eASYNC_TYPE_INVALID = -1,

	eASYNC_TYPE_LINEAR,		// 線形同期（一定）
	eASYNC_TYPE_UP,			// 拡散同期（低速→じょじょに速く）
	eASYNC_TYPE_DOWN,		// 収束同期（高速→じょじょに遅く）
	eASYNC_TYPE_UP_DOWN,	// 拡散→収束（低速→じょじょに速く→高速→徐々に遅く）
	eASYNC_TYPE_DOWN_UP,	// 収束→拡散（高速→じょじょに遅く→低速→徐々に速く）

	eASYNC_TYPE_MAX
};

#define IS_ASYNC_TYPE_VALID( _t )   ((_t)>=(eASYNC_TYPE)0 && (_t)<eASYNC_TYPE_MAX)

//-----------------------------------------------------------------
// FPS（※フレームに関わる時間処理は浮動小数で行う）
//-----------------------------------------------------------------
#define FPS                         60.0f

//-----------------------------------------------------------------
// 定数：秒（※アクション等のゲームデータ指定用）
//-----------------------------------------------------------------
#define SEC_RATE                     1000       // 千分率の固定少数
#define SEC_RATE_MIN                    0       // 最低秒値（※[short]想定）
#define SEC_RATE_MAX                30000       // 最大秒値（※[short]想定）

//-----------------------------------------------------------------
// 定数：座標（※アクション等のゲームデータ指定用）
//-----------------------------------------------------------------
#define POSITION_MIN	            -30000	    // 最低座標値（※[short]想定）
#define POSITION_MAX	             30000	    // 最大座標値（※[short]想定）

//-----------------------------------------------------------------
// 定数：スケール（※アクション等のゲームデータ指定用）
//-----------------------------------------------------------------
#define SCALE_RATE		             1000	    // 千分率の固定少数
#define SCALE_RATE_MIN		            0	    // 最低スケール値（※[short]想定）
#define SCALE_RATE_MAX		        30000       // 最大スケール値（※[short]想定）

//-----------------------------------------------------------------
// 定数：ベジェ座標（※ベジェデータ指定用）
//-----------------------------------------------------------------
#define BEZIER_POSITION_RATE        10000       // 割合座標精度（※万分率）
#define BEZIER_POSITION_RATE_MIN   -30000       // 最低割合座標値（※[short]想定）
#define BEZIER_POSITION_RATE_MAX    30000       // 最大割合座標値（※[short]想定）

//-----------------------------------------------------------------
// 定数：ベジェスケール（※ベジェデータ指定用）
//-----------------------------------------------------------------
#define BEZIER_SCALE_RATE           10000       // 割合スケール精度（※万分率）
#define BEZIER_SCALE_RATE_MIN      -30000       // 最低割合スケール値（※[short]想定）
#define BEZIER_SCALE_RATE_MAX       30000       // 最大割合スケール値（※[short]想定）

//-----------------------------------------------------------------
// 定数：ベジェ回転（※ベジェデータ指定用）
//-----------------------------------------------------------------
#define BEZIER_ROTATION_RATE           10       // 割合回転精度（※十分率）
#define BEZIER_ROTATION_RATE_MIN    -3600       // 最低割合回転値（※[short]想定）
#define BEZIER_ROTATION_RATE_MAX     3600       // 最大割合回転値（※[short]想定）

//-------------------------------------------------------------------
// ユニークID（※[int]で管理できるように８桁の整数としてみる）
//（※列挙子のルールと同様に負で無効値、０以上で有効な値としておく）
//（※本当は[UID_***]としたいが、[android]環境でシステム側の定義とかぶるので断念）
//-------------------------------------------------------------------
#define UNIQUE_ID_INVALID   -1
#define UNIQUE_ID_MIN       0
#define UNIQUE_ID_MAX       99999999

#define IS_UNIQUE_ID_VALID( _uid )  ((_uid)>=UNIQUE_ID_MIN && (_uid)<=UNIQUE_ID_MAX)

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------
// 定数
//-----------------
class CConst{
public:
	//--------------------------------------
	// 各種変換
	//--------------------------------------
	// [秒←→フレーム]変換
    inline static float ConvFrameToSec( float frame ){ return( frame / FPS ); }
	inline static float ConvSecToFrame( float sec ){ return( FPS * sec ); }
	inline static float ConvSecRateToFrame( int secRate ){ return( FPS * secRate / SEC_RATE ); }

	// [浮動小数←→固定少数（※千分率）]変換
	inline static int   ConvScaleToRateScale( float scale ){ return( (int)(scale*SCALE_RATE) ); }
	inline static float ConvRateScaleToScale( int rateScale ){ return( ((float)rateScale)/SCALE_RATE ); }
    
    // 割合座標変換
    inline static float ConvBezierPositionRate( int baseSize, int posRate ){ return( ((float)(baseSize*posRate))/BEZIER_POSITION_RATE ); }
    inline static float ConvBezierRateScale( int rateScale ){ return( ((float)(rateScale))/BEZIER_SCALE_RATE ); }
    inline static float ConvBezierRotationRate( int rotRate ){ return( ((float)(rotRate))/BEZIER_ROTATION_RATE ); }

	//----------------------------------------------------------------
	// 同期処理（※[rate]は[0.0～1.0f]を想定）
    //----------------------------------------------------------------
    // 線形で計算した割合[rate]に対して、[type]で指定されるイージング処理を行う
    // この結果を[Calc***]系の[rate]へ渡す流れを想定
	//----------------------------------------------------------------
	static float ApplyAsyncType( eASYNC_TYPE type, float rateSrc, float pow=0.5f );	// 同期タイプの適用

	//------------------------------------------------------------
	// 各種計算（※[rate]は[0.0～1.0f]を想定）
	//------------------------------------------------------------
	static int    CalcAsyncI( float rate, int vFrom, int vTo );		                // 割合により２点間の値を求める：int
	static float  CalcAsyncF( float rate, float fFrom, float fTo );	                // 割合により２点間の値を求める：float
    static BYTE   CalcAsyncB( float rate, BYTE aFrom, BYTE aTo );                   // 割合により２点間の値を求める：BYTE
	static uint32 CalcAsyncRGBA( float rate, uint32 rgbaFrom, uint32 rgbaDest );    // 割合により２色間の値を求める
	static uint32 CalcAvgForRGBA( uint32 rgba0, uint32 rgba1 );		                // 色の平均
	static uint32 CalcMultiForRGBA( uint32 rgba, uint32 rgbaM );	                // 色の掛け算
    
    // カウンタによる色アニメ
    static uint32 CalcMoveForRGBA( uint32 rgbaFrom, uint32 rgbaDest, int count, int countMax );

	//----------------------------------------------
	// カウンタ（※目的値に到達したら[true]が返る）
	//----------------------------------------------
	static bool IncrementToMax( float *pVal, float max, float delta );
	static bool DecrementToZero( float *pVal, float delta );

	// 判定
	inline static bool IsIncrementableToMax( float val, float max ){ return( val < max ); }
	inline static bool IsDecrementableToZero( float val ){ return( val > 0.0f ); }

	//----------------------------------------------
	// フラグ
	//----------------------------------------------
	static bool CheckFlag( int flag, int bit );
	static void SetFlagOn( int* pFlag, int bit );
	static void SetFlagOff( int* pFlag, int bit );
	static void SwapFlag( int *pFlag, int bit );

	//-------------------------------------------------------------------------------
	// 文字列置換（※パラメータ付き）
	// [pStrSrc]の[0],[1],[2]...の文字を、[pArrParam]の要素で置き換える（※必要なら数を増やす）
	//-------------------------------------------------------------------------------
	static bool ReplaceString( char* buf, const char* pStrSrc, const char** pArrParam );
	static bool ReplaceString1( char* buf, const char* pStrSrc, const char* pParam0 );
	static bool ReplaceString2( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1 );
	static bool ReplaceString3( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1, const char* pParam2 );
	static bool ReplaceString4( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1, const char* pParam2, const char* pParam3 );
	static bool ReplaceString5( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1, const char* pParam2, const char* pParam3, const char* pParam4 );

	//-----------------------------------------------------------------
	// 一時ファイル名設定
	//-----------------------------------------------------------------
	static bool SetFileName( char* buf, const char* fileName, const char* defaultFileName, bool isTemporary );
    
private:
	// インスタンス生成は不可
	CConst( void ){}
	virtual ~CConst( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelAsyncType[];

#endif /* __CONST_HPP__ */
