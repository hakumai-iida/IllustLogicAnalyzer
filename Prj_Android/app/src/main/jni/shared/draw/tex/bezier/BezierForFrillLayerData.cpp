/*+----------------------------------------------------------------+
  |	Title:		BezierForFrillLayerData.cpp [共通環境]
  |	Comment:	ベジェ曲線：フリルのレイヤーデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"
#include "BezierDataConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// フリルデータ確保
#define ALLOC_FRILL_THEN_CHECK( _frillId )                          \
    s_pArrLayerDataForFrill[(_frillId)] = CLayerData::Alloc();      \
    if( s_pArrLayerDataForFrill[(_frillId)] == NULL ){              \
        return( false );                                            \
    }

// フリルデータ解放
#define RELEASE_FRILL( _frillId )                                   \
    if( s_pArrLayerDataForFrill[(_frillId)] != NULL ){              \
        CLayerData::Free( s_pArrLayerDataForFrill[(_frillId)] );    \
        s_pArrLayerDataForFrill[(_frillId)] = NULL;                 \
    }

// フリル確保処理ワーク
#define ALLOC_FRILL_WORK( _frillId )                                \
    CLayerData* pLD = s_pArrLayerDataForFrill[(_frillId)];          \
    CLineObjectData* pLOD;                                          \
    CAnchorPointData* pAP;                                          \
    CPaintObjectData* pPOD;                                         \
    CFillPointData* pFP;                                            \
    eBRUSH brush = eBRUSH_C_BASE;                                   \
    eBUCKET bucket = eBUCKET_TEST_MONO;                             \
    ePAL_OFS bucketPalOfs = ePAL_OFS_C_SKIN

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
//-----------------------------------
// [FrillLayerData] フリルデータの取得
//-----------------------------------
CLayerData* CBezier::GetLayerDataForFrill( eSTROKE_FRILL frillId ){
    // 用心
    if( frillId < 0 || frillId >= eSTROKE_FRILL_MAX ){
        LOGE( "@ CBezier::GetLayerDataForFrill: INVALID: frillId=%d\n", frillId );
        return( NULL );
    }
    
    return( s_pArrLayerDataForFrill[frillId] );
}

//-----------------------------------
// [FrillLayerData] フリルデータ確保
//-----------------------------------
bool CBezier::AllocFrill( void ){
    if( ! AllocFrillForTestCircle() ){ return( false ); }
    if( ! AllocFrillForTestTriangle() ){ return( false ); }
    if( ! AllocFrillForTestSquare() ){ return( false ); }
    
    return( true );
}

//-----------------------------------
// [FrillLayerData] フリルデータ解放
//-----------------------------------
void CBezier::ReleaseFrill( void ){
    for( int i=0; i<eSTROKE_FRILL_MAX; i++ ){
        RELEASE_FRILL( i );
    }
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_TEST_CIRCLE
//--------------------------------------------------------
bool CBezier::AllocFrillForTestCircle( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_TEST_CIRCLE;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );

    //-----------------------------------
    // [TEST]: 外円
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pLOD->addData( pAP );

    //-----------------------------------
    // [TEST]: 内円（※塗りに影響させない）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 0,0, 5000, 5000 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0, bucketPalOfs );
    pPOD->addData( pFP );

    return( true );
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_TEST_TRIANGLE
//--------------------------------------------------------
bool CBezier::AllocFrillForTestTriangle( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_TEST_TRIANGLE;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );

    //-----------------------------------
    // [TEST]: 三角外側
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1500, 0,-1500 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-3750, -500,0, 500,0 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1500, 0,1500 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pLOD->addData( pAP );
    
    //-----------------------------------
    // [TEST]: 三角内側（※塗りに影響させない）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 0,0, 5000, 5000 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1500, 0,-1500 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-3750-250, -500,0, 500,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1500, 0,1500 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0, bucketPalOfs );
    pPOD->addData( pFP );
    
    return( true );
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_TEST_SQUARE
//--------------------------------------------------------
bool CBezier::AllocFrillForTestSquare( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_TEST_SQUARE;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );
    
    //-----------------------------------
    // [TEST]: 内四角（※塗りに影響させない）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setFlagOn( eLOD_FLAG_GUIDE_RESET_AFTER_STROKE );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1000,-1500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1000,-1500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1000,1000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1000,1000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //-----------------------------------
    // [TEST]: 外円
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-2500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-2500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,2000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,2000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0, bucketPalOfs );
    pPOD->addData( pFP );
    
    return( true );
}
