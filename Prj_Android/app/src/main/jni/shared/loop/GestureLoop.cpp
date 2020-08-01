/*+----------------------------------------------------------------+
  |	Title:		GestureLoop.hpp [共通環境]
  |	Comment:	ジェスチャーループ（※スワイプ等による値の変動基本）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "GestureLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define PINCH_MOVE_LEN_BASE     24.0f
#define PINCH_MOVE_SCALE_BASE   1.0f
#define PINCH_SCALE_MAX         32.0f
#define PINCH_SCALE_MIN         1.0f

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
//------------------------
// 範囲設定
//------------------------
void CGestureLoop::setGestureBase( int x, int y, int r ){
	m_nGestureBaseX = x;
	m_nGestureBaseY = y;
	m_nGestureBaseR = r;
}

//------------------------
// 移動設定
//------------------------
void CGestureLoop::setGestureMoveXY( float x, float y ){
	m_fGestureMoveX = x;
	m_fGestureMoveY = y;
}

//--------------------------
// 移動チェック（※スワイプ）
//--------------------------
bool CGestureLoop::checkGestureMove( bool isUpdateSkip ){
    // ログのクリア
    m_fGestureLogX = 0.0f;
    m_fGestureLogY = 0.0f;

    // ピンチ中は無視
    if( m_bGesturePinch ){
        m_bGestureMove = false;
        return( false );
    }
    
	CTouchStatusCell* pCell;

    // 領域判定用（※タッチ判定は左上基準＆円が収まる矩形で指定）
    float checkW = 2*m_nGestureBaseR;
    float checkH = 2*m_nGestureBaseR;
    float checkX = m_nGestureBaseX - checkW/2.0f;
    float checkY = m_nGestureBaseY - checkH/2.0f;
	if( m_bGestureMove ){
		// 押され続けていたら移動判定
		pCell = CTouchMgr::CheckRect( eTOUCH_CHECK_PRESSED, 0, 0, SA_W, SA_H );
		if( pCell != NULL ){
			// 移動差分の検出
			pCell = CTouchMgr::CheckOval( eTOUCH_CHECK_MOVED, checkX, checkY, checkW, checkH );
			if( pCell != NULL ){
				m_fGestureLogX += pCell->getMoveX();
				m_fGestureLogY += pCell->getMoveY();
			}
		}else{
			m_bGestureMove = false;
		}
	}else{
		pCell = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, checkX, checkY, checkW, checkH );
		if( pCell != NULL ){
			m_bGestureMove = true;
		}
	}
    
    // 更新（※指定があればスキップ）
    if( ! isUpdateSkip ){
        m_fGestureMoveX += m_fGestureLogX;
        m_fGestureMoveY += m_fGestureLogY;
    }

	return( m_bGestureMove );
}

//------------------------
// スケール設定
//------------------------
void CGestureLoop::setGestureScale( float s ){
	m_fGestureScale = s;
}

//-----------------------------
// スケールチェック（※ピンチ）
//-----------------------------
bool CGestureLoop::checkGestureScale( bool isUpdateSkip ){
    // ログのクリア
    m_fGestureLogScale = 0.0f;
    
    if( m_bGesturePinch ){
        float len = 0.0f;

        // さらに大きめの範囲で開始判定（※タッチ判定は左上基準＆円が収まる矩形で指定）
        float checkW = 3.0f*(2*m_nGestureBaseR);
        float checkH = 3.0f*(2*m_nGestureBaseR);
        float checkX = m_nGestureBaseX - checkW/2.0f;
        float checkY = m_nGestureBaseY - checkH/2.0f;
        if( CTouchMgr::IsPinched( checkX, checkY, checkW, checkH, &len ) ){
            float div = len - m_fGesturePinchLen0;
            if( div >= PINCH_MOVE_LEN_BASE ){
                m_fGesturePinchLen0 += PINCH_MOVE_LEN_BASE;
                m_fGestureLogScale += PINCH_MOVE_SCALE_BASE;
            }else if( div <= -PINCH_MOVE_LEN_BASE ){
                m_fGesturePinchLen0 -= PINCH_MOVE_LEN_BASE;
                m_fGestureLogScale -= PINCH_MOVE_SCALE_BASE;
            }
        }else{
            m_bGesturePinch = false;
        }
    }else{
        // やや大きめの範囲で開始判定（※タッチ判定は左上基準＆円が収まる矩形で指定）
        float checkW = 1.5f*(2*m_nGestureBaseR);
        float checkH = 1.5f*(2*m_nGestureBaseR);
        float checkX = m_nGestureBaseX - checkW/2.0f;
        float checkY = m_nGestureBaseY - checkH/2.0f;
        if( CTouchMgr::IsPinched( checkX, checkY, checkW, checkH, &m_fGesturePinchLen0 ) ){
            m_bGesturePinch = true;
        }
    }

    // 更新（※指定があれば無視）
    if( ! isUpdateSkip ){
        m_fGestureScale += m_fGestureLogScale;
        if( m_fGestureScale > PINCH_SCALE_MAX ){
            m_fGestureScale = PINCH_SCALE_MAX;
        }else if( m_fGestureScale < PINCH_SCALE_MIN ){
            m_fGestureScale = PINCH_SCALE_MIN;
        }
    }

	return( m_bGesturePinch );
}

//------------------------
// ジェスチャー領域表示
//------------------------
void CGestureLoop::drawGestureBase( int32 rgba ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();

	pDC->clear();
	pDC->setImg( eIMG_DEBUG_CIRCLE );
	pDC->setRGBA( rgba );
	pDC->setBlendAlpha();
	pDC->setWidth( 2*m_nGestureBaseR );
	pDC->setHeight( 2*m_nGestureBaseR );
	pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
	pDC->draw( m_nGestureBaseX, m_nGestureBaseY );
}
