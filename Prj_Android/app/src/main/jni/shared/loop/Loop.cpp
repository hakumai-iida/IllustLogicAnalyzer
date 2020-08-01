/*+----------------------------------------------------------------+
  |	Title:		Loop.cpp [共通環境]
  |	Comment:	ループ基底
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Loop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------
// コンストラクタ
//-----------------------
CLoop::CLoop( void ): CListNode(), CDrawer(), CUpdater(){
	// ループはオリジナルのデルタ時間で動作する
	setUpdateWithOriginalDeltaFrame( true );

	// 無効値
	m_nId = eLOOP_ID_INVALID;
	m_nStep = eLOOP_STEP_INVALID;
	m_nStep0 = eLOOP_STEP_INVALID;
	m_nStepAtAwake = eLOOP_STEP_INVALID;
	m_nStepAtResume = eLOOP_STEP_INVALID;
}

//-----------------------
// デストラクタ
//-----------------------
CLoop::~CLoop( void ){
	// ノード解放
	m_oListNode.deleteNodeAll();
}

//-----------------------
// 初期化
//-----------------------
void CLoop::onInit( CLoop* pParent, int option ){
	m_pParent = pParent;
	m_nOption = option;

	// 開始予約（※必要であれば[init0]内で上書き）
	setStep( eLOOP_STEP_START );

	// 初期化内で確保するデフォルトメモリ領域はAPP領域としておく
	CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
	init0();
	CMemMgr::PopTargetField();
}

//-----------------------
// 終了
//-----------------------
void CLoop::onExit( void ){
	// 終了予約（※必要であれば[exit0]内で上書き）
	setStep( eLOOP_STEP_FINISHED );

	exit0();
}

//-----------------------
// 更新
//-----------------------
void CLoop::onUpdate( void ){
	// 終了済みは無視
	if( isFinished() ){ return; }

	// ノード更新
	CNode* pNode = (CNode*) m_oListNode.getHead();
	while( pNode != NULL ){
		pNode->onUpdate();
		pNode = pNode->getNext();
	}

    // 自身の更新
	update();
}

//-----------------------
// 描画登録
//-----------------------
void CLoop::onRegistForDraw( void ){
	// 終了済みは無視
	if( isFinished() ){ return; }

	// 自身を登録
	registForDraw();

	// ループに含まれる要素を登録
	registForDraw0();
}

//-----------------------
// 描画：UI
//-----------------------
void CLoop::onDrawNode( void ){
	// 終了済みは無視
	if( isFinished() ){ return; }

    drawUI0();

    // ノード表示
	CNode* pNode = (CNode*) m_oListNode.getHead();
	while( pNode != NULL ){
		pNode->onDraw();
		pNode = pNode->getNext();
	}
}

//-----------------------
// 描画：ログ
//-----------------------
void CLoop::onDrawLog( void ){
	// 終了済みは無視
	if( isFinished() ){ return; }

	drawLog0();
}

//------------------------------------
// スリープ
//------------------------------------
void CLoop::onSleep( void ){
	// 終了済み／スリープ済みは無視
	if( isFinished() || isSleep() ){ return; }

	m_nStepAtAwake = m_nStep;
	setStep( eLOOP_STEP_SLEEP );

	sleep0();
}

//------------------------------------
// アウェイク
//------------------------------------
void CLoop::onAwake( void ){
	// 終了済みは／アウェイク済みは無視
	if( isFinished() || !isSleep() ){ return; }

	// ステップを戻す（※必要があれば[awake0]内で上書き）
	setStep( m_nStepAtAwake );

	awake0();
}

//-----------------------
// サスペンド
//-----------------------
void CLoop::onSuspend( void ){
	// 終了済み／中断済みは無視
	if( isFinished() || isSuspend() ){ return; }

	m_nStepAtResume = m_nStep;
	setStep( eLOOP_STEP_SUSPEND );

	suspend0();
}

//-----------------------
// リジューム
//-----------------------
void CLoop::onResume( void ){
	// 終了済み／再開済みは無視
	if( isFinished() || !isSuspend() ){ return; }

	// ステップを戻す（※必要があれば[resume0]内で上書き）
	setStep( m_nStepAtResume );

	resume0();
}

//-----------------------
// タスクが開始した
//-----------------------
void CLoop::onTaskStarted( int taskId ){
	// 終了済みは無視
	if( isFinished() ){ return; }

	taskStarted0( taskId );
}

//-----------------------
// タスクが終了した
//-----------------------
void CLoop::onTaskFinished( int taskId, bool isSuccess, int code, const char* pResponse ){
	// 終了済みは無視
	if( isFinished() ){ return; }

	taskFinished0( taskId, isSuccess, code, pResponse );
}

//-----------------------
// ノード追加
//-----------------------
void CLoop::addNode( CNode* pNode ){
	m_oListNode.add( pNode );
}

//-----------------------
// ノード一括設定:表示
//-----------------------
void CLoop::setVisibleNodeAll( bool flag ){
	CNode* pNode = (CNode*) m_oListNode.getHead();
	while( pNode != NULL ){
		pNode->setVisible( flag );
		pNode = pNode->getNext();
	}
}

//-----------------------
// ノード一括設定：活動
//-----------------------
void CLoop::setActiveNodeAll( bool flag ){
	CNode* pNode = (CNode*) m_oListNode.getHead();
	while( pNode != NULL ){
		pNode->setActive( flag );
		pNode = pNode->getNext();
	}
}

//-----------------------
// ノード一括設定：暗転
//-----------------------
void CLoop::setDarkNodeAll( bool flag ){
    CNode* pNode = (CNode*) m_oListNode.getHead();
    while( pNode != NULL ){
        pNode->setDark( flag );
        pNode = pNode->getNext();
    }
}

//-----------------------
// ノード一括設定：入場
//-----------------------
void CLoop::enterNodeAll( void ){
	CNode* pNode = (CNode*) m_oListNode.getHead();
	while( pNode != NULL ){
		pNode->enter();
		pNode = pNode->getNext();
	}
}

//-----------------------
// ノード一括設定：退場
//-----------------------
void CLoop::exitNodeAll( void ){
	CNode* pNode = (CNode*) m_oListNode.getHead();
	while( pNode != NULL ){
		pNode->exit();
		pNode = pNode->getNext();
	}
}

//-----------------------
// レイアウト設定：基本
//-----------------------
void CLoop::setLayoutBase( float x, float y, int alignX, int alignY, int dirX, int dirY ){
	m_nLayoutCurX = x;
	m_nLayoutCurY = y;
	m_nLayoutAlignX = alignX;
	m_nLayoutAlignY = alignY;
	m_nLayoutDirX = dirX;
	m_nLayoutDirY = dirY;
}

//-----------------------
// レイアウト設定：左上
//-----------------------
void CLoop::setLayoutLT( float ofsX, float ofsY, int dirX, int dirY  ){
	setLayoutBase( LAYOUT_X_L+ofsX, LAYOUT_Y_T+ofsY, eDRAW_ALIGN_X_L, eDRAW_ALIGN_Y_T, dirX, dirY );
}

//-----------------------
// レイアウト設定：中上
//-----------------------
void CLoop::setLayoutCT( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_C+ofsX, LAYOUT_Y_T+ofsY, eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_T, dirX, dirY );
}

//-----------------------
// レイアウト設定：右上
//-----------------------
void CLoop::setLayoutRT( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_R+ofsX, LAYOUT_Y_T+ofsY, eDRAW_ALIGN_X_R, eDRAW_ALIGN_Y_T, dirX, dirY );
}

//-----------------------
// レイアウト設定：左中
//-----------------------
void CLoop::setLayoutLM( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_L+ofsX, LAYOUT_Y_M+ofsY, eDRAW_ALIGN_X_L, eDRAW_ALIGN_Y_M, dirX, dirY );
}

//-----------------------
// レイアウト設定：中中
//-----------------------
void CLoop::setLayoutCM( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_C+ofsX, LAYOUT_Y_M+ofsY, eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M, dirX, dirY );
}

//-----------------------
// レイアウト設定：右中
//-----------------------
void CLoop::setLayoutRM( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_R+ofsX, LAYOUT_Y_M+ofsY, eDRAW_ALIGN_X_R, eDRAW_ALIGN_Y_M, dirX, dirY );
}

//-----------------------
// レイアウト設定：左下
//-----------------------
void CLoop::setLayoutLB( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_L+ofsX, LAYOUT_Y_B+ofsY, eDRAW_ALIGN_X_L, eDRAW_ALIGN_Y_B, dirX, dirY );
}

//-----------------------
// レイアウト設定：中下
//-----------------------
void CLoop::setLayoutCB( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_C+ofsX, LAYOUT_Y_B+ofsY, eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_B, dirX, dirY );
}

//-----------------------
// レイアウト設定：右下
//-----------------------
void CLoop::setLayoutRB( float ofsX, float ofsY, int dirX, int dirY ){
	setLayoutBase( LAYOUT_X_R+ofsX, LAYOUT_Y_B+ofsY, eDRAW_ALIGN_X_R, eDRAW_ALIGN_Y_B, dirX, dirY );
}

//-----------------------
// ノードの配置
//-----------------------
void CLoop::putNodeOnLayout( CNode* pNode, float nextAdjX, float nextAdjY ){
	float x, y;
	float w = pNode->getW();
	float h = pNode->getH();

	// Ｘ座標
	switch( m_nLayoutAlignX ){
	case eDRAW_ALIGN_X_C:	x = m_nLayoutCurX - w/2;		break;
	case eDRAW_ALIGN_X_R:	x = m_nLayoutCurX - w;			break;
	default:				x = m_nLayoutCurX;				break;
	}

	// Ｘ座標更新
	switch( m_nLayoutDirX ){
	case eLAYOUT_DIR_F:		m_nLayoutCurX += (w+LAYOUT_MARGIN_X);	break;
	case eLAYOUT_DIR_B:		m_nLayoutCurX -= (w+LAYOUT_MARGIN_X);	break;
	default:														break;
	}

	// Ｙ座標
	switch( m_nLayoutAlignY ){
	case eDRAW_ALIGN_Y_M:	y = m_nLayoutCurY - h/2;		break;
	case eDRAW_ALIGN_Y_B:	y = m_nLayoutCurY - h;			break;
	default:				y = m_nLayoutCurY;		    	break;
	}

	// Ｙ座標更新
	switch( m_nLayoutDirY ){
	case eLAYOUT_DIR_F:		m_nLayoutCurY += (h+LAYOUT_MARGIN_Y);	break;
	case eLAYOUT_DIR_B:		m_nLayoutCurY -= (h+LAYOUT_MARGIN_Y);	break;
	default:														break;
	}

	// 配置
	pNode->put( x, y );

	// 配置後にオフセット調整
	switch( m_nLayoutDirX ){
	case eLAYOUT_DIR_B:     m_nLayoutCurX -= nextAdjX;	break;
    default:
    case eLAYOUT_DIR_F:     m_nLayoutCurX += nextAdjX;  break;
	}

	switch( m_nLayoutDirY ){
	case eLAYOUT_DIR_B:		m_nLayoutCurY -= nextAdjY;	break;
	default:
    case eLAYOUT_DIR_F:     m_nLayoutCurY += nextAdjY;  break;
	}
}
