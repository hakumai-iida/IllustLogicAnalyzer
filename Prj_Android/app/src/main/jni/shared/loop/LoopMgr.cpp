/*+----------------------------------------------------------------+
  |	Title:		LoopMgr.cpp [共通環境]
  |	Comment:	ループ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LoopMgr.hpp"
#include "helper/DialogHelper.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/    
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 管理データ
CList* CLoopMgr::s_pListLoop;
CList* CLoopMgr::s_pListReserveLoop;
CList* CLoopMgr::s_pListReserveLoopTask;
CLoopTaskReserve* CLoopMgr::s_pActiveLoopTask;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CLoopMgr::Init( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CLoopMgr::Init()\n" );
#endif

    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );

	// 初期化
	s_pListLoop = new CList();
	s_pListReserveLoop = new CList();
	s_pListReserveLoopTask = new CList();
	s_pActiveLoopTask = NULL;

    CMemMgr::PopTargetField();

	return( true );
}

//--------------------------
// 終了
//--------------------------
void CLoopMgr::Exit( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CLoopMgr::Exit()\n" );
#endif

	if( s_pListLoop != NULL ){
		// ループの終了
		CLoop* pLoop = (CLoop*) s_pListLoop->getHead();
		while( pLoop != NULL ){
			pLoop->onExit();
			pLoop = pLoop->getNext();
		}

		s_pListLoop->deleteNodeAll();
		SAFE_DELETE( s_pListLoop );
	}

	if( s_pListReserveLoop != NULL ){
		s_pListReserveLoop->deleteNodeAll();
		SAFE_DELETE( s_pListReserveLoop );
	}

	if( s_pListReserveLoopTask != NULL ){
		s_pListReserveLoopTask->deleteNodeAll();
		SAFE_DELETE( s_pListReserveLoopTask );
	}
}

//--------------------------
// 更新
//--------------------------
void CLoopMgr::Update( void ){
	//-------------------------
	// ループタスクの終了待ち
	//-------------------------
	if( s_pActiveLoopTask != NULL ){
		// タスクが終了したらリストから削除
		if( CheckLoopTaskFinish( s_pActiveLoopTask ) ){
			s_pListReserveLoopTask->deleteNode( s_pActiveLoopTask );
			s_pActiveLoopTask = NULL;
		}
	}

	//-------------------------
	// ループタスク予約実行
	//-------------------------
	if( s_pActiveLoopTask == NULL ){
		CLoopTaskReserve* pTask = (CLoopTaskReserve*) s_pListReserveLoopTask->getHead();
		while( pTask != NULL ){
			// 呼び出しが行われていたらアクティブに設定
			if( CheckLoopTaskStart( pTask ) ){
				s_pActiveLoopTask = pTask;
				break;
			}

			// ここまで来たら無効な予約としてリストから削除
			CLoopTaskReserve* pNext = pTask->getNext();
			s_pListReserveLoopTask->deleteNode( pTask );
			pTask = pNext;
		}
	}

	//-------------------------
	// ループ予約実行
	//-------------------------
    CLoopReserve* pReserve = (CLoopReserve*) s_pListReserveLoop->getHead();
	while( pReserve != NULL ){
		ExecReserveLoop( pReserve );
		pReserve = pReserve->getNext();
	}
	// 予約要素の削除
	s_pListReserveLoop->deleteNodeAll();

	//-------------------------
	// 不要ループ破棄
	//-------------------------
	CLoop* pLoop = (CLoop*) s_pListLoop->getHead();
	while( pLoop != NULL ){
        CLoop* pNext = pLoop->getNext();
		if( pLoop->isFinished() ){
			s_pListLoop->deleteNode( pLoop );
		}
        pLoop = pNext;
	}

	//-------------------------
	// ループ更新
	//-------------------------
	pLoop = (CLoop*) s_pListLoop->getHead();
	while( pLoop != NULL ){
		pLoop->onUpdate();
		pLoop = pLoop->getNext();
	}
}

//--------------------------
// 描画登録
//--------------------------
void CLoopMgr::RegistForDraw( void ){
	CLoop* pLoop = (CLoop*) s_pListLoop->getHead();
	while( pLoop != NULL ){
		pLoop->onRegistForDraw();
		pLoop = pLoop->getNext();
	}
}

//--------------------------
// ノード描画
//--------------------------
void CLoopMgr::DrawNode( void ){
	CLoop* pLoop = (CLoop*) s_pListLoop->getHead();
	while( pLoop != NULL ){
		pLoop->onDrawNode();
		pLoop = pLoop->getNext();
	}
}

//--------------------------
// ログ描画
//--------------------------
void CLoopMgr::DrawLog( void ){
	CLoop* pLoop = (CLoop*) s_pListLoop->getHead();
	while( pLoop != NULL ){
		pLoop->onDrawLog();
		pLoop = pLoop->getNext();
	}
}

//-------------
// ループ確認
//-------------
bool CLoopMgr::IsLoopActive( int id ){
	CLoop* pLoop = (CLoop*) s_pListLoop->getHead();
	while( pLoop != NULL ){
		if( pLoop->getId() == id ){
			return( true );
		}
		pLoop = pLoop->getNext();
	}

	return( false );
}

//-------------
// 中断
//-------------
void CLoopMgr::OnAppSuspend( void ){
    LOGD( "@ CLoopMgr::OnAppSuspend\n" );

	CLoop* pLoop = (CLoop*) s_pListLoop->getHead();
	while( pLoop != NULL ){
		pLoop->onSuspend();
		pLoop = pLoop->getNext();
	}
}

//-------------
// 再開
//-------------
void CLoopMgr::OnAppResume( void ){
    LOGD( "@ CLoopMgr::OnAppResume\n" );

	CLoop* pLoop = (CLoop*)s_pListLoop->getHead();
	while( pLoop != NULL ){
		pLoop->onResume();
		pLoop = pLoop->getNext();
	}
}

//-------------
// ループ予約
//-------------
void CLoopMgr::ReserveLoop( CLoop* pLoop, eLOOP_RESERVE reserveType, int loopId, int option ){
	// 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );

	CLoopReserve* pReserve = new CLoopReserve();
	pReserve->set( pLoop, reserveType, loopId, option );
	s_pListReserveLoop->add( pReserve );

    CMemMgr::PopTargetField();
}

//-------------
// ループの生成
//-------------
void CLoopMgr::CreateLoop( int loopId, int option, CLoop* pParent ){
	// ループの作成（※デフォルトのメモリ確保先は[D_APP]としておく）
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    CLoop* pLoop = CreateLoopForApp( loopId );
	CMemMgr::PopTargetField();

	// 作成されなければ致命的
	if( pLoop == NULL ){
		LOGE( "@ CLoopMgr::StartLoop: NOT CREATED: loopId=%d\n", loopId );
		return;
	}

	// ループの初期化
	LOGD( "@ LoopMgr::StartLoopCell:%d\n", pLoop->getId() );
	pLoop->onInit( pParent, option );

	// リストへ登録
	s_pListLoop->add( pLoop );
}

//---------------
// ループ予約実行
//---------------
void CLoopMgr::ExecReserveLoop( CLoopReserve* pReserve ){
	CLoop* pLoopCallFrom = pReserve->getLoop();
	CLoop* pLoopParent = NULL;

	// 呼び出し元が有効であればIDとその親の取り出し
	if( pLoopCallFrom != NULL ){
		pLoopParent = pLoopCallFrom->getParent();
	}

	switch( pReserve->getReserveType() ){
	// 予約元の処理を終了／親がいれば起こす
	case eLOOP_RESERVE_EXIT:
		pLoopCallFrom->onExit();
		if( pLoopParent != NULL ){
			pLoopParent->onAwake();
		}
		break;

	// 指定されたIDのループを作成
	case eLOOP_RESERVE_CREATE:
		CreateLoop( pReserve->getLoopId(), pReserve->getOption(), NULL );
		break;

	// 予約元の処理を終了／指定されたIDのループを作成
	case eLOOP_RESERVE_JUMP:
		pLoopCallFrom->onExit();
		CreateLoop( pReserve->getLoopId(), pReserve->getOption(), pLoopParent );
		break;

	// 予約元の処理を停止／指定されたIDのループを作成
	case eLOOP_RESERVE_CALL:
		if( pLoopCallFrom != NULL ){
			pLoopCallFrom->onSleep();
		}
		CreateLoop( pReserve->getLoopId(), pReserve->getOption(), pLoopCallFrom );
		break;

	default:
		break;
	}
}

//------------------------------
// 新規ループタスク予約の取得
//------------------------------
CLoopTaskReserve* CLoopMgr::GetNewLoopTaskReserve( void ){
    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );

    CLoopTaskReserve* pNew = new CLoopTaskReserve();

    CMemMgr::PopTargetField();

	s_pListReserveLoopTask->add( pNew );

	return( pNew );
}

//---------------------
// ループタスク開始
//---------------------
bool CLoopMgr::CheckLoopTaskStart( CLoopTaskReserve* pReserve ){
	switch( pReserve->getTask() ){
	case eLOOP_TASK_RESERVE_DIALOG:
		CDialogHelper::CallDialog( pReserve->getParamAt( 0 ),
                                   pReserve->getParamAt( 1 ),
                                   pReserve->getParamAt( 2 ),
                                   NULL,                        // [Neutral]は指定しない
                                   pReserve->getParamAt( 3 )
                                 );
        break;

	case eLOOP_TASK_RESERVE_DIALOG_INPUT:
		CDialogHelper::CallDialogInputString( pReserve->getParamAt( 0 ),
                                              pReserve->getParamAt( 1 ),
                                              pReserve->getParamAt( 2 ),
                                              NULL,                         // [Neutral]は指定しない
                                              pReserve->getParamAt( 3 ),
                                              pReserve->getParamAt( 4 )
                                            );
        break;

	default:
        return( false );
	}

    // 呼び出し元が有効ならコールバックを呼んでおく
    CLoop* pLoopCallFrom = pReserve->getLoop();
    if( pLoopCallFrom != NULL ){
        pLoopCallFrom->onTaskStarted( pReserve->getTaskId() );
    }

    return( true );
}

//-------------------
// タスク終了待ち
//-------------------
bool CLoopMgr::CheckLoopTaskFinish( CLoopTaskReserve* pReserve ){
	switch( pReserve->getTask() ){
	case eLOOP_TASK_RESERVE_DIALOG:
	case eLOOP_TASK_RESERVE_DIALOG_INPUT:
		if( ! CDialogHelper::IsBusy() ){
			CLoop* pLoopCallFrom = pReserve->getLoop();
			if( pLoopCallFrom != NULL ){
				bool isPositive = CDialogHelper::IsSelectedPositive();
				char* buf = CMemMgr::GetBufFromTempStr();
				uint32 size = CMemMgr::GetSizeFromTempStr();
				CDialogHelper::GetInputString( buf, size );

				// ループへ通知
				pLoopCallFrom->onTaskFinished( pReserve->getTaskId(), isPositive, 0, buf );
			}
			return( true );
		}
		return( false );

	default:
		break;
	}

	// ここまできたら無効（※終了済み扱い）
	return( true );
}
