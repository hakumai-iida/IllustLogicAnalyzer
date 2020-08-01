/*+----------------------------------------------------------------+
  |	Title:		LogicSolid.cpp [共通環境]
  |	Comment:	ロジック（コア）：マス（単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicSolid.hpp"

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
CLogicSolid::CLogicSolid( void ){
    clear();
}

//-----------------------
// デストラクタ
//-----------------------
CLogicSolid::~CLogicSolid( void ){   
}

//-----------------------
// クリア
//-----------------------
void CLogicSolid::clear( void ){
    reset();
}

//-----------------------
// リセット
//-----------------------
void CLogicSolid::reset( void ){
    m_eStatus = eLOGIC_SOLID_STATUS_INVALID;
    m_eType = eLOGIC_SOLID_TYPE_INVALID;
    m_nAssumedLevel = 0;
}

//-----------------------
// コピー
//-----------------------
void CLogicSolid::copy( CLogicSolid* pData ){
    m_eStatus = pData->m_eStatus;
    m_eType = pData->m_eType;
    m_nAssumedLevel = pData->m_nAssumedLevel;
}

//-----------------------
// 設定
//-----------------------
bool CLogicSolid::set( eLOGIC_SOLID_STATUS status, eLOGIC_SOLID_TYPE type, int assumedLevel, bool isForce ){
    // 無効は無視（用心）
    if( status < 0 || status >= eLOGIC_SOLID_STATUS_MAX || type < 0 || type >= eLOGIC_SOLID_TYPE_MAX ){
        LOGE( "@ CLogicSolid::set: INVALID PARAM: status=%d, type=%d, level=%d\n", status, type, assumedLevel );
        return( false );
    }
    
    // 仮定レベル確認
    if( status == eLOGIC_SOLID_STATUS_ASSUMED || status == eLOGIC_SOLID_STATUS_ASSUMED_START ){
        if( assumedLevel <= 0 ){
            LOGE( "@ CLogicSolid::set: INVALID ASSUMED LEVEL: status=%d, type=%d, level=%d\n", status, type, assumedLevel );
        }
    }else{
        if( assumedLevel > 0 ){
            LOGE( "@ CLogicSolid::set: INVALID ASSUMED LEVEL: status=%d, type=%d, level=%d\n", status, type, assumedLevel );
        }
    }
    
    // 確定済みは無視（※強制指定がなければ）
    if( m_eStatus != eLOGIC_SOLID_STATUS_INVALID && !isForce ){
        return( false );
    }
    
    // 差分があれば設定（※仮定レベルの差分は見ない＝仮定したマスのレベルは固定）
    if( m_eStatus != status || m_eType != type ){
        m_eStatus = status;
        m_eType = type;
        m_nAssumedLevel = assumedLevel;
        
        // 設定が変更されたことの通知
        return( true );
    }

    return( false );
}

//---------------------------------------
// 開発用スワップ
//---------------------------------------
bool CLogicSolid::swapForDev( eLOGIC_SOLID_STATUS status, int assumedLevel ){
    // 次のタイプへ
    eLOGIC_SOLID_TYPE type = (eLOGIC_SOLID_TYPE)(m_eType + 1);
    
    // 無効になったらリセット
    if( type < 0 || type >= eLOGIC_SOLID_TYPE_MAX ){
        reset();
    }else{
        m_eStatus = status;
        m_eType = type;
        m_nAssumedLevel = assumedLevel;
    }
    
    return( true );
}

//---------------------------------------
// 調整：仮定成功（※ロジックがクリアされた）
//---------------------------------------
bool CLogicSolid::fixForAssumedDetermined( int assumedLevel ){
    // 仮定状態であれば
    if( isAssumed() ){
        // 自身の仮定レベルが指定よりも大きければば確定
        if( m_nAssumedLevel > assumedLevel ){
            // 仮定レベルが有効の場合は仮定状態へ
            if( assumedLevel > 0 ){
                m_eStatus = eLOGIC_SOLID_STATUS_ASSUMED;
            }
            // 仮定レベルが無効の場合は確定へ
            else{
                m_eStatus = eLOGIC_SOLID_STATUS_DETERMINED;
            }

            // 仮定レベルは指定の値へ
            m_nAssumedLevel = assumedLevel;
            
            return( true );
        }
    }
    
    return( false );
}

//---------------------------------------
// 調整：仮定失敗（※ロジックが矛盾した）
//---------------------------------------
bool CLogicSolid::fixForAssumedImcpompatible( int assumedLevel ){
    // 仮定レベルが指定よりも大きければ
    if( m_nAssumedLevel > assumedLevel ){
        // 仮定開始状態であれば反転して確定
        if( m_eStatus == eLOGIC_SOLID_STATUS_ASSUMED_START ){
            if( m_eType == eLOGIC_SOLID_TYPE_BLACK){
                m_eType = eLOGIC_SOLID_TYPE_WHITE;
            }else{
                m_eType = eLOGIC_SOLID_TYPE_BLACK;
            }

            // 仮定レベルが有効の場合は仮定状態へ
            if( assumedLevel > 0 ){
                m_eStatus = eLOGIC_SOLID_STATUS_ASSUMED;
            }
            // 仮定レベルが無効の場合は確定へ
            else{
                m_eStatus = eLOGIC_SOLID_STATUS_DETERMINED;
            }

            // 仮定レベルは指定の値へ
            m_nAssumedLevel = assumedLevel;

            return( true );
        }
        
        // その他の仮定状態であればリセット（※空きマスに）
        if( isAssumed() ){
            reset();
            return( true );
        }
    }

    return( false );
}

//---------------------------------------
// 調整：仮定中断
//---------------------------------------
bool CLogicSolid::fixForAssumedAborted( int assumedLevel ){
    // 仮定状態であれば
    if( isAssumed() ){
        // 仮定レベルが指定よりも大きければリセット
        if( m_nAssumedLevel > assumedLevel ){
            reset();
            return( true );
        }
    }
    
    return( false );
}
