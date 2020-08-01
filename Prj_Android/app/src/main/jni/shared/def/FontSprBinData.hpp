/*+----------------------------------------------------------------+
  |	Title:		FontSprBinData.hpp [共通環境]
  |	Comment:	フォントスプライトバイナリデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FONT_SPR_BIN_DATA_HPP__
#define __FONT_SPR_BIN_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/DrawConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define FONT_SPR_LABEL_LEN      32

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フォントスプライトバイナリデータ
//----------------------------------------
class CFontSprBinData{
public:
    //------------------------
    // バイナリサイズ取得
    //------------------------
    inline static int GetBinSize( void ){
        return( FONT_SPR_LABEL_LEN + 8*2 + 4 );
    }

protected:
    //------------------------
    // メンバー
    //------------------------
    char m_cArrLabel[FONT_SPR_LABEL_LEN];   // [32]: ラベル
    int16 m_nNo;                            // [2]: 画像番号
    int16 m_nU0;                            // [2]: U0
    int16 m_nV0;                            // [2]: V0
    int16 m_nU1;                            // [2]: U1
    int16 m_nV1;                            // [2]: V1
    int16 m_nOfsX;                          // [2]: オフセットX
    int16 m_nOfsY;                          // [2]: オフセットY
    int16 m_nAdvX;                          // [2]: 次の文字の開始位置調整
    uint32 m_nCode;                         // [4]: ユニコード値

public:
    //------------------------
    // コンストラクタ／デストラクタ
    //------------------------
    CFontSprBinData(){}
    virtual ~CFontSprBinData(){}

    //------------------------
    // 取得
    //------------------------
    inline const char* getLabel( void ){ return( m_cArrLabel ); }
    inline int getNo( void ){ return( m_nNo ); }
    inline int getU0( void ){ return( m_nU0 ); }
    inline int getV0( void ){ return( m_nV0 ); }
    inline int getU1( void ){ return( m_nU1 ); }
    inline int getV1( void ){ return( m_nV1 ); }
    inline int getOfsX( void ){ return( m_nOfsX ); }
    inline int getOfsY( void ){ return( m_nOfsY ); }
    inline int getAdvX( void ){ return( m_nAdvX ); }
    inline DWORD getCode( void ){ return( m_nCode ); }

    //-----------------------------------------------------
    // 読み込み（※[pIB]は有効である前提＆値の整合性等の判断はしない）
    //-----------------------------------------------------
    inline void read( CInputBuffer* pIB ){
        pIB->readBlock( m_cArrLabel, FONT_SPR_LABEL_LEN );
        m_nNo = pIB->readInt16();
        m_nU0 = pIB->readInt16();
        m_nV0 = pIB->readInt16();
        m_nU1 = pIB->readInt16();
        m_nV1 = pIB->readInt16();
        m_nOfsX = pIB->readInt16();
        m_nOfsY = pIB->readInt16();
        m_nAdvX = pIB->readInt16();
        m_nCode = pIB->readUint32();
    }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __FONT_SPR_BIN_DATA_HPP__
