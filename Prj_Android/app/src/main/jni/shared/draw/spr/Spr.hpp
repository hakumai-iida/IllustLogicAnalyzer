/*+----------------------------------------------------------------+
  |	Title:		Spr.hpp [共通環境]
  |	Comment:	スプライト
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SPR_HPP__
#define __SPR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/img/Img.hpp"
#include "draw/frame_buffer/FrameBuffer.hpp"
#include "def/SprBinData.hpp"
#include "def/FontBinData.hpp"
#include "def/FontSprBinData.hpp"

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
// スプライト
//----------------------------------------
class CSpr{
protected:
	eFRAME_BUFFER m_eFrameBufferId;		// 参照フレームバッファID
    eIMG  m_eImgId;						// 参照画像ID
    int   m_nImgDotNo;					// 参照画像ドット番号
	float m_fWidth, m_fHeight;			// スプライトの大きさ（※ピクセル値）
	float m_fU0, m_fV0, m_fU1, m_fV1;	// UV（※[0.0-1.0]調整値）

	// フォント用
	uint32 m_nCharCode;					// スプライトに対応する文字コード（※Unicode値）
	float m_fCharOfsX, m_fCharOfsY;		// 表示オフセット（※ピクセル値）
	float m_fCharAdvX;					// 表示後の足し込み値（※ピクセル値）

public:
    // コンストラクタ／デストラクタ
	CSpr( void );
	virtual ~CSpr( void ){}

    // クリア
	void clear( void );

	// 判定
	bool isValid( void );

	// テクスチャ取得
	CTex* getTexDot( void );
	CTex* getTexPal( void );

    // 取得
	inline float getWidth( void ){ return( m_fWidth ); }
	inline float getHeight( void ){ return( m_fHeight ); }
	inline float getU0( void ){ return( m_fU0 ); }
	inline float getV0( void ){ return( m_fV0 ); }
	inline float getU1( void ){ return( m_fU1 ); }
	inline float getV1( void ){ return( m_fV1 ); }
	inline uint32 getCharCode( void ){ return( m_nCharCode ); }
	inline float getCharOfsX( void ){ return( m_fCharOfsX ); }
	inline float getCharOfsY( void ){ return( m_fCharOfsY ); }
	inline float getCharAdvX( void ){ return( m_fCharAdvX ); }

    // 設定
	inline void setFrameBufferId( eFRAME_BUFFER fbId ){ m_eFrameBufferId = fbId; }
    inline void setTexId( eIMG imgId ){ m_eImgId = imgId; }
    inline void setImgDotNo( int no ){ m_nImgDotNo = no; }
	inline void setWidth( float w ){ m_fWidth = w; }
	inline void setHeight( float h ){ m_fHeight = h; }
	inline void setU0( float u0 ){ m_fU0 = u0; }
	inline void setV0( float v0 ){ m_fV0 = v0; }
	inline void setU1( float u1 ){ m_fU1 = u1; }
	inline void setV1( float v1 ){ m_fV1 = v1; }
	inline void setCharCode( uint32 c ){ m_nCharCode = c; }
	inline void setCharOfsX( float x ){ m_fCharOfsX = x; }
	inline void setCharOfsY( float y ){ m_fCharOfsY = y; }
	inline void setCharAdvX( float x ){ m_fCharAdvX = x; }
    
    // バイナリ適用
    bool applyBin( CSprBinData* pBin );
    bool applyBinForChar( CFontBinData* pBin, CFontSprBinData* pSprBin );

	// スプライト設定
	bool setWithImg( eIMG imgId, int no, int u0, int v0, int u1, int v1 );
	bool setWithFrameBuffer( eFRAME_BUFFER fbId, int u0, int v0, int u1, int v1 );

	// フォントスプライト設定
	bool setWithImgForChar( eIMG imgId, int no, int u0, int v0, int u1, int v1, uint32 c, int oX, int oY, int advX, int adjAdvX );

protected:
	bool set0( CTex* pTex, int u0, int v0, int u1, int v1 );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __SPR_HPP__
