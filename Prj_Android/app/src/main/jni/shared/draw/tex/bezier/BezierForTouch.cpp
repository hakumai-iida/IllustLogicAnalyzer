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
//-----------------------------------------------------------------
// [Touch] タッチ対象の座標に塗りガイド描画
//-----------------------------------------------------------------
bool CBezier::DrawFillGuideForTouch( stBEZIER_BASE_PARAM* pParam ){
    // 無効は無視
    if( pParam == NULL ){
        LOGE( "@ CBezier::DrawFillGuideForTouch: INVALID PARAM\n" );
        return( false );
    }
    
    // 無効は無視
    if( pParam->pBufFillGuide == NULL || pParam->texW <= 0 || pParam->texH <= 0 ){
        LOGE( "@ CBezier::DrawFillGuideForTouch: INVALID DATA: pBufFillGuide=%p, texW=%d, texH=%d\n",
              pParam->pBufFillGuide, pParam->texW, pParam->texH );
        return( false );
    }
    
    // work
    BYTE* pBuf = pParam->pBufFillGuide;

    // [pBuf]はクリアしないでおく（※ガイドのクリアは都度行われているはずなのでここではクリアしない＝必要なら[LOD]のフラグを指定する）
    //memset( pBuf, 0, pParam->texW*pParam->texH );

    // タッチの登録座標をガイドに反映（※ここのタッチ座標は傾き適用後の値）
    for( int i=0; i<eSTROKE_TOUCH_TARGET_MAX; i++ ){
        int numPoint = s_nArrTouchPointNum[i];
        if( numPoint > 0 ){
            float* arrX = s_fArrArrTouchPointX[i];
            float* arrY = s_fArrArrTouchPointY[i];
            for( int j=0; j<numPoint; j++ ){
                int at = ((int)arrY[j])*pParam->texW + ((int)arrX[j]);
                pBuf[at] = BEZIER_PAL_UNDERCOAT_COL;
            }
        }
    }
    
    return( true );
}

//--------------------------------------------------------------
// [Touch] 描画実体：タッチ
//--------------------------------------------------------------
// [pHead]は２点で１組のデータ構成とし、複数の線が指定される想定
//（※この処理では[pHead]を配列として扱い、リスト構造を崩してしまう点に注意）
//--------------------------------------------------------------
void CBezier::DrawTouch( stBEZIER_ANCHOR_POINT* pHead ){
    // ワークパス表示時は無視（※対応しない）
    if( s_bWorkPath ){
        return;
    }
    
    stBEZIER_ANCHOR_POINT* pHead0 = pHead;

    //-------------------------
    // データ数を調べる
    //-------------------------
    int numData = 0;
    while( pHead != NULL ){
        numData++;
        
        // 次の要素へ
        pHead = (stBEZIER_ANCHOR_POINT*) pHead->pNext;
        
        // 用心(※クローズパスであれば抜けておく）
        if( pHead == pHead0 ){
            break;
        }
    }
    
    // ２の倍数でなければ無視（※ペア構成が不正）
    if( numData <= 0 || (numData%2) != 0 ){
        LOGE( "@ CBezier::DrawTouch: INVALID DATA NUM: numData=%d\n", numData );
        return;
    }

    //-----------------------------------
    // 描画レイヤー情報の取得
    //-----------------------------------
    bool isFlipH = (s_pLayerParam->pixelScaleX < 0);
    bool isFlipV = (s_pLayerParam->pixelScaleY < 0);
    
    // work
    bool isLastValid = false;
    bool isSubNext = false;
    float curCenterX = 0.0f;
    float curCenterY = 0.0f;
    float lastCenterX = 0.0f;
    float lastCenterY = 0.0f;
    float lastCenterX0 = 0.0f;
    float lastCenterY0 = 0.0f;

    //--------------------------------------------------------------
    // 対象をストロークとして、ペア毎に描画する（※[pHead0]は配列として利用する）
    //--------------------------------------------------------------
    int numPair = numData/2;
    for( int i=0; i<numPair; i++ ){
        // ペアを取り出し２要素のリストにする（※元のリスト構造が破壊される）
        stBEZIER_ANCHOR_POINT* pPairHead = &pHead0[2*i];
        pPairHead[0].pNext = &pPairHead[1];
        pPairHead[1].pNext = NULL;

        // 情報の参照先はペアの先頭
        stBEZIER_ANCHOR_POINT* pBAP = &pPairHead[0];

        // 対象の確認（※無効であれば無視）
        int target = pBAP->touchTarget;
        if( target < 0 || target >= eSTROKE_TOUCH_TARGET_MAX ){
            LOGE( "@ CBezier::DrawTouch: INVALID TARGET: at=%d, target=%d\n", 2*i, target );
            continue;
        }

        // 登録情報の取得
        int numPoint = s_nArrTouchPointNum[target];
        if( numPoint <= 0 ){
            continue;
        }
        
        // ストライプワークのクリア
        ClearWorkStripePoint();
        
        // タッチ登録点の距離の算出
        float totalLen = CalcTouchWorkForLen0( target );

        // 対象座標（※ストローク処理で傾き処理が行われるためここで扱う座標は傾き前の値）
        float* arrX0 = s_fArrArrTouchPointX0[target];
        float* arrY0 = s_fArrArrTouchPointY0[target];
        
        // 起点となる座標の特定
        float baseLen = totalLen * pBAP->touchBasePos;
        if( baseLen < 0.0f ){
            baseLen = 0.0f;
        }else if( baseLen > totalLen ){
            baseLen = totalLen;
        }
        
        int pivotAt = SearchTouchIndexForLen0( target, baseLen, 0, true );
        if( pivotAt < 0 ){
            LOGE( "@ CBezier::DrawTouch: SEARCH MISS: baseLen=%f, target=%d\n", baseLen, target );
            return;
        }

        //--------------------------------
        // 起点を描画
        //--------------------------------
        float rot = pBAP->touchBaseRot;
        if( isFlipH ){ rot *= -1.0f; }
        if( isFlipV ){ rot *= -1.0f; }

        // １つおきステップが無効なら描画（※有効な場合は前後の描画時に調整）
        if( pBAP->touchFrontStep2 <= 0 && pBAP->touchBackStep2 <= 0 ){
            DrawTouchAt( pPairHead, arrX0[pivotAt], arrY0[pivotAt], pBAP->touchBaseSize, rot, &lastCenterX0, &lastCenterY0 );
            isLastValid = true;
        }
        
        //---------------------------
        // 前方の確認
        //---------------------------
        // ステップ幅：前方（※用心に最低間隔を設けておく）
        float step = s_nBufW * pBAP->touchFrontStep;
        if( step < pBAP->touchStepMarginMin ){
            step = pBAP->touchStepMarginMin;
        }
        
        // 一つおきのステップ幅：前方
        float step2 = s_nBufW * pBAP->touchFrontStep2;
        
        // ステップ調整値：前方
        float stepRnd = s_nBufW * pBAP->touchFrontStepRnd;

        isSubNext = false;   // メインから開始する
        lastCenterX = lastCenterX0;
        lastCenterY = lastCenterY0;

        // 末端となる座標の特定
        float frontEndLen = baseLen * (1.0f-pBAP->touchFrontArea);
        if( frontEndLen < 0.0f ){
            frontEndLen = 0.0f;
        }else if( frontEndLen > baseLen ){
            frontEndLen = baseLen;
        }
        
        int frontEndAt = SearchTouchIndexForLen0( target, frontEndLen, pivotAt, false );
        if( frontEndAt < 0 ){
            LOGE( "@ CBezier::DrawTouch: SEARCH MISS: frontEndLen=%f, target=%d\n", frontEndLen, target );
            return;
        }
        if( frontEndAt > pivotAt ){
            frontEndAt = pivotAt;
        }

        // 前方の領域が有効であれば描画
        float frontRange = baseLen - frontEndLen;
        if( frontRange > 0.0f ){
            int numTouch = 0;
            int at = pivotAt;
            int at0 = at;

            // ステップ開始位置（※一つおきが有効ならその片側を表示：もう片方は後方描画で行う）
            float curLen = baseLen;
            if( step2 > 0.0f ){
                curLen -= step/2.0f;
                numTouch++;
            }else{
                curLen -= step + CRand::GetRandF( stepRnd );
            }

            // 端に到達するまでタッチの表示
            while( curLen > frontEndLen ){
                float rate = (1.0f*(curLen-frontEndLen))/frontRange;
                float size = (1.0f-rate)*pBAP->touchFrontSize + rate*pBAP->touchBaseSize;
                
                // 回転
                rot = (1.0f-rate)*pBAP->touchFrontRot + rate*pBAP->touchBaseRot;
                if( isFlipH ){ rot *= -1; }
                if( isFlipV ){ rot *= -1; }
                
                // 表示
                at = SearchTouchIndexForLen0( target, curLen, at, false );
                if( at < 0 ){
                    LOGE( "@ CBezier::DrawTouch: SEARCH MISS FOR FRONT: curLen=%f, target=%d\n", curLen, target );
                    return;
                }
                DrawTouchAt( pBAP, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY );
                
                // 次の位置へ
                at0 = at;
                curLen -= step + CRand::GetRand( stepRnd );
                if( ((++numTouch)%2) == 0 ){
                    curLen -= step2;
                }

                //----------------------------
                // ストライプ登録
                //----------------------------
                // 直前値が無効であれば設定のみ
                if( ! isLastValid ){
                    lastCenterX0 = curCenterX;
                    lastCenterY0 = curCenterY;
                    isLastValid = true;
                }else{
                    // パレットが有効であれば登録
                    BYTE palOfsGrp = (isSubNext)? pBAP->stripeSubPalGrp: pBAP->stripeMainPalGrp;
                    if( palOfsGrp > 0 ){
                        AddStripePoint( palOfsGrp, (lastCenterX+curCenterX)/2.0f, (lastCenterY+curCenterY)/2.0f );
                    }
                    
                    isSubNext = ! isSubNext;
                }

                lastCenterX = curCenterX;
                lastCenterY = curCenterY;
            }
            
            // 前方端の塗りつぶし指定があれば
            if( pBAP->isStripeFillFront ){
                // 直近のタッチストロークの基準点からほんの少しずらした位置を塗り地点としておく
                if( isLastValid ){
                    // タッチの方向性の算出
                    float deltaX = arrX0[frontEndAt] - arrX0[at0];
                    float deltaY = arrY0[frontEndAt] - arrY0[at0];
                    float len = CMath::Len( deltaX, deltaY );
                    
                    if( len > 0.0f ){
                        float addX = 2*pBAP->touchStepMarginMin*deltaX/len;
                        float addY = 2*pBAP->touchStepMarginMin*deltaY/len;

                        BYTE palOfsGrp = (isSubNext)? pBAP->stripeSubPalGrp: pBAP->stripeMainPalGrp;
                        AddStripePoint( palOfsGrp, lastCenterX+addX, lastCenterY+addY );
                    }
                }
            }
        }

        //---------------------------
        // 後方の確認
        //---------------------------
        // ステップ幅：後方（※用心に最低間隔を設けておく）
        step = s_nBufW * pBAP->touchBackStep;
        if( step < pBAP->touchStepMarginMin ){
            step = pBAP->touchStepMarginMin;
        }
        
        // 一つおきのステップ幅：後方
        step2 = s_nBufW * pBAP->touchBackStep2;
        
        // ステップ調整値：後方（※乱数で足しこむ用）
        stepRnd = s_nBufW * pBAP->touchBackStepRnd;

        isSubNext = true;   // サブから開始する
        lastCenterX = lastCenterX0;
        lastCenterY = lastCenterY0;
        
        float backEndLen = baseLen + (totalLen-baseLen)*pBAP->touchBackArea;
        if( backEndLen < baseLen ){
            backEndLen = baseLen;
        }else if( backEndLen > totalLen ){
            backEndLen = totalLen;
        }
        
        int backEndAt = SearchTouchIndexForLen0( target, backEndLen, pivotAt, true );
        if( backEndAt < 0 ){
            LOGE( "@ CBezier::DrawTouch: SEARCH MISS: backEndLen=%f, target=%d\n", backEndLen, target );
            return;
        }
        if( backEndAt < pivotAt ){
            backEndAt = pivotAt;
        }
        
        // 後方の領域が有効であれば描画
        float backRange = backEndLen - baseLen;
        if( backRange > 0 ){
            int numTouch = 0;
            int at = pivotAt;
            int at0 = at;
            
            // ステップ開始位置（※一つおきが有効ならその片側を表示：もう片方は後方描画で行う）
            float curLen = baseLen;
            if( step2 > 0.0f ){
                curLen += step/2.0f;
                numTouch++;
            }else{
                curLen += step + CRand::GetRandF( stepRnd );
            }

            // 端に到達するまで繰り返す
            while( curLen < backEndLen ){
                float rate = (1.0f*(backEndLen-curLen))/backRange;
                float size = (1.0f-rate)*pBAP->touchBackSize + rate*pBAP->touchBaseSize;
                
                // 回転
                rot = (1.0f-rate)*pBAP->touchBackRot + rate*pBAP->touchBaseRot;
                if( isFlipH ){ rot *= -1; }
                if( isFlipV ){ rot *= -1; }
                
                // 表示
                at = SearchTouchIndexForLen0( target, curLen, at, true );
                if( at < 0 ){
                    LOGE( "@ CBezier::DrawTouch: SEARCH MISS FOR BACK: curLen=%f, target=%d\n", curLen, target );
                    return;
                }
                DrawTouchAt( pPairHead, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY );
                
                // 次の位置へ
                at0 = at;
                curLen += step + CRand::GetRand( stepRnd );
                if( ((++numTouch)%2) == 0 ){
                    curLen += step2;
                }
                
                //----------------------------
                // ストライプ登録
                //----------------------------
                // 直前値が無効であれば設定のみ
                if( ! isLastValid ){
                    lastCenterX0 = curCenterX;
                    lastCenterY0 = curCenterY;
                    isLastValid = true;
                }else{
                    // パレットが有効であれば登録
                    BYTE palOfsGrp = (isSubNext)? pBAP->stripeSubPalGrp: pBAP->stripeMainPalGrp;
                    if( palOfsGrp > 0 ){
                        AddStripePoint( palOfsGrp, (lastCenterX+curCenterX)/2.0f, (lastCenterY+curCenterY)/2.0f );
                    }
                    
                    isSubNext = ! isSubNext;
                }

                lastCenterX = curCenterX;
                lastCenterY = curCenterY;
            }
            
            // 後方端の塗りつぶし指定があれば
            if( pBAP->isStripeFillBack ){
                // 直近のタッチストロークの基準点からほんの少しずらした位置を塗り地点としておく
                if( isLastValid ){
                    // タッチの方向性の算出
                    float deltaX = arrX0[backEndAt] - arrX0[at0];
                    float deltaY = arrY0[backEndAt] - arrY0[at0];
                    float len = CMath::Len( deltaX, deltaY );
                    
                    if( len > 0.0f ){
                        float addX = 2.0f*pBAP->touchStepMarginMin*deltaX/len;
                        float addY = 2.0f*pBAP->touchStepMarginMin*deltaY/len;
                        
                        BYTE palOfsGrp = (isSubNext)? pBAP->stripeSubPalGrp: pBAP->stripeMainPalGrp;
                        AddStripePoint( palOfsGrp, lastCenterX+addX, lastCenterY+addY );
                    }
                }
            }
        }
        
        //-------------------------------
        // 指定があればストライプ点の塗りつぶし
        //-------------------------------
        if( s_bStripe ){
            for( int j=0; j<s_nStripePointNum; j++ ){
                float x = s_fArrStripePointX[j];
                float y = s_fArrStripePointY[j];
                BYTE palOfsGrp = s_nArrStripePalGrp[j];
                
                // 塗りつぶし（※とりあえず「単色塗り」）
                CFill* pFill = CFillMgr::GetFill( eFILL_TYPE_MONO );
                pFill->fill( s_pBaseParam->pBufColor, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH,
                             (int)x, (int)y, palOfsGrp, BEZIER_PAL_VAL_BASE, s_nTestPalGrp );
            }
        }
    }
}

//-------------------------------
// [Touch] タッチ描画：単体
//-------------------------------
void CBezier::DrawTouchAt( stBEZIER_ANCHOR_POINT* pPairHead, float x, float y, float size, float rot, float* pCenterX, float* pCenterY ){
    // ストロークデータの退避
    float x0 = pPairHead[0].x;
    float y0 = pPairHead[0].y;
    float x1 = pPairHead[1].x;
    float y1 = pPairHead[1].y;
    
    float xIn0 = pPairHead[0].xIn;
    float yIn0 = pPairHead[0].yIn;
    float xIn1 = pPairHead[1].xIn;
    float yIn1 = pPairHead[1].yIn;
    
    float xOut0 = pPairHead[0].xOut;
    float yOut0 = pPairHead[0].yOut;
    float xOut1 = pPairHead[1].xOut;
    float yOut1 = pPairHead[1].yOut;

    // オフセット算出＆回転
    float ofsX, ofsY;
    if( rot != 0.0f ){
        ofsX = CMath::RotX( x1-x0, y1-y0, rot );
        ofsY = CMath::RotY( x1-x0, y1-y0, rot );

        pPairHead[0].xIn = CMath::RotX( xIn0, yIn0, rot );
        pPairHead[0].yIn = CMath::RotY( xIn0, yIn0, rot );
        pPairHead[1].xIn = CMath::RotX( xIn1, yIn1, rot );
        pPairHead[1].yIn = CMath::RotY( xIn1, yIn1, rot );
        
        pPairHead[0].xOut = CMath::RotX( xOut0, yOut0, rot );
        pPairHead[0].yOut = CMath::RotY( xOut0, yOut0, rot );
        pPairHead[1].xOut = CMath::RotX( xOut1, yOut1, rot );
        pPairHead[1].yOut = CMath::RotY( xOut1, yOut1, rot );
    }else{
        ofsX = x1 - x0;
        ofsY = y1 - y0;
    }
    
    // タッチ位置から線が引かれるように調整（※平行移動）
    pPairHead[0].x = x;
    pPairHead[0].y = y;
    pPairHead[1].x = pPairHead[0].x + ofsX;
    pPairHead[1].y = pPairHead[0].y + ofsY;
    
    // ワーククリア
    ClearWorkPoint();
        
    // ストローク描画
    DrawStroke( pPairHead, size );
    
    // 実際に出力したガイド座標の値を基準点とし呼び出し元へ返す（※ストライプの塗りつぶし開始点）
    if( s_nNumStripeGuidePoint > 0 ){
        // 塗りがガイド範囲外にならないように、始点からすこし移動したポイントを基準点とする
        int pivot = 2.0f*pPairHead->touchStepMarginMin;
    
        // 一応用心
        if( pivot >= s_nNumStripeGuidePoint ){
            pivot = s_nNumStripeGuidePoint-1;
        }

        if( pCenterX != NULL ){ *pCenterX = s_nArrStripeGuidePointX[pivot]; }
        if( pCenterY != NULL ){ *pCenterY = s_nArrStripeGuidePointY[pivot]; }
    }

    // ストロークデータの復旧
    pPairHead[0].x = x0;
    pPairHead[0].y = y0;
    pPairHead[1].x = x1;
    pPairHead[1].y = y1;

    if( rot != 0.0f ){
        pPairHead[0].xIn = xIn0;
        pPairHead[0].yIn = yIn0;
        pPairHead[1].xIn = xIn1;
        pPairHead[1].yIn = yIn1;
        
        pPairHead[0].xOut = xOut0;
        pPairHead[0].yOut = yOut0;
        pPairHead[1].xOut = xOut1;
        pPairHead[1].yOut = yOut1;
    }
}

//-----------------------------------------------------------------------
// タッチの長さをワークへ設定
//-----------------------------------------------------------------------
// １座標毎の距離はほぼ１なので、「s_fArrTouchWorkForLen0[i] = i;」としたいのだが、
// 微妙に距離がばらつくので真面目に計算している（※もう少しなんとかしたい所）
//-----------------------------------------------------------------------
float CBezier::CalcTouchWorkForLen0( int target ){
    if( target < 0 || target >= eSTROKE_TOUCH_TARGET_MAX ){
        LOGE( "@ CBezier::CalcTouchWorkForLen0: INVALID TARGET: target=%d\n", target );
        return( 0.0f );
    }

    // 開始点の距離は０
    s_fArrTouchWorkForLen0[0] = 0.0f;

    // ２点未満であれば返す
    int numPoint = s_nArrTouchPointNum[target];
    if( numPoint < 2 ){ return( 0.0f ); }

    // work（※ストローク処理で傾き処理が行われるためここで扱う座標は傾き前の値）
    float* arrX0 = s_fArrArrTouchPointX0[target];
    float* arrY0 = s_fArrArrTouchPointY0[target];
    
    // 各点までの移動距離を累積で設定
    for( int i=1; i<numPoint; i++ ){
        float divX = arrX0[i] - arrX0[i-1];
        float divY = arrY0[i] - arrY0[i-1];
        s_fArrTouchWorkForLen0[i] = s_fArrTouchWorkForLen0[i-1] + CMath::Len( divX, divY );
    }

    // タッチポイント全ての距離を返しておく
    return( s_fArrTouchWorkForLen0[numPoint-1] );
}

//---------------------------
// タッチ点インデックスの検索
//---------------------------
int CBezier::SearchTouchIndexForLen0( int target, float len, int from, bool isBack ){
    // ２点未満であれば返す
    int numPoint = s_nArrTouchPointNum[target];
    if( numPoint < 2 ){ return( -1 ); }

    // 後方へ
    if( isBack ){
        for( int i=from; i<numPoint; i++ ){
            if( s_fArrTouchWorkForLen0[i] >= len ){
                return( i );
            }
        }
    }
    // 前方へ
    else{
        for( int i=from; i>=0; i-- ){
            if( s_fArrTouchWorkForLen0[i] <= len ){
                return( i );
            }
        }
    }

    // ここまできたら終端
    return( -1 );
}
