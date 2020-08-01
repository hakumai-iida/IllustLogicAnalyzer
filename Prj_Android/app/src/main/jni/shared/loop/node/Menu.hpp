/*+----------------------------------------------------------------+
  |	Title:		Menu.hpp [共通環境]
  |	Comment:	メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __MENU_HPP__
#define __MENU_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Win.hpp"

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
// メニュー
//----------------------------------------
class CMenu: public CWin{
protected:
    // 状況判定用
    int  m_nSelect;         // 選択状況（※インデックス）
	bool m_bDecided;		// 決定されたか？（※すでに選択状態の項目がタップされた）
    bool m_bBlocked;        // ブロックされたか？（※すでに選択状態の項目がタップされたが決定できなかった）
	bool m_bPressedLong;	// タップではなく長押しによる選択か？
	bool m_bSelectChanged;	// 選択が変わったか？

    // 設定値
    int  m_nItemNum;        // 要素数
    int  m_nNumInBlock;     // 一行／一列内の要素数（※２次元メニュー用）

    bool m_bSeOff;          // ＳＥを鳴らさない
    bool m_bScrollY;        // Ｙ方向へのスクロールか？

    // 入力領域
    int  m_nMaxCenterW, m_nMaxCenterH;
    
    // 入力制限用
    bool* m_bArrItemBlock;
    bool m_bItemBlockAll;

public:
    // コンストラクタ／デストラクタ（※メニューの見た目に関わる設定はコンストラクタでのみ行う想定）
	CMenu( eGRP grpId, int cW, int cH, bool isScrollY, int numInBlock );
    virtual ~CMenu( void );

	// メニュー確定
	virtual void fixMenu( void );

	// 設定
	virtual void setScrollY( bool flag ){ m_bScrollY = flag; }
	virtual void setNumInBlock( int num ){ m_nNumInBlock = num; }
    virtual void setMaxCenterWH( int w, int h ){ m_nMaxCenterW = w; m_nMaxCenterH = h; }
	virtual int setSelect( int select );
    virtual int addSelect( int add, bool isLoopable=true );
	virtual int fixSelect( bool isScrollAdjust=false );

	// 取得
	virtual inline void setItemNum( int num, bool isBlockRequired=false ){ m_nItemNum = num; }
	virtual inline void setSeOff( bool flag ){ m_bSeOff = flag; }
	virtual inline bool isDecided( void ){ return( m_bDecided ); }
    virtual inline bool isBlocked( void ){ return( m_bBlocked ); }
	virtual inline bool isPressedLong( void ){ return( m_bPressedLong ); }
	virtual inline bool isSelectChanged( void ){ return( m_bSelectChanged ); }
	virtual inline int  getItemNum( void ){ return( m_nItemNum ); }
	virtual inline int  getSelect( void ){ return( m_nSelect ); }

    // 選択不可項目関連
    virtual void setItemBlock( void );
    virtual void releaseItemBlock( void );
    virtual void setItemBlockAt( int at, bool flag );
    virtual bool checkItemBlockedAt( int at );
    inline virtual void setItemBlockAll( bool flag ){ m_bItemBlockAll = flag; }

protected:
	// 窓の可動領域の算出
	virtual float calcInternalW( void );
	virtual float calcInternalH( void );

	// １要素の縦横幅の算出
	virtual float getItemTapW( void );
	virtual float getItemTapH( void );

	// 選択確定
	virtual void fixSelectForScrollX( bool isScrollAdjust );
	virtual void fixSelectForScrollY( bool isScrollAdjust );

	// 更新
	virtual void onUpdate0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __MENU_HPP__ */
