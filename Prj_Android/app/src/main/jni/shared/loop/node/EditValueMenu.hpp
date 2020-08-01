/*+----------------------------------------------------------------+
  |	Title:		EditValueMenu.hpp [共通環境]
  |	Comment:	値編集メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __EDIT_VALUE_MENU_HPP__
#define __EDIT_VALUE_MENU_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ScrollBarMenu.hpp"
#include "EditValueDialog.hpp"
#include "sys/data/EditValue.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//--------------------------------------------
// メニュー項目（※表示用＆ダイアログ呼び出しパラメータ）
//--------------------------------------------
struct stEDIT_VALUE_ITEM_MENU_ITEM{
	const char*			pName;          // 表示名
	void*				pVal;           // 編集対象
	eEDIT_VALUE_TYPE	eType;          // 値のタイプ
    int32               nMin;           // 最小値（※この値まで減算できる）
	int32				nMax;           // 最大値（※この値まで加算できる）
	const char**		pArrLabel;      // ラベル表記時の参照先）
	bool				bBool;          // ブール扱いか？
	bool				bBit;           // ビット扱いか？
	int					nBit;           // 対象ビット
    
    // 表示指定
    bool                bSeparator;     // 要素の下にセパレータ（横線）を表示するか？
};

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 値編集メニュー
//----------------------------------------
class CEditValueMenu: public CScrollBarMenu{
protected:
	// メンバー
	stEDIT_VALUE_ITEM_MENU_ITEM* m_stArrEditItem;
	int m_nValueOfsX;
	bool m_bEditBoolDirect;

	// 管理フラグ
	bool m_bChanged;

public:
	CEditValueMenu( eGRP grpId, int cW, int cH );
	virtual ~CEditValueMenu( void );

	void releaseEditItem( void );

	virtual void setItemNum( int num, bool isBlockRequired=false );

    //--------------------------------------
    // アイテム設定
    //--------------------------------------
	bool setItemAt( int id, stEDIT_VALUE_ITEM_MENU_ITEM* pItem, bool isBlock=false );
	bool setItemAt( int id, const char* pName, void* pVal, eEDIT_VALUE_TYPE type, int32 min, int32 max, bool isBlock=false );
	bool setItemAtAsLabel( int id, const char* pName, void* pVal,
                           eEDIT_VALUE_TYPE type, int32 min, int32 max, const char** pArrLabel, bool isBlock=false );
	bool setItemAtAsBool( int id, const char* pName, void* pVal, eEDIT_VALUE_TYPE type=eEDIT_VALUE_TYPE_INT8, bool isBlock=false );
	bool setItemAtAsBit( int id, const char* pName, void* pVal, eEDIT_VALUE_TYPE type, int bit, bool isBlock=false );
    
    void setSeparatorAt( int id, bool flag );

	// 取得
	stEDIT_VALUE_ITEM_MENU_ITEM* getItemAt( int at );
	stEDIT_VALUE_ITEM_MENU_ITEM* getSelectedItem( void );
    inline bool isChanged( void ){ return( m_bChanged ); }

    // ブールの編集を直接行うか？（※デフォルトはオン）
    inline void setEditBoolDirect( bool flag ){ m_bEditBoolDirect = flag; }

    // ダイアログ設定
	void setEditValueDialog( CEditValueDialog* pDialog );

protected:
	virtual float calcInternalW( void );
	virtual float calcInternalH( void );
	virtual float getItemTapH( void );

	virtual void onUpdate0( void );
	virtual void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __EDIT_VALUE_MENU_HPP__ */
