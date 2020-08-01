/*+----------------------------------------------------------------+
  |	Title:		TouchExObj.hpp [共通環境]
  |	Comment:	タッチ可能ゲーム要素規定：拡張（※ドラッグ／長押し等に対応）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TOUCH_EX_OBJ_HPP__
#define __TOUCH_EX_OBJ_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchObj.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジック：タッチ範囲
//----------------------------------------
class CTouchExObj: public CTouchObj{
protected:
    float m_fTouchExPressedFrameMax;    // 長押し最大フレーム
    float m_fTouchExPressedFrame;       // 長押しフレーム
    
    // 入力検出対象
    int m_nTouchExCellId;

    // 検出履歴（※毎フレーム設定される）
    bool m_bTouchExPressed;
    bool m_bTouchExPressedDown;
    bool m_bTouchExLongPressed;
    bool m_bTouchExTapped;
    bool m_bTouchExFlicked;
    
    float m_fTouchExPressedX, m_fTouchExPressedY;
    float m_fTouchExPressedDownX, m_fTouchExPressedDownY;
    float m_fTouchExTappedX, m_fTouchExTappedY;
    float m_fTouchExDragDistX, m_fTouchExDragDistY;
    float m_fTouchExFlickDistX, m_fTouchExFlickDistY;

public:
	// コンストラクタ＆デストラクタ
	CTouchExObj( void );
    virtual ~CTouchExObj( void ){}
    
    // クリア（※初期化用）
    void clearTouchExObj( void );
    
    // リセット（更新用）
    void resetTouchExObjForUpdate( void );
    
    // 長押し関連
    inline void setTouchExLongPressedMax( float max ){ m_fTouchExPressedFrameMax = max; }
    inline float getTouchExLongPressedFrame( void ){ return( m_fTouchExPressedFrame ); }
    inline float getTouchExLongPressedFrameMax( void ){ return( m_fTouchExPressedFrameMax ); }
    float getTouchExLongPressedFrameRate( void );
    
    // 入力確認
    inline bool isTouchExPressed( void ){ return( m_bTouchExPressed ); }
    inline bool isTouchExPressedDown( void ){ return( m_bTouchExPressedDown ); }
    inline bool isTouchExLongPressed( void ){ return( m_bTouchExLongPressed ); }
    inline bool isTouchExTapped( void ){ return( m_bTouchExTapped ); };
    inline bool isTouchExFlicked( void ){ return( m_bTouchExFlicked ); }

    inline float getTouchExPressedX( void ){ return( m_fTouchExPressedX ); }
    inline float getTouchExPressedY( void ){ return( m_fTouchExPressedY ); }
    inline float getTouchExTappedX( void ){ return( m_fTouchExTappedX ); }
    inline float getTouchExTappedY( void ){ return( m_fTouchExTappedY ); }
    inline float getTouchExDragDistX( void ){ return( m_fTouchExDragDistX ); }
    inline float getTouchExDragDistY( void ){ return( m_fTouchExDragDistY ); }
    inline float getTouchExFlickDistX( void ){ return( m_fTouchExFlickDistX ); }
    inline float getTouchExFlickDistY( void ){ return( m_fTouchExFlickDistY ); }

    // 更新：[CTouchObj]を上書き
    virtual void update( void );

protected:
    bool preUpdateForTouchExObj( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TOUCH_EX_OBJ_HPP__
