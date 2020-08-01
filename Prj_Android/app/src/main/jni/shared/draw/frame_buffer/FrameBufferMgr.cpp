/*+----------------------------------------------------------------+
  |	Title:		CFrameBufferMgr.cpp [共通環境]
  |	Comment:	フレームバッファ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FrameBufferMgr.hpp"

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
CFrameBuffer* CFrameBufferMgr::s_oArrFrameBuffer;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CFrameBufferMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFrameBufferMgr::OnCreate()\n" );
#endif

	if( !CreateAll() ){
		LOGE( "@ CFrameBufferMgr::CreateAll(): FAILED!!\n" );
		env_push_error_log( "[CFrameBufferMgr::CreateAll()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CFrameBufferMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFrameBufferMgr::OnDestroy()\n" );
#endif

	// 解放
	SAFE_DELETE_ARRAY( s_oArrFrameBuffer );
}

//-------------------------
// フレームバッファデータ作成
//-------------------------
bool CFrameBufferMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFrameBufferMgr::CreateAll()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	s_oArrFrameBuffer = new CFrameBuffer[eFRAME_BUFFER_MAX];
	CMemMgr::PopTargetField();

    // 各データの設定
    CDefBinTable<CFrameBufferBinData>* pTable = CDefTable::GetFrameBufferTable();
	for( int i=0; i<eFRAME_BUFFER_MAX; i++){
        CFrameBufferBinData* pBin = pTable->getDataAt( i );
		if( ! s_oArrFrameBuffer[i].applyBin( pBin ) ){
            char* buf = CMemMgr::GetBufFromTempStr();
			sprintf( buf, "フレームバッファ[%s]の設定に失敗しました。\n", g_pArrLabelFrameBuffer[i] );
			env_push_error_log( buf );
			return( false );	// FATAL ERROR
		}
	}

	return( true );
}

//-----------------------
// 取得：フレームバッファ
//-----------------------
CFrameBuffer* CFrameBufferMgr::GetFrameBuffer( eFRAME_BUFFER fbId ){
	if( ! IS_FRAME_BUFFER_VALID( fbId ) ){
		return( NULL );
	}

	if( ! s_oArrFrameBuffer[fbId].isValid() ){
		return( NULL );
	}

	return( &s_oArrFrameBuffer[fbId] );
}
