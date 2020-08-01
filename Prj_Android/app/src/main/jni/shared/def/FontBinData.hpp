/*+----------------------------------------------------------------+
  |	Title:		FontBinData.hpp [共通環境]
  |	Comment:	フォントバイナリデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FONT_BIN_DATA_HPP__
#define __FONT_BIN_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/DrawConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define FONT_FILE_NAME_LEN       32

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フォントバイナリデータ
//----------------------------------------
class CFontBinData{
public:
    //------------------------
    // バイナリサイズ取得
    //------------------------
    inline static int GetBinSize( void ){
        return( FONT_FILE_NAME_LEN + 4*2 + 2*1 + 2 );
    }

protected:
    //------------------------
    // メンバー
    //------------------------
    char m_cArrFileName[FONT_FILE_NAME_LEN];    // [32]: データファイル名
    eIMG m_eImg;                                // [enum/2]: フォント画像
    int16 m_nLineH;                             // [2]: 高さ
    int16 m_nLineY;                             // [2]: 基本Y座標
    int16 m_nAdjX;                              // [2]: X調整値
    bool m_bAsciiOnly;                          // [1]: ASCII文字のみか？
    bool m_bRenderAtOnce;                       // [1]: 一括レンダリングが可能か？
    eDRAW_SET_TYPE m_eDrawSetType;              // [2]: 描画設定タイプ

public:
    //------------------------
    // コンストラクタ／デストラクタ
    //------------------------
    CFontBinData(){}
    virtual ~CFontBinData(){}

    //------------------------
    // 取得
    //------------------------
    inline const char* getFileName( void ){ return( m_cArrFileName ); }
    inline eIMG getImg( void ){ return( m_eImg ); }
    inline int getLineH( void ){ return( m_nLineH ); }
    inline int getLineY( void ){ return( m_nLineY ); }
    inline int getAdjX( void ){ return( m_nAdjX ); }
    inline bool isAsciiOnly( void ){ return( m_bAsciiOnly ); }
    inline bool isRenderAtOnce( void ){ return( m_bRenderAtOnce ); }
    inline eDRAW_SET_TYPE getDrawSetType( void ){ return( m_eDrawSetType ); }

    //-----------------------------------------------------
    // 読み込み（※[pIB]は有効である前提＆値の整合性等の判断はしない）
    //-----------------------------------------------------
    inline void read( CInputBuffer* pIB ){
        pIB->readBlock( m_cArrFileName, FONT_FILE_NAME_LEN );
        m_eImg = (eIMG)pIB->readInt16();
        m_nLineH = pIB->readInt16();
        m_nLineY = pIB->readInt16();
        m_nAdjX = pIB->readInt16();
        m_bAsciiOnly = (pIB->readUint8() != 0);
        m_bRenderAtOnce = (pIB->readUint8() != 0);
        m_eDrawSetType = (eDRAW_SET_TYPE)pIB->readInt16();
    }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __FONT_BIN_DATA_HPP__
