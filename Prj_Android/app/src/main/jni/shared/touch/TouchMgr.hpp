/*+----------------------------------------------------------------+
  |	Title:		TouchMgr.hpp [共通環境]
  |	Comment:	タッチ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TOUCH_MGR_HPP__
#define __TOUCH_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchStatusCell.hpp"
#include "TouchInfoCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// [Android] [BACK]ボタン監視
#define CHECK_BACK_BUTTON

enum eTOUCH_CHECK{
    // 指で押されている（※押されている時間（フレーム数）は[CTouchStatusCell.getPressTimer]で取得）
	eTOUCH_CHECK_PRESSED,

    // 指が動いた（※移動量は[CTouchStatusCell.getMoveX/Y]で取得）
    eTOUCH_CHECK_MOVED,

    // 指が触れた
	eTOUCH_CHECK_TOUCHED_DOWN,

    // 指が離された
	eTOUCH_CHECK_TOUCHED_UP,

    // タップされた
	eTOUCH_CHECK_TAPPED,

    // フリックされた（※移動量は[CTouchStatusCell.getFlickX/Y]で取得）
	eTOUCH_CHECK_FLICKED,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// タッチ管理
//----------------------------------------
class CTouchMgr{
private:
	// タッチログ情報
	static int s_nNumInfo;
	static CTouchStatusCell* s_oArrStatusCell;
	static CTouchInfoCell* s_oArrInfoCell;

	// 挙動制御（※重複入力無視）
	static bool s_bIgnoreDuplicated;

	// バックボタン押下／押上
	static bool s_bBackButtonDown, s_bBackButtonDown0;
	static bool s_bBackButtonUp, s_bBackButtonUp0;

	// タッチ効果関連
	static bool s_bTouchEffect;
	static bool s_bTouchEffectSkip;
	static bool s_bTouchEffectSe;
	static bool s_bTouchEffectSeSkip;
	static int  s_nTouchEffectNum;
	static int* s_nArrTouchEffectPosX;
	static int* s_nArrTouchEffectPosY;

public:
	static bool OnCreate( void );
	static void OnDestroy( void );

	// 重複無視フラグの設定（※１回の更新で複数回のループを処理する場合、タッチ判定が重複しないためのフラグ）
	static void SetIgnoreDuplicated( bool flag ){ s_bIgnoreDuplicated = flag; }
	static bool IsIgnoreDuplicated( void ){ return( s_bIgnoreDuplicated ); }

	static void Update( void );
	static void UpdateForGame( void );

    //----------------------------------------
    // タッチエフェクト（※画面を押した際の反応）
    //----------------------------------------
	inline static void SetTouchEffect( bool flag ){ s_bTouchEffect = flag; }
	inline static void SetTouchEffectSkip( bool flag ){ s_bTouchEffectSkip = flag; }
	inline static void SetTouchEffectSe( bool flag ){ s_bTouchEffectSe = flag; }
	inline static void SetTouchEffectSeSkip( bool flag ){ s_bTouchEffectSeSkip = flag; }

    // ステータスの全クリア
    static void ClearStatusAll( void );

    // タッチイベントデータ登録
    static bool PushLog( stTOUCH_EVENT_DESCRIPTOR* pTED );

    //----------------------------------------------------------
	// 入力の検出
	//----------------------------------------------------------
    // どこでもいいので押されたか？
    static bool IsTouchedAnywhere( void );

    // 矩形判定
	static CTouchStatusCell* CheckRect( int check, float x, float y, float w, float h, int at=0 );

    // 円判定
	static CTouchStatusCell* CheckOval( int check, float x, float y, float w, float h, int at=0 );
    
    // ピンチ（※２点押されているか？）
    static bool IsPinched( float x, float y, float w, float h, float* pLen );   // [pLen]に距離が入る

    //------------------------------------------
    // IDによるセルの取得（※入力の継続検出用）
    //------------------------------------------
    static CTouchStatusCell* GetCellById( int id );

    //------------------------------------------
	// バックボタン判定（※Android）
    //------------------------------------------
    static bool CheckBackButtonDown( void );
	static bool CheckBackButtonUp( void );
	static bool CheckBackButton( void );

private:
	static void CheckLog( int id, int act, float x, float y );

	// インスタンス生成は不可
	CTouchMgr( void ){};
	virtual ~CTouchMgr( void ){};
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TOUCH_MGR_HPP__
