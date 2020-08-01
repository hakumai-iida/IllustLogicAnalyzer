/*+----------------------------------------------------------------+
  |	Title:		ShaderMgr.cpp [共通環境]
  |	Comment:	シェーダー管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "./ShaderMgr.hpp"

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
CShader* CShaderMgr::s_oArrShader;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CShaderMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CShaderMgr::OnCreate()\n" );
#endif
    
    if( ! CShader::Init() ){
        LOGE( "@ CShader::Init(): FAILED!!\n" );
        env_push_error_log( "[CShader::Init()]に失敗しました。\n" );
        return( false );    // FATAL ERROR
    }

	if( ! CreateAll() ){
		LOGE( "@ CShaderMgr::CreateAll(): FAILED!!\n" );
		env_push_error_log( "[CShaderMgr::CreateAll()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CShaderMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CShaderMgr::OnDestroy()\n" );
#endif
    
	// 解放
	SAFE_DELETE_ARRAY( s_oArrShader );
    
    CShader::Exit();
}

//------------------
// 取得
//------------------
CShader* CShaderMgr::GetShader( eSHADER shaderId ){
	if( ! IS_SHADER_VALID( shaderId ) ){
		return( NULL );
	}
	return( &s_oArrShader[shaderId] );
}

//------------------
// 全データ読み込み
//------------------
bool CShaderMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CShaderMgr::CreateAll()\n" );
#endif

	// パックファイル読み込み
	stFPACK fpack;
	fpack_init( &fpack );
	if( !fpack_set( &fpack, "shader.fpk", true ) ){
		LOGE( "@ CShaderMgr::CreateAll: PACK LOAD FAILED\n" );
		env_push_error_log( "[shader.fpk]の読み込みに失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	s_oArrShader = new CShader[eSHADER_MAX];
	CMemMgr::PopTargetField();

    // 各データの読み込み
    CDefBinTable<CShaderBinData>* pTable = CDefTable::GetShaderTable();
	for( uint32 i=0; i<eSHADER_MAX; i++){
        CShaderBinData* pBin = pTable->getDataAt( i );
		if( ! s_oArrShader[i].applyBinWithFpack( pBin, &fpack ) ){
            const char* pVertexName = pBin->getVertexName();
            const char* pFragmentName = pBin->getFragmentName();
			char* buf = CMemMgr::GetBufFromTempStr();
			sprintf( buf, "シェーダー[%s][%s][%s]の設定に失敗しました。\n", g_pArrLabelShader[i], pVertexName, pFragmentName );
			env_push_error_log( buf );
			fpack_release( &fpack );
			return( false );
		}
	}

	// パック解放
	fpack_release( &fpack );

	return( true );
}
