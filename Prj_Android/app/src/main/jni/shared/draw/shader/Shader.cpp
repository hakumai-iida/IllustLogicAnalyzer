/*+----------------------------------------------------------------+
  |	Title:		Shader.cpp [GLES2.0環境]
  |	Comment:	シェーダー
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Shader.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// バッファサイズ関連
#define FLOAT_SIZE			4
#define VERTEX_XY_STRIDE	(2*FLOAT_SIZE)		// float X, Y;
#define VERTEX_UV_STRIDE	(2*FLOAT_SIZE)		// float U, V;
#define VERTEX_XY_IN_DRAW	2
#define VERTEX_UV_IN_DRAW	2
#define MAX_VERTEX			4096

// テクスチャのバインド先
#define TEX_BIND_TARGET_FOR_TEX		GL_TEXTURE0
#define TEX_BIND_TARGET_FOR_TEX_P	GL_TEXTURE1

#define TEX_BIND_ID_FOR_TEX			0 	// GLES20.GL_TEXTURE0
#define TEX_BIND_ID_FOR_TEX_P		1 	// GLES20.GL_TEXTURE1

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// バッファ
int CShader::s_nVertexUse;
float* CShader::s_fArrVertexXY;
float* CShader::s_fArrVertexUV;

// キャッシュ
uint32 CShader::s_nLastProgram;
int CShader::s_nLastBindTex;
int CShader::s_nLastBindTexP;

// 簡便のため
float CShader::s_fCurRenderAreaWH, CShader::s_fCurRenderAreaHH;
float CShader::s_fCurRenderAreaX0, CShader::s_fCurRenderAreaY0;
float CShader::s_fCurRenderAreaX1, CShader::s_fCurRenderAreaY1;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CShader::Init( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CShader::Init()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	s_fArrVertexXY = new float[MAX_VERTEX*VERTEX_XY_IN_DRAW];
    s_fArrVertexUV = new float[MAX_VERTEX*VERTEX_UV_IN_DRAW];
    CMemMgr::PopTargetField();

	return( true );
}

//------------------------
// 終了
//------------------------
void CShader::Exit( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CShader::Exit()\n" );
#endif

	SAFE_DELETE_ARRAY( s_fArrVertexXY );
	SAFE_DELETE_ARRAY( s_fArrVertexUV );
}

//----------------------
// レンダー領域サイズ設定
//----------------------
void CShader::SetCurRenderAreaSize( float w, float h ){
	s_fCurRenderAreaWH = w/2.0f;
	s_fCurRenderAreaHH = h/2.0f;
}

//----------------------
// レンダー領域設定
//----------------------
void CShader::SetCurRenderArea( float x, float y, float w, float h ){
	// 頂点のクリップ処理用に各座標を算出しておく（※中心を原点に[-1.0〜+1.0f]の範囲）
	s_fCurRenderAreaX0 = (x-s_fCurRenderAreaWH)/s_fCurRenderAreaWH;
	s_fCurRenderAreaY0 = (s_fCurRenderAreaHH-y)/s_fCurRenderAreaHH;
	s_fCurRenderAreaX1 = s_fCurRenderAreaX0 + (w/s_fCurRenderAreaWH);
	s_fCurRenderAreaY1 = s_fCurRenderAreaY0 - (h/s_fCurRenderAreaHH);
}

//----------------------------
// キャッシュクリア
//----------------------------
void CShader::ClearCache( void ){
	s_nLastProgram = 0;
	s_nLastBindTex = -1;
	s_nLastBindTexP = -1;
}

//------------------
// シェーダー読み込み
//------------------
uint CShader::LoadShader( int shaderType, const char* pSrc ){
	// ソース確認
	//LOGD( "@CShader::LoadShader: type=%d\n%s\n", shaderType, pSrc );

	// シェーダー作成
	uint32 shader = glCreateShader( shaderType );
	if( shader == 0 ){
		LOGE( "@ CShader::LoadShader: CAN'T CREATE SHADER\n" );
		return( 0 );
	}

	glShaderSource( shader, 1, (const char**)&pSrc, NULL );
	glCompileShader( shader );

	int compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if( compiled == 0) {
		LOGE( "@ CShader::LoadShader: COMPILE FILED:\n" );

		char* log = CMemMgr::GetBufFromTempStr();
		int size = CMemMgr::GetSizeFromTempStr();
		int len;
		glGetShaderInfoLog( shader, size, &len, log );
		LOGE( "@ glGetShaderInfoLog = \n%s\n", log );

		glDeleteShader( shader );

		return( 0 );
	}

	return( shader );
}

//------------------------
// コンストラクタ
//------------------------
CShader::CShader( void ){
	clear();
}

//------------------------
// デストラクタ
//------------------------
CShader::~CShader( void ){
    clear();
}

//----------------------------
// クリア（※プログラムの内容の破棄）
//----------------------------
void CShader::clear( void ){
    // 確保領域の解放
    if( m_nProgram > 0 ){
        glDeleteProgram( m_nProgram );
        m_nProgram = 0;
    }
    
    if( m_nVertexShader > 0 ){
        glDeleteShader( m_nVertexShader );
        m_nVertexShader = 0;
    }
    
    if( m_nFragmentShader > 0 ){
        glDeleteShader( m_nFragmentShader );
        m_nFragmentShader = 0;
    }

    // クリア
    m_aXY = -1;
	m_aUV = -1;
	m_uDepth = -1;
	m_uColor = -1;
	m_uTex = -1;
	m_uTexP = -1;

    // リセット
	reset();
}

//----------------------------
// リセット（※描画設定の破棄）
//----------------------------
void CShader::reset( void ){
    m_bRenderForGame = false;
    m_bRenderForSafeArea = false;
    m_bRenderWithIntPosition = false;

	m_fDepth = 0.0f;
    m_fColR = m_fColG = m_fColB = m_fColA = 1.0f;
	m_fApplyRate = 0.0f;

    m_bFlipX = m_bFlipY = false;
    m_fRotation = 0.0f;
    m_fRotX = m_fRotY = 0.0f;
}

//---------------------------
// バイナリの適用
//---------------------------
bool CShader::applyBinWithFpack( CShaderBinData* pBin, stFPACK* pFpack ){
    // 用心
    if( pBin == NULL ){
        return( false );
    }
    
    return( setFromFpack( pFpack, pBin->getVertexName(), pBin->getFragmentName() ) );
}

//---------------------------
// ファイルから読み込む
//---------------------------
bool CShader::setFromFile( const char* pVertexSourceName, const char* pFragmentSourceName ){
	//LOGD( "@ CShader::setFromFile( %s, %s )\n", pVertexSourceName, pFragmentSourceName );

	// バッファ確保
	char* pSrc = (char*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_IN );
	uint32 size = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_IN );
	uint32 readed;

	// 頂点シェーダー読み込み
	if( !CFileMgr::LoadFile( pVertexSourceName, pSrc, size-1, &readed ) ){
		LOGE( "@ CShader::setFromFile: LOAD FAILED: %s\n", pVertexSourceName );
		return( 0 );
	}
	pSrc[readed] = '\0';	// 用心に終端指定

	int vertexShader = LoadShader( GL_VERTEX_SHADER, pSrc );
	if( vertexShader == 0 ){
		LOGE( "@ CShader::setFromFile: INVALID VERTEX SHADER: %s\n", pVertexSourceName );
		return( false );
	}

	// フラグメントシェーダー読み込み
	if( !CFileMgr::LoadFile( pFragmentSourceName, pSrc, size-1, &readed ) ){
		LOGE( "@ CShader::setFromFile: LOAD FAILED: %s\n", pFragmentSourceName );
		return( false );
	}
	pSrc[readed] = '\0';	// 用心に終端指定

	int fragmentShader = LoadShader( GL_FRAGMENT_SHADER, pSrc );
	if( fragmentShader == 0 ){
		LOGE( "@ CShader::setFromFile: INVALID FRAGMENT SHADER: %s\n", pFragmentSourceName );
		return( false );
	}

	return( setFromId( vertexShader, fragmentShader ) );
}

//---------------------------
// パックファイルから読み込む
//---------------------------
bool CShader::setFromFpack( stFPACK* pPack, const char* pVertexSourceName, const char* pFragmentSourceName ){
	//LOGD( "@ CShader::setFromPack( %s, %s )\n", pVertexSourceName, pFragmentSourceName );

	int target;
	uint32 size;
	char* pSrc;

	// 頂点シェーダー読み込み
	target = fpack_search_file( pPack, pVertexSourceName, false );
	if( target < 0 ){
		LOGE( "@ CShader::setFromFpack: NOT FOUND: %s\n", pVertexSourceName );
		return( false );
	}
	pSrc = (char*) fpack_get_data( pPack, target, &size, false );
	if( pSrc == NULL ){
		LOGE( "@ CShader::setFromFpack: LOAD FAILED: %s\n", pVertexSourceName );
		return( false );
	}
	pSrc[size] = '\0';	// 用心に終端指定

	int vertexShader = LoadShader( GL_VERTEX_SHADER, pSrc );
	if( vertexShader == 0 ){
		LOGE( "@ CShader::setFromFile: INVALID VERTEX SHADER: %s\n", pVertexSourceName );
		return( false );
	}

	// フラグメントシェーダー読み込み
	target = fpack_search_file( pPack, pFragmentSourceName, false );
	if( target < 0 ){
		LOGE( "@ CShader::setFromFpack: NOT FOUND: %s\n", pFragmentSourceName );
		return( false );
	}
	pSrc = (char*) fpack_get_data( pPack, target, &size, false );
	if( pSrc == NULL ){
		LOGE( "@ CShader::setFromFpack: LOAD FAILED: %s\n", pFragmentSourceName );
		return( false );
	}
	pSrc[size] = '\0';	// 用心に終端指定

	int fragmentShader = LoadShader( GL_FRAGMENT_SHADER, pSrc );
	if( fragmentShader == 0 ){
		LOGE( "@ CShader::setFromFile: INVALID FRAGMENT SHADER: %s\n", pFragmentSourceName );
		return( false );
	}

	return( setFromId( vertexShader, fragmentShader ) );
}

//---------------------------
// ＩＤから設定
//---------------------------
bool CShader::setFromId( uint32 glVertexId, uint32 glFragmentId ){
    // プログラム作成
    int program = glCreateProgram();
    if( program == 0 ){
        LOGE( "@ CShader::setFromId: CAN'T CREATE PROGRAM\n" );
        return( false );
    }
    
    // シェーダーにアタッチしてリンク
    glAttachShader( program, glVertexId );
    glAttachShader( program, glFragmentId );
    glLinkProgram( program );
    
    int linkStatus;
    glGetProgramiv( program, GL_LINK_STATUS, &linkStatus );
    if( linkStatus != GL_TRUE ){
        LOGE( "@ CShader::setFromId: CAN'T LINK PROGRAM\n" );
        return( false );
    }
    
    // 属性＆ユニフォームの取得
    int aXY = glGetAttribLocation( program, "aXY" );
    int aUV = glGetAttribLocation( program, "aUV" );
    int uDepth = glGetUniformLocation( program, "uDepth" );
    int uColor = glGetUniformLocation( program, "uColor" );
    int uApplyRate = glGetUniformLocation( program, "uApplyRate" );
    int uTex = glGetUniformLocation( program, "uTexture" );
    int uTexP = glGetUniformLocation( program, "uTextureP" );
    
    //-------------------------------
    // シェーダーの事前設定（※一回でよいもの）
    //-------------------------------
    glUseProgram( program );
    
    // アトリビュート有効化
    if( aXY != -1 ){ glEnableVertexAttribArray( aXY ); }
    if( aUV != -1 ){ glEnableVertexAttribArray( aUV ); }
    
    // テクスチャのバインド
    if( uTex != -1 ){ glUniform1i( uTex, TEX_BIND_ID_FOR_TEX ); }
    if( uTexP != -1 ){ glUniform1i( uTexP, TEX_BIND_ID_FOR_TEX_P ); }
    
    //---------------------------
    // ここまできたら有効
    //---------------------------
    m_nProgram = program;
    m_nVertexShader = glVertexId;
    m_nFragmentShader = glFragmentId;
    m_aXY = aXY;
    m_aUV = aUV;
    m_uDepth = uDepth;
    m_uColor = uColor;
    m_uApplyRate = uApplyRate;
    m_uTex = uTex;
    m_uTexP = uTexP;
    
    return( true );
}

//---------------------
// 頂点情報設定：矩形
//---------------------
void CShader::pushRect( float x, float y, float w, float h ){
	// 座標設定＆画面内チェック
	if( setVertexXYThenCheck( x, y, w, h ) ){
		// 登録に成功したら使用数に＋４（矩形分）
		s_nVertexUse += 4;
	}
}

//-----------------------------------------------------------
// 頂点情報設定：テクスチャ（※その都度テクスチャ情報とUVから計算する）
//-----------------------------------------------------------
void CShader::pushTex( CTex* pTex, float x, float y, float w, float h, float u, float v, float uW, float vH ){
	// 座標設定＆画面内チェック
	if( setVertexXYThenCheck( x, y, w, h ) ){
		// UV登録
        if( setVertexUV( u/pTex->getWidth(), v/pTex->getHeight(), (u+uW)/pTex->getWidth(), (v+vH)/pTex->getHeight() ) ){
            // 登録に成功したら使用数に＋４（矩形分）
            s_nVertexUse += 4;
        }
	}
}

//------------------------------------------------------------------
// 頂点情報設定：スプライト（※スプライトの保持する値を参照する）
//------------------------------------------------------------------
void CShader::pushSpr( CSpr* pSpr, float x, float y, float scaleW, float scaleH ){
	float w = scaleW * pSpr->getWidth();
	float h = scaleH * pSpr->getHeight();

	// 座標設定＆画面内チェック
	if( setVertexXYThenCheck( x, y, w, h ) ){
		// UV登録
        if( setVertexUV( pSpr->getU0(), pSpr->getV0(), pSpr->getU1(), pSpr->getV1() ) ){
            // 登録に成功したら使用数に＋４（矩形分）
            s_nVertexUse += 4;
        }
	}
}

//------------------------------------------------------------------
// 頂点情報設定：スプライト（※スプライトの保持する値を参照する）
//------------------------------------------------------------------
void CShader::pushSprWithUVScale( CSpr* pSpr, float x, float y, float scaleW, float scaleH ){
	y += (1.0f-scaleH) * pSpr->getHeight();
	float w = scaleW * pSpr->getWidth();
	float h = scaleH * pSpr->getHeight();

	// 座標設定＆画面内チェック
	if( setVertexXYThenCheck( x, y, w, h ) ){
		// UV登録
        if( setVertexUV( pSpr->getU0(), pSpr->getV1()-(pSpr->getV1()-pSpr->getV0())*scaleH, pSpr->getU0()+(pSpr->getU1()-pSpr->getU0())*scaleW, pSpr->getV1() ) ){
            // 登録に成功したら使用数に＋４（矩形分）
            s_nVertexUse += 4;
        }
	}
}

//--------------
// 頂点登録
//--------------
void CShader::pushVertex( float x, float y, float u, float v ){
    if( setVertexXYUV( x, y, u, v ) ){
        // 登録に成功したら使用数に＋１（頂点分）
        s_nVertexUse += 1;
    }
}

//-----------------------------------------------------------------------------
// 頂点バッファへの登録（※Ｙ軸をOpenGL系とは逆の下向きに調整している点に注意）
//-----------------------------------------------------------------------------
bool CShader::setVertexXYThenCheck( float x, float y, float w, float h ){
    // このタイミングでゲーム座標を実座標に調整
    if( m_bRenderForGame ){
        // 指定があればセーフエリア調整
        if( m_bRenderForSafeArea ){
            x += SA_OFS_X;
            y += SA_OFS_Y;
        }
        
        x *= GM_P_RATE;
        y *= GM_P_RATE;
        w *= GM_P_RATE;
        h *= GM_P_RATE;
    }
    
    // 指定があれば整数調整
    if( m_bRenderWithIntPosition ){
        x = (int)x;
        y = (int)y;
    }
    
    float x0, y0, x1, y1;

    //----------------------------
    // 回転しない場合
    //----------------------------
    if( m_fRotation == 0.0f ){
        // 座標設定＆画面に収まっていなければ無視
        x0 = (x-s_fCurRenderAreaWH)/s_fCurRenderAreaWH;
        if( x0 > s_fCurRenderAreaX1 ){ return( false ); }
        
        y0 = (s_fCurRenderAreaHH-y)/s_fCurRenderAreaHH;
        if( y0 < s_fCurRenderAreaY1 ){ return( false ); }
        
        x1 = x0 + w/s_fCurRenderAreaWH;
        if( x1 < s_fCurRenderAreaX0 ){ return( false ); }
        
        y1 = y0 - h/s_fCurRenderAreaHH;
        if( y1 > s_fCurRenderAreaY0 ){ return( false ); }
        
        // 座標に設定
        int ofs = s_nVertexUse * VERTEX_XY_IN_DRAW;
        s_fArrVertexXY[ofs+0] = x0;        s_fArrVertexXY[ofs+1] = y0;
        s_fArrVertexXY[ofs+2] = x0;        s_fArrVertexXY[ofs+3] = y1;
        s_fArrVertexXY[ofs+4] = x1;        s_fArrVertexXY[ofs+5] = y0;
        s_fArrVertexXY[ofs+6] = x1;        s_fArrVertexXY[ofs+7] = y1;
        
        return( true );
    }
    
    //----------------------------
    // 回転している場合
    //----------------------------
    // 回転中心の調整
    float rotX = m_fRotX;
    float rotY = m_fRotY;
    if( m_bRenderForGame ){
        if( m_bRenderForSafeArea ){
            rotX += SA_OFS_X;
            rotY += SA_OFS_Y;
        }
        
        rotX *= GM_P_RATE;
        rotY *= GM_P_RATE;
    }

    // 回転はゲーム内座標で行う（※先にGL系に変換すると画面比の関係で結果が狂う）
    x0 = x - rotX;
    y0 = y - rotY;
    x1 = x0 + w;
    y1 = y0 + h;
    
    int ofs = s_nVertexUse * VERTEX_XY_IN_DRAW;
    float cos = CMath::GetCos( m_fRotation );
    float sin = CMath::GetSin( m_fRotation );
    
    //-----------------------------------
    // 座標設定＆領域確認：左右
    //-----------------------------------
    // 回転
    s_fArrVertexXY[ofs+0] = x0 * cos - y0 * sin;
    s_fArrVertexXY[ofs+2] = x0 * cos - y1 * sin;
    s_fArrVertexXY[ofs+4] = x1 * cos - y0 * sin;
    s_fArrVertexXY[ofs+6] = x1 * cos - y1 * sin;
    
    // X座標をGL系に
    s_fArrVertexXY[ofs+0] = ((s_fArrVertexXY[ofs+0]+rotX)-s_fCurRenderAreaWH)/s_fCurRenderAreaWH;
    s_fArrVertexXY[ofs+2] = ((s_fArrVertexXY[ofs+2]+rotX)-s_fCurRenderAreaWH)/s_fCurRenderAreaWH;
    s_fArrVertexXY[ofs+4] = ((s_fArrVertexXY[ofs+4]+rotX)-s_fCurRenderAreaWH)/s_fCurRenderAreaWH;
    s_fArrVertexXY[ofs+6] = ((s_fArrVertexXY[ofs+6]+rotX)-s_fCurRenderAreaWH)/s_fCurRenderAreaWH;
    
    // 左右の境界の検出
    float xL = s_fArrVertexXY[ofs+0];
    float xR = s_fArrVertexXY[ofs+0];
    
    if( xL > s_fArrVertexXY[ofs+2] ){ xL = s_fArrVertexXY[ofs+2];}
    if( xR < s_fArrVertexXY[ofs+2] ){ xR = s_fArrVertexXY[ofs+2];}
    if( xL > s_fArrVertexXY[ofs+4] ){ xL = s_fArrVertexXY[ofs+4];}
    if( xR < s_fArrVertexXY[ofs+4] ){ xR = s_fArrVertexXY[ofs+4];}
    if( xL > s_fArrVertexXY[ofs+6] ){ xL = s_fArrVertexXY[ofs+6];}
    if( xR < s_fArrVertexXY[ofs+6] ){ xR = s_fArrVertexXY[ofs+6];}
    
    // 画面に収まっていなければ無視
    if( xL > s_fCurRenderAreaX1 ){ return( false ); }
    if( xR < s_fCurRenderAreaX0 ){ return( false ); }
    
    //-----------------------------------
    // 座標設定＆領域確認：上下
    //-----------------------------------
    // 回転
    s_fArrVertexXY[ofs+1] = x0 * sin + y0 * cos;
    s_fArrVertexXY[ofs+3] = x0 * sin + y1 * cos;
    s_fArrVertexXY[ofs+5] = x1 * sin + y0 * cos;
    s_fArrVertexXY[ofs+7] = x1 * sin + y1 * cos;
    
    // Y座標をGL系に
    s_fArrVertexXY[ofs+1] = (s_fCurRenderAreaHH-(s_fArrVertexXY[ofs+1]+rotY))/s_fCurRenderAreaHH;
    s_fArrVertexXY[ofs+3] = (s_fCurRenderAreaHH-(s_fArrVertexXY[ofs+3]+rotY))/s_fCurRenderAreaHH;
    s_fArrVertexXY[ofs+5] = (s_fCurRenderAreaHH-(s_fArrVertexXY[ofs+5]+rotY))/s_fCurRenderAreaHH;
    s_fArrVertexXY[ofs+7] = (s_fCurRenderAreaHH-(s_fArrVertexXY[ofs+7]+rotY))/s_fCurRenderAreaHH;
    
    // 上下の境界の検出
    float yT = s_fArrVertexXY[ofs+1];
    float yB = s_fArrVertexXY[ofs+1];
    
    if( yT < s_fArrVertexXY[ofs+3] ){ yT = s_fArrVertexXY[ofs+3];}
    if( yB > s_fArrVertexXY[ofs+3] ){ yB = s_fArrVertexXY[ofs+3];}
    if( yT < s_fArrVertexXY[ofs+5] ){ yT = s_fArrVertexXY[ofs+5];}
    if( yB > s_fArrVertexXY[ofs+5] ){ yB = s_fArrVertexXY[ofs+5];}
    if( yT < s_fArrVertexXY[ofs+7] ){ yT = s_fArrVertexXY[ofs+7];}
    if( yB > s_fArrVertexXY[ofs+7] ){ yB = s_fArrVertexXY[ofs+7];}
    
    // 画面に収まっていなければ無視
    if( yT < s_fCurRenderAreaY1 ){ return( false ); }
    if( yB > s_fCurRenderAreaY0 ){ return( false ); }
    
    return( true );
}

//---------------------
// UVバッファへの登録
//---------------------
bool CShader::setVertexUV( float u0, float v0, float u1, float v1 ){
    if( m_bFlipX ){
        float temp = u0;
        u0 = u1;
        u1 = temp;
    }
    
    if( m_bFlipY ){
        float temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    // 登録
    int ofs = s_nVertexUse * VERTEX_UV_IN_DRAW;
    s_fArrVertexUV[ofs+0] = u0;    s_fArrVertexUV[ofs+1] = v0;
    s_fArrVertexUV[ofs+2] = u0;    s_fArrVertexUV[ofs+3] = v1;
    s_fArrVertexUV[ofs+4] = u1;    s_fArrVertexUV[ofs+5] = v0;
    s_fArrVertexUV[ofs+6] = u1;    s_fArrVertexUV[ofs+7] = v1;
    
    return( true );
}

//--------------
// 頂点設定
//--------------
bool CShader::setVertexXYUV( float x, float y, float u, float v ){
    // このタイミングでゲーム座標を実座標に調整）
    if( m_bRenderForGame ){
        // 指定があればセーフエリア調整
        if( m_bRenderForSafeArea ){
            x += SA_OFS_X;
            y += SA_OFS_Y;
        }
        
        x *= GM_P_RATE;
        y *= GM_P_RATE;
    }
    
    // 指定があれば整数調整
    if( m_bRenderWithIntPosition ){
        x = (int)x;
        y = (int)y;
    }
    
    // XY登録
    int ofs = s_nVertexUse * VERTEX_XY_IN_DRAW;
    s_fArrVertexXY[ofs+0] = (x-s_fCurRenderAreaWH)/s_fCurRenderAreaWH;
    s_fArrVertexXY[ofs+1] = (s_fCurRenderAreaHH-y)/s_fCurRenderAreaHH;
    
    // UV登録
    ofs = s_nVertexUse * VERTEX_UV_IN_DRAW;
    s_fArrVertexUV[ofs+0] = u;
    s_fArrVertexUV[ofs+1] = v;
 
    return( true );
}

//----------------------------
// 描画
//----------------------------
void CShader::render( CTex* pTex, CTex* pTexP ){
    // 無効は無視
    if( s_nVertexUse <= 0 ){ return; }
    
    // プログラム設定
    if( s_nLastProgram != m_nProgram ){
        glUseProgram( m_nProgram );
        s_nLastProgram = m_nProgram;
    }
    
    // XY設定
    if( m_aXY != -1 ){
        glVertexAttribPointer( m_aXY, 2, GL_FLOAT, false, VERTEX_XY_STRIDE, s_fArrVertexXY );
    }
    
    // UV 設定
    if( m_aUV != -1 ){
        glVertexAttribPointer( m_aUV, 2, GL_FLOAT, false, VERTEX_UV_STRIDE, s_fArrVertexUV );
    }
    
    // デプス設定
    if( m_uDepth != -1 ){
        glUniform1f( m_uDepth, m_fDepth );
    }
    
    // 色設定設定
    if( m_uColor != -1 ){
        glUniform4f( m_uColor, m_fColR, m_fColG, m_fColB, m_fColA );
    }
    
    // 適用率設定
    if( m_uApplyRate != -1 ){
        glUniform1f( m_uApplyRate, m_fApplyRate );
    }
    
    // 画素設定
    if( m_uTex != -1 ){
        if( s_nLastBindTex != pTex->getGlTexture() ){
            glActiveTexture( TEX_BIND_TARGET_FOR_TEX );
            glBindTexture( GL_TEXTURE_2D, pTex->getGlTexture() );
            s_nLastBindTex = pTex->getGlTexture();
        }
    }
    
    // パレット
    if( m_uTexP != -1 ){
        if( s_nLastBindTexP != pTexP->getGlTexture() ){
            glActiveTexture( TEX_BIND_TARGET_FOR_TEX_P );
            glBindTexture( GL_TEXTURE_2D, pTexP->getGlTexture() );
            s_nLastBindTexP = pTexP->getGlTexture();
        }
    }

    //---------------------------------------------
    // 頂点を転送して描画
    //---------------------------------------------
    glDrawArrays( GL_TRIANGLE_STRIP, 0, s_nVertexUse );

    // 使用数をクリアしておく
    s_nVertexUse = 0;
}
