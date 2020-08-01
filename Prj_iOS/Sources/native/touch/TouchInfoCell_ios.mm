/*+----------------------------------------------------------------+
  |	Title:		TouchInfoCell_ios.mm [iOS環境]
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
// 判定許容範囲（※同時に保持できる状況検出数）
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
	m_nAction = 0;
	m_nLength = 0;

	// データ確保（※確保先は呼び出し元に任せる）
	m_nArrId = new int[MAX_ID];
	m_fArrX = new float[MAX_ID];
	m_fArrY = new float[MAX_ID];
}

//------------------------
// [共通] デストラクタ
//------------------------
CTouchInfoCell::~CTouchInfoCell( void ){
	SAFE_DELETE_ARRAY( m_nArrId );
	SAFE_DELETE_ARRAY( m_fArrX );
	SAFE_DELETE_ARRAY( m_fArrY );
}

//---------------------
// [iOS] データ設定
//---------------------
void CTouchInfoCell::setMotionEvent( stTOUCH_EVENT_DESCRIPTOR* pTED ){
	UIEvent* pEvent = pTED->pEvent;

	m_nAction = pTED->nEventType;
	m_nLength = 0;

    // 状況に合致したイベントのみを取り出す
    NSSet* pSets = [pEvent allTouches];
    for( UITouch* pTouch in pSets ){
        if( m_nAction == pTouch.phase ){
            CGPoint point = [pTouch locationInView:g_stEnvNative.pView];
            m_nArrId[m_nLength] = (int)pTouch.hash;
            m_fArrX[m_nLength] = point.x;
            m_fArrY[m_nLength] = point.y;
            m_nLength++;
        }
    }
}

//----------------
// [iOS] ログ出力
//----------------
void CTouchInfoCell::DumpLog( stTOUCH_EVENT_DESCRIPTOR* pTED ){
    UIEvent* pEvent = pTED->pEvent;
    
    LOGD( "@ TouchLog:*Action: %d\n", pTED->nEventType );
    
    int id = 0;
    NSSet* pSets = [pEvent allTouches];
	for( UITouch* pTouch in pSets ){
        CGPoint point = [pTouch locationInView:g_stEnvNative.pView];
		LOGD( "@ TouchLog:Point[%d](%d) phase=%d pos={ %.1f, %.1f}\n", id, (int)pTouch.hash, (int)pTouch.phase, point.x, point.y );
        id++;
	}
}
