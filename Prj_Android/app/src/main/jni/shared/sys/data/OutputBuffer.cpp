/*+----------------------------------------------------------------+
  |	Title:		OutputBuffer.cpp [共通環境]
  |	Comment:	書き込みバッファ（※パディング調整済データ用）
  |				[CInputBuffer]と対になる
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "OutputBuffer.hpp"

#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//#define CHECK_ARRAY_SIZE

// 最大書き込み位置の更新
#define CHECK_MAX_AT()	if( m_nMaxAt < m_nAt ){ m_nMaxAt = m_nAt; }

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
//-------------------
// コンストラクタ
//-------------------
COutputBuffer::COutputBuffer( void ){
	clear();
}

//-----------------
// クリア
//-----------------
void COutputBuffer::clear( void ){
	m_pBuf = NULL;
	m_nSize = 0;
	m_nAt = 0;
	m_nMaxAt = 0;
	m_bAdjustEndian = false;
}

//-----------------
// 設定
//-----------------
void COutputBuffer::setBuffer( void* pBuf, int size ){
	clear();

	m_pBuf = (BYTE*) pBuf;
	m_nSize = size;
}

//--------------------------------------
// リトルエンディアン指定
//--------------------------------------
void COutputBuffer::setForLittleEndian( void ){
	// 環境がビッグエンディアンなら調整
	m_bAdjustEndian = util_endian_is_big();
}

//--------------------------------------
// ビッグエンディアン指定
//--------------------------------------
void COutputBuffer::setForBigEndian( void ){
	// 環境がリトルエンディアンなら調整
	m_bAdjustEndian = util_endian_is_little();
}

//--------------------------------------
// 書き込み位置調整
//--------------------------------------
bool COutputBuffer::seek( int at ){
	if( at < 0 || at >= m_nSize ){
		return( false );
	}

	m_nAt = at;
	return( true );
}

//-------------------
// 書き込み：int8
//-------------------
bool COutputBuffer::writeInt8( int8 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int8))){
		LOGE( "@ COutputBuffer::writeInt8: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	*((int8*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(int8);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：uint8
//-------------------
bool COutputBuffer::writeUint8( uint8 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint8))){
		LOGE( "@ COutputBuffer::writeUint8: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	*((uint8*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(uint8);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：int16
//-------------------
bool COutputBuffer::writeInt16( int16 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int16))){
		LOGE( "@ COutputBuffer::writeInt16: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(int16) );
	}

	*((int16*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(int16);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：uint16
//-------------------
bool COutputBuffer::writeUint16( uint16 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint16))){
		LOGE( "@ COutputBuffer::writeUint16: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(uint16) );
	}

	*((uint16*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(uint16);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：int32
//-------------------
bool COutputBuffer::writeInt32( int32 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int32))){
		LOGE( "@ COutputBuffer::writeInt32: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(int32) );
	}

	*((int32*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(int32);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：uint32
//-------------------
bool COutputBuffer::writeUint32( uint32 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint32))){
		LOGE( "@ COutputBuffer::writeUint32: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(uint32) );
	}

	*((uint32*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(uint32);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：int64
//-------------------
bool COutputBuffer::writeInt64( int64 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int64))){
		LOGE( "@ COutputBuffer::writeInt64: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(int64) );
	}

	*((int64*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(int64);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：uint64
//-------------------
bool COutputBuffer::writeUint64( uint64 v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint64))){
		LOGE( "@ COutputBuffer::writeUint64: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(uint64) );
	}

	*((uint64*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(uint64);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：float
//-------------------
bool COutputBuffer::writeFloat( float v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(float))){
		LOGE( "@ COutputBuffer::writeFloat: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(float) );
	}

	*((float*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(float);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：double
//-------------------
bool COutputBuffer::writeDouble( double v ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(double))){
		LOGE( "@ COutputBuffer::writeDouble: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( false );
	}
#endif

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(double) );
	}

	*((double*)(&m_pBuf[m_nAt])) = v;
	m_nAt += sizeof(double);

	CHECK_MAX_AT();
	return( true );
}

//-------------------
// 書き込み：ブロック
//-------------------
bool COutputBuffer::writeBlock( void* pDat, int len ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (m_nSize-len)){
		LOGE( "@ COutputBuffer::writeBlock: SIZE ERROR: at=%d, size=%d, len=%d\n", m_nAt, m_nSize, len );
		return( false );
	}
#endif

	memcpy( &m_pBuf[m_nAt], pDat, len );
	m_nAt += len;

	CHECK_MAX_AT();
	return( true );
}
