/*+----------------------------------------------------------------+
  |	Title:		FileMgr.cpp [共通環境]
  |	Comment:	ファイル管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FileMgr.hpp"

#include "sys/sdarc/sdarc.hpp"
#include "sys/util/util_encrypt.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CFileMgr::Init( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFileMgr::Init()\n" );
#endif

	return( true );
}

//------------------------
// 終了
//------------------------
void CFileMgr::Exit( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFileMgr::Exit()\n" );
#endif
}

//------------------------------------------------------
// ファイルを指定バッファに読み込む
//------------------------------------------------------
bool CFileMgr::LoadFile( const char* pTarget, void* pBuf, uint32 sizeBuf, uint32* pReaded ){
	stFILE_DESCRIPTOR fd;

	// ファイルを開く
	if( !OpenFile( &fd, pTarget, NULL ) ){
		return( false );
	}

	// 読み込み
	bool isSuccess = ReadData( &fd, pBuf, sizeBuf, pReaded );
	CFileMgr::CloseFile( &fd );

	return( isSuccess );
}

//------------------------------------------------------
// sdarcファイルを解凍してバッファに読み込む
//------------------------------------------------------
bool CFileMgr::LoadFileSdarc( const char* pTarget, void* pBufIn, uint32 sizeBufIn, void* pBufOut, uint32 sizeBufOut, uint32* pSizeData ){
	// データ読み込み
	uint32 readed;
	if( !LoadFile( pTarget, pBufIn, sizeBufIn, &readed ) ){
		return( false );
	}

	// データ解凍
	uint32 size = sdarc_unpack( pBufOut, sizeBufOut, pBufIn, readed );

	if( size <= 0 ){
		return( false );
	}

	*pSizeData = size;
	return( true );
}

//------------------------------------------------
// ファイルを開く
//------------------------------------------------
bool CFileMgr::OpenFile( stFILE_DESCRIPTOR* pFd, const char* pTarget, uint32* pSize ){
	return( file_io_open_file( pFd, pTarget, pSize ) );
}

//------------------------------------------------
// ファイルシーク
//------------------------------------------------
bool CFileMgr::Seek( stFILE_DESCRIPTOR* pFd, uint32 target ){
	return( file_io_seek( pFd, target ) );
}

//------------------------------------------------
// データの読み込み
//------------------------------------------------
bool CFileMgr::ReadData( stFILE_DESCRIPTOR* pFd, void* pBuf, uint32 size, DWORD* pReaded ){
	return( file_io_read( pFd, pBuf, size, pReaded ) );
}

//------------------------------------------------
// ファイルのクローズ
//------------------------------------------------
bool CFileMgr::CloseFile( stFILE_DESCRIPTOR* pFd ){
	return( file_io_close( pFd ) );
}

//------------------------------------------------
// ローカルデータの読み込み
//------------------------------------------------
bool CFileMgr::LoadLocalData( const char* pTarget, void* pBuf, uint32 size, uint32* pReaded, bool isObfuscate, bool isRefCaches ){
	// 難読化ファイル読み込み
	if( isObfuscate ){
#if 1
		// ローカルデータ読み込み
		uint32 readed;
		if( !file_io_load_local_data( pTarget, pBuf, size, &readed ) ){
			return( false );
		}

		// 解読
		util_encrypt_decode( pBuf, readed );
		if( pReaded != NULL ){ *pReaded = readed; }
		return( true );
#else
		// 解凍（※時間がかかるので不採用）
		void* pBufIn = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_IN );
		uint32 sizeIn = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_IN );

		// ローカルデータ読み込み
		uint32 readedIn;
		if( !file_io_load_local_data( pTarget, pBufIn, sizeIn, &readedIn ) ){
			return( false );
		}

		// 解読
		util_encrypt_decode( pBufIn, readedIn );

		// 解凍
		int readed = sdarc_unpack( pBuf, size, pBufIn, readedIn );
		if( pReaded != NULL ){ *pReaded = readed; }

		// 解凍の正否はサイズの有効性で判断
		return( readed >= 0 );
#endif
	}

	// 通常読み込み
	return( file_io_load_local_data( pTarget, pBuf, size, pReaded, isRefCaches ) );
}

//------------------------------------------------
// ローカルデータの書き込み
//------------------------------------------------
bool CFileMgr::SaveLocalData( const char* pTarget, void* pBuf, uint32 size, uint32* pWrote, bool isObfuscate, bool isSaveCaches ){
	// 難読化ファイル書き込み
	if( isObfuscate ){
#if 1
		// 暗号化して書き込む
		util_encrypt_encode( pBuf, size );
		return( file_io_save_local_data( pTarget, pBuf, size, pWrote ) );
#else
		// 圧縮（※時間がかかるので不採用）
		void* pBufOut = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_OUT );
		uint32 sizeOut = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_OUT );
		uint32 writeOut = sdarc_pack( pBufOut, sizeOut, pBuf, size );

		// 暗号化して書き込む
		util_encrypt_encode( pBufOut, writeOut );
		return( file_io_save_local_data( pTarget, pBufOut, writeOut, pWrote ) );
#endif
	}

	// 通常書き込み
	return( file_io_save_local_data( pTarget, pBuf, size, pWrote, isSaveCaches ) );
}

//------------------------------------------------
// ローカルデータをキャッシュフォルダーから読み込み
//------------------------------------------------
bool CFileMgr::LoadLocalDataFromCaches( const char* pTarget, void* pBuf, uint32 size, uint32* pReaded, bool isObfuscate ){
    return( CFileMgr::LoadLocalData( pTarget, pBuf, size, pReaded, isObfuscate, true ) );
}

//------------------------------------------------
// ローカルデータをキャッシュフォルダーに書き込む
//------------------------------------------------
bool CFileMgr::SaveLocalDataToCaches( const char* pTarget, void* pBuf, uint32 size, uint32* pWrote, bool isObfusecate ){
    return ( CFileMgr::SaveLocalData( pTarget, pBuf, size, pWrote, isObfusecate, true ) );
}

//------------------------------------------------
// キャッシュフォルダーのパス取得
//------------------------------------------------
const char* CFileMgr::GetCachesDirectoryPath( void ) {
    return ( get_caches_directory_path() );
}
