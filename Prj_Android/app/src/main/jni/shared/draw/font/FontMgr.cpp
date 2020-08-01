/*+----------------------------------------------------------------+
  |	Title:		FontMgr.cpp [共通環境]
  |	Comment:	フォント管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FontMgr.hpp"

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
CFont* CFontMgr::s_oArrFont;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CFontMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFontMgr::OnCreate()\n" );
#endif

	if( !CreateAll() ){
		LOGE( "@ CFontMgr::CreateAll(): FAILED!!\n" );
		env_push_error_log( "[CFontMgr::CreateAll()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CFontMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFontMgr::OnDestroy()\n" );
#endif

	SAFE_DELETE_ARRAY( s_oArrFont );
}

//------------------
// 全データ読み込み
//------------------
bool CFontMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFontMgr::CreateAll()\n" );
#endif

    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
    s_oArrFont = new CFont[eFONT_MAX];
    CMemMgr::PopTargetField();

    // 各データの設定
    CDefBinTable<CFontBinData>* pTable = CDefTable::GetFontTable();
    for( uint32 i=0; i<eFONT_MAX; i++ ){
        CFontBinData* pBin = pTable->getDataAt( i );
        if( ! s_oArrFont[i].applyBin( pBin ) ){
            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "フォント[%s]の設定に失敗しました。\n", g_pArrLabelFont[i] );
            env_push_error_log( buf );
            return( false );	// FATAL ERROR
        }
    }

	return( true );
}

//---------------------
// フォント取得
//---------------------
CFont* CFontMgr::GetFont( eFONT fontId ){
	if( ! IS_FONT_VALID( fontId ) ){
		return( NULL );
	}
	return( &s_oArrFont[fontId] );
}
