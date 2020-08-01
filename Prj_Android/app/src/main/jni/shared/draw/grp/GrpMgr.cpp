/*+----------------------------------------------------------------+
  |	Title:		GrpMgr.cpp [共通管理]
  |	Comment:	スプライトグループ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "GrpMgr.hpp"

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
CGrp* CGrpMgr::s_oArrGrp;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CGrpMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CGrpMgr::OnCreate()\n" );
#endif

	if( !CreateAll() ){
		LOGE( "@ CGrpMgr::CreateAll(): FAILED!!\n" );
		env_push_error_log( "[CGrpMgr::CreateAll()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CGrpMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CGrpMgr::OnDestroy()\n" );
#endif

	SAFE_DELETE_ARRAY( s_oArrGrp );
}

//------------------
// 全データ読み込み
//------------------
bool CGrpMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CGrpMgr::CreateAll()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	s_oArrGrp = new CGrp[eGRP_MAX];
	CMemMgr::PopTargetField();
    
	// 各データの設定
    CDefBinTable<CGrpBinData>* pTable = CDefTable::GetGrpTable();
	for( int i=0; i<eGRP_MAX; i++ ){
        CGrpBinData* pBin = pTable->getDataAt( i );
        if( ! s_oArrGrp[i].applyBin( pBin ) ){
            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "グループ[%s]の設定に失敗しました。\n", g_pArrLabelGrp[i] );
            env_push_error_log( buf );
            return( false );    // FATAL ERROR
        }
	}

	return( true );
}

//------------------
// 取得
//------------------
CGrp* CGrpMgr::GetGrp( eGRP grpId ){
   	if( ! IS_GRP_VALID( grpId ) ){
   		return( NULL );
   	}
   	return( &s_oArrGrp[grpId] );
}

