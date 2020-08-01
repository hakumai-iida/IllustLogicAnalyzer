/*+----------------------------------------------------------------+
  |	Title:		PullDownMenu.hpp [共通環境]
  |	Comment:	プルダウンメニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "PullDownMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 各種サイズ
#define PDM_INTERNAL_W			220
#define PDM_INTERNAL_H          204
#define PDM_DRAW_W_MAX          (m_fCenterW-18)

// アニメカウント
#define PDM_EXPAND_ANIM_COUNT   12

// 表示色
#define PDM_BASE_RGBA		    0xFFFFFFC0
#define PDM_FRAME_RGBA			0x000000FF

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
CPullDownMenu::CPullDownMenu( bool isDirUp, bool isButtonRight, bool isAutoClose ):
               CStrMenu( eGRP_INVALID, PDM_INTERNAL_W, PDM_INTERNAL_H )
{
	m_bExpandDirUp = isDirUp;
    m_bPutButtonRight = isButtonRight;
    m_bAutoCloseIfDecided = isAutoClose;

	// ボタン
	m_oButton.setGrp( eGRP_DEBUG_BUTTON_U );
	m_oButton.setFlipY( !m_bExpandDirUp );
}

//-------------------
// デストラクタ
//-------------------
CPullDownMenu::~CPullDownMenu( void ){}

//-------------------
// メニュー確定
//-------------------
void CPullDownMenu::fixMenu( void ){
    // 状況を保持する
#if 0
    m_bExpand = false;
    m_nExpandCount = 0;
#endif
    
    // 確定
    fixBaseMenu();
}

//-------------------
// 最大領域の設定
//-------------------
void CPullDownMenu::setMaxCenterWH( int w, int h ){
    m_nMaxCenterW = w;
    m_nMaxCenterH = h;
    
    setCenterWH( w, h );
}

//-------------------
// 配置
//-------------------
void CPullDownMenu::put( float x, float y ){
	m_fBaseX = x;
	m_fBaseY = y;

    // 配置
    CStrMenu::put( x, y );

    // 確定
    fixBaseMenu();
}

//-------------------
// 入場
//-------------------
void CPullDownMenu::enter( void ){
    // 状況を保持する
#if 0
	m_bExpand = false;
	m_nExpandCount = 0;
#endif

	CStrMenu::enter();
	m_oButton.enter();
    m_oScrollBar.enter();   // スクロールバーは常に表示
}

//-------------------
// 退場
//-------------------
void CPullDownMenu::exit( void ){
    // 状況を保持する
#if 0
	m_bExpand = false;
	m_nExpandCount = 0;
#endif

	CStrMenu::exit();
	m_oButton.exit();
    m_oScrollBar.exit();
}

//-------------------
// 活動指定
//-------------------
void CPullDownMenu::setActive( bool isActive ){
	CStrMenu::setActive( isActive );
	m_oButton.setActive( isActive );
    m_oScrollBar.setActive( isActive );
}

//-------------------
// 表示領域の設定
//-------------------
void CPullDownMenu::setCenterWH( float w, float h ){
    m_fInternalW = m_fCenterW = w;
    m_fInternalH = m_fCenterH = h;
    
    m_fCenterX = 0;
    m_fCenterY = 0;
    m_fWinW = m_fInternalW;
    m_fWinH = m_fInternalH;
    
    // タップ領域
    setTouchRect( m_fCenterX, m_fCenterY, m_fCenterW, m_fCenterH );
}

//-------------------
// 横幅の取得
//-------------------
float CPullDownMenu::calcInternalW( void ){
    return( m_fInternalW/*PDM_INTERNAL_W*/ );
}

//-------------------
// 縦幅の取得
//-------------------
float CPullDownMenu::calcInternalH( void ){
    int itemNum = m_nItemNum;
    if( itemNum <= 0 ){
        itemNum = 1;
    }

    // 開閉でサイズがかわるのであれば補完
	float hClosed = SM_BASE_ITEM_STEP_Y;
	float hExpand = itemNum * SM_BASE_ITEM_STEP_Y;
    if( hClosed != hExpand ){
        float rate = CConst::ApplyAsyncType( eASYNC_TYPE_LINEAR, ((float)m_nExpandCount)/PDM_EXPAND_ANIM_COUNT );
        float h = (int)CConst::CalcAsyncF( rate, hClosed, hExpand );    // 拡張アニメ時にガクつくので整数に
        return( h );
    }
    
	return( hClosed );
}

//-------------------
// 項目の縦幅取得
//-------------------
float CPullDownMenu::getItemTapH( void ){
	return( SM_BASE_ITEM_STEP_Y );
}

//-------------------
// 更新
//-------------------
void CPullDownMenu::onUpdate0( void ){
	int count0 = m_nExpandCount;

    // 開いていたら
    if( m_bExpand ){
        if( m_nExpandCount < PDM_EXPAND_ANIM_COUNT ){
            m_nExpandCount++;
        }
    }
	// 閉じていたら
    else{
        if( m_nExpandCount > 0 ){
            m_nExpandCount--;
        }
	}

	// アニメ中であれば調整
	if( m_nExpandCount != count0 ){
        fixBaseMenu();
        
		return;
	}

	// ボタン動作
	m_oButton.onUpdate();

	// メニュー動作
	int select0 = m_nSelect;
	CStrMenu::onUpdate0();

    // 開いていたら
    if( m_bExpand ){
        // 開閉ボタンが押された／決定されたら閉じる
        if( m_oButton.isTapped() || (m_bAutoCloseIfDecided && isDecided() ) ){
            m_bExpand = false;
        }
    }
	// 閉じていたら
	else{
		// 開閉ボタンが押された／タップされたら開く
		if( m_oButton.isTapped() || isTapped() ){
			m_bExpand = true;
			setSelect( select0 );
            
            // 決定扱いにはしない
            m_bDecided = false;
		}
	}
}

//-------------------
// 描画
//-------------------
void CPullDownMenu::onDraw0( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();
    
    // 色
    bool bDark = isDark();
    DWORD rgba;
    DWORD rgba0 = GET_NODE_RGBA( bDark, false );
    DWORD rgbaBase = CConst::CalcMultiForRGBA( PDM_BASE_RGBA, rgba0 );
    DWORD rgbaBorder = CConst::CalcMultiForRGBA( PDM_FRAME_RGBA, rgba0 );

    // 位置＆サイズ
    float x = m_fX + m_fCenterX;
	float y = m_fY + m_fCenterY;
	float w = calcInternalW();
	float h = calcInternalH();
    
    if( w > m_fCenterW ){
        w = m_fCenterW;
    }
    
    if( h > m_fCenterH ){
        h = m_fCenterH;
    }

	// 土台表示
	CDrawCommon::RenderWindowRect( x, y, w, h, rgbaBase, rgbaBorder, 5, 5, 2 );

    // シザー（※無効は無視）
    if( ! CMainRenderer::SetScissor( x-2, y-2, w+4, h+4 ) ){
        return;
    }
    
    // 座標調整
    x = x - m_fInternalOfsX + 1;
    y = y - m_fInternalOfsY;

	// 拡張カウントが有効であれば項目の表示
	float rate = CConst::ApplyAsyncType( eASYNC_TYPE_LINEAR, ((float)m_nExpandCount)/PDM_EXPAND_ANIM_COUNT );
	int alpha = CConst::CalcAsyncI( rate, 0x00, 0xFF );
    float sY = rate;
	if( m_nExpandCount > 0 ){
		for( int i=0; i<m_nItemNum; i++ ){
			if( m_nSelect == i ){ continue; }

            // 表示色
            if( bDark){ rgba = SM_ITEM_RGBA_DARK; }
            else{ rgba = SM_ITEM_RGBA; }
           
            if( checkItemBlockedAt( i ) ){
                rgba = CConst::CalcMultiForRGBA( rgba, SM_ITEM_BLOCKED_RGBA_RATE );
            }
            
			pDC->clear();
			pDC->setStr( eFONT_DEBUG, m_pArrItem[i] );
            pDC->setRGBA( rgba );
            pDC->setAlpha( alpha );
            pDC->setScaleXY( 2.0f, 2.0f );
            if( pDC->getWidth() > PDM_DRAW_W_MAX ){
                pDC->setWidth( PDM_DRAW_W_MAX );
            }
			pDC->draw( x, y + (int)(sY*SM_BASE_ITEM_STEP_Y*i) );      // 下合わせ時にガクつくので整数で
		}
	}

	// 選択項目は最前面に描く
	if( m_nSelect >= 0 ){
        // 表示色
        if( bDark){ rgba = SM_ITEM_SELECTED_RGBA_DARK; }
        else{ rgba = SM_ITEM_SELECTED_RGBA; }

        if( checkItemBlockedAt( m_nSelect ) ){
            rgba = CConst::CalcMultiForRGBA( rgba, SM_ITEM_BLOCKED_RGBA_RATE );
        }

		pDC->clear();
        pDC->setStr( eFONT_DEBUG, m_pArrItem[m_nSelect] );
        pDC->setRGBA( rgba );
        pDC->setScaleXY( 2.0f, 2.0f );
        if( pDC->getWidth() > PDM_DRAW_W_MAX ){
            pDC->setWidth( PDM_DRAW_W_MAX );
        }
		pDC->draw( x, y + (int)(sY*SM_BASE_ITEM_STEP_Y*m_nSelect) );  // 下合わせ時にガクつくので整数で
	}

    CMainRenderer::SetScissorOff();

    // 部品表示
    m_oScrollBar.onDraw();
	m_oButton.onDraw();
}

//-----------------------
// 規定の調整
//-----------------------
void CPullDownMenu::fixBaseMenu( void ){
    int select = getSelect();
    
    // 窓のサイズと位置
    if( m_bExpandDirUp ){
        float h = calcInternalH();
        if( h > m_nMaxCenterH ){
            h = m_nMaxCenterH;
        }
        
        float x = m_fBaseX;
        float y = m_fBaseY - h + SM_BASE_ITEM_STEP_Y;
        CStrMenu::put( x, y );
    }
    CStrMenu::fixMenu();
    
    // ボタン＆スクロールバー確定
    fixButton();
    fixScrollBar();
    
    setSelect( select );
}

//----------------------
// スクロールバーの確定
//----------------------
void CPullDownMenu::fixScrollBar( void ){
    // スクロールバーの配置
    m_oScrollBar.put( m_fX+m_fCenterW-1, m_fY-1 );
    
    // スクロールバーのサイズ
    m_oScrollBar.setForDisp( true, 2, m_fCenterH+2, 1 );
}

//----------------------
// ボタンの確定
//----------------------
void CPullDownMenu::fixButton( void ){
    // ボタンの配置
    if( m_bPutButtonRight){
        m_oButton.put( m_fBaseX+m_fInternalW+/*PDM_INTERNAL_W*/+8, m_fBaseY-5 );
    }else{
        m_oButton.put( m_fBaseX-40, m_fBaseY-5 );
    }
    
    // ボタンの向き
    m_oButton.setFlipY( (m_bExpandDirUp&&m_bExpand) || (!m_bExpandDirUp&&!m_bExpand) );
}
