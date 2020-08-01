/*+----------------------------------------------------------------+
  | Title:      StrokeDot.hpp [共通環境]
  | Comment:    ストローク画素（※ストロークの描画に利用される画素部分）
  |             １ピクセル未満のはみ出し段階に応じた画像を総当たりでもつ仕組み
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "StrokeDot.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ログ指定
//#define DUMP_STROKE_DOT_CREATE_LOG
//#define DUMP_STROKE_DOT_FIX_LOG

// データチェック（※オフにしておく＝正常系でしか動かない前提）
//#define CHECK_DATA_ACCESS

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------------------------
// 確保メモリセル数（※インスタンス内で[new]するセル数）
//----------------------------------------------
int CStrokeDot::GetAllocCellNum( void ){
    int num = 0;
    
    //--------------------------------------------------------------------
    // m_pBufDot = new BYTE[STROKE_DOT_OUT_PRECISION_MAX*dotBufSize];
    // m_pArrDot = new BYTE*[STROKE_DOT_OUT_PRECISION_MAX];
    //--------------------------------------------------------------------
    num += 2;
    
    return( num );
}

//-----------------------------------------------
// 確保メモリサイズ算出（※インスタンス内で[new]するサイズ）
//-----------------------------------------------
int CStrokeDot::GetAllocSize( float size0 ){
    int allocSize = 0;
    
    //--------------------------------------------------------------------
    // m_pBufDot = new BYTE[STROKE_DOT_OUT_PRECISION_MAX*dotBufSize];
    //--------------------------------------------------------------------
    int size = FixDotSize( size0 );
    int sizeDot = size + 1;             // 確保サイズは調整後に余白分の＋１
    int dotBufSize = sizeDot * sizeDot;
    allocSize += sizeof(BYTE) * STROKE_DOT_OUT_PRECISION_MAX * dotBufSize;
    
    //--------------------------------------------------------------------
    // m_pArrDot = new BYTE*[STROKE_DOT_OUT_PRECISION_MAX];
    //--------------------------------------------------------------------
    allocSize += sizeof(BYTE*) * STROKE_DOT_OUT_PRECISION_MAX;
    
    return( allocSize );
}

//----------------------------------
// サイズ調整（※浮動小数の端数を加味する）
//----------------------------------
int CStrokeDot::FixDotSize( float size0 ){
    int size = (int)size0;
    
    // はみ出しがあれば＋１
    if( (size0-size) > 0.0f ){
        size += 1;
    }
    
    return( size );
}

//-------------------
// コンストラクタ
//-------------------
CStrokeDot::CStrokeDot( void ){
    clear();
}

//-------------------
// デストラクタ
//-------------------
CStrokeDot::~CStrokeDot( void ){
    clear();    // 領域解放
}

//-------------------
// クリア
//-------------------
void CStrokeDot::clear( void ){
    m_nSizeDot = 0;

    SAFE_DELETE_ARRAY( m_pBufDot );
    SAFE_DELETE_ARRAY( m_pArrDot );
}

//---------------------------------------------------
// 画素の取得（※はみ出し具合に応じた画素を返す）
// [rateOutX/rateOutY]の値は[0.0以上、1.0未満)である想定
//---------------------------------------------------
BYTE* CStrokeDot::getDot( float rateOutX, float rateOutY ){
    int outX = (int)(rateOutX * STROKE_DOT_OUT_PRECISION_W);
    int outY = (int)(rateOutY * STROKE_DOT_OUT_PRECISION_H);
    int dotAt = outY*STROKE_DOT_OUT_PRECISION_W + outX;

#ifdef CHECK_DATA_ACCESS
    // 用心
    if( dotAt < 0 || dotAt >= STROKE_DOT_OUT_PRECISION_MAX ){
        LOGE( "@ CStrokeDot::getDot:: INVALID RATE OUT: rateOutX=%f, rateOutY=%f\n", rateOutX, rateOutY );
        
        // ごかましておく
        dotAt = STROKE_DOT_OUT_PRECISION_MAX-1;
    }
#endif
    
    return( m_pArrDot[dotAt] );
}

//---------------------------
// ストローク画素作成
//---------------------------
bool CStrokeDot::createDot( BYTE* pSrc, int sizeSrc ){
    // 用心にクリア
    clear();

    // ドットサイズは余白分＋１する
    m_nSizeDot = sizeSrc + 1;
    
    //------------------------------------------------------
    // 領域確保
    //------------------------------------------------------
    int dotBufSize = m_nSizeDot * m_nSizeDot;
    
    CMemMgr::PushTargetField( eMEM_FIELD_D_STROKE );
    m_pBufDot = new BYTE[STROKE_DOT_OUT_PRECISION_MAX*dotBufSize];
    m_pArrDot = new BYTE*[STROKE_DOT_OUT_PRECISION_MAX];
    CMemMgr::PopTargetField();
    
    // バッファを精度段階に応じて配列に割り当てておく
    for( int i=0; i<STROKE_DOT_OUT_PRECISION_MAX; i++ ){
        m_pArrDot[i] = &m_pBufDot[i*dotBufSize];
    }

    //--------------------------------------------------
    // ワーク確保 ＆ ソースのコピー（※ワークなので[APP]から確保）
    //--------------------------------------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    BYTE* pBufWork = new BYTE[dotBufSize];
    CMemMgr::PopTargetField();

    // ソースのコピー
    for( int i=0; i<sizeSrc; i++ ){
        for( int j=0; j<sizeSrc; j++ ){
            pBufWork[m_nSizeDot*i+j] = pSrc[sizeSrc*i+j];
        }
    }
    
    // 余白のクリア
    for( int i=0; i<m_nSizeDot; i++ ){
        pBufWork[m_nSizeDot*i + (m_nSizeDot-1)] = 0x00;   // 右端
        pBufWork[m_nSizeDot*(m_nSizeDot-1) + i] = 0x00;   // 下端
    }
    
#ifdef DUMP_STROKE_DOT_CREATE_LOG
    // check
    LOGD( "@ createDot: pDot[(%d+1)x(%d+1)] = {\n", sizeSrc, sizeSrc );
    for( int i=0; i<m_nSizeDot; i++ ){
        LOGD( "  " );
        for( int j=0; j<m_nSizeDot; j++ ){
            LOGD( "%02X ", pBufWork[m_nSizeDot*i+j] );
        }
        LOGD( "\n" );
    }
    LOGD( " }\n" );
#endif

    //--------------------------------------
    // はみ出し割合毎のデータの作成
    //--------------------------------------
    for( int i=0; i<STROKE_DOT_OUT_PRECISION_H; i++ ){
        for( int j=0; j<STROKE_DOT_OUT_PRECISION_W; j++ ){
            fixDot( m_pArrDot[STROKE_DOT_OUT_PRECISION_W*i+j], m_nSizeDot, j, i, pBufWork );
        }
    }

    // ワーク開放
    SAFE_DELETE_ARRAY( pBufWork );
    
    return( true );
}

//---------------------------------------------------------------
// 画素データ確定
// [pSrc]で指定される画像をはみ出し値[outX/outY]で調整したデータを作成する
//---------------------------------------------------------------
void CStrokeDot::fixDot( BYTE* pDot, int sizeDot, int outX, int outY, BYTE* pSrc ){
    int inX = STROKE_DOT_OUT_PRECISION_W - outX; // はみ出していない値：X
    int inY = STROKE_DOT_OUT_PRECISION_H - outY; // はみ出していない値：Y

    // １ピクセルに対するドット抽出割合（※あるドットを調べる際、そのドットから左上(-1,-1)方向へのはみ出しを加味する考え）
    int rate11 = outY * outX;   // 左上[i=-1/j=-1]ドットの割合
    int rate10 = outY * inX;    // 上[i=-1/j=0]ドットの割合
    int rate01 = inY * outX;    // 左[i=0/j=-1]ドットの割合
    int rate00 = inY * inX;     // 中央[i=0/j=0]ドットの割合

    int val11, val10, val01, val00;
    int val;

    // 値の敷き詰め（※左上[val11]へのはみ出しが有効な領域）
    for( int i=1; i<sizeDot; i++ ){
        for( int j=1; j<sizeDot; j++ ){
            val11 = rate11*pSrc[sizeDot*(i-1)+(j-1)];  // 左上からはみ出てきた部分
            val10 = rate10*pSrc[sizeDot*(i-1)+j];      // 上からはみ出てきた部分
            val01 = rate01*pSrc[sizeDot*i+(j-1)];      // 左からはみ出てきた部分
            val00 = rate00*pSrc[sizeDot*i+j];          // はみ出ていない部分
            val = val11 + val10 + val01 + val00;
            pDot[sizeDot*i+j] = (BYTE)((val11 + val10 + val01 + val00)/STROKE_DOT_OUT_PRECISION_MAX);
        }
    }
    
    // 左端（※上[val10]へのはみ出しのみ有効な領域）
    for( int i=1; i<sizeDot; i++ ){
        val10 = rate10*pSrc[sizeDot*(i-1)+0];  // 上からはみ出てきた部分
        val00 = rate00*pSrc[sizeDot*i+0];      // はみ出ていない部分
        pDot[sizeDot*i+0] = (BYTE)((val10 + val00)/STROKE_DOT_OUT_PRECISION_MAX);
    }
    
    // 上端（※左[val01]へのはみ出しのみ有効な領域）
    for( int i=1; i<sizeDot; i++ ){
        val01 = rate01*pSrc[i-1];   // 左からはみ出てきた部分
        val00 = rate00*pSrc[i];     // はみ出ていない部分
        pDot[i] = (BYTE)((val01 + val00)/STROKE_DOT_OUT_PRECISION_MAX);
    }

    // 左上端（※はみ出しの影響がない領域）
    val00 = rate00*pSrc[0]; // はみ出ていない部分
    pDot[0] = (BYTE)(val00/STROKE_DOT_OUT_PRECISION_MAX);

#ifdef DUMP_STROKE_DOT_FIX_LOG
    LOGD( "@ fixDot(%d/%d): pDot[%dx%d] = {\n", outX, outY, sizeDot, sizeDot );
    for( int i=0; i<sizeDot; i++ ){
        LOGD( "  " );
        for( int j=0; j<sizeDot; j++ ){
            LOGD( "%02X ", pDot[sizeDot*i+j] );
        }
        LOGD( "\n" );
    }
    LOGD( " }\n" );
#endif
}
