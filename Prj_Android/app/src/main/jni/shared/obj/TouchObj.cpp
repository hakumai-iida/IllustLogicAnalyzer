/*+----------------------------------------------------------------+
  |	Title:		TouchObj.cpp [共通環境]
  |	Comment:	タッチ可能なゲーム要素基底（※単発の入力判定のみ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchObj.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
bool CTouchObj::s_bTouchAreaCheck;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------
// コンストラクタ
//--------------------
CTouchObj::CTouchObj( void ): CObj(){
	clearTouchObj();
}

//--------------------
// クリア
//--------------------
void CTouchObj::clearTouchObj( void ){
    m_nTouchCheckType = eTOUCH_CHECK_TAPPED;    // デフォルト値
    m_bTouchOval = false;
	m_bTouchActive = false;
	m_bTouchWithBackButton = false;
    m_bTouchCheckDrawBack = false;
    
    m_fTouchOfsX = m_fTouchOfsY = 0;
	m_fTouchAreaW = m_fTouchAreaH = 0;
	m_fTouchedX = m_fTouchedY = 0;
	m_fTouchBlockFrame = 0.0f;
    
    m_nTouchAreaRGBA = 0xFF80FFFF;  // デフォルト値
}

//-------------------
// タッチ設定：矩形
//-------------------
void CTouchObj::setTouchRect( int ofsX, int ofsY, int w, int h ){
	m_bTouchOval = false;
	m_fTouchOfsX = ofsX;
	m_fTouchOfsY = ofsY;
	m_fTouchAreaW = w;
	m_fTouchAreaH = h;
}

//-------------------
// タッチ設定：円
//-------------------
void CTouchObj::setTouchOval( int ofsX, int ofsY, int w, int h ){
    m_bTouchOval = true;
    m_fTouchOfsX = ofsX;
    m_fTouchOfsY = ofsY;
    m_fTouchAreaW = w;
    m_fTouchAreaH = h;
}

//-------------------
// タッチされたか？
//-------------------
bool CTouchObj::isTouched( void ){
	// 無効は無視
	if( isBlank() ){
		return( false );
	}

	if( !isTouchActive() || !isTouchAreaValid() ){
		return( false );
	}

	// タッチ判定
	CTouchStatusCell* pTouchStatus;
	float x = getX() + m_fTouchOfsX;
	float y = getY() + m_fTouchOfsY;
	if( m_bTouchOval ){
		pTouchStatus = CTouchMgr::CheckOval( m_nTouchCheckType, x, y, m_fTouchAreaW, m_fTouchAreaH );
	}else{
		pTouchStatus = CTouchMgr::CheckRect( m_nTouchCheckType, x, y, m_fTouchAreaW, m_fTouchAreaH );
	}

	// セルが有効なら座標の取得（※オブジェからみた相対座標／[m_fTouchOfsX/Y]はそのまま）
	if( pTouchStatus != NULL ){
		m_fTouchedX = pTouchStatus->getBaseX() - getX();
		m_fTouchedY = pTouchStatus->getBaseY() - getY();
		return( true );
	}

	// 指定があればバックボタンも見る
	if( m_bTouchWithBackButton ){
        // 判定タイプがタップ／押下であれば（※ドラッグ等の指定はないはずだが用心）
        if( m_nTouchCheckType == eTOUCH_CHECK_TAPPED || m_nTouchCheckType == eTOUCH_CHECK_TOUCHED_DOWN ){
            if( CTouchMgr::CheckBackButton() ){
                return( true );
            }
		}
	}

	return( false );
}

//--------------------
// 更新：上書き
//--------------------
void CTouchObj::update( void ){
    // [CObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preUpdateForObj() ){
        return;
    }

    // [CTouchObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preUpdateForTouchObj() ){
        return;
    }

	// 実更新
	update0();
}

//---------------------------
// [CTouchObj]に関する更新
//---------------------------
bool CTouchObj::preUpdateForTouchObj( void ){
    // タッチブロックフレーム更新
    if( m_fTouchBlockFrame > 0 ){
        m_fTouchBlockFrame -= m_fUpdateDeltaFrame;
    }

    return( true );
}

//--------------------
// 描画：上書き
//--------------------
void CTouchObj::onDraw( void ){
    // [CObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preDrawForObj() ){
        return;
    }
    
    // [CTouchObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preDrawForTouchObj() ){
        return;
    }
    
    // 実描画
    onDraw0();

    // [CTouchObj] 事後処理
    postDrawForTouchObj();

    // [CObj] 事後処理
    postDrawForObj();
}

//-----------------------------
// [CTouchObj]に関する事前描画
//-----------------------------
bool CTouchObj::preDrawForTouchObj( void ){
    // タッチエリア確認（奥）
    if( m_bTouchCheckDrawBack ){
        drawTouchArea();
    }
    
    return( true );
}

//-----------------------------
// [CTouchObj]に関する事前描画
//-----------------------------
void CTouchObj::postDrawForTouchObj( void ){
    // タッチエリア確認（前）
    if( ! m_bTouchCheckDrawBack ){
        drawTouchArea();
    }
}

//------------------------
// 表示：タッチエリア
//------------------------
void CTouchObj::drawTouchArea( void ){
	// 指定が無い／無効は無視
	if( !s_bTouchAreaCheck || !isTouchActive() ){ return; }

	float x = getX();
	float y = getY();

	CDrawCell* pDC = CDrawCell::GetFreeCell();
	pDC->clear();

	if( m_bTouchOval ){
		pDC->setImg( eIMG_DEBUG_CIRCLE );
        pDC->setWidth( m_fTouchAreaW );
        pDC->setHeight( m_fTouchAreaH );
	}else{
		pDC->setRect( m_fTouchAreaW, m_fTouchAreaH );
	}

	pDC->setRGBA( m_nTouchAreaRGBA );
	pDC->setBlendAlpha();
	pDC->draw( x+m_fTouchOfsX, y+m_fTouchOfsY );
}
