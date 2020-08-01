/*+----------------------------------------------------------------+
  |	Title:		BezierSub.cpp [共通環境]
  |	Comment:	ベジェ曲線：サブ
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
// ストロークの先端塗りの基本サイズ
#define STROKE_EDGE_FILL_BASE_SIZE          (10*GM_P_RATE)

// ストロークの先端塗りの高さ判定ボーダー
#define STROKE_EDGE_FILL_BORDER_HEIGHT      (2*GM_P_RATE)

// ストロークの先端塗りのログ指定
//#define STROKE_EDGE_FILL_LOG

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
//----------------------------------------------------------------------------
// [Sub] ワークパス用のテクスチャドットの作成
// この時点でのドット値はフラグとしての値となる[1]（※利用側がドットを置くかの判断につかうのみ）
//----------------------------------------------------------------------------
void CBezier::ReadyDotForWorkPath( void ){
    int size;
    
    // 各点（※矩形なのでバッファは不要）
    s_nSizeForWorkPathLine = BEZIER_WP_LINE_SIZE * GM_P_RATE;
    s_nSizeForWorkPathPoint = BEZIER_WP_POINT_SIZE * GM_P_RATE;
    s_nSizeForWorkPathSelected = BEZIER_WP_SELECTED_SIZE * GM_P_RATE;

    // 方向線の先端部分（※三角形：ここでは基本形として上向き）
    s_nSizeForWorkPathTriangle = BEZIER_WP_TRIANGLE_SIZE * GM_P_RATE;
    size = s_nSizeForWorkPathTriangle;
    int m = size/5;
    int cW = (size-2*m)/2;
    for( int i=0; i<(size-2*m); i++ ){
        for( int j=0; j<(size-2*m); j++ ){
            int border = ((size-2*m)-i)/2;
            int check = -1;
            
            // 右半分
            if( cW <= j ){
                check = (size-2*m) - (j+1);
            }
            // 左半分
            else{
                check = j;
            }
            
            if( check >= border ){
                s_pDotForWorkPathTriangle[size*(i+m)+(j+m)] = 1;
            }
        }
    }
    
    // 塗りポイント（※バツ）
    s_nSizeForWorkPathCross = BEZIER_WP_CROSS_SIZE * GM_P_RATE;
    size = s_nSizeForWorkPathCross;
    for( int i=0; i<size; i++ ){
        for( int j=1; j<(size-1); j++ ){
            if( i==j || (size-1-i) == j ){
                s_pDotForWorkPathCross[size*(i-1)+j-1] = 1;
                s_pDotForWorkPathCross[size*(i-1)+j] = 1;
                s_pDotForWorkPathCross[size*(i-1)+j+1] = 1;

                s_pDotForWorkPathCross[size*i+j-1] = 1;
                s_pDotForWorkPathCross[size*i+j] = 1;
                s_pDotForWorkPathCross[size*i+j+1] = 1;
                
                s_pDotForWorkPathCross[size*(i+1)+j-1] = 1;
                s_pDotForWorkPathCross[size*(i+1)+j] = 1;
                s_pDotForWorkPathCross[size*(i+1)+j+1] = 1;
            }
        }
    }
    
    // スロットポイント（※プラス）
    s_nSizeForWorkPathPlus = BEZIER_WP_PLUS_SIZE * GM_P_RATE;
    size = s_nSizeForWorkPathPlus;
    m = size/2;
    
    // ヨコ
    for( int i=0; i<size; i++ ){
        s_pDotForWorkPathPlus[size*i+m-1] = 1;
        s_pDotForWorkPathPlus[size*i+m] = 1;
        s_pDotForWorkPathPlus[size*i+m+1] = 1;
    }
    // タテ
    for( int j=0; j<size; j++ ){
        s_pDotForWorkPathPlus[size*(m-1)+j] = 1;
        s_pDotForWorkPathPlus[size*(m)+j] = 1;
        s_pDotForWorkPathPlus[size*(m+1)+j] = 1;
    }
}

//---------------------------------
// [Sub] ワークパス用：テクスチャの出力
//---------------------------------
void CBezier::PutTexForWorkPath( float x0, float y0, int size, BYTE* pTex, BYTE pal, int cut ){
    int w = size;
    int h = size;
    int iX = (int)(x0-(w-1)/2.0f);
    int iY = (int)(y0-(h-1)/2.0f);
    
    // 範囲確認（※領域外にはみ出していたら調整）
    if( iX < 0 ){
        w += iX;
        iX = 0;
    }else if( (iX+w) >= s_nBufW ){
        w += (s_nBufW-1) - (iX+w);
    }
    
    if( iY < 0 ){
        h += iY;
        iY = 0;
    }else if( (iY+h) >= s_nBufH ){
        h += (s_nBufH-1) - (iY+h);
    }
    
    // サイズが無効になったら無視
    if( w <= 0 || h <= 0 ){
        return;
    }
    
    // ここまできたらドットを出力
    for( int i=cut; i<(h-cut); i++ ){
        for( int j=cut; j<(w-cut); j++ ){
            int at = s_nBufW*(iY+i) + (iX+j);
            int texAt = size*i + j;
            
            // 有効なドット＆既存の値よりも大きければ出力
            if( pTex == NULL || pTex[texAt] != 0 ){
                BYTE target = s_pBuf[at];

                // 未出力／オフセットが大きい／値が大きい場合は出力（※上書き判定時、線は弱くしておく）
                if( target == 0 ){
                    s_pBuf[at] = pal;
                }else if( (pal&0xF) > (target&0xF) && (pal&0xF0) != BEZIER_WP_LINE_PAL ){
                    s_pBuf[at] = pal;
                }else if( (pal&0xF) == (target&0xF) && pal > target ){
                    s_pBuf[at] = pal;
                }
            }
        }
    }
}

//-------------------------------
// [Sub] ワークパス用：線ドットの出力
//-------------------------------
void CBezier::PutLineDotForWorkPath( float x0, float y0, BYTE pal ){
    if( s_bTouch ){
        PutTexForWorkPath( x0, y0, s_nSizeForWorkPathLine/2, NULL, pal );
    }else{
        PutTexForWorkPath( x0, y0, s_nSizeForWorkPathLine, NULL, pal );
    }
}

//--------------------------
// [Sub] ワークパス用：点の出力
//--------------------------
void CBezier::PutPointForWorkPath( float x0, float y0, BYTE pal ){
    // 選択中であればサイズを大きく（※常時アクティブ時は無視）
    if( (pal&0x0F) == BEZIER_WP_PAL_VAL_FOR_SELECTED && !IsDispPathActiveAlways() ){
        if( s_bTouch ){
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathSelected/2, NULL, pal );
        }else{
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathSelected, NULL, pal );
        }
    }else{
        if( s_bTouch ){
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPoint/2, NULL, pal );
        }else{
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPoint, NULL, pal );
        }
    }
}

//---------------------------
// [Sub] ワークパス用：三角の出力
//---------------------------
static BYTE __bufForWorkPath[4*1024];
void CBezier::PutTriangleForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal, bool isFlip ){
    BYTE *pBuf = __bufForWorkPath;
    BYTE *pBuf0 = s_pDotForWorkPathTriangle;
    int size = s_nSizeForWorkPathTriangle;

    // バッファのクリア
    memset( pBuf, 0, size*size );

    // 方向の取得（※回転の都合で軸をずらして計算）
    float deg = CMath::GetDeg( ofsY, ofsX ) - 90;
    if( isFlip ){
        deg += 180;
    }
    
    float fSin = CMath::GetSin( deg );
    float fCos = CMath::GetCos( deg );

    // ソースを回転してバッファにコピー
    float hS = size / 2.0f;
    for( int i=0; i<size; i++ ){
        for( int j=0; j<size; j++ ){
            int x = (int)(fCos*(j-hS) - fSin*(i-hS) + hS);
            int y = (int)(fSin*(j-hS) + fCos*(i-hS) + hS);

            if( x >= 0 && x < size && y >= 0 && y < size ){
                if( pBuf0[size*y+x] != 0 ){
                    pBuf[size * i + j] = 1;
                }
            }
        }
    }

    // 作成したバッファで描画
    PutTexForWorkPath( x0+ofsX, y0+ofsY, size, pBuf, pal );
}

//----------------------------
// [Sub] ワークパス用：バツの出力
//----------------------------
void CBezier::PutCrossForWorkPath( float x0, float y0, BYTE pal ){
    PutTexForWorkPath( x0, y0, s_nSizeForWorkPathCross, s_pDotForWorkPathCross, pal );
}

//----------------------------
// [Sub] ワークパス用：プラスの出力
//----------------------------
void CBezier::PutPlusForWorkPath( float x0, float y0, BYTE pal ){
    PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPlus, s_pDotForWorkPathPlus, pal );
}

//--------------------------------
// [Sub] ワークパス用：プラスの出力(小)
//--------------------------------
void CBezier::PutPlusForWorkPathS( float x0, float y0, BYTE pal ){
    PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPlus, s_pDotForWorkPathPlus, pal, 2 );
}

//------------------------------------------------------
// [Sub] ワークパス用：２点間を線形補間で描画（※方向線用）
//------------------------------------------------------
void CBezier::PutLinearForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal ){
    // 二点間のドット数の算出（※整数で判定）
    int distX = (int)ofsX;
    int distY = (int)ofsY;
    if( distX < 0 ){ distX = -distX; }
    if( distY < 0 ){ distY = -distY; }
    int div = ((distX>distY)? distX: distY);
    
    // 用心（※始点と終点にドット差分がない場合は終点が描画されるように）
    int start = 0;
    if( div <= 0 ){
        start = div = 1;
    }
    
    //----------------------------------------------------------------------------------------
    // 各ドットの描画（※[i<=div]なので終点[x1,y1]まで描画される）
    //----------------------------------------------------------------------------------------
    for( int i=start; i<=div; i++ ){
        float t = ((float)i)/div;
        float x = x0 + ofsX*t;
        float y = y0 + ofsY*t;
        
        int iX = (int)x;
        int iY = (int)y;
        
        PutLineDotForWorkPath( iX, iY, pal );
    }
}

//------------------------
// [Sub] 描画実体：ストローク
//------------------------
void CBezier::DrawStroke( stBEZIER_ANCHOR_POINT* pHead, float limitRate ){
    stBEZIER_ANCHOR_POINT* pP0 = pHead;
    
    //---------------------------------------------------
    // フック調整
    //---------------------------------------------------
    pP0 = pHead;
    while( pP0 != NULL ){
        // フック解決（※傾き適用前）
        resolveHookPoint( pP0 );

        // 次のポイントへ（※閉じていたら抜ける）
        pP0 = (stBEZIER_ANCHOR_POINT*) pP0->pNext;
        if( pP0 == pHead ){
            break;
        }
    }
    
    //---------------------------------------------------
    // ワークパス指定であればポイントと方向線の表示
    //---------------------------------------------------
    if( s_bWorkPath ){
        pP0 = pHead;
        while( pP0 != NULL ){
            float x = pP0->x;
            float y = pP0->y;
            PutPointForWorkPath( x, y, BEZIER_WP_POINT_PAL + pP0->workPathPalOfsS );
            
            // スリープ中でなければ方向線の表示（※スリープ中は煩わしいので非表示）
            if( !IsDispOffDirVectorAlways() && (pP0->workPathPalOfsS != BEZIER_WP_PAL_VAL_FOR_SLEEP || IsDispDirVectorAlways()) ){
                if( pP0->xIn != 0.0f || pP0->yIn != 0.0f ){
                    PutLinearForWorkPath( x, y, pP0->xIn, pP0->yIn, BEZIER_WP_DIR_IN_PAL + pP0->workPathPalOfsS );
                    PutTriangleForWorkPath( x, y, pP0->xIn, pP0->yIn, BEZIER_WP_DIR_IN_PAL + pP0->workPathPalOfsS, false );
                }
                
                if( pP0->xOut != 0.0f || pP0->yOut != 0.0f ){
                    PutLinearForWorkPath( x, y, pP0->xOut, pP0->yOut, BEZIER_WP_DIR_OUT_PAL + pP0->workPathPalOfsS );
                    PutTriangleForWorkPath( x, y, pP0->xOut, pP0->yOut, BEZIER_WP_DIR_OUT_PAL + pP0->workPathPalOfsS, true );
                }
            }
            
            // 次のポイントへ（※閉じていたら抜ける）
            pP0 = (stBEZIER_ANCHOR_POINT*) pP0->pNext;
            if( pP0 == pHead ){
                break;
            }
        }
    }
    
    //------------------------------------------------------------------
    // タッチ登録調整ワーク（※ストローク処理前のタッチ登録数の退避と反転フラグの準備）
    // ストロークの描画順と方向によりタッチポイント登録がちぐはぐになってしまう際は、
    // [isTouchPointReverse]指定によりタッチ登録を並び替えることで方向を反転させる
    //------------------------------------------------------------------
    bool arrIsTouchPointReverse[eSTROKE_TOUCH_TARGET_MAX];
    int arrTouchPointNum0[eSTROKE_TOUCH_TARGET_MAX];
    
    // 通常ストロークであれば退避（※タッチストロークの場合は無視）
    if( !s_bWorkPath && !s_bTouch ){
        for( int i=0; i<eSTROKE_TOUCH_TARGET_MAX; i++ ){
            arrIsTouchPointReverse[i] = false;
            arrTouchPointNum0[i] = s_nArrTouchPointNum[i];
        }
    }

    //---------------------------------------------------
    // 分割点リストの構築（※[pNext==NULL]で抜けている点に注意）
    //---------------------------------------------------
    pP0 = pHead;
    stBEZIER_ANCHOR_POINT* pLastPrev = pP0;
    while( pP0->pNext != NULL ){
        pLastPrev = pP0;
        stBEZIER_ANCHOR_POINT* pP1 = (stBEZIER_ANCHOR_POINT*) pP0->pNext;
        
        // [pP0, pP1)間の分割点をリストへ登録
        RegistDivisionPointList( pP0, pP1 );
        
        // タッチ調整フラグがあれば反転の予約
        if( pP0->touchTarget >= 0 && pP0->touchTarget < eSTROKE_TOUCH_TARGET_MAX ){
            if( pP0->isTouchPointReverse ){
                arrIsTouchPointReverse[pP0->touchTarget] = true;
            }
        }
    
        // 次のポイントへ（※閉じていたら抜ける）
        pP0 = pP1;
        if( pP0 == pHead ){
            break;
        }
    }

    // 終点を追加（※[registDivisionPointList]は[pP1]の座標を登録しないため、最後の点を別途追加）
    //（※ここで追加される点は、オープンパスなら最後のアンカーポイント、クローズパスなら最初のアンカーポイントとなる）
    float strokeSize = pLastPrev->strokeSize;
    if( pLastPrev->strokeEndRange > 0.0f ){
        strokeSize = pLastPrev->strokeEndSize;
    }
    AddDivisionPoint( pP0->x, pP0->y, strokeSize, true, pP0 );

    // 用心（※分割点が２未満だと線が引けない）
    if( s_nUseDivisionPoint < 2 ){
        LOGE( "@ CBezier::DrawStroke: DIVISION POINT(%d) LESS THAN 2\n", s_nUseDivisionPoint );
        return;
    }

    //---------------------------------------------------------
    // タッチ指定があれば制限数の算出（※[1.0f]を超える場合はそのままの線）
    //---------------------------------------------------------
    if( !s_bWorkPath && s_bTouch ){
        // 各分割点間のポイント数の算出
        for( int i=1; i<s_nUseDivisionPoint; i++ ){
            int numPoint = CalcPointNum( s_stArrDivisionPoint[i-1].x, s_stArrDivisionPoint[i-1].y,
                                         s_stArrDivisionPoint[i].x, s_stArrDivisionPoint[i].y );
            s_nTouchStrokePointMax += numPoint;
        }
        
        // オープンパスであれば終点を考慮
        stBEZIER_DIVISION_POINT* pDP = &s_stArrDivisionPoint[s_nUseDivisionPoint-1];
        if( pDP->x != s_stArrDivisionPoint[0].x || pDP->y != s_stArrDivisionPoint[0].y ){
            s_nTouchStrokePointMax += 1;
        }
        
        // 制限数の確定
        s_nTouchStrokePointLimit = (int)(s_nTouchStrokePointMax * limitRate);
        if( s_nTouchStrokePointLimit > s_nTouchStrokePointMax ){
            s_nTouchStrokePointLimit = s_nTouchStrokePointMax;
        }

        // 用心（※出力するポイントが無ければ返す）
        if( s_nTouchStrokePointLimit <= 0 ){
            return;
        }
    }

    // ブラシのブレ設定
    ResetBrushShake();
    SetBrushShake();
    
    //--------------------------
    // 分割点による線の描画
    //--------------------------
    for( int i=1; i<s_nUseDivisionPoint; i++ ){
        // 始点[i-1]から、終点[i]に線を伸ばす（※スタイル／色は始点[i-1]のもの）
        PutDotLinear( &s_stArrDivisionPoint[i-1], &s_stArrDivisionPoint[i], s_stArrDivisionPoint[i-1].pAP );
    }
    
    // オープンパスであれば終点を描画（※[registDivisionPointList]同様に最後の点が表示されないので別途描画）
    //（※ここは[最後の一つ手前]の分割点から見た終点を描画するため[s_nUseDivisionPoint-2]の[AP]を渡している）
    stBEZIER_DIVISION_POINT* pDP = &s_stArrDivisionPoint[s_nUseDivisionPoint-1];
    if( pDP->x != s_stArrDivisionPoint[0].x || pDP->y != s_stArrDivisionPoint[0].y  ){
        PutDotLinear( pDP, pDP, s_stArrDivisionPoint[s_nUseDivisionPoint-2].pAP );
    }
    
    //-----------------------------------
    // 後始末：通常ストロークであれば
    //-----------------------------------
    if( !s_bWorkPath && !s_bTouch ){
        // 指定があればタッチ登録反転
        for( int i=0; i<eSTROKE_TOUCH_TARGET_MAX; i++ ){
            if( arrIsTouchPointReverse[i] ){
                ReverseTouchPoint( (eSTROKE_TOUCH_TARGET)i, arrTouchPointNum0[i], s_nArrTouchPointNum[i]-1 );
            }
        }

        // ストロークのエッジ塗りの確認
        for( int i=0; i<s_nUseDivisionPoint; i++ ){
            if( s_stArrDivisionPoint[i].isEdgeFillCheck ){
                FillStrokeEdge( i );
            }
        }
    }
}

//------------------------
// [Sub] タッチポイントの反転
//------------------------
void CBezier::ReverseTouchPoint( eSTROKE_TOUCH_TARGET targetTouchId, int atFrom, int atTo ){
    // 範囲が有効であれば
    if( atTo > atFrom ){
        float temp;
        float* arrX = s_fArrArrTouchPointX[targetTouchId];
        float* arrY = s_fArrArrTouchPointY[targetTouchId];
        float* arrX0 = s_fArrArrTouchPointX0[targetTouchId];
        float* arrY0 = s_fArrArrTouchPointY0[targetTouchId];

        int numSwap = ((atTo+1)-atFrom)/2;
        while( numSwap > 0 ){
            // Ｘ座標スワップ
            temp = arrX[atTo];
            arrX[atTo] = arrX[atFrom];
            arrX[atFrom] = temp;
            
            // Ｙ座標スワップ
            temp = arrY[atTo];
            arrY[atTo] = arrY[atFrom];
            arrY[atFrom] = temp;

            // Ｘ座標（傾き前）スワップ
            temp = arrX0[atTo];
            arrX0[atTo] = arrX0[atFrom];
            arrX0[atFrom] = temp;
            
            // Ｙ座標（傾き前）スワップ
            temp = arrY0[atTo];
            arrY0[atTo] = arrY0[atFrom];
            arrY0[atFrom] = temp;

            // 値の更新
            numSwap--;
            atFrom++;
            atTo--;
        }
    }
}

//------------------------------------------
// [Sub] 指定の分割点を中心にストロークのエッジ埋め
//------------------------------------------
void CBezier::FillStrokeEdge( int targetDivisionPoint ){
    // 無効は無視
    if( targetDivisionPoint < 0 || targetDivisionPoint >= s_nUseDivisionPoint ){
        return;
    }

    stBEZIER_DIVISION_POINT* pTarget = &s_stArrDivisionPoint[targetDivisionPoint];
    stBEZIER_ANCHOR_POINT* pAP = pTarget->pAP;

    // ストロークが無効であれば無視（※目に見える線でなければエッジを埋める必要はない）
    CStroke* pStroke = NULL;
    if( pAP->type != eSTROKE_TYPE_OFF ){
        pStroke = CStrokeMgr::GetStroke( pAP->type, pAP->size, pAP->ofsRange );
    }
    
    if( pStroke == NULL ){
        return;
    }
    
    // ガイドリセット
    memset( s_pBufForEdgeFillGuide, BEZIER_PAL_WITHOUT_COL, sizeof(BYTE)*s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide );

    //---------------------------------------------
    // 起点の設定（※対象のAPが塗りガイドの中央に来るように）
    //---------------------------------------------
    float baseX = pTarget->x;
    float baseY = pTarget->y;
    int ofsX = s_nBufSizeForEdgeFillGuide/2 - (int)baseX;
    int ofsY = s_nBufSizeForEdgeFillGuide/2 - (int)baseY;

    // 検出するポイント数（※この値が大きいほどエッジの塗りつぶしが大きくなる）
    int numCheck = (int)(STROKE_EDGE_FILL_BASE_SIZE*pAP->strokeEdgeFillSize);
    
    // work
    stBEZIER_DIVISION_POINT* pTemp0;
    stBEZIER_DIVISION_POINT* pTemp1;
    int num, point, at;
    int numRest, pointAt;

    //--------------------------------------
    // 手前の線（侵入してくるストローク）を確認
    //--------------------------------------
    float frontX = baseX;
    float frontY = baseY;

    pTemp0 = NULL;
    pTemp1 = pTarget;
    numRest = numCheck;
    pointAt = targetDivisionPoint - 1;
    while( numRest > 0 ){
        // 対象が始点を超えた場合
        if( pointAt < 0 ){
            pointAt = s_nUseDivisionPoint - 1;  // 終点へ

            // クローズパスでなければ抜ける（※以降のポイントが存在しない）
            if( s_stArrDivisionPoint[pointAt].pAP != s_stArrDivisionPoint[0].pAP ){
                break;
            }
            
            // クローズパスであればもう一つ先へ（※終点は重複登録されているので）
            pointAt--;
        }

        // 対象の分割点の取り出し
        pTemp0 = &s_stArrDivisionPoint[pointAt];
        
        // [pTemp0]〜[pTemp1]間の座標を登録
        point = CalcThenSetPointForTempBuf( pTemp0, pTemp1, pTemp0->pAP );
        num = (numRest>point)? point: numRest;
        
        for( int i=0; i<num; i++ ){
            frontX = s_fArrTempPointX[point-(i+1)];
            frontY = s_fArrTempPointY[point-(i+1)];

            at = ((int)frontY+ofsY)*s_nBufSizeForEdgeFillGuide + ((int)frontX+ofsX);
            s_pBufForEdgeFillGuide[at] = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;
        }

        // 処理した分だけ残り回数を減らす
        numRest -= num;

        // 次へ
        pTemp1 = pTemp0;
        pointAt--;
    }
    
    //--------------------------------------
    // 起点からの線（出立するストローク）を確認
    //--------------------------------------
    float backX = baseX;
    float backY = baseY;

    pTemp0 = pTarget;
    pTemp1 = NULL;
    numRest = numCheck;
    pointAt = targetDivisionPoint + 1;
    while( numRest > 0 ){
        // 対象が終点に達した場合（※終点は重複登録されるため）
        if( pointAt >= (s_nUseDivisionPoint-1) ){
            pointAt = 0;  // 始点へ
            
            // クローズパスでなければ抜ける（※以降のポイントが存在しない）
            if( s_stArrDivisionPoint[pointAt].pAP != s_stArrDivisionPoint[s_nUseDivisionPoint-1].pAP ){
                break;
            }
        }
        
        // 対象の分割点の取り出し
        pTemp1 = &s_stArrDivisionPoint[pointAt];
        
        // [pTemp0]〜[pTemp1]間の座標を登録
        point = CalcThenSetPointForTempBuf( pTemp0, pTemp1, pTemp0->pAP );
        num = (numRest>point)? point: numRest;
        
        for( int i=0; i<num; i++ ){
            backX = s_fArrTempPointX[i];
            backY = s_fArrTempPointY[i];
            
            at = ((int)backY+ofsY)*s_nBufSizeForEdgeFillGuide + ((int)backX+ofsX);
            s_pBufForEdgeFillGuide[at] = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;
        }
        
        // 処理した分だけ残り回数を減らす
        numRest -= num;
        
        // 次へ
        pTemp0 = pTemp1;
        pointAt++;
    }
    
    //------------------------------------
    // ３点による三角形の確認
    //------------------------------------
    float centerX = (baseX + frontX + backX) / 3.0f;
    float centerY = (baseY + frontY + backY) / 3.0f;
    float height = CMath::Len( baseX-centerX, baseY-centerY );

#ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ STROKE EDGE FILL LOG\n" );
    LOGD( "@ ofsX=%d, ofsY=%d\n", ofsX, ofsY );
    LOGD( "@ baseX=%f, baseY=%f\n", baseX, baseY );
    LOGD( "@ frontX=%f, frontY=%f\n", frontX, frontY );
    LOGD( "@ backX=%f, backY=%f\n", backX, backY );
    LOGD( "@ centerX = %f, centerY = %f\n", centerX, centerY );
    LOGD( "@ height = %f(%d)\n", height, STROKE_EDGE_FILL_BORDER_HEIGHT );
#endif
    
    // 三角形の高さが一定以上なければ無視（※塗りつぶしの面積が無い）
    if( height < STROKE_EDGE_FILL_BORDER_HEIGHT ){
#ifdef STROKE_EDGE_FILL_LOG
        LOGD( "@ SKIP: HEIGHT NOT ENOUGH\n" );
#endif
        return;
    }
    
    //------------------------------------
    // 前後のストローク間を補完
    //------------------------------------
    // 出立＆侵入方向の算出
    float outX = 2.0f*(baseX-frontX)/3.0f;
    float outY = 2.0f*(baseY-frontY)/3.0f;
    float inX = 2.0f*(baseX-backX)/3.0f;
    float inY = 2.0f*(baseY-backY)/3.0f;

    int x0 = (int)frontX;
    int y0 = (int)frontY;
    int x1, y1;
    num = CalcPointNum( frontX, frontY, backX, backY );
    // ループ範囲は始点／終点を含めておく（※始点／終点前後でピクセル飛びが起こらないように）
    for( int i=0; i<=num; i++ ){
        // t, (1-t)
        float t1 = ((float)i)/num;
        float tR1 = 1.0f - t1;
        
        // t^2, (1-t)^2
        float t2 = t1 * t1;
        float tR2 = tR1 * tR1;
        
        // t^3, (1-t)^3
        float t3 = t2 * t1;
        float tR3 = tR2 * tR1;
        
        // (1-t)^2*t, (1-t)*t^2
        float tR2_t1 = tR2 * t1;
        float tR1_t2 = tR1 * t2;
        
        // 分割点座標の算出（※ベジェ曲線）
        float x = frontX*tR3 + 3*(frontX+outX)*tR2_t1 + 3*(backX+inX)*tR1_t2 + backX*t3;
        float y = frontY*tR3 + 3*(frontY+outY)*tR2_t1 + 3*(backY+inY)*tR1_t2 + backY*t3;
        
        // ストロークの描画（※ここは閉じる必要がないので１ドットのみ出力）
        PutDotAt( x, y, 1.0f, s_nTestPalGrp, pStroke, pAP );

        //------------------------------------------------------------------------------------
        // 塗りガイドが２ピクセル以上離れてしまっても領域が閉じるように前回描画座標からの距離でピクセルを処理する
        //------------------------------------------------------------------------------------
        x1 = (int)x;
        y1 = (int)y;

        int divX = x1-x0;
        int divY = y1-y0;
        if( divX < 0 ){ divX *= -1; }
        if( divY < 0 ){ divY *= -1; }
        int len = ((divX>divY)? divX: divY);

#if 1
        // ３以上であれば偶数調整（※分母が３になると「割り算→掛け算」の計算誤差で色々と困るので小細工）
        if( len >= 3 ){
            len = 2*((len+1)/2);
        }
#endif
        
        if( len > 0 ){
            for( int j=0; j<len; j++ ){
                t1 = ((float)j)/len;
                tR1 = 1.0f - t1;
                x = x1*tR1 + x0*t1;
                y = y1*tR1 + y0*t1;
                
                int at = ((int)y+ofsY)*s_nBufSizeForEdgeFillGuide + ((int)x+ofsX);
                s_pBufForEdgeFillGuide[at] = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;
            }
            
            x0 = x1;
            y0 = y1;
        }
    }

    // ストロークを塗りつぶす（※基準点と三角形の中心を開始点に指定しておく）
    s_nOfsXForEdgeFillGuide = ofsX;
    s_nOfsYForEdgeFillGuide = ofsY;
    s_pApForEdgeFillGuide = pAP;
    int startX = (int)((centerX+baseX)/2+ofsX);
    int startY = (int)((centerY+baseY)/2+ofsY);

#ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ STROKE EDGE GUIDE: BEFORE\n" );
    for( int i=0; i<s_nBufSizeForEdgeFillGuide; i++ ){
        for( int j=0; j<s_nBufSizeForEdgeFillGuide; j++ ){
            int at = i*s_nBufSizeForEdgeFillGuide + j;
            if( i==startY && j==startX ){
                LOGD( "*" );
            }else if( s_pBufForEdgeFillGuide[at] == BEZIER_PAL_WITHOUT_COL ){
                LOGD( "." );
            }else{
                LOGD( "@" );
            }
        }
        LOGD( "\n" );
    }
#endif

    // TODO:開始点／終了点付近で微妙な閉領域が残ってしまうことがあるがどうにかならないか？
    FillStrokeAt( 0, startX, startY );

#ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ STROKE EDGE GUIDE: AFTER\n" );
    for( int i=0; i<s_nBufSizeForEdgeFillGuide; i++ ){
        for( int j=0; j<s_nBufSizeForEdgeFillGuide; j++ ){
            int at = i*s_nBufSizeForEdgeFillGuide + j;
            if( i==startY && j==startX ){
                LOGD( "*" );
            }else if( s_pBufForEdgeFillGuide[at] == BEZIER_PAL_WITHOUT_COL ){
                LOGD( "." );
            }else{
                LOGD( "@" );
            }
        }
        LOGD( "\n" );
    }
#endif
}

//-----------------------------------------------------------------
// 指定位置のストロークを塗りガイドにより埋める
// TODO:ここで無限ループになることがあるので対策する
//-----------------------------------------------------------------
// 固定値を引数で渡すのを避けるため呼び出し前には下記の値を設定する必要がある
// ・s_nOfsXForEdgeFillGuide：描画オフセット：X
// ・s_nOfsYForEdgeFillGuide：描画オフセット：Y
// ・s_pApForEdgeFillGuide：ドットを出力するAP（※描画情報用）
//------------------------------------------------------------------
#ifdef STROKE_EDGE_FILL_LOG
static int __fill_stroke_depth = 0;
#endif
void CBezier::FillStrokeAt( int from, int x, int y ){
    // すでに塗られていたら返す
    int at = y*s_nBufSizeForEdgeFillGuide + x;
    if( s_pBufForEdgeFillGuide[at] != BEZIER_PAL_WITHOUT_COL ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth--;
#endif
        return;
    }
    
    // 済みフラグを立てる
    s_pBufForEdgeFillGuide[at] = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;

    // ストロークの塗りつぶし（※出力の判定は[PutDotAt]と同じにしておくこと）
    stBEZIER_ANCHOR_POINT* pAP = s_pApForEdgeFillGuide;
    int bufAt = s_nBufW*(y-s_nOfsYForEdgeFillGuide) + (x-s_nOfsXForEdgeFillGuide);
    
    //------------------
    // テストに通過したら
    //------------------
    bool checkResult;
    if( pAP->isDotPutOnOut ){
        checkResult = ((s_pBufFillGuide[bufAt]&BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG)!=0x00) && (s_pBufFillGuard[bufAt]==0);
    }else{
        checkResult = ((s_pBufFillGuide[bufAt]&BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG)==0x00) && (s_pBufFillGuard[bufAt]==0);
    }
    if( (checkResult&&BEZIER_CONV_DOT_TO_PAL_GRP(s_pBufTest[bufAt])==s_nTestPalGrp) || pAP->isDotPutForce ){
        // ストロークバッファへ出力（※[UpdateStrokeBufInfo]は不要＝エッジ塗りは描画済みの領域をはみ出ることはない）
        s_pBuf[bufAt] = (BYTE)pAP->dotPutMax;
    }
    
    // 左右上下へ（※[from]の値により移動元には行かないようにしておく）
    if( from != 1 && x > 0 ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        LOGD( "@@@ [LEFT]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeAt( 2, x-1, y );
    }
    if( from != 2 && x < (s_nBufSizeForEdgeFillGuide-1) ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        LOGD( "@@@ [RIGHT]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeAt( 1, x+1, y );
    }
    if( from != 3 && y > 0 ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        LOGD( "@@@ [UP]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeAt( 4, x, y-1 );
    }
    if( from != 4 && y < (s_nBufSizeForEdgeFillGuide-1) ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        LOGD( "@@@ [DOWN]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeAt( 3, x, y+1 );
    }
    
#ifdef STROKE_EDGE_FILL_LOG
    __fill_stroke_depth--;
#endif
}

//----------------------
// [Sub] 描画実体：ドット
//----------------------
void CBezier::DrawDot( stBEZIER_ANCHOR_POINT* pHead ){
    stBEZIER_ANCHOR_POINT* pAP = pHead;
    
    // 各点の表示
    while( pAP != NULL ){
        // フック解決（※アングル適用前）
        resolveHookPoint( pAP );
        
        float x = pAP->x;
        float y = pAP->y;

        // ワークパス時はポイントの描画
        if( s_bWorkPath ){
            PutPointForWorkPath( x, y, BEZIER_WP_POINT_PAL + pAP->workPathPalOfsS );
        }
        // 通常時はブラシによる描画
        else{
            CStroke* pStroke = NULL;
            if( pAP->type != eSTROKE_TYPE_OFF ){
                pStroke = CStrokeMgr::GetStroke( pAP->type, pAP->size, pAP->ofsRange );
            }

            if( pStroke != NULL ){
                // ドット時はアンカーポイント（開始点）のサイズのみを反映させる（※終点は無視）
                float strokeSize = pAP->strokeSize;
                if( pAP->strokeStartRange > 0 ){
                    strokeSize = pAP->strokeStartSize;
                }
                
                if( strokeSize > 0.0f ){
                    PutDotAt( x, y, strokeSize, s_nTestPalGrp, pStroke, pAP );
                }
            }
        }
        
        // 次のポイントへ
        pAP = (stBEZIER_ANCHOR_POINT*) pAP->pNext;
        
        // 用心（※クローズパスの場合は抜ける）
        if( pAP == pHead ){
            break;
        }
    }
}

//------------------------------
// ブラシのブレ：リセット
//------------------------------
void CBezier::ResetBrushShake( void ){
    s_oSpringForBrushPos.clearSpring();
    s_oSpringForBrushSize.clearSpring();
    m_nResetCountForBrushPos = 0;
    m_nResetCountForBrushSize = 0;
    m_nUpdateFrameForBrushPos = 0.0f;
    m_nUpdateFrameForBrushSize = 0.0f;
}

//------------------------------
// ブラシのブレ：設定
//------------------------------
void CBezier::SetBrushShake( void ){
    // バネの動きは一定（※緩い動きで減衰しない）
    s_oSpringForBrushPos.set( 10, 0.01f, 1.0f );
    s_oSpringForBrushPos.setV( 10 );
    s_oSpringForBrushPos.start();
    
    // バネの動きは一定（※緩い動きで減衰しない）
    s_oSpringForBrushSize.set( 10, 0.01f, 1.0f );
    s_oSpringForBrushSize.setV( 10 );
    s_oSpringForBrushSize.start();
}

//------------------------------
// ブラシのブレ：更新
//------------------------------
void CBezier::UpdateBrushShake( void ){
    if( m_nResetCountForBrushPos <= 0 ){
        m_nUpdateFrameForBrushPos = 0.3f + CRand::GetRandF();
        m_nResetCountForBrushPos = 32 + CRand::GetRand( 64 );
    }else{
        m_nResetCountForBrushPos--;
    }
    s_oSpringForBrushPos.updateWithFrame( m_nUpdateFrameForBrushPos );
    
    if( m_nResetCountForBrushSize <= 0 ){
        m_nUpdateFrameForBrushSize = 0.3f + CRand::GetRandF();
        m_nResetCountForBrushSize = 32 + CRand::GetRand( 64 );
    }else{
        m_nResetCountForBrushSize--;
    }
    s_oSpringForBrushSize.updateWithFrame( m_nUpdateFrameForBrushSize );
}

//------------------------------
// ブラシのブレ：Ｘ位置
//------------------------------
float CBezier::GetBrushShakeForPosX( void ){
    return( s_oSpringForBrushPos.getY()/100.0f );
}

//------------------------------
// ブラシのブレ：Ｙ位置
//------------------------------
float CBezier::GetBrushShakeForPosY( void ){
    return( s_oSpringForBrushPos.getY()/100.0f );
}

//------------------------------
// ブラシのブレ：サイズ
//------------------------------
float CBezier::GetBrushShakeForSize( void ){
    return( s_oSpringForBrushSize.getY()/300.0f );
}
