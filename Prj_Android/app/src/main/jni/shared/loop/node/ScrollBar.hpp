/*+----------------------------------------------------------------+
  |	Title:		ScrollBar.hpp [共通環境]
  |	Comment:	スクロールバー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SCROLL_BAR_HPP__
#define __SCROLL_BAR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Node.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// スクロールバー
//----------------------------------------
class CScrollBar: public CNode{
protected:
	// 表示情報
	bool m_bVertical;			// 縦向き表示か？
	int  m_nWidth;				// 横幅（スクロール領域サイズ）
	int  m_nHeight;				// 縦幅（スクロール領域サイズ）
	int  m_nFrameSize;			// 枠のサイズ

	// 論理サイズ
	float m_fScrollAreaSize;	// スクロール領域のサイズ（※全スクロール領域）
	float m_fScrollBarSize;		// スクロールバーのサイズ（※一度に表示できる領域）
	float m_fScrollOfs;			// スクロールオフセット（※どれだけスクロールしているか？）

	// 色
	DWORD m_nBaseRGBA;
	DWORD m_nFrameRGBA;
	DWORD m_nBarRGBA;
	BYTE m_nAlpha;

public:
    // コンストラクタ／デストラクタ
	CScrollBar( void );
	virtual ~CScrollBar( void ){}

	void reset( void );
	void setForDisp( bool isV, int w, int h, int b );
	void setForScroll( float area, float bar );
	void setScrollOfs( float ofs );
	void setAlpha( BYTE alpha ){ m_nAlpha = alpha; }
	void setBaseRGBA( DWORD rgba ){ m_nBaseRGBA = rgba; }
	void setFrameRGBA( DWORD rgba ){ m_nFrameRGBA = rgba; }
	void setBarRGBA( DWORD rgba ){ m_nBarRGBA = rgba; }

	inline bool isFill( void ){ return( m_fScrollBarSize >= m_fScrollAreaSize ); }

	inline virtual float getW( void ){ return( m_nWidth + 2*m_nFrameSize ); }
	inline virtual float getH( void ){ return( m_nHeight + 2*m_nFrameSize );}

protected:
	virtual void onUpdate0( void );
	virtual void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __SCROLL_BAR_HPP__ */
