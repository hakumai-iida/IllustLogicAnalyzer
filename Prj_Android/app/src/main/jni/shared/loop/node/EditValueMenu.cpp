/*+----------------------------------------------------------------+
  |	Title:		EditValueMenu.hpp [共通環境]
  |	Comment:	値編集メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "EditValueMenu.hpp"
#include "StrMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 座標／サイズ
#define BASE_VALUE_AREA_W	56

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
//-------------------
// コンストラクタ
//-------------------
CEditValueMenu::CEditValueMenu( eGRP grpId, int cW, int cH ): CScrollBarMenu( grpId, cW, cH ){
    // ブールの直接編集はデフォルトでオン
    m_bEditBoolDirect = true;
}

//-------------------
// デストラクタ
//-------------------
CEditValueMenu::~CEditValueMenu( void ){
	releaseEditItem();
}

//-------------------
// 解放
//-------------------
void CEditValueMenu::releaseEditItem( void ){
	SAFE_DELETE_ARRAY( m_stArrEditItem );
}

//-------------------
// アイテム数設定
//-------------------
void CEditValueMenu::setItemNum( int num, bool isBlockRequired ){
	// 領域の解放
	releaseEditItem();
    releaseItemBlock();

	// メニュー項目数設定
	CScrollBarMenu::setItemNum( num );
    
    if( isBlockRequired ){
        setItemBlock();
    }

	// 枠の確保
	m_stArrEditItem = new stEDIT_VALUE_ITEM_MENU_ITEM[m_nItemNum];
}

//-------------------
// アイテム設定
//-------------------
bool CEditValueMenu::setItemAt( int id, stEDIT_VALUE_ITEM_MENU_ITEM* pItem, bool isBlock ){
	// 無効は無視
	if( id < 0 || id >= m_nItemNum ){ return( false ); }
	if( pItem == NULL  ){ return( false ); }

	// 丸毎コピー
	m_stArrEditItem[id] = *pItem;

    setItemBlockAt( id, isBlock );
    
	return( true );
}

//-------------------
// アイテム設定
//-------------------
bool CEditValueMenu::setItemAt( int id, const char* pName, void* pVal, eEDIT_VALUE_TYPE type, int32 min, int32 max, bool isBlock ){
	// 無効は無視
	if( id < 0 || id >= m_nItemNum ){ return( false ); }

	// 対象のクリア（セパレーターフラグは前もって指定される想定なので退避＆復元させている）
	stEDIT_VALUE_ITEM_MENU_ITEM* pItem = &m_stArrEditItem[id];
    bool bSeparator = pItem->bSeparator;
	memset( pItem, 0, sizeof(stEDIT_VALUE_ITEM_MENU_ITEM) );
    pItem-> bSeparator = bSeparator;

	// 設定
	pItem->pName	= pName;
	pItem->pVal		= pVal;
	pItem->eType	= type;
	pItem->nMin		= min;
	pItem->nMax		= max;

    setItemBlockAt( id, isBlock );

    return( true );
}

//-------------------
// アイテム設定（※ラベル値）
//-------------------
bool CEditValueMenu::setItemAtAsLabel( int id, const char* pName, void* pVal, eEDIT_VALUE_TYPE type, int32 min, int32 max, const char** pArrLabel, bool isBlock ){
	if( !setItemAt( id, pName, pVal, type, min, max, isBlock ) ){ return( false ); }

	m_stArrEditItem[id].pArrLabel = pArrLabel;
	return( true );
}

//-------------------
// アイテム設定（※ブール値）
//-------------------
bool CEditValueMenu::setItemAtAsBool( int id, const char* pName, void* pVal, eEDIT_VALUE_TYPE type, bool isBlock ){
	if( !setItemAt( id, pName, pVal, type, 0, 1, isBlock ) ){ return( false ); }

	m_stArrEditItem[id].bBool = true;

    setItemBlockAt( id, isBlock );

    return( true );
}

//---------------------
// アイテム設定（※ブール値）
//---------------------
bool CEditValueMenu::setItemAtAsBit( int id, const char* pName, void* pVal, eEDIT_VALUE_TYPE type, int bit, bool isBlock ){
	if( !setItemAtAsBool( id, pName, pVal, type, isBlock ) ){ return( false ); }

	m_stArrEditItem[id].bBit = true;
	m_stArrEditItem[id].nBit = bit;

    setItemBlockAt( id, isBlock );

    return( true );
}

//-------------------
// セパレータの設定
//-------------------
void CEditValueMenu::setSeparatorAt( int id, bool flag ){
    // 無効は無視
    if( id < 0 || id >= m_nItemNum ){ return; }
    
    m_stArrEditItem[id].bSeparator = flag;
}


//-------------------
// 横幅の取得
//-------------------
stEDIT_VALUE_ITEM_MENU_ITEM* CEditValueMenu::getItemAt( int at ){
	if( at < 0 || at >= m_nItemNum ){
		return( NULL );
	}

	return( &m_stArrEditItem[at] );
}

//-------------------
// 横幅の取得
//-------------------
stEDIT_VALUE_ITEM_MENU_ITEM* CEditValueMenu::getSelectedItem( void ){
	return( getItemAt( m_nSelect ) );
}

//-------------------
// 値編集ダイアログの設定
//-------------------
void CEditValueMenu::setEditValueDialog( CEditValueDialog* pDialog ){
	stEDIT_VALUE_ITEM_MENU_ITEM* pItem = getSelectedItem();
    const char* pName = pItem->pName;
    const char** pArrLabel = pItem->pArrLabel;

	// ビット編集として設定
	if( pItem->bBit ){
		pDialog->setAsBit( pItem->pVal, pItem->eType, pItem->nBit );
	}
	// ブール編集として設定
	else if( pItem->bBool ){
		pDialog->setAsBool( pItem->pVal, pItem->eType);
	}
	// 値編集として設定
	else{
        int min = pItem->nMin;
        int max = pItem->nMax;
        
		pDialog->set( pItem->pVal, pItem->eType, min, max );
	}

	pDialog->setName( pName );
	pDialog->setLabel( pArrLabel );
    
	pDialog->fixDialog();
}

//-------------------
// 横幅の取得
//-------------------
float CEditValueMenu::calcInternalW( void ){
    // ここではタイトル幅は計算にいれないでおく（※事前に値が確定しないため）
	m_nValueOfsX = m_nMaxCenterW - BASE_VALUE_AREA_W;
	return( m_nMaxCenterW );
}

//-------------------
// 縦幅の取得
//-------------------
float CEditValueMenu::calcInternalH( void ){
    int numSeparator = 0;
    
#if 0
    // メニュー項目のタップ判定がずれてしまうので一旦無効化
    for( int i=0; i<m_nItemNum; i++ ){
        if( m_stArrEditItem[i].bSeparator ){
            numSeparator++;
        }
    }
#endif
    
	return( m_nItemNum * SM_BASE_ITEM_STEP_Y - 1.0f + 2*numSeparator );
}

//-------------------
// 項目の縦幅取得
//-------------------
float CEditValueMenu::getItemTapH( void ){
	return( SM_BASE_ITEM_STEP_Y );
}

//-------------------
// 更新
//-------------------
void CEditValueMenu::onUpdate0( void ){
	CScrollBarMenu::onUpdate0();

	m_bChanged = false;
	if( m_bDecided && m_bEditBoolDirect ){
		stEDIT_VALUE_ITEM_MENU_ITEM* pItem = getSelectedItem();

		// ビット値反転
		if( pItem->bBit ){
			CEditValue::SwapValueForBit( pItem->pVal, pItem->eType, pItem->nBit );
			m_bChanged = true;
			m_bDecided = false;
		}
		// ブール値反転
		else if( pItem->bBool ){
			CEditValue::SwapValue( pItem->pVal, pItem->eType );
			m_bChanged = true;
			m_bDecided = false;
		}
	}
}

//-------------------
// 描画
//-------------------
void CEditValueMenu::onDraw0( void ){
	CScrollBarMenu::onDraw0();

	// シザー（※無効は無視）
	if( ! CMainRenderer::SetScissor( m_fX+m_fCenterX, m_fY+m_fCenterY-1, m_fCenterW, m_fCenterH+2 ) ){
		return;
	}

	DWORD rgba;
	int32 val;
	stEDIT_VALUE_ITEM_MENU_ITEM* pItem;
	char* buf = CMemMgr::GetBufFromTempStr();
	CDrawCell* pDC = CDrawCell::GetFreeCell();
	float x = m_fX + m_fCenterX - m_fInternalOfsX;
	float y = m_fY + m_fCenterY - m_fInternalOfsY;
	for( int i=0; i<m_nItemNum; i++ ){
		pItem = &m_stArrEditItem[i];

        // 表示色
        if( m_nSelect == i ){
            if( isDark() ){ rgba = SM_ITEM_SELECTED_RGBA_DARK; }
            else{ rgba = SM_ITEM_SELECTED_RGBA; }
        }else{
            if( isDark() ){ rgba = SM_ITEM_RGBA_DARK; }
            else{ rgba = SM_ITEM_RGBA; }
        }
        
        if( checkItemBlockedAt(i) ){
            rgba = CConst::CalcMultiForRGBA( rgba, SM_ITEM_BLOCKED_RGBA_RATE );
        }
        
        //----------------------------------
        // セパレータ
        //----------------------------------
        if( pItem->bSeparator ){
            pDC->clear();
            pDC->setRect( m_fCenterW, 2 );
            pDC->setRGBA( 0x40404080 );
            pDC->draw( x, y-2 );

#if 0
            // 位置調整は無効化（タップ判定との兼ね合い）
            y += 2;
#endif
        }

        //----------------------------------
		// 項目名表示：左寄せ
        //----------------------------------
		pDC->clear();
		pDC->setStr( eFONT_DEBUG, pItem->pName );
        pDC->setScaleXY( 1.0f, 2.0f );
        pDC->setRGBA( rgba );

		// 横幅が大きすぎる場合は横幅に収める
		if( pDC->getOriginalWidth() > (m_fCenterW-64) ){
			pDC->setWidth( m_fCenterW-64 );
		}

		pDC->draw( x, y-2 );

        //----------------------------------
		// 値のラベル設定
        //----------------------------------
		if( pItem->bBit ){
			val = CEditValue::GetValueForBit( pItem->pVal, pItem->eType, pItem->nBit );
		}else{
			val = CEditValue::GetValue( pItem->pVal, pItem->eType );
		}

		// [bool]形式
		if( pItem->bBool ){
            sprintf( buf, "%s", (val!=0)? "TRUE": "FALSE" );
            
        }
        // ラベル形式
        else if( pItem->pArrLabel != NULL ){
            const char* pLabel = ENUM_NOT_FOUND_LABEL;
            if( val == INVALID_VAL ){ pLabel = ENUM_INVALID_LABEL; }
            else if( val >= 0 && val <= pItem->nMax ){ pLabel = pItem->pArrLabel[val]; }
            sprintf( buf, "%s", pLabel );
        }

        // 数値形式
		else{ sprintf( buf, "%d", val ); }

        //----------------------------------
        // 値表示：右寄せ
        //----------------------------------
        pDC->clear();
        pDC->setStr( eFONT_DEBUG, buf );
        pDC->setScaleXY( 1.0f, 2.0f );
        pDC->setRGBA( rgba );
        pDC->setAlignX( eDRAW_ALIGN_X_R );

        // 横幅が大きすぎる場合は横幅に収める
		if( pDC->getWidth() > m_nValueOfsX ){
			pDC->setWidth( m_nValueOfsX );
		}
        
		pDC->draw( x+m_nValueOfsX+BASE_VALUE_AREA_W-4, y-2 );

		y += SM_BASE_ITEM_STEP_Y;
	}

	CMainRenderer::SetScissorOff();
}
