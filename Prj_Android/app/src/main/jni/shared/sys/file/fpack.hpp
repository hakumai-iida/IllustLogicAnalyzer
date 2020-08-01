/*+----------------------------------------------------------------+
  |	Title:		fpack.hpp [共通環境]
  |	Comment:	ファイルパック(※[fpack.exe]の実装に依存)
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef	__FPACK_HPP__
#define	__FPACK_HPP__

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
//--------------------------------------------------------
// ファイル情報
//--------------------------------------------------------
struct stFPACK_CELL{
	DWORD	offset;		// ファイルデータへのオフセット
	DWORD	size;		// ファイルデータサイズ
};

//--------------------------------------------------------
// ファイルヘッダ
//--------------------------------------------------------
struct stFPACK_HEADER{
	DWORD	numFile;		// ファイル数
	DWORD	ofsFilePath;	// ファイルパス情報へのオフセット
	DWORD	sizeFilePath;	// ファイルパス情報のサイズ
};

//--------------------------------------------------------
// 管理構造体
//--------------------------------------------------------
struct stFPACK{
	stFPACK_HEADER	header;			// ヘッダー
	stFPACK_CELL*	stArrCell;		// ファイル情報配列[メモリを確保する]
	char*			cArrPathData;	// ファイルパスデータ
	char**			pArrPath;		// ファイルパス
	char			fileName[COMMON_NAME_LEN];	// ファイル名

	// データ情報
	stFILE_DESCRIPTOR	fd;			// ファイルディスクリプタ（※その都度シークアクセス）

	// 作業バッファ
	void*		pBufRead;	// 読み込みバッファ（※圧縮ファイル領域）
	DWORD		sizeRead;	// 読み込みバッファサイズ

	void*		pBufData;	// データバッファ（※解凍領域）
	DWORD		sizeData;	// データバッファサイズ

	// 常駐処理用
	bool		isLoadAll;
	void*		pBufFile;	// ファイルバッファ

	// ファイル読み込みの際に利用する共有メモリ
	eMEM_FIELD_SHARE	useShareId;
};

/*+----------------------------------------------------------------+
  |	Prototype	関数定義
  +----------------------------------------------------------------+*/
// 基本関連
extern void fpack_init( stFPACK* pPack );		// 初期化
extern void fpack_release( stFPACK* pPack );	// 解放
extern bool fpack_is_valid( stFPACK* pPack );	// 判定

// 作業バッファ設定
extern void fpack_set_read_buf( stFPACK* pPack, void* pBuf, DWORD size );
extern void fpack_set_data_buf( stFPACK* pPack, void* pBuf, DWORD size );
extern void fpack_set_share_id( stFPACK* pPack, eMEM_FIELD_SHARE useShareId );

//-----------------------------------------------------------------------------------------
// 設定：ファイルとの関連付け（※ファイル情報用にメモリ領域を確保する）
//-----------------------------------------------------------------------------------------
// [isLoadAll==true]の場合：ファイルの全内容を読み込む
//  ⇒ 個々のデータをファイルアクセス無しに取得ができる（※全ファイル利用時）
//
// [isLoadAll==false]の場合： ヘッダ部分のみ常駐させる
//  ⇒ 個々のデータの取得にファイルアクセスが発生する（※一部ファイル利用時）
//
// [noClose==true]の場合: [set]後、ファイルを開いたままにする
//  ⇒ その後のファイル取得時に[fpack_file_opne]が不要
//
// [noClose==false]の場合: [set]後、ファイルを閉じる
//  ⇒ その後のファイル取得時に[fpack_file_open]が必要
//
// 必要な読み込み処理が終了したら[fpack_file_close]によりファイルを閉じること
// パックアクセスが不要になったら[fpack_release]により保持しているメモリ領域を解放すること
//-----------------------------------------------------------------------------------------
// 現状、デフォルト設定で[isLoadAll==true]を指定した場合、
// ファイルの読み込み先は汎用領域[CMemMgr::eMEM_FIELD_SHARE_FPACK]となるため、
// 以降の処理でデータが破壊されない保証がない
// 常駐化して利用したい場合は専用の領域を確保し、[fpack_set_share_id]で指定する必要がある
//-----------------------------------------------------------------------------------------
extern bool fpack_set( stFPACK* pPack, const char* file_name, bool isLoadAll=false, bool noClose=false );

// リセット（※ファイルポインタの開き直し）
extern bool fpack_reset( stFPACK* pPack );

// ファイルオープン／クローズ
extern bool fpack_open_file( stFPACK* pPack );
extern void fpack_close_file( stFPACK* pPack );

// 要素数取得
extern int fpack_get_num( stFPACK* pPack );

// 情報取得
extern bool fpack_get_info( stFPACK* pPack, int id, DWORD* pOfs, DWORD* pSize );

// データ取得（※事前に対象となるデータファイルを開いておく必要がある）
extern void* fpack_get_data( stFPACK* pPack, int id, DWORD* pSize, bool isUnpack );

// ストリーミング用データ取得：返値は実際にロードしたサイズ（※事前に対象となるデータファイルを開いておく必要がある）
extern DWORD fpack_streaming( stFPACK* pPack, int id, DWORD readOfs, DWORD readSize, void* pReadBuff );

// ファイル名取得（※パック時の指定により保持されない場合もあり、その際は失敗文字（空文字）が返る）
extern const char* fpack_get_name( stFPACK* pPack, int id );

// ファイル名からデータIDの検索（※失敗時は[-1]が返る）
extern int fpack_search_file( stFPACK* pPack, const char* name, bool isExtIgnore );

// デバッグダンプ
extern void fpack_dump( stFPACK* pPack );

#endif	// __FPACK_HPP__
