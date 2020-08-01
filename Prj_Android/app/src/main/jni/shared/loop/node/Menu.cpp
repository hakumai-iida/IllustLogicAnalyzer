/*+----------------------------------------------------------------+
  |	Title:		Menu.cpp [共通環境]
  |	Comment:	メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Menu.hpp"

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
CMenu::CMenu( eGRP grpId, int cW, int cH, bool isScrollY, int numInBlock ): CWin( grpId, cW, cH ){
	setMaxCenterWH( cW, cH );
	setScrollY( isScrollY );
	setNumInBlock( numInBlock );

	// 無効値を選択しておく
	m_nSelect = -1;
}

//--------------------
// デストラクタ
//--------------------
CMenu::~CMenu( void ){
    releaseItemBlock();
}

//-------------------
// メニュー確定
//-------------------
void CMenu::fixMenu( void ){
	int winW = m_nMaxCenterW;
	int winH = m_nMaxCenterH;
	int cW = (int)calcInternalW();
	int cH = (int)calcInternalH();
	float ofsX = (int)getInternalOfsX();
	float ofsY = (int)getInternalOfsY();

	// 表示領域を最大可動範囲に抑える
	// 実際の窓の大きさが表示領域をこえている場合は窓内でスクロールすることになる
	if( cW < m_nMaxCenterW ){ winW = cW; }
	if( cH < m_nMaxCenterH ){ winH = cH; }

	// 窓の設定
	setCenterWH( winW, winH );
	setInternalWH( cW, cH );

	// スクロール位置を戻す
	setInternalOfsX( ofsX );
	setInternalOfsY( ofsY );

	// 選択調整
	fixSelect();
}

//-------------------
// 選択対象設定
//-------------------
int CMenu::setSelect( int select ){
	m_nSelect = select;
	return( fixSelect( true ) );
}

//-------------------------------------------------------------
// 選択対象増減（※フリックによるスクロールではないのでループ指定ができる）
//-------------------------------------------------------------
int CMenu::addSelect( int add, bool isLoopable ){
    m_nSelect += add;

    // 指定があればループ判定
    if( isLoopable ){
        if( m_nSelect < 0 ){
            m_nSelect = m_nItemNum - 1;
        }else if( m_nSelect >= m_nItemNum ){
            m_nSelect = 0;
        }
    }

    // 確定（※このなかではループしない範囲判定が行われる）
    return( fixSelect( true ) );
}

//-------------------
// 選択対象調整
//-------------------
int CMenu::fixSelect( bool isScrollAdjust ){
	// 要素０なら負になるように
	if( m_nSelect < 0 ){ m_nSelect = 0; }
	if( m_nSelect >= m_nItemNum ){ m_nSelect = m_nItemNum - 1; }

	if( m_bScrollY ){ fixSelectForScrollY( isScrollAdjust ); }
	else{ fixSelectForScrollX( isScrollAdjust ); }
    
    return( m_nSelect );
}

//--------------------------
// ブロック設定
//--------------------------
void CMenu::setItemBlock( void ){
    // 用心
    releaseItemBlock();
    
    // 領域確保（※確保先は呼び出し元に委ねる）
    m_bArrItemBlock = new bool[m_nItemNum];
}

//--------------------------
// ブロック要素開放
//--------------------------
void CMenu::releaseItemBlock( void ){
    m_bItemBlockAll = false;
    SAFE_DELETE_ARRAY( m_bArrItemBlock );
}

//--------------------------
// 対象をブロックする
//--------------------------
void CMenu::setItemBlockAt( int at, bool flag ){
    if( m_bArrItemBlock != NULL ){
        if( at >= 0 && at < m_nItemNum ){
             m_bArrItemBlock[at] = flag;
        }
    }
}

//--------------------------
// 対象がブロックされているか？
//--------------------------
bool CMenu::checkItemBlockedAt( int at ){
    // 全ブロックが立っていたら常に[true]
    if( m_bItemBlockAll ){
        return( true );
    }
    
    if( m_bArrItemBlock != NULL ){
        if( at >= 0 && at < m_nItemNum ){
            return( m_bArrItemBlock[at] );
        }
    }
    
    return( false );
}

//-------------------
// 窓の可動横幅の算出
//-------------------
float CMenu::calcInternalW( void ){
	return( m_nMaxCenterW );
}

//-------------------
// 窓の可動縦幅の算出
//-------------------
float CMenu::calcInternalH( void ){
	return( m_nMaxCenterH );
}

//-------------------
// 項目の横幅取得
//-------------------
float CMenu::getItemTapW( void ){
	if( m_nItemNum < 0 ){
		return( m_nMaxCenterW/m_nItemNum );
	}
	return( 0 );
}

//-------------------
// 項目の縦幅取得
//-------------------
float CMenu::getItemTapH( void ){
	if( m_nItemNum < 0 ){
		return( m_nMaxCenterH/m_nItemNum );
	}
	return( 0 );
}

//-------------------------------
// 選択対象調整：Ｘ方向スクロール
//-------------------------------
void CMenu::fixSelectForScrollX( bool isScrollAdjust ){
	// スクロール調整
	if( isScrollAdjust ){
		float ofsX = getInternalOfsX();
		float tapItemW = getItemTapW();
		float selectedOfsX = tapItemW * m_nSelect / m_nNumInBlock;

		// 選択項目がスクロール領域内に収まるように（※左右に項目がある場合、その項目が見えるように）
		if( ofsX > (selectedOfsX-tapItemW/2) ){
			ofsX = selectedOfsX - tapItemW/2;
		}else if( (ofsX+m_fCenterW) < (selectedOfsX+(3*tapItemW/2)) ){
			ofsX = selectedOfsX + (3*tapItemW/2) - m_fCenterW;
		}

		setInternalOfsX( ofsX );
	}
}

//-------------------------------
// 選択対象調整：Ｙ方向スクロール
//-------------------------------
void CMenu::fixSelectForScrollY( bool isScrollAdjust ){
	// スクロール調整
	if( isScrollAdjust ){
		float ofsY = getInternalOfsY();
		float tapItemH = getItemTapH();
		float selectedOfsY = tapItemH * m_nSelect / m_nNumInBlock;

		// 選択項目がスクロール領域内に収まるように（※上下に項目がある場合、その項目が見えるように）
		if( ofsY > (selectedOfsY-tapItemH/2) ){
			ofsY = selectedOfsY - tapItemH/2;
		}else if( (ofsY+m_fCenterH) < (selectedOfsY+(3*tapItemH/2)) ){
			ofsY = selectedOfsY + (3*tapItemH/2) - m_fCenterH;
		}

		setInternalOfsY( ofsY );
	}
}

//-------------------
// 更新
//-------------------
void CMenu::onUpdate0( void ){
	CWin::onUpdate0();
	m_bDecided = false;
    m_bBlocked = false;
	m_bPressedLong = false;
	m_bSelectChanged = false;

	// 無効は無視
	if( m_nItemNum <= 0 ){
		return;
	}

	// 長押し
	CTouchStatusCell* pCell = getCellPressed();
	if( pCell != NULL ){
		if( pCell->isPressedLong() ){
			m_bPressedLong = true;
		}
	}

	// タップされたら
	bool bTapped = isTapped();
	if( bTapped || m_bPressedLong ){
		int select0 = m_nSelect;

		// 選択対象の検出
		if( bTapped ){ pCell = getCellTapped(); }
		else{ pCell = getCellPressed(); }

		int target = 0;
		if( m_bScrollY ){
			float y = pCell->getBaseY() - (m_fY+m_fCenterY) + m_fInternalOfsY;
			target = m_nNumInBlock*((int)(y / getItemTapH()));
		}else{
			float x = pCell->getBaseX() - (m_fX+m_fCenterX) + m_fInternalOfsX;
			target = m_nNumInBlock*((int)(x / getItemTapW()));
		}

		// 二次元メニューであれば
		if( m_nNumInBlock > 1 ){
			if( m_bScrollY ){
				float x = pCell->getBaseX() - (m_fX+m_fCenterX) + m_fInternalOfsX;
				target += (int)(x / getItemTapW());
			}else{
				float y = pCell->getBaseY() - (m_fY+m_fCenterY) + m_fInternalOfsY;
				target += (int)(y / getItemTapH());
			}
		}

		// 選択確定
		m_nSelect = target;
		fixSelect();

		if( m_nSelect == select0 && bTapped ){
			if( checkItemBlockedAt( m_nSelect ) ){
                m_bBlocked = true;
			}else{
				m_bDecided = true;
			}
		}else if( m_nSelect != select0 ){
			m_bSelectChanged = true;
		}
	}
}
