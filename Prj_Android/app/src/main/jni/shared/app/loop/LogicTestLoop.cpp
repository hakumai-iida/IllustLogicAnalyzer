/*+----------------------------------------------------------------+
  |	Title:		LogicTestLoop.cpp [共通環境]
  |	Comment:	テストロジックループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "LogicTestLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ステップ
enum eSTEP{
    eSTEP_HOME,
    eSTEP_SELECT_SIZE,
    eSTEP_EDIT,
};

//------------------
// ループタスク
//------------------
enum eMY_LOOP_TASK{
    eMY_LOOP_TASK_INVALID = -1,         // 無効値

    eMY_LOOP_TASK_DIALOG_FOR_NOTICE,    // エラー通知
    eMY_LOOP_TASK_DIALOG_FOR_LOAD,      // ロード確認
    eMY_LOOP_TASK_DIALOG_FOR_SAVE,      // セーブ確認
};

// 表示位置＆サイズ
#define LOGIC_DISP_CX       (SA_XC+40)
#define LOGIC_DISP_CY       (SA_YM+30)
#define LOGIC_DISP_W        (SA_W-80)
#define LOGIC_DISP_H        (SA_H-80)

// ログ表示位置：ステータス
#define LOGIC_LOG_X         0
#define LOGIC_LOG_Y         36

// デフォルト保存名
#define DEFAULT_SAVE_NAME   "new.txt"

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// ファイルメニュー
static const char* _file_menu[] = {
    "sample0.txt",      // 10x10: りんご（Apple)
    "sample1.txt",      // 15x15: ドロイド君(Android)
    "sample2.txt",      // 20x20: メジェド様
    "sample3.txt",      // 25x25: Duke(Java)
    "sample4.txt",      // 30x30: 鳥(twitter)
    "sample5.txt",      // 35x35: オクトキャット(GitHub)
    "sample6.txt",      // 40x40: ヌー(GNU)
    "sample7.txt",      // 45x40: 現場ネコ
    "sample8.txt",      // 50x50: ゆっくり霊夢

    NULL
};

// サイズメニュー
static const char* _size_menu[] = {
    "10x10",    "10x15",    "10x20",    "10x25",    "10x30",    "10x35",    "10x40",    "10x45",    "10x50",
    "15x10",    "15x15",    "15x20",    "15x25",    "15x30",    "15x35",    "15x40",    "15x45",    "15x50",
    "20x10",    "20x15",    "20x20",    "20x25",    "20x30",    "20x35",    "20x40",    "20x45",    "20x50",
    "25x10",    "25x15",    "25x20",    "25x25",    "25x30",    "25x35",    "25x40",    "25x45",    "25x50",
    "30x10",    "30x15",    "30x20",    "30x25",    "30x30",    "30x35",    "30x40",    "30x45",    "30x50",
    "35x10",    "35x15",    "35x20",    "35x25",    "35x30",    "35x35",    "35x40",    "35x45",    "35x50",
    "40x10",    "40x15",    "40x20",    "40x25",    "40x30",    "40x35",    "40x40",    "40x45",    "40x50",
    "45x10",    "45x15",    "45x20",    "45x25",    "45x30",    "45x35",    "45x40",    "45x45",    "45x50",
    "50x10",    "50x15",    "50x20",    "50x25",    "50x30",    "50x35",    "50x40",    "50x45",    "50x50",

    NULL,
};

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// コンストラクタ
//------------------------
CLogicTestLoop::CLogicTestLoop( void ){
	setId( eLOOP_ID_LOGIC_TEST );

	setDrawLayer( eDP_LAYER_LOOP );
}

//------------------------
// デストラクタ
//------------------------
CLogicTestLoop::~CLogicTestLoop( void ){}

//------------------------
// 初期化
//------------------------
void CLogicTestLoop::init0( void ){
    // デバッグ表示オフ
    CAppMgr::SetDebugLog( false , false );
    
    //--------------------------
	// ロジック確保＆初期化
    //--------------------------
    allocThenInitLogic();

    //--------------------------
    // レイアウト：左下
    //--------------------------
    setLayoutLB( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );

    // 黒仮定ボタン
    m_pButtonForAssumeBlack = new CButton( eGRP_DEBUG_BUTTON, "ASM BLK" );
    putNodeOnLayout( m_pButtonForAssumeBlack );

    // オートボタン
    m_pButtonAuto = new CButton( eGRP_DEBUG_BUTTON, "AUTO" );
    putNodeOnLayout( m_pButtonAuto );

    // リセットボタン
    m_pButtonReset = new CButton( eGRP_DEBUG_BUTTON, "RESET" );
    putNodeOnLayout( m_pButtonReset );

    //--------------------------
    // レイアウト：右上
    //--------------------------
    setLayoutRT( 0, 0, eLAYOUT_DIR_B, eLAYOUT_DIR_N );

    // 新規作成ボタン
    m_pButtonNew = new CButton( eGRP_DEBUG_BUTTON, "NEW" );
    putNodeOnLayout( m_pButtonNew );

    // ロードボタン
    m_pButtonLoad = new CButton( eGRP_DEBUG_BUTTON, "LOAD" );
    putNodeOnLayout( m_pButtonLoad );

    // エディットボタン
    m_pButtonEdit = new CButton( eGRP_DEBUG_BUTTON, "EDIT" );
    putNodeOnLayout( m_pButtonEdit );

    //--------------------------
    // レイアウト：右上
    //--------------------------
    setLayoutRT( 0, 0, eLAYOUT_DIR_B, eLAYOUT_DIR_N );

    // キャンセルボタン
    m_pButtonCancel = new CButton( eGRP_DEBUG_BUTTON, "CANCEL" );
    putNodeOnLayout( m_pButtonCancel );

    //------------------------
    // 右上
    //------------------------
    setLayoutRT( 0, 0, eLAYOUT_DIR_B, eLAYOUT_DIR_N );

    // 戻るボタン
    m_pButtonBack = new CButton( eGRP_DEBUG_BUTTON, "BACK" );
    putNodeOnLayout( m_pButtonBack );

    // セーブボタン
    m_pButtonSave = new CButton( eGRP_DEBUG_BUTTON, "SAVE" );
    putNodeOnLayout( m_pButtonSave );

    // チェックボタン
    m_pButtonCheck = new CButton( eGRP_DEBUG_BUTTON, "CHECK" );
    putNodeOnLayout( m_pButtonCheck );

    //---------------------------------
    // 左上（※このボタンは動的に配置される）
    //---------------------------------
    setLayoutLT( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_N );

    // 前のヒントボタン
    m_pButtonCluePrev = new CButton( eGRP_DEBUG_BUTTON_L, "" );
    putNodeOnLayout( m_pButtonCluePrev );

    // 次のヒントボタン
    m_pButtonClueNext = new CButton( eGRP_DEBUG_BUTTON_L, "" );
    putNodeOnLayout( m_pButtonClueNext );

    // ヒント追加ボタン
    m_pButtonClueAdd = new CButton( eGRP_DEBUG_BUTTON_P, "ADD" );
    putNodeOnLayout( m_pButtonClueAdd );

    // ヒント削除ボタン
    m_pButtonClueDelete = new CButton( eGRP_DEBUG_BUTTON_M, "DEL" );
    putNodeOnLayout( m_pButtonClueDelete );

    //------------------------
    // 右下
    //------------------------
    // ヒント数字
    char buf[16];
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        sprintf( buf, "%2d", i+1 );
        m_pArrButtonClueVal[i] = new CButton( eGRP_DEBUG_BUTTON_M, buf );
    }

    int depth = -1;
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        if( (i%10) == 0 ){
            depth++;
            setLayoutRB( 0, -depth*(32+4), eLAYOUT_DIR_B, eLAYOUT_DIR_N );
        }
        putNodeOnLayout( m_pArrButtonClueVal[i], -4, 0 );
    }

    //------------------------
    // レイアウト：左上
    //------------------------
    setLayoutLT( 40, 5, eLAYOUT_DIR_N, eLAYOUT_DIR_N );
    
    // ファイルメニュー
    m_pMenuFile = new CPullDownMenu( false, false, true );
    m_pMenuFile->setMaxCenterWH( 184, 240 );
    m_pMenuFile->setItemArrayThenFix( _file_menu );
    putNodeOnLayout( m_pMenuFile );

    //------------------------
    // レイアウト：左上
    //------------------------
    setLayoutLT( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_N );

    m_pMenuSize = new CStrMenu( eGRP_DEBUG_WINDOW, 128, 288 );
    m_pMenuSize->setTitle( "Size Menu");
    m_pMenuSize->setItemArrayThenFix( _size_menu );
    putNodeOnLayout( m_pMenuSize );

    //-----------------------------------------
    // ノード追加
    //-----------------------------------------
    addNode( m_pButtonReset );
    addNode( m_pButtonAuto );
    addNode( m_pButtonForAssumeBlack );

    addNode( m_pButtonNew );
    addNode( m_pButtonLoad );
    addNode( m_pButtonEdit );
    
    addNode( m_pButtonCancel );

    addNode( m_pButtonCheck );
    addNode( m_pButtonSave );
    addNode( m_pButtonBack );

    addNode( m_pButtonCluePrev );
    addNode( m_pButtonClueNext );
    addNode( m_pButtonClueAdd );
    addNode( m_pButtonClueDelete );
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        addNode( m_pArrButtonClueVal[i] );
    }

    addNode( m_pMenuFile );
    addNode( m_pMenuSize );
    
    // 初期値
    m_bBlackForAssume = true;
    m_pButtonForAssumeBlack->setFocused( m_bBlackForAssume );
}

//------------------------
// 終了
//------------------------
void CLogicTestLoop::exit0( void ){
	// ロジック解放
    releaseLogic();
}

//------------------------
// 更新
//------------------------
void CLogicTestLoop::update0( void ){
    
    if( isStepChanged() ){
        exitNodeAll();

        switch( m_nStep ){
        case eSTEP_HOME:
            m_pButtonReset->enter();
            m_pButtonAuto->enter();
            m_pButtonForAssumeBlack->enter();

            m_pButtonNew->enter();
            m_pButtonLoad->enter();
            m_pButtonEdit->enter();

            m_pMenuFile->enter();
                
            // オフセットリセット
            m_pLogicDisp->setOfsXY( 0, 0 );
            break;
                
        case eSTEP_SELECT_SIZE:
            m_pButtonCancel->enter();
                
            m_pMenuSize->enter();

            // サイズ確認用のロジックの初期化
            initLogicForEdit( m_pMenuSize->getSelect(), m_pLogicForSize, m_pLogicDispForSize );
            fixLogicForEdit( m_pLogicForSize, m_pLogicDispForSize );
            break;
                
        case eSTEP_EDIT:
            // 前方遷移であれば
            if( m_nStep > m_nStep0 ){
                // 新規の指定であれば指定サイズでロジックを初期化
                if( m_nStep0 ){
                    initLogicForEdit( m_pMenuSize->getSelect(), m_pLogic, m_pLogicDisp );
                }
                // 既存のロジックの編集であればリセット
                else{
                    onReset();
                }

                fixLogicForEdit( m_pLogic, m_pLogicDisp );

                // 未選択であれば初期位置の選択
                if( m_pLogic->getSelectClueForColumn() < 0 && m_pLogic->getSelectClueForRow() < 0 ){
                    m_pLogic->setSelectClueForRow( 0, 0 );
                }
            }
                
            m_pButtonCheck->enter();
            m_pButtonSave->enter();
            m_pButtonBack->enter();
                
            fixButtonForEdit();
            break;
        }
        
        fixStep();
    }else{
        switch( m_nStep ){
        case eSTEP_HOME:
            if( m_pButtonReset->isTapped() ){ onReset(); }
            else if( m_pButtonAuto->isTapped() ){ onAuto(); }
            else if( m_pButtonForAssumeBlack->isTapped() ){
                m_bBlackForAssume = ! m_bBlackForAssume;
                m_pButtonForAssumeBlack->setFocused( m_bBlackForAssume );
            }
            else if( m_pButtonNew->isTapped() ){ setStep( eSTEP_SELECT_SIZE ); }
            else if( m_pButtonLoad->isTapped() ){
                CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
                pReserve->setDialogInput( this, eMY_LOOP_TASK_DIALOG_FOR_LOAD,
                                          "LOAD LOGIC TEXT FILE", "INPUT LOAD FILE NAME", "OK", "CANCEL", m_cArrFileName );
            }
            else if( m_pButtonEdit->isTapped() ){ setStep( eSTEP_EDIT ); }
            else if( m_pMenuFile->isDecided() ){ onFile(); }
            else{ updateForAuto(); }
            break;
                
        case eSTEP_SELECT_SIZE:
            if( m_pButtonCancel->isTapped() ){ setStep( eSTEP_HOME ); }
            else if( m_pMenuSize->isDecided() ){ setStep( eSTEP_EDIT ); }
            else if( m_pMenuSize->isSelectChanged() ){
                initLogicForEdit( m_pMenuSize->getSelect(), m_pLogicForSize, m_pLogicDispForSize );
                fixLogicForEdit( m_pLogicForSize, m_pLogicDispForSize );
            }
            break;
                
        case eSTEP_EDIT:
            if( m_pButtonCheck->isTapped() ){ onCheck(); }
            else if( m_pButtonSave->isTapped() ){
                CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
                pReserve->setDialogInput( this, eMY_LOOP_TASK_DIALOG_FOR_SAVE,
                                          "SAVE LOGIC TXT FILE", "INPUT SAVE FILE NAME", "OK", "CANCEL", m_cArrFileName );
            }
            else if( m_pButtonBack->isTapped() ){ setStep( eSTEP_HOME ); }
            else{ updateForEdit(); }
            break;
        }
     
        if( isStepChanged() ){
            setActiveNodeAll( false );
        }
    }
}

//------------------------
// 描画
//------------------------
void CLogicTestLoop::onDraw0( void ){
    // 選択対象取得
    int selCol = -1;
    int selRow = -1;
    int selClueForCol = -1;
    int selClueForRow = -1;
    int focusClueForCol = -1;
    int focusClueForRow = -1;

    if( m_nStep0 == eSTEP_HOME && m_pLogic->isAutoOn() ){
        selCol = m_pLogic->getSelectColumn();
        selRow = m_pLogic->getSelectRow();
    }else if( m_nStep0 == eSTEP_EDIT ){
        selClueForCol =  m_pLogic->getSelectClueForColumn();
        selClueForRow = m_pLogic->getSelectClueForRow();
        focusClueForCol = m_pLogic->getFocusClueForColumn();
        focusClueForRow = m_pLogic->getFocusClueForRow();
    }
    
    // 通常表示（オート／エディット）
    if( m_nStep0 != eSTEP_SELECT_SIZE ){
        m_pLogicDisp->draw( true, selCol, selRow, selClueForCol, selClueForRow, focusClueForCol, focusClueForRow );

        // ホームであればログの表示
        if( m_nStep0 == eSTEP_HOME ){
            m_pLogic->drawLog( LOGIC_LOG_X, LOGIC_LOG_Y );
        }
    }
    // サイズ確認用表示
    else{
        m_pLogicDispForSize->draw( false, -1, -1, -1, -1, -1, -1 );
    }
}

//--------------------
// タスクが開始された
//--------------------
void CLogicTestLoop::taskStarted0( int taskId ){
    // 特になし
}

//--------------------
// タスクが終了した
//--------------------
void CLogicTestLoop::taskFinished0( int taskId, bool isSuccess, int code, const char* pResponse ){
    if( isSuccess ){
        switch( taskId ){
        // 通知
        case eMY_LOOP_TASK_DIALOG_FOR_NOTICE:
            // 特に何もしない
            break;

        // ロード
        case eMY_LOOP_TASK_DIALOG_FOR_LOAD:
            if( m_pLogic->initWithFile( pResponse, m_cArrErrorText, true ) ){
                // ファイル名を保持
                util_str_cpy( m_cArrFileName, pResponse, COMMON_NAME_LEN );
            }else{
                // エラー通知
                CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
                pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "LOAD ERROR", m_cArrErrorText, "OK", NULL );
            }
                
            m_pLogicDisp->init( m_pLogic->getGrid(), LOGIC_DISP_CX, LOGIC_DISP_CY, LOGIC_DISP_W, LOGIC_DISP_H );
            break;

        // セーブ
        case eMY_LOOP_TASK_DIALOG_FOR_SAVE:
        {
            // 編集中のロジック（ヒント）の内容の取得
            char* buf = CMemMgr::GetBufFromTempStr();
            CLogicGrid* pGrid = m_pLogic->getGrid();
            pGrid->dumpToBuf( buf );

            // 指定ファイルに出力
            if( CFileMgr::SaveLocalData( pResponse, buf, util_str_len(buf) ) ){
                // ファイル名を保持
                util_str_cpy( m_cArrFileName, pResponse, COMMON_NAME_LEN );
            }else{
                sprintf( m_cArrErrorText, "・エラー：「%s」のセーブに失敗しました", pResponse );
                
                // エラー通知
                CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
                pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "LOAD ERROR", m_cArrErrorText, "OK", NULL );
            }
        }
            break;
        }
    }
}

//----------------------
// バッファ確保＆初期化
//----------------------
void CLogicTestLoop::allocThenInitLogic( void ){
    // 用心に解放
    releaseLogic();

    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_LOGIC );
    m_pLogic = new CLogic();
    m_pLogicDisp = new CLogicDispForDev();
    
    m_pLogicForSize = new CLogic();
    m_pLogicDispForSize = new CLogicDispForDev();
    CMemMgr::PopTargetField();

    // ロジック初期化（※メニューの第一項目）
    m_pLogic->initWithFile( _file_menu[0] );
    m_pLogicDisp->init( m_pLogic->getGrid(), LOGIC_DISP_CX, LOGIC_DISP_CY, LOGIC_DISP_W, LOGIC_DISP_H );

    // デフォルト出力ファイル名
    util_str_cpy( m_cArrFileName, DEFAULT_SAVE_NAME, COMMON_NAME_LEN );

    // リセット
    onReset();
}

//----------------------
// ロジック確認：領域解放
//----------------------
void CLogicTestLoop::releaseLogic( void ){
    SAFE_DELETE( m_pLogic );
    SAFE_DELETE( m_pLogicDisp );

    SAFE_DELETE( m_pLogicForSize );
    SAFE_DELETE( m_pLogicDispForSize );
}

//------------------------
// 更新：オート
//------------------------
void CLogicTestLoop::updateForAuto( void ){
    bool isAutoOn0 = m_pLogic->isAutoOn();
    m_pLogic->playAutoWithLog( NULL );

#ifdef ENABLE_SS
    // 一定のマスが埋まるたびにスクショを取る
    if( isAutoOn0 ){
        CLogicGrid* pGrid = m_pLogic->getGrid();
        int col = pGrid->getColumn();
        int row = pGrid->getRow();
        CLogicSolid* pSolid;
        
        int num = 0;
        for( int i=0; i<col; i++ ){
            for( int j=0; j<row; j++ ){
                pSolid = pGrid->getSolid( i, j );
                if( pSolid->isDeterminedOrAssumed() ){
                    num++;
                }
            }
        }

        // 仮定による埋まりもみるため増減の両方で検出する
        if( (num-m_nCurSolidForSS) > 16 || (num-m_nCurSolidForSS) < -16 ){
            m_nCurSolidForSS = num;
            CMainRenderer::ReserveScreenShot();
        }
    }
#endif

    // 状況が変わっていたら（※オートが終了した）
    if( isAutoOn0 != m_pLogic->isAutoOn() ){
        // ボタンの再設定
        fixButtonsForAuto( m_pLogic->isAutoOn() );
        
        // オートの終了内容によって通知
        if( isAutoOn0 ){
            eLOGIC_STATUS status = m_pLogic->getStatus();
            
            // クリア
            if( status == eLOGIC_STATUS_CLEAR ){
                CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
                pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "SUCCESS",
                                     "解析が正常に終了しました", "OK", NULL );
            }
            // 矛盾（※ロジックの問題として解析が不可能）
            else if( status == eLOGIC_STATUS_IMCOMPATIBLE ){
                CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
                pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "IMCOMPATIBLE",
                                     "解析が矛盾しました（埋まったマスとヒントの間で整合性が崩れたため処理を中断しました）", "OK", NULL );
            }
            // ギブアップ（※手詰まりをした場合仮置きを始めるのでここにはこないはず）
            else if( status == eLOGIC_STATUS_GIVE_UP ){
                CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
                pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "GIVE UP",
                                     "解析が行き詰まりました（これ以上解析を行えないため処理を中断しました）", "OK", NULL );
            }
        }
        
#ifdef ENABLE_SS
        // 最終状況のスクショ
        CMainRenderer::ReserveScreenShot();
#endif
    }
}

//------------------------
// 更新：エディット
//------------------------
void CLogicTestLoop::updateForEdit( void ){
    //------------------------------------------
    // 入力確認
    //------------------------------------------
    bool isChanged = false;
    int selectCol, selectRow;

    // タップ確認：ヒント（ヨコ）
    if( m_pLogicDisp->checkTouchForColClue( eTOUCH_CHECK_TAPPED, &selectCol ) ){
        isChanged = m_pLogic->selectClueForColumn( selectCol, true );
    }
    // タップ確認：ヒント（タテ）
    else if( m_pLogicDisp->checkTouchForRowClue( eTOUCH_CHECK_TAPPED, &selectRow ) ){
        isChanged = m_pLogic->selectClueForRow( selectRow, true );
    }
    // 各種ボタンの確認
    else{
        // 前のヒント
        if( m_pButtonCluePrev->isTapped() ){
            isChanged = m_pLogic->prevClue();
        }
        // 次のヒント
        if( m_pButtonClueNext->isTapped() ){
            isChanged = m_pLogic->nextClue();
        }
        // ヒントの追加
        else if( m_pButtonClueAdd->isTapped() ){
            isChanged = m_pLogic->addClue();
        }
        // ヒントの削除
        else if( m_pButtonClueDelete->isTapped() ){
            isChanged = m_pLogic->deleteClue();
        }
        // 各種数字ボタン確認
        else{
            for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
                if( m_pArrButtonClueVal[i]->isTapped() ){
                    isChanged = m_pLogic->setClue( i+1 );
                    break;
                }
            }
        }
    }

    if( isChanged ){
        fixButtonForEdit();
    }
}

//----------------------
// ボタン押下：リセット
//----------------------
void CLogicTestLoop::onReset( void ){
    m_pLogic->reset();
    
#ifdef ENABLE_SS
    m_nCurSolidForSS = 0;
#endif
}

//----------------------
// ボタン押下：オート
//----------------------
void CLogicTestLoop::onAuto( void ){
    if( m_pLogic->isAutoOn() ){
        if( ! m_pLogic->setAutoOff() ){
            CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
            pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "AUTO OFF FAILED",
                                 "解析が正常に終了しました", "OK", NULL );
        }
    }else{
        // 仮定マスの色を指定
        m_pLogic->setAutoSelectBlackForAssume( m_bBlackForAssume );

        if( ! m_pLogic->setAutoOn() ){
            CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
            pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "AUTO ON FAILED",
                                 "再度解析を行う場合は「RESET」をしてください", "OK", NULL );
        }
    }
    
    fixButtonsForAuto( m_pLogic->isAutoOn() );
}

//----------------------
// メニュー選択：ファイル決定
//----------------------
void CLogicTestLoop::onFile( void ){
    const char* pFile = m_pMenuFile->getSelectedLabel();
    m_pLogic->initWithFile( pFile );
    m_pLogicDisp->init( m_pLogic->getGrid(), LOGIC_DISP_CX, LOGIC_DISP_CY, LOGIC_DISP_W, LOGIC_DISP_H );

    // リセット
    onReset();
}

//----------------------
// ボタン押下：チェック
//----------------------
void CLogicTestLoop::onCheck( void ){
    // 選択状況の待避（※再選択用）
    int selectRow = m_pLogic->getSelectClueForRow();
    int focusRow = m_pLogic->getFocusClueForRow();
    int selectColumn = m_pLogic->getSelectClueForColumn();
    int focusColumn = m_pLogic->getFocusClueForColumn();

    char* buf = CMemMgr::GetBufFromTempStr();
    CLogicGrid* pGrid = m_pLogic->getGrid();
    pGrid->dumpToBuf( buf );

    // 初期化処理を整合性チェックに流用
    if( m_pLogic->initWithDataStr( buf, m_cArrErrorText ) ){
        // 成功通知
        CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
        pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "CHECK OK",
                             "ヒントの設定に問題はなさそうです\n\n"
                             "「BACK」で前の画面に戻り、「AUTO」にて解析が可能です\n\n"
                             "（※「SAVE」により名前をつけて保存しておき、あとで「LOAD」により読み直すことができます）", "OK", NULL );
    }else{
        // エラー通知
        CLoopTaskReserve* pReserve = CLoopMgr::GetNewLoopTaskReserve();
        pReserve->setDialog( this, eMY_LOOP_TASK_DIALOG_FOR_NOTICE, "CHECK ERROR", m_cArrErrorText, "OK", NULL );
    }

    // 選択状況の復旧
    if( selectRow >= 0) {
        m_pLogic->setSelectClueForRow( selectRow, focusRow );
    }else if( selectColumn >= 0 ){
        m_pLogic->setSelectClueForColumn( selectColumn, focusColumn );
    }    
}

//--------------------------------
// 「オート」に対するボタン設定
//--------------------------------
void CLogicTestLoop::fixButtonsForAuto( bool isAutoOn ){
    m_pButtonReset->setActive( ! isAutoOn );

    if( isAutoOn ){
        m_pButtonAuto->setName( "STOP" );
    }else{
        m_pButtonAuto->setName( "AUTO" );
    }
    
    m_pButtonForAssumeBlack->setActive( ! isAutoOn );

    m_pButtonNew->setActive( ! isAutoOn );
    m_pButtonLoad->setActive( ! isAutoOn );
    m_pButtonEdit->setActive( ! isAutoOn );

    m_pMenuFile->setActive( ! isAutoOn );
}

//---------------------------
// エディット用ロジック初期化
//---------------------------
void CLogicTestLoop::initLogicForEdit( int target, CLogic* pLogic, CLogicDispForDev* pDisp ){
    // [target]は10開始で5刻みに50までの数字の組み合わせ
    int maxRow = 10 + 5*(target%9);
    int maxCol = 10 + 5*(target/9);

    pLogic->initForSize( maxCol, maxRow );
    pDisp->init( pLogic->getGrid(), LOGIC_DISP_CX, LOGIC_DISP_CY, LOGIC_DISP_W, LOGIC_DISP_H );
    pLogic->fixForSize();
}

//---------------------------
// エディット用ロジック調整
//---------------------------
void CLogicTestLoop::fixLogicForEdit( CLogic* pLogic, CLogicDispForDev* pDisp ){
    float baseX = pDisp->getClueX();

    // エディットボタンが重ならないように左へ寄せる
    float marginX = baseX - 72;
    if( marginX < 0 ){ marginX = 0; }
    
    pDisp->setOfsXY( -marginX, 0 );
}

//--------------------------------
// エディットに対するボタン設定
//--------------------------------
void CLogicTestLoop::fixButtonForEdit( void ){
    // work
    CLogicGrid* pGrid = m_pLogic->getGrid();
    bool isRow = false;
    int selectAt = -1;
    int numSolid = 0;
    int maxClue = 0;
    int numClue = 0;

    // 選択対象の取得
    if( (selectAt=m_pLogic->getSelectClueForColumn()) >= 0 ){
        isRow = false;
        numSolid = pGrid->getRow();
        maxClue = (numSolid+1)/2;
        numClue = pGrid->getClueNumForColumn( selectAt );
    }else if( (selectAt=m_pLogic->getSelectClueForRow()) >= 0 ){
        isRow = true;
        numSolid = m_pLogic->getColumn();
        maxClue = (numSolid+1)/2;
        numClue = pGrid->getClueNumForRow( selectAt );
    }
    
    // 対象が有効であれば領域前後にボタンの表示
    if( selectAt >= 0 ){
        float x0, y0, x1, y1;
        if( isRow ){
            m_pLogicDisp->getXYForClue( -1, selectAt, &x0, &y0, &x1, &y1 );
            x0 -=16;
            y0 -= 32;
            x1 -=16;
            y1 += 2;
            m_pButtonCluePrev->put( x0, y0 );
            m_pButtonClueNext->put( x1, y1 );
            m_pButtonClueAdd->put( x1, y1 + 1*(32+4) );
            m_pButtonClueDelete->put( x1, y1 + 2*(32+4) );

            m_pButtonCluePrev->setGrp( eGRP_DEBUG_BUTTON_U );
            m_pButtonClueNext->setGrp( eGRP_DEBUG_BUTTON_U );
            m_pButtonClueNext->setFlipX( false );
            m_pButtonClueNext->setFlipY( true );
        }else{
            m_pLogicDisp->getXYForClue( selectAt, -1, &x0, &y0, &x1, &y1 );
            y0 -=16;
            x0 -= 32;
            y1 -=16;
            x1 += 2;
            m_pButtonCluePrev->put( x0, y0 );
            m_pButtonClueNext->put( x1, y1 );
            m_pButtonClueAdd->put( x1+1*(32+4), y1 );
            m_pButtonClueDelete->put( x1+2*(32+4), y1 );

            m_pButtonCluePrev->setGrp( eGRP_DEBUG_BUTTON_L );
            m_pButtonClueNext->setGrp( eGRP_DEBUG_BUTTON_L );
            m_pButtonClueNext->setFlipX( true );
            m_pButtonClueNext->setFlipY( false );
        }
        
        // 前のヒントボタン（※ヒントが１つであれば制限）
        m_pButtonCluePrev->enter();
        m_pButtonCluePrev->setDark( numClue <= 1 );

        // 次のヒントボタン（※ヒントが１つであれば制限）
        m_pButtonClueNext->enter();
        m_pButtonClueNext->setDark( numClue <= 1 );

        // ヒントの追加ボタン（※ヒントが最大であれば制限）
        m_pButtonClueAdd->enter();
        m_pButtonClueAdd->setDark( numClue >= maxClue );

        // ヒントの削除ボタン（※ヒントが１つであれば割り振り）
        m_pButtonClueDelete->enter();
        if( numClue <= 1 ){
            m_pButtonClueDelete->setName( "ZERO" );
        }else{
            m_pButtonClueDelete->setName( "DEL" );
        }
    }
    // 未選択であれば非表示
    else{
        m_pButtonCluePrev->exit();
        m_pButtonClueNext->exit();
        m_pButtonClueAdd->exit();
        m_pButtonClueDelete->exit();
    }

    // 数値ボタン（※編集中のロジックのタテ／ヨコ幅に応じた数字までを表示）
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        if( i < numSolid ){
            m_pArrButtonClueVal[i]->enter();
        }else{
            m_pArrButtonClueVal[i]->exit();
        }
    }
}
