/*+----------------------------------------------------------------+
  |	Title:		file_io.hpp [共通環境]
  |	Comment:	ファイル入出力
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef	__FILE_IO_HPP__
#define	__FILE_IO_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//---------------------------------
// ファイルディスクリプタ ： 環境により内容が変わる
//---------------------------------
struct stFILE_DESCRIPTOR{
#ifdef EXE_ANDROID
	// [Android] アセットインスタンス
	AAsset*	pAsset;
#endif

#ifdef EXE_IOS
	// [iOS] 普通のファイルポインタ（※長さは簡便のため）
    FILE* pFile;
    uint32 length;
#endif
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// ファイル操作（※リソースフォルダ向け）
extern bool file_io_open_file( stFILE_DESCRIPTOR* pFd, const char* pTarget, uint32* pSize=NULL );
extern bool file_io_seek( stFILE_DESCRIPTOR* pFd, uint32 pos );
extern bool file_io_read( stFILE_DESCRIPTOR* pFd, void* pBuf, uint32 size, uint32* pReaded=NULL );
extern bool file_io_close( stFILE_DESCRIPTOR* pFd );

// ファイル操作（※ローカルデータフォルダ向け）
extern bool file_io_load_local_data( const char* pTarget, void* pBuf, uint32 size, uint32* pReaded=NULL, bool isRefCaches=false );
extern bool file_io_save_local_data( const char* pTarget, void* pBuf, uint32 size, uint32* pWrote=NULL, bool isSaveCaches=false );

// ドキュメントフォルダーのパス取得
extern const char* get_documents_directory_path( void );
// キャッシュフォルダーのパス取得
extern const char* get_caches_directory_path( void );

#endif	/* __FILE_IO_HPP__ */
