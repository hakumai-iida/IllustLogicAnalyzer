/*+----------------------------------------------------------------+
  |	Title:		FrameBuffer.hpp [GLES2.0環境]
  |	Comment:	フレームバッファ（※[CTex]から派生）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FrameBuffer.hpp"

#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//---------------------------
// コンストラクタ
//---------------------------
CFrameBuffer::CFrameBuffer( void ): CTex(){
}

//---------------------------
// デストラクタ
//---------------------------
CFrameBuffer::~CFrameBuffer( void ){
}

//---------------------------
// クリア
//---------------------------
void CFrameBuffer::clear( void ){
	m_nGlRenderBuffer = 0;
	m_nGlFrameBuffer = 0;

	CTex::clear();
}

//---------------------------
// gl領域解放
//---------------------------
void CFrameBuffer::release( bool isTextureIgnore ){
	// フレームバッファオブジェクト削除
	if( m_nGlFrameBuffer > 0 ){
		glDeleteFramebuffers( 1, &m_nGlFrameBuffer );
	}

	// レンダーバッファオブジェクト削除
	if( m_nGlRenderBuffer > 0 ){
		glDeleteRenderbuffers( 1, &m_nGlRenderBuffer );
	}

	CTex::release( isTextureIgnore );
}

//----------------------
// バイナリの適用
//----------------------
bool CFrameBuffer::applyBin( CFrameBufferBinData* pBin ){
    // 用心
    if( pBin == NULL ){
        return( false );
    }
    
    // 作成
    if( ! create( pBin->getWidth(), pBin->getHeight(), pBin->getOption() ) ){
        return( false );
    }
    
    // ここまできたら各種設定
    setR( pBin->getR() );
    setG( pBin->getG() );
    setB( pBin->getB() );
    setA( pBin->getA() );

    return( true );
}

//---------------------------
// フレームバッファの作成
//---------------------------
bool CFrameBuffer::create( int w, int h, uint32 opt ){
	uint32 frameBuffer;
    uint32 renderBuffer;
    uint32 tex;

    //----------------------------------------
    // フレームバッファオブジェクト作成
    //----------------------------------------
    glGenFramebuffers( 1, &frameBuffer );
    
    // スクショ指定があれば
    if( (opt&TEX_CREATE_OPT_SCREEN_SHOT) != 0 ){
        w = SS_W;
        h = SS_H;
    }

	// 指定が無効であればアプリの描画領域のサイズを指定
	if( w <= 0 ){ w = FB_W; }
	if( h <= 0 ){ h = FB_H; }

    //----------------------------------------
	// テクスチャオブジェクトの作成
    //----------------------------------------
	glGenTextures( 1, &tex );
	glBindTexture( GL_TEXTURE_2D, tex );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

    int format = GL_LINEAR; // デフォルトは[LINEAR]
    if( (opt&TEX_CREATE_OPT_NEAREST) != 0 ){ format = GL_NEAREST; }
    else if( (opt&TEX_CREATE_OPT_LINEAR) != 0 ){ format = GL_LINEAR; }
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, format );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, format );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    //----------------------------------------
	// レンダーバッファオブジェクトの作成
    //----------------------------------------
	glGenRenderbuffers( 1, &renderBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, renderBuffer );

	// 指定があればデプス
	if( (opt&TEX_CREATE_OPT_DEPTH) != 0 ){
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h );
	}

	// FBOにテクスチャとレンダーバッファオブジェクトをアタッチする
	glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0 );

	// 指定があればデプス
	if( (opt&TEX_CREATE_OPT_DEPTH) != 0 ){
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer );
	}

	// check
	int e = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if( e != GL_FRAMEBUFFER_COMPLETE ){
        LOGE( "@ CFrameBuffer::create: FRAME BUFFER OBJECT CREATE FAILED: w=%d, h=%d, opt=0x%X\n", w, h, opt );
		return( false );
	}

	//------------------------------
	// 生成情報を設定
	//------------------------------
	m_nGlRenderBuffer = renderBuffer;
	m_nGlFrameBuffer = frameBuffer;
	m_nGlTexture = tex;
	m_fWidth  = w;
	m_fHeight = h;

	return( true );
}
