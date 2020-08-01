/*+----------------------------------------------------------------+
  |	Title:		LogicClueUpdater.cpp [共通環境]
  |	Comment:	ロジック（コア）：ヒント更新（※ヒントの候補マスを更新する処理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicClueUpdater.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 引数（実際に呼び出す際の指定）
#define LOGIC_CLUE_UPDATER_PARAM pGrid, arrClue, sizeClue, &isImcompatible, isRow, targetIndex, solidOfs, solidNum

//------------------------------------------
// マスブロック共通：処理の開始
//------------------------------------------
#define CLUE_UPDATE_START                   \
    int numChanged = 0;                     \
    *isImcompatible = true

//------------------------------------------
// マスブロック共通：処理の正常終了
//------------------------------------------
#define CLUE_UPDATE_END                     \
    *isImcompatible = false;                \
    return( numChanged )

//------------------------------------------
// マスブロック共通：処理の矛盾終了
//------------------------------------------
#define CLUE_UPDATE_IMCOMPATIBLE_END        \
    *isImcompatible = true;                 \
    return( numChanged )

//------------------------------------------
// マスブロック共通：処理のスキップ
//------------------------------------------
#define CLUE_UPDATE_SKIP_IF_SINGLE_CLUE     \
    if( sizeClue <= 1 ){                    \
        *isImcompatible = false;            \
        return( numChanged );               \
    }

//------------------------------------------
// マスの変化のインクリメントと整合性の確認
//------------------------------------------
#define INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( _pClue, _logType )   \
    numChanged++;                                                                       \
    if( ! (_pClue)->checkVerification( solidNum ) ){                                    \
        *isImcompatible = true;                                                         \
        return( numChanged );                                                           \
    }                                                                                   \
    (_pClue)->addLogWithCurVal( (_logType) )

//------------------------------------------
// 各マスブロック処理結果の確認
//------------------------------------------
#define CHECK_CLUE_UPDATE_THEN_RETURN_OR_CONTINUE( _label, _score, _imcompatibleScore ) \
    if( isImcompatible ){                                                               \
        (_imcompatibleScore) += 1;                                                      \
        return( false );                                                                \
    }                                                                                   \
    if( numChanged > 0 ){                                                               \
        (_score) += numChanged;                                                         \
        continue;                                                                       \
    }

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
int CLogicClueUpdater::s_nTotalTry;
int CLogicClueUpdater::s_nFixSolidScoreBase;
int CLogicClueUpdater::s_nFixSolidScore_00;
int CLogicClueUpdater::s_nFixSolidScore_01;
int CLogicClueUpdater::s_nFixSolidScore_02;
int CLogicClueUpdater::s_nFixSolidScore_03;
int CLogicClueUpdater::s_nFixSolidScore_04;
int CLogicClueUpdater::s_nFixSolidScore_05;
int CLogicClueUpdater::s_nImcompatibleScoreBase;
int CLogicClueUpdater::s_nImcompatibleScore_00;
int CLogicClueUpdater::s_nImcompatibleScore_01;
int CLogicClueUpdater::s_nImcompatibleScore_02;
int CLogicClueUpdater::s_nImcompatibleScore_03;
int CLogicClueUpdater::s_nImcompatibleScore_04;
int CLogicClueUpdater::s_nImcompatibleScore_05;

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------
// スコアのリセット
//----------------------------
void CLogicClueUpdater::ResetFixScore( void ){
    s_nTotalTry = 0;
    s_nFixSolidScoreBase = 0;
    s_nFixSolidScore_00 = 0;
    s_nFixSolidScore_01 = 0;
    s_nFixSolidScore_02 = 0;
    s_nFixSolidScore_03 = 0;
    s_nFixSolidScore_04 = 0;
    s_nFixSolidScore_05 = 0;
    s_nImcompatibleScoreBase = 0;
    s_nImcompatibleScore_00 = 0;
    s_nImcompatibleScore_01 = 0;
    s_nImcompatibleScore_02 = 0;
    s_nImcompatibleScore_03 = 0;
    s_nImcompatibleScore_04 = 0;
    s_nImcompatibleScore_05 = 0;
}

//------------------------------------------------------------------------------------
// ヒントの更新
//------------------------------------------------------------------------------------
bool CLogicClueUpdater::UpdateClue( CLogicGrid* pGrid, CLogicClue* arrClue, int sizeClue, int blockAt,
                                    bool isRow, int targetIndex, int solidOfs, int solidNum ){
    // 施行回数のインクリメント
    s_nTotalTry++;

    // work
    CLogicClue* pClue;
    
    //---------------------------------------
    // ヒントのブロック基本情報の確定
    //---------------------------------------
    for( int j=0; j<sizeClue; j++ ){
        // 対象のヒントログをクリア
        pClue = &arrClue[j];
        pClue->resetLog();

        // 基本設定（※返値が[false]の場合は矛盾を含んでいる）
        s_nFixSolidScoreBase++;
        if( ! _setBaseSolidInfoForClue( pGrid, arrClue, sizeClue, j, isRow, targetIndex, solidOfs, solidNum ) ){
            s_nImcompatibleScoreBase++;
            return( false );
        }
        
        // ログ登録
        pClue->addLogWithCurVal( eLOGIC_CLUE_UPDATE_BASE );
    }
    
    // ヒントのブロックを相互に調整
    int numChanged;
    bool isImcompatible;
    do{
        numChanged = 0;
        
        // [00]: 届かないマスを除外
        numChanged += _fixSolidBlocksForUnreachable( LOGIC_CLUE_UPDATER_PARAM );
        CHECK_CLUE_UPDATE_THEN_RETURN_OR_CONTINUE( "_fixSolidBlocksForUnreachable", s_nFixSolidScore_00, s_nImcompatibleScore_00 );

        // [01]: 前後のブロックによる調整（※手前のブロックよりも前に行かない、後ろのブロックよりも後に行かない）
        numChanged += _fixSolidBlocksForBeforeAndAfter( LOGIC_CLUE_UPDATER_PARAM );
        CHECK_CLUE_UPDATE_THEN_RETURN_OR_CONTINUE( "_fixSolidBlocksForBeforeAndAfter", s_nFixSolidScore_01, s_nImcompatibleScore_01 );

        // [02]: トリミング（※前後の無効マスの除外）
        numChanged += _fixSolidBlocksForTrim( LOGIC_CLUE_UPDATER_PARAM );
        CHECK_CLUE_UPDATE_THEN_RETURN_OR_CONTINUE( "_trimSolidBlocks", s_nFixSolidScore_02, s_nImcompatibleScore_02 );

        // [03]: 黒マスの分布による調整
        numChanged += _fixSolidBlocksForBlackDistribution( LOGIC_CLUE_UPDATER_PARAM );
        CHECK_CLUE_UPDATE_THEN_RETURN_OR_CONTINUE( "_fixSolidBlocksForBlackDistribution", s_nFixSolidScore_03, s_nImcompatibleScore_03 );

        // [04]: マスの数がヒントの範囲内で確定していたら調整
        numChanged += _fixSolidBlocksForBlackNum( LOGIC_CLUE_UPDATER_PARAM );
        CHECK_CLUE_UPDATE_THEN_RETURN_OR_CONTINUE( "_fixSolidBlocksForBlackNum", s_nFixSolidScore_04, s_nImcompatibleScore_04 );
        
        // [05]: マスの塊の数が確定していたら調整
        numChanged += _fixSolidBlocksForSeparatedBlock( LOGIC_CLUE_UPDATER_PARAM );
        CHECK_CLUE_UPDATE_THEN_RETURN_OR_CONTINUE( "_fixSolidBlocksForSeparatedBlock", s_nFixSolidScore_05, s_nImcompatibleScore_05 );


    } while( numChanged > 0 );

    // ここまできたら更新完了
    return( true );
}

//------------------------------------------------------------------------------------
// 黒マスの一意性を考慮に入れない基本的なブロック設定（※指定のヒントが取りうるソリッドの範囲）
//------------------------------------------------------------------------------------
//・対象のヒントに対し、前のヒントマスを前詰した領域と後のヒントマスを後詰した領域を除外した範囲を設定
//------------------------------------------------------------------------------------
bool CLogicClueUpdater::_setBaseSolidInfoForClue( CLogicGrid* pGrid, CLogicClue* arrClue, int sizeClue,
                                                  int blockAt, bool isRow, int targetIndex, int solidOfs, int solidNum ){
    // work
    int at, front, back;
    int numCheck, clueVal;
    
    //------------------------------------------------------
    // 前方の開始位置の確認（※先行するヒントが最低限占有する領域の除外）
    //------------------------------------------------------
    at = 0;
    front = 0;
    while( at < sizeClue && front < solidNum ){
        // 白マスは無視
        while( front < solidNum ){
            if( ! pGrid->checkSolidWhite( isRow, targetIndex, front, solidOfs ) ){
                break;
            }
            front++;
        }
        
        // 領域のサイズを確認（※ヒントの黒マスが入る余裕があるか？）
        numCheck = 0;
        clueVal = arrClue[at].getVal();
        while( front < solidNum && numCheck < clueVal ){
            if( pGrid->checkSolidWhite( isRow, targetIndex, front, solidOfs ) ){
                break;
            }
            front++;
            numCheck++;
        }
        
        // 余裕がなければこの領域は候補から外れる
        if( numCheck < clueVal ){
            continue;
        }
        
        // 対象のブロックに達していたら確認終了
        if( at >= blockAt ){
            front -= numCheck;   // 枠の開始点を巻き戻しておく
            break;
        }
        
        // ここまできたら次のヒントへ
        front++;     // ヒント間の白マス分加算
        at++;
    }
    
    //------------------------------------------------------
    // 後方の開始位置の確認（※後続するヒントが最低限占有する領域の除外）
    //------------------------------------------------------
    at = sizeClue - 1;
    back = solidNum - 1;
    while( at >= 0 && back >= 0 ){
        // 白マスは無視
        while( back >= 0 ){
            if( ! pGrid->checkSolidWhite( isRow, targetIndex, back, solidOfs ) ){
                break;
            }
            back--;
        }
        
        // 領域のサイズを確認（※ヒントの黒マスが入る余裕があるか？）
        numCheck = 0;
        clueVal = arrClue[at].getVal();
        while( back >= 0 && numCheck < clueVal ){
            if( pGrid->checkSolidWhite( isRow, targetIndex, back, solidOfs ) ){
                break;
            }
            back--;
            numCheck++;
        }
        
        // 余裕がなければこの領域は候補から外れる
        if( numCheck < clueVal ){
            continue;
        }
        
        // 対象のブロックに達していたら確認終了
        if( at <= blockAt ){
            back += numCheck;   // 枠の開始点を巻き戻しておく
            break;
        }
        
        // ここまできたら次のヒントへ
        back--;     // ヒント間の白マス分減算
        at--;
    }
    
    // 整合性が崩れていたら返す（※矛盾）
    if( front > back ){
        //LOGE( "@ CLogicClueUpdater::_setBaseSolidInfoForClue: IMCOMPATIBLE: target=%d, at=%d, front=%d, back=%d\n", targetIndex, blockAt, front, back );
        return( false );
    }
    
    //-----------------------------------------------------------------------
    // ここまできたら設定（※この時点では[startSolidAt]の値に[solidOfs]を足しこまない）
    //-----------------------------------------------------------------------
    arrClue[blockAt].setSolidBlock( front/*+solidOs*/, back-front+1 );
    
    return( true );
}

//------------------------------------------------------------------------------------
// [00]: マスブロックの調整：届かないマスを除外
//------------------------------------------------------------------------------------
// ・行き止まり(盤面の端／白マス)付近にある一意の黒マスを逆方向に伸ばした際に、届かないマスを除外
//------------------------------------------------------------------------------------
int CLogicClueUpdater::_fixSolidBlocksForUnreachable( LOGIC_CLUE_UPDATER_PARAM_DEF ){
    CLUE_UPDATE_START;
    
    for( int i=0; i<sizeClue; i++ ){
        CLogicClue* pClue = &arrClue[i];
        
        //--------------------------------
        // 届かないマスを除外する：前方から
        //--------------------------------
        int blackAt = -1;
        int whiteAt = -1;
        for( int j=0; j<pClue->getSolidNum(); j++ ){
            int solidAt = pClue->getSolidStartAt() + j;
            
            // 黒マスであれば
            if( pGrid->checkSolidBlack( isRow, targetIndex, solidAt, solidOfs ) ){
                // 処理対象以外のブロックに含まれていないか確認
                bool isUnique = true;
                for( int k=0; k<sizeClue; k++ ){
                    if( k != i ){
                        // 含まれていたら一意ではない
                        if( arrClue[k].isSolidContained( solidAt ) ){
                            isUnique = false;
                            break;
                        }
                    }
                }
                
                // 一意であれば抜ける
                if( isUnique ){
                    blackAt = j;
                    break;
                }
            }
            // 白マスであれば位置を保持しておく
            else if( pGrid->checkSolidWhite( isRow, targetIndex, solidAt, solidOfs ) ){
                whiteAt = j;
            }
        }
        
        // 一意の黒マスがあれば
        if( blackAt >= 0 ){
            // 途中に白マスが存在していたらそこが起点になる
            if( whiteAt >= 0 ){
                blackAt -= (whiteAt+1);
                pClue->addSolidStartAt( (whiteAt+1) );
                pClue->addSolidNum( -(whiteAt+1) );
                
                // マス変化数更新＆整合性判定＆ログ登録
                INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_00_UNREACHABLE );
            }
            
            // 用心（※黒マスの先頭まで戻す：一意でない黒マスであっても一意の黒マスにつながっていれば一意として扱う）
            while( blackAt > 0 ){
                if( ! pGrid->checkSolidBlack( isRow, targetIndex, pClue->getSolidStartAt()+(blackAt-1), solidOfs ) ){
                    break;
                }
                blackAt--;
            }
            
            // 範囲が狭まる場合は終点を更新
            int numCheck = blackAt + arrClue[i].getVal();
            if( numCheck < pClue->getSolidNum() ){
                pClue->setSolidNum( numCheck );
                
                // マス変化数更新＆整合性判定＆ログ登録
                INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_00_UNREACHABLE );
            }
        }
        
        //--------------------------------
        // 届かないマスを除外する：後方から
        //--------------------------------
        blackAt = -1;
        whiteAt = -1;
        for( int j=pClue->getSolidNum()-1; j>=0; j-- ){
            int solidAt = pClue->getSolidStartAt() + j;
            
            // 黒マスであれば
            if( pGrid->checkSolidBlack( isRow, targetIndex, solidAt, solidOfs ) ){
                // 処理対象以外のブロックに含まれていないか確認
                bool isUnique = true;
                for( int k=0; k<sizeClue; k++ ){
                    if( k != i ){
                        // 含まれていたら一意ではない
                        if( arrClue[k].isSolidContained( solidAt ) ){
                            isUnique = false;
                            break;
                        }
                    }
                }
                
                // 一意であれば抜ける
                if( isUnique ){
                    blackAt = j;
                    break;
                }
            }
            // 白マスであれば位置を保持
            else if( pGrid->checkSolidWhite( isRow, targetIndex, solidAt, solidOfs ) ){
                whiteAt = j;
            }
        }
        
        if( blackAt >= 0 ){
            // 途中に白マスが存在していたらそこが起点になる
            if( whiteAt >= 0 ){
                pClue->setSolidNum( whiteAt );
                
                // マス変化数更新＆整合性判定＆ログ登録
                INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_00_UNREACHABLE );
            }
            
            // 用心（※黒マスの末尾まで戻す：一意でない黒マスであっても一意の黒マスにつながっていれば一意として扱う）
            while( blackAt < (pClue->getSolidNum()-1) ){
                if( ! pGrid->checkSolidBlack( isRow, targetIndex, pClue->getSolidStartAt()+(blackAt+1), solidOfs ) ){
                    break;
                }
                blackAt++;
            }
            
            // 範囲が狭まる場合は始点を更新
            int numCheck = blackAt - (arrClue[i].getVal()-1);
            if( numCheck > 0 ){
                pClue->addSolidStartAt( numCheck );
                pClue->addSolidNum( -numCheck );
                
                // マス変化数更新＆整合性判定＆ログ登録
                INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_00_UNREACHABLE );
            }
        }
    }
    
    CLUE_UPDATE_END;
}

//--------------------------------------------------------------------------------------
// [01]: 前後の範囲内による調整（※複数ヒントが対象）
//--------------------------------------------------------------------------------------
// ・前方のブロックの開始位置よりも前の領域を含んでいる場合、開始位置を前方のブロックの最小開始位置まで調整
// ・後方のブロックの終了位置よりも後の領域を含んでいる場合、終了位置を後方のブロックの最大開始位置まで調整
//--------------------------------------------------------------------------------------
int CLogicClueUpdater::_fixSolidBlocksForBeforeAndAfter( LOGIC_CLUE_UPDATER_PARAM_DEF ){
    CLUE_UPDATE_START;
    
    //-------------------------------
    // ヒントの数が１つ以下であればスキップ
    //-------------------------------
    CLUE_UPDATE_SKIP_IF_SINGLE_CLUE;
    
    //-------------------------------
    // 前方のブロックの開始位置を確認
    //-------------------------------
    for( int i=1; i<sizeClue; i++ ){
        CLogicClue* pCluePrev = &arrClue[i-1];
        CLogicClue* pClue = &arrClue[i];
        
        // 手前のブロックの前詰終端に食い込んでいる場合は調整（※手前の[開始位置＋ヒント数＋白マス]分まで対象の開始位置を離す）
        int tempAt = pCluePrev->getSolidStartAt() + pCluePrev->getVal() + 1;
        int checkNum = tempAt - pClue->getSolidStartAt();
        if( checkNum > 0 ){
            pClue->addSolidStartAt( checkNum );
            pClue->addSolidNum( -checkNum );
            
            // マス変化数更新＆整合性判定＆ログ登録
            INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_01_BEFORE_AND_AFTER );
        }
    }
    
    //-------------------------------
    // 後方のブロックの終了位置を確認
    //-------------------------------
    for( int i=sizeClue-2; i>=0; i-- ){
        CLogicClue* pClueNext = &arrClue[i+1];
        CLogicClue* pClue = &arrClue[i];
        
        // 後ろのブロックの後詰開始位置に食い込んでいる場合は調整（※後ろの[終了位置ー最小黒マスー白マス]分まで対象の終了位置を離す）
        int tempAt = pClueNext->getSolidEndAt() - pClueNext->getVal() - 1;
        int checkNum = pClue->getSolidEndAt() - tempAt;
        if( checkNum > 0 ){
            pClue->addSolidNum( -checkNum );
            
            // マス変化数更新＆整合性判定＆ログ登録
            INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_01_BEFORE_AND_AFTER );
        }
    }
    
    CLUE_UPDATE_END;
}

//------------------------------------------------------------------------------------
// [02]: トリミング（※前後の無効マスを除外）
//------------------------------------------------------------------------------------
// ・白マスの除外
// ・ヒントに対して小さい領域の除外（※ヒントの黒マスが入らない領域）
// ・ヒントよりも大きい黒マスの除外（※ヒントより大きい黒マスは別のヒントの領域）
//------------------------------------------------------------------------------------
int CLogicClueUpdater::_fixSolidBlocksForTrim( LOGIC_CLUE_UPDATER_PARAM_DEF ){
    CLUE_UPDATE_START;
    
    for( int i=0; i<sizeClue; i++ ){
        CLogicClue* pClue = &arrClue[i];
        int startAt = pClue->getSolidStartAt();
        int endAt = pClue->getSolidEndAt();
        int val = arrClue[i].getVal();
        
        int ofs, len;
        
        //--------------------------
        // トリミング：前方
        //--------------------------
        // 白を飛ばす
        while( startAt < endAt && pGrid->checkSolidWhite( isRow, targetIndex, startAt, solidOfs ) ){
            startAt++;
        }

        // 黒が収まらなければ飛ばす（※ヒント分の黒マスを収められない領域は対象外）
        len = 1;    // 事前の処理で[len=0]のマスが白以外だとわかっているので１開始
        while( (startAt+len) <= endAt && !pGrid->checkSolidWhite( isRow, targetIndex, startAt+len, solidOfs ) ){
            len++;
        }

        if( len < val ){
            startAt += len;
        }

        // ヒントが重複して存在できない領域を調べ、黒マスの連続サイズがヒントを超過していたら飛ばす（※ヒントより大きな黒マスは対象外）
        ofs = -1;
        for( int j=startAt; j<=endAt; j++ ){
            if( pGrid->checkSolidBlack( isRow, targetIndex, j, solidOfs ) ){
                ofs = j - startAt;
                break;
            }
        }
        
        if( ofs >= 0 && ofs <= val && (startAt+ofs) <= endAt ){
            len = 1;    // 事前の処理で[len=0]のマスが黒だとわかっているので１開始
            while( (startAt+ofs+len) <= endAt && pGrid->checkSolidBlack( isRow, targetIndex, startAt+ofs+len, solidOfs ) ){
                len++;
            }
            
            if( len > val ){
                startAt += ofs + len + 1;   // 別のヒントの黒マスなので白マス分＋１
            }
        }

        //--------------------------
        // トリミング：後方
        //--------------------------
        // 白を飛ばす
        while( endAt > startAt && pGrid->checkSolidWhite( isRow, targetIndex, endAt, solidOfs ) ){
            endAt--;
        }

        // 黒が収まらなければ飛ばす（※ヒント分の黒マスを収められない領域は対象外）
        len = 1;    // 事前の処理で[len=0]のマスが白以外だとわかっているので１開始
        while( (endAt-len) >= startAt && !pGrid->checkSolidWhite( isRow, targetIndex, endAt-len, solidOfs ) ){
            len++;
        }
        
        if( len < val ){
            endAt -= len;
        }

        // ヒントが重複して存在できない領域を調べ、黒マスの連続サイズがヒントを超過していたら飛ばす（※ヒントより大きな黒マスは対象外）
        ofs = -1;
        for( int j=endAt; j>=startAt; j-- ){
            if( pGrid->checkSolidBlack( isRow, targetIndex, j, solidOfs ) ){
                ofs = endAt - j;
                break;
            }
        }
        
        if( ofs >= 0 && ofs <= val && (endAt-ofs) >= startAt ){
            len = 1;    // 事前の処理で[len=0]のマスが黒だとわかっているので１開始
            while( (endAt-ofs-len) >= startAt && pGrid->checkSolidBlack( isRow, targetIndex, endAt-ofs-len, solidOfs ) ){
                len++;
            }
            
            if( len > val ){
                endAt -= ofs + len + 1; // 別のヒントの黒マスなので白マス分＋１
            }
        }

        //------------------------------
        // 差分があれば調整
        //------------------------------
        int num = endAt - startAt + 1;
        if( pClue->getSolidStartAt() != startAt || pClue->getSolidNum() != num ){
            pClue->setSolidBlock( startAt, num );
            
            // マス変化数更新＆整合性判定＆ログ登録
            INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_02_TRIM );
        }
    }

    CLUE_UPDATE_END;
}


//-------------------------------------------------------------------------------------------
// [03]: 黒マスの分布による調整（※複数ヒントが対象）
//-------------------------------------------------------------------------------------------
// ・領域内の連続した黒マスに対応するヒントを検出し、そのヒントをピボットにしてヒント配列を割する（※再帰呼び出し）
//-------------------------------------------------------------------------------------------
int CLogicClueUpdater::_fixSolidBlocksForBlackDistribution( LOGIC_CLUE_UPDATER_PARAM_DEF ){
    CLUE_UPDATE_START;
    
    //-------------------------------
    // ヒントの数が１つ以下であればスキップ
    //-------------------------------
    CLUE_UPDATE_SKIP_IF_SINGLE_CLUE;

    //----------------------------
    // 連続する黒マスの検出
    //----------------------------
    int numBlock = 0;
    int blockStartAt[LOGIC_CLUE_WH_MAX];
    int blackNumInBlock[LOGIC_CLUE_WH_MAX];
    
    bool isInBlock = false;
    for( int i=0; i<solidNum; i++ ){
        if( pGrid->checkSolidBlack( isRow, targetIndex, i, solidOfs ) ){
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
    
    // ブロックがなければ返す（正常終了）
    if( numBlock <= 0 ){
        CLUE_UPDATE_END;
    }

    //-----------------------------------------------------------------
    // ブロックが複数ある場合は独立性の確認
    //（※白マスで分割されている／黒マスでつなげた場合に矛盾する場合は独立性がある）
    //-----------------------------------------------------------------
    if( numBlock > 1 ){
        // 最大ヒントの抽出
        int clueValMax = 1;
        for( int i=0; i<sizeClue; i++ ){
            if( clueValMax < arrClue[i].getVal() ){
                clueValMax = arrClue[i].getVal();
            }
        }
        
        // 各ブロックの確認
        for( int i=1; i<numBlock; i++ ){
            int checkStart = blockStartAt[i-1] + blackNumInBlock[i-1];
            int checkEnd = blockStartAt[i] - 1;

            // ブロックが繋がるか？
            bool isConnectable = true;
            while( checkStart <= checkEnd ){
                if( pGrid->checkSolidWhite( isRow, targetIndex, checkStart, solidOfs )  ){
                    isConnectable = false;
                    break;
                }
                checkStart++;
            }
            
            // 繋がるのであれば最大ヒントを超えているか？
            if( isConnectable ){
                int len = blockStartAt[i] + blackNumInBlock[i] - blockStartAt[i-1];

                // 繋がるのであれば独立性が確保できないので返す（正常終了）
                if( len <= clueValMax ){
                    CLUE_UPDATE_END;
                }
            }
        }
    }

    //---------------------------------------------------
    // ヒントとブロックの割り当て確認
    //---------------------------------------------------
    int numCandForBlock = 0;
    int candBlockAt[LOGIC_CLUE_WH_MAX];

    int numCandForClue = 0;
    int candClueAt[LOGIC_CLUE_WH_MAX];

    // ヒントの数とブロックが一致していたら、１対１で割り当てられる
    if( sizeClue == numBlock ){
        for( int i=0; i<sizeClue; i++ ){
            candBlockAt[numCandForBlock++] = i;
            candClueAt[numCandForClue++] = i;
        }
    }
    // ヒントの数とブロックが一致していなければ、一番大きなブロック以上のヒントを検出
    else{
        // 一番大きなブロックサイズの検出
        int numMax = blackNumInBlock[0];
        for( int i=1; i<numBlock; i++ ){
            if( numMax < blackNumInBlock[i] ){
                numMax = blackNumInBlock[i];
            }
        }
        
        // 一番大きなブロックを抽出
        for( int i=0; i<numBlock; i++ ){
            if( blackNumInBlock[i] >= numMax ){
                candBlockAt[numCandForBlock++] = i;
            }
        }

        // 一番大きなブロック以上のヒントを抽出
        for( int i=0; i<sizeClue; i++ ){
            if( arrClue[i].getVal() >= numMax ){
                candClueAt[numCandForClue++] = i;
            }
        }
    }

    // ブロックとヒントの候補が一致しなければ返す（正常終了）
    //（※黒マスとヒントの並びを特定できない場合は分割できない）
    if( numCandForBlock != numCandForClue ){
        CLUE_UPDATE_END;
    }

    //------------------------------------------------------------
    // ここまできたら黒マスのブロックに１対１で割り当てられるヒントが存在する
    //------------------------------------------------------------
    // 差分判定用に情報の保持
    int oldStartSolidAt[LOGIC_CLUE_WH_MAX];
    int oldStartNum[LOGIC_CLUE_WH_MAX];
    for( int i=0; i<sizeClue; i++ ){
        oldStartSolidAt[i] = arrClue[i].getSolidStartAt();
        oldStartNum[i] = arrClue[i].getSolidNum();
    }

    // 対象の黒マスブロックをピボットとして再帰的にサブ処理を呼ぶ
    int lastSolidAt = 0;
    int remainAt = -1;
    int candAt = 0;
    for( int i=0; i<sizeClue; i++ ){
        // 候補と一致したら分割
        if( candClueAt[candAt] == i ){
            // ピボット前のヒント
            if( remainAt >= 0 ){
                // 分割情報
                int sizeTempClue = i - remainAt;
                CLogicClue* arrTempClue = &arrClue[remainAt];
                int solidTempStart = lastSolidAt;
                int solidTempEnd = blockStartAt[candBlockAt[candAt]] - (1+1);    // 白マス分＋１
                int solidTempOfs = solidOfs + solidTempStart;
                int solidTempNum = solidTempEnd - solidTempStart + 1;

                // 分割した情報で再起呼び出し
                bool isTempImcompatible = true;
                numChanged += _fixSolidBlocksForBlackDistributionSub( pGrid, arrTempClue, sizeTempClue, &isTempImcompatible,
                                                               isRow, targetIndex, solidTempOfs, solidTempNum,
                                                               solidTempStart, solidTempEnd );
                //------------------
                // 矛盾があれば矛盾終了
                //------------------
                if( isTempImcompatible ){
                    CLUE_UPDATE_IMCOMPATIBLE_END;
                }
                
                // 次のマスの開始位置（※前の黒ブロックの終端）
                if( candBlockAt[candAt] > 0 ){
                    int targetAt = candBlockAt[candAt] - 1;
                    lastSolidAt = blockStartAt[targetAt] + blackNumInBlock[targetAt] + 1;    // 白マス分＋１
                }
            }
            
            // ピボット自身の登録
            {
                // 分割情報
                int sizeTempClue = 1;
                CLogicClue* arrTempClue = &arrClue[i];
                int solidTempStart = lastSolidAt;

                int solidTempEnd;
                if( candBlockAt[candAt] < (numBlock-1) ){
                    solidTempEnd = blockStartAt[candBlockAt[candAt]+1] - (1+1);    // 白マス分＋１
                }else{
                    solidTempEnd = solidNum - 1;
                }

                int solidTempOfs = solidOfs + solidTempStart;
                int solidTempNum = solidTempEnd - solidTempStart + 1;

                // 分割した情報で再起呼び出し
                bool isTempImcompatible = true;
                numChanged += _fixSolidBlocksForBlackDistributionSub( pGrid, arrTempClue, sizeTempClue, &isTempImcompatible,
                                                               isRow, targetIndex, solidTempOfs, solidTempNum,
                                                               solidTempStart, solidTempEnd );
                //------------------
                // 矛盾があれば矛盾終了
                //------------------
                if( isTempImcompatible ){
                    CLUE_UPDATE_IMCOMPATIBLE_END;
                }

                lastSolidAt = blockStartAt[candBlockAt[candAt]] + blackNumInBlock[candBlockAt[candAt]] + 1;    // 白マス分＋１
            }

            // 候補を次へ
            candAt++;
            remainAt = -1;
            
            // 候補がなくなったら抜ける（※右側にヒントが残っていたら[remainAt]を設定しておく）
            if( candAt >= numCandForClue ){
                if( i < (sizeClue-1) ){
                    remainAt = i + 1;
                }
                break;
            }
        }else if( remainAt < 0 ){
            remainAt = i;
        }
    }
    
    // 最終分割点以降のヒントがあれば
    if( remainAt >= 0 ){
        // 分割情報
        int sizeTempClue = sizeClue - remainAt;
        CLogicClue* arrTempClue = &arrClue[remainAt];
        int solidTempStart = lastSolidAt;
        int solidTempEnd = solidNum - 1;
        int solidTempOfs = solidOfs + solidTempStart;
        int solidTempNum = solidTempEnd - solidTempStart + 1;

        // 分割した情報で再起呼び出し
        bool isTempImcompatible = true;
        numChanged += _fixSolidBlocksForBlackDistributionSub( pGrid, arrTempClue, sizeTempClue, &isTempImcompatible,
                                                      isRow, targetIndex, solidTempOfs, solidTempNum,
                                                      solidTempStart, solidTempEnd );
        //------------------
        // 矛盾があれば矛盾終了
        //------------------
        if( isTempImcompatible ){
            CLUE_UPDATE_IMCOMPATIBLE_END;
        }
    }

    //---------------------------------------------------
    // スコアの加算と整合性判定（※処理まえの値と差分がでていたら）
    //---------------------------------------------------
    for( int i=0; i<sizeClue; i++ ){
        CLogicClue* pClue = &arrClue[i];
        
        // 差分がでていたらスコア加算
        if( oldStartSolidAt[i] != pClue->getSolidStartAt() || oldStartNum[i] != pClue->getSolidNum() ){
            INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_03_BLACK_DISTRIBUTION );
        }
    }
    
    CLUE_UPDATE_END;
}

// [03]: 黒マスの分布による調整の再帰呼び出し窓口
int CLogicClueUpdater::_fixSolidBlocksForBlackDistributionSub( LOGIC_CLUE_UPDATER_PARAM_DEF, int solidStart, int solidEnd ){
    CLUE_UPDATE_START;

    //-----------------------------------------------------
    // 分割によるヒントの範囲調整（※[solidStart]分開始位置をずらす）
    //-----------------------------------------------------
    for( int i=0; i<sizeClue; i++ ){
        CLogicClue* pClue = &arrClue[i];
        
        // 分割位置より前方であれば手前に削る
        if( solidStart > pClue->getSolidStartAt() ){
            int num = pClue->getSolidNum() - (solidStart-pClue->getSolidStartAt());
            pClue->setSolidBlock( 0, num );
        }
        // 分割位置よりも後方であればその分ずらす
        else{
            pClue->addSolidStartAt( -solidStart );
        }

        // 終端がはみ出ている場合
        int endCheck = pClue->getSolidEndAt() - (solidEnd-solidStart);
        if( endCheck > 0 ){
            pClue->addSolidNum( -endCheck );
        }

        //---------------------
        // 不整合がでたら盾終了
        //---------------------
        if( ! pClue->checkVerification( solidNum ) ){
            CLUE_UPDATE_IMCOMPATIBLE_END;
        }
    }
    
    //---------------------------------------
    // 分割した情報で再起呼び出し
    //---------------------------------------
    numChanged += _fixSolidBlocksForBlackDistribution( pGrid, arrClue, sizeClue, isImcompatible, isRow, targetIndex, solidOfs, solidNum );

    //---------------------------------------
    // 範囲調整の戻し（※ずらした[solidStart]を戻す
    //---------------------------------------
    for( int i=0; i<sizeClue; i++ ){
        arrClue[i].addSolidStartAt( solidStart );
    }
    
    CLUE_UPDATE_END;
}

//------------------------------------------------------------------------------------
// [04]: 黒マスの数がヒントの範囲内で確定していたら調整
//------------------------------------------------------------------------------------
// ・一番大きな値のヒントが１つ存在する場合、その数に等しい黒マスがあれば確定される
//------------------------------------------------------------------------------------
int CLogicClueUpdater::_fixSolidBlocksForBlackNum( LOGIC_CLUE_UPDATER_PARAM_DEF ){
    CLUE_UPDATE_START;
    
    //----------------------------
    // 連続する黒マスの検出
    //----------------------------
    int numBlock = 0;
    int blockStartAt[LOGIC_CLUE_WH_MAX];
    int blackNumInBlock[LOGIC_CLUE_WH_MAX];
    
    bool isInBlock = false;
    for( int i=0; i<solidNum; i++ ){
        if( pGrid->checkSolidBlack( isRow, targetIndex, i, solidOfs ) ){
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
    
    //-------------------------------------------
    // 連続する黒マスに対して対応するヒントがあるか確認
    //-------------------------------------------
    for( int i=0; i<numBlock; i++ ){
        int startAt = blockStartAt[i];
        int endAt = startAt + blackNumInBlock[i] - 1;
        
        // 対象の黒マスを含むブロックの検出
        int numCand = 0;
        int arrCandClueId[LOGIC_CLUE_WH_MAX];
        int arrCandClueVal[LOGIC_CLUE_WH_MAX];
        for( int j=0; j<sizeClue; j++ ){
            CLogicClue* pClue = &arrClue[j];
            if( pClue->isSolidContained( startAt ) && pClue->isSolidContained( endAt ) ){
                arrCandClueId[numCand] = j;
                arrCandClueVal[numCand] = pClue->getVal();
                numCand++;
            }
        }
        
        // 最も大きなヒントが１つだけ存在するか？
        if( numCand > 0 ){
            int maxTarget = 0;
            int maxNum = 1;
            for( int j=1; j<numCand; j++ ){
                // 大きければ対象を更新
                if( arrCandClueVal[maxTarget] < arrCandClueVal[j] ){
                    maxTarget = j;
                    maxNum = 1;
                }
                // 等しい場合は数を更新
                else if( arrCandClueVal[maxTarget] == arrCandClueVal[j] ){
                    maxNum++;
                }
            }
            
            // 最も大きなヒントが１つだけ存在し、マスの数と合致した場合は確定
            if( maxNum == 1 ){
                if( arrCandClueVal[maxTarget] == blackNumInBlock[i] ){
                    int clueTarget = arrCandClueId[maxTarget];
                    CLogicClue* pClue = &arrClue[clueTarget];
                    // 差分があれば設定
                    if( pClue->getSolidStartAt() != blockStartAt[i] || pClue->getSolidNum() != blackNumInBlock[i] ){
                        pClue->setSolidBlock( blockStartAt[i], blackNumInBlock[i] );
                        
                        // マス変化数更新＆整合性判定＆ログ登録
                        INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_04_BALCK_NUM );
                    }
                }
            }
        }
    }
    
    CLUE_UPDATE_END;
}

//------------------------------------------------------------------------------------
// [05]: 白マスで分断された黒マスを含むブロックの数がヒントと等しければ調整
//------------------------------------------------------------------------------------
// ・白マスで分割された黒マスを含む領域がヒントの数と等しければ、ヒントのそれぞれに対して領域が確定される
//------------------------------------------------------------------------------------
int CLogicClueUpdater::_fixSolidBlocksForSeparatedBlock( LOGIC_CLUE_UPDATER_PARAM_DEF ){
    CLUE_UPDATE_START;
    
    //----------------------------------------------------
    // 白マスに分断されるマスブロックの抽出（※黒マスを含むものだけ）
    //----------------------------------------------------
    int numBlock = 0;
    int blockStartAt[LOGIC_CLUE_WH_MAX];
    int blockNum[LOGIC_CLUE_WH_MAX];
    
    bool isInBlock = false;
    bool isBlackContained = false;
    for( int i=0; i<solidNum; i++ ){
        if( pGrid->checkSolidWhite( isRow, targetIndex, i, solidOfs ) ){
            if( isInBlock ){
                isInBlock = false;
                if( isBlackContained ){
                    numBlock++;
                }
            }
        }else{
            if( ! isInBlock ){
                isInBlock = true;
                isBlackContained = false;
                blockStartAt[numBlock] = i;
                blockNum[numBlock] = 0;
            }
            
            blockNum[numBlock]++;
            
            if( pGrid->checkSolidBlack( isRow, targetIndex, i, solidOfs ) ){
                isBlackContained = true;
            }
        }
    }
    
    if( isInBlock ){
        if( isBlackContained ){
            numBlock++;
        }
    }
    
    //---------------------------------------
    // ブロック数がヒント数を超過したら矛盾終了
    //---------------------------------------
    if( numBlock > sizeClue ){
        CLUE_UPDATE_IMCOMPATIBLE_END;
    }
    
    //---------------------------------------
    // ブロック数とヒント数が一致していたら範囲の調整
    //---------------------------------------
    if( numBlock == sizeClue ){
        for( int i=0; i<sizeClue; i++ ){
            CLogicClue* pClue = &arrClue[i];
            int startAt = blockStartAt[i];
            int endAt = startAt + blockNum[i] - 1;
            
            // 開始位置の確認
            int numCheck = startAt - pClue->getSolidStartAt();
            if( numCheck > 0 ){
                pClue->addSolidStartAt( numCheck );
                pClue->addSolidNum( -numCheck );
                
                // マス変化数更新＆整合性判定＆ログ登録
                INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_05_SEPARATED_BLOCK );
            }
            
            // 終了位置の確認
            numCheck = pClue->getSolidEndAt() - endAt;
            if( numCheck > 0 ){
                pClue->addSolidNum( -numCheck );
                
                // マス変化数更新＆整合性判定＆ログ登録
                INCREMENT_NUM_CHANGED_THEN_CHECK_VERIFICATION_AND_ADD_LOG( pClue, eLOGIC_CLUE_UPDATE_05_SEPARATED_BLOCK );
            }
        }
    }
    
    CLUE_UPDATE_END;
}
