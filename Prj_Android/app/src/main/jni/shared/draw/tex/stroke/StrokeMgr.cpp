/*+----------------------------------------------------------------+
  |	Title:		StrokeMgr.cpp [共通環境]
  |	Comment:	ストローク管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "StrokeMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ログ指定
//#define DUMP_STROKE_DOT_SRC_LOG

// [参照値〜サイズ値]の相互変換
#define CONV_AT_TO_SIZE0( _at ) ((1.0f+(_at))/STROKE_DOT_SIZE_PRECISION)
#define CONV_SIZE0_TO_AT( _s0 ) ((int)((_s0)*STROKE_DOT_SIZE_PRECISION-1.0f))

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
CStroke CStrokeMgr::s_oCurStroke;

CStrokeDot* CStrokeMgr::s_oArrStrokeDotForSquare;
CStrokeDot* CStrokeMgr::s_oArrStrokeDotForCircle;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------------------------
// 確保数（※管理内で[new]するセル数）
//--------------------------------------
int CStrokeMgr::GetAllocCellNum( void ){
    int num = 0;
    
    //---------------------------------------------------------------------------
    // s_oArrStrokeDotForSquare = new CStrokeDot[numAllocDot];
    // s_oArrStrokeDotForCircle = new CStrokeDot[numAllocDot];
    //----------------------------------------------------------------------------
    int numAllocDot = STROKE_DOT_ALLOC_SIZE_MAX * STROKE_DOT_SIZE_PRECISION;
    num += 2 * (1 + numAllocDot*CStrokeDot::GetAllocCellNum() );

    return( num );
}

//--------------------------------------
// 確保サイズ（※管理内で[new]する領域サイズ）
//--------------------------------------
int CStrokeMgr::GetAllocSizeK( void ){
    int size = 0;

    //-----------------------------------------------------------------------------
    // int numAllocDot = STROKE_DOT_ALLOC_SIZE_MAX * STROKE_DOT_SIZE_PRECISION;
    // s_oArrStrokeDotForSquare = new CStrokeDot[numAllocDot];
    // s_oArrStrokeDotForCircle = new CStrokeDot[numAllocDot];
    //-----------------------------------------------------------------------------
    for( int i=0; i<STROKE_DOT_ALLOC_SIZE_MAX; i++ ){
        for( int j=0; j<STROKE_DOT_SIZE_PRECISION; j++ ){
            float sizeSrc = CONV_AT_TO_SIZE0( i*STROKE_DOT_SIZE_PRECISION + j );
            size += 2 * (sizeof(CStrokeDot) + CStrokeDot::GetAllocSize( sizeSrc ));
        }
    }
    
    // キロ単位にして返す
    int sizeK = (size+KB-1)/KB;
    return( sizeK );
}

//------------------------
// 初期化
//------------------------
bool CStrokeMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CStrokeMgr::OnCreate()\n" );
#endif

    // ストローク画素確保数（※ピクセル率で増減する）
    int numAllocDot = STROKE_DOT_ALLOC_SIZE_MAX * STROKE_DOT_SIZE_PRECISION;

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_STROKE );
    s_oArrStrokeDotForSquare = new CStrokeDot[numAllocDot];
    s_oArrStrokeDotForCircle = new CStrokeDot[numAllocDot];
	CMemMgr::PopTargetField();

    //--------------------------------------------
    // ストローク画素の作成（※ワークなので[APP]から確保）
    //--------------------------------------------
    // ワーク確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    int sizeMax = numAllocDot/STROKE_DOT_SIZE_PRECISION + 1;
    BYTE* pSrc = new BYTE[sizeMax*sizeMax];
    CMemMgr::PopTargetField();

    // 全サイズ分のストローク画素を作成
    int size;
    float size0;
    for( int i=0; i<numAllocDot; i++ ){
        size0 = CONV_AT_TO_SIZE0( i );

        // SQUARE
        size = SetDotSrcForSquare( pSrc, size0 );
        s_oArrStrokeDotForSquare[i].createDot( pSrc, size );
        
        // CIRCLE
        size = SetDotSrcForCircle( pSrc, size0 );
        s_oArrStrokeDotForCircle[i].createDot( pSrc, size );
    }

    // ワーク開放
    SAFE_DELETE_ARRAY( pSrc );
    
	return( true );
}

//------------------------
// 終了
//------------------------
void CStrokeMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CStrokeMgr::OnDestroy()\n" );
#endif

	// 解放
    SAFE_DELETE_ARRAY( s_oArrStrokeDotForSquare );
    SAFE_DELETE_ARRAY( s_oArrStrokeDotForCircle );
}

//--------------------------
// ストローク取得
//--------------------------
CStroke* CStrokeMgr::GetStroke( eSTROKE_TYPE type, float size, float range ){
    if( s_oCurStroke.set( type, size, range ) ){
        return( &s_oCurStroke );
    }
    
    return( NULL );
}

//------------------------------------------
// ストローク画素配列から対応サイズのデータを取得する
//------------------------------------------
CStrokeDot* CStrokeMgr::GetStrokeDot( float size0, CStrokeDot* pArrStrokeDot ){
    int at = CONV_SIZE0_TO_AT( size0 );

    // 用心：指定が小さすぎる場合は下限
    if( at < 0 ){
        LOGE( "@ CStrokeMgr::GetStrokeDot: TOO SMALL SIZE: size0=%f, at=%d\n", size0, at );
        at = 0;
    }

    // 用心：指定が大きすぎる場合は上限
    if( at >= STROKE_DOT_ALLOC_SIZE_MAX*STROKE_DOT_SIZE_PRECISION ){
        LOGE( "@ CStrokeMgr::GetStrokeDot: TOO HUGE SIZE: size0=%f, at=%d\n", size0, at );
        at = STROKE_DOT_ALLOC_SIZE_MAX*STROKE_DOT_SIZE_PRECISION - 1;
    }

    return( &pArrStrokeDot[at] );
}

//------------------------------
// ストローク画素ソースの生成：矩形
//------------------------------
int CStrokeMgr::SetDotSrcForSquare( BYTE* pBuf, float size0 ){
    int size = CStrokeDot::FixDotSize( size0 );

    // 色を置く
    for( int i=0; i<size; i++ ){
        for( int j=0; j<size; j++ ){
            pBuf[size*i+j] = STROKE_DOT_OUT_PRECISION_MAX;
        }
    }
    
    // 端数があれば調整（※右下にはみ出させておく）
    BYTE outVal = (BYTE)(STROKE_DOT_OUT_PRECISION_MAX*(size0-((int)size0)));
    if( outVal > 0 ){
        for( int i=0; i<size; i++ ){
            pBuf[i*size + size-1] = outVal;     // 右端
            pBuf[(size-1)*size + i] = outVal;   // 下端
        }
    }
    
#ifdef DUMP_STROKE_DOT_SRC_LOG
    // check
    LOGD( "@ CStrokeMgr::SetDotSrcForSquare: size0=%f, pBuf[%dx%d] = {\n", size0, size, size );
    for( int i=0; i<size; i++ ){
        LOGD( "  " );
        for( int j=0; j<size; j++ ){
            LOGD( "%02X ", pBuf[size*i+j] );
        }
        LOGD( "\n" );
    }
    LOGD( " }\n" );
#endif
    
    return( size );
}

//------------------------------
// ストローク画素ソースの生成：円
//------------------------------
int CStrokeMgr::SetDotSrcForCircle( BYTE* pBuf, float size0 ){
    int size = CStrokeDot::FixDotSize( size0 );

    //--------------------------------------------------------------------------
    // 中心からの距離に応じて画素を配置
    //（※太さが３未満の場合に値の強弱が前後するが、これはこれで直線の感じが出ているのでヨシとする）
    //--------------------------------------------------------------------------
    BYTE val;
    float len, outLen;
    float r = size0 / 2.0f;
    float border = 1.0f;
    
    for( int i=0; i<size; i++ ){
        for( int j=0; j<size; j++ ){
            len = CMath::Len( (j+0.5f-r), (i+0.5f-r) ); // 円の中心からの距離
            outLen = (len+border) - r;                  // 円の中心からはみ出した距離
            
            // はみ出し具合で出力の強さを変える
            if( outLen <= 0.0f ){
                val = STROKE_DOT_OUT_PRECISION_MAX;
            }else if( outLen < border ){
                val = (BYTE)(STROKE_DOT_OUT_PRECISION_MAX*(border-outLen)/border);
            }else/* if( outLen >= border )*/{
                val = 0x00;
            }
            pBuf[size*i+j] = val;
        }
    }
    
#ifdef DUMP_STROKE_DOT_SRC_LOG
    // check
    LOGD( "@ CStrokeMgr::SetDotSrcForCircle: size0=%f, pBuf[%dx%d] = {\n", size0, size, size );
    for( int i=0; i<size; i++ ){
        LOGD( "  " );
        for( int j=0; j<size; j++ ){
            LOGD( "%02X ", pBuf[size*i+j] );
        }
        LOGD( "\n" );
    }
    LOGD( " }\n" );
#endif
    
    return( size );
}
