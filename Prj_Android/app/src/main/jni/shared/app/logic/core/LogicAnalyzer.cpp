/*+----------------------------------------------------------------+
  |	Title:		LogicAnalyzer.cpp [共通環境]
  |	Comment:	ロジック（コア）：解析（※実際にマスを埋める処理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicAnalyzer.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 矛盾判定時にヨコ／タテを見るか？
//#define CHECK_COLUMN_AND_ROW_FOR_IMCOMPATIBLE_CHECK

// 引数
#define LOGIC_ANALYZER_PARAM_ARGS_FOR_COLUMN    pGrid, false, targetColumn, pLog
#define LOGIC_ANALYZER_PARAM_ARGS_FOR_ROW       pGrid, true, targetRow, pLog

//----------------------
// 解析スコア
//----------------------
// クロ加点
#define CHECK_ANALYZE_SCORE_BLACK_THEN_CONTINUE( _score )   \
    if( num > 0 ){                                          \
        (_score) += num;                                    \
        numBlack += num;                                    \
        numTotal += num;                                    \
        continue;                                           \
    }

// シロ加点
#define CHECK_ANALYZE_SCORE_WHITE_THEN_CONTINUE( _score )   \
    if( num > 0 ){                                          \
        (_score) += num;                                    \
        numWhite += num;                                    \
        numTotal += num;                                    \
        continue;                                           \
    }

//-------------------------
// ログの追加
//-------------------------
// 黒マスの確定
#define ADD_LOG_BLACK( _at )                                                                                    \
    if( pLog != NULL ){                                                                                         \
        int addColumn = (isRow)? (_at): targetIndex;                                                            \
        int addRow = (isRow)? targetIndex: (_at);                                                               \
        pLog->addLog_ANALYZE_SOLID( addColumn, addRow, assumedLv, eLOGIC_SOLID_TYPE_BLACK );                    \
        if( s_nTargetColumn == addColumn && s_nTargetRow == addRow ){                                           \
            pLog->addLog_ANALYZE_TARGET_DETERMINED( addColumn, addRow, assumedLv, eLOGIC_SOLID_TYPE_BLACK );    \
        }                                                                                                       \
    }

// 白マスの確定
#define ADD_LOG_WHITE( _at )                                                                                    \
    if( pLog != NULL ){                                                                                         \
        int addColumn = (isRow)? (_at): targetIndex;                                                            \
        int addRow = (isRow)? targetIndex: (_at);                                                               \
        pLog->addLog_ANALYZE_SOLID( addColumn, addRow, assumedLv, eLOGIC_SOLID_TYPE_WHITE );                    \
        if( s_nTargetColumn == addColumn && s_nTargetRow == addRow ){                                           \
            pLog->addLog_ANALYZE_TARGET_DETERMINED( addColumn, addRow, assumedLv, eLOGIC_SOLID_TYPE_WHITE );    \
        }                                                                                                       \
    }

//-------------------------
// メソッド指定
//-------------------------
// メソッド開始
#define START_ANALYZE_METHOD( _type )                                                       \
    if( pLog != NULL ){                                                                     \
        pLog->addLog_INFO_START( eLLC_INFO_ANALYZE_METHOD, (int)(_type), ((isRow)? 1: 0) ); \
    }

// メソッド終了
#define END_ANALYZE_METHOD()                                                                \
    if( pLog != NULL ){                                                                     \
        pLog->addLog_INFO_END( eLLC_INFO_ANALYZE_METHOD, num );                             \
    }

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
int CLogicAnalyzer::s_nTargetColumn;
int CLogicAnalyzer::s_nTargetRow;

int CLogicAnalyzer::s_nTotalTry;
int CLogicAnalyzer::s_nAnalyzeScore_00;
int CLogicAnalyzer::s_nAnalyzeScore_01;
int CLogicAnalyzer::s_nAnalyzeScore_02;
int CLogicAnalyzer::s_nAnalyzeScore_03;
int CLogicAnalyzer::s_nAnalyzeScore_04;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------
// スコアのリセット
//------------------
void CLogicAnalyzer::ResetAnalyzeScore( void ){
    s_nTotalTry = 0;
    s_nAnalyzeScore_00 = 0;
    s_nAnalyzeScore_01 = 0;
    s_nAnalyzeScore_02 = 0;
    s_nAnalyzeScore_03 = 0;
    s_nAnalyzeScore_04 = 0;
}

//------------------------------------------------------------------------------
// 解析：指定のマス[targetColumn,targetRow]を中心に縦横のラインを変化がなくなるまで処理する
//------------------------------------------------------------------------------
int CLogicAnalyzer::Analyze( CLogic* pLogic, int targetColumn, int targetRow,
                             CLogicLog* pLog, int* pNumBlack, int* pNumWhite ){
    // 対象の設定
    s_nTargetColumn = targetColumn;
    s_nTargetRow = targetRow;

    // 施行回数
    s_nTotalTry++;

    // ワーク
    CLogicGrid* pGrid = pLogic->getGrid();
    int num;
    int numBlack = 0;
    int numWhite = 0;
    int numTotal = 0;
    bool isColumnCompleted = (targetColumn < 0);
    bool isRowCompleted = (targetRow < 0);

    // 対象マスの抽出（※選択が有効で空白であれば＝空白でなければ変化しないので無視できる）
    CLogicSolid* pTargetSolid = NULL;
    if( targetColumn >= 0  && targetRow >= 0 ){
        pTargetSolid = pGrid->getSolid( targetColumn, targetRow );
        if( pTargetSolid != NULL && !pTargetSolid->isBlank() ){
            pTargetSolid = NULL;
        }

        if( pLog != NULL ){
            // log:対象のマス情報
            pLog->addLog_ANALYZE_TARGET_INFO( targetColumn, targetRow );
        }
    }
    
    // 指定行／列に対し差分がなくなるまで各処理を施行
    while( !isColumnCompleted || !isRowCompleted ){
        //------------------------------------------
        // ヨコの施行
        //------------------------------------------
        if( ! isColumnCompleted ){
            if( targetColumn >= 0 ){
                int numTotal0 = -1;
                while( numTotal0 != numTotal ){
                    numTotal0 = numTotal;

                    // 開始前にステータス更新（※ヒントの状況を確定しておく）
                    pLogic->fixStatusWithLog( pLog, false );    // 前方遷移なのでステータスは持ち越す

                    // analyze:00
                    num = _checkHintLenThenSetBlack( LOGIC_ANALYZER_PARAM_ARGS_FOR_COLUMN );
                    CHECK_ANALYZE_SCORE_BLACK_THEN_CONTINUE( s_nAnalyzeScore_00 );

                    // analyze:01
                    num = _checkIsolatedSolidThenSetWhite( LOGIC_ANALYZER_PARAM_ARGS_FOR_COLUMN );
                    CHECK_ANALYZE_SCORE_WHITE_THEN_CONTINUE( s_nAnalyzeScore_01 );

                    // analyze:02
                    num = _checkImcompatibleForWhiteThenSetBlack( LOGIC_ANALYZER_PARAM_ARGS_FOR_COLUMN );
                    CHECK_ANALYZE_SCORE_BLACK_THEN_CONTINUE( s_nAnalyzeScore_02 );

                    // analyze:03
                    num = _checkImcompatibleForBlackThenSetWhite( LOGIC_ANALYZER_PARAM_ARGS_FOR_COLUMN );
                    CHECK_ANALYZE_SCORE_WHITE_THEN_CONTINUE( s_nAnalyzeScore_03 );

                    // analyze:04
                    num = _checkcMaxClueForBlockThenSetWhite( LOGIC_ANALYZER_PARAM_ARGS_FOR_COLUMN );
                    CHECK_ANALYZE_SCORE_WHITE_THEN_CONTINUE( s_nAnalyzeScore_04 );
                }

                // ここまできたら完了
                isColumnCompleted = true;
                
                // 対象マスが変化していた場合は列を未完にする（※列に含まれるマスに変化がでたので再計算が必要）
                if( pTargetSolid != NULL && !pTargetSolid->isBlank() ){
                    isRowCompleted = false;
                    pTargetSolid = NULL;
                }
            }else{
                // 選択が無効なら完了
                isColumnCompleted = true;
            }
        }

        //------------------------------------------
        // タテの施行
        //------------------------------------------
        if( !isRowCompleted ){
            if( targetRow >= 0 ){
                int numTotal0 = -1;
                while( numTotal0 != numTotal ){
                    numTotal0 = numTotal;
                    
                    // 開始前にステータス更新（※ヒントの状況を確定しておく）
                    pLogic->fixStatusWithLog( pLog, false );    // 前方遷移なのでステータスは持ち越す
                    
                    // analyze:00
                    num = _checkHintLenThenSetBlack( LOGIC_ANALYZER_PARAM_ARGS_FOR_ROW );
                    CHECK_ANALYZE_SCORE_BLACK_THEN_CONTINUE( s_nAnalyzeScore_00 );

                    // analyze:01
                    num = _checkIsolatedSolidThenSetWhite( LOGIC_ANALYZER_PARAM_ARGS_FOR_ROW );
                    CHECK_ANALYZE_SCORE_WHITE_THEN_CONTINUE( s_nAnalyzeScore_01 );
                    
                    // analyze:02
                    num = _checkImcompatibleForWhiteThenSetBlack( LOGIC_ANALYZER_PARAM_ARGS_FOR_ROW );
                    CHECK_ANALYZE_SCORE_BLACK_THEN_CONTINUE( s_nAnalyzeScore_02 );
                    
                    // analyze:03
                    num = _checkImcompatibleForBlackThenSetWhite( LOGIC_ANALYZER_PARAM_ARGS_FOR_ROW );
                    CHECK_ANALYZE_SCORE_WHITE_THEN_CONTINUE( s_nAnalyzeScore_03 );

                    // analyze:04
                    num = _checkcMaxClueForBlockThenSetWhite( LOGIC_ANALYZER_PARAM_ARGS_FOR_ROW );
                    CHECK_ANALYZE_SCORE_WHITE_THEN_CONTINUE( s_nAnalyzeScore_04 );
                }
                
                // ここまできたら完了
                isRowCompleted = true;
                
                // 対象マスが変化していた場合は行を未完にする（※行に含まれるマスに変化がでたので再計算が必要）
                if( pTargetSolid != NULL && !pTargetSolid->isBlank() ){
                    isColumnCompleted = false;
                    pTargetSolid = NULL;
                }
            }else{
                // 選択が無効なら完了
                isRowCompleted = true;
            }
        }
    }
  
    // スコア設定
    if( pNumBlack != NULL ){
        *pNumBlack = numBlack;
    }

    if( pNumWhite != NULL ){
        *pNumWhite = numWhite;
    }
    
    return( numTotal );
}

//----------------------------------------------------------------------
// [00/黒マスの特定]:ブロックの両端からヒントの数だけマスをのばして重った部分は黒マス
//----------------------------------------------------------------------
int CLogicAnalyzer::_checkHintLenThenSetBlack( LOGIC_ANALYZER_PARAM ){
    int num = 0;
    int assumedLv = pGrid->getAssumedLevel();

    // work（※対象のヒント配列）
    int numClue;
    CLogicClue* arrClue;
    if( ! pGrid->getClueArray( isRow, targetIndex, &arrClue, &numClue ) ){
        return( 0 );
    }
    
    // メソッド開始
    START_ANALYZE_METHOD( eLOGIC_ANALYZE_00_CHECK_HINT_LEN_THEN_SET_BLACK );

    // 各ブロックを調べる
    for( int i=0; i<numClue; i++ ){
        // 確定済みは無視
        if( arrClue[i].isDeterminedOrAssumed() ){
            continue;
        }
        
        CLogicClue* pClue;
        if( (pClue=pGrid->getUpdatedClue( isRow, targetIndex, i )) != NULL ){
            // ブロックの両端に黒マスを置いた場合の重複数を算出（※ブロックがヒントの２倍の長さを受け入れられるか判定）
            int overlap = 2*pClue->getVal() - pClue->getSolidNum();
            if( overlap > 0 ){
                // 両端から内側へ[ofs]ずつせばめた中央が重なる部分
                int ofs = (pClue->getSolidNum() - overlap)/2;
                for( int j=0; j<overlap; j++ ){
                    int at = pClue->getSolidStartAt() + ofs + j;
                    if( pGrid->setSolidBlack( isRow, targetIndex, at ) ){
                        num++;
                        
                        // log
                        ADD_LOG_BLACK( at );
                    }
                }
            }
        }
    }

    // メソッド終了
    END_ANALYZE_METHOD();
    return( num );
}

//-------------------------------------------------
// [01/白マスの特定]:どのヒントにも所属しないマスは白マス
//-------------------------------------------------
int CLogicAnalyzer::_checkIsolatedSolidThenSetWhite( LOGIC_ANALYZER_PARAM ){
    int num = 0;
    int solidNum = pGrid->getSolidNum( isRow );
    int assumedLv = pGrid->getAssumedLevel();
    
    // メソッド開始
    START_ANALYZE_METHOD( eLOGIC_ANALYZE_01_CHECK_ISOLATED_SOLID_THEN_SET_WHITE );

    for( int i=0; i<solidNum; i++ ){
        // 空マスでなければ無視
        if( ! pGrid->checkSolidBlank( isRow, targetIndex, i ) ){
            continue;
        }

        // 孤立していたら白マス
        if( pGrid->checkSolidIsolatedAt( isRow, targetIndex, i ) ){
            if( pGrid->setSolidWhite( isRow, targetIndex, i ) ){
                num++;
                
                // log
                ADD_LOG_WHITE( i );
            }
        }
    }
    
    // メソッド終了
    END_ANALYZE_METHOD();
    return( num );
}

//----------------------------------------------------------------------
// [02/矛盾の特定]:空マスを白マスにした場合、矛盾するようなら黒マス
//----------------------------------------------------------------------
int CLogicAnalyzer::_checkImcompatibleForWhiteThenSetBlack( LOGIC_ANALYZER_PARAM ){
    int num = 0;
    int solidNum = pGrid->getSolidNum( isRow );
    int assumedLv = pGrid->getAssumedLevel();
    
    // メソッド開始
    START_ANALYZE_METHOD( eLOGIC_ANALYZE_02_CHECK_IMCOMPATIBLE_FOR_WHITE_THEN_SET_BLACK );
    
    for( int i=0; i<solidNum; i++ ){
        // 空マスでなければ無視
        if( ! pGrid->checkSolidBlank( isRow, targetIndex, i ) ){
            continue;
        }
        
        // 白に設定して矛盾が発生するか確認
        pGrid->setSolidWhite( isRow, targetIndex, i );

        bool isUmcompatible = false;

#ifdef CHECK_COLUMN_AND_ROW_FOR_IMCOMPATIBLE_CHECK
        // ヨコのヒントを確認
        int targetAt = (isRow)? i: targetIndex;
        int numClue = pGrid->getClueNumForColumn( targetAt );
        for( int j=0; j<numClue; j++ ){
            if( pGrid->getUpdatedClue( false, targetAt, j ) == NULL ){
                isUmcompatible = true;
                break;
            }
        }
        
        // ヨコのヒントに矛盾がなければタテを確認
        if( ! isUmcompatible ){
            targetAt = (isRow)? targetIndex: i;
            numClue = pGrid->getClueNumForRow( targetAt );
            for( int j=0; j<numClue; j++ ){
                if( pGrid->getUpdatedClue( true, targetAt, j ) == NULL ){
                    isUmcompatible = true;
                    break;
                }
            }
        }
#else
        // 注目しているヨコ／タテのみ調べる
        int numClue = (isRow)? pGrid->getClueNumForRow( targetIndex ): pGrid->getClueNumForColumn( targetIndex );
        for( int j=0; j<numClue; j++ ){
            if( pGrid->getUpdatedClue( isRow, targetIndex, j ) == NULL ){
                isUmcompatible = true;
                break;
            }
        }
#endif
        
        // 矛盾が生じたら対象のマスを黒に設定
        if( isUmcompatible ){
            pGrid->setSolidBlack( isRow, targetIndex, i, true );    // 強制
            num++;
            
            // log
            ADD_LOG_BLACK( i );
            continue;
        }
        
        // ここまできたら空マスに戻す
        pGrid->setSolidBlank( isRow, targetIndex, i );
    }
    
    // メソッド終了
    END_ANALYZE_METHOD();
    return( num );
}

//----------------------------------------------------------------------
// [03/矛盾の特定]:空マスを黒マスにした場合、矛盾するようなら白マス
//----------------------------------------------------------------------
int CLogicAnalyzer::_checkImcompatibleForBlackThenSetWhite( LOGIC_ANALYZER_PARAM ){
    int num = 0;
    int solidNum = pGrid->getSolidNum( isRow );
    int assumedLv = pGrid->getAssumedLevel();
    
    // メソッド開始
    START_ANALYZE_METHOD( eLOGIC_ANALYZE_03_CHECK_IMCOMPATIBLE_FOR_BLACK_THEN_SET_WHITE );

    for( int i=0; i<solidNum; i++ ){
        // 空マスでなければ無視
        if( ! pGrid->checkSolidBlank( isRow, targetIndex, i ) ){
            continue;
        }
        
        // 黒に設定して矛盾が発生するか確認
        pGrid->setSolidBlack( isRow, targetIndex, i );
        
        bool isUmcompatible = false;

#ifdef CHECK_COLUMN_AND_ROW_FOR_IMCOMPATIBLE_CHECK
        // ヨコのヒントを確認
        int targetAt = (isRow)? i: targetIndex;
        int numClue = pGrid->getClueNumForColumn( targetAt );
        for( int j=0; j<numClue; j++ ){
            if( pGrid->getUpdatedClue( false, targetAt, j ) == NULL ){
                isUmcompatible = true;
                break;
            }
        }
        
        // ヨコのヒントに矛盾がなければタテを確認
        if( ! isUmcompatible ){
            targetAt = (isRow)? targetIndex: i;
            numClue = pGrid->getClueNumForRow( targetAt );
            for( int j=0; j<numClue; j++ ){
                if( pGrid->getUpdatedClue( true, targetAt, j ) == NULL ){
                    isUmcompatible = true;
                    break;
                }
            }
        }
#else
        // 注目しているヨコ／タテのみ調べる
        int numClue = (isRow)? pGrid->getClueNumForRow( targetIndex ): pGrid->getClueNumForColumn( targetIndex );
        for( int j=0; j<numClue; j++ ){
            if( pGrid->getUpdatedClue( isRow, targetIndex, j ) == NULL ){
                isUmcompatible = true;
                break;
            }
        }
#endif
        
        // 矛盾が生じたら対象のマスを白に設定
        if( isUmcompatible ){
            pGrid->setSolidWhite( isRow, targetIndex, i, true );    // 強制
            num++;
            
            // log
            ADD_LOG_WHITE( i );
            continue;
        }
        
        // ここまできたら空マスに戻す
        pGrid->setSolidBlank( isRow, targetIndex, i );
    }
    
    // メソッド終了
    END_ANALYZE_METHOD();
    return( num );
}

//----------------------------------------------------------
// [04/白マスの特定]:黒マスの長さがヒントの最大と等しければ両端は白マス
//----------------------------------------------------------
int CLogicAnalyzer::_checkcMaxClueForBlockThenSetWhite( LOGIC_ANALYZER_PARAM ){
    int num = 0;
    int solidNum = pGrid->getSolidNum( isRow );
    int assumedLv = pGrid->getAssumedLevel();
    
    // work（※対象のヒント配列）
    int numClue;
    CLogicClue* arrClue;
    if( ! pGrid->getClueArray( isRow, targetIndex, &arrClue, &numClue ) ){
        return( 0 );
    }

    // メソッド開始
    START_ANALYZE_METHOD( eLOGIC_ANALYZE_04_CHECK_MAX_CLUE_FOR_BLACK_THEN_SET_WHITE );
    
    //----------------------------
    // 連続する黒マスの検出
    //----------------------------
    int numBlock = 0;
    int blockStartAt[LOGIC_CLUE_WH_MAX];
    int blackNumInBlock[LOGIC_CLUE_WH_MAX];
    
    bool isInBlock = false;
    for( int i=0; i<solidNum; i++ ){
        if( pGrid->checkSolidBlack( isRow, targetIndex, i ) ){
            if( ! isInBlock ){
                isInBlock = true;
                blockStartAt[numBlock] = i;
                blackNumInBlock[numBlock] = 0;
            }
            
            blackNumInBlock[numBlock]++;
        }else{
            if( isInBlock ){
                isInBlock = false;
                numBlock++;
            }
        }
    }
    
    if( isInBlock ){
        numBlock++;
    }
    
    //------------------------------------------------------------
    // 各黒マス毎に対応ヒントを調べ、最大の未確定の黒マスであればその両端を白に
    //------------------------------------------------------------
    for( int i=0; i<numBlock; i++ ){
        // 黒マスの前後が閉じているか？
        bool isBothSideOk = true;
        if( blockStartAt[i] > 0 ){
            if( ! pGrid->checkSolidWhite( isRow, targetIndex, blockStartAt[i]-1 ) ){
                isBothSideOk = false;
            }
        }
        
        if( (blockStartAt[i]+blackNumInBlock[i]-1) < (solidNum-1) ){
            if( ! pGrid->checkSolidWhite( isRow, targetIndex, (blockStartAt[i]+blackNumInBlock[i]-1)+1 ) ){
                isBothSideOk = false;
            }
        }
        
        // 閉じた黒マスは無視
        if( isBothSideOk ){
            continue;
        }
        
        //---------------------------------------------------------------------
        // チェック対象となるヒント数の確認（※対象の黒マスを範囲に含むヒントの最大数を調べる）
        //---------------------------------------------------------------------
        int clueValMax = 0;
        for( int j=0; j<numClue; j++ ){
            CLogicClue* pClue;
            if( (pClue=pGrid->getUpdatedClue( isRow, targetIndex, j )) != NULL ){
                if( pClue->isSolidContained( blockStartAt[i] ) && pClue->isSolidContained( blockStartAt[i] + blackNumInBlock[i] - 1 ) ){
                    int val = pClue->getVal();
                    if( clueValMax < val ){
                        clueValMax = val;
                    }
                }
            }
        }

        //--------------------------------------------
        // 黒マスが最大ヒントと合致するのであれば前後に白マス
        //--------------------------------------------
        if( blackNumInBlock[i] == clueValMax ){
            if( blockStartAt[i] > 0 ){
                int at = blockStartAt[i] - 1;
                if( pGrid->setSolidWhite( isRow, targetIndex, at ) ){
                    num++;

                    // log
                    ADD_LOG_WHITE( at );
                }
            }
            
            if( (blockStartAt[i]+blackNumInBlock[i]-1) < (solidNum-1) ){
                int at = (blockStartAt[i]+blackNumInBlock[i]-1) + 1;
                if( pGrid->setSolidWhite( isRow, targetIndex, at ) ){
                    num++;
                    
                    // log
                    ADD_LOG_WHITE( at );
                }
            }
        }
    }
    
    // メソッド終了
    END_ANALYZE_METHOD();
    return( num );
}
