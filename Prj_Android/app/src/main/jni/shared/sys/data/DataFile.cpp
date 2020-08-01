/*+----------------------------------------------------------------+
  |	Title:		DataFile.hpp [共通環境]
  | Comment:    データファイル（※ロード／セーブをするデータ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DataFile.hpp"

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
//-----------------------------------
// ロード（※アセット／ローカルから）
//-----------------------------------
bool CDataFile::load( const char* pFileName, const char* pDirName, bool isLoadFromLocal, bool isTemporary ){
	// ファイル名設定
	char* fileName = CMemMgr::GetBufFromTempStr();
	if( ! CConst::SetFileName( fileName, pFileName, NULL, isTemporary ) ){
		LOGE( "@ CDataFile::load: INVALID filaName\n" );
		return( false );
	}

	// データ読み込み
	void* pBuf = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_DATA );
	uint32 size = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_DATA );
	uint32 readed;

	// ローカル指定の場合フォルダ指定は無視
	if( isLoadFromLocal || isTemporary ){
		if( ! CFileMgr::LoadLocalData( fileName, pBuf, size, &readed ) ){
			LOGE( "@ CDataFile::load(LOCAL): LOAD FAILED: %s\n", fileName );
			return( false );
		}
	}
	// アセット指定の場合はフォルダ指定が有効
	else{
		char* buf = CMemMgr::GetBufFromTempStr();
		if( pDirName != NULL && pDirName[0] != '\0' ){
			sprintf( buf, "%s/%s", pDirName, fileName );
		}else{
			sprintf( buf, "%s", fileName );
		}

		if( ! CFileMgr::LoadFile( buf, pBuf, size, &readed ) ){
			LOGE( "@ CDataFile::load(ASSET): LOAD FAILED: %s\n", buf );
			return( false );
		}
	}

	// 入力バッファ（※リトルエンディアン）
	CInputBuffer ib;
	ib.setBuffer( pBuf, readed );

	// 読み込み（※リトルエンディアン）
	ib.setForLittleEndian();
	load0( &ib );

	LOGD( "@ LOADED: fileName=%s, pDirName=%s, readed=%u, isLocal=%d, isTemp=%d\n", fileName, pDirName, readed, isLoadFromLocal, isTemporary );

	return( true );
}

//-----------------------
// セーブ（※ローカルへ）
//-----------------------
bool CDataFile::save( const char* pFileName, bool isTemporary ){
	// ファイル名設定
	char* fileName = CMemMgr::GetBufFromTempStr();
	if( ! CConst::SetFileName( fileName, pFileName, NULL, isTemporary ) ){
		LOGE( "@ CDataFile::save: INVALID filaName\n" );
		return( false );
	}

	// バッファ確保
	void* pBuf = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_DATA );
	uint32 size = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_DATA );

	// 出力バッファ（※リトルエンディアン）
	COutputBuffer ob;
	ob.setBuffer( pBuf, size );
	ob.setForLittleEndian();

	// 書き込み
	save0( &ob );

	// 出力バッファ解放
	int sizeData = ob.getSize();

	// ファイルへ保存
	if( ! CFileMgr::SaveLocalData( fileName, pBuf, sizeData ) ){
		LOGE( "@ CDataFile::save(LOCAL): SAVE FAILED: %s\n", fileName );
		return( false );
	}

	LOGD( "@ WROTE: fileName=%s, wrote=%u, isTemp=%d\n", fileName, sizeData, isTemporary );
	return( true );
}
