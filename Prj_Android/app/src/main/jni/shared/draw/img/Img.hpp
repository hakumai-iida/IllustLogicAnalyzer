/*+----------------------------------------------------------------+
  |	Title:		Img.hpp [共通環境]
  |	Comment:	画像
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __IMG_HPP__
#define __IMG_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ImgFormat.hpp"
#include "draw/tex/Tex.hpp"
#include "def/ImgBinData.hpp"
#include "sys/file/fpack.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define IMG_FILE_NAME_LEN		32

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 画像
//----------------------------------------
class CImg{
protected:
	// パレット
	CTex**  m_pArrPal;
	int m_nNumPal;

	// 画素
	CTex** m_pArrDot;
	int* m_arrDotDelay;
	int m_nNumDot;

	// テクスチャ（※実体）
	CTex* m_oArrTex;
	int m_nNumTex;

	// 管理用
	char m_bufFileName[IMG_FILE_NAME_LEN];
	int m_nVramSize;

public:
    // コンストラクタ／デストラクタ
	CImg( void );
	virtual ~CImg( void );

    // クリア
	void clear( void );
    
	// パレット取得
	CTex* getPal( int at=0 );
	inline int getNumPal( void ){ return( m_nNumPal); }

	// 画素取得
	CTex* getDot( int at=0 );
	inline int getNumDot( void ){ return( m_nNumDot); }

	// 時間取得
	float getDotDelay( int at=0 );
	float getDotDelayTotal( void );
	int   getDotAtByDelay( float time, bool isLoop=true );

	// フレーム取得
	int getDotDelayAsFrame( int at=0 );
	int getDotDelayTotalAsFrame( void );
	int getDotAtByDelayAsFrame( int frame, bool isLoop=true );
    
    // バイナリ適用
    bool applyBinWithFpack( CImgBinData* pBin, stFPACK* pPack );

	// 設定
	bool setFromFile( const char* pFileName, DWORD opt );					// ファイル
	bool setFromFpack( stFPACK* pPack, const char* pFileName, DWORD opt );	// ファイルパック（※名前指定）
	bool setFromFpack( stFPACK* pPack, int no, DWORD opt );					// ファイルパック（※番号指定）
	bool setFromBuf( BYTE* pBuf, uint32 sizeBuf, DWORD opt );					// バッファ経由（※実体）

protected:
	bool setTexWithHifBlockInfo( CTex* pTex, stHIF_BLOCK_INFO* pInfo, DWORD opt );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __IMG_HPP__
