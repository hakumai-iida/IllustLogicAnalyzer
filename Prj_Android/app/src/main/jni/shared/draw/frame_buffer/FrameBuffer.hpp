/*+----------------------------------------------------------------+
  |	Title:		FrameBuffer.hpp [GLES2.0環境]
  |	Comment:	フレームバッファ（※[CTex]から派生）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FRAME_BUFFER_HPP__
#define __FRAME_BUFFER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/tex/Tex.hpp"
#include "def/FrameBufferBinData.hpp"

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
// フレームバッファ（※[CTex]から派生）
//----------------------------------------
class CFrameBuffer: public CTex{
protected:
	// レンダーバッファオブジェクト識別子（※１始めの符号なし整数、０で無効）
	uint32 m_nGlRenderBuffer;

	// フレームバッファオブジェクト識別子（※１始めの符号なし整数、０で無効）
	uint32 m_nGlFrameBuffer;

public:
    // コンストラクタ／デストラクタ
	CFrameBuffer( void );
	virtual ~CFrameBuffer( void );

    // クリア
	virtual void clear( void );
    
    // 解放
	virtual void release( bool isTextureIgnore=false );
    
	// 判定
    virtual bool isValid( void ){ return( CTex::isValid() && m_nGlFrameBuffer > 0 && m_nGlRenderBuffer > 0 ); }
	virtual bool isFrameBuffer( void ){ return( true ); }

    // 取得
    inline uint32 getGlRenderBuffer( void ){ return( m_nGlRenderBuffer ); }
    inline uint32 getGlFrameBuffer( void ){ return( m_nGlFrameBuffer ); }

    // バイナリの適用
    bool applyBin( CFrameBufferBinData* pBin );

    // 作成
	bool create( int w, int h, uint32 opt=0 );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __FRAME_BUFFER_HPP__
