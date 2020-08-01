/*+----------------------------------------------------------------+
  |	Title:		sdarc.cpp [共通環境]
  |	Comment:	シンプルなデータ圧縮処理(※[simplearc.exe]の実装に依存)
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sdarc.hpp"
#include "sdarc_unpak3.hpp"
#include "sdarc_pak3.hpp"

#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// 解凍実体
static uint32 unpack( BYTE* inBuf, uint32 in_size, BYTE* outBuf, uint32 out_size );

// 圧縮実体
static uint32 pack( BYTE* inpBuf, uint32 in_size, BYTE* outBuf, uint32 out_size );

/*+----------------------------------------------------------------+
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//------------------------------------------------------------------
// 有効性と解凍に必要なバッファサイズの確認
//------------------------------------------------------------------
bool sdarc_is_valid( void* inp_buf, uint32 inp_size, uint32* pReqBufSize ){
	uint32 size;
	stSDARC_HEADER header;

	// ヘッダの取得に失敗したら無効
	if( !sdarc_get_header( &header, inp_buf, inp_size ) ){
		LOGE( "@ sdarc_is_valid FAILED" );
		return( false );
	}

	// サイズポインタが有効であれば作業バッファの指定
	if( pReqBufSize != NULL ){
		// オプションには対応していない
		if( header.option == eSDARC_ADJ_TYPE_INVALID ){
			size  = (header.arrSize[0]+3)/4*4;
		}else{
			LOGE( "@sdarc_unpack: NOT SUPPORTED: option=%d\n", header.option );
			return( false );
		}
		*pReqBufSize = size;
	}

	return( true );
}

//---------------------------------------------------------------------------
// ヘッダ取得
//---------------------------------------------------------------------------
bool sdarc_get_header( stSDARC_HEADER* pHeader, void* pBuf, uint32 size ){
	// ヘッダサイズ未満のデータは無効
	if( size < sizeof(stSDARC_HEADER) ){
		return( false );
	}

	// 入力バッファ（※リトルエンディアン）
	CInputBuffer ib;
	ib.setBuffer( pBuf, size );
	ib.setForLittleEndian();

	// ヘッダ読み込み
	for( int i=0; i<4; i++ ){ pHeader->signature[i] = ib.readUint8(); }
	pHeader->option = ib.readUint16();
	for( int i=0; i<SDARC_DATA_BLOCK_NUM; i++ ){ pHeader->arrType[i] = ib.readUint8(); }
	for( int i=0; i<SDARC_DATA_BLOCK_NUM; i++ ){ pHeader->arrSize[i] = ib.readUint32(); }
	for( int i=0; i<SDARC_DATA_BLOCK_NUM; i++ ){ pHeader->arrOfs[i] = ib.readUint32(); }

	// 署名の確認
	if( pHeader->signature[0] != 0x89 ||
        pHeader->signature[1] != 'S'  ||
        pHeader->signature[2] != 'A'  ||
        pHeader->signature[3] != 'P'  ){
		return( false );
	}

	// ここまで来たら有効な[sdarc pack]ファイル
	return( true );
}

//------------------------------------------------------------------
// 解凍処理
//------------------------------------------------------------------
uint32 sdarc_unpack( void* out_buf, uint32 buf_size, void* inp_buf, uint32 inp_size ){
	return( unpack( (BYTE*)inp_buf, (int)inp_size, (BYTE*)out_buf, (int)buf_size ) );
}

//---------------------------------------------------------------------------
// 解凍実体
//---------------------------------------------------------------------------
static uint32 unpack( BYTE* inBuf, uint32 in_size, BYTE* outBuf, uint32 out_size ){
	stSDARC_HEADER header;

	// ヘッダ確認
	if( !sdarc_get_header( &header, inBuf, (uint32)in_size ) ){
		LOGE( "@sdarc_unpack: INVALID_HEADER\n" );
		return( 0 );
	}

	// 圧縮オプションには非対応
	if( header.option != eSDARC_ADJ_TYPE_INVALID ){
		LOGE( "@sdarc_unpack: UN-SUPPORTED: option=%d\n", header.option );
		return( 0 );
	}

	// 解凍後のサイズがバッファに収まらない場合はエラー
	if( header.arrSize[0] > out_size ){
		LOGE( "[unpack] buffer overflow [%d/%d]\n", header.arrSize[0], out_size );
		return( 0 );
	}

	//----------------------------------------------
	// 解凍
	//----------------------------------------------
	int type = header.arrType[0];
	int sizeSrc = in_size - header.arrOfs[0];
	BYTE* bufSrc = &inBuf[header.arrOfs[0]];

	// [TYPE_3] 解凍処理
	if( type == eSDARC_TYPE_3 ){
		sdarc_unpak3( outBuf, bufSrc, sizeSrc );
	}
	// [TYPE_255] 未圧縮
	else if( type == eSDARC_TYPE_INVALID ){
		memcpy( outBuf, bufSrc, sizeSrc );
	}
	// その他の形式は非対応
	else{
		LOGE( "[unpack] UN-SUPPORTED type=%d\n", type );
		return( 0 );
	}

	// ヘッダに保存されているサイズを返す
	return( header.arrSize[0] );
}

//---------------------------------------------------------------------------
// 圧縮処理
//---------------------------------------------------------------------------
uint32 sdarc_pack( void* out_buf, uint32 buf_size, void* inp_buf, uint32 inp_size ){
	return( pack( (BYTE*)inp_buf, (int)inp_size, (BYTE*)out_buf, (int)buf_size ) );
}

//---------------------------------------------------------------------------
// 圧縮実体
//---------------------------------------------------------------------------
static uint32 pack( BYTE* inBuf, uint32 in_size, BYTE* outBuf, uint32 out_size ){
	// 出力バッファ（※リトルエンディアン）
	COutputBuffer ob;
	ob.setBuffer( outBuf, out_size );
	ob.setForLittleEndian();

	// データオフセット（※バイナリデータ上の圧縮データ開始位置）
	uint32 ofs = 16;

	// ヘッダ出力
	ob.writeUint8( 0x89 );						// BYTE  signature[0]
	ob.writeUint8( (BYTE)'S' );				    // BYTE  signature[1]
	ob.writeUint8( (BYTE)'A' );				    // BYTE  signature[2]
	ob.writeUint8( (BYTE)'P' );				    // BYTE  signature[3]
	ob.writeUint16( eSDARC_ADJ_TYPE_INVALID );	// WORD  option
	ob.writeUint8( eSDARC_TYPE_3 );			    // BYTE  arrType[0]（※[TYPE3]固定）
	ob.writeUint8( eSDARC_TYPE_INVALID );	    // BYTE  arrType[1]
	ob.writeUint32( in_size );				    // DWORD arrSize[0]
	ob.writeUint32( 0 );					    // DWORD arrSize[1]
	ob.writeUint32( ofs );					    // DWORD arrOfs[0]
	ob.writeUint32( 0 );					    // DWORD arrOfs[1]

	// 環境設定
	DWORD workSize = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_WORK );
	void* pWork = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_WORK );
	sdarc_pak3_set_env( workSize, pWork );

    // ヘッダに続けて圧縮データの出力（※内部でエンディアン調整される）
	DWORD size = sdarc_pak3( inBuf, in_size, &outBuf[ofs] );

    // [ヘッダ＋ボディ]がファイルサイズ
	return( ofs + size );
}
