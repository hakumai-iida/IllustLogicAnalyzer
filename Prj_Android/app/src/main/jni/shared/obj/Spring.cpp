/*+----------------------------------------------------------------+
  |	Title:		Spring.cpp [共通環境]
  |	Comment:	バネの移動系
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Spring.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// コンストラクタ
//------------------------
CSpring::CSpring( void ){
	clearSpring();
}

//------------------------
// クリア
//------------------------
void CSpring::clearSpring(){
	m_fLength = 0.0f;
	m_fStiffness = 0.0f;
	m_fDamping = 0.0f;
    m_fRotation = 0.0f;
    
    m_bAutoStop = false;
	m_fBorderV = 0.0f;
	m_fBorderDist = 0.0f;

    m_bMoving = false;
    m_bUpdateSkip = false;
	m_fPos = 0.0f;
	m_fV = 0.0f;
}

//------------------------
// 設定
//------------------------
void CSpring::set( float len, float stiff, float damp, float rot ){
	m_fLength = len;
	m_fStiffness = stiff;
	m_fDamping = damp;
    m_fRotation = rot;

	// デフォルトは自動で停止
	m_bAutoStop = true;
	m_fBorderDist = 0.5f;
	m_fBorderV = 0.5f;

	// 設定直後は動いていない
    m_bMoving = false;
	m_fPos = m_fLength;
	m_fV = 0.0f;
}

//--------------------
// 開始
//--------------------
void CSpring::start( void ){
    m_bMoving = true;
}

//--------------------
// 停止
//--------------------
void CSpring::stop( bool isPosReset ){
    m_bMoving = false;
    if( isPosReset ){
        m_fV = 0.0f;
        m_fPos = m_fLength;
    }
}

//------------------------------------------------
// 差分取得（※中央[m_fLength]を０とした時の位置）
//------------------------------------------------
float CSpring::getPosDelta( void ){
	float delta = m_fPos - m_fLength;
	return( delta );
}

//------------------------------------------------
// 差分取得：X
//------------------------------------------------
float CSpring::getX( void ){
    float pos = getPosDelta();
    return( pos * CMath::RotX( m_fRotation ));
}

//------------------------------------------------
// 差分取得：Y
//------------------------------------------------
float CSpring::getY( void ){
    float pos = getPosDelta();
    return( pos * CMath::RotY( m_fRotation ) );
}

//-----------------------------------------------------
// 収束したか？（※ある一定以上の移動におさまったか？）
//-----------------------------------------------------
bool CSpring::isConvergenced( void ){
    float dist = getPosDelta();
	if( dist < 0.0f ){ dist *= -1.0f; }

	float v = m_fV;
	if( v < 0.0f ){ v *= -1.0f; }

	return( dist < m_fBorderDist && v < m_fBorderV );
}

//-----------------------------------------------------
// 速度合成（※現在の状況に指定の速度を加える）
//-----------------------------------------------------
void CSpring::addV( float vX, float vY ){
    // 無効は無視
    if( m_fLength < 0.0f ){
        return;
    }

    // 無効は無視
    if( vX == 0.0f && vY == 0.0f ){
        return;
    }

    float dist0 = getPosDelta();
    
    // 現在の位置
    float curX = getX();
    float curY = getY();
    //LOGD( "@ [BEFORE] m_fRotation=%f, m_fPos=%f, curX=%f, curY=%f\n", m_fRotation, m_fPos, curX, curY );

    // 移動値を適用（※バネが伸縮しているほど影響が少ない）
    float temp = dist0;
    if( temp < 0 ){ temp *= -1; }
    if( temp < m_fLength ){
        temp = (m_fLength-temp)/m_fLength;
        curX += vX*temp;
        curY += vY*temp;
    }
    
    if( dist0 < 0 ){
        m_fRotation = CMath::GetDeg( curX, curY );
        m_fPos = m_fLength - CMath::Len( curX, curY );
    }else{
        m_fRotation = CMath::GetDeg( -curX, -curY );
        m_fPos = m_fLength + CMath::Len( curX, curY );
    }
    //LOGD( "@ [AFTER] m_fRotation=%f, m_fPos=%f, curX=%f, curY=%f\n", m_fRotation, m_fPos, curX, curY );

    // 更新をスキップしてみる
    start();
    m_bUpdateSkip = true;
}

//-----------------------------------------------------
// 更新
//-----------------------------------------------------
void CSpring::updateWithFrame( float deltaFrame ){
    // 更新を無視する
    if( m_bUpdateSkip ){
        m_bUpdateSkip = false;
        return;
    }

	// 移動中でなければ無視
	if( ! m_bMoving ){
		return;
	}
    
	// 速度の減衰
    float damping = deltaFrame * m_fDamping + (1.0f - deltaFrame);
	m_fV *= damping;

	// 位置の更新
	m_fPos += deltaFrame * m_fV;

	// 加速度の発生
	float a = (m_fLength - m_fPos) * m_fStiffness;
	m_fV += deltaFrame * a;
    
   	// 停止判定
   	if( m_bAutoStop ){
		if( isConvergenced() ){
			stop( true );
   		}
   	}
}
