/*+----------------------------------------------------------------+
  |	Title:		config_for_app.cpp [共通環境]
  | Comment:    アプリ内容に依存したコンフィグ
  |             [app]以下の内容に依存する項目をまとめてみる（気分の問題）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "app/loop/LogicTestLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
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
//-------------------------------------
// [for CMemMgr] アプリ依存メモリ確保
//-------------------------------------
bool CMemMgr::AllocForApp( void ){
    //---------------------------
    // 管理メモリ領域確保
    //---------------------------
    uint32 size = 64*MB;

    //---------------------------
    // 解像度による変動値の調整
    //---------------------------
    uint32 kbSizeForStrokeMgr = CStrokeMgr::GetAllocSizeK();
    LOGD( "@ ALLOC SIZE for CStrokeMgr: %dKB\n", kbSizeForStrokeMgr );
    size += kbSizeForStrokeMgr*KB;
    
    uint32 kbSizeForFillMgr = CFillMgr::GetAllocSizeK();
    LOGD( "@ ALLOC SIZE for CFillMgr: %dKB\n", kbSizeForFillMgr );
    size += kbSizeForFillMgr*KB;

    uint32 kbSizeForBezier = CBezier::GetAllocSizeK();
    LOGD( "@ ALLOC SIZE for CBezier: %dKB\n", kbSizeForBezier );
    size += kbSizeForBezier*KB;

    //---------------------------
    // 実メモリ確保
    //---------------------------
    void* pMem = util_sys_mem_alloc( size );
    if( pMem == NULL ){
        env_push_error_log( "メモリの確保に失敗しました。\n" );
        return( false );    // FATAL ERROR
    }
    
    // プロパティ設定
    s_pAddr = pMem;
    s_nSize  = size;
    s_nUsed  = 0;
    
    //---------------------------------------------------
    // 共有フィールド内の領域割り振り
    //---------------------------------------------------
    if( !set_field_share( eMEM_FIELD_SHARE_FPACK,                0*MB,  14*MB ) ){ return( false ); }    // 起動時のファイルパック
    if( !set_field_share( eMEM_FIELD_SHARE_DATA,                 0*MB,   4*MB ) ){ return( false ); }    // データ：１ファイルの最大サイズ
    if( !set_field_share( eMEM_FIELD_SHARE_SDARC_IN,             4*MB,   4*MB ) ){ return( false ); }    // データ：sdarc圧縮データの単位最大サイズ
    if( !set_field_share( eMEM_FIELD_SHARE_SDARC_OUT,            8*MB,   4*MB ) ){ return( false ); }    // データ：sdarc解凍データの単位最大サイズ
    if( !set_field_share( eMEM_FIELD_SHARE_SDARC_WORK,          12*MB,   2*MB ) ){ return( false ); }    // データ：sdarc作業バッファサイズ
    if( !set_field_share( eMEM_FIELD_SHARE_BMP_GEN_OR_SS,       14*MB,  32*MB ) ){ return( false ); }    // BMP生成バッファ／スクショバッファ
    if( !set_field_share( eMEM_FIELD_SHARE_FILL_CELL_OR_SS,     46*MB,   3*MB ) ){ return( false ); }    // 塗りつぶしセル／スクショバッファ
    if( !set_field_share( eMEM_FIELD_SHARE_SS_OUT,              14*MB,  14*MB ) ){ return( false ); }    // スクショバッファ
    if( !set_field_share( eMEM_FIELD_SHARE_SS_WORK,             28*MB,  22*MB ) ){ return( false ); }
    if( !set_field_share( eMEM_FIELD_SHARE_TEMP_STR,            49*MB,  MEM_TEMP_STR_SIZE ) ){ return( false ); }
    
    //---------------------------------------------------
    // フィールド必要サイズ算出
    //---------------------------------------------------
    uint32 sizeShare = calc_share_size();

    uint32 sizeStroke = (256 + kbSizeForStrokeMgr) * KB;
    uint32 numStroke = 8 + CStrokeMgr::GetAllocCellNum();

    uint32 sizeFill = (256 + kbSizeForFillMgr) * KB;
    uint32 numFill = 8 + CFillMgr::GetAllocCellNum();

    uint32 sizeBezier = (256 + kbSizeForBezier ) * KB;
    uint32 numBezier = 8 + CBezier::GetAllocCellNum();

    //---------------------------------------------------
    // 各フィールドの設定
    //---------------------------------------------------
    if( !set_field( eMEM_FIELD_S_SHARE,   sizeShare,   true,          0,  "Share",  false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_BASE,       512*KB,   false,      2048,  "Base",   false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_CSV,        256*KB,   false,       128,  "Csv",    false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_DRAW,         1*MB,   false,       512,  "Draw",   false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_STROKE, sizeStroke,   false, numStroke,  "Stroke", false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_FILL,     sizeFill,   false,   numFill,  "Fill",   false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_BEZIER, sizeBezier,   false, numBezier,  "Bezier", false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_TEX,          1*MB,   false,       128,  "Tex",    false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_OBJ,         64*KB,   false,        16,  "Obj",    false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_LOGIC,        8*MB,   false,       128,  "Logic",  false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_BG,          64*KB,   false,        16,  "BG",     false ) ){ return( false ); }
    if( !set_field( eMEM_FIELD_D_APP,          1*MB,   false,      1024,  "App",    false ) ){ return( false ); }
    
    // 残った領域がフリー
    if( !set_field( eMEM_FIELD_D_FREE, s_nSize-s_nUsed, false, 1024, "Free", false ) ){ return( false ); }
    
    // ここまできたら成功
    return( true );
}

//---------------------------------
// [for CLoopMgr] アプリ依存ループ作成
//---------------------------------
CLoop* CLoopMgr::CreateLoopForApp( int loopId ){
    CLoop* pLoop = NULL;
    
    switch( loopId ){
    case eLOOP_ID_LOGIC_TEST:           pLoop = new CLogicTestLoop();       break;
    }
    
    return( pLoop );
}
