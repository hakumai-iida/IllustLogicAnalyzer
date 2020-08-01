/*+----------------------------------------------------------------+
  |	Title:		Node.hpp [共通環境]
  |	Comment:	ノード基底（※ループにより更新＆描画管理されるもの）
  |             開発／デバッグ用の表示物（※ゲーム表示要素の[CObj]とは別実装）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __NODE_HPP__
#define __NODE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sys/data/ListNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define RGBA_NODE_ACTIVE		0xFFFFFFFF
#define	RGBA_NODE_DARK			0x808080FF
#define RGBA_NODE_LOCKED		0xA0A0A0FF
#define RGBA_NODE_LOCKED_DARK	0x505050FF

#define GET_NODE_RGBA( _bDark, _bLocked )				((_bDark)? ((_bLocked)? RGBA_NODE_LOCKED_DARK: RGBA_NODE_DARK): ((_bLocked)? RGBA_NODE_LOCKED: RGBA_NODE_ACTIVE))

#define GET_NODE_RGB_WITH_A( _bDark, _bLocked, _a  )	((((_bDark)? ((_bLocked)? RGBA_NODE_LOCKED_DARK: RGBA_NODE_DARK): ((_bLocked)? RGBA_NODE_LOCKED: RGBA_NODE_ACTIVE))&0xFFFFFF00) | (_a))

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ノード基底
//----------------------------------------
class CNode: public CListNode{
    // 実装
    IMPLEMENT_NODE( CNode )

protected:
	float m_fX, m_fY;	// 配置座標
	bool m_bVisible;	// 可視性
	bool m_bActive;		// 活動性
	bool m_bDark;		// 暗転表示（※活動がオフになる）

public:
	CNode( void ){}
	virtual ~CNode( void ){}

	virtual void put( float x, float y );

	inline virtual float getX( void ){ return( m_fX ); }
	inline virtual float getY( void ){ return( m_fY ); }
	inline virtual float getW( void ){ return( 0 ); }
	inline virtual float getH( void ){ return( 0 ); }

	virtual void setVisible( bool flag );
	virtual bool isVisible( void );

	virtual void setActive( bool flag );
	virtual bool isActive( void );

	virtual void setDark( bool flag );
	virtual bool isDark( void );

	virtual void enter( void );
	virtual void exit( void );

    // ループから呼んでもらう
	virtual void onUpdate( void );
	virtual void onDraw( void );

protected:
	// 処理実体：派生先で実装すべきメソッド
	virtual void onUpdate0( void ) = 0;
	virtual void onDraw0( void ) = 0;
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __NODE_HPP__ */
