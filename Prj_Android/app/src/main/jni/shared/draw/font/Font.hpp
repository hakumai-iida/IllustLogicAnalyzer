/*+----------------------------------------------------------------+
  |	Title:		Font.hpp [共通環境]
  |	Comment:	フォント
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FONT_HPP__
#define __FONT_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "def/FontBinData.hpp"
#include "draw/spr/Spr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フォント
//----------------------------------------
class CFont {
protected:
	// データ
	int m_nCharNum;			        // コード数
	CSpr *m_oArrSprFont;	        // スプライト配列
	char *m_cArrLabelBuf;	        // ラベルバッファ（領域本体）
	char **m_pArrLabel;	            // ラベル配列（アクセス用）

	// 設定
	int m_nLineHeight;		        // 行の高さ
	int m_nLineBaseY;		        // ベースオフセット
	bool m_bAsciiOnly;		        // ASCIIコードオンリーか？
	bool m_bRenderAtOnce;	        // 一括レンダー可能か？
	eDRAW_SET_TYPE m_eDrawSetType;	// 描画設定

public:
    // コンストラクタ／デストラクタ
	CFont( void );
	virtual ~CFont( void );

    // クリア
	void clear( void );
    
	// バイナリの適用
    bool applyBin( CFontBinData* pBin );

    // 取得
	CSpr *getSprChar( int c );
	CSpr *getSprCharAt( int at );
	const char **getCharLabelArray( void );

	inline int getLineHeight( void ){ return( m_nLineHeight ); }
	inline int getLineBaseY( void ){ return( m_nLineBaseY ); }
	inline bool isRenderAtOnce( void ){ return( m_bRenderAtOnce ); }
	inline eDRAW_SET_TYPE getDrawSetType( void ){ return( m_eDrawSetType ); }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __FONT_HPP__
