/*+----------------------------------------------------------------+
  |	Title:		Obj.cpp [共通環境]
  |	Comment:	ゲーム要素基底（※更新と描画を行う要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Obj.hpp"

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
float CObj::s_fQuakeX;
float CObj::s_fQuakeY;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------
// コンストラクタ
//--------------------
CObj::CObj( void ): CUpdater(), CDrawer(){
	clearObj();
}

//--------------------
// クリア
//--------------------
void CObj::clearObj( void ){
	m_pParent = NULL;
	m_fX = m_fY = 0.0f;
    m_fW = m_fH = 0.0f;
    m_bBlank = false;
	m_nStatus = m_nStatus0 = INVALID_VAL;

	m_bInfoValid = false;
	m_nInfoId = 0;

    m_bQuakeIgnore = false;

	resetExitTemporary();
    
	// クリア直後はブランク
	setBlank( true );
}

//-------------------------------------
// 座標取得（※親がいる場合は同期する）
//-------------------------------------
// Ｘ座標
float CObj::getX( void ){
	if( m_pParent != NULL ){
		return( m_pParent->getX() + m_fX );
	}

	return( m_fX );
}

// Ｙ座標
float CObj::getY( void ){
	if( m_pParent != NULL ){
		return( m_pParent->getY() + m_fY );
	}

	return( m_fY );
}

//-------------------
// 状態設定
//-------------------
bool CObj::setStatus( int status ){
	// 状態が変化する場合は[BLANK]解除
	bool isChanged = !checkStatus( status );
	if( isChanged ){
		setBlank( false );
	}

	// 待ち時間はクリア
	resetUpdateWait();

	m_nStatus = status;
	return( isChanged );
}

//-------------------
// 状態設定（強制）
//-------------------
bool CObj::setStatusForce( int status ){
    m_nStatus0 = INVALID_VAL;
    return( setStatus( status ) );
}

//-------------------
// 状態確認
//-------------------
bool CObj::checkStatus( int status ){
	return( m_nStatus == status && m_nStatus == m_nStatus0 );
}

//-----------------
// 一時避難リセット
//-----------------
void CObj::resetExitTemporary( void ){
	m_bExitTemporary = false;
	m_fCountForExitTemporary = 0;
	m_fCountMaxForExitTemporary = 0;
	m_fXForExitTemporary = 0;
	m_fYForExitTemporary = 0;
}

//---------------
// 一時避難
//---------------
void CObj::setExitTemporary( float count, float x, float y ){
	m_bExitTemporary = true;
	m_fCountForExitTemporary = 0;
	m_fCountMaxForExitTemporary = count;
	m_fXForExitTemporary = x;
	m_fYForExitTemporary = y;
}

//-----------------
// 一時避難から復帰
//-----------------
void CObj::recoverExitTemporary( void ){
	m_bExitTemporary = false;
}

//--------------------
// 更新：上書き
//--------------------
void CObj::update( void ){
    // [CObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preUpdateForObj() ){
        return;
    }

	// 実更新
	update0();
}

//------------------------
// [CObj]に関する事前更新
//------------------------
bool CObj::preUpdateForObj( void ){
    // 情報フラグは都度クリア
    m_bInfoValid = false;
    
    // ブランクであれば無視
    if( isBlank() ){
        return( false );
    }
    
    // フレーム更新
    updateFrameForUpdate();
    
    // 一時退避が有効であれば無視
    if( m_fCountMaxForExitTemporary > 0 ){
        // フラグが寝ていたら復帰
        if( ! m_bExitTemporary ){
            if( CConst::IncrementToMax( &m_fCountForExitTemporary, m_fCountMaxForExitTemporary, m_fUpdateDeltaFrame ) ){
                resetExitTemporary();
            }
        }
        
        return( false );
    }
    
    // 待ち時間中なら無視
    if( updateWaitCountForUpdate() ){
        return( false );
    }

    // ここまできたら更新可能
    return( true );
}

//--------------------
// 描画：上書き
//--------------------
void CObj::onDraw( void ){
    // [CObj] 事前処理（※[false]が返ったらスキップ）
    if( ! preDrawForObj() ){
        return;
    }

	// 実描画
	onDraw0();

	// [CObj] 事後処理
    postDrawForObj();
}

//----------------------
// [CObj]に関する事前描画
//----------------------
bool CObj::preDrawForObj( void ){
    // ブランクであれば無視
    if( isBlank() ){
        return( false );
    }
    
    // 描画影響値
    BYTE a = 0xFF;
    float ofsX = 0.0f;
    float ofsY = 0.0f;
    
    // 地震影響
    if( ! m_bQuakeIgnore ){
        ofsX += s_fQuakeX;
        ofsY += s_fQuakeY;
    }
    
    // 一時退避中であれば
    if( m_fCountMaxForExitTemporary > 0 ){
        // 復帰していなければスキップ
        if( m_bExitTemporary ){
            return( false );
        }
        
        // 描画調整
        float rate = m_fCountForExitTemporary/m_fCountMaxForExitTemporary;
        rate = CConst::ApplyAsyncType( eASYNC_TYPE_DOWN, rate, 0.5f );
        
        a = (BYTE)(0xFF*rate);
        ofsX += m_fXForExitTemporary * (1.0f-rate);
        ofsY += m_fYForExitTemporary * (1.0f-rate);
    }

    // 描画調整適用
    CDrawCell::SetOfsXY( ofsX, ofsY );
    CDrawCell::SetAlpha( a );

    // ここまできたら表示可能
    return( true );
}

//----------------------
// [CObj]に関する事後描画
//----------------------
void CObj::postDrawForObj( void ){
    // 描画調整リセット
    CDrawCell::ResetOfsXY();
    CDrawCell::ResetAlpha();
}
