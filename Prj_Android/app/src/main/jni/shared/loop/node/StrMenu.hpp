/*+----------------------------------------------------------------+
  |	Title:		StrMenu.hpp [共通環境]
  |	Comment:	文字列メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __STR_MENU_HPP__
#define __STR_MENU_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ScrollBarMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 座標／サイズ
#define SM_BASE_ITEM_H			    18
#define SM_BASE_ITEM_MARGIN_Y	     4
#define SM_BASE_ITEM_STEP_Y		    (SM_BASE_ITEM_H + SM_BASE_ITEM_MARGIN_Y)

// 状況による項目色
#define SM_ITEM_RGBA			    0xFFFFFFFF
#define SM_ITEM_RGBA_DARK           0x808080FF
#define SM_ITEM_SELECTED_RGBA	    0xFF6060FF
#define SM_ITEM_SELECTED_RGBA_DARK  0x803030FF

// ブロックされた項目であればこの値を乗算する
#define SM_ITEM_BLOCKED_RGBA_RATE   0xA0A0A0FF

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 文字列メニュー
//----------------------------------------
class CStrMenu: public CScrollBarMenu{
protected:
	// メンバー
	char** m_pArrItem;
	char* m_cArrItemBuf;

public:
    // コンストラクタ／デストラクタ（※メニューの見た目に関わる設定はコンストラクタでのみ行う想定）
	CStrMenu( eGRP grpId, int cW, int cH );
	virtual ~CStrMenu( void );

	void releaseItem( void );

    // 要素数設定＆項目設定（※設定完了あとは[fixMenu]を呼ぶこと）
	virtual void setItemNum( int num, bool isBlockRequired=false );
	void setItemAt( int id, const char* pItem, bool isBlock=false );
    void updateItemLabelAt( int id, const char* pItem );

    // 一括設定
	void setItemArrayThenFix( const char** pArrItem, const bool* pArrBlock=NULL );
    
    // 選択項目のラベル取得（※表示したい時等）
    const char* getSelectedLabel( void );

protected:
	virtual float calcInternalW( void );
	virtual float calcInternalH( void );
	virtual float getItemTapH( void );

	virtual void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __STR_MENU_HPP__ */
