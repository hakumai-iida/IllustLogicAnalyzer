/*+----------------------------------------------------------------+
  |	Title:		FileHeader.hpp [共通環境]
  |	Comment:	ファイルヘッダ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FILE_HEADER_HPP__
#define __FILE_HEADER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DataCell.hpp"
#include "InputBuffer.hpp"
#include "OutputBuffer.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define FILE_HEADER_VERSION_LEN      16
#define FILE_HEADER_FILEKEY_LEN      40

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ファイルヘッダ
//----------------------------------------
class CFileHeader{
	IMPLEMENT_DATA( CFileHeader )                    // 実装：データ

protected:
	char m_cArrVersion[FILE_HEADER_VERSION_LEN];	// バージョン（文字列）
    char m_cArrFilekey[FILE_HEADER_FILEKEY_LEN];	// ファイルキー（文字列）
	int m_nDataSize;    // 個々のデータサイズ（※全てのデータのサイズが等しい場合に有効値[>0]となり、不一致で[-1]、データ数が０の場合[-2]となる）
	int m_nDataNum;		// データ数（※出力データ数）

public:
    // コンストラクタ／デストラクタ
	CFileHeader( void );
	virtual ~CFileHeader( void ){}

	// 設定
	inline void setVersion( const char* pVersion ){ util_str_cpy( m_cArrVersion, pVersion, FILE_HEADER_VERSION_LEN ); }
	inline void setFilekey( const char* pFilekey ){ util_str_cpy( m_cArrFilekey, pFilekey, FILE_HEADER_FILEKEY_LEN ); }
	inline void setDataSize( int size ){ m_nDataSize = size; }
	inline void setDataNum( int num ){ m_nDataNum = num; }

    // 取得
    inline const char* getVersion( void ){ return( m_cArrVersion ); }
    inline const char* getFilekey( void ){ return( m_cArrFilekey ); }
    inline int getDataSize( void ){ return( m_nDataSize ); }
    inline int getDataNum( void ){ return( m_nDataNum  ); }

    // サイズ判定用
    inline int getFileHeaderSize( void ){ return( FILE_HEADER_VERSION_LEN + FILE_HEADER_FILEKEY_LEN + 2*4 ); }
    inline int getFileBodySize( void ){ return( m_nDataSize * m_nDataNum ); }
    inline int getFileSize( void ){ return( getFileHeaderSize() + getFileBodySize() ); }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __FILE_HEADER_HPP__ */
