/*+----------------------------------------------------------------+
  |	Title:		LoopMgr.hpp [共通環境]
  |	Comment:	ループ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOOP_MGR_HPP__
#define __LOOP_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LoopReserve.hpp"
#include "LoopTaskReserve.hpp"

#include "sys/data/List.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ファイル管理インターフェイス
//----------------------------------------
class CLoopMgr{
private:
	// 管理データ
	static CList* s_pListLoop;
	static CList* s_pListReserveLoop;
	static CList* s_pListReserveLoopTask;
	static CLoopTaskReserve* s_pActiveLoopTask;

public:
	// 初期化／終了
	static bool Init( void );
	static void Exit( void );

	// 更新
	static void Update( void );

	// 描画
	static void RegistForDraw( void );	// 描画登録
	static void DrawNode( void );		// UI描画
	static void DrawLog( void );	    // ログ描画

	// ループ確認
	static bool IsLoopActive( int id );

	// 中断／再開
	static void OnAppSuspend( void );
	static void OnAppResume( void );

	// ループ予約
	static void ReserveLoop( CLoop* pLoop, eLOOP_RESERVE reserveType, int loopId, int option );

	// ループタスク予約
	static CLoopTaskReserve* GetNewLoopTaskReserve( void );

private:
	// ループ予約関連
	static void CreateLoop( int loopId, int option, CLoop* pParent );
	static void ExecReserveLoop( CLoopReserve* pReserve );

	// ダイアログ予約関連
	static bool CheckLoopTaskStart( CLoopTaskReserve* pReserve );
	static bool CheckLoopTaskFinish( CLoopTaskReserve* pReserve );
    
	// インスタンス生成は不可
	CLoopMgr( void ){};
	virtual ~CLoopMgr( void ){};
    
    // アプリ依存ループ作成（※[config_for_app.cpp]で実装される）
    static CLoop* CreateLoopForApp( int loopId );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOOP_MGR_HPP__
