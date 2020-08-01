/*+----------------------------------------------------------------+
  |	Title:		FileHeader.cpp [共通環境]
  |	Comment:	ファイルヘッダ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FileHeader.hpp"

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
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------
// コンストラクタ
//-------------------
CFileHeader::CFileHeader( void ){
	clear();
}

//-------------------------
// クリア（※データの破棄）
//-------------------------
void CFileHeader::clear( void ){
	memset( m_cArrVersion, 0, FILE_HEADER_VERSION_LEN );
	memset( m_cArrFilekey, 0, FILE_HEADER_FILEKEY_LEN );
	m_nDataSize = 0;
	m_nDataNum = 0;
}

//--------------------
// コピー
//--------------------
void CFileHeader::copy( CFileHeader* pData ){
	// 無効は無視
	if( pData == NULL ){
		return;
	}

    setVersion( pData->m_cArrVersion );
    setFilekey( pData->m_cArrFilekey );
    m_nDataSize = pData->m_nDataSize;
    m_nDataNum = pData->m_nDataNum;
}

//---------------------------
// 読み込み
//---------------------------
void CFileHeader::read( CInputBuffer* pIB ){
    pIB->readBlock( m_cArrVersion, FILE_HEADER_VERSION_LEN );
    pIB->readBlock( m_cArrFilekey, FILE_HEADER_FILEKEY_LEN );
    m_nDataSize = pIB->readInt32();
    m_nDataNum = pIB->readInt32();

#if 0
    LOGD( "@  m_cArrVersion=%s\n", m_cArrVersion );
    LOGD( "@  m_cArrFilekey=%s\n", m_cArrFilekey );
    LOGD( "@  m_nDataSize=%d(0x%08X)\n", m_nDataSize, m_nDataSize );
    LOGD( "@  m_nDataNum=%d(0x%08X)\n", m_nDataNum, m_nDataNum );
#endif
}

//---------------------------
// 書き込み
//---------------------------
void CFileHeader::write( COutputBuffer* pOB ){
    pOB->writeBlock( m_cArrVersion, FILE_HEADER_VERSION_LEN );
    pOB->writeBlock( m_cArrFilekey, FILE_HEADER_FILEKEY_LEN );
    pOB->writeInt32( m_nDataSize );
    pOB->writeInt32( m_nDataNum );
}
