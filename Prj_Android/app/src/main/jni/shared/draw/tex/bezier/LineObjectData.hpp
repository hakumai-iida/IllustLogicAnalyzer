/*+----------------------------------------------------------------+
  |	Title:		LineObjectData.hpp [共通環境]
  |	Comment:	線データ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LINE_OBJECT_DATA_HPP__
#define __LINE_OBJECT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerObject.hpp"
#include "AnchorPointData.hpp"

#include "sys/data/DataCell.hpp"
#include "sys/data/DataList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eLOD_FLAG{
	eLOD_FLAG_INVALID = -1,

    eLOD_FLAG_DISABLE,	                    // 00:[*] 無効（※[CLayerData::draw]で処理されない）
    eLOD_FLAG_CLOSE_PATH,                   // 01:[C] パスを閉じる（※終点から始点へストロークを引く）
    eLOD_FLAG_DOT,                          // 02:[D] パスではなくドットを描画する（※１ＡＰにつき１ドットのみ出力）
    eLOD_FLAG_TOUCH,                        // 03:[T] タッチを描画する
    eLOD_FLAG_TOUCH_IS_STRIPE,              // 04:[S] タッチ描画をストライプ扱いする
    eLOD_FLAG_TOUCH_IS_FRILL,               // 05:[F] タッチ描画をフリル扱いする
    eLOD_FLAG_GUIDE_DRAW_BEFORE_STROKE,     // 06:[dG] ストローク処理の前に登録タッチ情報でガイドを描画する
    eLOD_FLAG_GUIDE_RESET_AFTER_STROKE,     // 07:[rG] ストローク処理の後にガイドをリセットする
    eLOD_FLAG_GUIDE_RESET_FULL,             // 08:[grF] ガイドを完全にリセットする

    eLOD_FLAG_MAX,		                    // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eLOD_FLAG_TEMP_SELECTED = 16,           // 編集対象として選択されている（※表示切り替え用）
    eLOD_FLAG_TEMP_ACTIVE,                  // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 線データ
//----------------------------------------
class CLineObjectData: public CLayerObject, public CDataList{
    // 各種実装
    IMPLEMENT_TEMP_ADJUST                                           // 実装：一時しのぎ調整
    IMPLEMENT_DATA( CLineObjectData )                               // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eLOD_FLAG )                                // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eLOD_FLAG )                            // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                                           // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CLineObjectData )              // 実装：クラスメソッドによるインスタンス確保／解放
    IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CAnchorPointData )  // 実装：インスタンス確保／解放込みのリスト操作

public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_eFlag;                      // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CList m_oDataList;                // [X]: データリスト：CAnchorPointData
    
    eBRUSH m_eBrushId;                  // [enum]: ブラシID
    ePAL_OFS m_eTestPalOfsId;           // [enum]: テストするパレットオフセットID（※指定色の上にのみ線を引く）
    ePAL_OFS m_eTestPalOfsIdForRepair;  // [enum]: ラインの接続時の修復テストをするパレットオフセットID（※指定色の上に再度線を引く指定）

public:
    // コンストラクタ／デストラクタ
    CLineObjectData( void );
    virtual ~CLineObjectData( void );

    //----------------
    // 設定
    //----------------
    inline void setBrushId( eBRUSH brushId ){ m_eBrushId = brushId; }
    inline void setTestPalOfsId( ePAL_OFS palOfsId ){ m_eTestPalOfsId = palOfsId; }
    inline void setTestPalOfsIdForRepair( ePAL_OFS repairPalOfsId ){ m_eTestPalOfsIdForRepair = repairPalOfsId; }

	//----------------
	// 取得
	//----------------
    inline eBRUSH getBrushId( void ){ return( m_eBrushId ); }
    inline ePAL_OFS getTestPalOfsId( void ){ return( m_eTestPalOfsId ); }
    inline ePAL_OFS getTestPalOfsIdForRepair( void ){ return( m_eTestPalOfsIdForRepair ); }
    inline BYTE getTestPalGrp( void ){ return( CDefTable::GetPalOfsValue( m_eTestPalOfsId ) ); }
    inline BYTE getTestPalGrpForRepair( void ){ return( CDefTable::GetPalOfsValue( m_eTestPalOfsIdForRepair ) ); }

    //-----------------------------------------------
    // アンカーポイントの追加（※現在選択中の点と次の点の中間）
    //-----------------------------------------------
    CAnchorPointData* addDataAtIntermediate( void );

    //----------------
    // 鏡面コピー
    //----------------
    inline bool mirrorCopyH( void ){ return( mirrorCopy0( false ) ); }
    inline bool mirrorCopyV( void ){ return( mirrorCopy0( true ) ); }

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );
    
protected:
    bool mirrorCopy0( bool isVertical );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LINE_OBJECT_DATA_HPP__
