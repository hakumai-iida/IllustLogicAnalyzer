/*+----------------------------------------------------------------+
  |	Title:		EditValueDialog.hpp [共通環境]
  |	Comment:	値編集ダイアログ（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "EditValueDialog.hpp"

// RGB値の参照
#include "StrMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 基本サイズ
#define EV_DIALOG_W				397
#define EV_DIALOG_H				100
#define EV_DIALOG_DEGREE_MAX	  8

// 色
#define EV_DIALOG_RGBA			    0xFFFFFF80
#define EV_DIALOG_RGBA_DARK         0x80808080

#define EV_DIALOG_BORDER_RGBA	    0x000080FF
#define EV_DIALOG_BORDER_RGBA_DARK  0x000040FF

#define EV_DIALOG_BASE_RGBA		    0x00000040

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------
// コンストラクタ
//------------------
CEditValueDialog::CEditValueDialog( void ){
	// 各種ボタン
	m_pButtonZero = new CButton( eGRP_DEBUG_BUTTON_M, "ZERO" );
    m_pButtonNega = new CButton( eGRP_DEBUG_BUTTON_M, "NEGA" );
	m_pButtonUndo = new CButton( eGRP_DEBUG_BUTTON, "UNDO" );
	m_pButtonRedo = new CButton( eGRP_DEBUG_BUTTON, "REDO" );
	m_pButtonMin = new CButton( eGRP_DEBUG_BUTTON_L, "MIN" );
	m_pButtonMax = new CButton( eGRP_DEBUG_BUTTON_L, "MAX" );
	m_pButtonMax->setFlipX( true );
	m_pButtonCopy = new CButton( eGRP_DEBUG_BUTTON, "COPY" );
	m_pButtonPaste = new CButton( eGRP_DEBUG_BUTTON, "PASTE" );

    m_bLabelRight = false;
    m_bLabelBottom = false;

	reset();
}

//------------------
// デストラクタ
//------------------
CEditValueDialog::~CEditValueDialog( void ){
	SAFE_DELETE( m_pButtonZero );
    SAFE_DELETE( m_pButtonNega );
	SAFE_DELETE( m_pButtonUndo );
	SAFE_DELETE( m_pButtonRedo );
	SAFE_DELETE( m_pButtonMin );
	SAFE_DELETE( m_pButtonMax );
	SAFE_DELETE( m_pButtonCopy );
	SAFE_DELETE( m_pButtonPaste );
	SAFE_DELETE_ARRAY( m_oArrButtonInc );
	SAFE_DELETE_ARRAY( m_oArrButtonDec );
}

//------------------
// リセット
//------------------
void CEditValueDialog::reset( void ){
	SAFE_DELETE_ARRAY( m_oArrButtonInc );
	SAFE_DELETE_ARRAY( m_oArrButtonDec );

	m_eValueType = (eEDIT_VALUE_TYPE)0;
	m_pValue = NULL;
	m_nValueMax = 0;
	m_nValueMin = 0;
	m_nValueUndo = 0;
	m_nValueRedo = 0;

	// ペースト値は持ち越す
	//m_nValuePaste = 0;

	m_cArrName[0] = '\0';
	m_pArrLabel = NULL;
	m_nDegree = 0;
	m_nBit = 0;
	m_bBit = false;
	m_bBool = false;
	m_bChanged = false;
}

//------------------
// 設定
//------------------
void CEditValueDialog::set( void* pVal, eEDIT_VALUE_TYPE type, int32 min, int32 max ){
	// リセット
	reset();

	// 設定
	m_pValue = pVal;
	m_eValueType = type;
	m_nValueMin = min;
	m_nValueMax = max;
	m_nValueUndo = m_nValueRedo = getValue();

	// ペースト値は持ち越す
	//m_nValuePaste = 0;

	// 桁数確認
	m_nDegree = 1;
	int32 temp = max;
	if( temp < -min ){ temp = -min; }
	while( temp >= 10 ){
		m_nDegree++;
		temp /= 10;
	}
	if( m_nDegree > EV_DIALOG_DEGREE_MAX ){
		m_nDegree = EV_DIALOG_DEGREE_MAX;
	}
    
	// 増減ボタン
	m_oArrButtonInc = new CButton[m_nDegree];
	m_oArrButtonDec = new CButton[m_nDegree];
	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonDec[i].setGrp( eGRP_DEBUG_BUTTON_M );
		m_oArrButtonInc[i].setGrp( eGRP_DEBUG_BUTTON_P );
	}
}

//--------------------
// ブールとして設定
//--------------------
void CEditValueDialog::setAsBool( void *pVal, eEDIT_VALUE_TYPE type ){
	set( pVal, type, 0, 1 );
	m_bBool = true;
}

//--------------------
// ビットとして設定
//--------------------
void CEditValueDialog::setAsBit( void *pVal, eEDIT_VALUE_TYPE type, int bit ){
	setAsBool( pVal, type );
	m_nBit = bit;
	m_bBit = true;
}

//--------------------
// 名前設定
//--------------------
void CEditValueDialog::setName( const char* pName ){
	// 文字列のコピー（※バッファに収まらない部分は切り捨て）
	util_str_cpy( m_cArrName, pName, COMMON_NAME_LEN );
}

//--------------------
// ラベル設定
//--------------------
void CEditValueDialog::setLabel( const char** pArrLabel ){
	m_pArrLabel = pArrLabel;
}

//--------------------
// ダイアログ確定
//--------------------
void CEditValueDialog::fixDialog( void ){
	put( m_fX, m_fY );
}

//--------------------
// ダーク設定
//--------------------
void CEditValueDialog::setDark( bool isDark ){
    CNode::setDark( isDark );
    
    m_pButtonZero->setDark( isDark );
    m_pButtonNega->setDark( isDark );
    m_pButtonUndo->setDark( isDark );
    m_pButtonRedo->setDark( isDark );
    m_pButtonMin->setDark( isDark );
    m_pButtonMax->setDark( isDark );
    m_pButtonCopy->setDark( isDark );
    m_pButtonPaste->setDark( isDark );
    
    for( int i=0; i<m_nDegree; i++ ){
        m_oArrButtonInc[i].setDark( isDark );
        m_oArrButtonDec[i].setDark( isDark );
    }
}

//------------------
// 位置指定
//------------------
void CEditValueDialog::put( float x, float y ){
	CNode::put( x, y );

    m_pButtonZero->put( x + 1 + 0*(32+2), y + 1 + 0*(32+1) );
    m_pButtonNega->put( x + 1 + 1*(32+2-1), y + 1 + 0*(32+1) );
	m_pButtonUndo->put( x + 1+1, y + 1 + 1*(32+1) );
    m_pButtonRedo->put( x + 1+1, y + 1 + 2*(32+1) );

	m_pButtonMin->put( x + EV_DIALOG_W - (64+3-1), y + 1 + 0*(32+1) );
	m_pButtonMax->put( x + EV_DIALOG_W - (64+3) + 1*(32+2), y + 1 + 0*(32+1) );
	m_pButtonCopy->put( x + EV_DIALOG_W - (64+3) + 1, y + 1 + 1*(32+1) );
	m_pButtonPaste->put( x + EV_DIALOG_W - (64+3) + 1, y + 1 + 2*(32+1) );

	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonDec[i].put( x + EV_DIALOG_W - (64+3-1) - (i+1)*(32+1), y + 1 );
		m_oArrButtonInc[i].put( x + EV_DIALOG_W - (64+3-1) - (i+1)*(32+1), y + 1 + 2*(32+1) );
	}
}

//------------------
// 入場
//------------------
void CEditValueDialog::enter( void ){
	CNode::enter();

	m_bChanged = false;

	m_pButtonZero->enter();
    m_pButtonNega->enter();
	m_pButtonUndo->enter();
	m_pButtonRedo->enter();
	m_pButtonMin->enter();
	m_pButtonMax->enter();
	m_pButtonCopy->enter();
	m_pButtonPaste->enter();

	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonInc[i].enter();
		m_oArrButtonDec[i].enter();
	}
}

//------------------
// 退場
//------------------
void CEditValueDialog::exit( void ){
	CNode::exit();

	m_pButtonZero->exit();
    m_pButtonNega->exit();
	m_pButtonUndo->exit();
	m_pButtonRedo->exit();
	m_pButtonMin->exit();
	m_pButtonMax->exit();
	m_pButtonCopy->exit();
	m_pButtonPaste->exit();

	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonInc[i].exit();
		m_oArrButtonDec[i].exit();
	}
}

//------------------
// 活動指定
//------------------
void CEditValueDialog::setActive( bool flag ){
	CNode::setActive( flag );

	m_pButtonZero->setActive( flag );
    m_pButtonNega->setActive( flag );
	m_pButtonUndo->setActive( flag );
	m_pButtonRedo->setActive( flag );
	m_pButtonMin->setActive( flag );
	m_pButtonMax->setActive( flag );
	m_pButtonCopy->setActive( flag );
	m_pButtonPaste->setActive( flag );

	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonInc[i].setActive( flag );
		m_oArrButtonDec[i].setActive( flag );
	}
}

//------------------
// サイズ取得
//------------------
float CEditValueDialog::getW( void ){ return( EV_DIALOG_W ); }
float CEditValueDialog::getH( void ){ return( EV_DIALOG_H ); }

//------------------
// 更新
//------------------
void CEditValueDialog::onUpdate0( void ){
	m_bChanged = false;

	// UNDO
	m_pButtonUndo->onUpdate();
	if( m_pButtonUndo->isTapped() ){
		m_bChanged = setValue( m_nValueUndo );
		return;
	}

	// REDO
	m_pButtonRedo->onUpdate();
	if( m_pButtonRedo->isTapped() ){
		m_bChanged = setValue( m_nValueRedo );
		return;
	}

	// Min
	m_pButtonMin->onUpdate();
	if( m_pButtonMin->isTapped() ){
		m_bChanged = setValue( m_nValueMin );
        m_nValueRedo = getValue();
		return;
	}

	// MAX
	m_pButtonMax->onUpdate();
	if( m_pButtonMax->isTapped() ){
		m_bChanged = setValue( m_nValueMax );
        m_nValueRedo = getValue();
		return;
	}
    
    // ZERO
    m_pButtonZero->onUpdate();
    if( m_pButtonZero->isTapped() ){
        m_bChanged = setValue( 0 );
        m_nValueRedo = getValue();
        return;
    }
    
    // NEGA
    m_pButtonNega->onUpdate();
    if( m_pButtonNega->isTapped() ){
        m_bChanged = setValue( -getValue() );
        m_nValueRedo = getValue();
        return;
    }

	// Copy
	m_pButtonCopy->onUpdate();
	if( m_pButtonCopy->isTapped() ){
		m_nValuePaste = getValue();
		return;
	}

	// Paste
	m_pButtonPaste->onUpdate();
	if( m_pButtonPaste->isTapped() ){
		// 用心にリミット
		int temp = m_nValuePaste;
		if( temp < m_nValueMin ){ temp = m_nValueMin; }
		else if( temp > m_nValueMax ){ temp = m_nValueMax; }
		m_bChanged = setValue( temp );
        m_nValueRedo = getValue();
		return;
	}

	// Inc
	int add = 1;
	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonInc[i].onUpdate();
		if( m_oArrButtonInc[i].isTapped() ){
			m_bChanged = addValue( add );
			m_nValueRedo = getValue();
			return;
		}
		add *= 10;
	}

	// Dec
	int sub = -1;
	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonDec[i].onUpdate();
		if( m_oArrButtonDec[i].isTapped() ){
			m_bChanged = addValue( sub );
			m_nValueRedo = getValue();
			return;
		}
		sub *= 10;
	}
}

//------------------
// 描画
//------------------
void CEditValueDialog::onDraw0( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();

    DWORD rgba;
    
    if( isDark() ){ rgba = EV_DIALOG_RGBA_DARK; }
    else{ rgba = EV_DIALOG_RGBA; }

	// 下地の表示
    pDC->clear();
	pDC->setRect( EV_DIALOG_W, EV_DIALOG_H );
	pDC->setRGBA( rgba );
	pDC->setBlendAlpha();
	pDC->draw( m_fX, m_fY );

    if( isDark() ){ rgba = EV_DIALOG_BORDER_RGBA_DARK; }
    else{ rgba = EV_DIALOG_BORDER_RGBA; }

	// 枠線の表示
	pDC->clear();
	pDC->setRect( EV_DIALOG_W+4, 2 );
	pDC->setRGBA( rgba );
	pDC->draw( m_fX-2, m_fY-2 );
	pDC->draw( m_fX-2, m_fY+EV_DIALOG_H );

	pDC->clear();
	pDC->setRect( 2, EV_DIALOG_H );
	pDC->setRGBA( rgba );
	pDC->draw( m_fX-2, m_fY );
	pDC->draw( m_fX+EV_DIALOG_W, m_fY );
    
    // 無効は無視
    if( m_pValue == NULL ){
        return;
    }

    if( isDark() ){ rgba = SM_ITEM_RGBA_DARK; }
    else{ rgba = SM_ITEM_RGBA; }

	// 名前表示
	pDC->clear();
	pDC->setStr( eFONT_DEBUG, m_cArrName );
	pDC->setScaleXY( 1.0f, 2.0f );
    pDC->setRGBA( rgba );
    if( m_bLabelRight && m_bLabelBottom ){
        pDC->draw( m_fX + EV_DIALOG_W - pDC->getWidth(), m_fY + EV_DIALOG_H );
    }else if( m_bLabelRight ){
        pDC->draw( m_fX + EV_DIALOG_W - pDC->getWidth(), m_fY - 23 );
    }else if( m_bLabelBottom ){
        pDC->draw( m_fX,  m_fY + EV_DIALOG_H );
    }else{
        pDC->draw( m_fX, m_fY - 23 );
    }

	// ボタン表示
	m_pButtonZero->onDraw();
    m_pButtonNega->onDraw();
	m_pButtonUndo->onDraw();
	m_pButtonRedo->onDraw();
	m_pButtonMin->onDraw();
	m_pButtonMax->onDraw();
	m_pButtonCopy->onDraw();
	m_pButtonPaste->onDraw();

	for( int i=0; i<m_nDegree; i++ ){
		m_oArrButtonInc[i].onDraw();
		m_oArrButtonDec[i].onDraw();
	}

    int32 val = getValue();

    // BOOL表示
	if( m_bBool ){
		// 下枠
		pDC->clear();
		pDC->setRect( EV_DIALOG_DEGREE_MAX*(32+1)-1, 32 );
		pDC->setRGBA( EV_DIALOG_BASE_RGBA );
		pDC->setBlendAlpha();
		pDC->draw( m_fX + 64+2+1, m_fY + (32+2) );

		// ラベル
		pDC->clear();
		pDC->setStr( eFONT_DEBUG, (val != 0)? "TRUE": "FALSE" );
		pDC->setScaleXY( 1.0f, 3.0f );
		pDC->setAlignX( eDRAW_ALIGN_X_C );
        pDC->setRGBA( rgba );
		pDC->draw( m_fX + EV_DIALOG_W/2, m_fY + (32+2) - 1 );
	}
	// ラベル表示
	else if( m_pArrLabel != NULL ){
		float labelBaseW = EV_DIALOG_DEGREE_MAX*(32+1)-1;
		float labelBaseH =  32;

		// 下枠
		pDC->clear();
		pDC->setRect( labelBaseW, labelBaseH );
		pDC->setRGBA( EV_DIALOG_BASE_RGBA );
		pDC->setBlendAlpha();
        pDC->draw( m_fX + 64+2+1, m_fY + (32+2) );

		// ラベル（※[INVALID_VAL]は一律[NON]扱い）
		const char* pLabel = ENUM_NOT_FOUND_LABEL;
		if( val >= 0 && val <= m_nValueMax ){ pLabel = m_pArrLabel[val]; }
		else if( val == INVALID_VAL ){ pLabel = ENUM_INVALID_LABEL; }

		pDC->clear();
		pDC->setStr( eFONT_DEBUG, pLabel );
        pDC->setScaleXY( 1.0f, 2.0f );
		pDC->setAlignX( eDRAW_ALIGN_X_C );

		if( pDC->getWidth() > labelBaseW ){
			pDC->setWidth( labelBaseW );
		}

        pDC->setRGBA( rgba );
		pDC->draw( m_fX + EV_DIALOG_W/2, m_fY + (32+2) + 9 );

		// 数字
		pDC->clear();
		pDC->setNum( eNUM_DEBUG, val );
		pDC->setAlignX( eDRAW_ALIGN_X_C );
        pDC->setRGBA( rgba );
		pDC->draw( m_fX + EV_DIALOG_W/2, m_fY + (32+2) + 0 );
	}
	// 数値表示
	else{
		// 下枠
		for( int i=0; i<m_nDegree; i++ ){
			pDC->clear();
			pDC->setRect( 32, 32 );
			pDC->setRGBA( EV_DIALOG_BASE_RGBA );
			pDC->setBlendAlpha();
			pDC->draw( m_fX + (EV_DIALOG_W - (64+2) - (i+1)*(32+1)), m_fY + (32+2) );
		}

        // 負の最小値を反転すると[int32]に収まらないので[int64]で処理する
        int64 digit = val;
        
		// 数値の表示
		bool isMinus = false;
		if( digit < 0 ){
            isMinus = true;
			digit = -digit;
        }
        
		int i = 0;
		do{
			// 数字
			pDC->clear();
			pDC->setSpr( (eSPR)(eSPR_DEBUG_NUMBER_00 + (digit%10)) );
			pDC->setScaleXY( 3.0f, 3.0f );
			pDC->setAlignX( eDRAW_ALIGN_X_C );
            pDC->setRGBA( rgba );
			pDC->draw( m_fX + (EV_DIALOG_W - (64+2) - (i+1)*(32+1)) + 16, m_fY + (32+2) - 1 );
			i++;
			digit /= 10;
		}while( digit > 0 );

		// －(マイナス)表示
		if( isMinus ){
			pDC->clear();
			pDC->setStr( eFONT_DEBUG, "-" );
			pDC->setScaleXY( 3.0f, 3.0f );
			pDC->setAlignX( eDRAW_ALIGN_X_C );
            pDC->setRGBA( rgba );
			pDC->draw( m_fX + (EV_DIALOG_W - (64+2) - (i+1)*(32+1)) + 16, m_fY + (32+2) - 1 );
		}
	}
}

//---------------------
// 値の取得
//---------------------
int32 CEditValueDialog::getValue( void ){
	if( m_bBit ){ return( CEditValue::GetValueForBit( m_pValue, m_eValueType, m_nBit ) ); }
	return( CEditValue::GetValue( m_pValue, m_eValueType ) );
}

//---------------------
// 値の設定
//---------------------
bool CEditValueDialog::setValue( int32 val ){
	if( m_bBit ){ return( CEditValue::SetValueForBit( m_pValue, m_eValueType, val, m_nBit ) ); }
	return( CEditValue::SetValue( m_pValue, m_eValueType, val ) );
}

//---------------------
// 値の増減
//---------------------
bool CEditValueDialog::addValue( int32 add ){
	int32 val = getValue();

    val += add;
    if( val < m_nValueMin ){ val = m_nValueMin; }
    else if( val > m_nValueMax ){ val = m_nValueMax; }

	return( setValue( val ) );
}

