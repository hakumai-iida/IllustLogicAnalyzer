/*+----------------------------------------------------------------+
  |	Title:		Tex.cpp [GLES2.0環境]
  |	Comment:	テクスチャ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Tex.hpp"

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
CTex::CTex( void ){
	clear();
}

//---------------------------
// デストラクタ
//---------------------------
CTex::~CTex( void ){
	release();
}

//---------------------------
// クリア
//---------------------------
void CTex::clear( void ){
	m_nGlTexture = 0;

	m_fWidth = m_fHeight = 0.0f;
    
	m_fOfsL = m_fOfsR = 0.0f;
	m_fOfsT = m_fOfsB = 0.0f;
    
	m_fBaseX = m_fBaseY = 0.0f;
	m_fFocusX = m_fFocusY = 0.0f;
    
	m_nR = m_nG = m_nB = m_nA = 0;
    
	m_bIsUseAlphaForTexCache = false;
}

//---------------------------
// gl領域解放
//---------------------------
void CTex::release( bool isTextureReleaseIgnore ){
	// テクスチャオブジェクト削除
	if( ! isTextureReleaseIgnore ){
		if( m_nGlTexture > 0 ){
			glDeleteTextures( 1, &m_nGlTexture );
		}
	}
    
	clear();
}

//---------------------------
// テクスチャ作成
//---------------------------
bool CTex::create( BYTE* pBuf, eTEX_DATA_TYPE type, int w, int h, uint32 opt ){
	GLenum format = 0;
	GLenum filter = 0;
	uint32 comp = 0;

    //------------------------------
	// タイプによる割り振り
    //------------------------------
	switch( type ){
	case eTEX_DATA_TYPE_RGBA:
		format = GL_RGBA;
		filter = GL_LINEAR;
		comp = 4;
		break;

	case eTEX_DATA_TYPE_RGB:
		format = GL_RGB;
		filter = GL_LINEAR;
		comp = 3;
		break;

	case eTEX_DATA_TYPE_GRAY:
		format = GL_ALPHA;
		filter = GL_LINEAR;
		comp = 1;
		break;

	case eTEX_DATA_TYPE_P8P:
		format = GL_RGBA;
		filter = GL_NEAREST;
		comp = 4;
		break;

	case eTEX_DATA_TYPE_P8D:
		format = GL_LUMINANCE;
		filter = GL_NEAREST;
		comp = 1;
		break;

	default:
        LOGE( "@ CTex::setFromBuf: INVALID: type=%d, w=%d, h=%d, opt=0x%X\n", type, w, h, opt );
		return( false );
	}

	//-------------------------------------------------------------------
	// パレット例外
	//-------------------------------------------------------------------
    // 画素幅を１ピクセルにするとパレット参照時、意図しない表示となるため、
    // パレット画像を大き目にコピーして意図した色が参照されるようにする
	//-------------------------------------------------------------------
    if( type == eTEX_DATA_TYPE_P8P ){
        // 用心
        if( w != 256 ){
            LOGE( "@ CTex::create: INVALID SIZE FOR P8P: type=%d, w=%d, h=%d, opt=0x%0X\n", type, w, h, opt );
            return( false );
        }
        
    	int out = 0;

    	// データの割当先設定
    	int ofsR, ofsG, ofsB, ofsA;
    	if( (opt&TEX_CREATE_OPT_PAL_IS_ABGR) != 0 ){
       		ofsR = 3;
       		ofsG = 2;
       		ofsB = 1;
       		ofsA = 0;
    	}else{
       		ofsR = 0;
       		ofsG = 1;
       		ofsB = 2;
       		ofsA = 3;
    	}

		//------------------------------------------
        // 塗りパレット調整指定
		//------------------------------------------
        if( (opt&TEX_CREATE_OPT_PAL_ADJ_FOR_ARITH_COL) != 0 ){
            // 各パレットの調整：[明色〜基本色〜暗色]のグラデの作成
            for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
                int pg = BEZIER_CONV_PAL_ID_TO_PAL_GRP( BEZIER_PAL_GRP_ID_HEAD + i );
                int dotMin = BEZIER_CONV_PAL_INFO_TO_DOT( pg, BEZIER_PAL_VAL_MIN );
                int dotBase = BEZIER_CONV_PAL_INFO_TO_DOT( pg, BEZIER_PAL_VAL_BASE );
                int dotMax = BEZIER_CONV_PAL_INFO_TO_DOT( pg, BEZIER_PAL_VAL_MAX );
                
                // 明色〜基本色のグラデ作成
                int fromR = pBuf[4*dotMin+ofsR];
                int fromG = pBuf[4*dotMin+ofsG];
                int fromB = pBuf[4*dotMin+ofsB];
                int toR = pBuf[4*dotBase+ofsR];
                int toG = pBuf[4*dotBase+ofsG];
                int toB = pBuf[4*dotBase+ofsB];

                int numCol = BEZIER_PAL_VAL_BASE - BEZIER_PAL_VAL_MIN;
                for( int j=1; j<numCol; j++ ){
                    pBuf[4*(dotMin+j)+ofsR] = (BYTE)((fromR*(numCol-j)+toR*j)/numCol);
                    pBuf[4*(dotMin+j)+ofsG] = (BYTE)((fromG*(numCol-j)+toG*j)/numCol);
                    pBuf[4*(dotMin+j)+ofsB] = (BYTE)((fromB*(numCol-j)+toB*j)/numCol);
                }
                
                // 基本色〜暗色のグラデ作成
                fromR = pBuf[4*dotBase+ofsR];
                fromG = pBuf[4*dotBase+ofsG];
                fromB = pBuf[4*dotBase+ofsB];
                toR = pBuf[4*dotMax+ofsR];
                toG = pBuf[4*dotMax+ofsG];
                toB = pBuf[4*dotMax+ofsB];
                
                numCol = BEZIER_PAL_VAL_MAX - BEZIER_PAL_VAL_BASE;
                for( int j=1; j<numCol; j++ ){
                    pBuf[4*(dotBase+j)+ofsR] = (BYTE)((fromR*(numCol-j)+toR*j)/numCol);
                    pBuf[4*(dotBase+j)+ofsG] = (BYTE)((fromG*(numCol-j)+toG*j)/numCol);
                    pBuf[4*(dotBase+j)+ofsB] = (BYTE)((fromB*(numCol-j)+toB*j)/numCol);
                }
            }
            
            // オフセット未満のアルファは０にしておく（※塗りバッファの基本値は非０となるため）
            for( int i=0; i<BEZIER_PAL_GRP_HEAD; i++ ){
                pBuf[4*i+ofsA] = 0x00;
            }
        }

    	// 転送用のバッファに設定（※とりあえずスタックで確保）
    	BYTE bufTemp[TEX_PAL_EXPAND_RATE*256*4];
   		for( int i=0; i<256; i++ ){
   			for( int j=0; j<TEX_PAL_EXPAND_RATE; j++ ){
   				bufTemp[out++] = pBuf[4*i+ofsR];
   				bufTemp[out++] = pBuf[4*i+ofsG];
   				bufTemp[out++] = pBuf[4*i+ofsB];
   				bufTemp[out++] = pBuf[4*i+ofsA];
   			}
   		}
        
		// バッファ差し替え
		pBuf = bufTemp;
    	w = TEX_PAL_EXPAND_RATE*256;
    }

	// オプション確認
	if( (opt&TEX_CREATE_OPT_LINEAR) != 0 ){ filter = GL_LINEAR; }
	else if( (opt&TEX_CREATE_OPT_NEAREST) != 0 ){ filter = GL_NEAREST; }

	// パレット画像は[NEAREST]固定（※リニアになっても困るだけなので）
	if( type == eTEX_DATA_TYPE_P8P || type == eTEX_DATA_TYPE_P8D ){
		filter = GL_NEAREST;
	}

    //------------------------------------
	// glに転送
    //------------------------------------
	uint32 tex = 0;
	glGenTextures( 1, &tex );

	// [TEXTURE0]にバインドして転送
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, tex );
	glTexImage2D( GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pBuf );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	//------------------------------
	// 生成情報を設定
	//------------------------------
	m_nGlTexture = tex;
	m_fWidth  = w;
	m_fHeight = h;

	// パレットであれば０番色をクリア色に設定しておく
	if( type == eTEX_DATA_TYPE_P8P ){
		m_nR = pBuf[0];
		m_nG = pBuf[1];
		m_nB = pBuf[2];
		m_nA = pBuf[3];
	}

	return( true );
}

//-----------------------------------------------------------------------
// キャッシュ用にコピー
//（※パレット画像をRGBA画像としてキャッシュする際に画像サイズ等を元画像から受け取る想定）
//------------------------------------------------------------------------
void CTex::copyForCache( uint32 tex, CTex* pTex ){
	release();

	// テクスチャID
	m_nGlTexture = tex;

	// テクスチャサイズ
	m_fWidth = pTex->m_fWidth;
	m_fHeight = pTex->m_fHeight;

	// 画像端調整値
	m_fOfsL = pTex->m_fOfsL;
	m_fOfsR = pTex->m_fOfsR;
	m_fOfsT = pTex->m_fOfsT;
	m_fOfsB = pTex->m_fOfsB;

	// 画像基底位置
	m_fBaseX  = pTex->m_fBaseX;
	m_fBaseY  = pTex->m_fBaseY;
    
	m_fFocusX = pTex->m_fFocusX;
	m_fFocusY = pTex->m_fFocusY;
}
