/*+----------------------------------------------------------------+
  |	Title:		LogicGrid.cpp [共通環境]
  |	Comment:	ロジック（コア）：グリッド（盤面全体）：ヒント関連
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
//----------------------------------------
// ヒント取得：ヨコ
//----------------------------------------
// ヒントの数
int CLogicGrid::getClueNumForColumn( int targetIndex ){
    if( targetIndex < 0 || targetIndex >= m_nColumn ){
        LOGE( "@ CLogicGrid::getClueNumForColumn: INVALID PARAM: targetIndex=%d/%d\n", targetIndex, m_nColumn );
        return( 0 );
    }
    
    return( m_nArrNumClueForColumn[targetIndex] );
}

// ヒント
CLogicClue* CLogicGrid::getClueForColumn( int targetIndex, int clueAt ){
    int numClue = getClueNumForColumn( targetIndex );
    
    if( clueAt < 0 || clueAt >= numClue ){
        LOGE( "@ CLogicGrid::getClueForColumn: INVALID PARAM: targetIndex=%d/%d, clueAt=%d/%d\n", targetIndex, m_nColumn, clueAt, numClue );
        return( NULL );
    }
    
    return( &m_oArrArrClueForColumn[targetIndex][clueAt] );
}

//----------------------------------------
// ヒント取得：タテ
//----------------------------------------
// ヒントの数
int CLogicGrid::getClueNumForRow( int targetIndex ){
    if( targetIndex < 0 || targetIndex >= m_nRow ){
        LOGE( "@ CLogicGrid::getClueNumForRow: INVALID PARAM: targetIndex=%d/%d\n", targetIndex, m_nRow );
        return( 0 );
    }
    
    return( m_nArrNumClueForRow[targetIndex] );
}

// ヒント
CLogicClue* CLogicGrid::getClueForRow( int targetIndex, int clueAt ){
    int numClue = getClueNumForRow( targetIndex );
    
    if( clueAt < 0 || clueAt >= numClue ){
        LOGE( "@ CLogicGrid::getClueForRow: INVALID PARAM: targetIndex=%d/%d, clueAt=%d/%d\n", targetIndex, m_nRow, clueAt, numClue );
        return( NULL );
    }
    
    return( &m_oArrArrClueForRow[targetIndex][clueAt] );
}

//----------------------------------------
// 指定のヒント配列を取得
//----------------------------------------
bool CLogicGrid::getClueArray( bool isRow, int targetIndex, CLogicClue** ppClue, int* pSize ){
    // 無効は無視
    if( ppClue == NULL || pSize == NULL ){
        LOGE( "@ CLogicGrid::getClueArray: INVALID DATA: ppClue=%p, pSize=%p\n", ppClue, pSize );
        return( false );
    }
    
    if( isRow ){
        // 無効は無視
        if( targetIndex < 0 || targetIndex >= m_nRow ){
            LOGE( "@ CLogicGrid::getClueArray: INVALID ROW TARGET: index=%d/%d\n", targetIndex, m_nRow );
            return( false );
        }
        
        *pSize = m_nArrNumClueForRow[targetIndex];
        *ppClue = m_oArrArrClueForRow[targetIndex];
    }else{
        // 無効は無視
        if( targetIndex < 0 || targetIndex >= m_nColumn ){
            LOGE( "@ CLogicGrid::getClueArray: INVALID COLUMN TARGET: index=%d/%d\n", targetIndex, m_nColumn );
            return( false );
        }
        
        *pSize = m_nArrNumClueForColumn[targetIndex];
        *ppClue = m_oArrArrClueForColumn[targetIndex];
    }
    
    return( true );
}

//--------------------------------------------
// ヒントの確定（※内部でヒントの更新が予約される）
//--------------------------------------------
void CLogicGrid::fixClueAll( bool isResetStatusAll ){
    //----------------------------------------------------------------
    // 指定があればヒントの状態を無効化（※[reset]だとブロック情報が消えるのでダメ）
    //----------------------------------------------------------------
    if( isResetStatusAll ){
        // ヨコ
        for( int i=0; i<m_nColumn; i++ ){
            for( int j=0; j<m_nArrNumClueForColumn[i]; j++ ){
                m_oArrArrClueForColumn[i][j].resetStatus();
            }
        }
        
        // タテ
        for( int i=0; i<m_nRow; i++ ){
            for( int j=0; j<m_nArrNumClueForRow[i]; j++ ){
                m_oArrArrClueForRow[i][j].resetStatus();
            }
        }
    }
    
    //-------------------------------
    // ヒントの状態再構築
    //-------------------------------
    // ヨコ
    for( int i=0; i<m_nColumn; i++ ){
        while( fixClueDeterminedAt( false, i ) > 0 );
        fixClueImcompatibleAt( false, i );
    }
    
    // タテ
    for( int i=0; i<m_nRow; i++ ){
        while( fixClueDeterminedAt( true, i ) > 0 );
        fixClueImcompatibleAt( true, i );
    }
}

//-------------------------------------------
// ヒント／ライン確定
//-------------------------------------------
void CLogicGrid::fixClueAndLineDeterminedWithLog( CLogicLog* pLog ){
    //-------------------------------------------------------------
    // ヒント／ライン確定検出
    //-------------------------------------------------------------
    // 確定値：[>0]で仮定確定（値は仮定レベル）、[0]で確定、[-1]で未確定、[-2]で矛盾
    int arrClueDeterminedForRow[LOGIC_SOLID_WH_MAX][LOGIC_CLUE_WH_MAX];
    int arrClueDeterminedForCol[LOGIC_SOLID_WH_MAX][LOGIC_CLUE_WH_MAX];
    int arrLineDeterminedForRow[LOGIC_SOLID_WH_MAX];
    int arrLineDeterminedForCol[LOGIC_SOLID_WH_MAX];
    
    //-------------------------------------------
    // 状況の検出
    //-------------------------------------------
    // ヒント＆ライン（タテ）
    for( int i=0; i<m_nRow; i++ ){
        bool isImcompatible = false;
        int num = getClueNumForRow( i );
        for( int j=0; j<num; j++ ){
            CLogicClue* pClue = getClueForRow( i, j );
            if( pClue->isDetermined() ){ arrClueDeterminedForRow[i][j] = 0; }
            else if( pClue->isAssumed() ){ arrClueDeterminedForRow[i][j] = m_nAssumedLevel; }
            else if( pClue->isImcompatible() ){ arrClueDeterminedForRow[i][j] = -2; isImcompatible = true; }
            else{ arrClueDeterminedForRow[i][j] = -1; }
        }
        
        if( isImcompatible ){
            arrLineDeterminedForRow[i] = -2;
        }else{
            arrLineDeterminedForRow[i] = (checkLineCompleted( true, i ))? m_nAssumedLevel: -1;
        }
    }
    
    // ヒント＆ライン（ヨコ）
    for( int i=0; i<m_nColumn; i++ ){
        bool isImcompatible = false;
        int num = getClueNumForColumn( i );
        for( int j=0; j<num; j++ ){
            CLogicClue* pClue = getClueForColumn( i, j );
            if( pClue->isDetermined() ){ arrClueDeterminedForCol[i][j] = 0; }
            else if( pClue->isAssumed() ){ arrClueDeterminedForCol[i][j] = m_nAssumedLevel; }
            else if( pClue->isImcompatible() ){ arrClueDeterminedForCol[i][j] = -2; isImcompatible = true; }
            else{ arrClueDeterminedForCol[i][j] = -1; }
        }
        
        if( isImcompatible ){
            arrLineDeterminedForCol[i] = -2;
        }else{
            arrLineDeterminedForCol[i] = (checkLineCompleted( false, i ))? m_nAssumedLevel: -1;
        }
    }
    
    //--------------------------------------------------------------
    // ヒントログの登録
    //--------------------------------------------------------------
    // タテ
    for( int i=0; i<m_nRow; i++ ){
        int num = getClueNumForRow( i );
        for( int j=0; j<num; j++ ){
            // ヒントの矛盾
            if( arrClueDeterminedForRow[i][j] == -2 ){
                if( m_nArrClueDeterminedForRow[i][j] != arrClueDeterminedForRow[i][j] ){
                    if( pLog != NULL ){
                        // log: ヒント（タテ）確定（矛盾）
                        pLog->addLog_STATUS_CLUE_IN_ROW( i, j, 0, eLOGIC_CLUE_STATUS_IMCOMPATIBLE );
                    }
                    m_nArrClueDeterminedForRow[i][j] = arrClueDeterminedForRow[i][j];
                }
            }
            // ヒントの確定
            else if( arrClueDeterminedForRow[i][j] >= 0 ){
                if( m_nArrClueDeterminedForRow[i][j] < 0 || m_nArrClueDeterminedForRow[i][j] > arrClueDeterminedForRow[i][j] ){
                    if( pLog != NULL ){
                        // log: ヒント（タテ）確定
                        if( arrClueDeterminedForRow[i][j] > 0 ){
                            pLog->addLog_STATUS_CLUE_IN_ROW( i, j, m_nAssumedLevel, eLOGIC_CLUE_STATUS_ASSUMED );
                        }else{
                            pLog->addLog_STATUS_CLUE_IN_ROW( i, j, 0, eLOGIC_CLUE_STATUS_DETERMINED );
                        }
                    }
                    m_nArrClueDeterminedForRow[i][j] = arrClueDeterminedForRow[i][j];
                }
            }
            // ヒントの解除
            else{
                if( m_nArrClueDeterminedForRow[i][j] != arrClueDeterminedForRow[i][j] ){
                    if( pLog != NULL ){
                        // log: ヒント（タテ）確定（解除）
                        pLog->addLog_STATUS_CLUE_IN_ROW( i, j, 0, eLOGIC_CLUE_STATUS_INVALID );
                    }
                    m_nArrClueDeterminedForRow[i][j] = arrClueDeterminedForRow[i][j];
                }
            }
        }
    }
    
    // ヨコ
    for( int i=0; i<m_nColumn; i++ ){
        int num = getClueNumForColumn( i );
        for( int j=0; j<num; j++ ){
            // ヒントの矛盾
            if( arrClueDeterminedForCol[i][j] == -2 ){
                if( m_nArrClueDeterminedForCol[i][j] != arrClueDeterminedForCol[i][j] ){
                    if( pLog != NULL ){
                        // log: ヒント(ヨコ）確定（矛盾）
                        pLog->addLog_STATUS_CLUE_IN_COLUMN( i, j, 0, eLOGIC_CLUE_STATUS_IMCOMPATIBLE );
                    }
                    m_nArrClueDeterminedForCol[i][j] = arrClueDeterminedForCol[i][j];
                }
            }
            // ヒントの確定
            else if( arrClueDeterminedForCol[i][j] >= 0 ){
                if( m_nArrClueDeterminedForCol[i][j] < 0 || m_nArrClueDeterminedForCol[i][j] > arrClueDeterminedForCol[i][j] ){
                    if( pLog != NULL ){
                        // log: ヒント(ヨコ）確定
                        if( arrClueDeterminedForCol[i][j] > 0 ){
                            pLog->addLog_STATUS_CLUE_IN_COLUMN( i, j, m_nAssumedLevel, eLOGIC_CLUE_STATUS_ASSUMED );
                        }else{
                            pLog->addLog_STATUS_CLUE_IN_COLUMN( i, j, 0, eLOGIC_CLUE_STATUS_DETERMINED );
                        }
                    }
                    m_nArrClueDeterminedForCol[i][j] = arrClueDeterminedForCol[i][j];
                }
            }
            // ヒントの解除
            else{
                if( m_nArrClueDeterminedForCol[i][j] != arrClueDeterminedForCol[i][j] ){
                    if( pLog != NULL ){
                        // log: ヒント(ヨコ）確定（解除）
                        pLog->addLog_STATUS_CLUE_IN_COLUMN( i, j, 0, eLOGIC_CLUE_STATUS_INVALID );
                    }
                    m_nArrClueDeterminedForCol[i][j] = arrClueDeterminedForCol[i][j];
                }
            }
        }
    }
    
    //--------------------------------------------------------------
    // ラインログの登録
    //--------------------------------------------------------------
    // タテ
    for( int i=0; i<m_nRow; i++ ){
        // ラインの矛盾
        if( arrLineDeterminedForRow[i] == -2 ){
            if( m_nArrLineDeterminedForRow[i] != arrLineDeterminedForRow[i] ){
                if( pLog != NULL ){
                    // log: ライン（タテ）確定（矛盾）
                    pLog->addLog_STATUS_LINE_IN_ROW( i, 0, eLOGIC_LINE_STATUS_IMCOMPATIBLE );
                }
                m_nArrLineDeterminedForRow[i] = arrLineDeterminedForRow[i];
            }
        }
        // ラインの確定
        else if( arrLineDeterminedForRow[i] >= 0 ){
            if( m_nArrLineDeterminedForRow[i] < 0 || m_nArrLineDeterminedForRow[i] > arrLineDeterminedForRow[i] ){
                if( pLog != NULL ){
                    // log: ライン（タテ）確定
                    if( arrLineDeterminedForRow[i] > 0 ){
                        pLog->addLog_STATUS_LINE_IN_ROW( i, m_nAssumedLevel, eLOGIC_LINE_STATUS_ASSUMED );
                    }else{
                        pLog->addLog_STATUS_LINE_IN_ROW( i, 0, eLOGIC_LINE_STATUS_DETERMINED );
                    }
                }
                m_nArrLineDeterminedForRow[i] = arrLineDeterminedForRow[i];
            }
        }
        // ラインの解除
        else{
            if( m_nArrLineDeterminedForRow[i] != arrLineDeterminedForRow[i] ){
                if( pLog != NULL ){
                    // log: ライン（タテ）確定（解除）
                    pLog->addLog_STATUS_LINE_IN_ROW( i, m_nAssumedLevel, eLOGIC_LINE_STATUS_INVALID );
                }
                m_nArrLineDeterminedForRow[i] = arrLineDeterminedForRow[i];
            }
        }
    }
    
    // ヨコ
    for( int i=0; i<m_nColumn; i++ ){
        // ラインの矛盾
        if( arrLineDeterminedForCol[i] == -2 ){
            if( m_nArrLineDeterminedForCol[i] != arrLineDeterminedForCol[i] ){
                if( pLog != NULL ){
                    // log: ライン(ヨコ)確定
                    pLog->addLog_STATUS_LINE_IN_COLUMN( i, m_nAssumedLevel, eLOGIC_LINE_STATUS_IMCOMPATIBLE );
                }
                m_nArrLineDeterminedForCol[i] = arrLineDeterminedForCol[i];
            }
        }
        // ラインの確定
        else if( arrLineDeterminedForCol[i] >= 0 ){
            if( m_nArrLineDeterminedForCol[i] < 0 || m_nArrLineDeterminedForCol[i] > arrLineDeterminedForCol[i] ){
                if( pLog != NULL ){
                    // log: ライン(ヨコ)確定
                    if( arrLineDeterminedForCol[i] > 0 ){
                        pLog->addLog_STATUS_LINE_IN_COLUMN( i, m_nAssumedLevel, eLOGIC_LINE_STATUS_ASSUMED );
                    }else{
                        pLog->addLog_STATUS_LINE_IN_COLUMN( i, 0, eLOGIC_LINE_STATUS_DETERMINED );
                    }
                }
                m_nArrLineDeterminedForCol[i] = arrLineDeterminedForCol[i];
            }
        }
        // 解除
        else{
            if( m_nArrLineDeterminedForCol[i] != arrLineDeterminedForCol[i] ){
                if( pLog != NULL ){
                    // log: ライン(ヨコ)確定（解除）
                    pLog->addLog_STATUS_LINE_IN_COLUMN( i, m_nAssumedLevel, eLOGIC_LINE_STATUS_INVALID );
                }
                m_nArrLineDeterminedForCol[i] = arrLineDeterminedForCol[i];
            }
        }
    }
}

//----------------------------------------------------------------------------------------
// 確定ヒントの取得（※この処理で取得したヒントは、その取りうるマスブロック情報が最新であることが保証される）
//----------------------------------------------------------------------------------------
CLogicClue* CLogicGrid::getUpdatedClue( bool isRow, int targetIndex, int clueAt ){
    //--------------------------------
    // 対象ヒント配列の取り出し
    //--------------------------------
    int sizeClue;
    CLogicClue* arrClue;
    if( !getClueArray( isRow, targetIndex, &arrClue, &sizeClue ) || clueAt < 0 || clueAt >= sizeClue ){
        LOGE( "@ CLogicGrid::getUpdatedClue: FAILED getClueArray: isRow=%d, targetIndex=%d, clueAt=%d\n", isRow, targetIndex, clueAt );
        return( NULL );
    }
    
    //-------------------------------
    // ヒントの要素が０であればそのまま返す
    //-------------------------------
    if( arrClue[clueAt].getVal() <= 0 ){
        return( &arrClue[clueAt] );
    }
    
    //-------------------------------
    // 確定ヒントであればそのまま返す
    //-------------------------------
    if( arrClue[clueAt].isDeterminedOrAssumed() ){
        return( &arrClue[clueAt] );
    }
    
    //-------------------------------
    // 更新済みであればそのまま返す
    //-------------------------------
    if( (isRow && !m_bArrUpdateClueSolidInfoForRow[targetIndex]) ||
       (!isRow && !m_bArrUpdateClueSolidInfoForColumn[targetIndex]) ){
        return( &arrClue[clueAt] );
    }
    
    //================================================================================================
    // ここまできたらキャッシュミスなのでデータを更新する
    //================================================================================================
    // 全ヒントの基本ブロックを作成（※返すのは指定ブロックの情報のみだが、処理仮定で同じラインのブロックを相互参照するため）
    //================================================================================================
    int solidNum = getSolidNum( isRow );
    
    int numPivot = 0;
    static int arrClueStart[LOGIC_CLUE_WH_MAX];
    static int arrClueNum[LOGIC_CLUE_WH_MAX];
    static int arrSolidStart[LOGIC_CLUE_WH_MAX];
    static int arrSolidNum[LOGIC_CLUE_WH_MAX];
    
    // 確定されたヒントをピボットとして、未確定ヒントのブロックを検出
    int lastPivotAt = -1;
    int lastStartAt = 0;
    for( int i=0; i<sizeClue; i++ ){
        // ヒントが確定していたらピボット作成
        if( arrClue[i].isDeterminedOrAssumed() ){
            // 手前に未確定要素があれば登録（※ここが実際のブロック処理対象となる）
            if( lastPivotAt >= 0 ){
                arrClueStart[numPivot] = lastPivotAt;
                arrClueNum[numPivot] = i - lastPivotAt;
                arrSolidStart[numPivot] = lastStartAt;
                arrSolidNum[numPivot] = arrClue[i].getSolidStartAt() - (arrSolidStart[numPivot]+1); // 黒の隣の白マス分浮かせる
                numPivot++;
            }
            
            lastPivotAt = -1;
            lastStartAt = arrClue[i].getSolidStartAt() + (arrClue[i].getSolidNum()+1);  // 黒の隣の白マス分浮かせる
        }
        // ヒントが未確定なら
        else{
            // 未設定であれば開始点の保持
            if( lastPivotAt < 0 ){
                lastPivotAt = i;
            }
        }
    }
    
    // 情報が残っていたら登録
    if( lastPivotAt >= 0 ){
        arrClueStart[numPivot] = lastPivotAt;
        arrClueNum[numPivot] = sizeClue - lastPivotAt;
        arrSolidStart[numPivot] = lastStartAt;
        arrSolidNum[numPivot] = solidNum - arrSolidStart[numPivot];
        numPivot++;
    }
    
    //----------------------------------------------------
    // 未確定ヒントのブロックを処理
    //----------------------------------------------------
    for( int i=0; i<numPivot; i++ ){
        CLogicClue* arrTempClue = &arrClue[arrClueStart[i]];
        int sizeTempClue = arrClueNum[i];
        int solidOfs = arrSolidStart[i];
        int solidNum = arrSolidNum[i];
 
        // ヒントの更新（※矛盾すると[false]が返る）
        if( ! CLogicClueUpdater::UpdateClue( this, arrTempClue, sizeTempClue, clueAt, isRow, targetIndex, solidOfs, solidNum ) ){
            return( NULL );
        }
        
        // 調整後に[solidOfs]を適用（※これで分割前のヒントの並でマス参照ができる）
        for( int j=0; j<sizeTempClue; j++ ){
            arrTempClue[j].addSolidStartAt( solidOfs );
        }
    }
    
    // キャッシュ更新
    if( isRow ){
        m_bArrUpdateClueSolidInfoForRow[targetIndex] = false;
    }else{
        m_bArrUpdateClueSolidInfoForColumn[targetIndex] = false;
    }
    
    return( &arrClue[clueAt] );
}

//-------------------------------------------------------
// ヒントブロックの更新指定：全部
//-------------------------------------------------------
void CLogicGrid::updateClueSolidInfoForAll( void ){
    for( int i=0; i<m_nColumn; i++ ){
        m_bArrUpdateClueSolidInfoForColumn[i] = true;
    }
    
    for( int i=0; i<m_nRow; i++ ){
        m_bArrUpdateClueSolidInfoForRow[i] = true;
    }
}

//-------------------------------------------------------
// キーの更新：ヨコ
//-------------------------------------------------------
void CLogicGrid::updateClueSolidInfoForColumnAt( int at ){
    // 指定枠の更新
    if( at >= 0 && at < m_nColumn ){
        m_bArrUpdateClueSolidInfoForColumn[at] = true;
    }
}

//-------------------------------------------------------
// キーの更新：タテ
//-------------------------------------------------------
void CLogicGrid::updateClueSolidInfoForRowAt( int at ){
    // 指定枠の更新
    if( at >= 0 && at < m_nRow ){
        m_bArrUpdateClueSolidInfoForRow[at] = true;
    }
}

//--------------------------------------------------------------------
// ヒントの確定：ラインに対して確定マスを調べる（※内部でヒントの更新が予約される）
//--------------------------------------------------------------------
int CLogicGrid::fixClueDeterminedAt( bool isRow, int targetIndex ){
    int solidNum = getSolidNum( isRow );
    
    // work（※対象のヒント配列）
    int numClue;
    CLogicClue* arrClue;
    if( ! getClueArray( isRow, targetIndex, &arrClue, &numClue ) ){
        return( 0 );
    }

    //-------------------------------------------------
    // 特殊（※ヒントが０の場合は白マスの埋め尽くしで確定）
    //（※[numClue]は０でも[arrClue]の第一要素は有効である想定）
    //--------------------------------------------------
    if( numClue <= 0 || arrClue[0].getVal() <= 0 ){
        // 対象のヒントが未確定であれば
        if( arrClue[0].isUnsettled() ){
            bool isOk = true;
            int assumedLevel = 0;
            
            for( int i=0; i<solidNum; i++ ){
                CLogicSolid* pSolid = getSolid( isRow, targetIndex, i );
                
                // 白でないマスがあれば抜ける
                if( ! pSolid->isWhite() ){
                    isOk = false;
                    break;
                }

                // 仮定レベルの確認（※最も大きな値を保持する）
                if( assumedLevel < pSolid->getAssumedLevel() ){
                    assumedLevel = pSolid->getAssumedLevel();
                }
            }

            // 全て白であれば確定
            if( isOk ){
                if( assumedLevel > 0 ){
                    arrClue[0].setStatus( eLOGIC_CLUE_STATUS_ASSUMED, assumedLevel );
                }else{
                    arrClue[0].setStatus( eLOGIC_CLUE_STATUS_DETERMINED );
                }

                // 確定マス情報の設定（※一応設定しておく＝全て白なので０）
                arrClue[0].setSolidBlock( 0, 0 );

                // ヒントが確定したのでキャッシュキーの更新
                if( isRow ){
                    updateClueSolidInfoForRowAt( targetIndex );
                }else{
                    updateClueSolidInfoForColumnAt( targetIndex );
                }
                
                return( 1 );
            }
        }
        
        return( 0 );
    }
    
    //-------------------------------------------
    // ここまできたら通常判定（※黒マスがある）
    //-------------------------------------------
    // work
    int numDetermined = 0;
    bool isInBlock = false;
    int assumedLevel = 0;
    int numBlack = 0;
    int lastSolidAt = -1;

    //-------------------------------------
    // 黒マスの確認（※ヒントが確定されたか？）
    //-------------------------------------
    int i = 0;
    while( i <= solidNum ){
        CLogicSolid* pSolid = NULL;
        if( i < solidNum ){
            pSolid = getSolid( isRow, targetIndex, i );
        }
        
        // 黒マスであれば
        if( pSolid != NULL && pSolid->isBlack() ){
            // データ更新
            isInBlock = true;
            numBlack++;
            lastSolidAt = i;
            
            // 仮定レベルの確認（※最も大きな値を保持する）
            if( assumedLevel < pSolid->getAssumedLevel() ){
                assumedLevel = pSolid->getAssumedLevel();
            }
        }
        // 黒マスでない／端に到達したら
        else{
            // ブロックが開始していたら終端判定
            if( isInBlock ){
                // 最後の所属ヒントの確認
                int uniqueIndex = checkUniqueBlockIndexWithSolidAt( isRow, targetIndex, lastSolidAt );

                // 黒マスが複数ある場合、最初の所属ヒントの確認
                if( numBlack > 1 ){
                    if( uniqueIndex != checkUniqueBlockIndexWithSolidAt( isRow, targetIndex, lastSolidAt-(numBlack-1) ) ){
                        uniqueIndex = -1;
                    }
                }
                
                // 一意のブロックであれば（※先頭と末尾の黒マスが一意であれば）
                if( uniqueIndex >= 0 && uniqueIndex < numClue ){
                    // 黒マスの数がヒントの値と一致していれば確定
                    if( numBlack == arrClue[uniqueIndex].getVal() ){
                        // 対象のヒントが未確定であれば
                        if( arrClue[uniqueIndex].isUnsettled() ){
                            if( assumedLevel > 0 ){
                                arrClue[uniqueIndex].setStatus( eLOGIC_CLUE_STATUS_ASSUMED, assumedLevel );
                            }else{
                                arrClue[uniqueIndex].setStatus( eLOGIC_CLUE_STATUS_DETERMINED );
                            }
                            
                            // 確定マス情報の設定
                            arrClue[uniqueIndex].setSolidBlock( lastSolidAt-(numBlack-1), numBlack );

                            // ヒントが確定したのでキャッシュキーの更新
                            if( isRow ){
                                updateClueSolidInfoForRowAt( targetIndex );
                            }else{
                                updateClueSolidInfoForColumnAt( targetIndex );
                            }
                            
                            numDetermined++;
                        }
                    }
                }
            }
            
            // ブロックの終了
            isInBlock = false;
            assumedLevel = 0;
            numBlack = 0;
        }
        
        // 次へ
        i++;
    }
    
    return( numDetermined );
}

//-------------------------------------------------
// ヒントの確定：ラインに対して矛盾を調べる
//-------------------------------------------------
bool CLogicGrid::fixClueImcompatibleAt( bool isRow, int targetIndex ){
    int numClue;
    CLogicClue* arrClue;
    if( ! getClueArray( isRow, targetIndex, &arrClue, &numClue ) ){
        return( false );
    }

    // ブロック設定に失敗したら矛盾（※白マスに不整合がある）
    if( checkImcompatibleAt( isRow, targetIndex ) ){
        // ヒント全てを矛盾にしておく（※とりあえず）
        for( int i=0; i<numClue; i++ ){
            arrClue[i].setStatus( eLOGIC_CLUE_STATUS_IMCOMPATIBLE, 0, true );    // 強制設定
        }
        return( true );
    }
    
    // 孤立した黒マスがあれば矛盾
    int solidNum = getSolidNum( isRow );
    for( int i=0; i<solidNum; i++ ){
        if( checkSolidBlack( isRow, targetIndex, i ) ){
            if( checkSolidIsolatedAt( isRow, targetIndex, i ) ){
                // ヒント全てを矛盾にしておく（※とりあえず）
                for( int j=0; j<numClue; j++ ){
                    arrClue[j].setStatus( eLOGIC_CLUE_STATUS_IMCOMPATIBLE, 0, true );    // 強制設定
                }
                return( true );
            }
        }
    }
    
    return( false );
}
