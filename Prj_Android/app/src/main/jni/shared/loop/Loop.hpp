/*+----------------------------------------------------------------+
  |	Title:		Loop.hpp [共通環境]
  |	Comment:	ループ基底
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOOP_HPP__
#define __LOOP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "node/Node.hpp"

#include "obj/Drawer.hpp"
#include "obj/Updater.hpp"

#include "sys/data/List.hpp"
#include "sys/data/ListNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// [eLOOP_ID] はアプリに依存するので [config/config_for_app.hpp] で定義

//-----------------------------------------------
// ステップ
// ループの処理は[m_nStep]により割り振られる設計
//-----------------------------------------------
enum eLOOP_STEP{
	eLOOP_STEP_FINISHED	= -4,	// 終了済み（※解放可能）
	eLOOP_STEP_SUSPEND	= -3,	// 中断中
	eLOOP_STEP_SLEEP	= -2,	// スリープ中
	eLOOP_STEP_INVALID	= -1,	// 無効値（※生成直後の値）
	eLOOP_STEP_START	=  0,	// ループ開始時（※０の値を初期ステップに想定）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ループ基底
//----------------------------------------
class CLoop: public CListNode, public CDrawer, public CUpdater{
    // 実装
    IMPLEMENT_NODE( CLoop )
    
protected:
	int m_nId;              // ループID
	int m_nStep;            // ステップ
	int m_nStep0;           // ステップ履歴（※変更検出用）
	int m_nStepAtAwake;     // 起床時のステップ（※スリープした際のステップ）
	int m_nStepAtResume;    // リジューム時のステップ（※サスペンドした際のステップ）

	// 起動データ
	CLoop* m_pParent;	    // 呼び出し元
	int m_nOption;		    // 呼び出し時のオプション

	// レイアウト座標
	float m_nLayoutCurX, m_nLayoutCurY;
	int m_nLayoutDirX, m_nLayoutDirY;
	int m_nLayoutAlignX,  m_nLayoutAlignY;

	// ノードリスト（※レイアウトに登録された要素管理用）
	CList m_oListNode;

public:
	//--------------------------------------------------------------------------------
	// 初期化～終了までの流れは以下のイメージ
	//--------------------------------------------------------------------------------
	// 1.生成予約を検出したCLoopMgrによりループのコンストラクタが呼ばれる
	// 　基本的にコンストラクタではループIDの設定をするのみ（※領域確保はしない）
	//
	// 2.コンストラクタ直後、CLoopMgrにより[init]がよばれる
	//  [init(init0)]では必要な領域の確保、各種設定をする
	//  基底では、ここでステップを[eLOOP_STEP_START]に設定する
	//
	// 3.ループ処理
	//  [update(update0)]～[draw(draw0)]のループ
	//
	// 4.ループの切り替え／終了時は、自身でCLoopMgrに通知
	//  CLoopMgrに終了予約[eLOOP_RESERVE_END]を発行すると、
	//  次回フレーム以降、CLoopMgrより[exit]がよばれる
	//  基底では、ここで[eLOOP_STEP_FINISHED]に設定される
	//  [exit(exit0)]では終了に際しての後始末を行う（※領域解放はしない）
	//  以降、CLoopMgrにより[isFinished]が検出されるとデストラクタがよばれる
	//  ループ内で確保した領域の解放はデストラクタで行う想定
	//--------------------------------------------------------------------------------
	CLoop( void );
	virtual ~CLoop( void );

	// 初期化／終了
	void onInit( CLoop* pParent, int option );
	void onExit( void );

	// 更新
	void onUpdate( void );

	// 表示
	void onRegistForDraw( void );
	void onDrawNode( void );
	void onDrawLog( void );

	//--------------------------------------------------------------------------------
	// 終了判定
	//--------------------------------------------------------------------------------
	// 終了ステップ[eLOOP_STEP_FINISHED]になると、
	// その後のあらゆる処理は無効化され、CLoopMgrから解放されるのを待つのみとなる
	//--------------------------------------------------------------------------------
	inline bool isFinished( void ){ return( m_nStep == eLOOP_STEP_FINISHED ); }

	//--------------------------------------------------------------------------------
	// スリープ関連
	//--------------------------------------------------------------------------------
	// 別ループの呼び出し[eLOOP_RESERVE_CALL]をすると、呼び出し元はスリープする
	// とは言え、スリープ状態[eLOOP_STEP_SLEEP]になっても、処理に制限はないため、
	// 呼び出し元で、更新や表示の制御をしたい場合、ループ個々で適宜処理をすることも可能
	// CLoopMgrによりスリープされると[onSleep]が、 解除される際には[onAwake]がよばれる
	//--------------------------------------------------------------------------------
	inline bool isSleep( void ){ return( m_nStep == eLOOP_STEP_SLEEP ); }

	// スリープ／アウェイク時に呼ばれる
	void onSleep( void );
	void onAwake( void );

	//-------------------------------------------------------------
	// 中断関連：基本的にスリープと同じ流れで処理される
	//-------------------------------------------------------------
	inline bool isSuspend( void ){ return( m_nStep == eLOOP_STEP_SUSPEND ); }

	// ゲームのサスペンド／ リジューム時に呼ばれる
	void onSuspend( void );
	void onResume( void );

	//-------------------------------------------------------------
	// タスク関連：開始時／終了時
	//-------------------------------------------------------------
	void onTaskStarted( int taskId );
	void onTaskFinished( int taskId, bool isSuccess, int code, const char* pResponse );

    //-------------------------------------------------------------
	// 取得
    //-------------------------------------------------------------
	inline int getId( void ){ return( m_nId ); }
	inline CLoop* getParent( void ){ return( m_pParent ); }
	inline int getOption( void ){ return( m_nOption ); }

protected:
    //-------------------------------------------------------------
    // 設定（※派生先のコンストラクタでIDを設定する）
    //-------------------------------------------------------------
    inline void setId( int id ){ m_nId = id; }

    //-------------------------------------------------------------
	// ステップ判定
    //-------------------------------------------------------------
	inline bool isStepChanged( void ){ return( m_nStep != m_nStep0 ); }
	inline void setStep( int step ){ m_nStep = step; }
    inline void setStepF( int step ){ m_nStep = step; m_nStep0 = eLOOP_STEP_INVALID; }
	inline void fixStep( void ){ m_nStep0 = m_nStep; }
    inline void resetStep( void ){ m_nStep0 = eLOOP_STEP_INVALID; }

    //-------------------------------------------------------------
	// 処理実体：派生先で実装すべきメソッド
    //-------------------------------------------------------------
	virtual void init0( void ) = 0;
	virtual void exit0( void ) = 0;

    //-------------------------------------------------------------
	// 処理実体：必要であれば上書きする
    //-------------------------------------------------------------
	virtual void registForDraw0( void ){}
	virtual void drawUI0( void ){}
	virtual void drawLog0( void ){}

	virtual void sleep0( void ){}
	virtual void awake0( void ){}

	virtual void suspend0( void ){}
	virtual void resume0( void ){}

	virtual void taskStarted0( int taskId ){}
	virtual void taskFinished0( int taskId, bool isSuccess, int code, const char* pResponse ){}

	//------------------------------------------------------------
	// ノード関連
	//------------------------------------------------------------
	// ノードの登録の流れは基本的に下記の通り
	//   1.レイアウト基準位置の設定:[setLayoutLT]
	//   2.ノードの配置:[putNodeOnLayout]
	//   3.ノードの登録:[addNode]
	// [addNode]で登録されたノードはループのデストラクタで解放される
	//------------------------------------------------------------
	// 例：窓の登録（※[CWin]は[CNode]のサブクラス）
	//		setLayoutLT( 0, 48, LAYOUT_DIR_N, LAYOUT_DIR_N );
	//
	//		m_pWin = new CWin( eGRP_WINDOW, 320, 240 );
	//		m_pWin->setTitle( "WINDOW TEST");
	//		putNodeOnLayout( m_pWin );
	//
	//		addNode( m_pWin );
	//------------------------------------------------------------
	void addNode( CNode* pNode );
	void setVisibleNodeAll( bool flag );
	void setActiveNodeAll( bool flag );
    void setDarkNodeAll( bool flag );
	void enterNodeAll( void );
	void exitNodeAll( void );

	// レイアウト関連
	void setLayoutBase( float x, float y, int alignX, int alignY, int dirX, int dirY );
	void setLayoutLT( float ofsX, float ofsY, int dirX, int dirY ); // 左上
	void setLayoutCT( float ofsX, float ofsY, int dirX, int dirY ); // 中上
	void setLayoutRT( float ofsX, float ofsY, int dirX, int dirY ); // 右上
	void setLayoutLM( float ofsX, float ofsY, int dirX, int dirY ); // 左中
	void setLayoutCM( float ofsX, float ofsY, int dirX, int dirY ); // 中央
	void setLayoutRM( float ofsX, float ofsY, int dirX, int dirY ); // 右中
	void setLayoutLB( float ofsX, float ofsY, int dirX, int dirY ); // 左下
	void setLayoutCB( float ofsX, float ofsY, int dirX, int dirY ); // 中下
	void setLayoutRB( float ofsX, float ofsY, int dirX, int dirY ); // 右下

	// ノード配置：レイアウト設定により、ノードのサイズと指定した調整値だけ次のノードの登録位置が調整される
	void putNodeOnLayout( CNode* pNode, float nextAdjX=0, float nextAdjY=0 );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __LOOP_HPP__ */
