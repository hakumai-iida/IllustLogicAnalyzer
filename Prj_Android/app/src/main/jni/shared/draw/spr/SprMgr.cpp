/*+----------------------------------------------------------------+
  |	Title:		SprMgr.cpp [共通環境]
  |	Comment:	スプライト管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "SprMgr.hpp"

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
CSpr* CSprMgr::s_oArrSpr;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CSprMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CSprMgr::OnCreate()\n" );
#endif

	if( !CreateAll() ){
		LOGE( "@ CSprMgr::CreateAll(): FAILED!!\n" );
		env_push_error_log( "[CSprMgr::CreateAll()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CSprMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CSprMgr::OnDestroy()\n" );
#endif

	// 解放
	SAFE_DELETE_ARRAY( s_oArrSpr );
}

//------------------
// スプライト読み込み
//------------------
bool CSprMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CSprMgr::CreateAll()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	s_oArrSpr = new CSpr[eSPR_MAX];
	CMemMgr::PopTargetField();

    // 各データの読み込み
    CDefBinTable<CSprBinData>* pTable = CDefTable::GetSprTable();
	for( uint32 i=0; i<eSPR_MAX; i++ ){
        CSprBinData* pBin = pTable->getDataAt( i );
		if( ! s_oArrSpr[i].applyBin( pBin ) ){
			char* buf = CMemMgr::GetBufFromTempStr();
			sprintf( buf, "スプライト[%s]の設定に失敗しました。\n", g_pArrLabelSpr[i] );
			env_push_error_log( buf );
			return( false );	// FATAL ERROR
		}
	}

	return( true );
}

//------------------
// 取得
//------------------
CSpr* CSprMgr::GetSpr( eSPR sprId ){
	if( ! IS_SPR_VALID( sprId ) ){
		return( NULL );
	}

	return( &s_oArrSpr[sprId] );
}
