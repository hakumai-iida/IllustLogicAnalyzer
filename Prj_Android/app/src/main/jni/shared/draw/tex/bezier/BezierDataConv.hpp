/*+----------------------------------------------------------------+
  |	Title:		BezierDataConv.hpp [共通環境]
  |	Comment:	ベジェ関連データ変換
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BEZIER_DATA_CONV_HPP__
#define __BEZIER_DATA_CONV_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BezierStruct.hpp"
#include "LineObjectData.hpp"
#include "PaintObjectData.hpp"
#include "SlotObjectData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ブラシの強制を有効にする
#define ENABLE_FIXED_BRUSH

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//------------------------------------------------------------------------------
// ベジェ関連データ変換
//------------------------------------------------------------------------------
// 各[rateOfsX0, rateOfsY0, scaleRateX0, scaleRateY0]は調整済みの値であることが前提
//（※[AdjustablePoint]で行われる傾向値等の調整と不整合が生じると予期せぬ表示になる点に注意）
//------------------------------------------------------------------------------
class CBezierDataConv{
#ifdef ENABLE_FIXED_BRUSH
private:
    static eBRUSH s_eFixedBrush;
    
public:
    inline static void SetFixedBrush( eBRUSH brush ){
        s_eFixedBrush = brush;
    }
#endif
    
public:
    //-----------------------------------------------------------------------------------------------
    // アンカーポイントの作成（※内部でメモリを確保する）
    // [CLineObjectData:割合座標＆中心原点]から[stBEZIER_ANCHOR_POINT:実座標＆左上原点]に変換したデータを返す
    //-----------------------------------------------------------------------------------------------
    static stBEZIER_ANCHOR_POINT* CreateBezierAnchorPoint( CLineObjectData* pLOD,
                                                           stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam );
    
    // アンカーポイントの解放（※引数は[CreateBezierAnchorPoint]で確保した領域を想定）
    static void ReleaseBezierAnchorPoint( stBEZIER_ANCHOR_POINT* pHead );

    //------------------------------------------------------------------------------------------
    // 塗りポイントの作成（※内部でメモリを確保する）
    // [CFillPointData:割合座標＆中心原点]から[stBEZIER_FILL_POINT:実座標＆左上原点]に変換したデータを返す
    //------------------------------------------------------------------------------------------
    static stBEZIER_FILL_POINT* CreateBezierFillPoint( CPaintObjectData* pPOD,
                                                       stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam );

    // 塗りポイントの解放（※引数は[CreateBezierFillPoint]で確保した領域を想定）
    static void ReleaseBezierFillPoint( stBEZIER_FILL_POINT* pHead );
    
    //------------------------------------------------------------------------------------------
    // スロットポイントの作成（※内部でメモリを確保する）
    // [CSlotPointData:割合座標＆中心原点]から[stBEZIER_SLOT_POINT:実座標＆左上原点]に変換したデータを返す
    //------------------------------------------------------------------------------------------
    static stBEZIER_SLOT_POINT* CreateBezierSlotPoint( CSlotObjectData* pSOD,
                                                       stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam );

    // スロットポイントの解放（※引数は[CreateBezierSlotPoint]で確保した領域を想定）
    static void ReleaseBezierSlotPoint( stBEZIER_SLOT_POINT* pHead );

private:
    // インスタンス作成は不可
    CBezierDataConv( void ){}
    virtual ~CBezierDataConv( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __BEZIER_DATA_CONV_HPP__ */
