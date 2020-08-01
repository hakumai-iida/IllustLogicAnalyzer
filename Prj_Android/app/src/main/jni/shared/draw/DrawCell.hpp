/*+----------------------------------------------------------------+
  |	Title:		DrawCell.hpp [共通環境]
  |	Comment:	描画要素
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DRAW_CELL_HPP__
#define __DRAW_CELL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 文字列表示長（※１度に表示できる文字列長）
#define DRAW_CELL_STR_DISP_LEN	256

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 描画要素
//----------------------------------------
class CDrawCell{
private:
    //----------------------
    // クラス変数
    //----------------------
	// 使いまわされるセル（※メモリ確保をその都度行わせないため）
	static CDrawCell s_oFreeCell;

	// ログ
	static eDRAW_ALIGN_X s_eLogAlignX;
	static eDRAW_ALIGN_Y s_eLogAlignY;
	static float s_fLogX;
	static float s_fLogY;
    static float s_fLogScale;

	// デプスの調整値
	static float s_fDepthOfs;

	// 一律描画補正値
	static float s_fOfsX;
	static float s_fOfsY;
	static BYTE  s_nAlpha;

    // 特殊表示用
	static bool  s_bIgnoreApplyRate;
	static float s_fNegaRate;
	static float s_fMonoRate;
	static float s_fSepiaRate;

protected:
    //-----------------------
	// メンバー変数
    //-----------------------
	eDRAW_TYPE    m_eType;
	eDRAW_SHADER  m_eShader;
	eDRAW_BLEND   m_eBlend;
	eDRAW_ALIGN_X m_eAlignX;
	eDRAW_ALIGN_Y m_eAlignY;

	float  m_fScaleX, m_fScaleY;
	uint32 m_nRGBA;
	BYTE   m_nAlpha;
	bool   m_bFlipX, m_bFlipY;
	float  m_fRotation;
	float  m_fRotX, m_fRotY;
	bool   m_bDepthTest;
	float  m_fDepth;
	bool   m_bUseTexCache;
	bool   m_bTexCacheTemporay;
	float  m_fMonoRate;
	float  m_fNegaRate;
	float  m_fSepiaRate;

    // for eDRAW_TYPE_RECT
    float  m_fRectW, m_fRectH;
    
	// for eDRAW_TYPE_TEX
	CTex* m_pTex;
	CTex* m_pTexP;
    float m_fDrawTexCutL, m_fDrawTexCutR;   // テクスチャ表示カット指定：左右
    float m_fDrawTexCutT, m_fDrawTexCutB;   // テクスチャ表示カット指定：上下

    // for eDRAW_TYPE_SPR
	CSpr* m_pSpr;
    bool   m_bSprWithUVScale;

    // for eDRAW_TYPE_STR
    eFONT m_eFontId;
	const char* m_pStr;
	float m_fStrMarginX;        // 文字間の調整値
	int m_nDispLen;             // 表示文字数（※タイプライター演出用）

    // for eDRAW_TYPE_NUM
    eNUM  m_eNumId;
	int m_nNum;
	int m_nForceFigureNum;      // 表示桁数
	float m_fFigureMarginX;     // 数字間の調整値
	bool m_bNumWithPlus;        // [＋]を描くか？
	int* m_nArrFigureRGBA;	    // 数字のグラデ演出用

    // for eDRAW_TYPE_POLY
	int m_nNumVertex;
	const float* m_pReffArrX;
	const float* m_pReffArrY;
	const float* m_pReffArrU;
	const float* m_pReffArrV;
    
    //---------------------------
    // ワーク
    //---------------------------
    // フラグ（※[draw0]からシェーダーに渡す用）
    bool m_bRenderForGame;
    bool m_bRenderForSafeArea;
    bool m_bRenderWithIntPosition;

public:
    //----------------------
    // クラスメソッド
    //----------------------
    // セル取得
    inline static CDrawCell* GetFreeCell( void ){ return( &s_oFreeCell ); }
    
    // ログ表示
    static void DrawLog( const char* pStr, float x, float y, uint32 rgba=0xFFFFFFFF, float scale=1.0f );

    // レイアウト付きログ表示
    static void DrawLogAlign( const char* pStr, uint32 rgba=0xFFFFFFFF, float scale=0.0f, float ofsX=0.0f, float ofsY=0.0f );
	static void SetLogAlign( eDRAW_ALIGN_X alignX, eDRAW_ALIGN_Y alignY, float baseScale=1.0f, float ofsX=0.0f, float ofsY=0.0f );
    inline static void SetLogAlignLT( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_L, eDRAW_ALIGN_Y_T, s, x, y ); }
    inline static void SetLogAlignLM( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_L, eDRAW_ALIGN_Y_M, s, x, y ); }
    inline static void SetLogAlignLB( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_L, eDRAW_ALIGN_Y_B, s, x, y ); }
    inline static void SetLogAlignCT( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_T, s, x, y ); }
    inline static void SetLogAlignCM( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M, s, x, y ); }
    inline static void SetLogAlignCB( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_B, s, x, y ); }
    inline static void SetLogAlignRT( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_R, eDRAW_ALIGN_Y_T, s, x, y ); }
    inline static void SetLogAlignRM( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_R, eDRAW_ALIGN_Y_M, s, x, y ); }
    inline static void SetLogAlignRB( float s=1.0f, float x=0.0f, float y=0.0f ){ SetLogAlign( eDRAW_ALIGN_X_R, eDRAW_ALIGN_Y_B, s, x, y ); }

    // デプスオフセット（※デプス指定の描画の度に更新されていく値のリセット）
	inline static void ResetDepthOfs( void ){ s_fDepthOfs = 0.0f; }

    // 広域指定
    inline static void ResetOfsXY(){ s_fOfsX = 0.0f; s_fOfsY = 0.0f; }
	inline static void SetOfsXY( float x, float y ){ s_fOfsX = x; s_fOfsY = y; }
    inline static void ResetAlpha(){ s_nAlpha = 0xFF; }
	inline static void SetAlpha( BYTE a ){ s_nAlpha = a; }

    // 特殊描画指定
    inline static void ResetApplyRate( void ){ s_bIgnoreApplyRate = false; s_fNegaRate = s_fMonoRate = s_fSepiaRate = 0.0f; }
	inline static void SetIgnoreApplyRate( bool flag ){ s_bIgnoreApplyRate = flag; }
	inline static void SetNegaRate( float rate ){ s_fNegaRate = rate; }
	inline static void SetMonoRate( float rate ){ s_fMonoRate = rate; }
	inline static void SetSepiaRate( float rate ){ s_fSepiaRate = rate; }

    //----------------------
    // メンバーメソッド
    //----------------------
    // コンストラクタ／デストラクタ
	CDrawCell( void );
	virtual ~CDrawCell( void ){}

	// クリア
	void clear( void );

    //---------------------------------------------
	// 矩形に設定：eDRAW_TYPE_RECT
    //---------------------------------------------
	void setRect( float w, float h );
    void setRectFull( void );

    //---------------------------------------------
    // テクスチャ設定：eDRAW_TYPE_TEX
    //---------------------------------------------
    void setTex( CTex* pTex, CTex* pTexP );
    inline void changeTex( CTex* pTex ){ m_pTex = pTex; }
    inline void changePal( CTex* pTexP ){ m_pTexP = pTexP; }

    // フレームバッファ設定
    inline void setFrameBuffer( CFrameBuffer* pFB ){ setTex( pFB, NULL ); }
    inline void setFrameBuffer( eFRAME_BUFFER fbId ){ setFrameBuffer( CFrameBufferMgr::GetFrameBuffer( fbId ) ); }

	// 画像設定
    inline void setImg( CImg* pImg, int dotAt=0, int palAt=0 ){ setTex( pImg->getDot(dotAt), pImg->getPal(palAt) ); }
    inline void setImg( eIMG imgId, int dotAt=0, int palAt=0 ){ setImg( CImgMgr::GetImg( imgId ), dotAt, palAt ); }
    
    // テクスチャ表示カット指定
    inline void setDrawTexCut( float l, float r, float t, float b ){
        m_fDrawTexCutL = l; m_fDrawTexCutR = r; m_fDrawTexCutT = t; m_fDrawTexCutB = b;
    }

    //---------------------------------------------
    // スプライト設定：eDRAW_TYPE_SPR
    //---------------------------------------------
    void setSpr( CSpr* pSpr );
    inline void setSpr( eSPR sprId ){ setSpr( CSprMgr::GetSpr( sprId ) ); }

    //---------------------------------------------
	// 文字列に設定：eDRAW_TYPE_STR
    //---------------------------------------------
	void setStr( eFONT fontId, const char* pStr, int dispLen=-1 );
    inline void changeStr( const char* pStr, int dispLen=-1 ){ m_pStr = pStr; m_nDispLen = dispLen; }
	inline void setStrMarginX( float x ){ m_fStrMarginX = x; }

    //---------------------------------------------
	// 数値に設定：eDRAW_TYPE_NUM
    //---------------------------------------------
	void setNum( eNUM numId, int numVal );
    inline void changeNum( int numVal ){ m_nNum = numVal; }
    inline void setNumFigure( int figureNum ){ m_nForceFigureNum = figureNum; }
    inline void setNumMarginX( float marginX ){ m_fFigureMarginX = marginX; }
    inline void setNumWithPlus( bool flag ){ m_bNumWithPlus = flag; }
    inline void setNumFigureRGBA( int* arrFigureRGBA ){ m_nArrFigureRGBA = arrFigureRGBA; }

    //---------------------------------------------
	// ポリゴンに設定：eDRAW_TYPE_POLY
    //---------------------------------------------
	void setPoly( CTex* pTex, CTex* pTexP, int numVert, const float* pArrX, const float* pArrY, const float* pArrU, const float* pArrV );
    inline void setPoly( int numVert, const float* pArrX, const float* pArrY ){ setPoly( NULL, NULL, numVert, pArrX, pArrY, NULL, NULL ); }

    // 揃え
    inline void setAlignX( eDRAW_ALIGN_X alignX ){ m_eAlignX = alignX; }
    inline void setAlignY( eDRAW_ALIGN_Y alignY ){ m_eAlignY = alignY; }
    inline void setAlignXY( eDRAW_ALIGN_X alignX, eDRAW_ALIGN_Y alignY ){ m_eAlignX = alignX; m_eAlignY = alignY; }
    
    // ブレンド設定
    inline void setBlend( eDRAW_BLEND blend ){ m_eBlend = blend; }
    inline void setBlendOff( void ){ m_eBlend = eDRAW_BLEND_OFF; }
    inline void setBlendAlpha( void ){ m_eBlend = eDRAW_BLEND_ALPHA; }
    inline void setBlendAlphaOne( void ){ m_eBlend = eDRAW_BLEND_ALPHA_ONE; }
    inline void setBlendAdd( void ){ m_eBlend = eDRAW_BLEND_ADD; }
    inline void setBlendMulti( void ){ m_eBlend = eDRAW_BLEND_MULTI; }

    // 描画タイプ設定
    void setDrawSetType( eDRAW_SET_TYPE type );

    // スケール設定
    inline void setScaleX( float x ){ m_fScaleX = x; }
    inline void setScaleY( float y ){ m_fScaleY = y; }
    inline void setScaleXY( float x, float y ){ m_fScaleX = x; m_fScaleY = y; }
    inline void setSprWithUVScale( bool flag ){ m_bSprWithUVScale = flag; }
    
    // フリップ
    inline void setFlipX( bool flag ){ m_bFlipX = flag; }
    inline void setFlipY( bool flag ){ m_bFlipY = flag; }

	// 回転
	inline void setRotation( float rot, float cX, float cY ){ m_fRotation = rot; m_fRotX = cX; m_fRotY = cY; }

	// 色関連
    inline void setRGBA( uint32 rgba ){ m_nRGBA = rgba; }
    inline void setAlpha( BYTE alpha ){ m_nAlpha = alpha; }

	// デプス関連
	void setDepth( float depth );

	// テクスチャキャッシュ指定
	inline void setUseTexCache( bool flag, bool isTemporary=false );

	// 特殊
	inline void setMonoRate( float rate ){ m_fMonoRate = rate; }
	inline void setNegaRate( float rate ){ m_fNegaRate = rate; }
	inline void setSepiaRate( float rate ){ m_fSepiaRate =rate; }

	// 取得
    inline CTex* getTex( void ){ return( m_pTex ); }
    inline CTex* getTexP( void ){ return( m_pTexP ); }
    inline CSpr* getSpr( void ){ return( m_pSpr ); }
    inline const char* getStr( void ){ return( m_pStr ); }
    inline int getNum( void ){ return( m_nNum ); }

	// 横幅／縦幅取得
	float getOriginalWidth( void );
	float getWidth( void );
	float getOriginalHeight( void );
	float getHeight( void );

	// 横幅／縦幅設定（※指定幅になるように調整）
	void setWidth( float w );
	void setHeight( float h );

	// シェーダー設定
	void setShaderNormal( void );
	void setShaderRGBA( void );
	void setShaderAlpha( void );

	// シェーダー取得
	CShader* getShaderObject( void );

	// 描画
	void draw0( float x, float y, bool isGame, bool isSafeArea, bool isInt );
    inline void draw( float x, float y ){ draw0( x, y, true, true, false ); }
    inline void drawF( float x, float y ){ draw0( x, y, true, true, false ); }
    inline void drawI( float x, float y ){ draw0( x, y, true, true, true ); }
    inline void drawFB_F( float x, float y ){ draw0( x, y, false, false, false ); }
    inline void drawFB_I( float x, float y ){ draw0( x, y, false, false, true ); }
    inline void drawGA_F( float x, float y ){ draw0( x, y, true, false, false ); }
    inline void drawGA_I( float x, float y ){ draw0( x, y, true, false, true ); }
    inline void drawSA_F( float x, float y ){ draw0( x, y, true, true, false ); }
	inline void drawSA_I( float x, float y ){ draw0( x, y, true, true, true ); }

private:
    //-----------------------
	// 各種描画
    //-----------------------
    // for eDRAW_TYPE_RECT
	void  drawRect( float x, float y );

    // for eDRAW_TYPE_TEX
	void  drawTex( float x, float y );

    // for eDRAW_TYPE_SPR
	void  drawSpr( float x, float y );

    // for eDRAW_TYPE_STR
	void  drawStr( float x, float y );
	float getStrOriginalWidth( void );
	float getStrOriginalHeight( void );

    // for eDRAW_TYPE_NUM
	void  drawNum( float x, float y );
	int   getNumFigure( int v );
	float getNumOriginalWidth( void );
	float getNumOriginalHeight( void );

    // for eDRAW_TYPE_POLY
	void  drawPoly( float x, float y );
	float getPolyOriginalWidth( void );
	float getPolyOriginalHeight( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __DRAW_CELL_HPP__
