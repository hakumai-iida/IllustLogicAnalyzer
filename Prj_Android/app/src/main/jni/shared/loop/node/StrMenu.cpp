/*+----------------------------------------------------------------+
  |	Title:		StrMenu.hpp [共通環境]
  |	Comment:	文字列メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "StrMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
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
CStrMenu::CStrMenu( eGRP grpId, int cW, int cH ): CScrollBarMenu( grpId, cW, cH ){
}

//-------------------
// デストラクタ
//-------------------
CStrMenu::~CStrMenu( void ){
	releaseItem();
}

//-------------------
// 解放
//-------------------
void CStrMenu::releaseItem( void ){
	SAFE_DELETE_ARRAY( m_pArrItem );
	SAFE_DELETE_ARRAY( m_cArrItemBuf );
}

//-------------------
// メニューセット
//-------------------
void CStrMenu::setItemNum( int num, bool isBlockRequired ){
	// 領域解放
	releaseItem();
    releaseItemBlock();

	// メニュー項目数設定
	CScrollBarMenu::setItemNum( num );

	// 枠の確保（※確保先は呼び出し元に委ねる）
	m_pArrItem = new char*[m_nItemNum];
	m_cArrItemBuf = new char[m_nItemNum*COMMON_NAME_LEN];

    // 指定があればブロック設定
    if( isBlockRequired ){
        setItemBlock();
    }

	// 各枠へバッファの割当
	for( int i=0; i<m_nItemNum; i++ ){
		m_pArrItem[i] = &m_cArrItemBuf[i*COMMON_NAME_LEN];
	}
}

//-------------------
// メニュー項目設定
//-------------------
void CStrMenu::setItemAt( int id, const char* pItem, bool isBlock ){
	// 無効は無視
	if( id < 0 || id >= m_nItemNum ){
		LOGE( "@ CStrMenu::setItemAt: INVALID ID: %d/%d\n", id, m_nItemNum );
		return;
	}

	// コピー（※バッファに収まらない場合は切り捨て）
	util_str_cpy( m_pArrItem[id], pItem, COMMON_NAME_LEN );
    
    // ブロック指定
    setItemBlockAt( id, isBlock );
}

//-------------------
// メニュー項目更新
//-------------------
void CStrMenu::updateItemLabelAt( int id, const char* pItem ){
    // 無効は無視
    if( id < 0 || id >= m_nItemNum ){
        LOGE( "@ CStrMenu::updateItemLabelAt: INVALID ID: %d/%d\n", id, m_nItemNum );
        return;
    }

    // コピー（※バッファに収まらない場合は切り捨て）
    util_str_cpy( m_pArrItem[id], pItem, COMMON_NAME_LEN );
}

//-------------------
// メニュー項目設定：一括
//-------------------
void CStrMenu::setItemArrayThenFix( const char** pArrItem, const bool* pArrBlock ){
	// 要素数の算出
	int num = 0;
	while( pArrItem[num] != NULL ){
		num++;
	}
	setItemNum( num, pArrBlock!=NULL );

	// 項目の割当
	for( int i=0; i<num; i++ ){
		setItemAt( i, pArrItem[i] );
	}
    
    // 指定があればブロック設定
    if( pArrBlock != NULL ){
        for( int i=0; i<num; i++ ){
            setItemBlockAt( i, pArrBlock[i] );
        }
    }

	// 確定
	fixMenu();
}

//------------------------------
// 選択対象のラベル（文字列）の取得
//------------------------------
const char* CStrMenu::getSelectedLabel( void ){
    if( m_nSelect >= 0 && m_nSelect < m_nItemNum ){
        return( m_pArrItem[m_nSelect] );
    }
    
    return( NULL );
}

//-------------------
// 横幅の取得
//-------------------
float CStrMenu::calcInternalW( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();

	// タイトル幅の確認
	pDC->clear();
	pDC->setStr( eFONT_DEBUG, m_cArrTitle );
    pDC->setScaleXY( 1.0f, 2.0f );
	float maxW = pDC->getWidth();

	// 項目の横幅の確認
	for( int i=0; i<m_nItemNum; i++ ){
		pDC->clear();
		pDC->setStr( eFONT_DEBUG, m_pArrItem[i] );
        pDC->setScaleXY( 1.0f, 2.0f );
		float w = pDC->getWidth();
		if( maxW < w ){
			maxW = w;
		}
	}

	return( maxW );
}

//-------------------
// 縦幅の取得
//-------------------
float CStrMenu::calcInternalH( void ){
	return( m_nItemNum * SM_BASE_ITEM_STEP_Y - 1.0f );
}

//-------------------
// 項目の縦幅取得
//-------------------
float CStrMenu::getItemTapH( void ){
	return( SM_BASE_ITEM_STEP_Y );
}

//-------------------
// 描画
//-------------------
void CStrMenu::onDraw0( void ){
	CScrollBarMenu::onDraw0();

	// シザー（※無効は無視）
	if( ! CMainRenderer::SetScissor( m_fX+m_fCenterX, m_fY+m_fCenterY-1, m_fCenterW, m_fCenterH+2 ) ){
		return;
	}

	CDrawCell* pDC = CDrawCell::GetFreeCell();
	float x = m_fX + m_fCenterX - m_fInternalOfsX;
	float y = m_fY + m_fCenterY - m_fInternalOfsY;
	for( int i=0; i<m_nItemNum; i++ ){
		pDC->clear();
        pDC->setStr( eFONT_DEBUG, m_pArrItem[i] );
        pDC->setScaleXY( 1.0f, 2.0f );
        
        DWORD rgba;
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

        pDC->setRGBA( rgba );
        pDC->draw( x, y-2 );
		y += SM_BASE_ITEM_STEP_Y;
	}

	CMainRenderer::SetScissorOff();
}
