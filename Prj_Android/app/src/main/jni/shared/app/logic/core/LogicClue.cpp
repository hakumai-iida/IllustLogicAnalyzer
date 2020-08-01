/*+----------------------------------------------------------------+
  |	Title:		LogicClue.cpp [共通環境]
  |	Comment:	ロジック（コア）：ヒント数字（単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicClue.hpp"

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
CLogicClue::CLogicClue( void ){
    clear();
}

//-----------------------
// デストラクタ
//-----------------------
CLogicClue::~CLogicClue( void ){
}

//-----------------------
// クリア
//-----------------------
void CLogicClue::clear( void ){
    m_nVal = 0;

    reset();
}

//-----------------------
// リセット
//-----------------------
void CLogicClue::reset( void ){
    m_eStatus = eLOGIC_CLUE_STATUS_INVALID;
    m_nAssumedLevel = 0;
    
    m_nSolidStartAt = -1;
    m_nSolidNum = 0;
    
    m_bError = false;
}

//-----------------------
// コピー
//-----------------------
void CLogicClue::copy( CLogicClue* pData ){
    m_eStatus = pData->m_eStatus;
    m_nVal = pData->m_nVal;
    m_nAssumedLevel = pData->m_nAssumedLevel;
    m_nSolidStartAt = pData->m_nSolidStartAt;
    m_nSolidNum = pData->m_nSolidNum;
    m_nCurLogAt = pData->m_nCurLogAt;
    
#if 0
    // とりあえずログはコピーしない（※コピーしたくなったら考える）
    for( int i=0; i<LOGIC_CLUE_LOG_CELL_MAX; i++ ){
        m_oArrLogCell[i] = pData->m_oArrLogCell[i];
    }
#else
    resetLog();
#endif
}

//-----------------------
// 設定
//-----------------------
bool CLogicClue::setStatus( eLOGIC_CLUE_STATUS status, int assumedLevel, bool isForce ){
    // 無効は無視（用心）
    if( status < 0 || status >= eLOGIC_CLUE_STATUS_MAX ){
        LOGE( "@ CLogicClue::setStatus: INVALID PARAM: status=%d\n", status );
        return( false );
    }
    
    // 仮定レベル確認
    if( status == eLOGIC_CLUE_STATUS_ASSUMED ){
        if( assumedLevel <= 0 ){
            LOGE( "@ CLogicClue::setStatus: INVALID ASSUMED LEVEL: status=%d, level=%d\n", status, assumedLevel );
        }
    }else{
        if( assumedLevel > 0 ){
            LOGE( "@ CLogicClue::setStatus: INVALID ASSUMED LEVEL: status=%d, level=%d\n", status, assumedLevel );
        }
    }
    
    // 確定済みは無視（※強制指定がなければ）
    if( m_eStatus != eLOGIC_CLUE_STATUS_INVALID && !isForce ){
        return( false );
    }
    
    // 差分があれば設定（※仮定レベルの差分は見ない＝仮定したマスのレベルは固定）
    if( m_eStatus != status ){
        m_eStatus = status;
        m_nAssumedLevel = assumedLevel;
        
        // 設定が変更されたことの通知
        return( true );
    }

    return( false );
}

//-----------------------
// 状態のリセット
//-----------------------
void CLogicClue::resetStatus( void ){
    m_eStatus = eLOGIC_CLUE_STATUS_INVALID;
    m_nAssumedLevel = 0;
}

//-----------------------
// マスブロック設定
//-----------------------
void CLogicClue::setSolidBlock( int start, int num ){
    m_nSolidStartAt = start;
    m_nSolidNum = num;
}

//-----------------------
// ソリッド開始位置増減
//-----------------------
void CLogicClue::addSolidStartAt( int add ){
    m_nSolidStartAt += add;

    if( m_nSolidStartAt < 0 ){
        m_nSolidStartAt = 0;
    }
}

//-----------------------
// ソリッド数増減
//-----------------------
void CLogicClue::addSolidNum( int add ){
    m_nSolidNum += add;

    if( m_nSolidNum < 0 ){
        m_nSolidNum = 0;
    }
}

//-----------------------
// 整合性確認
//-----------------------
bool CLogicClue::checkVerification( int solidNum ){
    // サイズの確認
    if( m_nSolidNum < m_nVal ){
        return( false );
    }

    // 開始／終了位置の確認
    if( m_nSolidStartAt < 0 || (m_nSolidStartAt+m_nSolidNum-1) >= solidNum ){
        return( false );
    }
    
    // ここまできたら整合性がとれている
    return( true );
}

//-----------------------
// ログのリセット
//-----------------------
void CLogicClue::resetLog( void ){
    m_nCurLogAt = 0;
}

//-------------------------
// ログの追加（※内容は現在の値）
//-------------------------
bool CLogicClue::addLogWithCurVal( int type ){
    if( m_nCurLogAt >= LOGIC_CLUE_LOG_CELL_MAX ){
        LOGE( "@ CLogicClue::addLogWithCurVal: addLogWithCurVal: BUF SHORTAGE\n" );
        return( false );
    }

    // 登録
    m_oArrLogCell[m_nCurLogAt++].set( type, m_nSolidStartAt, m_nSolidNum );
    return( true );
}

//-----------------------------------------------------------------------------------------------
// ログの調整（※ヒントは部分配列で処理されるので分割インデックスで処理した後に、元のインデックスに補正する必要がある）
//-----------------------------------------------------------------------------------------------
void CLogicClue::fixLog( int addForStartAt ){
    for( int i=0; i<m_nCurLogAt; i++ ){
        m_oArrLogCell[i].addSolidStartAt( addForStartAt );
    }
}

//------------------------------
// ログの取得
//------------------------------
CLogicClueLogCell* CLogicClue::getLogCellAt( int at ){
    if( at < 0 || at >= m_nCurLogAt ){
        LOGE( "@ CLogicClue::getLogCellAt: INVALID: at=%d/%d\n", at, m_nCurLogAt );
        return( NULL );
    }
    
    return( &m_oArrLogCell[at] );
}
