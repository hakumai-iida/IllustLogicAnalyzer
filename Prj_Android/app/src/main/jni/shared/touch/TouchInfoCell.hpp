/*+----------------------------------------------------------------+
  |	Title:		TouchInfoCell.hpp [共通環境]
  |	Comment:	システムから受け取るタッチ情報要素
  |             （※生タッチイベントの１要素：デバイス環境⇔マネージャ間の情報）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TOUCH_INFO_CELL_HPP__
#define __TOUCH_INFO_CELL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
// タッチイベント
struct stTOUCH_EVENT_DESCRIPTOR{
#ifdef EXE_ANDROID
    // [ANDROID] [struct android_app.onInputEvent]経由で受け取る
	AInputEvent* pInputEvent;
#endif

#ifdef EXE_IOS
    // [IOS] [UIView touchesXXX]経由で受け取る
    int nEventType;
    UIEvent* pEvent;
#endif
};

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// タッチ情報
//----------------------------------------
class CTouchInfoCell{
protected:
	int  m_nAction;     // イベントの種別
	int  m_nLength;     // 各配列の有効要素数
	int* m_nArrId;
	float* m_fArrX;
	float* m_fArrY;

public:
	CTouchInfoCell( void );
	virtual ~CTouchInfoCell( void );

	// 取得
	inline int getAction( void ){ return( m_nAction ); }
	inline int getLength( void ){ return( m_nLength ); }
	inline int getIdAt( int at ){ return( m_nArrId[at] ); }
	inline float getXAt( int at ){ return( m_fArrX[at] ); }
	inline float getYAt( int at ){ return( m_fArrY[at] ); }

    // 登録（※生イベントから設定）
	void setMotionEvent( stTOUCH_EVENT_DESCRIPTOR* pTED );

    // ログの出力
    static void DumpLog( stTOUCH_EVENT_DESCRIPTOR* pTED );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TOUCH_INFO_CELL_HPP__
