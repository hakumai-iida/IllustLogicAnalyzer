/*+----------------------------------------------------------------+
  |	Title:		Button.hpp [共通環境]
  |	Comment:	ボタン
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchNode.hpp"

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
// 窓
//----------------------------------------
class CButton: public CTouchNode{
protected:
	eGRP m_eGrpId;
	eIMG m_eGrpPalId;
	eSPR m_eSprId;
	eIMG m_eSprPalId;
	char m_cArrName[COMMON_NAME_LEN];
	bool m_bFlipX, m_bFlipY;
	uint32 m_nRGBABase, m_nRGBABaseDark;
	uint32 m_nRGBASpr, m_nRGBASprDark;
	uint32 m_nRGBAStr, m_nRGBAStrDark;
    uint32 m_nRGBAStrFocused, m_nRGBAStrDarkFocused;
	bool m_bCheckArea;
    bool m_bFocused;

public:
    // コンストラクタ／デストラクタ
	CButton( void );
	CButton( eGRP grpId );
	CButton( eGRP grpId, const char* pName );
	CButton( eGRP grpId, eSPR sprId );
	CButton( eGRP grpId, eSPR sprId, eIMG sprPalId );
	virtual ~CButton( void ){}

	// クリア
	void clear( void );

	// 設定
	void setGrp( eGRP grpId );
	void setName( const char* pName );
	inline void setGrpPal( eIMG palId ){ m_eGrpPalId = palId; }
	inline void setSpr( eSPR sprId ){ m_eSprId = sprId; }
	inline void setSprPal( eIMG sprPalId ){ m_eSprPalId = sprPalId; }
	inline void setFlipX( bool flag ){ m_bFlipX = flag; }
	inline void setFlipY( bool flag ){ m_bFlipY = flag; }
	inline void setRGBABase( uint32 rgba ){ m_nRGBABase = rgba; }
	inline void setRGBABaseDark( uint32 rgba ){ m_nRGBABaseDark = rgba; }
	inline void setRGBASpr( uint32 rgba ){ m_nRGBASpr = rgba; }
	inline void setRGBASprDark( uint32 rgba ){ m_nRGBASprDark = rgba; }
	inline void setRGBAStr( uint32 rgba ){ m_nRGBAStr = rgba; }
	inline void setRGBAStrDark( uint32 rgba ){ m_nRGBAStrDark = rgba; }
    inline void setRGBAStrFocused( uint32 rgba ){ m_nRGBAStrFocused = rgba; }
    inline void setRGBAStrDarkFocused( uint32 rgba ){ m_nRGBAStrDarkFocused = rgba; }
	inline void setCheckErea( bool flag ){ m_bCheckArea = flag; }
    inline void setFocused( bool flag ){ m_bFocused = flag; }

protected:
	void onUpdate0( void );
	void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __BUTTON_HPP__ */
