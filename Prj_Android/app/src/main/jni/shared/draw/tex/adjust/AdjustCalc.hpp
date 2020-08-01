/*+----------------------------------------------------------------+
  |	Title:		AdjustCalc.hpp [共通環境]
  |	Comment:	調整計算
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ADJUST_CALC_HPP__
#define __ADJUST_CALC_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AdjustablePoint.hpp"
#include "SlotAdjuster.hpp"
#include "DelayLog.hpp"

#include "draw/tex/bezier/LayerData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define FIX_ADJUST_RATE( _adjustRate )              \
    if( (_adjustRate) < -1.0f ){                    \
        (_adjustRate) = -1.0f;                      \
    }else if( (_adjustRate) > +1.0f ){              \
        (_adjustRate) = +1.0f;                      \
    }

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 調整計算
//-----------------------------------------------------------------
class CAdjustCalc{
public:
    //-------------------------------------------------------------
    // 座標計算：調整値[Ｈ/Ｖ]
    //-------------------------------------------------------------
    static void CalcRateXYForHV( int* pOutRateX, int* pOutRateY, int rX0, int rY0,
                                 CAdjustablePoint* pAP, float fixedAdjustRateH, float fixedAdjustRateV );

    //-----------------------------------------------------------------------------
    // 半径計算: 調整値[Ｈ]
    //-----------------------------------------------------------------------------
    static void CalcRateRForH( int *pOutRateR, int *pOutRateOfsR, int rR0, int rOfsForCenter0,
                               CSlotAdjuster* pSA, float fixedAdjustRateH );

    //-------------------------------------------------------------
    // 座標計算：遅延ログ
    //-------------------------------------------------------------
    static void CalcRateXYForDelayLog( int* pOutRateX, int* pOutRateY, int rX0, int rY0,
                                       bool isReverseH, bool isReverseV, float rot,
                                       CAdjustablePoint* pAP, CDelayLog* pDelayLog );
    
    //-------------------------------------------------------------
    // 角度計算：遅延ログ
    //-------------------------------------------------------------
    static void CalcRateRotForDelayLog( int* pOutRateRot,
                                        bool isReverseH, bool isReverseV,
                                        CLayerData* pLD, CDelayLog* pDelayLog );

    //-------------------------------------------------------------
    // 角度計算：テンション
    //-------------------------------------------------------------
    static void CalcRateRotForT( int* pOutRateRot, CSlotAdjuster* pSA, float fixedAdjustRateT );

    //-------------------------------------------------------------
    // サイズ計算：サイズ
    //-------------------------------------------------------------
    static void CalcRateScaleForS( int* pOutRateScale, CSlotAdjuster* pSA, float fixedAdjustRateS );

private:
    // インスタンス作成は不可
    CAdjustCalc( void ){}
    virtual ~CAdjustCalc( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __ADJUST_CALC_HPP__ */
