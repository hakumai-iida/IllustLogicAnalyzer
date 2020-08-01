/*+----------------------------------------------------------------+
  |	Title:		DelayLog.cpp [共通環境]
  | Comment:    遅延ログ（※遅延調整で利用する移動差分ワーク：実体はバネ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DelayLog.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// バネの長さ（とりあえず）
#define DELAY_LOG_SPRING_LENGTH     100
#define DELAY_LOG_POS_RATE           50

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
const char* g_pArrLabelDelayLog[] = {
    "CLOTH",    // 布
    "HAIR",     // 髪
    "SKIN",     // 肌
    "BUST",     // 胸（※肌よりも柔らかいなびき＆高反発）

    NULL
};

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------------
// コンストラクタ
//-------------------------
CDelayLog::CDelayLog( void ){
    clear();
}

//-------------------------
// クリア
//-------------------------
void CDelayLog::clear( void ){
    // クリア
    for( int i=0; i<eDELAY_LOG_MAX; i++ ){
        m_oArrSpring[i].clearSpring();
    }
    
    //-----------------------------------------------
    // デフォルト値：（とりあえず）
    //-----------------------------------------------
    m_oArrSpring[eDELAY_LOG_CLOTH].set( DELAY_LOG_SPRING_LENGTH, 0.10f, 0.80f );     // 布
    m_oArrSpring[eDELAY_LOG_HAIR].set(  DELAY_LOG_SPRING_LENGTH, 0.05f, 0.95f );     // 髪
    m_oArrSpring[eDELAY_LOG_SKIN].set(  DELAY_LOG_SPRING_LENGTH, 0.60f, 0.70f );     // 肌
    m_oArrSpring[eDELAY_LOG_BUST].set(  DELAY_LOG_SPRING_LENGTH, 0.40f, 0.85f );     // 胸

    // リセット
    reset();
}

//-------------------------
// リセット
//-------------------------
void CDelayLog::reset( void ){
    for( int i=0; i<eDELAY_LOG_MAX; i++ ){
        m_oArrSpring[i].stop( true );
        
        for( int j=0; j<DELAY_LOG_DEPTH; j++ ){
            m_nArrArrLogX[i][j] = 0;
            m_nArrArrLogY[i][j] = 0;
        }
    }
}

//-------------------------
// 更新
//-------------------------
void CDelayLog::updateWithFrame( float deltaFrame ){
    for( int i=0; i<eDELAY_LOG_MAX; i++ ){
        // 更新前の値を退避
        m_nArrArrLogX[i][0] = m_oArrSpring[i].getX();
        m_nArrArrLogY[i][0] = m_oArrSpring[i].getY();

        // バネの更新
        m_oArrSpring[i].updateWithFrame( deltaFrame );
        
        // ログの更新
        for( int j=DELAY_LOG_DEPTH-1; j>0; j-- ){
            m_nArrArrLogX[i][j] = m_nArrArrLogX[i][j-1];
            m_nArrArrLogY[i][j] = m_nArrArrLogY[i][j-1];
        }
    }
}

//-------------------------
// 揺れ
//-------------------------
void CDelayLog::shake( eDELAY_LOG type, float v, float rot ){
    if( IS_DELAY_LOG_VALID( type ) ){
        m_oArrSpring[type].setV( v );
        m_oArrSpring[type].setRotation( rot );
        m_oArrSpring[type].start();
    }
}

//-------------------------
// 引っ張り
//-------------------------
void CDelayLog::pull( eDELAY_LOG type, float vX, float vY ){
    if( IS_DELAY_LOG_VALID( type ) ){
        m_oArrSpring[type].addV( vX, vY );
    }
}

//-------------------------
// 取得：X座標
//-------------------------
int CDelayLog::getDelayRateX( eDELAY_LOG type, int depth ){
    float x = 0;

    if( IS_DELAY_LOG_VALID( type ) ){
        if( depth < 0 ){
            depth = 0;
        }else if( depth >= DELAY_LOG_DEPTH ){
            depth = DELAY_LOG_DEPTH - 1;
        }

        // 最新の指定であれば算出
        if( depth == 0 ){
            x = m_oArrSpring[type].getX();
        }else{
            x = m_nArrArrLogX[type][depth];
        }
    }
    
    return( (int)(DELAY_LOG_POS_RATE*x) );
}

//-------------------------
// 取得：Y座標
//-------------------------
int CDelayLog::getDelayRateY( eDELAY_LOG type, int depth ){
    float y = 0;

    if( IS_DELAY_LOG_VALID( type ) ){
        if( depth < 0 ){
            depth = 0;
        }else if( depth >= DELAY_LOG_DEPTH ){
            depth = DELAY_LOG_DEPTH - 1;
        }

        // 最新の指定であれば算出
        if( depth == 0 ){
            y = m_oArrSpring[type].getY();
        }else{
            y = m_nArrArrLogY[type][depth];
        }
    }
    
    return( (int)(DELAY_LOG_POS_RATE*y) );
}
