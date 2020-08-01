/*+----------------------------------------------------------------+
  |	Title:		LoopTaskReserve.hpp [共通環境]
  | Comment:    ループタスク予約：ヘルパー系の処理をタスクとして取り扱う
  |             （※マネージャで管理するほどではないシンプルな制御系の管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOOP_TASK_RESERVE_HPP__
#define __LOOP_TASK_RESERVE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Loop.hpp"

#include "sys/data/ListNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ループタスク予約タイプ
enum eLOOP_TASK_RESERVE{
	eLOOP_TASK_RESERVE_INVALID = -1,

    // CDialogHelper
	eLOOP_TASK_RESERVE_DIALOG,				// ダイアログの呼び出し
	eLOOP_TASK_RESERVE_DIALOG_INPUT,		// 入力ダイアログの呼び出し
    
	eLOOP_TASK_RESERVE_MAX,
};

// 文言長：パラメータはタスク側のバッファにコピーして管理する
#define LOOP_TASK_PARAM_MAX         8
#define LOOP_TASK_PARAM_BUF_LEN		4096

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ループタスク予約要素
//----------------------------------------
class CLoopTaskReserve: public CListNode{
    // 実装
    IMPLEMENT_NODE( CLoopTaskReserve )

protected:
	eLOOP_TASK_RESERVE m_eTask;		// 予約タスク
	CLoop* m_pLoop;					// 予約を発行した呼び出し元
	int m_nTaskId;					// タスクID(※呼び出し元側での識別用)
	
    // パラメータ
    int m_nNumParam;
    int m_nBufUse;
    char m_bufParam[LOOP_TASK_PARAM_BUF_LEN];
    const char* m_arrParam[LOOP_TASK_PARAM_MAX];

public:
    // コンストラクタ／デストラクタ
	CLoopTaskReserve( void );
	virtual ~CLoopTaskReserve( void ){}

    // クリア
	void clear( void );

    // 設定：ダイアログ
	bool setDialog( CLoop* pLoop, int taskId, const char* pTitle, const char* pText, const char* pPositive, const char* pNegative );

    // 設定：入力ダイアログ
	bool setDialogInput( CLoop* pLoop, int taskId, const char* pTitle, const char* pText, const char* pPositive, const char* pNegative, const char* pDefault );

	// 有効性の判定
	bool isValid( void );

	// 取得
	inline eLOOP_TASK_RESERVE getTask( void ){ return( m_eTask ); }
	inline CLoop* getLoop( void ){ return( m_pLoop); }
	inline int getTaskId( void ){ return( m_nTaskId ); }
    const char* getParamAt( int at );
    
protected:
    bool addParam( const char* pParam );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOOP_TASK_RESERVE_HPP__
