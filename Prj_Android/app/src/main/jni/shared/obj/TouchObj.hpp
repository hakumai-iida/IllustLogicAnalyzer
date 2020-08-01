/*+----------------------------------------------------------------+
  |	Title:		TouchObj.hpp [共通環境]
  |	Comment:	タッチ可能なゲーム要素基底（※単発の入力判定のみ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TOUCH_OBJ_HPP__
#define __TOUCH_OBJ_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Obj.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// ゲーム要素
//-----------------------------------------------------------------
class CTouchObj: public CObj{
	//-------------------------
	// クラス
	//-------------------------
protected:
    // 確認用の表示用（※タッチ範囲を表示するか？）
    static bool s_bTouchAreaCheck;

public:
	inline static void SetTouchAreaCheck( bool flag ){ s_bTouchAreaCheck = flag; }
	inline static void SwapTouchAreaCheck( void ){ s_bTouchAreaCheck = !s_bTouchAreaCheck; }

	//-------------------------
	// メンバー
	//-------------------------
protected:
    eTOUCH_CHECK m_nTouchCheckType;     // タッチ判定のタイプ
    bool  m_bTouchOval;                 // タッチ判定を円で行うか？
    bool  m_bTouchActive;               // タッチ判定が有効か？
    bool  m_bTouchWithBackButton;       // タッチ判定の際バックボタンを検出するか？（※[Android]向け）
    bool  m_bTouchCheckDrawBack;        // タッチ判定確認を奥に表示するか？

    float m_fTouchAreaW, m_fTouchAreaH; // タッチ領域サイズ（※[Oval]時はこの矩形に収まる円で判定する）
    float m_fTouchOfsX, m_fTouchOfsY;   // タッチ判定オフセット（※[getX(),getY()に対する調整用）
    float m_fTouchedX, m_fTouchedY;		// タッチ位置（※[getX(),getY()]からの相対位置：[m_fTouchOfsX,m_fTouchOfsY]は反映されない）
	float m_fTouchBlockFrame;			// タッチブロックタイマー（※連打防止用）

    DWORD m_nTouchAreaRGBA;             // チェック用の色

public:
	// コンストラクタ＆デストラクタ
	CTouchObj( void );
	virtual ~CTouchObj( void ){}

	// クリア（※初期化用）
	void clearTouchObj( void );

	// 設定
	void setTouchRect( int ofsX, int ofsY, int w, int h );
    void setTouchOval( int ofsX, int ofsY, int w, int h );

    inline void setTouchCheckType( eTOUCH_CHECK type ){ m_nTouchCheckType = type; }
	inline void setTouchActive( bool flag ){ m_bTouchActive = flag; }
	inline void setTouchWithBackButton( bool flag ){ m_bTouchWithBackButton = flag; }
	inline void setTouchCheckDrawBack( bool flag ){ m_bTouchCheckDrawBack = flag; }
	inline void setTouchBlockFrame( float frame ){ m_fTouchBlockFrame = frame; }
    inline void setTouchAreaRGBA( DWORD rgba ){ m_nTouchAreaRGBA = rgba; }
    
    // 判定
    inline bool isTouchAreaValid( void ){ return( m_fTouchAreaW > 0.0f && m_fTouchAreaH > 0.0f ); }
	inline bool isTouchActive( void ){ return( m_bTouchActive && m_fTouchBlockFrame <= 0 ); }

	// タッチされたか？
	virtual bool isTouched( void );
	inline float getTouchedX( void ){ return( m_fTouchedX ); }
	inline float getTouchedY( void ){ return( m_fTouchedY ); }

	// 更新：[CObj]を上書き
	virtual void update( void );

protected:
    // [CTouchObj]に関する更新（※事前のみ）
    bool preUpdateForTouchObj( void );

    // 描画：[CObj]を上書き
	virtual void onDraw( void );
    
    // [CTouchObj]に関する描画（※事前／事後）
    bool preDrawForTouchObj( void );
    void postDrawForTouchObj( void );
    
    // タッチエリア確認表示
    void drawTouchArea( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TOUCH_OBJ_HPP__
