/*+----------------------------------------------------------------+
  |	Title:		NumMgr.cpp [共通環境]
  |	Comment:	数字管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "NumMgr.hpp"

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
CNum* CNumMgr::s_oArrNum;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CNumMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CNumMgr::OnCreate()\n" );
#endif

	if( !CreateAll() ){
		LOGE( "@ CNumMgr::CreateAll(): FAILED!!\n" );
		env_push_error_log( "[CNumMgr::CreateAll()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CNumMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CNumMgr::OnDestroy()\n" );
#endif

    // 解放
    SAFE_DELETE_ARRAY( s_oArrNum );
}

//------------------
// 全データ読み込み
//------------------
bool CNumMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CNumMgr::CreateAll()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	s_oArrNum = new CNum[eNUM_MAX];
	CMemMgr::PopTargetField();

    // 各データの読み込み
    CDefBinTable<CNumBinData>* pTable = CDefTable::GetNumTable();
	for( uint32 i=0; i<eNUM_MAX; i++ ){
        CNumBinData* pBin = pTable->getDataAt( i );
		if( ! s_oArrNum[i].applyBin( pBin ) ){
            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "ナンバー[%s]の設定に失敗しました。\n", g_pArrLabelNum[i] );
            env_push_error_log( buf );
			return( false );	// FATAL ERROR
		}
	}

	return( true );
}

//-----------------
// 数字取得
//-----------------
CNum* CNumMgr::GetNum( eNUM numId ){
	if( ! IS_NUM_VALID( numId ) ){
		return( NULL );
	}
	return( &s_oArrNum[numId] );
}

