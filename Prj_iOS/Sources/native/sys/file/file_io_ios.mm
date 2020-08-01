/*+----------------------------------------------------------------+
  |	Title:		file_io_ios.cpp [iOS環境]
  |	Comment:	ファイル入出力
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sys/file/file_io.hpp"
#include <sys/stat.h>

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
//------------------------------------------------
// [iOS] ファイルを開く
//------------------------------------------------
bool file_io_open_file( stFILE_DESCRIPTOR* pFd, const char* pTarget, uint32* pSize ){
	if( pFd == NULL ){
		LOGE( "@ file_io_open_file: invalid fd\n" );
		return( false );
	}
    
    // リソースフォルダのパスを取得
    NSString* dirPath = [[NSBundle mainBundle] resourcePath];
    const char* pDirPath = [dirPath UTF8String];

    // リソースのパスの設定
    char* buf = CMemMgr::GetBufFromTempStr();
    sprintf( buf, "%s/%s", pDirPath, pTarget );
    
    // ファイルを開く
    FILE* pF = fopen( buf, "rb" );
    if( pF == NULL ){
    	LOGE( "@ file_io_open_file: [%s] not found\n", pTarget );
    	return( false );
    }
    
    // サイズの取得
    fpos_t length;
    fseek( pF, 0, SEEK_END );
    fgetpos( pF, &length );
    
    // 位置は戻しておく
    fseek( pF, 0, SEEK_SET );

    // 指定があればサイズ返却
    if( pSize != NULL ){
    	*pSize = (uint32) length;
    }

    // ここまできたら成功（※ディスクリプタに設定）
    pFd->pFile = pF;
    pFd->length = (uint32) length;
	return( true );
}

//------------------------------------------------
// [iOS]ファイルシーク
//------------------------------------------------
bool file_io_seek( stFILE_DESCRIPTOR* pFd, uint32 pos ){
	if( pFd == NULL ){
		LOGE( "@ file_io_seek: invalid fd\n" );
		return( false );
	}

	if( pFd->pFile == NULL ){
		LOGE( "@ file_io_seek: invalid file pointer\n" );
		return( false );
	}

	fseek( pFd->pFile, pos, SEEK_SET );

	return( true );
}

//------------------------------------------------
// [iOS]データの読み込み
//------------------------------------------------
bool file_io_read( stFILE_DESCRIPTOR* pFd, void* pBuf, uint32 size, DWORD* pReaded ){
	if( pFd == NULL ){
		LOGE( "@ file_io_read: invalid fd\n" );
		return( false );
	}

	if( pFd->pFile == NULL ){
		LOGE( "@ file_io_read: invalid file pointer\n" );
		return( false );
	}

    int read = (int)fread( pBuf, 1, (size_t)size, pFd->pFile );

    if( read < 0 ){
    	LOGE( "@ file_io_read: read failed\n" );
    	return( false );
    }

    if( pReaded != NULL ){
    	*pReaded = read;
    }

	return( true );
}

//------------------------------------------------
// [iOS]ファイルのクローズ
//------------------------------------------------
bool file_io_close( stFILE_DESCRIPTOR* pFd ){
	if( pFd == NULL ){
		LOGE( "@ file_io_close: invalid fd\n" );
		return( false );
	}

	if( pFd->pFile == NULL ){
		LOGE( "@ file_io_close: invalid file pointer\n" );
		return( false );
	}

    fclose( pFd->pFile );
    
    // ファイルポインタは無効化
    pFd->pFile = NULL;

	return( true );
}

//----------------------------------
// [iOS]ローカルデータのロード
//----------------------------------
bool file_io_load_local_data( const char* pTarget, void* pBuf, uint32 size, uint32* pReaded, bool isRefCaches ){
    // ローカルフォルダのパスを取得
    const char* pDirPath = !isRefCaches ? get_documents_directory_path() : get_caches_directory_path();
    
    char* buf = CMemMgr::GetBufFromTempStr();
    sprintf( buf, "%s/%s", pDirPath, pTarget );
    
    FILE* pF = fopen( buf, "rb" );
    if( pF == NULL ){
		LOGE( "@ file_io_load_local_data: file [%s] can't open\n", pTarget );
    	return( false );
    }
    
    // サイズの確認
    fpos_t length;
    fseek( pF, 0, SEEK_END );
    fgetpos( pF, &length );
    
    if( length > size ){
    	LOGE( "@ file_io_load_local_data: too large for the buffer size: %d > %d\n", (int32)length, size );
        fclose( pF );
    	return( false );
    }
    
    // 位置は戻しておく
    fseek( pF, 0, SEEK_SET );
    
	// ファイル読み込み～閉じる
	int readed = (int)fread( pBuf, 1, (size_t)size, pF );
	fclose( pF );

	if( pReaded != NULL ){
		*pReaded = (uint32)readed;
	}

	return( true );
}

//----------------------------------
// [iOS]ローカルデータのセーブ
//----------------------------------
bool file_io_save_local_data( const char* pTarget, void* pBuf, uint32 size, uint32* pWrote, bool isSaveCaches ){
    
    // ローカルフォルダのパスを取得
    const char* pDirPath = !isSaveCaches ? get_documents_directory_path() : get_caches_directory_path();
    
    char* buf = CMemMgr::GetBufFromTempStr();
    sprintf( buf, "%s/%s", pDirPath, pTarget );
    
    FILE* pF = fopen( buf, "wb" );
    if( pF == NULL ){
		LOGE( "@ file_io_save_local_data: file [%s] can't open\n", pTarget );
    	return( false );
    }
    
	// ファイル書き込み～閉じる
	int wrote = (int)fwrite( pBuf, 1, (size_t)size, pF );
	fclose( pF );
    
	if( pWrote != NULL ){
		*pWrote = (uint32)wrote;
	}
    
	return( true );
}

//----------------------------------
// [iOS]ドキュメントフォルダーのパスを取得
//----------------------------------
const char* get_documents_directory_path( void ){
    NSString* appHomePath = NSHomeDirectory();
    NSString* dirPath = [appHomePath stringByAppendingPathComponent: @"Documents" ];
    const char* pDirPath = [dirPath UTF8String];
    return ( pDirPath );
}

//----------------------------------
// [iOS]キャッシュフォルダーのパスを取得
//----------------------------------
const char* get_caches_directory_path( void ){
    NSString* appHomePath = NSHomeDirectory();
    NSString* dirPath = [appHomePath stringByAppendingPathComponent: @"Library/Caches" ];
    const char* pDirPath = [dirPath UTF8String];
    return ( pDirPath );
}
