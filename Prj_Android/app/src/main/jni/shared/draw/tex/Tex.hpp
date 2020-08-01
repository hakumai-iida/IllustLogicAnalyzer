/*+----------------------------------------------------------------+
  |	Title:		Tex.hpp [GLES2.0環境]
  |	Comment:	テクスチャ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TEX_HPP__
#define __TEX_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------------------------------
// テクスチャデータタイプ
//-------------------------------------------------
enum eTEX_DATA_TYPE{
	eTEX_DATA_TYPE_INVALID = -1,

	eTEX_DATA_TYPE_RGBA,		// RGBAデータ（※１画素４バイト：デフォルト＝リニア）
	eTEX_DATA_TYPE_RGB,		    // RGBデータ（※１画素３バイト：デフォルト＝リニア）
	eTEX_DATA_TYPE_GRAY,		// グレースケール（※１画素１バイト：デフォルト＝リニア）
	eTEX_DATA_TYPE_P8P,		    // 256色パレット（※１画素４バイト：デフォルト＝ニアリスト）
	eTEX_DATA_TYPE_P8D,		    // 256色画素（※１画素１バイト：デフォルト＝ニアリスト）
    
    eTEX_DATA_TYPE_MAX,
};

//-------------------------------------------------
// テクスチャ読み込みオプション
//-------------------------------------------------
// デプスを利用する（※フレームバッファ用）
#define TEX_CREATE_OPT_DEPTH                    (0x00000001)

// リニアとして扱う
#define TEX_CREATE_OPT_LINEAR	                (0x00000002)

// ニアリストとして扱う
#define TEX_CREATE_OPT_NEAREST	                (0x00000004)

// スクリーンショット用のサイズを指定する
#define TEX_CREATE_OPT_SCREEN_SHOT              (0x00000008)

// パレット色の並びを[ABGR]として扱う（※指定がない場合は[RGBA]）
#define TEX_CREATE_OPT_PAL_IS_ABGR              (0x00000010)

// アリスの塗りパレット補正を行う
#define TEX_CREATE_OPT_PAL_ADJ_FOR_ARITH_COL    (0x00000020)

//----------------------------------------------------------------
// パレット画像の拡張倍率（※等倍だとパレット参照がズレるので大き目の画像にする）
//----------------------------------------------------------------
#define TEX_PAL_EXPAND_RATE         2

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// テクスチャ
//----------------------------------------
class CTex{
protected:
    //----------------------------------------------------------------
	// テクスチャオブジェクト識別子（※１始めの符号なし整数、０で無効）
    //----------------------------------------------------------------
	uint32 m_nGlTexture;
    
    //----------------------------------------------------------------
	// テクスチャサイズ（※実サイズ）
    //----------------------------------------------------------------
	float m_fWidth, m_fHeight;
    
    //----------------------------------------------------------------
	// 画像端調整値（※表示時に適用される上下左右のオフセット）
	//（※テクスチャの上下左右の空白がカットされている場合に値が設定される）
    //----------------------------------------------------------------
	float m_fOfsL, m_fOfsR;
	float m_fOfsT, m_fOfsB;

    //----------------------------------------------------------------
	// 画像基底位置（※アクション等の基準点合わせ用）
    //----------------------------------------------------------------
	float m_fBaseX, m_fBaseY;		// キャラクタの足元検出用
	float m_fFocusX, m_fFocusY;		// キャラクタの顔中心検出用

    //----------------------------------------------------------------
	// クリアカラー（※フレームバッファクリア時に利用）
    //----------------------------------------------------------------
	BYTE m_nR, m_nG, m_nB, m_nA;

    //----------------------------------------------------------------
	// テクスチャキャッシュ描画時に透明を有効にするか？
    //----------------------------------------------------------------
	bool m_bIsUseAlphaForTexCache;

public:
    //-----------------------------
    // コンストラクタ／デストラクタ
    //-----------------------------
	CTex( void );
	virtual ~CTex( void );

    //-----------------------------
    // クリア／解放
    //-----------------------------
	virtual void clear( void );
	virtual void release( bool isTextureReleaseIgnore=false );

    //-----------------------------
	// 判定
    //-----------------------------
    virtual bool isValid( void ){ return( m_nGlTexture > 0 ); }
	virtual bool isFrameBuffer( void ){ return( false ); }

    //-----------------------------
	// 取得
    //-----------------------------
	inline int getGlTexture( void ){ return( m_nGlTexture ); }

	inline float getWidth( void ){ return( m_fWidth ); }
	inline float getHeight( void ){ return( m_fHeight ); }
	inline float getWidthOrig( void ){ return( m_fWidth + m_fOfsL + m_fOfsR ); }
	inline float getHeightOrig( void ){ return( m_fHeight + m_fOfsT + m_fOfsB ); }
    
	inline float getOfsL( void ){ return( m_fOfsL ); }
	inline float getOfsR( void ){ return( m_fOfsR ); }
	inline float getOfsT( void ){ return( m_fOfsT ); }
	inline float getOfsB( void ){ return( m_fOfsB ); }
    
	inline float getBaseX( void ){ return( m_fBaseX ); }
	inline float getBaseY( void ){ return( m_fBaseY ); }
	inline float getFocusX( void ){ return( m_fFocusX ); }
	inline float getFocusY( void ){ return( m_fFocusY ); }
    
	inline BYTE getR( void ){ return( m_nR ); }
	inline BYTE getG( void ){ return( m_nG ); }
	inline BYTE getB( void ){ return( m_nB ); }
	inline BYTE getA( void ){ return( m_nA ); }

    inline bool isUseAlphaForTexCache( void ){ return( m_bIsUseAlphaForTexCache ); }

    //-----------------------------
	// 設定
    //-----------------------------
	inline void setOfsL( float oL ){ m_fOfsL = oL; }
	inline void setOfsR( float oR ){ m_fOfsR = oR; }
	inline void setOfsT( float oT ){ m_fOfsT = oT; }
	inline void setOfsB( float oB ){ m_fOfsB = oB; }
    inline void setOfsLRTB( float oL, float oR, float oT, float oB ){ m_fOfsL = oL; m_fOfsR = oR; m_fOfsT = oT; m_fOfsB = oB; }

    inline void setBaseX( float x ){ m_fBaseX = x; }
	inline void setBaseY( float y ){ m_fBaseX = y; }
    inline void setBaseXY( float x, float y ){ m_fBaseX = x; m_fBaseY = y; }
    
	inline void setFocusX( float x ){ m_fFocusX = x; }
	inline void setFocusY( float y ){ m_fFocusY = y; }
    inline void setFocusXY( float x, float y ){ m_fFocusX = x; m_fFocusY = y; }

    inline void setR( BYTE r ){ m_nR = r; }
	inline void setG( BYTE g ){ m_nG = g; }
	inline void setB( BYTE b ){ m_nB = b; }
	inline void setA( BYTE a ){ m_nA = a; }
    inline void setRGBA( BYTE r, BYTE g, BYTE b, BYTE a ){ m_nR = r; m_nG = g; m_nB = b; m_nA = a; }
    
	inline void setUseAlphaForTexCache( bool flag ){ m_bIsUseAlphaForTexCache = flag; }
    
    //-----------------------------
    // 作成
    //-----------------------------
    bool create( BYTE* pBuf, eTEX_DATA_TYPE type, int w, int h, uint32 opt=0 );

    //-----------------------------
    // キャッシュ用に設定値のコピー
    //-----------------------------
	void copyForCache( uint32 tex, CTex* pTex );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TEX_HPP__
