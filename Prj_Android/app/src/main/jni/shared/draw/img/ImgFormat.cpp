/*+----------------------------------------------------------------+
  |	Title:		ImgFormat.cpp [共通環境]
  |	Comment:	画像フォーマット
  |				[bmp2hif.exe/img2hif.exe]の実装に依存
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ImgFormat.hpp"

#include "sys/util/util_endian.hpp"

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
  |	Prototype	プロト宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------
// [HIF] ヘッダ確認＆読み込み
//----------------------------
bool CImgFormat::CheckHifHeaderThenSet( stHIF_HEADER* pHeader, BYTE* pBuf ){
	// 入力バッファ（※リトルエンディアン）
	CInputBuffer ib;
	ib.setBuffer( pBuf, 8 );	// 読み込みサイズは８バイト
	ib.setForLittleEndian();

	// シグネチャ読み込み
	char s0 = (char) ib.readUint8();
	char s1 = (char) ib.readUint8();
	char s2 = (char) ib.readUint8();
	char s3 = (char) ib.readUint8();

	// シグネチャが無効なら失敗
	if( s0 != 'H' || s1 != 'I' || s2 != 'F' || s3 != '0' ){
		LOGE( "@ CImgFormat::CheckHifHeaderThenSet: INVALID SIGNATURE\n" );
		pHeader->isValid = false;
		return( false );
	}

	// ここまできたら有効（※フラグを立ててソースを保持）
	pHeader->isValid = true;
	pHeader->pSrc = pBuf;
	pHeader->numBlock  = ib.readUint16();
	//ib.readUint16();	// ２バイト読み捨て

	// オフセット配列をポイントしておく（※この段階ではBYTEポインタで保持するのみ）
	pHeader->pBlockOfs  = &pBuf[8];

	return( true );
}

//-----------------------
// [HIF] ブロック情報取得
//-----------------------
void CImgFormat::GetHifBlockInfoFromBuf( stHIF_BLOCK_INFO* pInfo, BYTE* pBuf ){
	// 入力バッファ（※リトルエンディアン）
	CInputBuffer ib;
	ib.setBuffer( pBuf, 24 );	// 読み込みサイズは２４バイト
	ib.setForLittleEndian();

	// 読み込み
	pInfo->type			= (eHIF_IMG_TYPE) ib.readUint8();
	ib.readUint8();	// 予約（読み捨て）
	pInfo->delay		= ib.readUint16();
	pInfo->size			= ib.readUint32();
	pInfo->width		= ib.readUint16();
	pInfo->height		= ib.readUint16();
	pInfo->ofsLeft		= ib.readUint16();
	pInfo->ofsTop		= ib.readUint16();
	pInfo->ofsRight		= ib.readUint16();
	pInfo->ofsBottom	= ib.readUint16();
	pInfo->baseX		= ib.readUint16();
	pInfo->baseY		= ib.readUint16();

	// バッファ
	pInfo->pBuf = &pBuf[24];
}

//--------------------
// [HIF] ブロック情報設定
//--------------------
void CImgFormat::SetHifBlockInfoForBuf( stHIF_BLOCK_INFO* pInfo, BYTE* pBuf ){
    // 出力バッファ（※リトルエンディアン）
    COutputBuffer ob;
    ob.setBuffer( pBuf, 24 );    // 読み込みサイズは２４バイト
    ob.setForLittleEndian();
 
    ob.writeUint8( pInfo->type );
    ob.writeUint8( 0 );  // ダミー
    ob.writeUint16( pInfo->delay );
    ob.writeUint32( pInfo->size );
    ob.writeUint16( pInfo->width );
    ob.writeUint16( pInfo->height );
    ob.writeUint16( pInfo->ofsLeft );
    ob.writeUint16( pInfo->ofsTop );
    ob.writeUint16( pInfo->ofsRight );
    ob.writeUint16( pInfo->ofsBottom );
    ob.writeUint16( pInfo->baseX );
    ob.writeUint16( pInfo->baseY );
}

//----------------------------
// [HIF] ブロック情報取り出し
//----------------------------
bool CImgFormat::GetHifBlockInfoFromHifHeaderAt( stHIF_BLOCK_INFO* pInfo, stHIF_HEADER* pHeader, int at ){
	// ヘッダが無効であれば無視
	if( ! pHeader->isValid ){
		LOGE( "@ CImgFormat::GetHifBlockInfoFromHifHeaderAt: INVALID HEADER\n" );
		return( false );
	}

	// 指定が無効であれば無視
	if( at < 0 || at >= pHeader->numBlock ){
		LOGE( "@ CImgFormat::GetHifBlockInfoFromHifHeaderAt: INVALID ID=%d/%d\n", at, pHeader->numBlock );
		return( false );
	}

	//-------------------------
	// オフセット抽出
	//-------------------------
	BYTE* pBuf = pHeader->pBlockOfs;
	DWORD ofs = *((DWORD*)&pBuf[4*at]);

    // ビッグエンディアン環境であれば調整
	if( util_endian_is_big() ){
		util_endian_adjust( &ofs, 4 );
	}

	//-------------------------
	// ブロック情報抽出
	//-------------------------
	GetHifBlockInfoFromBuf( pInfo, &pHeader->pSrc[ofs] );
	return( true );
}
