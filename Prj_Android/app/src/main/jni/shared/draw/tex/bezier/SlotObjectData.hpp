/*+----------------------------------------------------------------+
  |	Title:		SlotObjectData.hpp [共通環境]
  |	Comment:	スロットデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SLOT_OBJECT_DATA_HPP__
#define __SLOT_OBJECT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerObject.hpp"
#include "SlotPointData.hpp"

#include "sys/data/DataCell.hpp"
#include "sys/data/DataList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eSOD_FLAG{
	eSOD_FLAG_INVALID = -1,         // 無効値

	eSOD_FLAG_DISABLE,	            // 00:[*] 無効
    
    eSOD_FLAG_MAX,		            // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eSOD_FLAG_TEMP_SELECTED = 16,   // 編集対象として選択されている（※表示切り替え用）
    eSOD_FLAG_TEMP_ACTIVE,          // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// スロットデータ
//----------------------------------------
class CSlotObjectData: public CLayerObject, public CDataList{
    // 各種実装
    IMPLEMENT_TEMP_ADJUST                                           // 実装：一時しのぎ調整
    IMPLEMENT_DATA( CSlotObjectData )                               // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eSOD_FLAG )                                // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eSOD_FLAG )                            // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                                           // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CSlotObjectData )              // 実装：クラスメソッドによるインスタンス確保／解放
    IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CSlotPointData )    // 実装：インスタンス確保／解放込みのリスト操作

public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                      // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CList m_oDataList;                // [X]: データリスト：CSlotPointData

public:
    // コンストラクタ／デストラクタ
    CSlotObjectData( void );
    virtual ~CSlotObjectData( void );

    //----------------
    // 取得
    //----------------

	//----------------
	// 設定
	//----------------
    
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
#endif	// __SLOT_OBJECT_DATA_HPP__