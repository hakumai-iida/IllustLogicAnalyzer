/*+----------------------------------------------------------------+
  |	Title:		Enum.cpp [共通環境]
  |	Comment:	列挙子
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Enum.hpp"

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
//------------------------
// ラベル→値変換
//------------------------
int CEnum::CheckEnumLabel( const char* pLabel, const char** ppArrLabel, const char* pInvalidLabel, int invalidVal, int notFoundVal ){
    //-----------------------
    // 無効値の判定
    //-----------------------
    int i=0;
    while( pLabel[i] == pInvalidLabel[i] && pLabel[i] != '\0' ){
        i++;
    }

    // 文字列が完全に一致していたら検出
    if( i > 0 && pLabel[i] == '\0' && pInvalidLabel[i] == '\0' ){
        return( invalidVal );
    }

    //-----------------------
    // 有効値の判定
    //-----------------------
    int at=0;
    while( ppArrLabel[at] != NULL ){
        i=0;
        while( pLabel[i] == ppArrLabel[at][i] && pLabel[i] != '\0' ){
            i++;
        }
        
        // 文字列が完全に一致していたら検出
        if( i > 0 && pLabel[i] == '\0' && ppArrLabel[at][i] == '\0' ){
            return( at );
        }
        
        at++;
    }
    
    // ここまできたら見つからなかった値を返す
    LOGE( "@CEnum::CheckEnumLabel: NOT FOUND: pLabel=%s\n", pLabel );
    
    return( notFoundVal );
}

//------------------------
// 値→ラベル変換
//------------------------
const char* CEnum::CheckEnumVal( int val, int valMax, const char** ppArrLabel,
                                 const char* pInvalidLabel, int invalidVal, const char* pNotFoundLabel ){
    // 無効値の確認
    if( val == invalidVal ){
        return( pInvalidLabel );
    }

    // 値が収まっていればラベルを返す
    if( val >= 0 && val < valMax ){
        return( ppArrLabel[val] );
    }
    
    // ここまできたら見つからなかったラベルを返す
    LOGE( "@CEnum::CheckEnumVal: NOT FOUND: val=%d/%d\n", val, valMax );
    return( pNotFoundLabel );
}

//------------------------
// 列挙子の読み込み
//------------------------
int CEnum::ReadEnum( CInputBuffer* pIB, const char** ppArrLabel,
                     const char* pInvalidLabel, int invalidVal, int notFoundVal ){

    // 文字列長の読み込み
    int len = pIB->readUint8();
    
    // 無効値であれば無効値を返しておく
    if( len <= 0 ){
        return( invalidVal );
    }
    
    // 文字列の読み込み
    char buf[256];
    pIB->readBlock( buf, len );
    buf[len] = '\0';
    
    // 文字列から値を検出して返す
    return( CheckEnumLabel( buf, ppArrLabel, pInvalidLabel, invalidVal, notFoundVal ) );
}

//------------------------
// 列挙子の書き込み
//------------------------
int CEnum::WriteEnum( COutputBuffer* pOB, int val, int valMax, const char** ppArrLabel,
                      const char* pInvalidLabel, int invalidVal, const char* pNotFoundLabel ){
    // 無効値であれば無効出力
    if( val == invalidVal ){
        pOB->writeUint8( 0 );
        return( 0 );
    }

    // 列挙子の受け取り
    const char* pLabel = CheckEnumVal( val, valMax, ppArrLabel, pInvalidLabel, invalidVal, pNotFoundLabel );
    
    // 文字列長の出力
    int len = (int)strlen( pLabel );
    pOB->writeUint8( (uint8)len );
    
    // 文字列の出力
    pOB->writeBlock( (void*)pLabel, len );

    // とりあえず出力した文字列長を返しておく
    return( len );
}

