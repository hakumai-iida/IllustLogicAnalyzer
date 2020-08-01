/*+----------------------------------------------------------------+
  |	Title:		FillPointData.cpp [共通環境]
  |	Comment:	塗りポイントデータ（※[CPaintObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FillPointData.hpp"
#include "draw/tex/stroke/Stroke.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_FILL_POINT_DATA   (4)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CFillPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_FILL_POINT_DATA )

// 塗りオプションアンチの最低サイズ（小さすぎるとジャギるので）
#define FILL_OPTION_ANTI_SIZE_MIN   2

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
int CFillPointData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CFillPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_FILL_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CFillPointData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CFillPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_FILL_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CFillPointData) * MAX_ALLOC_FILL_POINT_DATA;
    
    return( allocSize );
}

//---------------------------
// コンストラクタ
//---------------------------
CFillPointData::CFillPointData( void ): CListNode(){
	clear();
}

//---------------------------
// クリア（※データの破棄）
//---------------------------
void CFillPointData::clear( void ){
    m_nFlag = 0;
    m_oAdjXY.clear();
    
    m_ePalOfsId = ePAL_OFS_DEFAULT;
    
    m_eFillOption = eFILL_OPTION_INVALID;
    m_eTouchTargetIdForOption = eSTROKE_TOUCH_TARGET_INVALID;
    m_nStrokeSizeForOption = FILL_OPTION_ANTI_SIZE_MIN;
    m_nStrokeRangeForOption = 0;
    m_nOfsXForOption = 0;
    m_nOfsYForOption = 0;
}

//--------------------
// 設定
//--------------------
void CFillPointData::set( int rX, int rY, ePAL_OFS palOfsId ){
    m_oAdjXY.setRateXY( rX, rY );
    m_ePalOfsId = palOfsId;
}

//-------------------------
// 設定：オプション
//-------------------------
void CFillPointData::setOption( eFILL_OPTION option, eSTROKE_TOUCH_TARGET touchId ){
    m_eFillOption = option;
    m_eTouchTargetIdForOption = touchId;
}

//-------------------------
// 設定：オプションストローク
//-------------------------
void CFillPointData::setOptionStroke( int size, int range ){
    m_nStrokeSizeForOption = size;
    m_nStrokeRangeForOption = range;
}

//-------------------------
// 設定：オプションストローク
//-------------------------
void CFillPointData::setOptionOfsXY( int ofsX, int ofsY ){
    m_nOfsXForOption = ofsX;
    m_nOfsYForOption = ofsY;
}

//--------------------
// コピー
//--------------------
void CFillPointData::copy( CFillPointData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_nFlag = pData->m_nFlag;
    m_oAdjXY.copy( &pData->m_oAdjXY );
    
    m_ePalOfsId = pData->m_ePalOfsId;
    
    m_eFillOption = pData->m_eFillOption;
    m_eTouchTargetIdForOption = pData->m_eTouchTargetIdForOption;
    m_nStrokeSizeForOption = pData->m_nStrokeSizeForOption;
    m_nStrokeRangeForOption = pData->m_nStrokeRangeForOption;
    m_nOfsXForOption = pData->m_nOfsXForOption;
    m_nOfsYForOption = pData->m_nOfsYForOption;
}

//---------------------------
// 読み込み
//---------------------------
void CFillPointData::read( CInputBuffer* pIB ){
    m_nFlag = pIB->readInt16();
    m_oAdjXY.read( pIB );

    m_ePalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
    
    m_eFillOption = (eFILL_OPTION)CEnum::ReadEnum( pIB, g_pArrLabelFillOption );
    m_eTouchTargetIdForOption = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );

    m_nStrokeSizeForOption = pIB->readInt16();
    m_nStrokeRangeForOption = pIB->readInt16();

    m_nOfsXForOption = pIB->readInt16();
    m_nOfsYForOption = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CFillPointData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    m_oAdjXY.write( pOB );
    
    CEnum::WriteEnum( pOB, m_ePalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
    
    CEnum::WriteEnum( pOB, m_eFillOption, eFILL_OPTION_MAX, g_pArrLabelFillOption );
    CEnum::WriteEnum( pOB, m_eTouchTargetIdForOption, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );

    pOB->writeInt16( (short)m_nStrokeSizeForOption );
    pOB->writeInt16( (short)m_nStrokeRangeForOption );

    pOB->writeInt16( (short)m_nOfsXForOption );
    pOB->writeInt16( (short)m_nOfsYForOption );
}

//---------------------------
// タッチ対象IDの取得
//---------------------------
eSTROKE_TOUCH_TARGET CFillPointData::getTouchTargetIdForOption( int slotIndex ){
    return( CStroke::AdjustTouchTargetForSlotIndex( m_eTouchTargetIdForOption, slotIndex ) );
}

//------------
// スケール適用
//------------
void CFillPointData::applyRateScale( int rateScale ){
    m_oAdjXY.applyRateScale( rateScale );
    
    // [m_ePalOfsId]は無視
    
    // [m_eFillOption]は無視
    // [m_eTouchTargetIdForOption]は無視
    m_nStrokeSizeForOption = m_nStrokeSizeForOption*rateScale/BEZIER_SCALE_RATE;
    m_nStrokeRangeForOption = m_nStrokeRangeForOption*rateScale/BEZIER_SCALE_RATE;
    m_nOfsXForOption = m_nOfsXForOption*rateScale/BEZIER_SCALE_RATE;
    m_nOfsYForOption = m_nOfsYForOption*rateScale/BEZIER_SCALE_RATE;
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CFillPointData::setEditValueMenu( CEditValueMenu* pMenu ){
	// 編集項目数設定
	pMenu->setItemNum( 7 + eFPD_FLAG_MAX );

	int id = 0;

	// 値
    pMenu->setItemAtAsLabel( id++, "PAL_OFS_ID", &m_ePalOfsId, eEDIT_VALUE_TYPE_INT32,
                             ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAtAsLabel( id++, "OPTION: TYPE", &m_eFillOption, eEDIT_VALUE_TYPE_INT32,
                             eFILL_OPTION_INVALID, eFILL_OPTION_MAX-1, g_pArrLabelFillOption );
    pMenu->setItemAtAsLabel( id++, "OPTION: TOUCH_TARGET_ID", &m_eTouchTargetIdForOption, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAt( id++, "OPTION: STROKE_SIZE", &m_nStrokeSizeForOption, eEDIT_VALUE_TYPE_INT32, FILL_OPTION_ANTI_SIZE_MIN, 15 );
    pMenu->setItemAt( id++, "OPTION: STROKE_RANGE", &m_nStrokeRangeForOption, eEDIT_VALUE_TYPE_INT32, 0, 15 );

    pMenu->setItemAt( id++, "OPTION: OFS_X", &m_nOfsXForOption, eEDIT_VALUE_TYPE_INT32, -99, 99 );
    pMenu->setItemAt( id++, "OPTION: OFS_Y", &m_nOfsYForOption, eEDIT_VALUE_TYPE_INT32, -99, 99 );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: TRANSPARENCY", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_TRANSPARENCY );
    
	// 確定
	pMenu->fixMenu();
}
