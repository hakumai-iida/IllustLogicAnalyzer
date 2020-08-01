/*+----------------------------------------------------------------+
  |	Title:		AnchorPointData.cpp [共通環境]
  |	Comment:	アンカーポイントデータ（※[CLineObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AnchorPointData.hpp"
#include "draw/tex/stroke/Stroke.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_ANCHOR_POINT_DATA   (32)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CAnchorPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_ANCHOR_POINT_DATA )

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
//----------------------------------------------
// 確保メモリセル数（※管理内で[new]するセル数）
//----------------------------------------------
int CAnchorPointData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CAnchorPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_ANCHOR_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CAnchorPointData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CAnchorPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_ANCHOR_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CAnchorPointData) * MAX_ALLOC_ANCHOR_POINT_DATA;
    
    return( allocSize );
}

//---------------------------
// コンストラクタ
//---------------------------
CAnchorPointData::CAnchorPointData( void ):  CListNode(){
	clear();
}

//----------------------------------------------------------------------------
// クリア（※データの破棄）
//----------------------------------------------------------------------------
void CAnchorPointData::clear( void ){
    m_nFlag = 0x0;
    m_oAdjXY.clear();
    m_oAdjInXY.clear();
    m_oAdjOutXY.clear();
        
    m_eJointPointId = eJOINT_POINT_INVALID;
    m_eHookTargetId = eSTROKE_HOOK_TARGET_INVALID;
    
    m_nStrokeSize = BEZIER_SCALE_RATE;
    m_nStrokeStartSize = 0;
    m_nStrokeStartRange = 0;
    m_nStrokeEndSize = 0;
    m_nStrokeEndRange = 0;
    m_nStrokeEdgeFillSize = 0;
    
    m_eTouchTargetId = eSTROKE_TOUCH_TARGET_INVALID;
    m_eFrillId = eSTROKE_FRILL_INVALID;

    // 初期値は０
    m_nP00_TouchBasePos = 0;
    m_nP01_TouchBaseSize = 0;
    m_nP02_TouchBaseRot = 0;
    m_nP03_TouchFrontArea = 0;
    m_nP04_TouchFrontSize = 0;
    m_nP05_TouchFrontRot = 0;
    m_nP06_TouchBackArea = 0;
    m_nP07_TouchBackSize = 0;
    m_nP08_TouchBackRot = 0;
    m_nP09_TouchFrontStep_FrillDummy = 0;
    m_nP10_TouchFrontStep2_FrillDummy = 0;
    m_nP11_TouchFrontStepRnd_FrillDummy = 0;
    m_nP12_TouchBackStep_FrillDummy = 0;
    m_nP13_TouchBackStep2_FrillDummy = 0;
    m_nP14_TouchBackStepRnd_FrillDummy = 0;
    
    m_eStripeMainPalOfsId = ePAL_OFS_INVALID;
    m_eStripeSubPalOfsId = ePAL_OFS_INVALID;

    m_bEditMenuForFrill = false;
}

//---------------------------
// 設定
//---------------------------
void CAnchorPointData::set( int rX, int rY, int inRX, int inRY, int outRX, int outRY ){
    m_oAdjXY.setRateXY( rX, rY );
    m_oAdjInXY.setRateXY( inRX, inRY );
    m_oAdjOutXY.setRateXY( outRX, outRY );
}

//---------------------------
// 設定：ストロークサイズ
//---------------------------
void CAnchorPointData::setStrokeSize( int size, int startSize, int startRange, int endSize, int endRange ){
    m_nStrokeSize = size;
    m_nStrokeStartSize = startSize;
    m_nStrokeStartRange = startRange;
    m_nStrokeEndSize = endSize;
    m_nStrokeEndRange = endRange;
}

//---------------------------
// 設定：タッチ（基本）
//---------------------------
void CAnchorPointData::setTouchBase( int pos, int size, int rot ){
    m_nP00_TouchBasePos = pos;
    m_nP01_TouchBaseSize = size;
    m_nP02_TouchBaseRot = rot;
}

//---------------------------
// 設定：タッチ（前方）
//---------------------------
void CAnchorPointData::setTouchFront( int area, int size, int rot, int step, int step2, int stepRnd ){
    m_nP03_TouchFrontArea = area;
    m_nP04_TouchFrontSize = size;
    m_nP05_TouchFrontRot = rot;
    
    m_nP09_TouchFrontStep_FrillDummy = step;
    m_nP10_TouchFrontStep2_FrillDummy = step2;
    m_nP11_TouchFrontStepRnd_FrillDummy = stepRnd;
}

//---------------------------
// 設定：タッチ（後方）
//---------------------------
void CAnchorPointData::setTouchBack( int area, int size, int rot, int step, int step2, int stepRnd ){
    m_nP06_TouchBackArea = area;
    m_nP07_TouchBackSize = size;
    m_nP08_TouchBackRot = rot;

    m_nP12_TouchBackStep_FrillDummy = step;
    m_nP13_TouchBackStep2_FrillDummy = step2;
    m_nP14_TouchBackStepRnd_FrillDummy = stepRnd;
}

//---------------------------
// 設定：フリル（基本）
//---------------------------
void CAnchorPointData::setFrillBase( int pos, int size, int rot ){
    m_nP00_TouchBasePos = pos;
    m_nP01_TouchBaseSize = size;
    m_nP02_TouchBaseRot = rot;

    // 基本値を設定しておく（※[setFrillFront/setFrillBack]を呼ばなくても基本的な表示がでるように）
    m_nP03_TouchFrontArea = BEZIER_SCALE_RATE;
    m_nP04_TouchFrontSize = size;
    m_nP05_TouchFrontRot = rot;
    m_nP06_TouchBackArea = BEZIER_SCALE_RATE;
    m_nP07_TouchBackSize = size;
    m_nP08_TouchBackRot = rot;
}

//---------------------------
// 設定：フリル（前方）
//---------------------------
void CAnchorPointData::setFrillFront( int area, int size, int rot ){
    m_nP03_TouchFrontArea = area;
    m_nP04_TouchFrontSize = size;
    m_nP05_TouchFrontRot = rot;
}

//---------------------------
// 設定：フリル（後方）
//---------------------------
void CAnchorPointData::setFrillBack( int area, int size, int rot ){
    m_nP06_TouchBackArea = area;
    m_nP07_TouchBackSize = size;
    m_nP08_TouchBackRot = rot;
}

//---------------------------
// 結合ポイントIDの取得
//---------------------------
eJOINT_POINT CAnchorPointData::getJointPointId( int slotIndex ){
    int id = m_eJointPointId;

    // スロットインデックスにより参照先を補正
    if( id >= 0 && id < eJOINT_POINT_MAX && slotIndex >= 0 ){
        id += JOINT_POINT_OFS_FOR_SLOT_INDEX * slotIndex;
    }

    return( (eJOINT_POINT) id );
}

//---------------------------
// フック対象IDの取得
//---------------------------
eSTROKE_HOOK_TARGET CAnchorPointData::getHookTargetId( int slotIndex ){
    return( CStroke::AdjustHookTargetForSlotIndex( m_eHookTargetId, slotIndex ) );
}

//---------------------------
// タッチ対象IDの取得
//---------------------------
eSTROKE_TOUCH_TARGET CAnchorPointData::getTouchTargetId( int slotIndex ){
    return( CStroke::AdjustTouchTargetForSlotIndex( m_eTouchTargetId, slotIndex ) );
}

//--------------------
// コピー
//--------------------
void CAnchorPointData::copy( CAnchorPointData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_nFlag = pData->m_nFlag;
    m_oAdjXY.copy( &pData->m_oAdjXY );
    m_oAdjInXY.copy( &pData->m_oAdjInXY );
    m_oAdjOutXY.copy( &pData->m_oAdjOutXY );
        
    m_eJointPointId = pData->m_eJointPointId;
    m_eHookTargetId = pData->m_eHookTargetId;
    
    m_nStrokeSize = pData->m_nStrokeSize;
    m_nStrokeStartSize = pData->m_nStrokeStartSize;
    m_nStrokeStartRange = pData->m_nStrokeStartRange;
    m_nStrokeEndSize = pData->m_nStrokeEndSize;
    m_nStrokeEndRange = pData->m_nStrokeEndRange;
    m_nStrokeEdgeFillSize = pData->m_nStrokeEdgeFillSize;

    m_eTouchTargetId = pData->m_eTouchTargetId;
    m_eFrillId = pData->m_eFrillId;
    
    m_nP00_TouchBasePos = pData->m_nP00_TouchBasePos;
    m_nP01_TouchBaseSize = pData->m_nP01_TouchBaseSize;
    m_nP02_TouchBaseRot = pData->m_nP02_TouchBaseRot;
    m_nP03_TouchFrontArea = pData->m_nP03_TouchFrontArea;
    m_nP04_TouchFrontSize = pData->m_nP04_TouchFrontSize;
    m_nP05_TouchFrontRot = pData->m_nP05_TouchFrontRot;
    m_nP06_TouchBackArea = pData->m_nP06_TouchBackArea;
    m_nP07_TouchBackSize = pData->m_nP07_TouchBackSize;
    m_nP08_TouchBackRot = pData->m_nP08_TouchBackRot;
    m_nP09_TouchFrontStep_FrillDummy = pData->m_nP09_TouchFrontStep_FrillDummy;
    m_nP10_TouchFrontStep2_FrillDummy = pData->m_nP10_TouchFrontStep2_FrillDummy;
    m_nP11_TouchFrontStepRnd_FrillDummy = pData->m_nP11_TouchFrontStepRnd_FrillDummy;
    m_nP12_TouchBackStep_FrillDummy = pData->m_nP12_TouchBackStep_FrillDummy;
    m_nP13_TouchBackStep2_FrillDummy = pData->m_nP13_TouchBackStep2_FrillDummy;
    m_nP14_TouchBackStepRnd_FrillDummy = pData->m_nP14_TouchBackStepRnd_FrillDummy;

    m_eStripeMainPalOfsId = pData->m_eStripeMainPalOfsId;
    m_eStripeSubPalOfsId = pData->m_eStripeSubPalOfsId;
}

//---------------------------
// 読み込み
//---------------------------
void CAnchorPointData::read( CInputBuffer* pIB ){
    m_nFlag = pIB->readInt16();
    m_oAdjXY.read( pIB );
    m_oAdjInXY.read( pIB );
    m_oAdjOutXY.read( pIB );
   
    m_eJointPointId = (eJOINT_POINT)CEnum::ReadEnum( pIB, g_pArrLabelJointPoint );
    m_eHookTargetId = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );

    m_nStrokeSize = pIB->readInt16();
    m_nStrokeStartSize = pIB->readInt16();
    m_nStrokeStartRange = pIB->readInt16();
    m_nStrokeEndSize = pIB->readInt16();
    m_nStrokeEndRange = pIB->readInt16();
    m_nStrokeEdgeFillSize = pIB->readInt16();

    m_eTouchTargetId = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );
    m_eFrillId = (eSTROKE_FRILL)CEnum::ReadEnum( pIB, g_pArrLabelStrokeFrill );
    
    m_nP00_TouchBasePos = pIB->readInt16();
    m_nP01_TouchBaseSize = pIB->readInt16();
    m_nP02_TouchBaseRot = pIB->readInt16();
    m_nP03_TouchFrontArea = pIB->readInt16();
    m_nP04_TouchFrontSize = pIB->readInt16();
    m_nP05_TouchFrontRot = pIB->readInt16();
    m_nP06_TouchBackArea = pIB->readInt16();
    m_nP07_TouchBackSize = pIB->readInt16();
    m_nP08_TouchBackRot = pIB->readInt16();
    m_nP09_TouchFrontStep_FrillDummy = pIB->readInt16();
    m_nP10_TouchFrontStep2_FrillDummy = pIB->readInt16();
    m_nP11_TouchFrontStepRnd_FrillDummy = pIB->readInt16();
    m_nP12_TouchBackStep_FrillDummy = pIB->readInt16();
    m_nP13_TouchBackStep2_FrillDummy = pIB->readInt16();
    m_nP14_TouchBackStepRnd_FrillDummy = pIB->readInt16();
    
    m_eStripeMainPalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
    m_eStripeSubPalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
}

//---------------------------
// 書き込み
//---------------------------
void CAnchorPointData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    m_oAdjXY.write( pOB );
    m_oAdjInXY.write( pOB );
    m_oAdjOutXY.write( pOB );
    
    CEnum::WriteEnum( pOB, m_eJointPointId, eJOINT_POINT_MAX, g_pArrLabelJointPoint );
    CEnum::WriteEnum( pOB, m_eHookTargetId, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );

    pOB->writeInt16( (int16)m_nStrokeSize );
    pOB->writeInt16( (int16)m_nStrokeStartSize );
    pOB->writeInt16( (int16)m_nStrokeStartRange );
    pOB->writeInt16( (int16)m_nStrokeEndSize );
    pOB->writeInt16( (int16)m_nStrokeEndRange );
    pOB->writeInt16( (int16)m_nStrokeEdgeFillSize );

    CEnum::WriteEnum( pOB, m_eTouchTargetId, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );
    CEnum::WriteEnum( pOB, m_eFrillId, eSTROKE_FRILL_MAX, g_pArrLabelStrokeFrill );
    
    pOB->writeInt16( (int16)m_nP00_TouchBasePos );
    pOB->writeInt16( (int16)m_nP01_TouchBaseSize );
    pOB->writeInt16( (int16)m_nP02_TouchBaseRot );
    pOB->writeInt16( (int16)m_nP03_TouchFrontArea );
    pOB->writeInt16( (int16)m_nP04_TouchFrontSize );
    pOB->writeInt16( (int16)m_nP05_TouchFrontRot );
    pOB->writeInt16( (int16)m_nP06_TouchBackArea );
    pOB->writeInt16( (int16)m_nP07_TouchBackSize );
    pOB->writeInt16( (int16)m_nP08_TouchBackRot );
    pOB->writeInt16( (int16)m_nP09_TouchFrontStep_FrillDummy );
    pOB->writeInt16( (int16)m_nP10_TouchFrontStep2_FrillDummy );
    pOB->writeInt16( (int16)m_nP11_TouchFrontStepRnd_FrillDummy );
    pOB->writeInt16( (int16)m_nP12_TouchBackStep_FrillDummy );
    pOB->writeInt16( (int16)m_nP13_TouchBackStep2_FrillDummy );
    pOB->writeInt16( (int16)m_nP14_TouchBackStepRnd_FrillDummy );
    
    CEnum::WriteEnum( pOB, m_eStripeMainPalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
    CEnum::WriteEnum( pOB, m_eStripeSubPalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
}

//-------------------------------
// 有効性の確認
//-------------------------------
bool CAnchorPointData::isValid( void ){
    // 無効は無視
    if( checkFlag( eAPD_FLAG_DISABLE ) ){
        return( false );
    }

    // ここまできたら有効
    return( true );
}

//-------------------------------------------
// [pPrev]と[pNext]の中間に配置する
//-------------------------------------------
void CAnchorPointData::setAtIntermediate( CAnchorPointData* pPrev, CAnchorPointData* pNext ){
    CAdjustablePoint* pPrevXY = pPrev->getAdjXY();
    CAdjustablePoint* pPrevInXY = pPrev->getAdjInXY();
    CAdjustablePoint* pPrevOutXY = pPrev->getAdjOutXY();
    CAdjustablePoint* pNextXY = pNext->getAdjXY();
    CAdjustablePoint* pNextInXY = pNext->getAdjInXY();
    CAdjustablePoint* pNextOutXY = pNext->getAdjOutXY();

    //-----------------------------------------------
    // 基本XY／進入XY／出立XYの算出
    //-----------------------------------------------
    int x0 = pPrevXY->getRateX();
    int y0 = pPrevXY->getRateY();
    int x1 = pNextXY->getRateX();
    int y1 = pNextXY->getRateY();
    int xOut = pPrevOutXY->getRateX();
    int yOut = pPrevOutXY->getRateY();
    int xIn = pNextInXY->getRateX();
    int yIn = pNextInXY->getRateY();

    // t, (1-t)
    float t1 = 0.5f;        // 中点
    float tR1 = 1.0f - t1;
    
    // t^2, (1-t)^2
    float t2 = t1 * t1;
    float tR2 = tR1 * tR1;
    
    // t^3, (1-t)^3
    float t3 = t2 * t1;
    float tR3 = tR2 * tR1;
    
    // (1-t)^2*t, (1-t)*t^2
    float tR2_t1 = tR2 * t1;
    float tR1_t2 = tR1 * t2;
    
    // 座標の算出
    float newX = x0*tR3 + 3*(x0+xOut)*tR2_t1 + 3*(x1+xIn)*tR1_t2 + x1*t3;
    float newY = y0*tR3 + 3*(y0+yOut)*tR2_t1 + 3*(y1+yIn)*tR1_t2 + y1*t3;
    m_oAdjXY.setRateXY( (int)newX, (int)newY );
    
    // 前後要素の方向線が有効であれば算出(IN/OUT)
    if( xOut != 0 || yOut != 0 || xIn != 0 || yIn != 0 ){
        float newInX = x0*tR2 + 2*(x0+xOut)*tR1*t1 + (x1+xIn)*t2;
        float newInY = y0*tR2 + 2*(y0+yOut)*tR1*t1 + (y1+yIn)*t2;
        m_oAdjInXY.setRateXY( (int)(newInX-newX), (int)(newInY-newY) );
        
        float newOutX = x1*t2 + 2*(x1+xIn)*t1*tR1 + (x0+xOut)*tR2;
        float newOutY = y1*t2 + 2*(y1+yIn)*t1*tR1 + (y0+yOut)*tR2;
        m_oAdjOutXY.setRateXY( (int)(newOutX-newX), (int)(newOutY-newY) );
        
        // 前後要素の方向線を調整
        pPrevOutXY->setRateXY( (int)(pPrevOutXY->getRateX()*tR1), (int)(pPrevOutXY->getRateY()*tR1) );
        pNextInXY->setRateXY( (int)(pNextInXY->getRateX()*t1), (int)(pNextInXY->getRateY()*t1) );
    }else{
        m_oAdjInXY.setRateXY( 0, 0 );
        m_oAdjOutXY.setRateXY( 0, 0 );
    }
    
    //------------------------------------------------------------------------------------------
    // 調整値の算出（※とりあえず各値は前後要素の平均にしておく）
    //（※遅延タイプの平均は全くもって意味不明だが、基本的には前後で同じ値になるはずなので実質的には問題ないはず）
    //------------------------------------------------------------------------------------------
    int avgType, avgDepth, avgX, avgY;

    // 基本XY
    avgX = (pPrevXY->getOfsMinRateXForH() + pNextXY->getOfsMinRateXForH())/2;
    avgY = (pPrevXY->getOfsMinRateYForH() + pNextXY->getOfsMinRateYForH())/2;
    m_oAdjXY.setOfsMinRateXYForH( avgX, avgY );

    avgX = (pPrevXY->getOfsMaxRateXForH() + pNextXY->getOfsMaxRateXForH())/2;
    avgY = (pPrevXY->getOfsMaxRateYForH() + pNextXY->getOfsMaxRateYForH())/2;
    m_oAdjXY.setOfsMaxRateXYForH( avgX, avgY );

    avgX = (pPrevXY->getOfsMinRateXForV() + pNextXY->getOfsMinRateXForV())/2;
    avgY = (pPrevXY->getOfsMinRateYForV() + pNextXY->getOfsMinRateYForV())/2;
    m_oAdjXY.setOfsMinRateXYForV( avgX, avgY );
    
    avgX = (pPrevXY->getOfsMaxRateXForV() + pNextXY->getOfsMaxRateXForV())/2;
    avgY = (pPrevXY->getOfsMaxRateYForV() + pNextXY->getOfsMaxRateYForV())/2;
    m_oAdjXY.setOfsMaxRateXYForV( avgX, avgY );

    avgType = (pPrevXY->getDelayType() + pNextXY->getDelayType())/2;
    avgDepth = (pPrevXY->getDelayDepth() + pNextXY->getDelayDepth())/2;
    avgX = (pPrevXY->getDelayPowerRateX() + pNextXY->getDelayPowerRateX())/2;
    avgY = (pPrevXY->getDelayPowerRateY() + pNextXY->getDelayPowerRateY())/2;
    m_oAdjXY.setDelayType( (eDELAY_LOG)avgType, avgDepth );
    m_oAdjXY.setDelayPowerRate( avgX, avgY );
    
    // 進入XY
    avgX = (pPrevInXY->getOfsMinRateXForH() + pNextInXY->getOfsMinRateXForH())/2;
    avgY = (pPrevInXY->getOfsMinRateYForH() + pNextInXY->getOfsMinRateYForH())/2;
    m_oAdjInXY.setOfsMinRateXYForH( avgX, avgY );
    
    avgX = (pPrevInXY->getOfsMaxRateXForH() + pNextInXY->getOfsMaxRateXForH())/2;
    avgY = (pPrevInXY->getOfsMaxRateYForH() + pNextInXY->getOfsMaxRateYForH())/2;
    m_oAdjInXY.setOfsMaxRateXYForH( avgX, avgY );
    
    avgX = (pPrevInXY->getOfsMinRateXForV() + pNextInXY->getOfsMinRateXForV())/2;
    avgY = (pPrevInXY->getOfsMinRateYForV() + pNextInXY->getOfsMinRateYForV())/2;
    m_oAdjInXY.setOfsMinRateXYForV( avgX, avgY );
    
    avgX = (pPrevInXY->getOfsMaxRateXForV() + pNextInXY->getOfsMaxRateXForV())/2;
    avgY = (pPrevInXY->getOfsMaxRateYForV() + pNextInXY->getOfsMaxRateYForV())/2;
    m_oAdjInXY.setOfsMaxRateXYForV( avgX, avgY );

    avgType = (pPrevInXY->getDelayType() + pNextInXY->getDelayType())/2;
    avgDepth = (pPrevInXY->getDelayDepth() + pNextInXY->getDelayDepth())/2;
    avgX = (pPrevInXY->getDelayPowerRateX() + pNextInXY->getDelayPowerRateX())/2;
    avgY = (pPrevInXY->getDelayPowerRateY() + pNextInXY->getDelayPowerRateY())/2;
    m_oAdjInXY.setDelayType( (eDELAY_LOG)avgType, avgDepth );
    m_oAdjInXY.setDelayPowerRate( avgX, avgY );

    // 出立XY
    avgX = (pPrevOutXY->getOfsMinRateXForH() + pNextOutXY->getOfsMinRateXForH())/2;
    avgY = (pPrevOutXY->getOfsMinRateYForH() + pNextOutXY->getOfsMinRateYForH())/2;
    m_oAdjOutXY.setOfsMinRateXYForH( avgX, avgY );
    
    avgX = (pPrevOutXY->getOfsMaxRateXForH() + pNextOutXY->getOfsMaxRateXForH())/2;
    avgY = (pPrevOutXY->getOfsMaxRateYForH() + pNextOutXY->getOfsMaxRateYForH())/2;
    m_oAdjOutXY.setOfsMaxRateXYForH( avgX, avgY );
    
    avgX = (pPrevOutXY->getOfsMinRateXForV() + pNextOutXY->getOfsMinRateXForV())/2;
    avgY = (pPrevOutXY->getOfsMinRateYForV() + pNextOutXY->getOfsMinRateYForV())/2;
    m_oAdjOutXY.setOfsMinRateXYForV( avgX, avgY );
    
    avgX = (pPrevOutXY->getOfsMaxRateXForV() + pNextOutXY->getOfsMaxRateXForV())/2;
    avgY = (pPrevOutXY->getOfsMaxRateYForV() + pNextOutXY->getOfsMaxRateYForV())/2;
    m_oAdjOutXY.setOfsMaxRateXYForV( avgX, avgY );

    avgType = (pPrevOutXY->getDelayType() + pNextOutXY->getDelayType())/2;
    avgDepth = (pPrevOutXY->getDelayDepth() + pNextOutXY->getDelayDepth())/2;
    avgX = (pPrevOutXY->getDelayPowerRateX() + pNextOutXY->getDelayPowerRateX())/2;
    avgY = (pPrevOutXY->getDelayPowerRateY() + pNextOutXY->getDelayPowerRateY())/2;
    m_oAdjOutXY.setDelayType( (eDELAY_LOG)avgType, avgDepth );
    m_oAdjOutXY.setDelayPowerRate( avgX, avgY );
};

//--------------------------
// 鏡面コピー用確定：ヨコ
//--------------------------
void CAnchorPointData::fixForMirrorCopyH( void ){
    // ヨコ方向に座標を反転
    CAdjustablePoint* pXY = getAdjXY();
    pXY->setRateXY( -pXY->getRateX(), pXY->getRateY() );
    pXY->setOfsMinRateXYForH( -pXY->getOfsMinRateXForH(), pXY->getOfsMinRateYForH() );
    pXY->setOfsMaxRateXYForH( -pXY->getOfsMaxRateXForH(), pXY->getOfsMaxRateYForH() );
    pXY->setOfsMinRateXYForV( -pXY->getOfsMinRateXForV(), pXY->getOfsMinRateYForV() );
    pXY->setOfsMaxRateXYForV( -pXY->getOfsMaxRateXForV(), pXY->getOfsMaxRateYForV() );

    // 方向線の反転＆入れ替え
    CAdjustablePoint* pInXY = getAdjInXY();
    CAdjustablePoint* pOutXY = getAdjOutXY();
    
    float tempX = pInXY->getRateX();
    float tempY = pInXY->getRateY();
    pInXY->setRateXY( -pOutXY->getRateX(), pOutXY->getRateY() );
    pOutXY->setRateXY( -tempX, tempY );

    tempX = pInXY->getOfsMinRateXForH();
    tempY = pInXY->getOfsMinRateYForH();
    pInXY->setOfsMinRateXYForH( -pOutXY->getOfsMinRateXForH(), pOutXY->getOfsMinRateYForH() );
    pOutXY->setOfsMinRateXYForH( -tempX, tempY );

    tempX = pInXY->getOfsMaxRateXForH();
    tempY = pInXY->getOfsMaxRateYForH();
    pInXY->setOfsMaxRateXYForH( -pOutXY->getOfsMaxRateXForH(), pOutXY->getOfsMaxRateYForH() );
    pOutXY->setOfsMaxRateXYForH( -tempX, tempY );

    tempX = pInXY->getOfsMinRateXForV();
    tempY = pInXY->getOfsMinRateYForV();
    pInXY->setOfsMinRateXYForV( -pOutXY->getOfsMinRateXForV(), pOutXY->getOfsMinRateYForV() );
    pOutXY->setOfsMinRateXYForV( -tempX, tempY );
    
    tempX = pInXY->getOfsMaxRateXForV();
    tempY = pInXY->getOfsMaxRateYForV();
    pInXY->setOfsMaxRateXYForV( -pOutXY->getOfsMaxRateXForV(), pOutXY->getOfsMaxRateYForV() );
    pOutXY->setOfsMaxRateXYForV( -tempX, tempY );
    
    // 遅延データには左右の概念がないので何もしない
}

//------------------------
// 鏡面コピー調整：ヨコの先頭
//------------------------
void CAnchorPointData::fixAsHeadForMirrorCopyH( void ){
    // 出立方向線を進入方向線へコピー
    CAdjustablePoint* pInXY = getAdjInXY();
    CAdjustablePoint* pOutXY = getAdjOutXY();

    pInXY->setRateXY( -pOutXY->getRateX(), pOutXY->getRateY() );
    pInXY->setOfsMinRateXYForH( -pOutXY->getOfsMinRateXForH(), pOutXY->getOfsMinRateYForH() );
    pInXY->setOfsMaxRateXYForH( -pOutXY->getOfsMaxRateXForH(), pOutXY->getOfsMaxRateYForH() );
    pInXY->setOfsMinRateXYForV( -pOutXY->getOfsMinRateXForV(), pOutXY->getOfsMinRateYForV() );
    pInXY->setOfsMaxRateXYForV( -pOutXY->getOfsMaxRateXForV(), pOutXY->getOfsMaxRateYForV() );

    // 遅延データには左右の概念がないので何もしない
}

//------------------------
// 鏡面コピー調整：ヨコの末尾
//------------------------
void CAnchorPointData::fixAsTailForMirrorCopyH( void ){
    // 進入方向線を出立方向線へコピー
    CAdjustablePoint* pInXY = getAdjInXY();
    CAdjustablePoint* pOutXY = getAdjOutXY();
    
    pOutXY->setRateXY( -pInXY->getRateX(), pInXY->getRateY() );
    pOutXY->setOfsMinRateXYForH( -pInXY->getOfsMinRateXForH(), pInXY->getOfsMinRateYForH() );
    pOutXY->setOfsMaxRateXYForH( -pInXY->getOfsMaxRateXForH(), pInXY->getOfsMaxRateYForH() );
    pOutXY->setOfsMinRateXYForV( -pInXY->getOfsMinRateXForV(), pInXY->getOfsMinRateYForV() );
    pOutXY->setOfsMaxRateXYForV( -pInXY->getOfsMaxRateXForV(), pInXY->getOfsMaxRateYForV() );

    // 遅延データには左右の概念がないので何もしない
}

//--------------------------
// 鏡面コピー用確定：タテ
//--------------------------
void CAnchorPointData::fixForMirrorCopyV( void ){
    // タテ方向に座標を反転
    CAdjustablePoint* pXY = getAdjXY();
    pXY->setRateXY( pXY->getRateX(), -pXY->getRateY() );
    pXY->setOfsMinRateXYForH( pXY->getOfsMinRateXForH(), -pXY->getOfsMinRateYForH() );
    pXY->setOfsMaxRateXYForH( pXY->getOfsMaxRateXForH(), -pXY->getOfsMaxRateYForH() );
    pXY->setOfsMinRateXYForV( pXY->getOfsMinRateXForV(), -pXY->getOfsMinRateYForV() );
    pXY->setOfsMaxRateXYForV( pXY->getOfsMaxRateXForV(), -pXY->getOfsMaxRateYForV() );
    
    // 方向線の反転＆入れ替え
    CAdjustablePoint* pInXY = getAdjInXY();
    CAdjustablePoint* pOutXY = getAdjOutXY();
    
    float tempX = pInXY->getRateX();
    float tempY = pInXY->getRateY();
    pInXY->setRateXY( pOutXY->getRateX(), -pOutXY->getRateY() );
    pOutXY->setRateXY( tempX, -tempY );
    
    tempX = pInXY->getOfsMinRateXForH();
    tempY = pInXY->getOfsMinRateYForH();
    pInXY->setOfsMinRateXYForH( pOutXY->getOfsMinRateXForH(), -pOutXY->getOfsMinRateYForH() );
    pOutXY->setOfsMinRateXYForH( tempX, -tempY );
    
    tempX = pInXY->getOfsMaxRateXForH();
    tempY = pInXY->getOfsMaxRateYForH();
    pInXY->setOfsMaxRateXYForH( pOutXY->getOfsMaxRateXForH(), -pOutXY->getOfsMaxRateYForH() );
    pOutXY->setOfsMaxRateXYForH( tempX, -tempY );
    
    tempX = pInXY->getOfsMinRateXForV();
    tempY = pInXY->getOfsMinRateYForV();
    pInXY->setOfsMinRateXYForV( pOutXY->getOfsMinRateXForV(), -pOutXY->getOfsMinRateYForV() );
    pOutXY->setOfsMinRateXYForV( tempX, -tempY );
    
    tempX = pInXY->getOfsMaxRateXForV();
    tempY = pInXY->getOfsMaxRateYForV();
    pInXY->setOfsMaxRateXYForV( pOutXY->getOfsMaxRateXForV(), -pOutXY->getOfsMaxRateYForV() );
    pOutXY->setOfsMaxRateXYForV( tempX, -tempY );
    
    // 遅延データには上下の概念がないので何もしない
}

//------------------------
// 鏡面コピー調整：タテの先頭
//------------------------
void CAnchorPointData::fixAsHeadForMirrorCopyV( void ){
    // 出立方向線を進入方向線へコピー
    CAdjustablePoint* pInXY = getAdjInXY();
    CAdjustablePoint* pOutXY = getAdjOutXY();
    
    pInXY->setRateXY( pOutXY->getRateX(), -pOutXY->getRateY() );
    pInXY->setOfsMinRateXYForH( pOutXY->getOfsMinRateXForH(), -pOutXY->getOfsMinRateYForH() );
    pInXY->setOfsMaxRateXYForH( pOutXY->getOfsMaxRateXForH(), -pOutXY->getOfsMaxRateYForH() );
    pInXY->setOfsMinRateXYForV( pOutXY->getOfsMinRateXForV(), -pOutXY->getOfsMinRateYForV() );
    pInXY->setOfsMaxRateXYForV( pOutXY->getOfsMaxRateXForV(), -pOutXY->getOfsMaxRateYForV() );

    // 遅延データには上下の概念がないので何もしない
}

//------------------------
// 鏡面コピー調整：タテの末尾
//------------------------
void CAnchorPointData::fixAsTailForMirrorCopyV( void ){
    // 進入方向線を出立方向線へコピー
    CAdjustablePoint* pInXY = getAdjInXY();
    CAdjustablePoint* pOutXY = getAdjOutXY();
    
    pOutXY->setRateXY( pInXY->getRateX(), -pInXY->getRateY() );
    pOutXY->setOfsMinRateXYForH( pInXY->getOfsMinRateXForH(), -pInXY->getOfsMinRateYForH() );
    pOutXY->setOfsMaxRateXYForH( pInXY->getOfsMaxRateXForH(), -pInXY->getOfsMaxRateYForH() );
    pOutXY->setOfsMinRateXYForV( pInXY->getOfsMinRateXForV(), -pInXY->getOfsMinRateYForV() );
    pOutXY->setOfsMaxRateXYForV( pInXY->getOfsMaxRateXForV(), -pInXY->getOfsMaxRateYForV() );

    // 遅延データには上下の概念がないので何もしない
}

//------------
// スケール適用
//------------
void CAnchorPointData::applyRateScale( int rateScale ){
    m_oAdjXY.applyRateScale( rateScale );
    m_oAdjInXY.applyRateScale( rateScale );
    m_oAdjOutXY.applyRateScale( rateScale );
        
    // [m_eJointPointId]は無視
    // [m_eHookTargetId]は無視
    
    // [m_nStrokeSize]は無視
    // [m_nStrokeStartSize]は無視
    // [m_nStrokeStartRange]は無視
    // [m_nStrokeEndSize]は無視
    // [m_nStrokeEndRange]は無視
    // [m_nStrokeEdgeFillSize]は無視
    
    // [m_eTouchTargetId]は無視
    // [eSTROKE_FRILL m_eFrillId]は無視
    
    // [m_nP00_TouchBasePos]は無視
    // [m_nP01_TouchBaseSize]は無視
    // [m_nP02_TouchBaseRot]は無視
    // [m_nP03_TouchFrontArea]は無視
    // [m_nP04_TouchFrontSize]は無視
    // [m_nP05_TouchFrontRot]は無視
    // [m_nP06_TouchBackArea]は無視
    // [m_nP07_TouchBackSize]は無視
    // [m_nP08_TouchBackRot]は無視
    // [m_nP09_TouchFrontStep_FrillDummy]は無視
    // [m_nP10_TouchFrontStep2_FrillDummy]は無視
    // [m_nP11_TouchFrontStepRnd_FrillDummy]は無視
    // [m_nP12_TouchBackStep_FrillDummy]は無視
    // [m_nP13_TouchBackStep2_FrillDummy]は無視
    // [m_nP14_TouchBackStepRnd_FrillDummy]は無視
    
    // [m_eStripeMainPalOfsId]は無視
    // [m_eStripeSubPalOfsId]は無視
    
    // [m_eOptionForValid]は無視
    // [m_eOptionForInvalid]は無視
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CAnchorPointData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 12 + 15 + eAPD_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "JOINT", &m_eJointPointId, eEDIT_VALUE_TYPE_INT32,
                             eJOINT_POINT_INVALID, eJOINT_POINT_MAX-1, g_pArrLabelJointPoint );

    pMenu->setItemAtAsLabel( id++, "HOOK", &m_eHookTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "STROKE: SIZE", &m_nStrokeSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "STROKE: START_SIZE", &m_nStrokeStartSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "STROKE: START_RANGE", &m_nStrokeStartRange, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "STROKE: END_SIZE", &m_nStrokeEndSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "STROKE: END_RANGE", &m_nStrokeEndRange, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "STROKE: EDGE_FILL_SIZE", &m_nStrokeEdgeFillSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAtAsLabel( id++, "TOUCH", &m_eTouchTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAtAsLabel( id++, "FRILL: ID", &m_eFrillId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_FRILL_INVALID, eSTROKE_FRILL_MAX-1, g_pArrLabelStrokeFrill );

    pMenu->setSeparatorAt( id, true );
    
    // 親の状況によりデータの扱いが異なる
    if( m_bEditMenuForFrill ){
        pMenu->setItemAt( id++, "FRILL: BASE_POS", &m_nP00_TouchBasePos, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: BASE_SIZE", &m_nP01_TouchBaseSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: BASE_ROT", &m_nP02_TouchBaseRot, eEDIT_VALUE_TYPE_INT32,
                          BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
        pMenu->setItemAt( id++, "FRILL: FRONT_AREA", &m_nP03_TouchFrontArea, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: FRONT_SIZE", &m_nP04_TouchFrontSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: FRONT_ROT", &m_nP05_TouchFrontRot, eEDIT_VALUE_TYPE_INT32,
                          BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
        pMenu->setItemAt( id++, "FRILL: BACK_AREA", &m_nP06_TouchBackArea, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: BACK_SIZE", &m_nP07_TouchBackSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: BACK_ROT", &m_nP08_TouchBackRot, eEDIT_VALUE_TYPE_INT32,
                          BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
        pMenu->setItemAt( id++, "FRILL: ----", &m_nP09_TouchFrontStep_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: ----", &m_nP10_TouchFrontStep2_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: ----", &m_nP11_TouchFrontStepRnd_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: ----", &m_nP12_TouchBackStep_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: ----", &m_nP13_TouchBackStep2_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "FRILL: ----", &m_nP14_TouchBackStepRnd_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    }else{
        pMenu->setItemAt( id++, "TOUCH: BASE_POS", &m_nP00_TouchBasePos, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: BASE_SIZE", &m_nP01_TouchBaseSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: BASE_ROT", &m_nP02_TouchBaseRot, eEDIT_VALUE_TYPE_INT32,
                          BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
        pMenu->setItemAt( id++, "TOUCH: FRONT_AREA", &m_nP03_TouchFrontArea, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: FRONT_SIZE", &m_nP04_TouchFrontSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: FRONT_ROT", &m_nP05_TouchFrontRot, eEDIT_VALUE_TYPE_INT32,
                          BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
        pMenu->setItemAt( id++, "TOUCH: BACK_AREA", &m_nP06_TouchBackArea, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: BACK_SIZE", &m_nP07_TouchBackSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: BACK_ROT", &m_nP08_TouchBackRot, eEDIT_VALUE_TYPE_INT32,
                          BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
        pMenu->setItemAt( id++, "TOUCH: FRONT_STEP", &m_nP09_TouchFrontStep_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: FRONT_STEP_2", &m_nP10_TouchFrontStep2_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: FRONT_STEP_RND", &m_nP11_TouchFrontStepRnd_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: BACK_STEP", &m_nP12_TouchBackStep_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: BACK_STEP_2", &m_nP13_TouchBackStep2_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
        pMenu->setItemAt( id++, "TOUCH: BACK_STEP_RND", &m_nP14_TouchBackStepRnd_FrillDummy, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    }
    
    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAtAsLabel( id++, "STRIPE_MAIN_PAL_F_ID", &m_eStripeMainPalOfsId, eEDIT_VALUE_TYPE_INT32,
                            ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );
    pMenu->setItemAtAsLabel( id++, "STRIPE_SUB_PAL_F_ID", &m_eStripeSubPalOfsId, eEDIT_VALUE_TYPE_INT32,
                            ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: TRANSPARENCY", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_TRANSPARENCY );
    pMenu->setItemAtAsBit( id++, "FLAG: NO_FILL_GUIDE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_NO_FILL_GUIDE );
    pMenu->setItemAtAsBit( id++, "FLAG: LINE_REPAIR_TEST", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_LINE_REPAIR_TEST );
    pMenu->setItemAtAsBit( id++, "FLAG: JOINT_NO_HOOK_IF_OW", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_JOINT_NO_HOOK_IF_OVERWRAPPED );
    pMenu->setItemAtAsBit( id++, "FLAG: STRIPE_FILL_FRONT", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_STRIPE_FILL_FRONT_EDGE );
    pMenu->setItemAtAsBit( id++, "FLAG: STRIPE_FILL_BACK", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_STRIPE_FILL_BACK_EDGE );
    pMenu->setItemAtAsBit( id++, "FLAG: REVERSE_TOUCH_POINT", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_REVERSE_TOUCH_POINT );
    pMenu->setItemAtAsBit( id++, "FLAG: PUT_DOT_ON_FILL_OUT", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_DOT_PUT_ON_FILL_OUT );
    pMenu->setItemAtAsBit( id++, "FLAG: DOT_ERASE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_DOT_ERASE );
    pMenu->setItemAtAsBit( id++, "FLAG: JOINT_DIR_REV_X", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_JOINT_DIR_REVERSE_X );
    pMenu->setItemAtAsBit( id++, "FLAG: JOINT_DIR_REV_Y", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_JOINT_DIR_REVERSE_Y );
    pMenu->setItemAtAsBit( id++, "FLAG: JOINT_DIR_SWAP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_JOINT_DIR_SWAP );

    // 確定
    pMenu->fixMenu();
}
