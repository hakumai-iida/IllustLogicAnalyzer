/*+----------------------------------------------------------------+
  |	Title:		EditValueDialog.hpp [共通環境]
  |	Comment:	値編集ダイアログ（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __EDIT_VALUE_DIALOG_HPP__
#define __EDIT_VALUE_DIALOG_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Node.hpp"
#include "Button.hpp"

#include "sys/data/EditValue.hpp"

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
// 値編集ダイアログ
//----------------------------------------
class CEditValueDialog: public CNode{
protected:
	CButton* m_pButtonZero;
    CButton* m_pButtonNega;
	CButton* m_pButtonUndo;
	CButton* m_pButtonRedo;
	CButton* m_pButtonMin;
	CButton* m_pButtonMax;
	CButton* m_pButtonCopy;
	CButton* m_pButtonPaste;
	CButton* m_oArrButtonInc;
	CButton* m_oArrButtonDec;

	// 対象
	void* m_pValue;

	// 各種値
	eEDIT_VALUE_TYPE m_eValueType;
	int32 m_nValueMax;
	int32 m_nValueMin;
	int32 m_nValueUndo;
	int32 m_nValueRedo;
	int32 m_nValuePaste;

	// 名前
	char m_cArrName[COMMON_NAME_LEN];

	// ラベル
	const char** m_pArrLabel;

	// 桁数
	int m_nDegree;
	int m_nBit;

	// 管理フラグ
	bool m_bBit;
	bool m_bBool;
	bool m_bChanged;
    bool m_bLabelRight;
    bool m_bLabelBottom;

public:
	CEditValueDialog( void );
	virtual ~CEditValueDialog( void );

	void reset( void );
	void set( void* pVal, eEDIT_VALUE_TYPE type, int32 min, int32 max );
	void setAsBit( void* pVal, eEDIT_VALUE_TYPE type, int bit );
	void setAsBool( void *pVal, eEDIT_VALUE_TYPE type );
	void setName( const char* pName );
	void setLabel( const char** pArrLabel );
    
    inline void setLabelRight( bool flag ){ m_bLabelRight = flag; }
    inline void setLabelBottom( bool flag ){ m_bLabelBottom = flag; }

	void fixDialog( void );
    
    virtual void setDark( bool isDark );

	virtual void put( float x, float y );
	virtual void enter( void );
	virtual void exit( void );
	virtual void setActive( bool flag );

	inline bool isChanged( void ){ return( m_bChanged ); }

	virtual float getW( void );
	virtual float getH( void );

protected:
	virtual void onUpdate0( void );
	virtual void onDraw0( void );

	// 値の取得／設定増減
	int32 getValue( void );
	bool setValue( int32 val );
	bool addValue( int32 add );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __EDIT_VALUE_DIALOG_HPP__ */
