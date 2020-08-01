/*+----------------------------------------------------------------+
  |	Title:		FillPointData.hpp [共通環境]
  |	Comment:	塗りポイントデータ（※[CPaintObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FILL_POINT_DATA_HPP__
#define __FILL_POINT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/tex/adjust/AdjustablePoint.hpp"
#include "draw/tex/stroke/Stroke.hpp"

#include "sys/data/ListNode.hpp"
#include "sys/data/DataCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eFPD_FLAG{
	eFPD_FLAG_INVALID = -1,         // 無効値

    eFPD_FLAG_DISABLE,	            // 00:[*] 無効（※[CPaintObjectData::CreateBezierFillPoint]で登録されない）
    eFPD_FLAG_TRANSPARENCY,         // 01:[(T)] 透明（※表示はしないが塗り処理を行う＝塗りガイドを埋めてマスク的な利用をする際）
    
    eFPD_FLAG_MAX,		            // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eFPD_FLAG_TEMP_SELECTED = 16,   // 編集対象として選択されている（※表示切り替え用）
    eFPD_FLAG_TEMP_ACTIVE,          // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 塗りポイントデータ
//----------------------------------------
class CFillPointData: public CListNode{
    // 各種実装
    IMPLEMENT_ADJUSTABLE_XY                             // 実装：調整XY座標
    IMPLEMENT_NODE( CFillPointData )                    // 実装：リストノード
    IMPLEMENT_DATA( CFillPointData )                    // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eFPD_FLAG )                    // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eFPD_FLAG )                // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                               // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CFillPointData )   // 実装：クラスメソッドによるインスタンス確保／解放

public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                                      // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CAdjustablePoint m_oAdjXY;                        // [X]: 座標（※画像サイズに対する割合座標）

	ePAL_OFS m_ePalOfsId;                               // [enum]: パレットオフセットID
    
    eFILL_OPTION m_eFillOption;                         // [enum]: 塗りオプション
    eSTROKE_TOUCH_TARGET m_eTouchTargetIdForOption;     // [enum]: オプションの対象タッチID
    int m_nStrokeSizeForOption;                         // [2]: オプションに対するストロークサイズ（※ゲーム解像度サイズ）
    int m_nStrokeRangeForOption;                        // [2]: オプションに対するストロークレンジ（※ゲーム解像度サイズ）
    int m_nOfsXForOption;                               // [2]: オプションに対するＸオフセット（※ゲーム解像度サイズ）
    int m_nOfsYForOption;                               // [2]: オプションに対するＹオフセット（※ゲーム解像度サイズ）

public:
    // コンストラクタ／デストラクタ
    CFillPointData( void );
    virtual ~CFillPointData( void ){}
    
    //------------
    // 設定
    //------------
    void set( int rX, int rY, ePAL_OFS palOfsId=ePAL_OFS_INVALID );
    void setOption( eFILL_OPTION option, eSTROKE_TOUCH_TARGET touchId=eSTROKE_TOUCH_TARGET_INVALID );
    void setOptionStroke( int size, int range );
    void setOptionOfsXY( int ofsX, int ofsY );

	//------------
	// 取得
	//------------
	inline ePAL_OFS getPalOfsId( void ){ return( m_ePalOfsId ); }
    inline eFILL_OPTION getFillOption( void ){ return( m_eFillOption ); }
    eSTROKE_TOUCH_TARGET getTouchTargetIdForOption( int slotIndex=-1 );
    inline int getStrokeSizeForOption( void ){ return( m_nStrokeSizeForOption ); }
    inline int getStrokeRangeForOption( void ){ return( m_nStrokeRangeForOption ); }
    inline int getOfsXForOption( void ){ return( m_nOfsXForOption ); }
    inline int getOfsYForOption( void ){ return( m_nOfsYForOption ); }

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
#endif	// __FILL_POINT_DATA_HPP__
