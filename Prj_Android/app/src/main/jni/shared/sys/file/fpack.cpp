/*+----------------------------------------------------------------+
  |	Title:		fpack.cpp [共通環境]
  |	Comment:	ファイルパック(※[fpack.exe]の実装に依存)
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "fpack.hpp"

#include "sys/sdarc/sdarc.hpp"
#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 取得系でパックの有効確認をするか？（※正常なデータアクセスであれば不要な処理）
//#define VALID_CEHCK_FOR_GET

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	関数定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------
// 初期化
//----------------------
void fpack_init( stFPACK* pPack ){
	memset( pPack, 0, sizeof(stFPACK) );

	// 作業バッファのデフォルト設定
	fpack_set_share_id( pPack, eMEM_FIELD_SHARE_FPACK );
	fpack_set_read_buf( pPack, CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_IN ), CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_IN ) );
	fpack_set_data_buf( pPack, CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_OUT ), CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_OUT ) );
}

//----------------------
// 解放
//----------------------
void fpack_release( stFPACK* pPack ){
	// 有効であればリソース解放
	if( fpack_is_valid( pPack ) ){
		fpack_close_file( pPack );

		pPack->pBufFile  = NULL;
		SAFE_DELETE_ARRAY( pPack->stArrCell );
		SAFE_DELETE_ARRAY( pPack->cArrPathData );
		SAFE_DELETE_ARRAY( pPack->pArrPath );
	}

	// 無効化（※共有情報は保持しておく）
	memset( &pPack->header, 0, sizeof(stFPACK_HEADER) );
	pPack->fileName[0] = '\0';
	pPack->isLoadAll = false;
}

//----------------------
// 判定
//----------------------
bool fpack_is_valid( stFPACK* pPack ){
	// ヘッダの配列が有効なら正常なfpackファイル
	if( pPack != NULL && pPack->stArrCell != NULL ){
		return( true );
	}
	return( false );
}

//---------------------------
// 読み込みバッファ設定
//---------------------------
void fpack_set_read_buf( stFPACK* pPack, void* pBuf, DWORD size ){
	pPack->pBufRead = pBuf;
	pPack->sizeRead = size;
}

//---------------------------
// データバッファ設定
//---------------------------
void fpack_set_data_buf( stFPACK* pPack, void* pBuf, DWORD size ){
	pPack->pBufData = pBuf;
	pPack->sizeData = size;
}

//---------------------------
// 共有メモリ設定
//---------------------------
void fpack_set_share_id( stFPACK* pPack, eMEM_FIELD_SHARE useShareId ){
	pPack->useShareId = useShareId;
}

//----------------------
// 設定
//----------------------
bool fpack_set( stFPACK* pPack, const char* file_name, bool isLoadAll, bool noClose ){
	// 同じファイルが設定済みなら
	if( fpack_is_valid( pPack ) && util_str_cmp( pPack->fileName, file_name ) == 0 ){
		// 読み込み指定が変わっていなければ（※指定が変化した場合は読み直す）
		if( pPack->isLoadAll == isLoadAll ){
			if( noClose ){ fpack_open_file( pPack ); }	// クローズ不可指定の場合は開いておく
			else{ fpack_close_file( pPack ); }
			return( true );
		}
	}

	// 用心にリリース
	fpack_release( pPack );

	stFPACK_HEADER* pHead = &pPack->header;
	stFILE_DESCRIPTOR fd;
	DWORD size;

	// ファイルのオープン
	if( !CFileMgr::OpenFile( &fd, file_name, &size ) ){
		return( false );
	}

	//-----------------
	// 全読み指定
	//-----------------
	if( isLoadAll ){
		// バッファの設定
		if( size > CMemMgr::GetSizeFromShare( pPack->useShareId ) ){
			LOGE( "@  == fpack_set( %s ): SHARE MEMORY SHORTAGE: [%d > %d]  ==\n", file_name, size, CMemMgr::GetSizeFromShare( pPack->useShareId ) );
			CFileMgr::CloseFile( &fd );
			return( false );
		}
		pPack->pBufFile = CMemMgr::GetAddrFromShare( pPack->useShareId );

		// 全データ読み込み
		if( !CFileMgr::ReadData( &fd, pPack->pBufFile, size ) ){
			CFileMgr::CloseFile( &fd );
			return( false );
		}

		// 入力バッファ（※リトルエンディアン）
		CInputBuffer ib;
		ib.setBuffer( pPack->pBufFile, size );
		ib.setForLittleEndian();

		// ヘッダ情報の読み込み
		pHead->numFile = ib.readUint32();
		pHead->ofsFilePath = ib.readUint32();
		pHead->sizeFilePath = ib.readUint32();

		// データ配列領域の読み込み
		int num = pHead->numFile;
		pPack->stArrCell = new stFPACK_CELL[num];
		if( pPack->stArrCell == NULL ){
			printf( "fpack_set malloc error(%s)\n", file_name );
            CFileMgr::CloseFile( &fd );
			return( false );
		}

		for( int i=0; i<num; i++ ){
			pPack->stArrCell[i].offset = ib.readUint32();
			pPack->stArrCell[i].size = ib.readUint32();
		}

		// 名前データが有効であれば確保＆読み込み
		if( pHead->ofsFilePath > 0 ){
			pPack->cArrPathData = new char[pHead->sizeFilePath];
			if( pPack->cArrPathData == NULL ){
				printf( "fpack_set malloc error(%s)\n", file_name );
                CFileMgr::CloseFile( &fd );
				return( false );
			}

			ib.seek( pHead->ofsFilePath );
			ib.readBlock( pPack->cArrPathData, pHead->sizeFilePath );
		}

		// 全読みフラグを立てておく
		pPack->isLoadAll = true;
	}
	//-----------------
	// 逐次読み指定
	//-----------------
	else{
		// ヘッダ情報の読み込み：１２バイト
		BYTE buf[64];
		if( ! CFileMgr::ReadData( &fd, buf, 12 ) ){ return( false ); }

		// 入力バッファ（※リトルエンディアン）
		CInputBuffer ib;
		ib.setBuffer( buf, 12 );
		ib.setForLittleEndian();

		pHead->numFile = ib.readUint32();
		pHead->ofsFilePath = ib.readUint32();
		pHead->sizeFilePath = ib.readUint32();

		// データ配列領域の確保
		int num = pPack->header.numFile;
		pPack->stArrCell = new stFPACK_CELL[num];
		if( pPack->stArrCell == NULL ){
			printf( "fpack_set malloc error(%s)\n", file_name );
            CFileMgr::CloseFile( &fd );
			return( false );
		}

		// 配列データ読み込み
		for( int i=0; i<num; i++ ){
			// ファイル情報の読み込み：８バイト
			if( ! CFileMgr::ReadData( &fd, buf, 8 ) ){ return( false ); }

			ib.setBuffer( buf, 8 );
			ib.setForLittleEndian();

			pPack->stArrCell[i].offset = ib.readUint32();
			pPack->stArrCell[i].size = ib.readUint32();
		}

		// 名前データが有効であれば領域確保＆読み込み
		if( pHead->ofsFilePath > 0 ){
			pPack->cArrPathData = new char[pHead->sizeFilePath];
			if( pPack->cArrPathData == NULL ){
				printf( "fpack_set malloc error(%s)\n", file_name );
                CFileMgr::CloseFile( &fd );
				return( false );
			}

			CFileMgr::Seek( &fd, (DWORD)pHead->ofsFilePath );
			if( ! CFileMgr::ReadData( &fd, pPack->cArrPathData, (DWORD)pHead->sizeFilePath ) ){
                printf( "fpack_set read error(%s)\n", file_name );
                CFileMgr::CloseFile( &fd );
                return( false );
            }
		}
	}

	// 名前データが有効であればポインタ配列へ設定
	if( pHead->ofsFilePath > 0 ){
		pPack->pArrPath = new char*[pHead->numFile];
		if( pPack->pArrPath == NULL ){
			printf( "fpack_set malloc error(%s)\n", file_name );
            CFileMgr::CloseFile( &fd );
			return( false );
		}

		// 文字列配列の設定
		int i, cur = 0;
		for( i=0; i<pHead->numFile-1; i++ ){
			pPack->pArrPath[i] = &pPack->cArrPathData[cur];

			// 次の文字列開始位置まで移動（※終端指定の０が続く場合がある点に注意）
			while( pPack->cArrPathData[cur] != '\0' ){ cur++; }
			while( pPack->cArrPathData[cur] == '\0' ){ cur++; }
		}

		// 最終要素（※最終要素の終端判定で配列外参照がおこる可能性があるのでループ外で）
		pPack->pArrPath[i] = &pPack->cArrPathData[cur];
	}

	// ファイル情報の保存（※バッファに収まらない場合はエラー）
	if( ! util_str_cpy( pPack->fileName, file_name, COMMON_NAME_LEN ) ){
		LOGE( "@ fpack_set: BUF SHORTAGE: %d >= %d, file_name=%s\n", util_str_len(file_name), COMMON_NAME_LEN, file_name );
		CFileMgr::CloseFile( &fd );
		return( false );
	}

	// クローズ不可指定があればファイルディスクリプタを保持しておく
	if( noClose ){
		pPack->fd = fd;	// 丸毎コピー
	}else{
		CFileMgr::CloseFile( &fd );
		CFileMgr::ClearFileDescriptor( &pPack->fd );
	}

	return( true );
}

//-----------------------------------
// リセット（※ファイルの開き直し）
//-----------------------------------
bool fpack_reset( stFPACK* pPack ){
	// 無効は無視
	if( !fpack_is_valid( pPack ) ){ return( false ); }

	// ファイルディスクリプタが無効なら無視（※正常終了）
	if( !CFileMgr::CheckFileDescriptor( &pPack->fd ) ){ return( true ); }

	// ファイルを閉じる（※事前に[CFileMgr]のリセットが行われている場合にこの処理はエラーとなるが、動作上問題無しとする）
	CFileMgr::CloseFile( &pPack->fd );
	CFileMgr::ClearFileDescriptor( &pPack->fd );

	// ファイルを開き直す
	return( CFileMgr::OpenFile( &pPack->fd, pPack->fileName ) );
}

//--------------------------
// ファイルを開く
//--------------------------
bool fpack_open_file( stFPACK* pPack ){
	// 無効は無視
	if( !fpack_is_valid( pPack ) ){ return( false ); }

	// 全読みの場合／オープン済みは正常終了
	if( pPack->isLoadAll || CFileMgr::CheckFileDescriptor( &pPack->fd ) ){ return( true ); }

	// ファイルのオープン
	return( CFileMgr::OpenFile( &pPack->fd, pPack->fileName ) );
}

//--------------------------
// ファイルを閉じる
//--------------------------
void fpack_close_file( stFPACK* pPack ){
	// 無効は無視
	if( !fpack_is_valid( pPack ) || !CFileMgr::CheckFileDescriptor( &pPack->fd ) ){ return; }

	CFileMgr::CloseFile( &pPack->fd );
	CFileMgr::ClearFileDescriptor( &pPack->fd );
}

//--------------------------
// 要素数取得
//--------------------------
int fpack_get_num( stFPACK* pPack ){
#ifdef VALID_CHECK_FOR_GET
	// 有効性確認
	if( !fpack_is_valid( pPack ) ){ return( 0 ); }
#endif

	return( pPack->header.numFile );
}

//--------------------------
// ファイル情報取得
//--------------------------
bool fpack_get_info( stFPACK* pPack, int id, DWORD* pOfs, DWORD* pSize ){
#ifdef VALID_CHECK_FOR_GET
	// 有効性確認
	if( !fpack_is_valid( pPack ) ){ return( false ); }
#endif

	// 無効は無視
	if( id < 0 || id >= pPack->header.numFile ){
		LOGE( "@ fpack_get_info: INVALID ACCESS: id=%d/%d, \n", id, pPack->header.numFile );
		return( false );
	}

	// 属性設定
	*pOfs  = pPack->stArrCell[id].offset;
	*pSize = pPack->stArrCell[id].size;
	return( true );
}

//--------------------------------------------------
// データ取得（※事前にデータファイルを開いておく必要がある）
//--------------------------------------------------
void* fpack_get_data( stFPACK* pPack, int id, DWORD* pSize, bool isUnpack ){
	void* pBuf = NULL;
	DWORD ofs, size, workSize;

	// 無効は無視
	if( !fpack_get_info( pPack, id, &ofs, &size ) ){
		return( NULL );
	}

	// 読み込みバッファサイズ確認
	if( size > pPack->sizeRead ){
		LOGE( "@ == fpack_get_data(%d) READ SIZE SHORTAGE [%d > %d]\n", id, size, pPack->sizeRead );
		return( NULL );
	}

	// 圧縮データの読み込み
	if( pPack->isLoadAll ){
		pBuf = &(((BYTE*)pPack->pBufFile)[ofs]);
	}else{
		// ファイルが開いていなければ失敗
		if( !CFileMgr::CheckFileDescriptor( &pPack->fd ) ){ return( NULL ); }

		// シーク／読み込みできなければ失敗
		if( !CFileMgr::Seek( &pPack->fd, ofs ) || !CFileMgr::ReadData( &pPack->fd, pPack->pBufRead, size ) ){
			return( NULL );
		}

		pBuf = pPack->pBufRead;
	}

	// データ読み込み
	if( pBuf != NULL ){
		// 解凍指定があれば
		if( isUnpack ){
			if( sdarc_is_valid( pBuf, size, &workSize ) ){
                if( workSize > pPack->sizeData ){
                    LOGE( "@ size = %d\n", size );
                    LOGE( "@  == fpack_get_data(%d) DATA SIZE SHORTAGE [%d > %d]\n", id, workSize,
                          pPack->sizeData );
                    return ( NULL );
                }

                // 解凍
                size = sdarc_unpack( pPack->pBufData, pPack->sizeData, (BYTE*) pBuf, size );

                // データバッファを返す
                if( pSize != NULL ){ *pSize = size; }
                return ( pPack->pBufData );
            }else{
                return( NULL );
            }
		}

		// ここに来た場合は読み込みバッファを返す
		if( pSize != NULL ){ *pSize = size; }
		// 一括読み込み時は読み込みバッファにコピーして返す（※呼び出し元でデータを修正する可能性がある）
		if( pPack->isLoadAll ){
			memcpy( pPack->pBufRead, pBuf, size );
		}
		return( pPack->pBufRead );
	}

	return( NULL );
}

//------------------------------------
// データ取得（※ストリーミング用）
//------------------------------------
DWORD fpack_streaming( stFPACK* pPack, int id, DWORD readOfs, DWORD readSize, void* pReadBuff ){
	DWORD ofs, size, readed;
	void* pBuf = NULL;

	// 無効は無視
	if( !fpack_get_info( pPack, id, &ofs, &size ) ){
		return( 0 );
	}

	if( readOfs >= size ){
		LOGE( "@ fpack_streaming: INVALID OFS: readOfs=%d/%d\n", readOfs, size );
		return( 0 );
	}

	// 読み込み位置調整
	ofs += readOfs;

	// データの読み込み
	if( pPack->isLoadAll ){
		pBuf = &(((BYTE* )pPack->pBufFile)[ofs]);
		if( size < (readOfs+readSize) ){ readSize = size-readOfs; }

		// データのコピー
		memcpy( pReadBuff, pBuf, readSize );
		readed = readSize;
	}else{
		// ファイルが開いていなければ失敗
		if( !CFileMgr::CheckFileDescriptor( &pPack->fd ) ){ return( 0 ); }

		// データの読み込み
		CFileMgr::Seek( &pPack->fd, ofs );
		if( !CFileMgr::ReadData( &pPack->fd, pReadBuff, readSize, &readed ) ){ readed = 0; }
	}

	return( readed );
}

//------------------------------------------------------------------------
// ファイル名取得
//（※パック時の指定により保持されない場合もあり、その際は失敗文字が返る）
//------------------------------------------------------------------------
static const char* _fpack_get_name_failed = "";
const char* fpack_get_name( stFPACK* pPack, int id ){
#ifdef VALID_CHECK_FOR_GET
	// 有効性確認
	if( !fpack_is_valid( pPack ) ||  pPack->cArrPathData == NULL ){
		return( _fpack_get_name_failed );
	}
#endif

	// idが有効なら文字列を返す
	if( id >= 0 && id < pPack->header.numFile ){
		return( pPack->pArrPath[id] );
	}

	return( _fpack_get_name_failed );
}

//-------------------------------------------------------------
// ファイル名からIDの取得（※失敗時は[-1]が返る）
//-------------------------------------------------------------
int fpack_search_file( stFPACK* pPack, const char* name, bool isExtIgnore ){
#ifdef VALID_CHECK_FOR_GET
	// 有効性確認
	if( !fpack_is_valid( pPack ) ||  pPack->cArrPathData == NULL ){
		return( -1 );
	}
#endif

	if( isExtIgnore ){
		// ファイルボディの比較
		int len = 0;
		while( name[len] != '.' && name[len] != '\0' ){ len++;}
		for( int i=0; i<pPack->header.numFile; i++ ){
			if( strncmp( name, pPack->pArrPath[i], (size_t)len ) == 0 ){ return( i ); }
		}
	}else{
		// ファイル名の完全比較
		for( int i=0; i<pPack->header.numFile; i++ ){
			if( util_str_cmp( name, pPack->pArrPath[i] ) == 0 ){ return( i ); }
		}
	}

	return( -1 );
}

//-----------------------
// デバッグ用
//-----------------------
void fpack_dump( stFPACK* pPack ){
	int i;
	stFPACK_HEADER* pHead;

	// 無効は無視
	if( !fpack_is_valid( pPack ) ){
		LOGD( "@ fpack_dump: INVALID DATA: pPack=%p\n", pPack );
		return;
	}

	// 情報の出力
	LOGD( "@ fpack_dump(%s)\n", pPack->fileName );

	// ヘッダ情報
	pHead = &pPack->header;
	LOGD( "@  numFile = %d\n", pHead->numFile );

	if( pPack->pArrPath != NULL ){
		for( i=0; i<pHead->numFile; i++ ){
			LOGD( "@  fileInfo[%d]: ofs=%d size=%d name=%s\n", i, pPack->stArrCell[i].offset, pPack->stArrCell[i].size, pPack->pArrPath[i] );
		}
	}else{
		for( i=0; i<pHead->numFile; i++ ){
			LOGD( "@  fileInfo[%d]: ofs=%d size=%d\n", i, pPack->stArrCell[i].offset, pPack->stArrCell[i].size );
		}
	}
}
