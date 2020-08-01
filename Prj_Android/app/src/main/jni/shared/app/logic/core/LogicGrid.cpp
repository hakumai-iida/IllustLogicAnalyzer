/*+----------------------------------------------------------------+
  |	Title:		LogicGrid.cpp [共通環境]
  |	Comment:	ロジック（コア）：グリッド（盤面全体）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicGrid.hpp"
#include "LogicClueUpdater.hpp"

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
CLogicGrid::CLogicGrid( void ){
    clear();
}

//-----------------------
// デストラクタ
//-----------------------
CLogicGrid::~CLogicGrid( void ){   
}

//-----------------------
// クリア
//-----------------------
void CLogicGrid::clear( void ){
    m_nColumn = 0;
    m_nRow = 0;
    
    // マス
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        for( int j=0; j<LOGIC_SOLID_WH_MAX; j++ ){
            m_oArrArrSolid[i][j].clear();
        }
    }
    
    // ヒント：ヨコ
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        m_nArrNumClueForColumn[i] = 0;
        for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
            m_oArrArrClueForColumn[i][j].clear();
        }
    }

    // ヒント：タテ
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        m_nArrNumClueForRow[i] = 0;
        for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
            m_oArrArrClueForRow[i][j].clear();
        }
    }

    reset();
}

//-----------------------
// リセット
//-----------------------
void CLogicGrid::reset( void ){
    // マス
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nRow; j++ ){
            m_oArrArrSolid[i][j].reset();
        }
    }
    
    // ヒント：ヨコ
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nArrNumClueForColumn[i]; j++ ){
            m_oArrArrClueForColumn[i][j].reset();
        }
    }

    // ヒント：タテ
    for( int i=0; i<m_nRow; i++ ){
        for( int j=0; j<m_nArrNumClueForRow[i]; j++ ){
            m_oArrArrClueForRow[i][j].reset();
        }
    }
    
    // 仮定レベル
    m_nAssumedLevel = 0;
    
    //--------------------------------------------------------
    // 確定検出ログ（※−２で矛盾、−１で未確定、０で確定、１以上で仮定確定）
    //--------------------------------------------------------
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
            m_nArrClueDeterminedForRow[i][j] = -1;
            m_nArrClueDeterminedForCol[i][j] = -1;
        }
        m_nArrLineDeterminedForRow[i] = -1;
        m_nArrLineDeterminedForCol[i] = -1;
    }
    
    // ヒントの参照ブロック更新指定
    updateClueSolidInfoForAll();

    // ヒント更新スコアのリセット（※リセットは[CLogicGrid]管理下＝ヒントの更新指定と足並みを揃えておく）
    CLogicClueUpdater::ResetFixScore();
}

//-------------
// コピー
//-------------
void CLogicGrid::copy( CLogicGrid* pData ){
    m_nColumn = pData->m_nColumn;
    m_nRow = pData->m_nRow;
    
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        for( int j=0; j<LOGIC_SOLID_WH_MAX; j++ ){
            m_oArrArrSolid[i][j].copy( &pData->m_oArrArrSolid[i][j] );
        }
    }

    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        m_nArrNumClueForColumn[i] = pData->m_nArrNumClueForColumn[i];
        for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
            m_oArrArrClueForColumn[i][j].copy( &pData->m_oArrArrClueForColumn[i][j] );
        }
    }

    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        m_nArrNumClueForRow[i] = pData->m_nArrNumClueForRow[i];
        for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
            m_oArrArrClueForRow[i][j].copy( &(pData->m_oArrArrClueForRow[i][j]) );
        }
    }
    
    m_nAssumedLevel = pData->m_nAssumedLevel;

    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++  ){
        m_bArrUpdateClueSolidInfoForColumn[i] = pData->m_bArrUpdateClueSolidInfoForColumn[i];
        m_bArrUpdateClueSolidInfoForRow[i] = pData->m_bArrUpdateClueSolidInfoForRow[i];
    }
    
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
            m_nArrClueDeterminedForRow[i][j] = pData->m_nArrClueDeterminedForRow[i][j];
            m_nArrClueDeterminedForCol[i][j] = pData->m_nArrClueDeterminedForCol[i][j];
        }
        m_nArrLineDeterminedForRow[i] = pData->m_nArrLineDeterminedForRow[i];
        m_nArrLineDeterminedForCol[i] = pData->m_nArrLineDeterminedForCol[i];
    }

}

//-------------------------------------------
// マスの数の取得（※その行／列に含まれるマス数の取得）
//-------------------------------------------
int CLogicGrid::getSolidNum( bool isRow ){
    if( isRow ){
        // 列のマス数なので行数
        return( m_nColumn );
    }
    
    // 行のマス数なので列数
    return( m_nRow );
}

//----------------------------------------
// マスの取得（※直接指定）
//----------------------------------------
CLogicSolid* CLogicGrid::getSolid( int column, int row ){
    // 無効は無視
    if( column < 0 || column >= m_nColumn || row < 0 || row >= m_nRow ){
        LOGE( "@ CLogicGrid::getSolid: INVALID PARAM: column=%d/%d, row=%d/%d\n", column, m_nColumn, row, m_nRow );
        return( NULL );
    }
    
    return( &m_oArrArrSolid[column][row] );
}

//----------------------------------------
// マスの取得（※汎用指定）
//----------------------------------------
CLogicSolid* CLogicGrid::getSolid( bool isRow, int targetIndex, int solidAt ){
    if( isRow ){
        // 無効は無視
        if( targetIndex < 0 || targetIndex >= m_nRow || solidAt < 0 || solidAt >= m_nColumn ){
            LOGE( "@ CLogicGrid::getSolid: INVALID ROW TARGET: index=%d/%d, at=%d/%d\n",
                  targetIndex, m_nRow, solidAt, m_nColumn );
            return( NULL );
        }
        
        return( &m_oArrArrSolid[solidAt][targetIndex] );
    }else{
        // 無効は無視
        if( targetIndex < 0 || targetIndex >= m_nColumn || solidAt < 0 || solidAt >= m_nRow ){
            LOGE( "@ CLogicGrid::getSolid: INVALID COLUMN TARGET: index=%d/%d\n, at=%d/%d",
                  targetIndex, m_nColumn, solidAt, m_nRow );
            return( NULL );
        }
        
        return( &m_oArrArrSolid[targetIndex][solidAt] );
    }
    
    return( NULL );
}

//----------------------------------------
// マスが空欄か？
//----------------------------------------
bool CLogicGrid::checkSolidBlank( bool isRow, int targetIndex, int solidAt, int solidOfs ){
    CLogicSolid* pSolid = getSolid( isRow, targetIndex, solidAt+solidOfs );
    return( pSolid != NULL && pSolid->isBlank() );
}

//----------------------------------------
// マスが黒か？
//----------------------------------------
bool CLogicGrid::checkSolidBlack( bool isRow, int targetIndex, int solidAt, int solidOfs ){
    CLogicSolid* pSolid = getSolid( isRow, targetIndex, solidAt+solidOfs );
    return( pSolid != NULL && pSolid->isBlack() );
}

//----------------------------------------
// マスが白か？
//----------------------------------------
bool CLogicGrid::checkSolidWhite( bool isRow, int targetIndex, int solidAt, int solidOfs ){
    CLogicSolid* pSolid = getSolid( isRow, targetIndex, solidAt+solidOfs );
    return( pSolid != NULL && pSolid->isWhite() );
}

//----------------------------------------
// マスの設定（※内部でヒントの更新が予約される）
//----------------------------------------
bool CLogicGrid::setSolid( bool isRow, int targetIndex, int solidAt, eLOGIC_SOLID_STATUS status, eLOGIC_SOLID_TYPE type,
                           int assumedLevel, bool isForce ){
    bool isChanged = false;
    
    if( isRow ){
        // 無効は無視
        if( targetIndex < 0 || targetIndex >= m_nRow || solidAt < 0 || solidAt >= m_nColumn ){
            LOGE( "@ CLogicGrid::setSolid: INVALID ROW TARGET: index=%d/%d, at=%d%d\n", targetIndex, m_nRow, solidAt, m_nColumn );
            return( false );
        }

        isChanged = m_oArrArrSolid[solidAt][targetIndex].set( status, type, assumedLevel, isForce );
    }else{
        // 無効は無視
        if( targetIndex < 0 || targetIndex >= m_nColumn || solidAt < 0 || solidAt >= m_nRow ){
            LOGE( "@ CLogicGrid::setSolid: INVALID COLUMN TARGET: index=%d/%d, at=%d/%d\n", targetIndex, m_nColumn, solidAt, m_nRow );
            return( false );
        }
      
        isChanged = m_oArrArrSolid[targetIndex][solidAt].set( status, type, assumedLevel, isForce );
    }
    
    if( isChanged ){
        // マスに変化があったのでキャッシュキーの更新
        updateClueSolidInfoForColumnAt( (isRow)? solidAt: targetIndex );
        updateClueSolidInfoForRowAt( (isRow)? targetIndex: solidAt );
    }
    
    return( isChanged );
}

//---------------------------------------------------
// マスの確定：空欄（※内部でヒントの更新が予約される）
//---------------------------------------------------
bool CLogicGrid::setSolidBlank( bool isRow, int targetIndex, int solidAt ){
    CLogicSolid* pSolid = getSolid( isRow, targetIndex, solidAt );
    
    // 空欄でなければリセットする（※空欄になる）
    if( pSolid != NULL && ! pSolid->isBlank() ){
        pSolid->reset();
        
        // マスに変化があったのでキャッシュキーの更新
        updateClueSolidInfoForColumnAt( (isRow)? solidAt: targetIndex );
        updateClueSolidInfoForRowAt( (isRow)? targetIndex: solidAt );
    }
    
    return( false );
}

//---------------------------------------------------
// マスの確定：黒（※内部でヒントの更新が予約される）
//---------------------------------------------------
bool CLogicGrid::setSolidBlack( bool isRow, int targetIndex, int solidAt, bool isForce ){
    if( isAssumed() ){
        return( setSolid( isRow, targetIndex, solidAt, eLOGIC_SOLID_STATUS_ASSUMED, eLOGIC_SOLID_TYPE_BLACK, m_nAssumedLevel, isForce ) );
    }
    
    return( setSolid( isRow, targetIndex, solidAt, eLOGIC_SOLID_STATUS_DETERMINED, eLOGIC_SOLID_TYPE_BLACK, 0, isForce ) );
}

//---------------------------------------------------
// マスの確定：白（※内部でヒントの更新が予約される）
//---------------------------------------------------
bool CLogicGrid::setSolidWhite( bool isRow, int targetIndex, int solidAt, bool isForce ){
    if( isAssumed() ){
        return( setSolid( isRow, targetIndex, solidAt, eLOGIC_SOLID_STATUS_ASSUMED, eLOGIC_SOLID_TYPE_WHITE, m_nAssumedLevel, isForce ) );
    }
    
    return( setSolid( isRow, targetIndex, solidAt, eLOGIC_SOLID_STATUS_DETERMINED, eLOGIC_SOLID_TYPE_WHITE, 0, isForce ) );
}

//---------------------------------------------------
// 開発用のマススワップ（※内部でヒントの更新が予約される）
//---------------------------------------------------
bool CLogicGrid::swapSolidForDev( int column, int row ){
    CLogicSolid* pSolid = getSolid( column, row );
    
    if( pSolid != NULL ){
        eLOGIC_SOLID_STATUS status = ((isAssumed())? eLOGIC_SOLID_STATUS_ASSUMED: eLOGIC_SOLID_STATUS_DETERMINED);
        
        if( pSolid->swapForDev( status, m_nAssumedLevel ) ){
            // マスに変化があったのでキャッシュキーの更新
            updateClueSolidInfoForColumnAt( column );
            updateClueSolidInfoForRowAt( row );

            return( true );
        }
    }
    
    return( false );
}

//------------------------------
// 指定のラインが完成しているか？
//------------------------------
bool CLogicGrid::checkLineCompleted( bool isRow, int targetIndex ){
    int solidNum = getSolidNum( isRow );

    // 空マスがあれば完成ではない
    for( int i=0; i<solidNum; i++ ){
        if( checkSolidBlank( isRow, targetIndex, i ) ){
            return( false );
        }
    }
    
    // ここまできたら完成
    return( true );
}

//--------------------------------------------------------------------------------
// 指定のマスが一意となるヒントがある場合にそのインデックスを返す（※一意ではない場合は負の値が返る）
//--------------------------------------------------------------------------------
int CLogicGrid::checkUniqueBlockIndexWithSolidAt( bool isRow, int targetIndex, int solidAt ){
    int sizeClue;
    CLogicClue* arrClue;
    if( ! getClueArray( isRow, targetIndex, &arrClue, &sizeClue ) ){
        return( -1 );
    }
    
    // 指定マスが１つのブロックだけ含まれるか確認
    int blockIndex = -1;
    CLogicClue* pClue;
    for( int i=0; i<sizeClue; i++ ){
        if( (pClue=getUpdatedClue( isRow, targetIndex, i )) != NULL ){
            // ブロックに含まれていたら
            if( pClue->isSolidContained( solidAt ) ){
                // すでに別のブロックに含まれていたら一意ではない
                if( blockIndex >= 0 ){
                    return( -1 );
                }
                
                // IDの更新
                blockIndex = i;
            }
        }
        // 設定ミス（矛盾）は抜ける（※続ける意味がない）
        else{
            return( -1 );
        }
    }
    
    // 結果を返す（※１回だけ設定されている場合のみ[blockIndex]は有意な値となる）
    return( blockIndex );
}

//----------------------------------------------------
// 指定のマスが孤立（※どのブロックにも含まれない）しているか？
//----------------------------------------------------
bool CLogicGrid::checkSolidIsolatedAt( bool isRow, int targetIndex, int solidAt ){
    int sizeClue;
    CLogicClue* arrClue;
    if( ! getClueArray( isRow, targetIndex, &arrClue, &sizeClue ) ){
        return( false );
    }
    
    // 全ブロック（ヒント数字）に対して含まれるかの確認
    CLogicClue* pClue;
    for( int i=0; i<sizeClue; i++ ){
        if( (pClue=getUpdatedClue( isRow, targetIndex, i )) != NULL ){
            // 含まれていたら終了（※孤立していない）
            if( pClue->isSolidContained( solidAt ) ){
                return( false );
            }
        }
        // 設定ミス（矛盾）は抜ける（※続ける意味がない）
        else{
            return( false );
        }
    }
    
    // ここまできたら孤立している（※どのヒントにも含まれていない）
    return( true );
}

//-------------------------------
// 指定のラインが矛盾しているか？
//-------------------------------
bool CLogicGrid::checkImcompatibleAt( bool isRow, int targetIndex ){
    int sizeClue;
    CLogicClue* arrClue;
    if( ! getClueArray( isRow, targetIndex, &arrClue, &sizeClue ) ){
        return( false );
    }

    // 確定ヒントの取得に失敗した場合はそのラインが矛盾を含む
    //（※[0]だけで判定の手抜きはＮＧ＝キャッシュが効いてしまう可能性があるので全判定すること）
    for( int i=0; i<sizeClue; i++ ){
        if( getUpdatedClue( isRow, targetIndex, i ) == NULL ){
            return( true );
        }
    }
    
    return( false );
}

//------------------------------------------
// クリア判定（※全てのヒントとマスが確定したらクリア）
//------------------------------------------
bool CLogicGrid::checkClear( void ){
    // ヨコの確認
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nArrNumClueForColumn[i]; j++ ){
            if( ! m_oArrArrClueForColumn[i][j].isDeterminedOrAssumed() ){
                return( false );
            }
        }
    }

    // タテの確認
    for( int i=0; i<m_nRow; i++ ){
        for( int j=0; j<m_nArrNumClueForRow[i]; j++ ){
            if( ! m_oArrArrClueForRow[i][j].isDeterminedOrAssumed() ){
                return( false );
            }
        }
    }
    
    // 用心（※マス目がすべて埋まっているか？
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nRow; j++ ){
            if( m_oArrArrSolid[i][j].isBlank() ){
                return( false );
            }
        }
    }
    
    // ここまできたらクリア
    return( true );
}

//-----------------------------------------
// 矛盾判定（※１つでも矛盾したヒントがあれば矛盾）
//-----------------------------------------
bool CLogicGrid::checkImcompatible( void ){
    // ヨコの確認
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nArrNumClueForColumn[i]; j++ ){
            if( m_oArrArrClueForColumn[i][j].isImcompatible() ){
                return( true );
            }
        }
    }

    // タテの確認
    for( int i=0; i<m_nRow; i++ ){
        for( int j=0; j<m_nArrNumClueForRow[i]; j++ ){
            if( m_oArrArrClueForRow[i][j].isImcompatible() ){
                return( true );
            }
        }
    }
    
    return( false );
}

//--------------------------------------------
// 仮定設定：オン（※内部でヒントの更新が予約される）
//--------------------------------------------
bool CLogicGrid::setAssumedOnWithLog( CLogicLog* pLog, bool isRow, int targetIndex, int solidAt, bool isBlack ){
    // 対象のマスを仮定開始状態へ
    CLogicSolid* pSolid = getSolid( isRow, targetIndex, solidAt );

    // 仮定開始マスに設定できたら成功
    if( pSolid->set( eLOGIC_SOLID_STATUS_ASSUMED_START, ((isBlack)? eLOGIC_SOLID_TYPE_BLACK: eLOGIC_SOLID_TYPE_WHITE), m_nAssumedLevel+1 ) ){
        // 仮定レベルを進める
        m_nAssumedLevel++;

        // マスに変化があったのでキャッシュキーの更新
        updateClueSolidInfoForColumnAt( (isRow)? solidAt: targetIndex );
        updateClueSolidInfoForRowAt( (isRow)? targetIndex: solidAt );

        if( pLog != NULL ){
            int col = (isRow)? solidAt: targetIndex;
            int row = (isRow)? targetIndex: solidAt;

            // log: 仮定開始
            pLog->addLog_ASSUME_START_INFO( col, row, m_nAssumedLevel, ((isBlack)? eLOGIC_SOLID_TYPE_BLACK: eLOGIC_SOLID_TYPE_WHITE) );
        }
        
        return( true );
    }

    // ここまできたらダメ
    LOGE( "@ CLogicGrid:setAssumedOn: ASSUMED ON: FAILED\n" );
    return( false );
}

//----------------------------------------------------
// 仮定設定：オフ（確定）（※内部でヒントの更新が予約される）
//----------------------------------------------------
bool CLogicGrid::setAssumedOffForDeterminedWithLog( CLogicLog* pLog, int* pScore ){
    if( pScore != NULL ){
        *pScore = 0;
    }

    // 仮定モードでなければ無視
    if( ! isAssumed() ){
        LOGE( "@ CLogicGrid:setAssumedOffForDetermined: NOT ASSUMED MODE YET\n" );
        return( false );
    }

    // クリアしているので仮定レベルを０へ
    m_nAssumedLevel = 0;

    int score = 0;
    
    // レベルを戻した後に各マスを更新
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nRow; j++ ){
            if( m_oArrArrSolid[i][j].fixForAssumedDetermined( m_nAssumedLevel ) ){
                score++;

                // マスに変化があったのでキャッシュキーの更新
                updateClueSolidInfoForColumnAt( i );
                updateClueSolidInfoForRowAt( j );
                
                if( pLog != NULL ){
                    // log: 仮定確定
                    pLog->addLog_ASSUME_DETERMINED( i, j, m_nAssumedLevel, m_oArrArrSolid[i][j].getType() );
                }
            }
        }
    }

    // 仮定から抜ける際にはスコアを返す
    if( ! isAssumed() ){
        if( pScore != NULL ){
            *pScore = score;
        }
    }
    
    return( true );
}

//----------------------------------------------------
// 仮定設定：オフ（矛盾）（※内部でヒントの更新が予約される）
//----------------------------------------------------
bool CLogicGrid::setAssumedOffForImcompatibleWithLog( CLogicLog* pLog, int* pScore ){
    if( pScore != NULL ){
        *pScore = 0;
    }

    // 仮定モードでなければ無視
    if( ! isAssumed() ){
        LOGE( "@ CLogicGrid:setAssumedOffForImcompatible: NOT ASSUMED MODE YET\n" );
        return( false );
    }

    // 仮定のレベルを戻す
    m_nAssumedLevel--;

    // レベルを戻した後に各マスを更新
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nRow; j++ ){
            eLOGIC_SOLID_TYPE solid0 = m_oArrArrSolid[i][j].getType();
            if( m_oArrArrSolid[i][j].fixForAssumedImcpompatible( m_nAssumedLevel ) ){
                // マスに変化があったのでキャッシュキーの更新
                updateClueSolidInfoForColumnAt( i );
                updateClueSolidInfoForRowAt( j );

                // 仮定を解除されたマスのログ（※仮確定状態のリセット）
                if( pLog != NULL ){
                    // log: 仮定解除
                    pLog->addLog_ASSUME_OFF( i, j, m_nAssumedLevel+1, solid0 );

                    // 確定したマスであればさらにログ（※仮定開始マスは[fixForAssumedImcompatible]内で反転確定される）
                    eLOGIC_SOLID_TYPE solid = m_oArrArrSolid[i][j].getType();
                    if( solid != eLOGIC_SOLID_TYPE_INVALID ){
                        // log: 仮定矛盾
                        pLog->addLog_ASSUME_IMCOMPATIBLE( i, j, m_nAssumedLevel+1, solid0 );
                        
                        // log: 仮定確定
                        pLog->addLog_ASSUME_DETERMINED( i, j, m_nAssumedLevel, solid );
                    }
                }
            }
        }
    }
    
    // 仮定から抜ける際にはスコアを返す（※とりあえず１点固定）
    if( ! isAssumed() ){
        if( pScore != NULL ){
            *pScore = 1;
        }
    }
    
    return( true );
}

//-----------------------------------------------------
// 仮定設定：オフ（中断）（※内部でヒントの更新が予約される）
//-----------------------------------------------------
bool CLogicGrid::setAssumedOffForAbortedWithLog( CLogicLog* pLog, int* pScore ){
    if( pScore != NULL ){
        *pScore = 0;
    }
    
    // 仮定モードでなければ無視
    if( ! isAssumed() ){
        LOGE( "@ CLogicGrid:setAssumedOffForAborted: NOT ASSUMED MODE YET\n" );
        return( false );
    }
    
    // 仮定のレベルを戻す
    m_nAssumedLevel--;

    // レベルを戻した後に各マスを更新
    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nRow; j++ ){
            eLOGIC_SOLID_TYPE solid0 = m_oArrArrSolid[i][j].getType();
            if( m_oArrArrSolid[i][j].fixForAssumedAborted( m_nAssumedLevel ) ){
                // マスに変化があったのでキャッシュキーの更新
                updateClueSolidInfoForColumnAt( i );
                updateClueSolidInfoForRowAt( j );

                if( pLog != NULL ){
                    // log: 仮定解除
                    pLog->addLog_ASSUME_OFF( i, j, m_nAssumedLevel+1, solid0 );
                }
            }
        }
    }
    
    // 中断の際はこれといってスコアはなし

    return( true );
}

//----------------------------
// ヒントの追加
//----------------------------
bool CLogicGrid::addClueAt( bool isRow, int targetIndex ){
    if( isRow ){
        if( targetIndex >= 0 && targetIndex < m_nRow ){
            int max = (m_nRow+1)/2;
            if( m_nArrNumClueForRow[targetIndex] < max ){
                 m_oArrArrClueForRow[targetIndex][m_nArrNumClueForRow[targetIndex]].setVal( 0 );
                 m_nArrNumClueForRow[targetIndex]++;
                return( true );
            }
        }
    }else{
        if( targetIndex >= 0 && targetIndex < m_nColumn ){
            int max = (m_nColumn+1)/2;
            if( m_nArrNumClueForColumn[targetIndex] < max ){
                m_oArrArrClueForColumn[targetIndex][m_nArrNumClueForColumn[targetIndex]].setVal( 0 );
                m_nArrNumClueForColumn[targetIndex]++;
                return( true );
            }
        }
    }

    return( false );
}

//----------------------------
// ヒントの削除
//----------------------------
bool CLogicGrid::deleteClueAt( bool isRow, int targetIndex, int clueAt ){
    if( isRow ){
        if( targetIndex >= 0 && targetIndex < m_nRow ){
            if( clueAt >= 0 && clueAt < m_nArrNumClueForRow[targetIndex] ){
                if( m_nArrNumClueForRow[targetIndex] <= 1 ){
                    m_nArrNumClueForRow[targetIndex] = 1;
                    m_oArrArrClueForRow[targetIndex][0].setVal( 0 );
                }else{
                    m_nArrNumClueForRow[targetIndex]--;
                    
                    for( int i=clueAt; i<m_nArrNumClueForRow[targetIndex]; i++ ){
                        m_oArrArrClueForRow[targetIndex][i].copy( &m_oArrArrClueForRow[targetIndex][i+1] );
                    }
                }
                return(true );
            }
        }
    }else{
        if( targetIndex >= 0 && targetIndex < m_nColumn ){
            if( clueAt >= 0 && clueAt < m_nArrNumClueForColumn[targetIndex] ){
                if( m_nArrNumClueForColumn[targetIndex] <= 1 ){
                    m_nArrNumClueForColumn[targetIndex] = 1;
                    m_oArrArrClueForColumn[targetIndex][0].setVal( 0 );
                }else{
                    m_nArrNumClueForColumn[targetIndex]--;
                    
                    for( int i=clueAt; i<m_nArrNumClueForColumn[targetIndex]; i++ ){
                        m_oArrArrClueForColumn[targetIndex][i].copy( &m_oArrArrClueForColumn[targetIndex][i+1] );
                    }
                }
                return(true );
            }
        }
    }

    return( false );
}


//----------------------------
// ヒントの設定
//----------------------------
bool CLogicGrid::setClueAt( bool isRow, int targetIndex, int clueAt, int val ){
    if( isRow ){
        if( targetIndex >= 0 && targetIndex < m_nRow ){
            if( clueAt >= 0 && clueAt < m_nArrNumClueForRow[targetIndex] ){
                m_oArrArrClueForRow[targetIndex][clueAt].setVal( val );
                return(true );
            }
        }
    }else{
        if( targetIndex >= 0 && targetIndex < m_nColumn ){
            if( clueAt >= 0 && clueAt < m_nArrNumClueForColumn[targetIndex] ){
                m_oArrArrClueForColumn[targetIndex][clueAt].setVal( val );
                return(true );
            }
        }
    }

    return( false );
}

//--------------
// データの出力
//--------------
bool CLogicGrid::dumpToBuf( char* buf ){
    // 用心
    if( buf == NULL ){
        LOGE( "@ CLogicGrid::dumpToBuf: buf is NULL\n" );
        return( false );
    }
    
    if( m_nRow <= 0 || m_nColumn <= 0 ){
        LOGE( "@ CLogicGrid::dumpToBuf: INVALID size: row=%d, col=%d\n", m_nRow, m_nColumn );
        return( false );
    }

    // サイズの出力
    sprintf( buf, "// サイズ：タテ×ヨコ\n" );
    buf = &buf[util_str_len(buf)];
    
    sprintf( buf, "%d,%d\n", m_nRow, m_nColumn );
    buf = &buf[util_str_len(buf)];
    
    // タテのヒント
    sprintf( buf, "\n// タテのヒント\n" );
    buf = &buf[util_str_len(buf)];

    for( int i=0; i<m_nRow; i++ ){
        for( int j=0; j<m_nArrNumClueForRow[i]; j++ ){
            sprintf( buf, "%d,", m_oArrArrClueForRow[i][j].getVal() );
            buf = &buf[util_str_len(buf)];
        }

        // 最後のカンマを改行に差し替え
        buf--;
        *buf = '\n';
        buf++;
    }

    // ヨコのヒント
    sprintf( buf, "\n// ヨコのヒント\n" );
    buf = &buf[util_str_len(buf)];

    for( int i=0; i<m_nColumn; i++ ){
        for( int j=0; j<m_nArrNumClueForColumn[i]; j++ ){
            sprintf( buf, "%d,", m_oArrArrClueForColumn[i][j].getVal() );
            buf = &buf[util_str_len(buf)];
        }

        // 最後のカンマを改行に差し替え
        buf--;
        *buf = '\n';
        buf++;
    }

    // ここまできたら成功
    return( true );
}

//--------------
// 整合性確認
//--------------
bool CLogicGrid::checkVerification( void ){

    return( false );
}

