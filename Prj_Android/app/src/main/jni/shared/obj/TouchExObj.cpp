/*+----------------------------------------------------------------+
  |	Title:		TouchExObj.cpp [共通環境]
  | Comment:    タッチ可能ゲーム要素規定：拡張（※ドラッグ／長押し等に対応）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchExObj.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------
// コンストラクタ
//-----------------------
CTouchExObj::CTouchExObj( void ): CTouchObj(){
    clearTouchExObj();
}

//-----------------------
// クリア
//-----------------------
void CTouchExObj::clearTouchExObj( void ){
    m_fTouchExPressedFrameMax = 0.0f;
    m_fTouchExPressedFrame = 0.0f;
    m_nTouchExCellId = -1;

    resetTouchExObjForUpdate();
}

//-----------------------
// リセット（更新毎に呼ばれる）
//-----------------------
void CTouchExObj::resetTouchExObjForUpdate( void ){
    m_bTouchExPressed = false;
    m_bTouchExPressedDown = false;
    m_bTouchExLongPressed = false;
    m_bTouchExTapped = false;
    m_bTouchExFlicked = false;
    
    m_fTouchExPressedX = m_fTouchExPressedY = 0.0f;
    m_fTouchExTappedX = m_fTouchExTappedY = 0.0f;
    m_fTouchExDragDistX = m_fTouchExDragDistY = 0.0f;
    m_fTouchExFlickDistX = m_fTouchExFlickDistY = 0.0f;
}

//---------------------------
// 長押し割合
//---------------------------
float CTouchExObj::getTouchExLongPressedFrameRate( void ){
    // 長押しフレームが有効であれば
    if( m_fTouchExPressedFrameMax > 0.0f ){
        float rate = m_fTouchExPressedFrame / m_fTouchExPressedFrameMax;
        
        if( rate > 1.0f ){
            rate = 1.0f;
        }else if( rate < 0.0f ){
            rate = 0.0f;
        }
        
        return( rate );
    }
    
    return( 0.0f );
}

//--------------------
// 更新：上書き
//--------------------
void CTouchExObj::update( void ){
    // [CObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preUpdateForObj() ){
        return;
    }
    
    // [CTouchObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preUpdateForTouchObj() ){
        return;
    }

    // [CTouchExObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preUpdateForTouchExObj() ){
        return;
    }
    
    // 実更新
    update0();
}

//--------------------
// タッチ確認
//--------------------
#define _CHECK_EX_TOUCH( _CHECK_TYPE )                                                      \
    if( m_bTouchOval ){                                                                     \
        pCell = CTouchMgr::CheckOval( (_CHECK_TYPE), x, y, m_fTouchAreaW, m_fTouchAreaH );  \
    }else{                                                                                  \
        pCell = CTouchMgr::CheckRect( (_CHECK_TYPE), x, y, m_fTouchAreaW, m_fTouchAreaH );  \
    }
bool CTouchExObj::preUpdateForTouchExObj( void ){
    // 更新毎のリセット
    resetTouchExObjForUpdate();

    // 無効は無視
    if( isBlank() ){
        return( true );
    }

    if( !isTouchAreaValid() || !isTouchActive() ){
        return( true );
    }

    // work
    float x = getX() + m_fTouchOfsX;
    float y = getY() + m_fTouchOfsY;
    CTouchStatusCell* pCell;
    
    //--------------------------
    // セルのIDが有効ならドラッグ検出
    //--------------------------
    if( m_nTouchExCellId >= 0 ){
        // 指が押されたままか？
        pCell = CTouchMgr::GetCellById( m_nTouchExCellId );
        if( pCell != NULL && pCell->isPressed() ){
            m_bTouchExPressed = true;
            m_fTouchExPressedX = pCell->getX() - getX();
            m_fTouchExPressedY = pCell->getY() - getY();
            m_fTouchExDragDistX = pCell->getMoveX();
            m_fTouchExDragDistY = pCell->getMoveY();
            
            if( pCell->isStable() ){
                m_fTouchExPressedFrame += m_fUpdateDeltaFrame;
            }
        }else{
            m_nTouchExCellId = -1;
        }
    }
    //--------------------------
    // セルのIDが無効なら入力開始検出
    //--------------------------
    else{
        //--------------------------
        // 押下確認
        //--------------------------
        _CHECK_EX_TOUCH( eTOUCH_CHECK_TOUCHED_DOWN );

        if( pCell != NULL ){
            m_nTouchExCellId = pCell->getId();

            m_bTouchExPressed = true;
            m_bTouchExPressedDown = true;
            m_fTouchExPressedX = pCell->getX() - getX();
            m_fTouchExPressedY = pCell->getY() - getY();
        }
    }
    
    //--------------------------
    // 長押し確認
    //--------------------------
    if( m_bTouchExPressed && m_fTouchExPressedFrameMax > 0.0f ){
        m_bTouchExLongPressed = (m_fTouchExPressedFrame >= m_fTouchExPressedFrameMax);
    }else{
        m_fTouchExPressedFrame = 0.0f;
    }
    
    //--------------------------
    // タップ確認
    //--------------------------
    _CHECK_EX_TOUCH( eTOUCH_CHECK_TAPPED );
    
    if( pCell != NULL ){
        m_bTouchExTapped = true;
        m_fTouchExTappedX = pCell->getBaseX() - getX();
        m_fTouchExTappedY = pCell->getBaseY() - getY();
    }
    
    //--------------------------
    // フリック確認
    //--------------------------
    _CHECK_EX_TOUCH( eTOUCH_CHECK_FLICKED );
    
    if( pCell != NULL ){
        m_bTouchExFlicked = true;
        m_fTouchExFlickDistX = pCell->getFlickX();
        m_fTouchExFlickDistY = pCell->getFlickY();
    }
    
    return( true );
}
