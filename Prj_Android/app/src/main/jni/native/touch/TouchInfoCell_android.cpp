/*+----------------------------------------------------------------+
  |	Title:		TouchInfoCell_android.cpp [Android環境]
  |	Comment:	システムから受け取るタッチ情報要素
  |             （※生タッチイベントの１要素：デバイス環境⇔マネージャ間の情報）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "touch/TouchInfoCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 判定許容範囲
#define MAX_ID	32

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// [共通] コンストラクタ
//------------------------
CTouchInfoCell::CTouchInfoCell( void ){
	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_BASE );
	m_nArrId = new int[MAX_ID];
	m_fArrX = new float[MAX_ID];
	m_fArrY = new float[MAX_ID];
	CMemMgr::PopTargetField();
}

//------------------------
// [共通] デストラクタ
//------------------------
CTouchInfoCell::~CTouchInfoCell( void ){
	SAFE_DELETE_ARRAY( m_nArrId );
	SAFE_DELETE_ARRAY( m_fArrX );
	SAFE_DELETE_ARRAY( m_fArrY );
}

//--------------------------
// [Android] データ設定
//--------------------------
void CTouchInfoCell::setMotionEvent( stTOUCH_EVENT_DESCRIPTOR* pTED ){
	AInputEvent* pEvent = pTED->pInputEvent;

	m_nAction = AMotionEvent_getAction( pEvent );
	m_nLength = AMotionEvent_getPointerCount( pEvent );

	// ヒストリは無視して最後の座標のみ保持
	for( int i=0; i<m_nLength; i++ ){
		m_nArrId[i] = AMotionEvent_getPointerId( pEvent, (size_t)i );
		m_fArrX[i] = AMotionEvent_getX( pEvent, (size_t)i );
		m_fArrY[i] = AMotionEvent_getY( pEvent, (size_t)i );
	}
}

//---------------------
// [Android] ログ出力
//---------------------
void CTouchInfoCell::DumpLog( stTOUCH_EVENT_DESCRIPTOR* pTED ){
	AInputEvent* pEvent = pTED->pInputEvent;

	// タッチイベントでなければアクション情報のみ表示して返す
	int action = AInputEvent_getType( pEvent );
	if( action != AINPUT_EVENT_TYPE_MOTION ){
		LOGD( "@ TouchLog:*Invalid Action: %d\n", action );
		return;
	}

	int historySize = AMotionEvent_getHistorySize( pEvent );
	int pointerCount = AMotionEvent_getPointerCount( pEvent );

	LOGD( "@ TouchLog:*Valid Action: %d (history=%d, pointer=%d)\n", action, historySize, pointerCount );

	for( int h=0; h<historySize; h++ ){
		LOGD( "@ TouchLog: History Time[%d]: %d\n", h, (int)AMotionEvent_getHistoricalEventTime( pEvent, (size_t)h ) );
		for( int p=0; p<pointerCount; p++ ){
			LOGD( "@ TouchLog: History Point[%d.%d] x=%f, y=%f\n", h, AMotionEvent_getPointerId( pEvent, (size_t)p ),
			 AMotionEvent_getHistoricalX( pEvent, (size_t)p, (size_t)h ), AMotionEvent_getHistoricalY( pEvent, (size_t)p, (size_t)h ) );
		}
	}

	LOGD( "@ TouchLog: Latest Time[%d]: %d\n", historySize, (int)AMotionEvent_getEventTime( pEvent ) );
	for( int p=0; p<pointerCount; p++ ){
		LOGD( "@ TouchLog: Latest Point[%d.%d]: x=%f, y=%f\n",
				historySize, AMotionEvent_getPointerId( pEvent, (size_t)p ),
				AMotionEvent_getX( pEvent, (size_t)p ), AMotionEvent_getY( pEvent, (size_t)p ) );
	}
}
