/*+----------------------------------------------------------------+
  |	Title:		Drawer.hpp [共通環境]
  |	Comment:	ゲーム要素表示基底
  |				（※このクラスは描画順の管理用なので位置情報はもたない）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DRAWER_HPP__
#define __DRAWER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------------------------
// 描画順は下記の値により管理される（※値が大きいほど先(奥)に描画される）
// [eDRAW_PROCESS], [eDP_LAYER] はアプリに依存するので [config/config_for_app.hpp] で定義
//-----------------------------------------------------------------------------------
// ・描画プロセス：描画処理の区分（※フレームバッファを変える場合等）
// ・描画レイヤー：[DRAW_LAYER_STEP]の値を乗算する
// ・描画オフセット：[DRAW_PRIORITY_OFS_STEP]の値を乗算する
// ・描画調整値：[DRAW_PRIORITY_ADJUST_MIN/MAX]間の値を指定
//-----------------------------------------------------------------------------------
#define	DRAW_LAYER_STEP	            100000000
#define DRAW_PRIORITY_OFS_STEP		1000000
#define DRAW_PRIORITY_ADJUST_MIN	(-DRAW_PRIORITY_OFS_STEP/2)
#define DRAW_PRIORITY_ADJUST_MAX	(DRAW_PRIORITY_OFS_STEP/2)

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// CDrawer
//-----------------------------------------------------------------
class CDrawer{
	//-------------------------
	// [static] クラス管理用
	//-------------------------
protected:
	static eDRAW_PROCESS s_eDrawProcess;        // 処理中のプロセス
	static CDrawer *s_pDrawHead;                // 描画登録リストのヘッド

public:
    // 描画プロセスの指定
	static void ReadyForRegist( eDRAW_PROCESS process );

    // 表示物の描画（※表示物は[registForDraw]により自身をリストへ追加する）
	static void DrawObjs( void );

protected:
    // 描画プロセス確認
    static bool CheckCurDrawProcess( eDRAW_PROCESS process );

    // 表示物の登録
	static void RegistObj( CDrawer* pNew );

	//-------------------------
	// [member] メンバー
	//-------------------------
protected:
    eDRAW_PROCESS m_eDrawProcess;   // 表示プロセス
    eDP_LAYER m_eDrawLayer;	        // 表示レイヤー（※表示プロセス内での表示順）
    int m_nDrawPriorityOfs;			// 表示優先順位オフセット（※レイヤー内での表示順）
    int m_nDrawPriorityAdjust;		// 表示優先順位調整値（※オフセット内での表示順）
    int m_nCurDrawPriority;         // 現在の表示優先順（※レイヤー／オフセット／調整値から算出される）
    CDrawer *m_pDrawNext;           // 次に表示する要素（※要素は単方向リストで管理される）

public:
    // コンストラクタ＆デストラクタ
    CDrawer( void );
    virtual ~CDrawer( void ){}
    
    // クリア（※初期化用）
    void clearDrawer( void );

    // 設定
	inline void setDrawProcess( eDRAW_PROCESS process ){ m_eDrawProcess = process; }
	inline void setDrawLayer( eDP_LAYER layer ){ m_eDrawLayer = layer; }
	inline void setDrawPriorityOfs( int ofs ){ m_nDrawPriorityOfs = ofs; }
	inline void setDrawPriorityAdjust( int adjust ){ m_nDrawPriorityAdjust = adjust; }

	// 取得
	inline eDRAW_PROCESS getDrawProcess( void ){ return( m_eDrawProcess ); }
	inline eDP_LAYER getDrawLayer( void ){ return( m_eDrawLayer ); }
	inline int getDrawPriorityOfs( void ){ return( m_nDrawPriorityOfs ); }
	inline int getDrawPriorityAdjust( void ){ return( m_nDrawPriorityAdjust ); }

    // 判定
	inline bool checkDrawProcess( eDRAW_PROCESS process ){ return( m_eDrawProcess == process ); }

	// 描画登録（※自身を管理リストへ登録する）
	virtual void registForDraw( void );

protected:
	inline void setDrawNext( CDrawer* pObj ){ m_pDrawNext = pObj; }
	inline CDrawer* getDrawNext( void ){ return( m_pDrawNext ); }

	// 登録後に管理から呼ばれる
	virtual void onDraw( void );

	// 実描画（※派生先で上書きする）
	virtual void onDraw0( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __DRAWER_HPP__
