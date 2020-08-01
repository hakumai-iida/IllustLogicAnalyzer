/*+----------------------------------------------------------------+
  |	Title:		BgMgr.hpp [共通環境]
  |	Comment:	背景管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BG_MGR_HPP__
#define __BG_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "obj/Obj.hpp"
#include "BgGrid.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
enum eBG{
	eBG_INVALID = -1,	// 無効値

    // デバッグ用
    eBG_GRID,			// グリッド: [CBgGridB/CBgGridF]

	eBG_MAX,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 背景管理
//----------------------------------------
class CBgMgr{
private:
	// 管理データ
	static eBG   s_eNextBgForNormal;
	static eBG   s_eCurBgForNormal;
	static CObj* s_pObjBackForNormal;
	static CObj* s_pObjFrontForNormal;

	static eBG   s_eNextBgForExtra;
	static eBG   s_eCurBgForExtra;
	static CObj* s_pObjBackForExtra;
	static CObj* s_pObjFrontForExtra;

	// バッファ
	static eBG*   s_pNextBg;
	static eBG*   s_pCurBg;
	static CObj** s_ppCurBufBack;
	static CObj** s_ppCurBufFront;

	// 振動値
	static float s_fQuakeX;
	static float s_fQuakeY;

    // 近景描画に一律適用するアルファ（※この値を使うかは背景しだい）
    static BYTE s_nAlphaForFront;

public:
	// 開始＆終了
    static bool OnAppStart( void );
    static void OnAppEnd( void );

    // 処理対象の切り替え
    static void SwitchBufForNormal( void );
    static void SwitchBufForExtra( void );

    static bool IsCurBuffForExtra( void );

    static void SetBg( eBG bg ){ (*s_pNextBg) = bg; }
    static void SetNextBg( void );
    
    // 個別指定：グリッド背景のタイプ指定
    static void SetBgGridType( eBG_GRID_TYPE type );

    // 外部からの指定値
    static inline void ResetQuakeXY( void ){ s_fQuakeX = 0.0f; s_fQuakeY = 0.0f; }
    static inline void SetQuakeXY( float x, float y ){ s_fQuakeX = x; s_fQuakeY = y; }
    static inline float GetQuakeX( void ){ return( s_fQuakeX ); }
    static inline float GetQuakeY( void ){ return( s_fQuakeY ); }

    static inline void SetAlphaForFront( BYTE a ){ s_nAlphaForFront = a; }
    static inline BYTE GetAlphaForFront( void ){ return( s_nAlphaForFront ); }

	// 更新
    static void Update( void );

	// 描画登録
	static void RegistForDraw( void );

protected:
    static void FixForChange( void );

private:
	// インスタンス作成は不可
	CBgMgr( void ){}
	virtual ~CBgMgr( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BG_MGR_HPP__
