/*+----------------------------------------------------------------+
  |	Title:		BezierForFrill.cpp [共通環境]
  |	Comment:	ベジェ曲線：フリル
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"
#include "BezierDataConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// フリルのサイズ算出（※フリルを１つ描画したら何ピクセルぐらい移動するかの目安）
//（※素材はバッファサイズに対してタテ／ヨコ５０％程度のサイズを想定しておく）
//-----------------------------------------------------------------
#define FRILL_SIZE( _size )     ((_size)*(s_nBufW+s_nBufH)/4)

//-----------------------------------------------------------------
// フリルのステップ（※半分ずらす際の割合＝若干重なるようにやや小さめにしてみる）
//-----------------------------------------------------------------
#define FRILL_STEP_RATE         (0.4f)

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
//--------------------------------------------------------------
// [Frill] 描画実体：フリル
//--------------------------------------------------------------
// [pHead]は１点で１組のデータ構成とし、複数指定される想定
//（※この処理では[pHead]を配列として扱う）
//--------------------------------------------------------------
void CBezier::DrawFrill( stBEZIER_ANCHOR_POINT* pHead ){
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

    //-----------------------------------
    // 描画レイヤー情報の取得
    //-----------------------------------
    // サイズ取得（※[X/Y]は反転用にわかれているがスケールの絶対値的には同じになるはず＝縦横の辺のサイズは等しい想定）
    float layerSizeAbs = s_pLayerParam->pixelScaleX;
    if( layerSizeAbs < 0 ){ layerSizeAbs *= -1; }
    
    // 回転
    bool isFlipH = (s_pLayerParam->pixelScaleX < 0);
    bool isFlipV = (s_pLayerParam->pixelScaleY < 0);
    float layerRot = s_pLayerParam->pixelRot;
    
    //--------------------------------------------------------------
    // 指定のアンカーポイント毎にフリルを処理する
    //--------------------------------------------------------------
    for( int i=0; i<numData; i++ ){
        stBEZIER_ANCHOR_POINT* pBAP = &pHead0[i];

        // フリルデータの取得（※無効は無視）
        CLayerData* pLD = GetLayerDataForFrill( (eSTROKE_FRILL)pBAP->frillId );
        if( pLD == NULL ){
            LOGE( "@ CBezier::Frill: INVALID FRILL DATA: at=%d, frillId=%d\n", i, pBAP->frillId );
            continue;
        }

        // タッチ対象の確認（※無効は無視）
        int target = pBAP->touchTarget;
        if( target < 0 || target >= eSTROKE_TOUCH_TARGET_MAX ){
            LOGE( "@ CBezier::DrawFrill: INVALID TOUCH TARGET: at=%d, target=%d\n", i, target );
            continue;
        }
        
        // タッチ登録点の確認（※無効は無視）
        int numPoint = s_nArrTouchPointNum[target];
        if( numPoint <= 0 ){
            LOGE( "@ CBezier::DrawFrill: NO TOUCH POINT: at=%d, target=%d\n", i, target );
            continue;
        }

        // work（※フリルの描画で傾き処理が行われるためここで扱う座標は傾き前の値）
        float* arrX0 = s_fArrArrTouchPointX0[target];
        float* arrY0 = s_fArrArrTouchPointY0[target];
        float totalLen0 = CalcTouchWorkForLen0( target );
        float baseLen0 = totalLen0 * pBAP->frillBasePos;

        // 起点となる座標の特定
        int pivotAt = SearchTouchIndexForLen0( target, baseLen0, 0, true );
        if( pivotAt < 0 ){
            LOGE( "@ CBezier::DrawFrill: PIVOT SEARCH MISS: at=%d, target=%d, baseLen0=%f\n", i, target, baseLen0 );
            continue;
        }
        
        // サイズ確認（※小さすぎる場合は無視）
        int baseFrillSize = FRILL_SIZE( pBAP->frillBaseSize * layerSizeAbs );
        if( baseFrillSize < pBAP->frillSizeMin ){
            LOGE( "@ CBezier::DrawFrill: SKIP SMALL FRILL: at=%d, target=%d, baseFrillSize=%d\n", i, target, baseFrillSize );
            continue;
        }

        //--------------------------------
        // ここまできたら起点を描画
        //--------------------------------
        float rot = pBAP->frillBaseRot;
        if( isFlipH ){ rot = CMath::FlipRotH( rot ); }
        if( isFlipV ){ rot = CMath::FlipRotV( rot ); }
        DrawFrillAt( pBAP, pLD, arrX0[pivotAt], arrY0[pivotAt], pBAP->frillBaseSize*layerSizeAbs, rot+layerRot );

        //---------------------------
        // 前方の確認
        //---------------------------
        // 末端となる座標の特定
        float frontEndLen = baseLen0 * (1.0f-pBAP->frillFrontArea);
        if( frontEndLen < 0.0f ){
            frontEndLen = 0.0f;
        }else if( frontEndLen > baseLen0 ){
            frontEndLen = baseLen0;
        }

        // 前方の領域が有効であれば描画
        float frontRange = baseLen0 - frontEndLen;
        if( frontRange > 0 ){
            int at = pivotAt;
            int curLen = baseLen0 - (baseFrillSize*layerSizeAbs*FRILL_STEP_RATE);
            
            // 端に到達するまでフリルの表示
            while( curLen > frontEndLen ){
                float rate = (1.0f*(curLen-frontEndLen))/frontRange;
                float size = (1.0f-rate)*pBAP->frillFrontSize + rate*pBAP->frillBaseSize;
                
                // サイズ確認（※小さくなりすぎたら抜ける）
                int frontFrillSize = FRILL_SIZE( size );
                if( frontFrillSize < pBAP->frillSizeMin ){
                    break;
                }
                
                // 中央揃え（※範囲をこえるのであれば抜ける）
                curLen -= frontFrillSize * layerSizeAbs * FRILL_STEP_RATE;
                if( curLen <= frontEndLen ){
                    break;
                }

                // 回転
                rot = (1.0f-rate)*pBAP->frillFrontRot + rate*pBAP->frillBaseRot;
                if( isFlipH ){ rot = CMath::FlipRotH( rot ); }
                if( isFlipV ){ rot = CMath::FlipRotV( rot ); }

                // 表示
                at = SearchTouchIndexForLen0( target, curLen, at, false );
                DrawFrillAt( pBAP, pLD, arrX0[at], arrY0[at], size*layerSizeAbs, rot+layerRot );

                // フリルの端へ
                curLen -= frontFrillSize*layerSizeAbs*FRILL_STEP_RATE;
            }
        }

        //---------------------------
        // 後方の確認
        //---------------------------
        float backEndLen = baseLen0 + (totalLen0-baseLen0)*pBAP->frillFrontArea;
        if( backEndLen < baseLen0 ){
            backEndLen = baseLen0;
        }else if( backEndLen > totalLen0 ){
            backEndLen = totalLen0;
        }

        // 後方の領域が有効であれば描画
        float backRange = backEndLen - baseLen0;
        if( backRange > 0 ){
            int at = pivotAt;
            int curLen = baseLen0 + (baseFrillSize*layerSizeAbs*FRILL_STEP_RATE);
            
            // 端に到達するまでフリルの表示
            while( curLen < backEndLen ){
                float rate = (1.0f*(backEndLen-curLen))/backRange;
                float size = (1.0f-rate)*pBAP->frillBackSize + rate*pBAP->frillBaseSize;
                
                // サイズ確認（※小さくなりすぎたら抜ける）
                int backFrillSize = FRILL_SIZE( size );
                if( backFrillSize < pBAP->frillSizeMin ){
                    break;
                }
                
                // 中央揃え（※範囲をこえるのであれば抜ける）
                curLen += backFrillSize * layerSizeAbs * FRILL_STEP_RATE;
                if( curLen >= backEndLen ){
                    break;
                }

                rot = (1.0f-rate)*pBAP->frillBackRot + rate*pBAP->frillBaseRot;
                if( isFlipH ){ rot = CMath::FlipRotH( rot ); }
                if( isFlipV ){ rot = CMath::FlipRotV( rot ); }

                // 表示
                at = SearchTouchIndexForLen0( target, curLen, at, true );
                DrawFrillAt( pBAP, pLD, arrX0[at], arrY0[at], size*layerSizeAbs, rot+layerRot );
                
                // フリルの端へ
                curLen += backFrillSize * layerSizeAbs * FRILL_STEP_RATE;
            }
        }
    }
}

//----------------------------------------------------------------------------------
// １フリルの表示（※返値はポイント数＝次のフリルを表示するためのオフセット）
//----------------------------------------------------------------------------------
//（※フリルのデータはラインを引いて塗りつぶすだけのシンプルなデータを想定＝最低限のフラグ等にのみ対応）
//（※[pLD]は素材中央を原点とし、[50%]サイズで作成されたデータとする）
//（※[size/rot]は画面サイズに対して調整された値とする）
//----------------------------------------------------------------------------------
void CBezier::DrawFrillAt( stBEZIER_ANCHOR_POINT* pBAP, CLayerData* pLD, float x, float y, float size, float rot ){
    //-------------------------------
    // フリル表示用レイヤーパラメータの準備
    //-------------------------------
    stBEZIER_LAYER_PARAM layerParam;
    CLEAR_BEZIER_LAYER_PARAM( &layerParam );
    layerParam.pixelScaleX = size;
    layerParam.pixelScaleY = size;
    layerParam.pixelRot = rot;
    
    //--------------------
    // 描画
    //--------------------
    CLayerObject* pObj = pLD->getDataHead();
    while( pObj != NULL ){
        //---------------------------
        // ラインであれば
        //---------------------------
        if( pObj->isLineObject() ){
            CLineObjectData* pLOD = pObj->getLineObject();
            
            // アンカーポイント変換
            stBEZIER_ANCHOR_POINT* pHead0 = CBezierDataConv::CreateBezierAnchorPoint( pLOD, s_pBaseParam, &layerParam );
            stBEZIER_ANCHOR_POINT* pHead = pHead0;
            
            // アンカーポイントの調整
            while( pHead != NULL ){
                // 指定座標平行移動
                pHead->x += x;
                pHead->y += y;
                
                // ブラシ設定のコピー
                pHead->type = pBAP->type;
                pHead->size = pBAP->size;
                pHead->ofsRange = pBAP->ofsRange;
                pHead->dotPutMax = pBAP->dotPutMax;
                pHead->dotPutRateBase = pBAP->dotPutRateBase;
                pHead->dotPutRateStrong = pBAP->dotPutRateStrong;
                pHead->dotPutRateWeak = pBAP->dotPutRateWeak;
                pHead->isDotPutForce = pBAP->isDotPutForce;
                pHead->isDotPutWhetherMax = pBAP->isDotPutWhetherMax;
                pHead->isDotPutOnOut = pBAP->isDotPutOnOut;

                // ストローク設定のコピー
                pHead->strokeSize = pBAP->strokeSize;
                pHead->strokeStartSize = pBAP->strokeStartSize;
                pHead->strokeStartRange = pBAP->strokeStartRange;
                pHead->strokeEndSize = pBAP->strokeEndSize;
                pHead->strokeEndRange = pBAP->strokeEndRange;
                
                // 傾き設定のコピー
                pHead->angleLocalSearchKey = pBAP->angleLocalSearchKey;
                pHead->angleLocalRateLR = pBAP->angleLocalRateLR;
                pHead->angleLocalRateUD = pBAP->angleLocalRateUD;

                pHead->angleSearchKey = pBAP->angleSearchKey;
                pHead->angleRateLR = pBAP->angleRateLR;
                pHead->angleRateUD = pBAP->angleRateUD;

                // 用心（※閉じていたら抜ける）
                if( pHead->pNext == pHead0 ){
                    break;
                }
                
                pHead = (stBEZIER_ANCHOR_POINT*) pHead->pNext;
            }

            // ワーククリア
            ClearWorkPoint();
            
            // 出力先を切り替えて描画
            SetBufForStroke( s_pBaseParam, s_pLayerParam, s_bWorkPath );
            DrawStroke( pHead0 );
            
            // アンカーポイント解放
            CBezierDataConv::ReleaseBezierAnchorPoint( pHead0 );
            
            // 指定があればガイドのクリア
            if( pLOD->checkFlag( eLOD_FLAG_GUIDE_RESET_AFTER_STROKE ) ){
                bool isFull = pLOD->checkFlag( eLOD_FLAG_GUIDE_RESET_FULL );
                CBmpGenerator::ResetGuide( isFull );
            }
        }
        //--------------------
        // 塗りであれば
        //--------------------
        else if( pObj->isPaintObject() ){
            CPaintObjectData* pPOD = pObj->getPaintObject();
            
            // オブジェクトが有効であれば描画
            if( ! pPOD->checkFlag( ePOD_FLAG_DISABLE ) ){
                stBEZIER_FILL_POINT* pHead = CBezierDataConv::CreateBezierFillPoint( pPOD, s_pBaseParam, &layerParam );

                // 指定座標平行移動
                pHead->x += x;
                pHead->y += y;

                // 傾き設定のコピー
                pHead->angleLocalSearchKey = pBAP->angleLocalSearchKey;
                pHead->angleLocalRateLR = pBAP->angleLocalRateLR;
                pHead->angleLocalRateUD = pBAP->angleLocalRateUD;
                pHead->angleSearchKey = pBAP->angleSearchKey;
                pHead->angleRateLR = pBAP->angleRateLR;
                pHead->angleRateUD = pBAP->angleRateUD;

                // 出力先を切り替えて描画
                SetBufForFill( s_pBaseParam, s_pLayerParam, s_bWorkPath );
                DrawFill( pHead );
                
                // 塗りポイント解放
                CBezierDataConv::ReleaseBezierFillPoint( pHead );
                
                // ガイドは塗るたびにクリア（※指定があれば持ち越す）
                if( ! pPOD->checkFlag( ePOD_FLAG_STAY_GUIDE_AFTER_FILL ) ){
                    bool isFull = pPOD->checkFlag( ePOD_FLAG_GUIDE_RESET_FULL );
                    CBmpGenerator::ResetGuide( isFull );
                }
            }
        }
        
        pObj = pObj->getNext();
    }
}
