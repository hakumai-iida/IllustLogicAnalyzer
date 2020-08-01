/*+----------------------------------------------------------------+
  |	Title:		BezierForFill.cpp [共通環境]
  |	Comment:	ベジェ曲線：塗り
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------------------------------------------------------------
// 塗り処理の共通部分（ストローク準備）
//----------------------------------------------------------------------------------------------
#define FILL_COMMON_STROKE                                                                      \
    int strokeSize = (int)pFP->optionStrokeSize;                                                \
    if( strokeSize <= 0 ){                                                                      \
        return;                                                                                 \
    }                                                                                           \
    int strokeRange = (int)pFP->optionStrokeRange;                                              \
    CStroke* pStroke = CStrokeMgr::GetStroke( eSTROKE_TYPE_CIRCLE, strokeSize, strokeRange );   \
    if( pStroke == NULL ){                                                                      \
        return;                                                                                 \
    }                                                                                           \
    int size = (int)pStroke->getSize();

//----------------------------------------------------------------------------------
// ポイント処理の共通部分（開始）
//----------------------------------------------------------------------------------
#define POINT_COMMON_START                                                          \
    /* 座標調整（※ドットの中央に描画されるように）*/                                       \
    x -=  size/2.0f;                                                                \
    y -=  size/2.0f;                                                                \
    /* 座標のはみ出し具合の算出 */                                                       \
    int iX0 = (int)x;                                                               \
    int iY0 = (int)y;                                                               \
    float rateOutX = x - iX0;                                                       \
    float rateOutY = y - iY0;                                                       \
    /* ストローク画像取得＆描画 */                                                       \
    stSTROKE_OUTPUT_CELL* pCell = pStroke->getStrokeCell( rateOutX, rateOutY );     \
    BYTE* pDot = pCell->pBuf;                                                       \
    int w = pCell->sizeW;                                                           \
    int h = pCell->sizeH;                                                           \
    int iX = iX0 + pCell->ofsX; /* １ピクセルを超えるオフセットの適用 */                   \
    int iY = iY0 + pCell->ofsY; /* １ピクセルを超えるオフセットの適用 */

//----------------------------------------------------------------------------------
// ポイント処理の共通部分（終了＝特に無し）
//----------------------------------------------------------------------------------
#define POINT_COMMON_END

//-----------------------------------------------------------------------------------
// アンチ処理の共通部分（開始）（※ここのタッチ座標は傾き適用後の値）
//-----------------------------------------------------------------------------------
#define ANTI_COMMON_START                                                            \
    int num = s_nArrTouchPointNum[touchId];                                          \
    float* arrX = s_fArrArrTouchPointX[touchId];                                     \
    float* arrY = s_fArrArrTouchPointY[touchId];                                     \
    /* 塗りバッファに対してストロークを描く（※色味の取り扱いが別なので[PutDotAt]は使えない）*/     \
    for( int target=0; target<num; target++ ){                                       \
        /* 座標調整（※ドットの中央に描画されるように）*/                                    \
        float x = arrX[target] - size/2.0f;                                          \
        float y = arrY[target] - size/2.0f;                                          \
        /* 座標のはみ出し具合の算出 */                                                    \
        int iX0 = (int)x;                                                            \
        int iY0 = (int)y;                                                            \
        float rateOutX = x - iX0;   /* 小数点以下がＸ方向のはみ出し */                    \
        float rateOutY = y - iY0;   /* 小数点以下がＹ方向のはみ出し */                    \
        /* ストローク画像取得＆描画 */                                                   \
        stSTROKE_OUTPUT_CELL* pCell = pStroke->getStrokeCell( rateOutX, rateOutY ); \
        BYTE* pDot = pCell->pBuf;                                                   \
        int w = pCell->sizeW;                                                       \
        int h = pCell->sizeH;                                                       \
        int iX = iX0 + pCell->ofsX; /* １ピクセルを超えるオフセットの適用 */               \
        int iY = iY0 + pCell->ofsY; /* １ピクセルを超えるオフセットの適用 */

//----------------------------------------------------------------------------------
// アンチ処理の共通部分（終了）
//----------------------------------------------------------------------------------
#define ANTI_COMMON_END                                                             \
    }

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
//-------------------------------
// [Fill] 描画実体：塗り
//-------------------------------
void CBezier::DrawFill( stBEZIER_FILL_POINT* pHead ){
    stBEZIER_FILL_POINT* pFP = pHead;
    
    while( pFP != NULL ){
        float x = pFP->x;
        float y = pFP->y;

        // ワークパスなら塗りつぶし位置の出力
        if( s_bWorkPath ){
            if( ! IsDispOnlyAnchorPoint() ){
                PutCrossForWorkPath( x, y, BEZIER_WP_PAINT_PAL + pFP->workPathPalOfsS );
            }
        }else{
            // 通常時ならフィルを取得して塗りつぶし
            // TODO:肌(顔)と髪のテストに対する補正は必要か？（※例えば髪のメッシュもテストに含まれるように調整すべきか？）
            CFill* pFill = CFillMgr::GetFill( pFP->type );
            BYTE palGrp, palVal;

            if( pFill != NULL ){
                switch( pFP->option ){
                // ポイント指定：明色
                case eFILL_OPTION_POINT_BRIGHT:
                    DrawFillPointBright( pFP, x, y, s_nTestPalGrp );
                    break;
                        
                // ポイント指定：暗色
                case eFILL_OPTION_POINT_DARK:
                    DrawFillPointDark( pFP, x, y, s_nTestPalGrp );
                    break;
                    
                // 広域指定：明色（※アンチできないので１段階のみにしておく）
                case eFILL_OPTION_AREA_BRIGHT:
                    pFill->fillAddForAdd( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, (int)x, (int)y, s_nTestPalGrp, -1 );
                    break;
                    
                // 広域指定：暗色（※アンチできないので１段階のみにしておく）
                case eFILL_OPTION_AREA_DARK:
                    pFill->fillAddForAdd( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, (int)x, (int)y, s_nTestPalGrp, +1 );
                    break;
                    
                // 全画素指定：明色（※アンチできないので１段階のみにしておく）
                case eFILL_OPTION_COLOR_BRIGHT:
                    pFill->fillAddForColor( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, pFP->optionOfsX, pFP->optionOfsY, -1 );
                    break;
                    
                // 全画素指定：暗色（※アンチできないので１段階のみにしておく）
                case eFILL_OPTION_COLOR_DARK:
                    pFill->fillAddForColor( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, pFP->optionOfsX, pFP->optionOfsY, +1 );
                    break;

                // マスク
                case eFILL_OPTION_MASK:
                    pFill->fillMask( s_pBufFillGuard, s_pBufFillGuide, s_nBufW, s_nBufH, (int)x, (int)y );
                    break;
                    
                // ここまできたら通常の塗り
                default:
                    // 塗りオプションによりパレット値の調整
                    palGrp = CDefTable::GetPalOfsValue( pFP->palOfsId );
                    palVal = BEZIER_PAL_VAL_BASE;
                    if( pFP->option == eFILL_OPTION_BRIGHT ){ palVal = BEZIER_PAL_VAL_MIN; }
                    else if( pFP->option == eFILL_OPTION_DARK ){ palVal = BEZIER_PAL_VAL_MAX; }
                    
                    pFill->fill( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, (int)x, (int)y, palGrp, palVal, s_nTestPalGrp );
                        
                    // 明暗予約であれば塗りの直後に適用（※下塗りバッファの状況が残っているうちに処理する）
                    if( pFP->option == eFILL_OPTION_RESERVE_BRIGHT ){
                        pFill->fillAddForReserve( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, pFP->optionOfsX, pFP->optionOfsY, -1 );
                    }else if( pFP->option == eFILL_OPTION_RESERVE_DARK ){
                        pFill->fillAddForReserve( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, pFP->optionOfsX, pFP->optionOfsY, +1 );
                    }
                    break;
                }
            }
        }
        // 次のポイントへ
        pFP = (stBEZIER_FILL_POINT*) pFP->pNext;
    }
}

//----------------------------------------------------------------
// [Fill] 描画実体：ポイント明色（※無効判定は呼び出し元で済んでいるものとする）
//----------------------------------------------------------------
void CBezier::DrawFillPointBright( stBEZIER_FILL_POINT* pFP, float x, float y, BYTE targetPalGrp ){
    FILL_COMMON_STROKE;
    POINT_COMMON_START;
    
    // 画素の出力
    for( int i=0; i<h; i++ ){
        for( int j=0; j<w; j++ ){
            //-----------------------------------
            // 画素が有効であれば（※出力に意味があれば）
            //-----------------------------------
            int dotAt = w*i + j;
            int subVal = ((BEZIER_PAL_VAL_BASE-BEZIER_PAL_VAL_MIN)*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;
            if( subVal > 0 ){
                //-----------------------------------
                // 対象パレットであれば
                //-----------------------------------
                int bufAt = s_nBufW*(iY+i) + (iX+j);
                int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( s_pBuf[bufAt] );
                if( palGrp >= BEZIER_PAL_GRP_HEAD && (targetPalGrp==0x00 || palGrp==targetPalGrp) ){
                    // ストロークの濃さにより対象画素を明るく（※値を減算）
                    int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( s_pBuf[bufAt] ) - subVal;
                    if( palVal < BEZIER_PAL_VAL_MIN ){
                        palVal = BEZIER_PAL_VAL_MIN;
                    }
                    
                    s_pBuf[bufAt] = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                }
            }
        }
    }
    
    POINT_COMMON_END;
}

//----------------------------------------------------------------
// [Fill] 描画実体：ポイント暗色（※無効判定は呼び出し元で済んでいるものとする）
//----------------------------------------------------------------
void CBezier::DrawFillPointDark( stBEZIER_FILL_POINT* pFP, float x, float y, BYTE targetPalGrp ){
    FILL_COMMON_STROKE;
    POINT_COMMON_START;
    
    // 画素の出力
    for( int i=0; i<h; i++ ){
        for( int j=0; j<w; j++ ){
            //-----------------------------------
            // 画素が有効であれば（※出力に意味があれば）
            //-----------------------------------
            int dotAt = w*i + j;
            int addVal = ((BEZIER_PAL_VAL_MAX-BEZIER_PAL_VAL_BASE)*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;
            if( addVal > 0 ){
                //-----------------------------------
                // 対象パレットであれば
                //-----------------------------------
                int bufAt = s_nBufW*(iY+i) + (iX+j);
                int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( s_pBuf[bufAt] );
                if( palGrp >= BEZIER_PAL_GRP_HEAD && (targetPalGrp==0x00 || palGrp==targetPalGrp) ){
                    // ストロークの濃さにより対象画素を明るく（※値を加算）
                    int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( s_pBuf[bufAt] ) + addVal;
                    if( palVal > BEZIER_PAL_VAL_MAX ){
                        palVal = BEZIER_PAL_VAL_MAX;
                    }
                    
                    s_pBuf[bufAt] = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                }
            }
        }
    }
    
    POINT_COMMON_END;
}

//-----------------------------------------------------------------------
// [Fill] 描画実体：塗りのアンチ（※タッチ対象座標に沿って塗りバッファにアンチを適用する）
//-----------------------------------------------------------------------
void CBezier::DrawFillAntiForTouch( stBEZIER_FILL_POINT* pHead ){
    // ワークパスであれば無視
    if( s_bWorkPath ){
        return;
    }
    
    stBEZIER_FILL_POINT* pFP = pHead;
    
    while( pFP != NULL ){
        // オプションのタッチ対象IDが有効なら
        eSTROKE_TOUCH_TARGET touchId = pFP->touchIdForOption;
        if( touchId >= 0 && touchId < eSTROKE_TOUCH_TARGET_MAX ){
            // オプションに応じた処理の呼び出し
            switch( pFP->option ){
            case eFILL_OPTION_BRIGHT:
                DrawFillAntiBrightForTouch( pFP, touchId, CDefTable::GetPalOfsValue( pFP->palOfsId ) );
                break;
                
            case eFILL_OPTION_DARK:
                DrawFillAntiDarkForTouch( pFP, touchId, CDefTable::GetPalOfsValue( pFP->palOfsId ) );
                break;
                    
            // その他はアンチを引けない
            default:
                break;
            }
        }
        
        // 次のポイントへ
        pFP = (stBEZIER_FILL_POINT*) pFP->pNext;
    }
}

//-----------------------------------------------------------
// [Fill] 描画実体：明るく（※無効判定は呼び出し元で済んでいるものとする）
//-----------------------------------------------------------
void CBezier::DrawFillAntiBrightForTouch( stBEZIER_FILL_POINT* pFP, eSTROKE_TOUCH_TARGET touchId, BYTE targetPalGrp ){
    FILL_COMMON_STROKE;
    ANTI_COMMON_START;
    
    // 画素の出力
    for( int i=0; i<h; i++ ){
        for( int j=0; j<w; j++ ){
            //----------------------------------------------------------------
            // 塗り完了色でなければ（※ストローク境界上の塗り潰されなかった画素がアンチ対象）
            //----------------------------------------------------------------
            int bufAt = s_nBufW*(iY+i) + (iX+j);
            if( ((s_pBufFillGuide[bufAt]&BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG)==0x00) && (s_pBufFillGuard[bufAt]==0) ){
                //-----------------------------------
                // 画素が有効であれば（※出力に意味があれば）
                //-----------------------------------
                int dotAt = w*i + j;
                int subVal = (BEZIER_PAL_VAL_MAX*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;
                if( subVal > 0 ){
                    //-----------------------------------
                    // 対象パレットであれば
                    //-----------------------------------
                    int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( s_pBuf[bufAt] );
                    if( palGrp == targetPalGrp ){
                        // ストロークの濃さにより対象画素を明るく（※累積すると汚いので上書き）
                        int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( s_pBuf[bufAt] );
                        subVal = BEZIER_PAL_VAL_MAX - subVal;   // 濃さを明るさに調整
                        if( subVal < palVal ){
                            s_pBuf[bufAt] = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, subVal );
                        }
                    }
                }
            }
        }
    }
    
    ANTI_COMMON_END;
}

//-----------------------------------------------------------
// [Fill] 描画実体：暗く（※無効判定は呼び出し元で済んでいるものとする）
//-----------------------------------------------------------
void CBezier::DrawFillAntiDarkForTouch( stBEZIER_FILL_POINT* pFP, eSTROKE_TOUCH_TARGET touchId, BYTE targetPalGrp ){
    FILL_COMMON_STROKE;
    ANTI_COMMON_START;

    // 画素の出力
    for( int i=0; i<h; i++ ){
        for( int j=0; j<w; j++ ){
            //----------------------------------------------------------------
            // 塗り完了色でなければ（※ストローク境界上の塗り潰されなかった画素がアンチ対象）
            //----------------------------------------------------------------
            int bufAt = s_nBufW*(iY+i) + (iX+j);
            if( ((s_pBufFillGuide[bufAt]&BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG)==0x00) && (s_pBufFillGuard[bufAt]==0) ){
                //-----------------------------------
                // 画素が有効であれば（※出力に意味があれば）
                //-----------------------------------
                int dotAt = w*i + j;
                int addVal = (BEZIER_PAL_VAL_MAX*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;
                if( addVal > 0 ){
                    //-----------------------------------
                    // 対象パレットであれば
                    //-----------------------------------
                    int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( s_pBuf[bufAt] );
                    if( palGrp == targetPalGrp ){
                        // ストロークの濃さにより対象画素を暗く（※累積すると汚いので上書き）
                        int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( s_pBuf[bufAt] );
                        if( addVal > palVal ){
                            s_pBuf[bufAt] = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, addVal );
                        }
                    }
                }
            }
        }
    }
    
    ANTI_COMMON_END;
}
