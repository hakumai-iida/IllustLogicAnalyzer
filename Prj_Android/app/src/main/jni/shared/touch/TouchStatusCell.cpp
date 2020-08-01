/*+----------------------------------------------------------------+
  |	Title:		TouchStatusCell.hpp [共通環境]
  |	Comment:	タッチ個々の状態要素（※管理環境における指一本の情報）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchStatusCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define STABLE_CANCEL_DIST		 4
#define FLICK_CHECK_DIST		20

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
// コンストラクタ
//------------------------
CTouchStatusCell::CTouchStatusCell( void ){
	clear();
}

//-------------------
// クリア
//-------------------
void CTouchStatusCell::clear( void ){
    m_nId = -1;
    
    m_bActive = false;
    m_bTapped = false;
    m_bPressed = false;
    m_bTouchedDown = false;
    m_bTouchedUp = false;
    m_bMoved = false;
    m_bFlicked = false;
    
    m_nTapTimer = 0;
    m_nPressTimer = 0;
    
    clearTemp();
    clearLogMoveXY();
}

//-------------------
// 更新
//-------------------
void CTouchStatusCell::update( void ){
	// 各種クリア
	m_bTapped = false;
	m_bTouchedDown = false;
	m_bTouchedUp = false;
	m_bMoved = false;
	m_bFlicked = false;

	// 前のフレームで押下であればこのフレームは活動中となる（※同フレーム内の押し下～押し離しでも立てておく）
	m_bActive = m_bPressed || m_bTempTouchedDownUp;

	m_fMoveX = 0.0f;
	m_fMoveY = 0.0f;
	m_fFlickX = 0.0f;
	m_fFlickY = 0.0f;

	//----------------------
	// 押下判定
	//----------------------
	if( m_bTempTouchedDown || m_bTempTouchedDownUp ){
		m_bTouchedDown = true;
		m_bPressed = true;
		m_bStable = true;
		m_nTapTimer = TOUCH_STATUS_CELL_TAP_CHECK_FRAME;
		m_nPressTimer = 0;

		// 位置リセット
		m_fBaseX = m_fX0 = m_fX = m_fTempX;
		m_fBaseY = m_fY0 = m_fY = m_fTempY;
	}

	//----------------------
	// 移動判定
	//----------------------
	if( m_bTempMoved ){
		m_fX = m_fTempX;
		m_fY = m_fTempY;

		// 移動確認
		if( m_fX != m_fX0 || m_fY != m_fY0 ){
			m_bMoved = true;
			m_fMoveX = m_fX - m_fX0;
			m_fMoveY = m_fY - m_fY0;
			m_fX0 = m_fX;
			m_fY0 = m_fY;

			// 安定状態であれば解除判定（※一定以上移動／一定時間経過したか？）
			if( m_bStable ){
				// 安定確認
				float dX = m_fX - m_fBaseX;
				float dY = m_fY - m_fBaseY;
                float dist = CMath::Len( dX, dY );
                
				if( dist >= STABLE_CANCEL_DIST ){
					m_bStable = false;
				}
			}
		}
	}

	// 移動値履歴
	pushLogMoveXY( m_fMoveX, m_fMoveY );

	//----------------------
	// 押上判定
	//----------------------
	if( m_bTempTouchedUp ){
		m_bTouchedUp = true;
		m_bPressed = false;

		// 受付時間内＆安定状態であればタップ
		if( m_nTapTimer > 0 && m_bStable ){
			m_bTapped = true;
		}else{
			// 一定以上移動していたらフリック
			float mX = getLogMoveX();
			float mY = getLogMoveY();
            float dist = CMath::Len( mX, mY );

			if( dist > FLICK_CHECK_DIST ){
				m_bFlicked = true;
				m_fFlickX = mX;
				m_fFlickY = mY;
			}
		}

		// 各種クリア
		m_nTapTimer = 0;
		m_nPressTimer = 0;
		clearLogMoveXY();

        // IDはここでクリア
        m_nId = -1;
	}

	// 押されていたら
	if( m_bPressed ){
		m_bActive = true;

		// タップカウンタ更新
		if( m_nTapTimer > 0 ){
			m_nTapTimer--;
		}

		m_nPressTimer++;
	}

	//-----------------
	// MOVE
	if( m_bMoved ){
        //LOGD( "@ TOUCH: MOVE:%d\n", m_nId );
	}

	// up
	if( m_bTouchedUp ){
        //LOGD( "@ TOUCH: UP:%d\n", m_nId );
	}

	// down
	if( m_bTouchedDown ){
        //LOGD( "@ TOUCH: DOWN:%d\n", m_nId );
	}

	// flick
	if( m_bFlicked ){
        //LOGD( "@ TOUCH: FLICKED:%d\n", m_nId );
	}

	// tap
	if( m_bTapped ){
        //LOGD( "@ TOUCH: TAPPED:%d\n", m_nId );
	}

	clearTemp();
}

//-------------------
// テンポラリクリア
//-------------------
void CTouchStatusCell::clearTemp( void ){
	m_bTempMoved = false;
	m_bTempTouchedDown = false;
	m_bTempTouchedUp = false;
	m_bTempTouchedDownUp = false;
	m_fTempX = m_fTempY = 0.0f;
}

//-------------------
// 移動履歴クリア
//-------------------
void CTouchStatusCell::clearLogMoveXY( void ){
	for( int i=0; i<TOUCH_STATUS_CELL_MOVE_LOG_NUM; i++ ){
		m_fArrLogMoveX[i] = 0.0f;
		m_fArrLogMoveY[i] = 0.0f;
	}
}

//-------------------
// 移動履歴
//-------------------
void CTouchStatusCell::pushLogMoveXY( float x, float y ){
	for( int i=TOUCH_STATUS_CELL_MOVE_LOG_NUM-1; i>0; i-- ){
		m_fArrLogMoveX[i] = m_fArrLogMoveX[i-1];
		m_fArrLogMoveY[i] = m_fArrLogMoveY[i-1];
	}
	m_fArrLogMoveX[0] = x;
	m_fArrLogMoveY[0] = y;
}

//-------------------
// フリック値：Ｘ
//-------------------
float CTouchStatusCell::getLogMoveX( void ){
	float x = 0.0f;
	for( int i=0; i<TOUCH_STATUS_CELL_MOVE_LOG_NUM; i++ ){
		x += m_fArrLogMoveX[i];
	}
	return( x );
}

//-------------------
// フリック値：Ｙ
//-------------------
float CTouchStatusCell::getLogMoveY( void ){
	float y = 0.0f;
	for( int i=0; i<TOUCH_STATUS_CELL_MOVE_LOG_NUM; i++ ){
		y += m_fArrLogMoveY[i];
	}
	return( y );
}

//-----------------------
// イベント受け取り：押された
//-----------------------
void CTouchStatusCell::onTouchedDown( float x, float y ){
	m_bTempTouchedDown = true;
	m_fTempX = x;
	m_fTempY = y;

	// とりあえず排他
	if( m_bTempTouchedUp ){
		m_bTempTouchedUp = false;
	}
}

//-----------------------
// イベント受け取り：離された
//-----------------------
void CTouchStatusCell::onTouchedUp( float x, float y ){
	m_bTempTouchedUp = true;
	m_fTempX = x;
	m_fTempY = y;

	// タップ／フリック判定用に移動フラグを立てておく
	m_bTempMoved = true;

	// とりあえず排他
	if( m_bTempTouchedDown ){
		m_bTempTouchedDown = false;

		// 同一フレーム内のタップとして判定する
		m_bTempTouchedDownUp = true;
	}
}

//-----------------------
// イベント受け取り：移動した
//-----------------------
void CTouchStatusCell::onMoved( float x, float y ){
	m_bTempMoved = true;
	m_fTempX = x;
	m_fTempY = y;
}

//--------------------------------
// 判定：現ポイントが矩形に含まれるか？
//--------------------------------
bool CTouchStatusCell::isCurPointInRect( float x, float y, float w, float h ){
	return( m_fX >= x && m_fX <= (x+w) && m_fY >= y && m_fY <= (y+h) );
}

//--------------------------------
// 判定：開始ポイントが矩形に含まれるか？
//--------------------------------
bool CTouchStatusCell::isStartPointInRect( float x, float y, float w, float h ){
	return( m_fBaseX >= x && m_fBaseX <= (x+w) && m_fBaseY >= y && m_fBaseY <= (y+h) );
}

//--------------------------------
// 判定：現ポイントが円に含まれるか？
//--------------------------------
bool CTouchStatusCell::isCurPointInOval( float x, float y, float w, float h ){
    float a = w/2.0f;
    float b = h/2.0f;
    if( a <= 0.0f || b <= 0.0f){
        return( false );
    }

    x += a - m_fX;
    y += b - m_fY;
    
    float temp = (x*x)/(a*a) + (y*y)/(b*b);
    return( temp <= 1.0f );
}

//--------------------------------
// 判定：開始ポイントが円に含まれるか？
//--------------------------------
bool CTouchStatusCell::isStartPointInOval( float x, float y, float w, float h ){
    float a = w/2.0f;
    float b = h/2.0f;
    if( a <= 0.0f || b <= 0.0f){
        return( false );
    }
    
    x += a - m_fBaseX;
    y += b - m_fBaseY;
    
    float temp = (x*x)/(a*a) + (y*y)/(b*b);
    return( temp <= 1.0f );
}
