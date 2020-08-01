/*+----------------------------------------------------------------+
  |	Title:		Bezier.cpp [共通環境]
  |	Comment:	ベジェ曲線（※対象はパレット画像[P8D]）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 分割ポイント確保枠
#define BEZIER_DIVISION_POINT_MAX       (1024*GM_P_RATE)

// テンポラリ確保枠
#define BEZIER_TEMP_POINT_MAX           (2048*GM_P_RATE)

// フック枠
#define BEZIER_HOOK_POINT_MAX           (eSTROKE_HOOK_TARGET_MAX+eJOINT_POINT_MAX)

// タッチポイント確保数（※１枠分）
#define BEZIER_TOUCH_POINT_MAX          BEZIER_TEMP_POINT_MAX

// ストライプポイント確保数（※ここはストロークによる分割数なのでピクセル率は関係ない）
#define BEZIER_STRIPE_POINT_MAX         128

// 塗りガイドポイント確保枠
#define BEZIER_STRIPE_GUIDE_POINT_MAX   BEZIER_TEMP_POINT_MAX

// ワークパスのテクスチャサイズ（※ワークなので[GM_P_RATE]は無視しておく）
#define BEZIER_WORK_DOT_SIZE            1024

// エッジ塗りつぶし基本サイズ（※[3*10*2*GM_P_RATE]あれば大丈夫だと思われるが用心して大きめに）
#define BEZIER_EDGE_FILL_GUIDE_SIZE     (64*GM_P_RATE)

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 制御
bool CBezier::s_bDispDirVectorAlways;
bool CBezier::s_bDispOffDirVectorAlways;
bool CBezier::s_bDispPathActiveAlways;
bool CBezier::s_bDispOnlyAnchorPoint;

// 分割点ワーク
stBEZIER_DIVISION_POINT* CBezier::s_stArrDivisionPoint;
int CBezier::s_nUseDivisionPoint;
int CBezier::s_nUseDivisionPointMax;

// テンポラリ
float* CBezier::s_fArrTempPointX;
float* CBezier::s_fArrTempPointY;
float* CBezier::s_fArrTempPointX0;
float* CBezier::s_fArrTempPointY0;
float* CBezier::s_fArrTempStrokeSize;

// フック点ワーク
bool* CBezier::s_bArrHookPoint;
float* CBezier::s_fArrHookPointX;
float* CBezier::s_fArrHookPointY;

// タッチワーク
int* CBezier::s_nArrTouchPointNum;
float** CBezier::s_fArrArrTouchPointX;
float** CBezier::s_fArrArrTouchPointY;
float* CBezier::s_fArrTouchPoint;
float** CBezier::s_fArrArrTouchPointX0;
float** CBezier::s_fArrArrTouchPointY0;
float* CBezier::s_fArrTouchPoint0;
float* CBezier::s_fArrTouchWorkForLen0;

int CBezier::s_nTouchStrokePointCur;
int CBezier::s_nTouchStrokePointMax;
int CBezier::s_nTouchStrokePointLimit;

// ストライプワーク
int CBezier::s_nStripePointNum;
BYTE* CBezier::s_nArrStripePalGrp;
float* CBezier::s_fArrStripePointX;
float* CBezier::s_fArrStripePointY;

// ストライプ用ガイドワーク
int CBezier::s_nNumStripeGuidePoint;
int* CBezier::s_nArrStripeGuidePointX;
int* CBezier::s_nArrStripeGuidePointY;

// エッジ塗り
int CBezier::s_nBufSizeForEdgeFillGuide;
BYTE* CBezier::s_pBufForEdgeFillGuide;
int CBezier::s_nOfsXForEdgeFillGuide;
int CBezier::s_nOfsYForEdgeFillGuide;
stBEZIER_ANCHOR_POINT* CBezier::s_pApForEdgeFillGuide;

// バッファ情報
int CBezier::s_nLeftForStrokeBuf;
int CBezier::s_nRightForStrokeBuf;
int CBezier::s_nTopForStrokeBuf;
int CBezier::s_nBottomForStrokeBuf;
int CBezier::s_nLeftForColorBuf;
int CBezier::s_nRightForColorBuf;
int CBezier::s_nTopForColorBuf;
int CBezier::s_nBottomForColorBuf;

// パラメータ
stBEZIER_BASE_PARAM*  CBezier::s_pBaseParam;
stBEZIER_LAYER_PARAM* CBezier::s_pLayerParam;

// 塗りつぶし対象
BYTE* CBezier::s_pBuf;
BYTE* CBezier::s_pBufFillGuide;
BYTE* CBezier::s_pBufFillGuard;
BYTE* CBezier::s_pBufTest;
int CBezier::s_nBufW;
int CBezier::s_nBufH;
BYTE CBezier::s_nTestPalGrp;
BYTE CBezier::s_nTestPalGrpForRepair;
bool CBezier::s_bTouch;
bool CBezier::s_bStripe;
bool CBezier::s_bFrill;
bool CBezier::s_bWorkPath;

// ワークパス用
int CBezier::s_nSizeForWorkPathLine;
int CBezier::s_nSizeForWorkPathPoint;
int CBezier::s_nSizeForWorkPathSelected;
int CBezier::s_nSizeForWorkPathTriangle;
int CBezier::s_nSizeForWorkPathCross;
int CBezier::s_nSizeForWorkPathPlus;
BYTE* CBezier::s_pDotForWorkPathTriangle;
BYTE* CBezier::s_pDotForWorkPathCross;
BYTE* CBezier::s_pDotForWorkPathPlus;

// フリルデータ
CLayerData* CBezier::s_pArrLayerDataForFrill[eSTROKE_FRILL_MAX];

// ブラシのブレ用
CSpring CBezier::s_oSpringForBrushPos;
CSpring CBezier::s_oSpringForBrushSize;
int CBezier::m_nResetCountForBrushPos;
int CBezier::m_nResetCountForBrushSize;
float CBezier::m_nUpdateFrameForBrushPos;
float CBezier::m_nUpdateFrameForBrushSize;

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------------------------
// 確保数（※管理内で[new]するセル数）
//--------------------------------------
int CBezier::GetAllocCellNum( void ){
    int num = 0;

    //---------------------------------
    // CAnchorPointData::Init()
    // CFillPointData::Init()
    // CSlotPointData::Init()
    // CLineObjectData::Init()
    // CPaintObjectData::Init()
    // CSlotObjectData::Init()
    // CLayerData::Init()
    //---------------------------------
    num += CAnchorPointData::GetAllocCellNum();
    num += CFillPointData::GetAllocCellNum();
    num += CSlotPointData::GetAllocCellNum();
    num += CLineObjectData::GetAllocCellNum();
    num += CPaintObjectData::GetAllocCellNum();
    num += CSlotObjectData::GetAllocCellNum();
    num += CLayerData::GetAllocCellNum();

    //---------------------------------------------------------------------------------
    // s_stArrDivisionPoint = new stBEZIER_DIVISION_POINT[BEZIER_DIVISION_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 1;

    //---------------------------------------------------------------------------------
    // s_fArrTempPointX = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointX0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempStrokeSize = new float[BEZIER_TEMP_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 5;

    //---------------------------------------------------------------------------------
    // s_nArrStripeGuidePointX = new int[BEZIER_STRIPE_GUIDE_POINT_MAX];
    // s_nArrStripeGuidePointY = new int[BEZIER_STRIPE_GUIDE_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 2;

    //---------------------------------------------------------------------------------
    // s_bArrHookPoint = new bool[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointX = new float[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointY = new float[BEZIER_HOOK_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 3;

    //---------------------------------------------------------------------------------
    // s_nArrTouchPointNum = new int[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrArrTouchPointX = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrArrTouchPointY = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrTouchPoint = new float[eSTROKE_TOUCH_TARGET_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrArrTouchPointX0 = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrArrTouchPointY0 = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrTouchPoint0 = new float[eSTROKE_TOUCH_TARGET_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 8;

    //---------------------------------------------------------------------------------
    // s_nArrStripePalGrp = new BYTE[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointX = new float[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointY = new float[BEZIER_STRIPE_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 3;

    //---------------------------------------------------------------------------------
    // s_nBufSizeForEdgeFillGuide = BEZIER_EDGE_FILL_GUIDE_SIZE;
    // s_pBufForEdgeFillGuide = new BYTE[s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide];
    //---------------------------------------------------------------------------------
    num += 1;

    //---------------------------------------------------------------------------------
    // s_pDotForWorkPathTriangle = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathCross = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathPlus = new BYTE[BEZIER_WORK_DOT_SIZE];
    //---------------------------------------------------------------------------------
    num += 3;
    
    return( num );
}

//--------------------------------------
// 確保サイズ（※管理内で[new]する領域サイズ）
//--------------------------------------
int CBezier::GetAllocSizeK( void ){
    int size = 0;

    //---------------------------------
    // CAnchorPointData::Init()
    // CFillPointData::Init()
    // CSlotPointData::Init()
    // CLineObjectData::Init()
    // CPaintObjectData::Init()
    // CSlotObjectData::Init()
    // CLayerData::Init()
    //---------------------------------
    size += CAnchorPointData::GetAllocSize();
    size += CFillPointData::GetAllocSize();
    size += CSlotPointData::GetAllocSize();
    size += CLineObjectData::GetAllocSize();
    size += CPaintObjectData::GetAllocSize();
    size += CSlotObjectData::GetAllocSize();
    size += CLayerData::GetAllocSize();

    //---------------------------------------------------------------------------------
    // s_stArrDivisionPoint = new stBEZIER_DIVISION_POINT[BEZIER_DIVISION_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(stBEZIER_DIVISION_POINT) * BEZIER_DIVISION_POINT_MAX;
    
    //---------------------------------------------------------------------------------
    // s_fArrTempPointX = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointX0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempStrokeSize = new float[BEZIER_TEMP_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += 5 * sizeof(float) * BEZIER_TEMP_POINT_MAX;

    //---------------------------------------------------------------------------------
    // s_nArrStripeGuidePointX = new int[BEZIER_STRIPE_GUIDE_POINT_MAX];
    // s_nArrStripeGuidePointY = new int[BEZIER_STRIPE_GUIDE_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += 2 * sizeof(int) * BEZIER_STRIPE_GUIDE_POINT_MAX;

    //---------------------------------------------------------------------------------
    // s_bArrHookPoint = new bool[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointX = new float[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointY = new float[BEZIER_HOOK_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(bool) * BEZIER_HOOK_POINT_MAX;
    size += 2 * sizeof(float) * BEZIER_HOOK_POINT_MAX;
    
    //---------------------------------------------------------------------------------
    // s_nArrTouchPointNum = new int[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrArrTouchPointX = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrArrTouchPointY = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrTouchPoint = new float[eSTROKE_TOUCH_TARGET_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrArrTouchPointX0 = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrArrTouchPointY0 = new float*[eSTROKE_TOUCH_TARGET_MAX];
    // s_fArrTouchPoint0 = new float[eSTROKE_TOUCH_TARGET_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(int) * eSTROKE_TOUCH_TARGET_MAX;
    size += 2 * sizeof(float*) * eSTROKE_TOUCH_TARGET_MAX;
    size += sizeof(float) * eSTROKE_TOUCH_TARGET_MAX * 2 * BEZIER_TOUCH_POINT_MAX;
    size += 2 * sizeof(float*) * eSTROKE_TOUCH_TARGET_MAX;
    size += sizeof(float) * eSTROKE_TOUCH_TARGET_MAX * 2 * BEZIER_TOUCH_POINT_MAX;
    size += sizeof(float) * BEZIER_TOUCH_POINT_MAX;

    //---------------------------------------------------------------------------------
    // s_nArrStripePalGrp = new BYTE[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointX = new float[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointY = new float[BEZIER_STRIPE_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(BYTE) * BEZIER_STRIPE_POINT_MAX;
    size += 2 * sizeof(float) * BEZIER_STRIPE_POINT_MAX;
    
    //---------------------------------------------------------------------------------
    // s_nBufSizeForEdgeFillGuide = BEZIER_EDGE_FILL_GUIDE_SIZE;
    // s_pBufForEdgeFillGuide = new BYTE[s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide];
    //---------------------------------------------------------------------------------
    size += sizeof(BYTE) * BEZIER_EDGE_FILL_GUIDE_SIZE * BEZIER_EDGE_FILL_GUIDE_SIZE;

    //---------------------------------------------------------------------------------
    // s_pDotForWorkPathTriangle = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathCross = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathPlus = new BYTE[BEZIER_WORK_DOT_SIZE];
    //---------------------------------------------------------------------------------
    size += 3 * sizeof(BYTE) * BEZIER_WORK_DOT_SIZE;

    // キロ単位にして返す
    int sizeK = (size+KB-1)/KB;
    return( sizeK );
}

//-------------------
// 開始
//-------------------
bool CBezier::OnCreate( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBezier::OnCreate()\n" );
#endif

    //-------------------------------------------------
    // [IMPLEMENT_DATA_ALLOCATOR]系管理クラスの初期化
    //-------------------------------------------------
    // アンカーポイントデータ管理初期化
    if( ! CAnchorPointData::Init() ){
        LOGE( "@ CAnchorPointData::Init(): FAILED!!\n" );
        env_push_error_log( "[CAnchorPointData::Init()]に失敗しました。\n" );
        return( false );
    }

    // 塗りポイントデータ管理初期化
    if( ! CFillPointData::Init() ){
        LOGE( "@ CFillPointData::Init(): FAILED!!\n" );
        env_push_error_log( "[CFillPointData::Init()]に失敗しました。\n" );
        return( false );
    }

    // スロットポイントデータ管理初期化
    if( ! CSlotPointData::Init() ){
        LOGE( "@ CSlotPointData::Init(): FAILED!!\n" );
        env_push_error_log( "[CSlotPointData::Init()]に失敗しました。\n" );
        return( false );
    }

    // ラインオブジェクトデータ管理初期化
    if( ! CLineObjectData::Init() ){
        LOGE( "@ CLineObjectData::Init(): FAILED!!\n" );
        env_push_error_log( "[CLineObjectData::Init()]に失敗しました。\n" );
        return( false );
    }

    // ペイントオブジェクトデータ管理初期化
    if( ! CPaintObjectData::Init() ){
        LOGE( "@ CPaintObjectData::Init(): FAILED!!\n" );
        env_push_error_log( "[CPaintObjectData::Init()]に失敗しました。\n" );
        return( false );
    }
    
    // スロットオブジェクトデータ管理初期化
    if( ! CSlotObjectData::Init() ){
        LOGE( "@ CSlotObjectData::Init(): FAILED!!\n" );
        env_push_error_log( "[CSlotObjectData::Init()]に失敗しました。\n" );
        return( false );
    }

    // レイヤーデータ管理初期化
    if( ! CLayerData::Init() ){
        LOGE( "@ CLayerData::Init(): FAILED!!\n" );
        env_push_error_log( "[CLayerData::Init()]に失敗しました。\n" );
        return( false );
    }
    
    // フリルデータ確保
    if( ! AllocFrill() ){
        LOGE( "@ AllocInit(): FAILED!!\n" );
        env_push_error_log( "[CBezier::AllocFrill()]に失敗しました。\n" );
        return( false );
    }
    
    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_BEZIER );
    s_stArrDivisionPoint = new stBEZIER_DIVISION_POINT[BEZIER_DIVISION_POINT_MAX];
    
    s_fArrTempPointX = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempPointY = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempPointX0 = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempPointY0 = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempStrokeSize = new float[BEZIER_TEMP_POINT_MAX];
    
    s_nArrStripeGuidePointX = new int[BEZIER_STRIPE_GUIDE_POINT_MAX];
    s_nArrStripeGuidePointY = new int[BEZIER_STRIPE_GUIDE_POINT_MAX];
    
    s_bArrHookPoint = new bool[BEZIER_HOOK_POINT_MAX];
    s_fArrHookPointX = new float[BEZIER_HOOK_POINT_MAX];
    s_fArrHookPointY = new float[BEZIER_HOOK_POINT_MAX];
    
    s_nArrTouchPointNum = new int[eSTROKE_TOUCH_TARGET_MAX];
    s_fArrArrTouchPointX = new float*[eSTROKE_TOUCH_TARGET_MAX];
    s_fArrArrTouchPointY = new float*[eSTROKE_TOUCH_TARGET_MAX];
    s_fArrTouchPoint = new float[eSTROKE_TOUCH_TARGET_MAX*2*BEZIER_TOUCH_POINT_MAX];
    s_fArrArrTouchPointX0 = new float*[eSTROKE_TOUCH_TARGET_MAX];
    s_fArrArrTouchPointY0 = new float*[eSTROKE_TOUCH_TARGET_MAX];
    s_fArrTouchPoint0 = new float[eSTROKE_TOUCH_TARGET_MAX*2*BEZIER_TOUCH_POINT_MAX];
    s_fArrTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];

    s_nArrStripePalGrp = new BYTE[BEZIER_STRIPE_POINT_MAX];
    s_fArrStripePointX = new float[BEZIER_STRIPE_POINT_MAX];
    s_fArrStripePointY = new float[BEZIER_STRIPE_POINT_MAX];
    
    s_nBufSizeForEdgeFillGuide = BEZIER_EDGE_FILL_GUIDE_SIZE;
    s_pBufForEdgeFillGuide = new BYTE[s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide];

    // ワークパス用
    s_pDotForWorkPathTriangle = new BYTE[BEZIER_WORK_DOT_SIZE];
    s_pDotForWorkPathCross = new BYTE[BEZIER_WORK_DOT_SIZE];
    s_pDotForWorkPathPlus = new BYTE[BEZIER_WORK_DOT_SIZE];
    CMemMgr::PopTargetField();

    // タッチXY配列へバッファの割り当て
    for( int i=0; i<eSTROKE_TOUCH_TARGET_MAX; i++ ){
        s_fArrArrTouchPointX[i] = &s_fArrTouchPoint[(2*i+0)*BEZIER_TOUCH_POINT_MAX];
        s_fArrArrTouchPointY[i] = &s_fArrTouchPoint[(2*i+1)*BEZIER_TOUCH_POINT_MAX];
        s_fArrArrTouchPointX0[i] = &s_fArrTouchPoint0[(2*i+0)*BEZIER_TOUCH_POINT_MAX];
        s_fArrArrTouchPointY0[i] = &s_fArrTouchPoint0[(2*i+1)*BEZIER_TOUCH_POINT_MAX];
    }
    
    // ワークパス用ドット準備
    ReadyDotForWorkPath();
    
    return( true );
}

//-------------------
// 終了
//-------------------
void CBezier::OnDestroy( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBezier::OnDestroy()\n" );
#endif

    // 領域解放
    SAFE_DELETE_ARRAY( s_stArrDivisionPoint );

    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointX );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointY );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointX0 );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointY0 );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempStrokeSize );
    
    SAFE_DELETE_ARRAY( CBezier::s_nArrStripeGuidePointX );
    SAFE_DELETE_ARRAY( CBezier::s_nArrStripeGuidePointY );

    SAFE_DELETE_ARRAY( s_bArrHookPoint );
    SAFE_DELETE_ARRAY( s_fArrHookPointX );
    SAFE_DELETE_ARRAY( s_fArrHookPointY );

    SAFE_DELETE_ARRAY( s_nArrTouchPointNum );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointX );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointY );
    SAFE_DELETE_ARRAY( s_fArrTouchPoint );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointX0 );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointY0 );
    SAFE_DELETE_ARRAY( s_fArrTouchPoint0 );
    SAFE_DELETE_ARRAY( s_fArrTouchWorkForLen0 );

    SAFE_DELETE_ARRAY( s_nArrStripePalGrp );
    SAFE_DELETE_ARRAY( s_fArrStripePointX );
    SAFE_DELETE_ARRAY( s_fArrStripePointY );
    
    SAFE_DELETE_ARRAY( s_pBufForEdgeFillGuide );

    // ワークパス用
    SAFE_DELETE_ARRAY( s_pDotForWorkPathTriangle );
    SAFE_DELETE_ARRAY( s_pDotForWorkPathCross );
    SAFE_DELETE_ARRAY( s_pDotForWorkPathPlus );

    // フリル解放
    ReleaseFrill();
    
    //-------------------------------------------------
    // [IMPLEMENT_DATA_ALLOCATOR]系管理クラスの終了
    //-------------------------------------------------
    CLayerData::Exit();
    CSlotObjectData::Exit();
    CPaintObjectData::Exit();
    CLineObjectData::Exit();
    CSlotPointData::Exit();
    CFillPointData::Exit();
    CAnchorPointData::Exit();
}

//------------------------
// 情報取得
//------------------------
int CBezier::GetDivisionPointNumMax( void ){ return( BEZIER_DIVISION_POINT_MAX ); }
int CBezier::GetDivisionPointNumUseMax( void ){ return( s_nUseDivisionPointMax ); }

//----------------------
// バッファ情報リセット
//----------------------
void CBezier::ResetBufInfo( int w, int h ){
    s_nLeftForStrokeBuf = w;
    s_nRightForStrokeBuf = -1;
    s_nTopForStrokeBuf = h;
    s_nBottomForStrokeBuf = -1;
    s_nLeftForColorBuf = w;
    s_nRightForColorBuf = -1;
    s_nTopForColorBuf = h;
    s_nBottomForColorBuf = -1;
}

//----------------------
// フック登録リセット
//----------------------
void CBezier::ResetHookPoint( bool isAll ){
    if( isAll ){
        // 全リセット
        for( int i=0; i<BEZIER_HOOK_POINT_MAX; i++ ){
            s_bArrHookPoint[i] = false;
        }
    }else{
        // テンポラリだけリセット
        for( int i=eSTROKE_HOOK_TARGET_TEMP_START; i<=eSTROKE_HOOK_TARGET_TEMP_END; i++ ){
            s_bArrHookPoint[i] = false;
        }
    }
}

//----------------------
// タッチ登録リセット
//----------------------
void CBezier::ResetTouchPoint( bool isAll ){
    if( isAll ){
        // 全リセット
        for( int i=0; i<eSTROKE_TOUCH_TARGET_MAX; i++ ){
            s_nArrTouchPointNum[i] = 0;
        }
    }else{
        // テンポラリだけリセット
        for( int i=eSTROKE_TOUCH_TARGET_TEMP_START; i<=eSTROKE_TOUCH_TARGET_TEMP_END; i++ ){
            s_nArrTouchPointNum[i] = 0;
        }
    }
}

//------------------------
// 全フラグリセット（※開発用）
//------------------------
void CBezier::ResetAllFlagForDev( void ){
    // LayerData
    CLayerData::SetFlagOffAll( eLAYER_FLAG_TEMP_SELECTED );
    CLayerData::SetFlagOffAll( eLAYER_FLAG_TEMP_ACTIVE );

    // LineObjectData
    CLineObjectData::SetFlagOffAll( eLOD_FLAG_TEMP_SELECTED );
    CLineObjectData::SetFlagOffAll( eLOD_FLAG_TEMP_ACTIVE );

    // PaintObjectData
    CPaintObjectData::SetFlagOffAll( ePOD_FLAG_TEMP_SELECTED );
    CPaintObjectData::SetFlagOffAll( ePOD_FLAG_TEMP_ACTIVE );

    // SlotObjectData
    CSlotObjectData::SetFlagOffAll( eSOD_FLAG_TEMP_SELECTED );
    CSlotObjectData::SetFlagOffAll( eSOD_FLAG_TEMP_ACTIVE );

    // AnchorPointData
    CAnchorPointData::SetFlagOffAll( eAPD_FLAG_TEMP_SELECTED );
    CAnchorPointData::SetFlagOffAll( eAPD_FLAG_TEMP_ACTIVE );

    // FillPointData
    CFillPointData::SetFlagOffAll( eFPD_FLAG_TEMP_SELECTED );
    CFillPointData::SetFlagOffAll( eFPD_FLAG_TEMP_ACTIVE );

    // SlotPointData
    CSlotPointData::SetFlagOffAll( eSPD_FLAG_TEMP_SELECTED );
    CSlotPointData::SetFlagOffAll( eSPD_FLAG_TEMP_ACTIVE );
}

//----------------------
// ストロークの描画
//----------------------
int CBezier::Stroke( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                     BYTE testPalGrp, BYTE testPalGrpForRepair, bool isDot, bool isWorkPath ){
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::Stroke: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();

    // バッファ設定
    if( ! SetBufForStroke( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::Stroke: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;
    s_nTestPalGrpForRepair = testPalGrpForRepair;

    //--------------------------
    // 描画：ストローク
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );

    // 実処理
    if( isDot ){
        DrawDot( pHead );
    }else{
        DrawStroke( pHead );
    }
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::Stroke(isDot:%d): %dus, divPoint=%d\n", isDot, time, s_nUseDivisionPoint );
    }
#endif
    
    return( time );
}

//----------------------
// タッチの描画
//----------------------
int CBezier::Touch( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                    BYTE testPalGrp, bool isStripe, bool isWorkPath ){
    // ワークパスには対応しない
    if( isWorkPath ){
        return( 0 );
    }
    
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::Touch: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();

    // バッファ設定
    if( ! SetBufForStroke( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::Touch: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;

    // タッチフラグオン
    s_bTouch = true;
    s_bStripe = isStripe;

    //--------------------------
    // 描画：タッチ
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );
    
    // 実処理
    DrawTouch( pHead );
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::Touch: %dus, divPoint=%d\n", time, s_nUseDivisionPoint );
    }
#endif
    
    return( time );
}

//----------------------
// フリルの描画
//----------------------
int CBezier::TouchForFrill( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                            BYTE testPalGrp, bool isWorkPath ){
    // ワークパスには対応しない
    if( isWorkPath ){
        return( 0 );
    }

    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::TouchForFrill: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForStroke( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::TouchForFrill: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;
    
    // フリルフラグオン
    s_bFrill = true;
    
    //--------------------------
    // 描画：フリル
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );
    
    // 実処理
    DrawFrill( pHead );
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::TouchForFrill: %dus, divPoint=%d\n", time, s_nUseDivisionPoint );
    }
#endif
    
    return( time );
}

//----------------------
// 塗りつぶし
//----------------------
int CBezier::Fill( stBEZIER_FILL_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                   BYTE testPalGrp, bool isWorkPath ){
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::Fill: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForFill( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::Fill: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;
    
    //--------------------------
    // 描画：塗り
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );
    
    // 実処理
    DrawFill( pHead );

    // 色アンチ（※タッチによる境界が指定されていれば）
    DrawFillAntiForTouch( pHead );
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::Fill: %dus\n", time );
    }
#endif
    
    return( time );
}

//----------------------
// スロット（※開発用）
//----------------------
void CBezier::SlotForDev( stBEZIER_SLOT_POINT* pHead, stBEZIER_BASE_PARAM* pParam ){
    // 指定があれば無視（※ここではスロットのマークを描くだけなので）
    if( IsDispOnlyAnchorPoint() ){
        return;
    }

    // 用心
    if( pHead == NULL  ){
        LOGE( "@ CBezier::SlotForDev: INVALID DATA: pHead=%p\n", pHead );
        return;
    }
    
    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForDev( pParam ) ){
        LOGE( "@ CBezier::SlotForDev: FAILED: BUF SETTING\n" );
        return;
    }
    
    //-------------------------------
    // 描画：スロット
    //-------------------------------
    stBEZIER_SLOT_POINT* pSP = pHead;
        
    while( pSP != NULL ){
        PutPlusForWorkPath( pSP->x, pSP->y, BEZIER_WP_SLOT_PAL + pSP->workPathPalOfs );
        
        // 次のポイントへ
        pSP = (stBEZIER_SLOT_POINT*) pSP->pNext;
    }
}

//------------------------------------------------------------------------
// 情報：基準点（※常にスロットの位置に表示されるので意味は薄いが、目印として表示してみる）
//------------------------------------------------------------------------
void CBezier::BasePointForDev( float x, float y, stBEZIER_BASE_PARAM* pParam, bool isActive, bool isSelected ){
    // 指定があれば無視（※ここでは基準点のマークを描くだけなので）
    if( IsDispOnlyAnchorPoint() ){
        return;
    }

    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForDev( pParam ) ){
        LOGE( "@ CBezier::BasePointForDev: FAILED: BUF SETTING\n" );
        return;
    }

    //-------------------------------
    // 描画：基準点
    //-------------------------------
    BYTE palOfs = BEZIER_WP_PAL_VAL_FOR_SLEEP;
    
    if( IsDispPathActiveAlways() ){
        palOfs = BEZIER_WP_PAL_VAL_FOR_SELECTED;    // 色味の関係で[SELECTED]
    }else if( isSelected ){
        palOfs = BEZIER_WP_PAL_VAL_FOR_SELECTED;
    }else if( isActive ){
        palOfs = BEZIER_WP_PAL_VAL_FOR_ACTIVE;
    }

    PutPlusForWorkPathS( x, y, BEZIER_WP_BASE_PAL + palOfs );
}

//-------------------
// ワーククリア
//-------------------
void CBezier::ClearWork( void ){
    ClearWorkSetting();
    ClearWorkPoint();
    ClearWorkStripePoint();
}

//-------------------
// ワーク設定クリア
//-------------------
void CBezier::ClearWorkSetting( void ){
    s_pBaseParam = NULL;
    s_pLayerParam = NULL;

    s_pBuf = NULL;
    s_pBufFillGuide = NULL;
    s_pBufFillGuard = NULL;
    s_pBufTest = NULL;
    
    s_nBufW = s_nBufH = 0;

    s_nTestPalGrp = 0x00;
    s_nTestPalGrpForRepair = 0x00;
    s_bTouch = false;
    s_bStripe = false;
    s_bFrill = false;
    s_bWorkPath = false;
}

//-------------------
// ワークポイントクリア
//-------------------
void CBezier::ClearWorkPoint( void ){
    s_nUseDivisionPoint = 0;
    s_nTouchStrokePointCur = 0;
    s_nTouchStrokePointMax = 0;
    s_nTouchStrokePointLimit = 0;
    s_nNumStripeGuidePoint = 0;
}

//------------------------
// ストライプ登録クリア
//------------------------
void CBezier::ClearWorkStripePoint( void ){
    s_nStripePointNum = 0;
}

//-------------------------
// ワーク設定：ストローク用
//-------------------------
bool CBezier::SetBufForStroke( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath ){
    // 無効確認：データ
    if( pParam == NULL  ){
        LOGE( "@ CBezier::SetBufForStroke: INVALID DATA: pParam=%p, pLayerParam=%p\n", pParam, pLayerParam );
        return( false );
    }
    
    // 無効確認：バッファ
    if( pParam->pBufLine == NULL || pParam->pBufFillGuide == NULL || pParam->pBufColor == NULL ||
        pParam->texW <= 0 || pParam->texH <= 0 ){
        LOGE( "@ CBezier::SetBufForStroke: INVALID PARAM: pBuf=%p, pBufFillGuide=%p, pBufColor=%p, w=%d, h=%d\n",
              pParam->pBufLine, pParam->pBufFillGuide, pParam->pBufColor, pParam->texW, pParam->texH );
        return( false );
    }

    s_pBaseParam = pParam;
    s_pLayerParam = pLayerParam;
    s_bWorkPath = isWorkPath;

    s_pBuf = s_pBaseParam->pBufLine;
    s_pBufFillGuide = s_pBaseParam->pBufFillGuide;
    s_pBufFillGuard = s_pBaseParam->pBufFillGuard;
    s_pBufTest = s_pBaseParam->pBufColor;
    s_nBufW = pParam->texW;
    s_nBufH = pParam->texH;
    
    return( true );
}

//-------------------------
// ワーク設定：塗り用
//-------------------------
bool CBezier::SetBufForFill( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath ){
    // 無効確認：データ
    if( pParam == NULL  ){
        LOGE( "@ CBezier::SetBufForFill: INVALID DATA: pParam=%p, pLayerParam=%p\n", pParam, pLayerParam );
        return( false );
    }
    
    // 無効確認：バッファ
    if( pParam->pBufColor == NULL ||  pParam->pBufFillGuide == NULL || pParam->texW <= 0 || pParam->texH <= 0 ){
        LOGE( "@ CBezier::SetBufForFill: INVALID PARAM: pBufColor=%p, pBufFillGuide=%p, w=%d, h=%d\n",
              pParam->pBufColor, pParam->pBufFillGuide, pParam->texW, pParam->texH );
        return( false );
    }

    s_pBaseParam = pParam;
    s_pLayerParam = pLayerParam;
    s_bWorkPath = isWorkPath;

    s_pBuf = s_pBaseParam->pBufColor;
    s_pBufFillGuide = s_pBaseParam->pBufFillGuide;
    s_pBufFillGuard = s_pBaseParam->pBufFillGuard;
    s_nBufW = s_pBaseParam->texW;
    s_nBufH = s_pBaseParam->texH;
    
    // ワークパスの指定であれば出力先をメインバッファへ
    if( s_bWorkPath ){
        s_pBuf = s_pBaseParam->pBufLine;
    }
    return( true );
}

//-------------------------
// ワーク設定：開発用
//-------------------------
bool CBezier::SetBufForDev( stBEZIER_BASE_PARAM* pParam ){
    // 無効確認：データ
    if( pParam == NULL  ){
        LOGE( "@ CBezier::SetBufForDev: INVALID DATA: pParam=%p\n", pParam );
        return( false );
    }
    
    // 無効確認：バッファ
    if( pParam->pBufLine == NULL || pParam->texW <= 0 || pParam->texH <= 0 ){
        LOGE( "@ CBezier::SetBufForDev: INVALID PARAM: pBuf=%p, w=%d, h=%d\n", pParam->pBufLine, pParam->texW, pParam->texH );
        return( false );
    }

    s_pBaseParam = pParam;
    
    s_pBuf = s_pBaseParam->pBufLine;
    s_nBufW = s_pBaseParam->texW;
    s_nBufH = s_pBaseParam->texH;
    
    return( true );
}

//--------------------------------------------------------------------------------------
// 分割点の登録：指定の２ＡＰ間（※[pP1]は追加されない点に注意＝次回の始点になるので重複描画されないように）
//--------------------------------------------------------------------------------------
void CBezier::RegistDivisionPointList( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 ){
    // 始点の登録
    float strokeSize = pP0->strokeSize;
    if( pP0->strokeStartRange > 0.0f ){
        strokeSize = pP0->strokeStartSize;
    }
    AddDivisionPoint( pP0->x, pP0->y, strokeSize, true, pP0 );

    //--------------------------------------------
    // 分割点の登録（※直線であっても諸々の都合上分割する）
    //--------------------------------------------
    // 分割数の算出
    int divNum = CalcDivisionNumForSegment( pP0, pP1 );

    // 分割点の登録（※ここでは始点[i==0]と終点[i==divNum]は追加されない）
    for( int i=1; i<divNum; i++ ){
        // t, (1-t)
        float t1 = ((float)i)/divNum;
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

        // 分割点座標の算出（※ベジェ曲線）
        float x = pP0->x*tR3 + 3*(pP0->x+pP0->xOut)*tR2_t1 + 3*(pP1->x+pP1->xIn)*tR1_t2 + pP1->x*t3;
        float y = pP0->y*tR3 + 3*(pP0->y+pP0->yOut)*tR2_t1 + 3*(pP1->y+pP1->yIn)*tR1_t2 + pP1->y*t3;
        
        // ストロークサイズ
        strokeSize = pP0->strokeSize;
        if( t1 < pP0->strokeStartRange ){
            // 開始サイズから徐々に基本サイズへ
            float range = t1 / pP0->strokeStartRange;
            strokeSize = (1.0f-range)*pP0->strokeStartSize + range*pP0->strokeSize;
        }else if( tR1 < pP0->strokeEndRange ){
            // 基本サイズから徐々に終了サイズへ
            float range = tR1 / pP0->strokeEndRange;
            strokeSize = (1.0f-range)*pP0->strokeEndSize + range*pP0->strokeSize;
        }
        
        // 分割点リストに登録
        AddDivisionPoint( x, y, strokeSize, false, pP0 );
    }
    
    // 終点は登録をせずに終了（※次のアンカーポイントの始点と重複するため）
}

//------------------------------------------------------------
// 分割数取得（※曲線を何個の直線に分割するかの判定）
// 各点間のX、Y距離の大きい要素の合計から分割数を決定
// メモ：浮動小数の割り算の分母になる値なので返値が偶数になるようにしている
//------------------------------------------------------------
#define ADD_MAX_DIST_TO_SUM( _dX, _dY ) \
    distX = (_dX);                      \
    distY = (_dY);                      \
    if( distX < 0 ){ distX = -distX; }  \
    if( distY < 0 ){ distY = -distY; }  \
    sum += (distX>distY)? distX: distY
int CBezier::CalcDivisionNumForSegment( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 ){
    int sum = 0;
    int distX, distY;
    
    // ポイント間（直線距離）
    ADD_MAX_DIST_TO_SUM( pP1->x-pP0->x, pP1->y-pP0->y );
    
    // 方向線：出発
    ADD_MAX_DIST_TO_SUM( pP0->xOut, pP0->yOut );
    
    // 方向線：進入
    ADD_MAX_DIST_TO_SUM( pP1->xIn, pP1->yIn );

    // 分割数判定（※描画品質と処理負荷の兼ね合いで調整）
    if( sum < 16 ){
        return( (sum+1)/2 );
    }
    if( sum < 256 ){
        return( 16 );
    }
    if( sum < 1024 ){
        return( 32 );
    }
    
    return( 64 );
}

//----------------------
// 分割点を追加
//----------------------
void CBezier::AddDivisionPoint( float x, float y, float strokeSize, bool isEdgeFillCheck, stBEZIER_ANCHOR_POINT* pAP ){
    // 用心
    if( s_nUseDivisionPoint >= BEZIER_DIVISION_POINT_MAX ){
        LOGE( "@ CBezier::AddDivisionPoint: BUF SHORTAGE: use=%d/%d\n", s_nUseDivisionPoint, BEZIER_DIVISION_POINT_MAX);
        return;
    }
    
    // 登録
    stBEZIER_DIVISION_POINT *pDP = &s_stArrDivisionPoint[s_nUseDivisionPoint];
    pDP->x0 = x;
    pDP->y0 = y;
    pDP->strokeSize = strokeSize;
    pDP->isEdgeFillCheck = (isEdgeFillCheck && pAP->strokeEdgeFillSize>0.0f);
    pDP->pAP = pAP;
    
    pDP->x = x;
    pDP->y = y;

    // 要素数更新＆利用数最大数確認
    s_nUseDivisionPoint++;
    if( s_nUseDivisionPoint > s_nUseDivisionPointMax ){
        s_nUseDivisionPointMax = s_nUseDivisionPoint;
    }
}

//----------------------------------------------------------------------------
// ２点間を線形補間で描画（※ループ内部での判定を避けるために座標計算後に状況毎の描画をしている）
//---------------------------------------------------------------------------
static int __stepForDotLine = 0;
void CBezier::PutDotLinear( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP ){
    // 分割点間の座標を算出して一時バッファに格納（※返値は登録された座標数）
    int point = CalcThenSetPointForTempBuf( pDP0, pDP1, pAP );
    if( point <=0 ){
        return;
    }
    
    // ストロークの取り出し
    CStroke* pStroke = NULL;
    if( pAP->type != eSTROKE_TYPE_OFF ){
        pStroke = CStrokeMgr::GetStroke( pAP->type, pAP->size, pAP->ofsRange );
    }

    //--------------------------------------
    // ワークパス指定があればポイントの出力
    //--------------------------------------
    if( s_bWorkPath ){
        // ストロークが無効であれば破線
        if( pStroke == NULL ){
            int step = 8;
            for( int i=0; i<point; i++ ){
                __stepForDotLine++;
                if( __stepForDotLine >= step ){
                    PutLineDotForWorkPath( (int)s_fArrTempPointX[i], (int)s_fArrTempPointY[i], BEZIER_WP_LINE_PAL + pAP->workPathPalOfsS );
                    __stepForDotLine = 0;
                }
            }
        }
        // ストロークが有効であれば実線
        else {
            for( int i=0; i<point; i++ ){
                PutLineDotForWorkPath( (int)s_fArrTempPointX[i], (int)s_fArrTempPointY[i], BEZIER_WP_LINE_PAL + pAP->workPathPalOfsS );
            }
        }
        
        // ワークパスはこれで終了
        return;
    }
    
    //--------------------------------------
    // ここまできたら通常出力
    //--------------------------------------
    // ストロークが有効なら
    if( pStroke != NULL ){
        //--------------------------
        // タッチ指定があれば
        //--------------------------
        if( s_bTouch ){
            // ストライプ時は普通に表示（※タッチのように線の強弱はつけない）
            if( s_bStripe ){
                for( int i=0; i<point; i++ ){
                    if( s_fArrTempStrokeSize[i] > 0.0f ){
                        PutDotAt( s_fArrTempPointX[i], s_fArrTempPointY[i], s_fArrTempStrokeSize[i], s_nTestPalGrp, pStroke, pAP );
                    }
                }
            }
            // タッチ時は始まりから終わりにかけてストロークサイズが細くなるように（※長さにばらつきがある線であっても、終端が消える感じになるように）
            else{
                for( int i=0; i<point; i++ ){
                    if( s_fArrTempStrokeSize[i] > 0.0f ){
                        // [150%〜50%]で調整
                        float strokeSize = s_fArrTempStrokeSize[i] * (1.5f - ((float)s_nTouchStrokePointCur)/s_nTouchStrokePointLimit);
                        PutDotAt( s_fArrTempPointX[i], s_fArrTempPointY[i], strokeSize, s_nTestPalGrp, pStroke, pAP );
                    }
                }
            }
        }
        //--------------------------
        // 通常ストローク
        //--------------------------
        else{
            for( int i=0; i<point; i++ ){
                if( s_fArrTempStrokeSize[i] > 0.0f ){
                    PutDotAt( s_fArrTempPointX[i], s_fArrTempPointY[i], s_fArrTempStrokeSize[i], s_nTestPalGrp, pStroke, pAP );
                }
            }
            
            // ライン修復指定があれば線の引き直し
            //（※BDP間で線を描く際、先行するBDPの線のエッジに色が塗られてしまうことで線の継ぎ目が欠けたようになるため、指定の塗り色を対象に線を引き直す）
            if( pAP->bLineRepairTest && s_nTestPalGrpForRepair != s_nTestPalGrp ){
                for( int i=0; i<point; i++ ){
                    if( s_fArrTempStrokeSize[i] > 0.0f ){
                        PutDotAt( s_fArrTempPointX[i], s_fArrTempPointY[i], s_fArrTempStrokeSize[i], s_nTestPalGrpForRepair, pStroke, pAP );
                    }
                }
            }
        }
    }

    // 非タッチストロークにタッチ対象指定があればポイントの登録
    if( !s_bTouch && pAP->touchTarget != eSTROKE_TOUCH_TARGET_INVALID ){
        int target = pAP->touchTarget;
        
        // 用心（※枠が不足していたら困る）
        if( (s_nArrTouchPointNum[target]+point) > BEZIER_TOUCH_POINT_MAX ){
            LOGE( "@ CBezier::PutDotLinear: TOUCH BUF SHORTAGE: touchNum[%d]=(%d+%d) > %d\n",
                  target, s_nArrTouchPointNum[target], point, BEZIER_TOUCH_POINT_MAX );
            return;
        }

        // ポイント（※傾き適用前後の両方を保持)
        memcpy( &s_fArrArrTouchPointX[target][s_nArrTouchPointNum[target]], s_fArrTempPointX, point*sizeof(float) );
        memcpy( &s_fArrArrTouchPointY[target][s_nArrTouchPointNum[target]], s_fArrTempPointY, point*sizeof(float) );
        memcpy( &s_fArrArrTouchPointX0[target][s_nArrTouchPointNum[target]], s_fArrTempPointX0, point*sizeof(float) );
        memcpy( &s_fArrArrTouchPointY0[target][s_nArrTouchPointNum[target]], s_fArrTempPointY0, point*sizeof(float) );
        s_nArrTouchPointNum[target] += point;
    }
    
    // タッチでなければガイド出力（※ストライプ時も出力縞模様を塗りつぶせるように）
    if( (!s_bTouch || s_bStripe) && ! pAP->bNoFillGuide ){
        // 下地色を１ピクセルで出力
        for( int i=0; i<point; i++ ){
            int at = s_nBufW*((int)s_fArrTempPointY[i]) + ((int)s_fArrTempPointX[i]);
            s_pBufFillGuide[at] = BEZIER_PAL_UNDERCOAT_COL;
        }

        // ストライプ時はガイド値を保持
        if( s_bStripe ){
            // 用心（※ガイド登録枠が足りないのは困る）
            if( (s_nNumStripeGuidePoint+point) > BEZIER_STRIPE_GUIDE_POINT_MAX ){
                LOGE( "@ CBezier::PutDotLinear: GUIDE BUF SHORTAGE: stripeGuidePoint=(%d+%d) > %d\n",
                       s_nNumStripeGuidePoint, point, BEZIER_STRIPE_GUIDE_POINT_MAX );
                return;
            }

            // 塗りつぶし座標の検出用にガイドへ出力した値を保持しておく（※ストライプは頻繁に呼ばれる処理ではないので上のループとは別に処理しておく）
            for( int i=0; i<point; i++ ){
                s_nArrStripeGuidePointX[s_nNumStripeGuidePoint] = (int)s_fArrTempPointX[i];
                s_nArrStripeGuidePointY[s_nNumStripeGuidePoint] = (int)s_fArrTempPointY[i];
                s_nNumStripeGuidePoint++;
            }
        }
    }
}

//---------------------------------------
// ２点間のポイント等を算出して一時バッファに格納
//---------------------------------------
int CBezier::CalcThenSetPointForTempBuf( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP ){
    // ポイント数の算出
    int point = CalcPointNum( pDP0->x, pDP0->y, pDP1->x, pDP1->y );
    
    // 用心（※座標登録枠が足りない場合は困る）
    if( point > BEZIER_TEMP_POINT_MAX ){
        LOGE( "@ CBezier::CalcThenSetPointForTempBuf: TEMP BUF SHORTAGE: point=%d > %d\n", point, BEZIER_TEMP_POINT_MAX );
        return( -1 );
    }
    
    //-----------------------------------------------------------------------------------------------------------
    // 出力に必要な座標とストロークサイズの算出
    //（※終了判定が[i<div]なので[t==1.0f]の値である[x1,y1]は描画されないが、これは次のポイントの始点[x0,y0]と重複しないための措置）
    //-----------------------------------------------------------------------------------------------------------
    for( int i=0; i<point; i++ ){
        float t = ((float)i)/point;
        float tR = 1.0f - t;
        
        s_fArrTempPointX[i] = pDP0->x*tR + pDP1->x*t;
        s_fArrTempPointY[i] = pDP0->y*tR + pDP1->y*t;
        s_fArrTempPointX0[i] = pDP0->x0*tR + pDP1->x0*t;
        s_fArrTempPointY0[i] = pDP0->y0*tR + pDP1->y0*t;
        s_fArrTempStrokeSize[i] = pDP0->strokeSize*tR + pDP1->strokeSize*t;
    }
    
    // 透明ストローク例外（※次のＡＰが透明であれば終点も登録しておく＝これがないと１ドット欠けた印象になってしまう）
    if( pAP->pNext != NULL ){
        stBEZIER_ANCHOR_POINT* pNextAP = (stBEZIER_ANCHOR_POINT*)pAP->pNext;
        if( pNextAP->type == eSTROKE_TYPE_OFF ){
            s_fArrTempPointX[point] = pDP1->x;
            s_fArrTempPointY[point] = pDP1->y;
            s_fArrTempPointX0[point] = pDP1->x0;
            s_fArrTempPointY0[point] = pDP1->y0;
            s_fArrTempStrokeSize[point] = pDP1->strokeSize;
            point++;
        }
    }

    return( point );
}

//-----------------------------------
// ２点間のポイント数の算出
//-----------------------------------
int CBezier::CalcPointNum( float x0, float y0, float x1, float y1 ){
    // 二点間のドット数の算出（※整数で判定）
    int distX = ((int)x1) - ((int)x0);
    int distY = ((int)y1) - ((int)y0);
    if( distX < 0 ){ distX = -distX; }
    if( distY < 0 ){ distY = -distY; }
    int num = ((distX>distY)? distX: distY);
    
    // ０以下は１点と考える（※同じ座標が指定された＝オープンパスの最後の点等を想定）
    if( num <= 0 ){
        num = 1;
    }

    return( num );
}

//-------------------
// ドットを置く
//-------------------
void CBezier::PutDotAt( float x0, float y0, float strokeSize, BYTE testPalGrp, CStroke* pStroke, stBEZIER_ANCHOR_POINT* pAP ){
    // タッチストローク時に制限値に達していたら無視
    if( s_bTouch && ! s_bStripe ){
        if( s_nTouchStrokePointCur >= s_nTouchStrokePointLimit ){
            return;
        }
        s_nTouchStrokePointCur++;
    }

#if 0
    // [pStroke]の[NULL]チェックは呼び出し側で行なっている「はず」なので保護コードはオフ
    if( pStroke == NULL ){
        LOGE( "@ CBezier::PutDotAt: INVALID STROKE\n" );
        return;
    }
#endif

    // ブラシのブレを反映
    UpdateBrushShake();
    x0 += pAP->shakeScaleForPos*GetBrushShakeForPosX();
    y0 += pAP->shakeScaleForPos*GetBrushShakeForPosY();
    strokeSize *= 1.0f + pAP->shakeScaleForSize*GetBrushShakeForSize();

    // ドットサイズ調整（※ストロークによるサイズ補正を反映）
    float size0 = pStroke->getSize();
    size0 *= strokeSize;

    // 座標調整（※ドットの中央に描画されるように）
    int size = (int)size0;
    float x = x0 - size/2.0f;
    float y = y0 - size/2.0f;

    //-----------------------------
    // 座標のはみ出し具合の算出
    //-----------------------------
    int iX0 = (int)x;
    int iY0 = (int)y;
    float rateOutX = x - iX0;   // 小数点以下がＸ方向のはみ出し
    float rateOutY = y - iY0;   // 小数点以下がＹ方向のはみ出し
    
    //--------------------------------------------------------------------------------------------------
    // ストローク画像取得＆描画（※はみ出し具合によるテクスチャが[pCell]に設定されて返る）
    // [pDot]で指定される画素の値域は[0〜STROKE_DOT_OUT_PRECISION_MAX]で値が大きいほど濃い（※MAXで不透明／０で透明）
    //--------------------------------------------------------------------------------------------------
    stSTROKE_OUTPUT_CELL* pCell = pStroke->getStrokeCell( rateOutX, rateOutY, strokeSize );
    BYTE* pDot = pCell->pBuf;
    int w = pCell->sizeW;
    int h = pCell->sizeH;
    int iX = iX0 + pCell->ofsX; // １ピクセルを超えるオフセットの適用
    int iY = iY0 + pCell->ofsY; // １ピクセルを超えるオフセットの適用

    // 画素の出力
    for( int i=0; i<h; i++ ){
        for( int j=0; j<w; j++ ){
            int dotAt = w*i + j;
            int bufAt = s_nBufW*(iY+i) + (iX+j);
            
            //------------------------------------------------------------------------------------------------------
            // ストローク値が有効であれば ＆ 出力に意味があれば（※基本的に不透明になった時点で画素は確定）
            //（※[isDotPutWhetherMax]の指定によりすでに出力された濃い色を薄めることができる＝黒地に黒を引いた時の境界を浮き出させる等）
            //------------------------------------------------------------------------------------------------------
            if( pDot[dotAt] > 0x00 && (pAP->isDotPutWhetherMax || s_pBuf[bufAt] < pAP->dotPutMax) ){
                bool checkResult;
                if( pAP->isDotPutOnOut ){
                    // 直近に出力した領域へのみ描き込み可能（※ガードされていたら不可）
                    checkResult = ((s_pBufFillGuide[bufAt]&BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG)!=0x00) && (s_pBufFillGuard[bufAt]==0);
                }else{
                    // 未出力領域へのみ描き込み可能（※ガードされていたら不可）
                    checkResult = ((s_pBufFillGuide[bufAt]&BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG)==0x00) && (s_pBufFillGuard[bufAt]==0);
                }

                //------------------
                // テストに通過したら
                //------------------
                if( (checkResult&&BEZIER_CONV_DOT_TO_PAL_GRP(s_pBufTest[bufAt])==testPalGrp) || pAP->isDotPutForce ){
                    //---------------------------------------------
                    // 出力値の算出：[dotPutRateBase]で出力の強さの調整
                    //---------------------------------------------
                    int val = (0xFF*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;
                    val = (val*pAP->dotPutRateBase)/100;

                    //--------------------------------------------------------------------------------------
                    // 色味の減算（※ここは単純な引き算）
                    //--------------------------------------------------------------------------------------
                    if( pAP->isDotErase ){
                        // 消去時は単純に引き算
                        if( val >= s_pBuf[bufAt] ){
                            s_pBuf[bufAt] = 0;
                        }else{
                            s_pBuf[bufAt] -= val;
                        }
                    }
                    //--------------------------------------------------------------------------------------
                    // 色味の加算（※濃い方に薄い方を足すイメージ）：[dotPutRateStrong/dotPutRateWeak]で重ね合わせの調整
                    //--------------------------------------------------------------------------------------
                    else{
                        int temp;
                        if( val > s_pBuf[bufAt] ){
                            temp = (val*pAP->dotPutRateStrong + s_pBuf[bufAt]*pAP->dotPutRateWeak)/100;
                        }else{
                            temp = (s_pBuf[bufAt]*pAP->dotPutRateStrong + val*pAP->dotPutRateWeak)/100;
                        }

                        // ストロークバッファへ出力
                        if( temp >= pAP->dotPutMax ){
                            s_pBuf[bufAt] = (BYTE)pAP->dotPutMax;
                        }else{
                            s_pBuf[bufAt] = (BYTE)temp;
                        }
                    }
                    
                    // ストロークバッファの利用状況更新
                    UpdateStrokeBufInfo( iX+j, iY+i );
                }
            }
        }
    }
}

//--------------------------------------
// フックの登録／設定
//--------------------------------------
void CBezier::resolveHookPoint( stBEZIER_ANCHOR_POINT* pBAP ){
    //--------------------------------------
    // システムフックが有効であれば
    //--------------------------------------
    bool isSystemHookValid = false;
    if( pBAP->systemHookTarget >= 0 && pBAP->systemHookTarget < BEZIER_HOOK_POINT_MAX ){
        // 登録座標があれば参照
        if( s_bArrHookPoint[pBAP->systemHookTarget] ){
            pBAP->x = s_fArrHookPointX[pBAP->systemHookTarget];
            pBAP->y = s_fArrHookPointY[pBAP->systemHookTarget];
        }
        // 登録座標がなければ登録
        else{
            s_fArrHookPointX[pBAP->systemHookTarget] = pBAP->x;
            s_fArrHookPointY[pBAP->systemHookTarget] = pBAP->y;
            s_bArrHookPoint[pBAP->systemHookTarget] = true;
        }

        // システムフックが有効
        isSystemHookValid = true;
    }
    
    //--------------------------------------
    // フック指定が有効であれば
    //--------------------------------------
    if( pBAP->hookTarget >= 0 && (int)pBAP->hookTarget < BEZIER_HOOK_POINT_MAX ){
        // 登録座標があれば
        if( s_bArrHookPoint[pBAP->hookTarget] ){
            // システムフックが無効であれば参照（※システムフックが有効であればそちらを優先）
            if( ! isSystemHookValid ){
                pBAP->x = s_fArrHookPointX[pBAP->hookTarget];
                pBAP->y = s_fArrHookPointY[pBAP->hookTarget];
            }
        }
        // 登録座標がなければ登録
        else{
            s_fArrHookPointX[pBAP->hookTarget] = pBAP->x;
            s_fArrHookPointY[pBAP->hookTarget] = pBAP->y;
            s_bArrHookPoint[pBAP->hookTarget] = true;
        }
    }
}

//---------------------------
// ストライプポイントの追加
//---------------------------
void CBezier::AddStripePoint( BYTE palOfsGrp, float x, float y ){
    // 用心
    if( s_nStripePointNum >= BEZIER_STRIPE_POINT_MAX ){
        LOGE( "@ CBezier::AddStripePoint: BUF SHORTAGE: num=%d >= %d\n", s_nStripePointNum, BEZIER_STRIPE_POINT_MAX );
        return;
    }
    
    s_nArrStripePalGrp[s_nStripePointNum] = palOfsGrp;
    s_fArrStripePointX[s_nStripePointNum] = x;
    s_fArrStripePointY[s_nStripePointNum] = y;
    s_nStripePointNum++;
}
