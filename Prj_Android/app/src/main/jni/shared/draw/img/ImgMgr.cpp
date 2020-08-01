/*+----------------------------------------------------------------+
  |	Title:		CImgMgr.cpp [共通環境]
  |	Comment:	画像管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ImgMgr.hpp"

#include "sys/file/fpack.hpp"

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
CImg* CImgMgr::s_oArrImg;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CImgMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CImgMgr::OnCreate()\n" );
#endif

	if( !CreateAll() ){
		LOGE( "@ CImgMgr::CreateAll(): FAILED!!\n" );
		env_push_error_log( "[CImgMgr::CreateAll()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CImgMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CImgMgr::OnDestroy()\n" );
#endif

	// 解放
	SAFE_DELETE_ARRAY( s_oArrImg );
}

//-------------------------
// データ作成
//-------------------------
bool CImgMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CImgMgr::CreateAll()\n" );
#endif

	// パックファイル
	stFPACK fpack;
	fpack_init( &fpack );
	if( ! fpack_set( &fpack, "image.fpk", true ) ){
		LOGE( "@ CImgMgr::CreateAll: PACK LOAD FAILED\n" );
		env_push_error_log( "[image.fpk]の読み込みに失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	s_oArrImg = new CImg[eIMG_MAX];
    CMemMgr::PopTargetField();

    // 各データの読み込み
    CDefBinTable<CImgBinData>* pTable = CDefTable::GetImgTable();
	for( uint32 i=0; i<eIMG_MAX; i++){
        CImgBinData* pBin = pTable->getDataAt( i );
		if( ! s_oArrImg[i].applyBinWithFpack( pBin, &fpack ) ){
			char* buf = CMemMgr::GetBufFromTempStr();
			sprintf( buf, "画像[%s][%s]の設定に失敗しました。\n", g_pArrLabelImg[i], pBin->getFileName() );
			env_push_error_log( buf );
			fpack_release( &fpack );
			return( false );	// FATAL ERROR
		}
	}

	// パック解放
	fpack_release( &fpack );

	return( true );
}

//------------------
// 取得：画像
//------------------
CImg* CImgMgr::GetImg( eIMG imgId ){
	if( ! IS_IMG_VALID( imgId ) ){
		return( NULL );
	}
	return( &s_oArrImg[imgId] );
}

//-----------------------
// 取得：画像（ピクセル率指定）
//-----------------------
CImg* CImgMgr::GetImgWithPixelRate( eIMG imgId, int rate ){
    if( rate <= 0 ){
        rate = GM_P_RATE;
    }
    imgId = (eIMG)(imgId + (rate - REQUIRED_P_RATE));
    return( GetImg( imgId ) );
}

//------------------
// 取得：画素
//------------------
CTex* CImgMgr::GetImgDot( eIMG imgId, int at ){
	CImg* pImg = GetImg( imgId );
	if( pImg != NULL ){
		return( pImg->getDot( at ) );
	}

	return( NULL );
}

//------------------
// 取得：パレット
//------------------
CTex* CImgMgr::GetImgPal( eIMG imgId, int at ){
	CImg* pImg = GetImg( imgId );
	if( pImg != NULL ){
		return( pImg->getPal( at ) );
	}

	return( NULL );
}
