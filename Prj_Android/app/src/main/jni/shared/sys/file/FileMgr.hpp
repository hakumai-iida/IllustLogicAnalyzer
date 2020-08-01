/*+----------------------------------------------------------------+
  |	Title:		FileMgr.hpp [共通環境]
  |	Comment:	ファイル管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FILE_MGR_HPP__
#define __FILE_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "file_io.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ファイル管理インターフェイス
//----------------------------------------
class CFileMgr{
public:
	// 初期化／終了
	static bool Init( void );
	static void Exit( void );

	//----------------------------------------------------------
	// ファイル読み込み（※読み込み専用のリソース領域）
	//----------------------------------------------------------
	static bool LoadFile( const char* pTarget, void* pBuf, uint32 sizeBuf, uint32* pReaded=NULL );
	static bool LoadFileSdarc( const char* pTarget, void* pBufIn, uint32 sizeBufIn, void* pBufOut, uint32 sizeBufOut, uint32* pSizeData );

	//----------------------------------------------------------
	// ファイル操作（※読み込み専用のリソース領域）
	//----------------------------------------------------------
#ifdef EXE_ANDROID
	// [Android] アセットが有効なら
	static bool CheckFileDescriptor( stFILE_DESCRIPTOR* pFd ){ return( pFd->pAsset != NULL ); }
#endif
#ifdef EXE_IOS
	// [iOS] ファイルポインタが有効なら
    static bool CheckFileDescriptor( stFILE_DESCRIPTOR* pFd ){ return( pFd->pFile != NULL ); }
#endif
	static void ClearFileDescriptor( stFILE_DESCRIPTOR* pFd ){ memset( pFd, 0, sizeof(stFILE_DESCRIPTOR) ); }
	static bool OpenFile( stFILE_DESCRIPTOR* pFd, const char* pTarget, uint32* pSize=NULL );
	static bool Seek( stFILE_DESCRIPTOR* pFd, uint32 target );
	static bool ReadData( stFILE_DESCRIPTOR* pFd, void* pBuf, uint32 size, uint32* pReaded=NULL );
	static bool CloseFile( stFILE_DESCRIPTOR* pFd );

	//-------------------------------------------------------------------------------
	// ローカルデータ操作（※読み書きできるローカル領域）
	// [isObfuscate]が指定された場合、[SDARC_IN]／[SDARC_OUT]を作業バッファとして使う
	//-------------------------------------------------------------------------------
	static bool LoadLocalData( const char* pTarget, void* pBuf, uint32 size, uint32* pReaded=NULL, bool isObfuscate=false, bool isRefCaches=false );
	static bool SaveLocalData( const char* pTarget, void* pBuf, uint32 size, uint32* pWrote=NULL, bool isObfusecate=false, bool isSaveCaches=false );
    static bool LoadLocalDataFromCaches( const char* pTarget, void* pBuf, uint32 size, uint32* pReaded=NULL, bool isObfuscate=false );
    static bool SaveLocalDataToCaches( const char* pTarget, void* pBuf, uint32 size, uint32* pWrote=NULL, bool isObfusecate=false );
    
    // キャッシュフォルダーのパス取得
    static const char* GetCachesDirectoryPath( void );
    
private:
	// インスタンス生成は不可
	CFileMgr( void ){};
	virtual ~CFileMgr( void ){};
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __FILE_MGR_HPP__
