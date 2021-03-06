/*+----------------------------------------------------------------+
  |	Title:		SlotPointData.hpp [共通環境]
  |	Comment:	スロットポイントデータ（※[CSlotObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SLOT_POINT_DATA_HPP__
#define __SLOT_POINT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/tex/adjust/AdjustablePoint.hpp"

#include "sys/data/ListNode.hpp"
#include "sys/data/DataCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eSPD_FLAG{
	eSPD_FLAG_INVALID = -1,         // 無効値

    eSPD_FLAG_DISABLE,	            // 00:[*] 無効
    eSPD_FLAG_FLIP_H,               // 01:[fH] 対象のスロットをヨコ方向に反転するか？
    eSPD_FLAG_FLIP_V,               // 02:[fV] 対象のスロットをタテ方向に反転するか？
    
    eSPD_FLAG_MAX,		            // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eSPD_FLAG_TEMP_SELECTED = 16,   // 編集対象として選択されている（※表示切り替え用）
    eSPD_FLAG_TEMP_ACTIVE,          // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// スロットポイントデータ
//----------------------------------------
class CSlotPointData: public CListNode{
    // 各種実装
    IMPLEMENT_ADJUSTABLE_XY                             // 実装：調整XY座標
    IMPLEMENT_NODE( CSlotPointData )                    // 実装：リストノード
    IMPLEMENT_DATA( CSlotPointData )                    // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eSPD_FLAG )                    // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eSPD_FLAG )                // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                               // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CSlotPointData )   // 実装：クラスメソッドによるインスタンス確保／解放
    
public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                  // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CAdjustablePoint m_oAdjXY;    // [X]: 座標（※画像サイズに対する割合座標）

public:
    // コンストラクタ／デストラクタ
    CSlotPointData( void );
    virtual ~CSlotPointData( void ){}
    
    //------------
    // 設定
    //------------
    void set( int rX, int rY );

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __SLOT_POINT_DATA_HPP__
