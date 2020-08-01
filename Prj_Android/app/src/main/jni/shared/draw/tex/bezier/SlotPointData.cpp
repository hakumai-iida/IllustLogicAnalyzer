/*+----------------------------------------------------------------+
  |	Title:		SlotPointData.cpp [共通環境]
  | Comment:    スロットポイントデータ（※[CSlotObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "SlotPointData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_SLOT_POINT_DATA   (4)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_POINT_DATA )

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
int CSlotPointData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CSlotPointData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CSlotPointData) * MAX_ALLOC_SLOT_POINT_DATA;
    
    return( allocSize );
}

//---------------------------
// コンストラクタ
//---------------------------
CSlotPointData::CSlotPointData( void ): CListNode(){
	clear();
}

//----------------------------------------------------------------------------
// クリア（※データの破棄）
//----------------------------------------------------------------------------
void CSlotPointData::clear( void ){
    m_nFlag = 0;
    m_oAdjXY.clear();
}

//--------------------
// 設定
//--------------------
void CSlotPointData::set( int rX, int rY ){
    m_oAdjXY.setRateXY( rX, rY );
}

//--------------------
// コピー
//--------------------
void CSlotPointData::copy( CSlotPointData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_nFlag = pData->m_nFlag;
    m_oAdjXY.copy( &pData->m_oAdjXY );
}

//---------------------------
// 読み込み
//---------------------------
void CSlotPointData::read( CInputBuffer* pIB ){
    m_nFlag = pIB->readInt16();
    m_oAdjXY.read( pIB );
}

//---------------------------
// 書き込み
//---------------------------
void CSlotPointData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    m_oAdjXY.write( pOB );
}

//------------
// スケール適用
//------------
void CSlotPointData::applyRateScale( int rateScale ){
    m_oAdjXY.applyRateScale( rateScale );
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CSlotPointData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( eSPD_FLAG_MAX );
    
    int id = 0;
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eSPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: FLIP_H", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eSPD_FLAG_FLIP_H );
    pMenu->setItemAtAsBit( id++, "FLAG: FLIP_V", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eSPD_FLAG_FLIP_V );
    
    // 確定
    pMenu->fixMenu();
}
