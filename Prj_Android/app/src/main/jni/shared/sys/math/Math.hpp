/*+----------------------------------------------------------------+
  |	Title:		Math.hpp [共通環境]
  |	Comment:	算数関連
  |             角度の処理は[度＝360で一周]＆[１２時を０度]として行なっている
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __MATH_HPP__
#define __MATH_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
class CMath{
public:
	// サイン／コサインの取得（※実処理）
	static float GetSin( float deg );
	static float GetCos( float deg );
    
    // サイン／コサインの取得（※簡易版の低精度）
    //（※引数は[0.0f]以上[360.0f]未満を想定）
    static float GetSinLite( float deg );
    static float GetCosLite( float deg );

	// 座標の回転Ｘ／Ｙ（※角度だけを指定した場合は上を０度として計算）
	static float RotX( float x0, float y0, float deg );
	static float RotY( float x0, float y0, float deg );
    inline static float RotX( float deg ){ return( RotX( 0.0f, 1.0f, deg ) ); }
    inline static float RotY( float deg ){ return( RotY( 0.0f, 1.0f, deg ) ); }

	// 角度の取得（※上が０度となる）
	static float GetDeg( float x, float y );
	static float GetDeg( float x0, float y0, float x1, float y1 );
    
    // 角度の反転
    static float FlipRotH( float rot );
    static float FlipRotV( float rot );

	// 距離と長さの取得
	static float Dist( float x0, float y0, float x1, float y1 );
	static float Len( float x, float y );

	// 切り上げ
	static int RoundUp( float vF );

	// [文字列 → 数字]変換
	static bool StrToLong( const char* pStr, int64* pVal );
    static bool StrToUnsignedLong( const char* pStr, uint64* pVal );
    static bool StrToDouble( const char* pStr, double* pVal );

private:
	// インスタンス生成は不可
	CMath( void ){}
	virtual ~CMath( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __MATH_HPP__ */
