/*+----------------------------------------------------------------+
  |	Title:		LogicForAssume.cpp [共通環境]
  |	Comment:	ロジック：コアデータ[CLogicGrid]の管理：仮定関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Logic.hpp"

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
//-----------------------
// [仮定] リセット
//-----------------------
void CLogic::resetAssume( void ){
    m_nAssumedTry = 0;
    m_nAssumedLevelMax = 0;
    m_nAssumedDeterminedSolid = 0;
    m_nAssumedImcompatibleSolid = 0;
}

//----------------------
// [仮定] 開始
//----------------------
bool CLogic::assumeOnWithLog( CLogicLog* pLog, bool isBlack ){
    // プレイ中でなければ返す
    if( m_eStatus != eLOGIC_STATUS_PLAYING ){
        LOGE( "@ CLogic::assumeOnWithLog: SKIP: NOT PLAYING\n" );
        return( false );
    }
    
    if( pLog != NULL ){
        // log: 仮定開始の開始
        pLog->addLog_INFO_START( eLLC_INFO_ASSUME_ON );
    }
    
    // 選択マスに対して仮定の開始
    bool result = m_oGrid.setAssumedOnWithLog( pLog, false, m_nSelectColumn, m_nSelectRow, isBlack );
    if( result ){
        m_nAssumedTry++;
        
        if( m_nAssumedLevelMax < m_oGrid.getAssumedLevel() ){
            m_nAssumedLevelMax = m_oGrid.getAssumedLevel();
        }
        
        // 仮定が開始できたら状況更新
        fixStatusWithLog( pLog, false );   // 前方遷移なのでステータスは持ち越す
    }
    
    if( pLog != NULL ){
        // log: 仮定開始の終了
        pLog->addLog_INFO_END( eLLC_INFO_ASSUME_ON );
    }
    
    return( result );
}

//----------------------
// [仮定] 終了
//----------------------
bool CLogic::assumeOffWithLog( CLogicLog* pLog ){
    // 仮定モードでなければ無視
    if( ! m_oGrid.isAssumed() ){
        LOGE( "@ CLogic::assumeOffWithLog: SKIP: NOT ASSUMED\n" );
        return( false );
    }
    
    //-------------------------------------------------------------------------
    // ここは仮定状態での矛盾判定、クリア判定の後に呼ばれる想定なので、ステータスの判定はしない
    //-------------------------------------------------------------------------
    
    if( pLog != NULL ){
        // log: 仮定終了の開始
        pLog->addLog_INFO_START( eLLC_INFO_ASSUME_OFF );
    }
    
    // work
    int solid = 0;
    bool result = false;
    
    // クリアしていたら仮定成立（※ロジックはクリア扱い）
    if( m_eStatus == eLOGIC_STATUS_CLEAR ){
        if( m_oGrid.setAssumedOffForDeterminedWithLog( pLog, &solid ) ){
            m_nAssumedDeterminedSolid += solid;
            m_nSolidDetermined += solid;
            
            result = true;
        }
    }
    // 矛盾していたら仮定不成立（※仮定開始マスを逆の値で確定する）
    else if( m_eStatus == eLOGIC_STATUS_IMCOMPATIBLE ){
        if( m_oGrid.setAssumedOffForImcompatibleWithLog( pLog, &solid  ) ){
            m_nAssumedImcompatibleSolid += solid;
            m_nSolidDetermined += solid;
            
            result = true;
        }
    }
    // 上記以外であれば仮定の断念
    else{
        if( m_oGrid.setAssumedOffForAbortedWithLog( pLog, &solid ) ){
            result = true;
        }
    }
    
    if( result ){
        // 仮定を解除できていたら状況更新
        fixStatusWithLog( pLog, true ); // 後方遷移の可能性があるのでステータスはリセット
    }
    
    if( pLog != NULL ){
        // log: 仮定終了の終了
        pLog->addLog_INFO_END( eLLC_INFO_ASSUME_OFF );
    }
    
    return( result );
}
