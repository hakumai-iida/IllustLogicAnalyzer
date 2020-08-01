/*+----------------------------------------------------------------+
  |	Title:		AdjustablePoint.cpp [共通環境]
  | Comment:    調整可能ポイント（※外部指定のデータにより調整可能な座標）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AdjustablePoint.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//--------------------------------
// マクロ（※加算してリミッタ）
//--------------------------------
#define ADD_RATE( _val, _add )                      \
    (_val) += (_add);                               \
    if( (_val) > BEZIER_POSITION_RATE_MAX ){        \
        (_val) = BEZIER_POSITION_RATE_MAX;          \
    }else if( (_val) < BEZIER_POSITION_RATE_MIN ){  \
        (_val) = BEZIER_POSITION_RATE_MIN;          \
    }

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
//-------------------------
// コンストラクタ
//-------------------------
CAdjustablePoint::CAdjustablePoint( void ){
	clear();
}

//----------------------------------------------------------------------------
// クリア（※データの破棄）
//----------------------------------------------------------------------------
void CAdjustablePoint::clear( void ){
    m_nRateX = m_nRateY = 0;
    
    m_nOfsMinRateXForH = m_nOfsMinRateYForH = 0;
    m_nOfsMaxRateXForH = m_nOfsMaxRateYForH = 0;
    
    m_nOfsMinRateXForV = m_nOfsMinRateYForV = 0;
    m_nOfsMaxRateXForV = m_nOfsMaxRateYForV = 0;

    m_eDelayType = eDELAY_LOG_INVALID;
    m_nDelayDepth = 0;
    m_nDelayPowerRateX = m_nDelayPowerRateY = 0;
}

//--------------------
// コピー
//--------------------
void CAdjustablePoint::copy( CAdjustablePoint* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_nRateX = pData->m_nRateX;
    m_nRateY = pData->m_nRateY;
    
    m_nOfsMinRateXForH = pData->m_nOfsMinRateXForH;
    m_nOfsMinRateYForH = pData->m_nOfsMinRateYForH;
    m_nOfsMaxRateXForH = pData->m_nOfsMaxRateXForH;
    m_nOfsMaxRateYForH = pData->m_nOfsMaxRateYForH;
    
    m_nOfsMinRateXForV = pData->m_nOfsMinRateXForV;
    m_nOfsMinRateYForV = pData->m_nOfsMinRateYForV;
    m_nOfsMaxRateXForV = pData->m_nOfsMaxRateXForV;
    m_nOfsMaxRateYForV = pData->m_nOfsMaxRateYForV;
    
    m_eDelayType = pData->m_eDelayType;
    m_nDelayDepth = 0;
    m_nDelayPowerRateX = pData->m_nDelayPowerRateX;
    m_nDelayPowerRateY = pData->m_nDelayPowerRateY;
}

//---------------------------
// 読み込み
//---------------------------
void CAdjustablePoint::read( CInputBuffer* pIB ){
    m_nRateX = pIB->readInt16();
    m_nRateY = pIB->readInt16();
    
    m_nOfsMinRateXForH = pIB->readInt16();
    m_nOfsMinRateYForH = pIB->readInt16();
    m_nOfsMaxRateXForH = pIB->readInt16();
    m_nOfsMaxRateYForH = pIB->readInt16();
    
    m_nOfsMinRateXForV = pIB->readInt16();
    m_nOfsMinRateYForV = pIB->readInt16();
    m_nOfsMaxRateXForV = pIB->readInt16();
    m_nOfsMaxRateYForV = pIB->readInt16();

    m_eDelayType = (eDELAY_LOG)CEnum::ReadEnum( pIB, g_pArrLabelDelayLog );
    m_nDelayDepth = pIB->readInt16();
    m_nDelayPowerRateX = pIB->readInt16();
    m_nDelayPowerRateY = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CAdjustablePoint::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nRateX );
    pOB->writeInt16( (int16)m_nRateY );

    pOB->writeInt16( (int16)m_nOfsMinRateXForH );
    pOB->writeInt16( (int16)m_nOfsMinRateYForH );
    pOB->writeInt16( (int16)m_nOfsMaxRateXForH );
    pOB->writeInt16( (int16)m_nOfsMaxRateYForH );
    
    pOB->writeInt16( (int16)m_nOfsMinRateXForV );
    pOB->writeInt16( (int16)m_nOfsMinRateYForV );
    pOB->writeInt16( (int16)m_nOfsMaxRateXForV );
    pOB->writeInt16( (int16)m_nOfsMaxRateYForV );

    CEnum::WriteEnum( pOB, m_eDelayType, eDELAY_LOG_MAX, g_pArrLabelDelayLog );
    pOB->writeInt16( (int16)m_nDelayDepth );
    pOB->writeInt16( (int16)m_nDelayPowerRateX );
    pOB->writeInt16( (int16)m_nDelayPowerRateY );
}

//--------------------------------------------------
// 値の増減
//--------------------------------------------------
// RateX/Y
void CAdjustablePoint::addRateXY( int addX, int addY ){
    ADD_RATE( m_nRateX, addX );
    ADD_RATE( m_nRateY, addY );
}

// MinRateX/YForH
void CAdjustablePoint::addMinRateXYForH( int addX, int addY ){
    ADD_RATE( m_nOfsMinRateXForH, addX );
    ADD_RATE( m_nOfsMinRateYForH, addY );
}

// MaxRateX/YForH
void CAdjustablePoint::addMaxRateXYForH( int addX, int addY ){
    ADD_RATE( m_nOfsMaxRateXForH, addX );
    ADD_RATE( m_nOfsMaxRateYForH, addY );
}

// MinRateX/YForV
void CAdjustablePoint::addMinRateXYForV( int addX, int addY ){
    ADD_RATE( m_nOfsMinRateXForV, addX );
    ADD_RATE( m_nOfsMinRateYForV, addY );
}

// MaxRateX/YForV
void CAdjustablePoint::addMaxRateXYForV( int addX, int addY ){
    ADD_RATE( m_nOfsMaxRateXForV, addX );
    ADD_RATE( m_nOfsMaxRateYForV, addY );
}

//-------------------------------------------------------------------
// 変更の適用
//-------------------------------------------------------------------
// オフセット
void CAdjustablePoint::applyOfs( int rateOfsX, int rateOfsY ){
    m_nRateX += rateOfsX;
    m_nRateY += rateOfsY;
}

// スケール
void CAdjustablePoint::applyScale( int rateScaleX, int rateScaleY ){
    m_nRateX = (m_nRateX * rateScaleX)/BEZIER_SCALE_RATE;
    m_nRateY = (m_nRateY * rateScaleY)/BEZIER_SCALE_RATE;
    
    m_nOfsMinRateXForH = (m_nOfsMinRateXForH * rateScaleX)/BEZIER_SCALE_RATE;
    m_nOfsMinRateYForH = (m_nOfsMinRateYForH * rateScaleY)/BEZIER_SCALE_RATE;
    
    m_nOfsMaxRateXForH = (m_nOfsMaxRateXForH * rateScaleX)/BEZIER_SCALE_RATE;
    m_nOfsMaxRateYForH = (m_nOfsMaxRateYForH * rateScaleY)/BEZIER_SCALE_RATE;
    
    m_nOfsMinRateXForV = (m_nOfsMinRateXForV * rateScaleX)/BEZIER_SCALE_RATE;
    m_nOfsMinRateYForV = (m_nOfsMinRateYForV * rateScaleY)/BEZIER_SCALE_RATE;
    
    m_nOfsMaxRateXForV = (m_nOfsMaxRateXForV * rateScaleX)/BEZIER_SCALE_RATE;
    m_nOfsMaxRateYForV = (m_nOfsMaxRateYForV * rateScaleY)/BEZIER_SCALE_RATE;

    // [m_eDelayType]は無視
    // [m_eDelayDepth]は無視
    m_nDelayPowerRateX = (m_nDelayPowerRateX * rateScaleX)/BEZIER_SCALE_RATE;
    m_nDelayPowerRateY = (m_nDelayPowerRateY * rateScaleY)/BEZIER_SCALE_RATE;
}

// スケール適用後にオフセット適用
void CAdjustablePoint::applyScaleThenOfs( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY ){
    applyScale( rateScaleX, rateScaleY );
    applyOfs( rateOfsX, rateOfsY );
}

//-------------------------------------------------------------------
// 変更の適用：調整値
//-------------------------------------------------------------------
// オフセット
void CAdjustablePoint::applyOfsForVal( int rateOfsX, int rateOfsY, int* pX, int* pY ){
    *pX += rateOfsX;
    *pY += rateOfsY;
}

// スケール
void CAdjustablePoint::applyScaleForVal( int rateScaleX, int rateScaleY, int* pX, int* pY ){
    float x = ((m_nRateX + *pX) * rateScaleX)/BEZIER_SCALE_RATE;
    float y = ((m_nRateY + *pY) * rateScaleY)/BEZIER_SCALE_RATE;

    // 調整座標と基本座標の差分を対象の値に設定
    *pX = x - m_nRateX;
    *pY = y - m_nRateY;
}

// スケール適用後にオフセット適用
void CAdjustablePoint::applyScaleThenOfsForVal( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY, int* pX, int* pY ){
    applyScaleForVal( rateScaleX, rateScaleY, pX, pY );
    applyOfsForVal( rateOfsX, rateOfsY, pX, pY );
}

//------------
// スケール適用
//------------
void CAdjustablePoint::applyRateScale( int rateScale ){
    m_nRateX = m_nRateX*rateScale/BEZIER_SCALE_RATE;
    m_nRateY = m_nRateY*rateScale/BEZIER_SCALE_RATE;
    
    m_nOfsMinRateXForH = m_nOfsMinRateXForH*rateScale/BEZIER_SCALE_RATE;
    m_nOfsMinRateYForH = m_nOfsMinRateYForH*rateScale/BEZIER_SCALE_RATE;
    m_nOfsMaxRateXForH = m_nOfsMaxRateXForH*rateScale/BEZIER_SCALE_RATE;
    m_nOfsMaxRateYForH = m_nOfsMaxRateYForH*rateScale/BEZIER_SCALE_RATE;
    
    m_nOfsMinRateXForV = m_nOfsMinRateXForV*rateScale/BEZIER_SCALE_RATE;
    m_nOfsMinRateYForV = m_nOfsMinRateYForV*rateScale/BEZIER_SCALE_RATE;
    m_nOfsMaxRateXForV = m_nOfsMaxRateXForV*rateScale/BEZIER_SCALE_RATE;
    m_nOfsMaxRateYForV = m_nOfsMaxRateYForV*rateScale/BEZIER_SCALE_RATE;
    
    // [m_eDelayType]は無視
    // [m_nDelayDepth]は無視
    m_nDelayPowerRateX = m_nDelayPowerRateX*rateScale/BEZIER_SCALE_RATE;
    m_nDelayPowerRateY = m_nDelayPowerRateY*rateScale/BEZIER_SCALE_RATE;
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CAdjustablePoint::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 14 );
    
    int id = 0;
    
    // 基本座標
    pMenu->setItemAt( id++, "X", &m_nRateX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "Y", &m_nRateY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );

    // ヨコ調整
    pMenu->setItemAt( id++, "H: MIN_X", &m_nOfsMinRateXForH, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "H: MIN_Y", &m_nOfsMinRateYForH, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "H: MAX_X", &m_nOfsMaxRateXForH, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "H: MAX_Y", &m_nOfsMaxRateYForH, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );

    // タテ調整
    pMenu->setItemAt( id++, "V: MIN_X", &m_nOfsMinRateXForV, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "V: MIN_Y", &m_nOfsMinRateYForV, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "V: MAX_X", &m_nOfsMaxRateXForV, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "V: MAX_Y", &m_nOfsMaxRateYForV, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );

    // 遅延調整
    pMenu->setItemAtAsLabel( id++, "DLY: TYPE", &m_eDelayType, eEDIT_VALUE_TYPE_INT32,
                             eDELAY_LOG_INVALID, eDELAY_LOG_MAX-1, g_pArrLabelDelayLog );
    pMenu->setItemAt( id++, "DLY: DEPTH", &m_nDelayDepth, eEDIT_VALUE_TYPE_INT32, 0, DELAY_LOG_DEPTH );
    pMenu->setItemAt( id++, "DLY: POW_RATE_X", &m_nDelayPowerRateX, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "DLY: POW_RATE_Y", &m_nDelayPowerRateY, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    
    // 確定
    pMenu->fixMenu();
}
