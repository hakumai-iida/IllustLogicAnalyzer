/*+----------------------------------------------------------------+
  |	Title:		file_io_android.cpp [Android環境]
  |	Comment:	ファイル入出力
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include <sys/stat.h>

#include "env.hpp"
#include "sys/file/file_io.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ローカルデータ格納フォルダパス（※アプリ毎に設定）
#define LOCAL_DATA_FOLDER_PATH	"/data/data/jp.hakumai.appenv/files"

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
//------------------------------------------------
// [Android] ファイルを開く
//------------------------------------------------
bool file_io_open_file( stFILE_DESCRIPTOR* pFd, const char* pTarget, uint32* pSize ){
	if( pFd == NULL ){
		LOGE( "@ file_io_open_file: invalid fd\n" );
		return( false );
	}

	AAssetManager* pMgr = g_stEnvNative.pApp->activity->assetManager;
    pFd->pAsset = AAssetManager_open( pMgr, pTarget, AASSET_MODE_BUFFER );

    if( pFd->pAsset == NULL ){
    	LOGE( "@ file_io_open_file: [%s] open failed\n", pTarget );
    	return( false );
    }

    // 指定があればサイズ取得
    if( pSize != NULL ){
    	*pSize = AAsset_getLength( pFd->pAsset );
    }

	return( true );
}

//------------------------------------------------
// [Android] ファイルシーク
//------------------------------------------------
bool file_io_seek( stFILE_DESCRIPTOR* pFd, uint32 pos ){
	if( pFd == NULL ){
		LOGE( "@ file_io_seek: invalid fd\n" );
		return( false );
	}

	if( pFd->pAsset == NULL ){
		LOGE( "@ file_io_seek: invalid asset\n" );
		return( false );
	}

	if( AAsset_seek( pFd->pAsset, pos, SEEK_SET ) < 0 ){
		LOGE( "@ file_io_seek: seek failed\n" );
		return( false );
	}

	return( true );
}

//------------------------------------------------
// [Android] データの読み込み
//------------------------------------------------
bool file_io_read( stFILE_DESCRIPTOR* pFd, void* pBuf, uint32 size, DWORD* pReaded ){
	if( pFd == NULL ){
		LOGE( "@ file_io_read: invalid fd\n" );
		return( false );
	}

	if( pFd->pAsset == NULL ){
		LOGE( "@ file_io_read: invalid asset\n" );
		return( false );
	}

    int read = AAsset_read( pFd->pAsset, pBuf, size );

    if( read < 0 ){
    	LOGE( "@ file_io_read: read failed\n" );
    	return( false );
    }

	// 有効であればサイズの設定
    if( pReaded != NULL ){
    	*pReaded = read;
    }

	return( true );
}

//------------------------------------------------
// [Android] ファイルのクローズ
//------------------------------------------------
bool file_io_close( stFILE_DESCRIPTOR* pFd ){
	if( pFd == NULL ){
		LOGE( "@ file_io_close: invalid fd\n" );
		return( false );
	}

	if( pFd->pAsset == NULL ){
		LOGE( "@ file_io_close: invalid asset\n" );
		return( false );
	}

    AAsset_close( pFd->pAsset );
    pFd->pAsset = NULL;

	return( true );
}

//----------------------------------
// [Android] ローカルデータのロード
//----------------------------------
bool file_io_load_local_data( const char* pTarget, void* pBuf, uint32 size, uint32* pReaded, bool isRefCaches ){
	// アクセス先を調整
	char* buf = CMemMgr::GetBufFromTempStr();
	sprintf( buf, "%s/%s", LOCAL_DATA_FOLDER_PATH, pTarget );

	// ファイルを開く
	FILE* pFile = fopen( buf, "rb" );
	if( pFile == NULL ){
		LOGE( "@ file_io_load_local_data: file [%s] open failed\n", pTarget );
		return( false );
	}

	// ファイル読み込み～閉じる
	int readed = fread( pBuf, 1, size, pFile );
	fclose( pFile );

	if( pReaded != NULL ){
		*pReaded = (uint32)readed;
	}

	return( true );
}

//----------------------------------
// [Android] ローカルデータのセーブ
//----------------------------------
bool file_io_save_local_data( const char* pTarget, void* pBuf, uint32 size, uint32* pWrote, bool isSaveCaches ){
	// アクセス先を調整
	char* buf = CMemMgr::GetBufFromTempStr();
	sprintf( buf, "%s/%s", LOCAL_DATA_FOLDER_PATH, pTarget );

	// ファイルを開く
	FILE* pFile = fopen( buf, "wb" );
	if( pFile == NULL ){
		LOGE( "@ file_io_save_local_data: file [%s] open failed\n", pTarget );
  		return( false );
	}

	// ファイル書き込み～閉じる
	int readed = fwrite( pBuf, 1, size, pFile );
	fclose( pFile );

	// 読み取り許可を付与（※外部からアクセスできるように）
	int temp = chmod( buf,
			S_IRUSR | S_IWUSR | S_IXUSR |	// rwx
			S_IRGRP | S_IWGRP | S_IXGRP |	// rwx
			S_IROTH | S_IWOTH | S_IXOTH		// rwx
	);

	if( temp != 0 ){
		LOGE( "@ file_io_save_local_data:: [%s] chmod failed\n", pTarget );
	}

	if( pWrote != NULL ){
		*pWrote = (uint32) readed;
	}

	return( true );
}

//---------------------------------------------
// [Android] ドキュメントフォルダーのパスを取得
//---------------------------------------------
const char* get_documents_directory_path( void ){
	// [Android]では「ドキュメント/キャッシュ」の概念が無いため、同一のパスを返す
	const char* pDirPath = LOCAL_DATA_FOLDER_PATH;
	return ( pDirPath );
}

//-------------------------------------------
// [Android] キャッシュフォルダーのパスを取得
//-------------------------------------------
const char* get_caches_directory_path( void ){
	// [Android]では「ドキュメント/キャッシュ」の概念が無いため、同一のパスを返す
	const char* pDirPath = LOCAL_DATA_FOLDER_PATH;
	return ( pDirPath );
}
