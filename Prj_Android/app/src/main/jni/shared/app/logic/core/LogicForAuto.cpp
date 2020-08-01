/*+----------------------------------------------------------------+
  | Title:      LogicForAuto.cpp [共通環境]
  | Comment:    ロジック：コアデータ[CLogicGrid]の管理：オート関連
  | Author:     K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  | Include     インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Logic.hpp"

/*+----------------------------------------------------------------+
  | Define      デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  | Struct      構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  | Global      グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  | Prototype   プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  | Program     プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// [オート] リセット
//------------------------
void CLogic::resetAuto( void ){
    m_bAutoPlay = false;
    m_bAutoForRow = false;
    m_nAutoPointLast = 0;
    m_nAutoPointCur = 0;
    m_nAutoColumnCur = 0;
    m_nAutoRowCur = 0;
}

//----------------------
// [オート] 開始
//----------------------
bool CLogic::setAutoOn( void ){
    // プレイ中でなければ返す
    if( m_eStatus != eLOGIC_STATUS_PLAYING ){
        LOGE( "@ CLogic::setAutoOn: SKIP: NOT PLAYING\n" );
        return( false );
    }
    
    // オートモードでなければ設定
    if( ! m_bAutoPlay ){
        // とりあえず諸々リセット
        m_nAutoPointLast = 0;
        m_nAutoPointCur = 0;
        m_nAutoColumnCur = 0;
        m_nAutoRowCur = 0;
        m_bAutoForRow = false;
        
        // フラグオン
        m_bAutoPlay = true;
        
        return( true );
    }
    
    LOGE( "@ CLogic::setAutoOn: ALREADY AUTO ON\n" );
    return( false );
}

//----------------------
// [オート] 終了
//----------------------
bool CLogic::setAutoOff( void ){
    // オートモードであれば解除
    if( m_bAutoPlay ){
        // フラグオフ
        m_bAutoPlay = false;
        
        return( true );
    }
    
    LOGE( "@ CLogic::setAutoOff: ALREADY AUTO OFF\n" );
    return( false );
}

//----------------------------
// [オート] 実行（※１サイクル）
//----------------------------
bool CLogic::playAutoWithLog( CLogicLog* pLog ){
    // オートプレイ中でなければ返す
    if( ! m_bAutoPlay ){
        return( false );
    }
    
    if( pLog != NULL ){
        // log: オートプレイの開始
        pLog->addLog_INFO_START( eLLC_INFO_AUTO_PLAY );
    }
    
    // オート実体
    bool ret = updateForAutoWithLog( pLog );
    
    // 返値が[true]なら終了
    if( ret ){
        m_bAutoPlay = false;
    }
    
    if( pLog != NULL ){
        // log: オートプレイの終了
        pLog->addLog_INFO_END( eLLC_INFO_AUTO_PLAY );
    }
    
    return( ret );
}

//--------------------------
// [オート] プレイ実体
//--------------------------
bool CLogic::updateForAutoWithLog( CLogicLog* pLog ){
    int row = m_oGrid.getRow();
    int col = m_oGrid.getColumn();
    
    //-----------------------
    // 列の処理
    //-----------------------
    if( m_bAutoForRow ){
        // 表示のために選択項目へも設定
        m_nSelectColumn = -1;
        m_nSelectClueForColumn = -1;
        m_nSelectClueForRow = -1;
        
        bool isDone = false;
        while( m_nAutoRowCur < row && !isDone ){
            m_nSelectRow = m_nAutoRowCur;
            
            // 対象の列が完了していなければ実行
            if( ! m_oGrid.checkLineCompleted( true, m_nSelectRow ) ){
                m_nAutoPointCur += analyzeWithLog( pLog );  // 通常の解析を呼ぶ
                isDone = true;
            }
            
            m_nAutoRowCur++;
        }
        
        // 最後の列に達していたら終了判定
        if( m_nAutoRowCur >= row ){
            // ポイントに差分があれば、再度列へ
            if( m_nAutoPointCur > m_nAutoPointLast ){
                m_bAutoForRow = false;
                m_nAutoColumnCur = 0;
                
                // 現在のポイントを保持しておく（※ギブアップ判定用）
                m_nAutoPointLast = m_nAutoPointCur;
            }
            // ポイントに変更がなければ終了（※行き詰まり）
            else{
                // クリアできていなければ
                if( m_eStatus == eLOGIC_STATUS_PLAYING ){
                    if( pLog != NULL ){
                        // log: 手詰まり
                        pLog->addLog_STATUS_LOGIC_GIVE_UP( m_oGrid.getAssumedLevel() );
                    }
                    
                    // 空欄を探して仮定を一段階進める（※とりあえず左上から）
                    int col = m_oGrid.getColumn();
                    int row = m_oGrid.getRow();
                    for( int i=0; i<col; i++ ){
                        for( int j=0; j<row; j++ ){
                            if( m_oGrid.checkSolidBlank( false, i, j ) ){
                                // TODO:確率の高い方へ（※残りの黒マスが多ければ黒、白マスが多ければ白）
                                
                                // 仮定開始を呼び出す（※とりあえず黒に仮定）
                                m_nSelectColumn = i;
                                m_nSelectRow = j;
                                if( assumeOnWithLog( pLog, m_bAutoSelectBlackForAssume ) ){
                                    // 処理の継続
                                    m_bAutoForRow = false;
                                    m_nAutoColumnCur = 0;
                                    m_nAutoPointLast = m_nAutoPointCur;
                                    return( false );
                                }
                            }
                        }
                    }
                    
                    // ここまできたらギブアップ終了（※実際には来るはずはないと思われる）
                    m_eStatus = eLOGIC_STATUS_GIVE_UP;
                    LOGE( "@ CLogic::updateForAuto: GIVE UP!!\n" );
                    return( true );
                }
            }
        }
    }
    //-----------------------
    // 行の処理
    //-----------------------
    else{
        // 表示のために選択項目へも設定
        m_nSelectRow = -1;
        m_nSelectClueForColumn = -1;
        m_nSelectClueForRow = -1;
        
        bool isDone = false;
        while( m_nAutoColumnCur < col && !isDone ){
            m_nSelectColumn = m_nAutoColumnCur;
            
            // 対象の行が完了していなければ実行
            if( ! m_oGrid.checkLineCompleted( false, m_nAutoColumnCur ) ){
                m_nAutoPointCur += analyzeWithLog( pLog );  // 通常の解析を呼ぶ
                isDone = true;
            }
            
            m_nAutoColumnCur++;
        }
        
        // 終了してたら列へ
        if( m_nAutoColumnCur >= col ){
            m_bAutoForRow = true;
            m_nAutoRowCur = 0;
        }
    }
    
    //-----------------------------------
    // クリアしていたら
    //-----------------------------------
    if( m_eStatus == eLOGIC_STATUS_CLEAR ){
        // 仮定中なら解除（※ここに来る場合は全マスが矛盾なく埋まっているので解除＝確定となる）
        if( m_oGrid.isAssumed() ){
            assumeOffWithLog( pLog );
        }
        
        // ここまできたらクリア終了
        return( true );
    }
    
    //-----------------------------------
    // 矛盾していたら
    //-----------------------------------
    if( m_eStatus == eLOGIC_STATUS_IMCOMPATIBLE ){
        // 仮定中であれば解除（※仮定レベルを１つ戻す）
        if( m_oGrid.isAssumed() ){
            if( assumeOffWithLog( pLog ) ){
                // 解除に成功したら処理の継続
                return( false );
            }
        }
        
        // ここまできたら矛盾終了
        return( true );
    }
    
    // ここまできたらオート処理の継続
    return( false );
}
