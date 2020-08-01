/*+----------------------------------------------------------------+
  |	Title:		OutputBuffer.hpp [共通環境]
  |	Comment:	出力バッファ（※パディング調整済データ用）
  |				[CInputBuffer]と対になる
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __OUTPUT_BUFFER_HPP__
#define __OUTPUT_BUFFER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 出力バッファ
//----------------------------------------
class COutputBuffer{
protected:
	// ワーク
	BYTE* m_pBuf;
	int m_nSize;
	int m_nAt;
	int m_nMaxAt;	// 最大書き込み位置

	// エンディアン調整フラグ
	bool m_bAdjustEndian;

public:
    // コンストラクタ／デストラクタ
	COutputBuffer( void );
	virtual ~COutputBuffer( void ){}

	// クリア
	void clear( void );

	// 設定（※内部で[clear]が呼ばれている）
	void setBuffer( void* pBuf, int size );
	
	// エンディアン指定（※バッファ指定の後に呼ぶこと）
	void setForLittleEndian( void );
	void setForBigEndian( void );

	// 書き込み位置調整
	bool seek( int at );

	// 取得
	inline int getCurPos( void ){ return( m_nAt ); }
	inline int getSize( void ){ return( m_nMaxAt ); }

	// 残りサイズ取得（※書き込み前に確認をしたい場合）
	inline int getRest( void ){ return( m_nSize - m_nAt ); }

	// 書き込み
	bool writeInt8( int8 v );
	bool writeUint8( uint8 v );
	bool writeInt16( int16 v );
	bool writeUint16( uint16 v );
	bool writeInt32( int32 v );
	bool writeUint32( uint32 v );
	bool writeInt64( int64 v );
	bool writeUint64( uint64 v );
	bool writeFloat( float v );
	bool writeDouble( double v );
	bool writeBlock( void* pDat, int len );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __OUTPUT_BUFFER_HPP__ */
