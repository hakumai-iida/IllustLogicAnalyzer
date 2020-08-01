/*+----------------------------------------------------------------+
  |	Title:		InputBuffer.cpp [共通環境]
  |	Comment:	入力バッファ
  |				[COutputBuffer]と対になる
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "InputBuffer.hpp"

#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//#define CHECK_ARRAY_SIZE

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
CInputBuffer::CInputBuffer( void ){
	clear();
}

//------------------------
// クリア
//------------------------
void CInputBuffer::clear( void ){
	m_pBuf = NULL;
	m_nSize = 0;
	m_nAt = 0;
	m_bAdjustEndian = false;
}

//-------------------
// 設定
//-------------------
void CInputBuffer::setBuffer( void* pBuf, int size ){
	clear();

	m_pBuf = (BYTE*) pBuf;
	m_nSize = size;
}

//--------------------------------------
// リトルエンディアン指定
//--------------------------------------
void CInputBuffer::setForLittleEndian( void ){
	// 環境がビッグエンディアンなら調整
	m_bAdjustEndian = util_endian_is_big();
}

//--------------------------------------
// ビッグエンディアン指定
//--------------------------------------
void CInputBuffer::setForBigEndian( void ){
	// 環境がリトルエンディアンなら調整
	m_bAdjustEndian = util_endian_is_little();
}

//-------------------
// 読み込み位置調整
//-------------------
bool CInputBuffer::seek( int at ){
	if( at < 0 || at >= m_nSize ){
		return( false );
	}

	m_nAt = at;
	return( true );
}

//-------------------
// 読み飛ばし
//-------------------
bool CInputBuffer::skip( int size ){
	if( size < 0 || (m_nAt+size) > m_nSize ){
		return( false );
	}

	m_nAt += size;
	return( true );
}

//-------------------
// 読み込み：int8
//-------------------
int8 CInputBuffer::readInt8( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int8))){
		LOGE( "@ CInputBuffer::readInt8: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	int8 v = *((int8*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof( int8 );

	return( v );
}

//-------------------
// 読み込み：uint8
//-------------------
uint8 CInputBuffer::readUint8( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint8))){
		LOGE( "@ CInputBuffer::readUint8: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	uint8 v = *((uint8*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(uint8);

	return( v );
}

//-------------------
// 読み込み：int16
//-------------------
int16 CInputBuffer::readInt16( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int16))){
		LOGE( "@ CInputBuffer::readInt16: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	int16 v = *((int16*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(int16);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(int16) );
	}

	return( v );
}

//-------------------
// 読み込み：uint16
//-------------------
uint16 CInputBuffer::readUint16( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint16))){
		LOGE( "@ CInputBuffer::readUint16: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	uint16 v = *((uint16*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(uint16);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(uint16) );
	}

	return( v );
}

//-------------------
// 読み込み：int32
//-------------------
int32 CInputBuffer::readInt32( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int32))){
		LOGE( "@ CInputBuffer::readInt32: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	int32 v = *((int32*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(int32);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(int32) );
	}

	return( v );
}

//-------------------
// 読み込み：uint32
//-------------------
uint32 CInputBuffer::readUint32( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint32))){
		LOGE( "@ CInputBuffer::readUint32: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	uint32 v = *((uint32*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(uint32);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(uint32) );
	}

	return( v );
}

//-------------------
// 読み込み：int64
//-------------------
int64 CInputBuffer::readInt64( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(int64))){
		LOGE( "@ CInputBuffer::readInt64: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	int64 v = *((int64*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(int64);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(int64) );
	}

	return( v );
}

//-------------------
// 読み込み：uint64
//-------------------
uint64 CInputBuffer::readUint64( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(uint64))){
		LOGE( "@ CInputBuffer::readUint64: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	uint64 v = *((uint64*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(uint64);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(uint64) );
	}

	return( v );
}

//-------------------
// 読み込み：float
//-------------------
float CInputBuffer::readFloat( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(float))){
		LOGE( "@ CInputBuffer::readFloat: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	float v = *((float*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(float);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(float) );
	}

	return( v );
}

//-------------------
// 読み込み：double
//-------------------
double CInputBuffer::readDouble( void ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (int)(m_nSize-sizeof(double))){
		LOGE( "@ CInputBuffer::readDouble: SIZE ERROR: at=%d, size=%d\n", m_nAt, m_nSize );
		return( 0 );
	}
#endif

	double v = *((double*)(&m_pBuf[m_nAt]));
	m_nAt += sizeof(double);

	if( m_bAdjustEndian ){
		util_endian_adjust( &v, sizeof(double) );
	}

	return( v );
}

//----------------------
// 読み込み：ブロック
//----------------------
bool CInputBuffer::readBlock( void* pDest, int len ){
#ifdef CHECK_ARRAY_SIZE
	if( m_nAt < 0 || m_nAt > (m_nSize-len)){
		LOGE( "@ CInputBuffer::readBlock: SIZE ERROR: at=%d, size=%d, len=%d\n", m_nAt, m_nSize, len );
		memset( pDest, 0, len );
		return( false );
	}
#endif

	memcpy( pDest, &m_pBuf[m_nAt], len );
	m_nAt += len;

	return( true );
}
