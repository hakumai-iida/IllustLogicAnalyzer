/*+----------------------------------------------------------------+
  |	Title:		Shader.hpp [GLES2.0環境]
  |	Comment:	シェーダー
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SHADER_HPP__
#define __SHADER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/tex/Tex.hpp"
#include "draw/spr/Spr.hpp"
#include "def/ShaderBinData.hpp"
#include "sys/file/fpack.hpp"

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
// シェーダー
//----------------------------------------
class CShader{
protected:
	//---------------------------------------------------------
	// クラスメンバー（※各インスタンスは頂点配列等のワークを共有する）
	//---------------------------------------------------------
    // 頂点バッファ
    static int s_nVertexUse;
    static float* s_fArrVertexXY;
    static float* s_fArrVertexUV;

    // キャッシュ（※あまり意味は無い？）
	static uint32 s_nLastProgram;
	static int s_nLastBindTex;
	static int s_nLastBindTexP;

    // 現在のレンダー対象領域（※簡便のため）
    static float s_fCurRenderAreaWH, s_fCurRenderAreaHH;
    static float s_fCurRenderAreaX0, s_fCurRenderAreaY0;
    static float s_fCurRenderAreaX1, s_fCurRenderAreaY1;

	//------------------------------------
	// メンバー：コンパイル時に設定されるもの
	//------------------------------------
    // プログラムオブジェクト（※１始めの識別子、０で無効）
	uint32 m_nProgram;

	// 頂点シェーダーオブジェクト（※１始めの識別子、０で無効）
	uint32 m_nVertexShader;

	// フラグメントシェーダーオブジェクト（※１始めの識別子、０で無効）
	uint32 m_nFragmentShader;

	// 属性／ユニフォーム（※０始めの識別子、－１で無効）
	int m_aXY;
	int m_aUV;
	int m_uDepth;
	int m_uColor;
	int m_uApplyRate;
	int m_uTex, m_uTexP;

    //------------------------------------
    // メンバー：描画時に設定するもの
    //------------------------------------
    // 制御フラグ
    bool m_bRenderForGame;          // ゲームエリアに描画するのか？（※論理座標を実座標へ調整するか？）
    bool m_bRenderForSafeArea;      // セーフエリアに描画するのか？（※セーフエリア調整をするか？）
    bool m_bRenderWithIntPosition;  // 描画座標を整数にするか？

    // 設定値
	float m_fDepth;
    float m_fColR, m_fColG, m_fColB, m_fColA;
	float m_fApplyRate;
    bool m_bFlipX, m_bFlipY;
    float m_fRotation;
    float m_fRotX, m_fRotY;
    
public:
	//------------------------------------
	// クラスメソッド
	//------------------------------------
    static bool Init( void );
    static void Exit( void );
    static void SetCurRenderAreaSize( float w, float h );
    static void SetCurRenderArea( float x, float y, float w, float h );
    static void ClearCache( void );
	static uint32 LoadShader( int shaderType, const char* pSrc );

	//------------------------------------
	// メソッド
	//------------------------------------
    // コンストラクタ／デストラクタ
    CShader( void );
	virtual ~CShader( void );

    // クリア
    void clear( void );

    // リセット
	void reset( void );
    
    // バイナリの適用
    bool applyBinWithFpack( CShaderBinData* pBin, stFPACK* fpack );
    
	// 設定
	inline void setDepth( float depth ){ m_fDepth = depth; }
	inline void setApplyRate( float rate ){ m_fApplyRate = rate; }
	inline void setRGBA( float r, float g, float b, float a ){ m_fColR = r; m_fColG = g; m_fColB = b; m_fColA = a; }
	inline void setRGB( float r, float g, float b ){ m_fColR = r; m_fColG = g; m_fColB = b; }
	inline void setFlipX( bool flag ){ m_bFlipX = flag; }
	inline void setFlipY( bool flag ){ m_bFlipY = flag; }
	inline void setRotation( float r ){ m_fRotation = r; }
	inline void setRotXY( float rX, float rY ){ m_fRotX = rX; m_fRotY = rY; }

    // フラグ設定
    inline void setRenderForGame( bool flag ){ m_bRenderForGame = flag; }
    inline void setRenderForSafeArea( bool flag ){ m_bRenderForSafeArea = flag; }
    inline void setRenderWithIntPosition( bool flag ){ m_bRenderWithIntPosition = flag; }

	// 矩形登録
	void pushRect( float x, float y, float w, float h );
	void pushTex( CTex* pTex, float x, float y, float w, float h, float u, float v, float uW, float vH );
	void pushSpr( CSpr* pSpr, float x, float y, float scaleW, float scaleH );
	void pushSprWithUVScale( CSpr* pSpr, float x, float y, float scaleW, float scaleH );

    // 頂点登録（※多角形を描く場合）
    void pushVertex( float x, float y, float u, float v );

    // 表示
	void render( CTex* pTex, CTex* pTexP );
    
protected:
    // シェーダー設定
    bool setFromFile( const char* pVertexSourceName, const char* pFragmentSourceName );
    bool setFromFpack( stFPACK* pPack, const char* pVertexSourceName, const char* pFragmentSourceName );
    bool setFromId( uint32 glVertexId, uint32 glFragmentId );

    // 以下の処理でゲーム座標調整が行われて頂点が設定される
	bool setVertexXYThenCheck( float x, float y, float w, float h );
	bool setVertexUV( float u0, float v0, float u1, float v1 );
    bool setVertexXYUV( float x, float y, float u, float v );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __SHADER_HPP__
