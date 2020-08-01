/*+----------------------------------------------------------------+
  |	Title:		SlotAdjuster.cpp [共通環境]
  | Comment:    スロット調整（※外部指定のデータによりスロットを調整）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "SlotAdjuster.hpp"

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
//-------------------------
// コンストラクタ
//-------------------------
CSlotAdjuster::CSlotAdjuster( void ){
	clear();
}

//-------------------------
// クリア（※データの破棄）
//-------------------------
void CSlotAdjuster::clear( void ){
    // for T
    m_nMinRateForT = 0;
    m_nMaxRateForT = 0;

    // for S
    m_nMinRateForS = BEZIER_SCALE_RATE;
    m_nMaxRateForS = BEZIER_SCALE_RATE;

    // joint
    m_eJointPointIdForFrom = eJOINT_POINT_INVALID;
    m_eJointPointIdForTo = eJOINT_POINT_INVALID;
    
    m_nJointRateRotR = 0;
    m_nJointMinRateOfsRForH = 0;
    m_nJointMaxRateOfsRForH = 0;
    m_nJointRateRotOfs = 0;
    m_nJointRateOfsForRotCenter = 0;
    
    m_nJointRateDirForFrom = 0;
    m_nJointRateDirForTo = 0;
    m_nJointRateDirForOpen = 0;
    m_nJointRateDirForClose = 0;
}

//--------------------
// コピー
//--------------------
void CSlotAdjuster::copy( CSlotAdjuster* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    // for T
    m_nMinRateForT = pData->m_nMinRateForT;
    m_nMaxRateForT = pData->m_nMaxRateForT;

    // for S
    m_nMinRateForS = pData->m_nMinRateForS;
    m_nMaxRateForS = pData->m_nMaxRateForS;

    // joint
    m_eJointPointIdForFrom = pData->m_eJointPointIdForFrom;
    m_eJointPointIdForTo = pData->m_eJointPointIdForTo;
    
    m_nJointRateRotR = pData->m_nJointRateRotR;
    m_nJointMaxRateOfsRForH = pData->m_nJointMaxRateOfsRForH;
    m_nJointMaxRateOfsRForH = pData->m_nJointMaxRateOfsRForH;
    m_nJointRateRotOfs = pData->m_nJointRateRotOfs;
    m_nJointRateOfsForRotCenter = pData->m_nJointRateOfsForRotCenter;
    
    m_nJointRateDirForFrom = pData->m_nJointRateDirForFrom;
    m_nJointRateDirForTo = pData->m_nJointRateDirForTo;
    m_nJointRateDirForOpen = pData->m_nJointRateDirForOpen;
    m_nJointRateDirForClose = pData->m_nJointRateDirForClose;
}

//---------------------------
// 読み込み
//---------------------------
void CSlotAdjuster::read( CInputBuffer* pIB ){
    // for T
    m_nMinRateForT = pIB->readInt16();
    m_nMaxRateForT = pIB->readInt16();

    // for S
    m_nMinRateForS = pIB->readInt16();
    m_nMaxRateForS = pIB->readInt16();

    // joint
    m_eJointPointIdForFrom = (eJOINT_POINT)CEnum::ReadEnum( pIB, g_pArrLabelJointPoint );
    m_eJointPointIdForTo = (eJOINT_POINT)CEnum::ReadEnum( pIB, g_pArrLabelJointPoint );
    
    m_nJointRateRotR = pIB->readInt16();
    m_nJointMinRateOfsRForH = pIB->readInt16();
    m_nJointMaxRateOfsRForH = pIB->readInt16();
    m_nJointRateRotOfs = pIB->readInt16();
    m_nJointRateOfsForRotCenter = pIB->readInt16();

    m_nJointRateDirForFrom = pIB->readInt16();
    m_nJointRateDirForTo = pIB->readInt16();
    m_nJointRateDirForOpen = pIB->readInt16();
    m_nJointRateDirForClose = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CSlotAdjuster::write( COutputBuffer* pOB ){
    // for T
    pOB->writeInt16( (int16)m_nMinRateForT );
    pOB->writeInt16( (int16)m_nMaxRateForT );

    // for S
    pOB->writeInt16( (int16)m_nMinRateForS );
    pOB->writeInt16( (int16)m_nMaxRateForS );

    // joint
    CEnum::WriteEnum( pOB, m_eJointPointIdForFrom, eJOINT_POINT_MAX, g_pArrLabelJointPoint );
    CEnum::WriteEnum( pOB, m_eJointPointIdForTo, eJOINT_POINT_MAX, g_pArrLabelJointPoint );
    
    pOB->writeInt16( (int16)m_nJointRateRotR );
    pOB->writeInt16( (int16)m_nJointMinRateOfsRForH );
    pOB->writeInt16( (int16)m_nJointMaxRateOfsRForH );
    pOB->writeInt16( (int16)m_nJointRateRotOfs );
    pOB->writeInt16( (int16)m_nJointRateOfsForRotCenter );
    
    pOB->writeInt16( (int16)m_nJointRateDirForFrom );
    pOB->writeInt16( (int16)m_nJointRateDirForTo );
    pOB->writeInt16( (int16)m_nJointRateDirForOpen );
    pOB->writeInt16( (int16)m_nJointRateDirForClose );
}

//-----------------------------
// 結合ポイントIDの取得（※接続元用）
//-----------------------------
eJOINT_POINT CSlotAdjuster::getJointPointIdForFrom( int slotIndex ){
    int id = m_eJointPointIdForFrom;
    
    // スロットインデックスにより参照先を補正
    if( id >= 0 && slotIndex >= 0 ){
        id += JOINT_POINT_OFS_FOR_SLOT_INDEX * slotIndex;
    }
    
    return( (eJOINT_POINT) id );
}

//-----------------------------
// 結合ポイントIDの取得（※接続元用）
//-----------------------------
eJOINT_POINT CSlotAdjuster::getJointPointIdForTo( int slotIndex ){
    int id = m_eJointPointIdForTo;
    
    // スロットインデックスにより参照先を補正
    if( id >= 0 && slotIndex >= 0 ){
        id += JOINT_POINT_OFS_FOR_SLOT_INDEX * slotIndex;
    }
    
    return( (eJOINT_POINT) id );
}

//------------
// スケール適用
//------------
void CSlotAdjuster::applyRateScale( int rateScale ){
    // for T
    // [m_nMinRateForT]は無視（※この値は割合）
    // [m_nMaxRateForT]は無視（※この値は割合）
    
    // for S
    // [m_nMinRateForS]は無視（※この値は割合）
    // [m_nMaxRateForS]は無視（※この値は割合）

    // for Joint
    // [m_eJointPointIdForFrom]は無視（※この値はIDなのでスケールに意味はない）
    // [m_eJointPointIdForTo]は無視（※この値はIDなのでスケールに意味はない）
    
    m_nJointRateRotR = m_nJointRateRotR*rateScale/BEZIER_SCALE_RATE;
    m_nJointMinRateOfsRForH = m_nJointMinRateOfsRForH*rateScale/BEZIER_SCALE_RATE;
    m_nJointMaxRateOfsRForH = m_nJointMaxRateOfsRForH*rateScale/BEZIER_SCALE_RATE;
    // [m_nJointRateRotOfs]は無視（※この値は割合）
    // [m_nJointRateOfsForRotCenter]は無視（この値は半径に対して適用される割合）
    
    // [m_nJointRateDirForFrom]は無視（※この値は割合値）
    // [m_nJointRateDirForTo]は無視（※この値は割合値）
    // [m_nJointRateDirForOpen]は無視（※この値は割合値）
    // [m_nJointRateDirForClose]は無視（※この値は割合値）
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CSlotAdjuster::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 2 + 2 + 11 );
    
    int id = 0;
    
    // for T
    pMenu->setItemAt( id++, "T: MIN_R", &m_nMinRateForT, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
    pMenu->setItemAt( id++, "T: MAX_R", &m_nMaxRateForT, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    // for S
    pMenu->setItemAt( id++, "S: MIN_R", &m_nMinRateForS, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "S: MAX_R", &m_nMaxRateForS, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    // joint
    pMenu->setItemAtAsLabel( id++, "J: ID_FROM", &m_eJointPointIdForFrom, eEDIT_VALUE_TYPE_INT32,
                             eJOINT_POINT_INVALID, eJOINT_POINT_MAX-1, g_pArrLabelJointPoint );
    pMenu->setItemAtAsLabel( id++, "J: ID_TO", &m_eJointPointIdForTo, eEDIT_VALUE_TYPE_INT32,
                             eJOINT_POINT_INVALID, eJOINT_POINT_MAX-1, g_pArrLabelJointPoint );

    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "J: ROT_R", &m_nJointRateRotR, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    
    pMenu->setItemAt( id++, "J: ADJ_MIN_R", &m_nJointMinRateOfsRForH, eEDIT_VALUE_TYPE_INT32,
                      -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    
    pMenu->setItemAt( id++, "J: ADJ_MAX_R", &m_nJointMaxRateOfsRForH, eEDIT_VALUE_TYPE_INT32,
                      -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    
    pMenu->setItemAt( id++, "J: ROT_OFS", &m_nJointRateRotOfs, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
    
    pMenu->setItemAt( id++, "J: RATE_FOR_ROT_CENTER_OFS", &m_nJointRateOfsForRotCenter, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "J: DIR FOR FROM", &m_nJointRateDirForFrom, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: DIR FOR TO", &m_nJointRateDirForTo, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: DIR FOR OPEN", &m_nJointRateDirForOpen, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setItemAt( id++, "J: DIR FOR CLOSE", &m_nJointRateDirForClose, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    // 確定
    pMenu->fixMenu();
}
