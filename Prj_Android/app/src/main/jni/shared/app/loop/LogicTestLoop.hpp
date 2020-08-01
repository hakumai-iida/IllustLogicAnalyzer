/*+----------------------------------------------------------------+
  |	Title:		LogicTestLoop.hpp [共通環境]
  |	Comment:	ロジックテストループ（※ロジック確認用の直接表示形式）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_TEST_LOOP_HPP__
#define __LOGIC_TEST_LOOP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "loop/Loop.hpp"
#include "loop/node/Button.hpp"
#include "loop/node/StrMenu.hpp"
#include "loop/node/PullDownMenu.hpp"

#include "app/logic/core/Logic.hpp"
#include "app/logic/dev/LogicDispForDev.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// スクショの有効化
//#define ENABLE_SS

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジックテストループ
//----------------------------------------
class CLogicTestLoop: public CLoop{
protected:
    //----------------
    // ボタン
    //----------------
    CButton* m_pButtonReset;            // リセット
    CButton* m_pButtonAuto;             // オートオン／オフ
    CButton* m_pButtonForAssumeBlack;   // 仮定を黒で行うか？
    
    CButton* m_pButtonNew;              // 新規作成
    CButton* m_pButtonLoad;             // ロード
    CButton* m_pButtonEdit;             // エディット
    
    CButton* m_pButtonCancel;           // キャンセル
    
    CButton* m_pButtonCheck;            // チェック
    CButton* m_pButtonSave;             // セーブ
    CButton* m_pButtonBack;             // 戻る

    //----------------
    // エディット
    //----------------
    CButton* m_pButtonCluePrev;         // 前のヒント
    CButton* m_pButtonClueNext;         // 次のヒント
    CButton* m_pButtonClueAdd;          // ヒント追加
    CButton* m_pButtonClueDelete;       // ヒント削除
    
    // ヒント数字ボタン
    CButton* m_pArrButtonClueVal[LOGIC_SOLID_WH_MAX];
    
    //----------------
    // メニュー
    //----------------
    CPullDownMenu*  m_pMenuFile;        // ファイルメニュー
    CStrMenu* m_pMenuSize;              // サイズメニュー
   
    //-------------------------
    // ロジック関連
    //-------------------------
    CLogic* m_pLogic;                   // ロジック
    CLogicDispForDev* m_pLogicDisp;     // ロジック表示

    // サイズ確認用
    CLogic* m_pLogicForSize;
    CLogicDispForDev* m_pLogicDispForSize;

    //-------------------------
    // ワーク
    //-------------------------
    // 入出力ファイル名
    char m_cArrFileName[COMMON_NAME_LEN];
    
    // エラー文言
    char m_cArrErrorText[4096];     // ダイアログ用
    
    // 仮定を黒で行うか？
    bool m_bBlackForAssume;

#ifdef ENABLE_SS
    int m_nCurSolidForSS;
#endif

public:
    // コンストラクタ／デストラクタ
	CLogicTestLoop( void );
	virtual ~CLogicTestLoop( void );

protected:
    // 初期化／終了
	virtual void init0( void );
	virtual void exit0( void );
    
    // 更新／描画
	virtual void update0( void );
	virtual void onDraw0( void );

    // タスク
    virtual void taskStarted0( int taskId );
    virtual void taskFinished0( int taskId, bool isSuccess, int code, const char* pResponse );

private:
    //------------------------------
	// ロジック確認
    //------------------------------
    void allocThenInitLogic( void );
    void releaseLogic( void );
    
    // 更新
    void updateForAuto( void );
    void updateForEdit( void );
 
    // ボタン押下
    void onReset( void );
    void onAuto( void );
    void onFile( void );
    void onCheck( void );

    // オート用
    void fixButtonsForAuto( bool isAutoOn );
    
    // エディット用
    void initLogicForEdit( int target, CLogic* pLogic, CLogicDispForDev* pDisp );
    void fixLogicForEdit( CLogic* pLogic, CLogicDispForDev* pDisp );
    void fixButtonForEdit( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __LOGIC_TEST_LOOP_HPP__ */
