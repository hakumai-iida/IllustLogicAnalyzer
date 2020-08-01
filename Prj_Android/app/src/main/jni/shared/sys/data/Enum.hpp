/*+----------------------------------------------------------------+
  |	Title:		Enum.hpp [共通環境]
  |	Comment:	列挙子
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ENUM_HPP__
#define __ENUM_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "InputBuffer.hpp"
#include "OutputBuffer.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 無効値と無効ラベル
#define ENUM_INVALID_VAL        (-1)
#define ENUM_INVALID_LABEL      "INVALID"
#define ENUM_INVALID_LABEL_S    "*"

#define ENUM_NOT_FOUND_VAL      (-2)
#define ENUM_NOT_FOUND_LABEL    "NOT_FOUND"
#define ENUM_NOT_FOUND_LABEL_S  "?"

// 個別ラベル取得マクロ
#define ENUM_RETURN_LABEL_AT( _arrLabel, _at, _max )    \
    if( (_at) >= 0 && (_at) < (_max) ){                 \
        return( _arrLabel[(_at)] );                     \
    }                                                   \
    char* buf = CMemMgr::GetBufFromTempStr();           \
    sprintf( buf, "NOT_FOUND:%d", (_at) );              \
    return( buf )

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------
// 列挙子
//-----------------
class CEnum{
public:
    // ラベル→値変換
    static int CheckEnumLabel( const char* pLabel, const char** ppArrLabel,
                               const char* pInvalidLabel=ENUM_INVALID_LABEL, int invalidVal=ENUM_INVALID_VAL,
                               int notFoundVal=ENUM_NOT_FOUND_VAL );
    
    // 値→ラベル変換
    static const char* CheckEnumVal( int val, int valMax, const char** ppArrLabel,
                                     const char* pInvalidLabel=ENUM_INVALID_LABEL, int invalidVal=ENUM_INVALID_VAL,
                                     const char* pNotFoundLabel=ENUM_NOT_FOUND_LABEL );

    
    // 列挙子の読み込み
    static int ReadEnum( CInputBuffer* pIB, const char** ppArrLabel,
                         const char* pInvalidLabel=ENUM_INVALID_LABEL, int invalidVal=ENUM_INVALID_VAL,
                         int notFoundVal=ENUM_NOT_FOUND_VAL );


    // 列挙子の書き込み
    static int WriteEnum( COutputBuffer* pOB, int val, int valMax, const char** ppArrLabel,
                          const char* pInvalidLabel=ENUM_INVALID_LABEL, int invalidVal=ENUM_INVALID_VAL,
                          const char* pNotFoundLabel=ENUM_NOT_FOUND_LABEL );

private:
	// インスタンス生成は不可
	CEnum( void ){}
	virtual ~CEnum( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __ENUM_HPP__ */
