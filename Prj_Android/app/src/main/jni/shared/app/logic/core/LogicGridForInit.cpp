/*+----------------------------------------------------------------+
  |	Title:		LogicGridForInit.cpp [共通環境]
  |	Comment:	ロジック（コア）：グリッド（盤面全体）：初期化関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicGrid.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 読み込みログの表示
#define DUMP_INIT_LOG

//-----------------------------------------------------------------
// データ文字列サイズ
//-----------------------------------------------------------------
// 想定最大行数：サイズ指定１行 ＋ 最大行数 ＋　最大列数　＋　最大行数
#define SRC_STR_LOAD_LINE_MAX   (1+3*LOGIC_SOLID_WH_MAX)

// データ文字列：１行の想定最大文字数：（2文字 ＋ カンマ） × 最大ヒント数
#define SRC_STR_SCAN_LINE_LEN   ((2+1)*LOGIC_CLUE_WH_MAX)

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
//-------------------
// エラー解除
//-------------------
void CLogicGrid::clearError( void ){
    for( int i=0; i<LOGIC_SOLID_WH_MAX; i++ ){
        for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
            m_oArrArrClueForRow[i][j].setError( false );
            m_oArrArrClueForColumn[i][j].setError( false );
        }
    }
}

//---------------------------------------------------------------------
// 初期化（※文字列から）
//---------------------------------------------------------------------
// 下記のルールでデータが指定されるものとする
// A.盤面のサイズを指定
//    [タテのヒント枠数],[ヨコのヒント枠数]
// B.タテのヒント情報、ヨコのヒント情報を指定
//    [１つ目の数字],[２つ目の数字],[...],[最後の数字]（タテのヒント枠数分）
//    [１つ目の数字],[２つ目の数字],[...],[最後の数字]（ヨコのヒント枠数分）
// C.解答情報を指定（任意）（※[0]で白マス、[1]で黒マス）
//    [１マス目の答え],[２マス目の答え],[...],[最後のマスの答え]（縦のサイズ分）
//---------------------------------------------------------------------
bool CLogicGrid::init( const char* pSrcStr, char* bufError ){
#ifdef DUMP_INIT_LOG
    LOGD( "@ CLogicGrid::init: pSrcStr=\n%s\n", pSrcStr );
#endif
    
    // 一旦クリア
    clear();

    // 作業バッファ準備（※とりあえずベタでもつ：[50x50]のパズルを最大として[15KB]弱ほど）
    char cArrArrSrc[SRC_STR_LOAD_LINE_MAX][SRC_STR_SCAN_LINE_LEN];
    for( int i=0; i<SRC_STR_LOAD_LINE_MAX; i++ ){
        cArrArrSrc[i][0] = '\0';
    }

    //---------------------------------------------------------------------
    // ソースから処理対象の文字だけを取り出す（※コメントや改行等、余分な文字の読み飛ばし）
    //---------------------------------------------------------------------
    char c;
    int line = 0;
    int at = 0;
    while( *pSrcStr != '\0' ){
        while( (c=*pSrcStr) != '\0' ){
            // 改行がきたら抜ける
            if( c == '\n' || c == '\r' ){
                break;
            }
            
            // [数字]と[カンマ]だけを抽出
            if( (c >= '0' && c <= '9') || c == ',' ){
                cArrArrSrc[line][at++] = c;
            }
            
            pSrcStr++;
        }
        
        // 読み込めていたら行扱い
        if( at > 0 ){
            cArrArrSrc[line][at] = '\0';    // 文字列終端
            
            // 次の行へ
            line++;
            at = 0;
        }
        
        // 改行を読み飛ばす
        while( (c=*pSrcStr) != '\0' ){
            if( c != '\n' && c != '\r' ){
                break;
            }
            pSrcStr++;
        }
    }
    
#ifdef DUMP_INIT_LOG
    for( int i=0; i<line; i++ ){
        LOGD( "@ CLogicGrid::init: cArrArrSrc[%d]: %s\n", i, cArrArrSrc[i] );
    }
#endif
    
    //----------------------
    // ヨコ／タテのサイズを確認
    //----------------------
    char* buf;
    int logicW, logicH;
    const char* pLogicW = NULL;
    const char* pLogicH = NULL;
    
    // ヨコの開始位置は先頭
    pLogicW = &cArrArrSrc[0][0];
    
    // タテの開始位置の検出（※１つ目のピリオドの次）
    pLogicH = &cArrArrSrc[0][0];
    while( *pLogicH != '\0' ){
        if( *pLogicH++ == ',' ){
            break;
        }
    }
    
    // ピリオドを終端文字へ（※これで先にとりだしたポインタがそれぞれ文字列となる）
    buf = &cArrArrSrc[0][0];
    while( *buf != '\0' ){
        if( *buf == ',' ){
            *buf = '\0';
        }
        buf++;
    }

    int64 temp = 0;
    CMath::StrToLong( pLogicW, &temp );
    logicW = (int)temp;
    
    temp = 0;
    CMath::StrToLong( pLogicH, &temp );
    logicH = (int)temp;
    
#ifdef DUMP_INIT_LOG
    LOGD( "@ CLogicGrid::init: logicW=%d, logicH=%d\n", logicW, logicH );
#endif
    
    // 用心
    if( logicW <= 0 || logicW > LOGIC_SOLID_WH_MAX || logicH <= 0 || logicH > LOGIC_SOLID_WH_MAX  ){
        LOGE( "@ CLogicGrid::init: INVALID LOGIC SIZE: w=%d, h=%d\n", logicW, logicH );
        if( bufError != NULL ){
            sprintf( bufError, "ロジックのサイズが不正です：%dx%d", logicW, logicH );
        }
        return( false );
    }
    
    //------------------------------------
    // ここまできたらデータの設定を開始
    //------------------------------------
    m_nRow = logicW;
    m_nColumn = logicH;
    
    //---------------------
    // ヒント：タテの設定
    //---------------------
    for( int i=0; i<logicW; i++ ){
        int numClue;
        bool isNumStart;
        const char* pStrNum;
        
        // 数字の数を検出
        numClue = 0;
        isNumStart = false;
        buf = &cArrArrSrc[1+i][0];
        while( (c=*buf) != '\0' ){
            if( c >= '0' && c <= '9' ){
                isNumStart = true;
            }else{
                if( isNumStart ){
                    numClue++;
                }
                isNumStart = false;
            }
            buf++;
        }
        if( isNumStart ){
            numClue++;
        }
        
        // ヒントのサイズを設定
        m_nArrNumClueForRow[i] = numClue;
        
        // 数字の検出
        numClue = 0;
        pStrNum = NULL;
        isNumStart = false;
        buf = &cArrArrSrc[1+i][0];
        while( (c=*buf) != '\0' ){
            if( c >= '0' && c <= '9' ){
                if( ! isNumStart ){
                    pStrNum = buf;
                }
                isNumStart = true;
            }else{
                if( isNumStart ){
                    *buf = '\0';
                    
                    int64 temp = 0;
                    CMath::StrToLong( pStrNum, &temp );
                    m_oArrArrClueForRow[i][numClue].setVal( (int)temp );
                    numClue++;
                }
                isNumStart = false;
            }
            buf++;
        }
        
        if( isNumStart ){
            int64 temp = 0;
            CMath::StrToLong( pStrNum, &temp );
            m_oArrArrClueForRow[i][numClue].setVal( (int)temp );
        }
    }
    
    //---------------------
    // ヒント：ヨコの設定
    //---------------------
    for( int i=0; i<logicH; i++ ){
        int numClue;
        bool isNumStart;
        const char* pStrNum;
        
        // 数字の数を検出
        numClue = 0;
        isNumStart = false;
        buf = &cArrArrSrc[1+logicW+i][0];
        while( (c=*buf) != '\0' ){
            if( c >= '0' && c <= '9' ){
                isNumStart = true;
            }else{
                if( isNumStart ){
                    numClue++;
                }
                isNumStart = false;
            }
            buf++;
        }
        if( isNumStart ){
            numClue++;
        }
        
        // ヒントのサイズを設定
        m_nArrNumClueForColumn[i] = numClue;
        
        // 数字の検出
        numClue = 0;
        pStrNum = NULL;
        isNumStart = false;
        buf = &cArrArrSrc[1+logicW+i][0];
        while( (c=*buf) != '\0' ){
            if( c >= '0' && c <= '9' ){
                if( ! isNumStart ){
                    pStrNum = buf;
                }
                isNumStart = true;
            }else{
                if( isNumStart ){
                    *buf = '\0';
                    
                    int64 temp = 0;
                    CMath::StrToLong( pStrNum, &temp );
                    m_oArrArrClueForColumn[i][numClue].setVal( (int)temp );
                    numClue++;
                }
                isNumStart = false;
            }
            buf++;
        }
        
        if( isNumStart ){
            int64 temp = 0;
            CMath::StrToLong( pStrNum, &temp );
            m_oArrArrClueForColumn[i][numClue].setVal( (int)temp );
        }
    }

    // 整合性確認結果を返す
    return( checkVerificationForInit( bufError ) );
}

//-----------------------
// 初期化（※サイズ確認用）
//-----------------------
bool CLogicGrid::initForSize( int col, int row ){
    clear();
    
    m_nColumn = col;
    m_nRow = row;
    
    for( int i=0; i<m_nColumn; i++ ){
        m_nArrNumClueForColumn[i] = (m_nColumn+1)/2;
    }

    for( int i=0; i<m_nRow; i++ ){
        m_nArrNumClueForRow[i] = (m_nRow+1)/2;
    }

    // 整合性の確認は無視
    return( true );
}

//-----------------------
// 調整（※サイズ確認用）
//-----------------------
void CLogicGrid::fixForSize( void ){
    for( int i=0; i<m_nColumn; i++ ){
        m_nArrNumClueForColumn[i] = 1;
    }

    for( int i=0; i<m_nRow; i++ ){
        m_nArrNumClueForRow[i] = 1;
    }
}

//------------------------------------
// ロジックの整合性確認（※設定データの確認）
//------------------------------------
bool CLogicGrid::checkVerificationForInit( char* bufError ){
    bool isError = false;
    
    //--------------------------------------
    // サイズ確認
    //--------------------------------------
    // 行
    if( m_nColumn <= 0 || m_nColumn > LOGIC_SOLID_WH_MAX ){
        LOGE( "@ INIT VERIFICATION: INVALID: column=%d\n", m_nColumn );
        if( bufError != NULL ){
            sprintf( bufError, "・エラー：ヨコ（行）のサイズが不正です：%d\n", m_nColumn );
            bufError = &bufError[util_str_len( bufError )];
        }
        isError = true;
    }

    // 列
    if( m_nRow <= 0 || m_nRow > LOGIC_SOLID_WH_MAX ){
        LOGE( "@ INIT VERIFICATION: INVALID: row=%d\n", m_nRow );
        if( bufError != NULL ){
            sprintf( bufError, "・エラー：タテ（列）のサイズが不正です：%d\n", m_nRow );
            bufError = &bufError[util_str_len( bufError )];
        }
        isError = true;
    }
    
    //--------------------------------------
    // ヒント確認：ヨコ
    //--------------------------------------
    int clueTotalForColumn = 0;
    
    for( int i=0; i<m_nColumn; i++ ){
        // ヒントの数確認
        int num = m_nArrNumClueForColumn[i];
        if( num <= 0 || num > (m_nRow+1)/2 ){
            LOGE( "@ INIT VERIFICATION: INVALID: column clue num[%d]=%d\n", i, num );
            if( bufError != NULL ){
                sprintf( bufError, "・エラー：ヨコのヒント[%d行目]が不正です\n", i+1 );
                bufError = &bufError[util_str_len( bufError )];
                
                // エラー指定
                for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
                    m_oArrArrClueForColumn[i][j].setError( true );
                }
            }
            isError = true;
            continue;
        }
        
        // 例外（※０の指定は要素１でのみ受け付ける）
        if( num == 1 && m_oArrArrClueForColumn[i][0].getVal() == 0 ){
            continue;
        }
        
        // ヒントの合計確認
        int temp = 0;
        for( int j=0; j<num; j++ ){
            int val = m_oArrArrClueForColumn[i][j].getVal();
            if( val <= 0 || val > m_nRow ){
                LOGE( "@ INIT VERIFICATION: INVALID: column clue val[%d][%d]=%d\n", i, j, val );
                if( bufError != NULL ){
                    sprintf( bufError, "・エラー：ヨコのヒント[%d行目の%d番目]が不正です\n", i+1, j+1 );
                    bufError = &bufError[util_str_len( bufError )];
                    
                    // エラー指定
                    m_oArrArrClueForColumn[i][j].setError( true );
                }
                isError = true;
                continue;
            }
            
            clueTotalForColumn += val;
            
            temp += val;
            if( j > 0 ){ temp += 1; }
        }
        
        if( temp > m_nRow ){
            LOGE( "@ INIT VERIFICATION: INVALID: column clue total[%d]=%d\n", i, temp );
            if( bufError != NULL ){
                sprintf( bufError, "・エラー：ヨコのヒント[%d行目]の合計が大きすぎます\n", i+1 );
                bufError = &bufError[util_str_len( bufError )];
                
                // エラー指定
                for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
                    m_oArrArrClueForColumn[i][j].setError( true );
                }
            }
            isError = true;
        }
    }

    //--------------------------------------
    // ヒント確認：タテ
    //--------------------------------------
    int clueTotalForRow = 0;
    
    for( int i=0; i<m_nRow; i++ ){
        // ヒントの数確認
        int num = m_nArrNumClueForRow[i];
        if( num <= 0 || num > (m_nColumn+1)/2 ){
            LOGE( "@ INIT VERIFICATION: INVALID: row clue num[%d]=%d\n", i, num );
            if( bufError != NULL ){
                sprintf( bufError, "・エラー：タテのヒント[%d列目]が不正です\n", i+1 );
                bufError = &bufError[util_str_len( bufError )];
                
                // エラー指定
                for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
                    m_oArrArrClueForRow[i][j].setError( true );
                }
            }
            isError = true;
            continue;
        }
        
        // 例外（※０の指定は要素１でのみ受け付ける）
        if( num == 1 && m_oArrArrClueForRow[i][0].getVal() == 0 ){
            continue;
        }
        
        // ヒントの合計確認
        int temp = 0;
        for( int j=0; j<num; j++ ){
            int val = m_oArrArrClueForRow[i][j].getVal();
            if( val <= 0 || val > m_nColumn ){
                LOGE( "@ INIT VERIFICATION: INVALID: row clue val[%d][%d]=%d\n", i, j, val );
                if( bufError != NULL ){
                    sprintf( bufError, "・エラー：タテのヒント[%d列目の%d番目]が不正です\n", i+1, j+1 );
                    bufError = &bufError[util_str_len( bufError )];
                }
                // エラー指定
                m_oArrArrClueForRow[i][j].setError( true );
                isError = true;
                continue;
            }
            
            clueTotalForRow += val;
            
            temp += val;
            if( j > 0 ){ temp += 1; }
        }
        
        if( temp > m_nColumn ){
            LOGE( "@ INIT VERIFICATION: INVALID: row clue total[%d]=%d\n", i, temp );
            if( bufError != NULL ){
                sprintf( bufError, "・エラー：タテのヒント[%d列目]の合計が大きすぎます\n", i+1 );
                bufError = &bufError[util_str_len( bufError )];
            }
            // エラー指定
            for( int j=0; j<LOGIC_CLUE_WH_MAX; j++ ){
                m_oArrArrClueForRow[i][j].setError( true );
            }
            isError = true;
        }
    }

    //---------------------------------
    // 黒マス合計の確認
    //---------------------------------
    if( clueTotalForColumn != clueTotalForRow ){
        LOGE( "@ INIT VERIFICATION: INVALID: total solid num: column(%d) != row(%d)\n", clueTotalForColumn, clueTotalForRow );
        if( bufError != NULL ){
            sprintf( bufError, "・エラー：タテのヒントの合計（%d）とヨコのヒントの合計（%d）が一致しません：\n", clueTotalForRow, clueTotalForColumn );
            bufError = &bufError[util_str_len( bufError )];
        }
        isError = true;
    }
    
    // 結果のダンプ
    if( isError ){
        LOGE( "@ INIT VERIFICATION: ERROR FOUND\n" );
    }else{
        LOGD( "@ INIT VERIFICATION: NO ERROR\n" );
    }

    return( ! isError );
}
