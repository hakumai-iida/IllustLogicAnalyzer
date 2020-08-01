/*+----------------------------------------------------------------+
  |	Title:		Logic.cpp [共通環境]
  |	Comment:	ロジック：コアデータ[CLogicGrid]の管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Logic.hpp"
#include "LogicClueUpdater.hpp"
#include "LogicAnalyzer.hpp"

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
//------------------------
// コンストラクタ
//------------------------
CLogic::CLogic( void ){
    clear();
}

//------------------------
// デストラクタ
//------------------------
CLogic::~CLogic( void ){
}

//------------------------
// クリア
//------------------------
void CLogic::clear( void ){
    m_oGrid.clear();
    
    reset();
}

//-----------------------
// リセット
//-----------------------
void CLogic::reset( void ){
    // データ＋解析クラス（※[Analyzer]を呼ぶのは[CLogic]なので足並みを揃えておく）
    m_oGrid.reset();
    CLogicAnalyzer::ResetAnalyzeScore();
    
    // 状況
    m_eStatus = eLOGIC_STATUS_INVALID;

    // 選択関連
    m_nSelectColumn = -1;
    m_nSelectRow = -1;
    m_nSelectClueForColumn = -1;
    m_nSelectClueForRow = -1;
    m_nFocusClueForColumn = -1;
    m_nFocusClueForRow = -1;

    // 情報
    m_nSolidDetermined = 0;
    
    // 仮定関連
    resetAssume();

    // オート関連
    resetAuto();
    
    // ステータスの更新（※ログは残さない）
    fixStatusWithLog( NULL );
}

//--------------------------
// 初期化（※引数はデータ文字列）
//--------------------------
bool CLogic::initWithDataStr( const char* pDataStr, char* bufError ){
    if( m_oGrid.init( pDataStr, bufError ) ){
        reset();

        return( true );
    }
    
    return( false );
}

//--------------------------
// 初期化（※引数はファイル名）
//--------------------------
bool CLogic::initWithFile( const char* pFileName, char* bufError, bool isLocal ){
    // バッファ確保
    char* buf = (char*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_DATA );
    DWORD bufSize = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_DATA );
    DWORD srcSize = 0;

    if( isLocal ){
        if( ! CFileMgr::LoadLocalData( pFileName, buf, bufSize, &srcSize ) ){
            if( bufError != NULL ){
                sprintf( bufError, "「%s」のロードに失敗しました", pFileName );
            }
            return( false );
        }
    }else{
        char* path = CMemMgr::GetBufFromTempStr();
        sprintf( path, "logic/%s", pFileName );
        if( ! CFileMgr::LoadFile( path, buf, bufSize, &srcSize ) ){
            if( bufError != NULL ){
                sprintf( bufError, "「%s」のロードに失敗しました", path );
            }
            return( false );
        }
    }
    
    // 終端指定（※文字列扱いなので用心）
    buf[srcSize] = '\0';

    // データ文字列による初期化処理へ
    return( initWithDataStr( buf, bufError ) );
}

//----------------------
// 初期化（サイズ確認用）
//----------------------
bool CLogic::initForSize( int col, int row ){
    if( m_oGrid.initForSize( col, row ) ){
        reset();

        return( true );
    }
    
    return( false );
}

//----------------------
// 初期化（サイズ確認用）
//----------------------
void CLogic::fixForSize(){
    m_oGrid.fixForSize();
}

//-------------------
// 選択：マス
//-------------------
bool CLogic::selectSolid( int selectColumn, int selectRow ){
    // オートプレイ中であれば返す
    if( m_bAutoPlay ){
        return( false );
    }
    
    int column = m_oGrid.getColumn();
    int row = m_oGrid.getRow();

    // 用心
    if( selectColumn < 0 ){
        selectColumn = 0;
    }else if( selectColumn >= column ){
        selectColumn = column - 1;
    }

    if( selectRow < 0 ){
        selectRow = 0;
    }else if( selectRow >= row ){
        selectRow = row - 1;
    }

    bool isChanged = false;

    //-----------------------
    // 選択に差分が出たら更新
    //-----------------------
    if( m_nSelectColumn != selectColumn ){
        m_nSelectColumn = selectColumn;
        isChanged = true;
    }

    if( m_nSelectRow != selectRow ){
        m_nSelectRow = selectRow;
        isChanged = true;
    }

    return( isChanged );
}

//-------------------
// 選択：ヒント（ヨコ）
//-------------------
bool CLogic::selectClueForColumn( int selectColumn, bool isTapped ){
    // オートプレイ中であれば返す
    if( m_bAutoPlay ){
        return( false );
    }

    int column = m_oGrid.getColumn();
    
    // 用心
    if( selectColumn < 0 ){
        selectColumn = 0;
    }else if( selectColumn >= column ){
        selectColumn = column - 1;
    }

    bool isChanged = false;
    
    // タップ／ドラッグ時：違うラインが選択されたら切り替え
    if( m_nSelectClueForColumn != selectColumn ){
        m_nSelectClueForColumn = selectColumn;
        isChanged = true;

        // ヒント対象はクリア
        m_nFocusClueForColumn = 0;
    }
    // タップ時：同じラインが選択されたらヒント対象を次へ
    else if( isTapped ){
        m_nFocusClueForColumn++;

        // いきすぎたら循環
        int num = m_oGrid.getClueNumForColumn( m_nSelectClueForColumn );
        if( m_nFocusClueForColumn >= num ){
            m_nFocusClueForColumn = 0;
        }

        isChanged = true;
    }
    
    // タテはクリアしておく
    if( isChanged ){
        m_nSelectClueForRow = -1;
        m_nFocusClueForRow = -1;
        return( true );
    }

    return( false );
}

//-------------------
// 選択：ヒント（タテ）
//-------------------
bool CLogic::selectClueForRow( int selectRow, bool isTapped ){
    // オートプレイ中であれば返す
    if( m_bAutoPlay ){
        return( false );
    }

    int row = m_oGrid.getRow();
    
    // 用心
    if( selectRow < 0 ){
        selectRow = 0;
    }else if( selectRow >= row ){
        selectRow = row - 1;
    }

    bool isChanged = false;
    
    // タップ／ドラッグ時：違うラインが選択されたら切り替え
    if( m_nSelectClueForRow != selectRow ){
        m_nSelectClueForRow = selectRow;
        isChanged = true;

        m_nFocusClueForRow = 0;
    }
    // タップ時：同じラインが選択されたらヒント対象を次へ
    else if( isTapped ){
        m_nFocusClueForRow++;

        // いきすぎたら循環
        int num = m_oGrid.getClueNumForRow( m_nSelectClueForRow );
        if( m_nFocusClueForRow >= num ){
            m_nFocusClueForRow = 0;
        }
        
        isChanged = true;
    }

    // ヨコはクリアしておく
    if( isChanged ){
        m_nSelectClueForColumn = -1;
        m_nFocusClueForColumn = -1;
        return( true );
    }

    return( false );
}

//----------------------
// 状況確定＆ログ登録
//----------------------
void CLogic::fixStatusWithLog( CLogicLog* pLog, bool isResetStatusAll ){
    if( pLog != NULL ){
        // log: 状況確定の開始
        pLog->addLog_INFO_START( eLLC_INFO_STATUS_FIX );
    }
    
    //----------------------
    // ヒント更新
    //----------------------
    // 全ヒント更新（※内部状態）
    m_oGrid.fixClueAll( isResetStatusAll );
    
    // ヒントとラインの確定（※ログ出力）
    m_oGrid.fixClueAndLineDeterminedWithLog( pLog );
    
    //-------------------------------------------------------------
    // 状態更新
    //-------------------------------------------------------------
    int assumedLv = m_oGrid.getAssumedLevel();
    
    // クリアしていれば
    if( m_oGrid.checkClear() ){
        m_eStatus = eLOGIC_STATUS_CLEAR;
        
        if( pLog != NULL ){
            // log: クリア
            pLog->addLog_STATUS_LOGIC_CLEAR( assumedLv );
        }
    }
    // 矛盾していれば
    else if( m_oGrid.checkImcompatible() ){
        m_eStatus = eLOGIC_STATUS_IMCOMPATIBLE;
        
        if( pLog != NULL ){
            // log: 矛盾
            pLog->addLog_STATUS_LOGIC_IMCOMPATIBLE( assumedLv );
        }
    }
    // その他であればプレイ中
    else{
        m_eStatus = eLOGIC_STATUS_PLAYING;
    }
    
    if( pLog != NULL ){
        // log: 状況確定の終了
        pLog->addLog_INFO_END( eLLC_INFO_STATUS_FIX );
    }
}

//-------------------
// 直接操作
//-------------------
bool CLogic::playManualWithLog( CLogicLog* pLog ){
    // オートプレイ中であれば無視
    if( m_bAutoPlay ){
        return( false );
    }

    if( pLog != NULL ){
        // log: マニュアルプレイの開始
        pLog->addLog_INFO_START( eLLC_INFO_MANUAL_PLAY );
    }
    
    // 対象マスのスワップ（※マス目情報が変化する）
    m_oGrid.swapSolidForDev( m_nSelectColumn, m_nSelectRow );
    
    if( pLog != NULL ){
        // log: マニュアル処理対象
        CLogicSolid* pSolid = m_oGrid.getSolid( m_nSelectColumn, m_nSelectRow );
        pLog->addLog_MANUAL_TARGET( m_nSelectColumn, m_nSelectRow, pSolid->getAssumedLevel(), pSolid->getType() );
    }
    
    // 状況更新
    fixStatusWithLog( pLog, true );   // マス目が後方遷移している可能性があるのでステータスはリセット（※スワップが一周すると空白になるので）
    
    if( pLog != NULL ){
        // log: マニュアルプレイの終了
        pLog->addLog_INFO_END( eLLC_INFO_MANUAL_PLAY );
    }
        
    return( true );
}

//----------------------
// アリスによる解析
//----------------------
int CLogic::analyzeWithLog( CLogicLog* pLog, bool* arrFlag, int size ){
    // プレイ中でなければ返す
    if( m_eStatus != eLOGIC_STATUS_PLAYING ){
        LOGE( "@ CLogic::analyzeWithLog: SKIP: NOT PLAYING\n" );
        return( 0 );
    }

    bool isAssumed = m_oGrid.isAssumed();
    int targetColumn = m_nSelectColumn;
    int targetRow = m_nSelectRow;

    if( pLog != NULL ){
        // log: 解析の開始
        pLog->addLog_INFO_START( eLLC_INFO_ANALYZE );
    }

    // 解析：通常（※指定マスの縦横のラインを解析）
    int  numTotal = CLogicAnalyzer::Analyze( this, targetColumn, targetRow, pLog );
    
    //----------------------------------------------------------------
    // 情報の確定
    //----------------------------------------------------------------
    // 仮定モードでなければ確定マス数の更新
    if( ! isAssumed ){
        m_nSolidDetermined += numTotal;
    }

    // 状況更新
    fixStatusWithLog( pLog, false );   // 前方遷移なのでステータスは持ち越す

    if( pLog != NULL ){
        // log: 解析の終了
        pLog->addLog_INFO_END( eLLC_INFO_ANALYZE );
    }
    
    return( numTotal );
}

//------------------------
// ログの描画
//------------------------
void CLogic::drawLog( float x, float y ){
    char* buf = CMemMgr::GetBufFromTempStr();
    DWORD rgba;
    
    CDrawCell::SetLogAlignLT( 1.0f, x, y );
    
    // 状況
    rgba = (m_bAutoPlay)? 0xFF8080FF: 0xFFFFFFFF;
    const char* pStatusLabel = "S:PLAYING";
    if( m_eStatus == eLOGIC_STATUS_CLEAR ){
        pStatusLabel = "S:CLEAR";
        rgba = 0x80FF80FF;
    }else if( m_eStatus == eLOGIC_STATUS_IMCOMPATIBLE ){
        pStatusLabel = "S:IMCOMPATIBLE";
        rgba = 0xFF8080FF;
    }else if( m_eStatus == eLOGIC_STATUS_GIVE_UP ){
        pStatusLabel = "S:GIVE UP";
        rgba = 0x8080FFFF;
    }else if( m_eStatus == eLOGIC_STATUS_INVALID ){
        pStatusLabel = "S:INVALID";
        rgba = 0x808080FF;
    }else if( m_eStatus != eLOGIC_STATUS_PLAYING ){
        pStatusLabel = "S:UNKNOWN";
        rgba = 0x404040FF;
    }
    CDrawCell::DrawLogAlign( pStatusLabel, rgba );
    
    // モード
    if( m_oGrid.isAssumed() ){
        sprintf( buf, "M:ASM.%d", m_oGrid.getAssumedLevel() );
        rgba = 0xFF00FFFF;
    }else{
        sprintf( buf, "M:NORMAL" );
        rgba = 0xFFFFFFFF;
    }
    CDrawCell::DrawLogAlign( buf, rgba );

#if 0
    // 空行
    CDrawCell::DrawLogAlign( "" );
    
    sprintf( buf, "SOLID: %d", m_nSolidDetermined );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "ASM_TRY: %d", m_nAssumedTry );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "ASM_MAX: %d", m_nAssumedLevelMax );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "ASM_DTM: %d", m_nAssumedDeterminedSolid );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "ASM_IMC: %d", m_nAssumedImcompatibleSolid );
    CDrawCell::DrawLogAlign( buf );
#endif
    
    // 空行
    CDrawCell::DrawLogAlign( "" );
    
    sprintf( buf, "UPDATE: %d", CLogicClueUpdater::GetTotalTry() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "uBS: %d(%d)", CLogicClueUpdater::GetFixSolidScoreBase(), CLogicClueUpdater::GetImcompatibleScoreBase() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "u00: %d(%d)", CLogicClueUpdater::GetFixSolidScore_00(), CLogicClueUpdater::GetImcompatibleScore_00() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "u01: %d(%d)", CLogicClueUpdater::GetFixSolidScore_01(), CLogicClueUpdater::GetImcompatibleScore_01()  );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "u02: %d(%d)", CLogicClueUpdater::GetFixSolidScore_02(), CLogicClueUpdater::GetImcompatibleScore_02() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "u03: %d(%d)", CLogicClueUpdater::GetFixSolidScore_03(), CLogicClueUpdater::GetImcompatibleScore_03() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "u04: %d(%d)", CLogicClueUpdater::GetFixSolidScore_04(), CLogicClueUpdater::GetImcompatibleScore_04() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "u05: %d(%d)", CLogicClueUpdater::GetFixSolidScore_05(), CLogicClueUpdater::GetImcompatibleScore_05() );
    CDrawCell::DrawLogAlign( buf );
    
    // 空行
    CDrawCell::DrawLogAlign( "" );
    
    // 解析関連
    sprintf( buf, "ANALYZE: %d", CLogicAnalyzer::GetTotalTry() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "a00: %d", CLogicAnalyzer::GetAnalyzeScore_00() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "a01: %d", CLogicAnalyzer::GetAnalyzeScore_01() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "a02: %d", CLogicAnalyzer::GetAnalyzeScore_02() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "a03: %d", CLogicAnalyzer::GetAnalyzeScore_03() );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "a04: %d", CLogicAnalyzer::GetAnalyzeScore_04() );
    CDrawCell::DrawLogAlign( buf );
}

//-------------------------
// 前のヒント
//-------------------------
bool CLogic::prevClue( void ){
    if( m_nSelectClueForRow >= 0 ){
        int numClue = m_oGrid.getClueNumForRow( m_nSelectClueForRow );
        if( numClue >= 2 ){
            m_nFocusClueForRow--;
            if( m_nFocusClueForRow < 0 ){
                m_nFocusClueForRow = numClue - 1;
            }
            return( true );
        }
    }else if( m_nSelectClueForColumn >= 0  ){
        int numClue = m_oGrid.getClueNumForColumn( m_nSelectClueForColumn );
        if( numClue >= 2 ){
            m_nFocusClueForColumn--;
            if( m_nFocusClueForColumn < 0 ){
                m_nFocusClueForColumn = numClue - 1;
            }
            return( true );
        }
    }
    
    return( false );
}

//-------------------------
// 次のヒント
//-------------------------
bool CLogic::nextClue( void ){
    if( m_nSelectClueForRow >= 0 ){
        int numClue = m_oGrid.getClueNumForRow( m_nSelectClueForRow );
        if( numClue >= 2 ){
            m_nFocusClueForRow++;
            if( m_nFocusClueForRow >= numClue ){
                m_nFocusClueForRow = 0;
            }
            return( true );
        }
    }else if( m_nSelectClueForColumn >= 0  ){
        int numClue = m_oGrid.getClueNumForColumn( m_nSelectClueForColumn );
        if( numClue >= 2 ){
            m_nFocusClueForColumn++;
            if( m_nFocusClueForColumn >= numClue ){
                m_nFocusClueForColumn = 0;
            }
            return( true );
        }
    }
    
    return( false );
}

//-------------------------
// ヒントの追加
//-------------------------
bool CLogic::addClue( void ){
    if( m_nSelectClueForRow >= 0 ){
        if( m_oGrid.addClueAt( true, m_nSelectClueForRow ) ){
            m_nFocusClueForRow = m_oGrid.getClueNumForRow( m_nSelectClueForRow ) - 1;
            return( true );
        }
    }else if( m_nSelectClueForColumn >= 0  ){
        if( m_oGrid.addClueAt( false, m_nSelectClueForColumn ) ){
            m_nFocusClueForColumn = m_oGrid.getClueNumForColumn( m_nSelectClueForColumn ) - 1;
            return( true );
        }
    }
    
    return( false );
}

//-------------------------
// ヒントの削除
//-------------------------
bool CLogic::deleteClue( void ){
    if( m_nSelectClueForRow >= 0 && m_nFocusClueForRow >= 0 ){
        if( m_oGrid.deleteClueAt( true, m_nSelectClueForRow, m_nFocusClueForRow ) ){
            int numClue = m_oGrid.getClueNumForRow( m_nSelectClueForRow );
            if( m_nFocusClueForRow >= numClue ){
                m_nFocusClueForRow =  numClue - 1;
            }
            return( true );
        }
    }else if( m_nSelectClueForColumn >= 0 && m_nFocusClueForColumn >= 0 ){
        if( m_oGrid.deleteClueAt( false, m_nSelectClueForColumn, m_nFocusClueForColumn ) ){
            int numClue = m_oGrid.getClueNumForColumn( m_nSelectClueForColumn );
            if( m_nFocusClueForColumn >= numClue ){
                m_nFocusClueForColumn =  numClue - 1;
            }
            return( true );
        }
    }
    
    return( false );
}

//-------------------------
// ヒントの設定
//-------------------------
bool CLogic::setClue( int val ){
    if( m_nSelectClueForRow >= 0 && m_nFocusClueForRow >= 0 ){
        if( m_oGrid.setClueAt( true, m_nSelectClueForRow, m_nFocusClueForRow, val ) ){
            m_nFocusClueForRow++;

            int numClue = m_oGrid.getClueNumForRow( m_nSelectClueForRow );
            if( m_nFocusClueForRow >= numClue ){
                m_nFocusClueForRow =  numClue - 1;
            }
            
            return( true );
        }
    }else if( m_nSelectClueForColumn >= 0 && m_nFocusClueForColumn >= 0 ){
        if( m_oGrid.setClueAt( false, m_nSelectClueForColumn, m_nFocusClueForColumn, val ) ){
            m_nFocusClueForColumn++;
            
            int numClue = m_oGrid.getClueNumForColumn( m_nSelectClueForColumn );
            if( m_nFocusClueForColumn >= numClue ){
                m_nFocusClueForColumn =  numClue - 1;
            }
            
            return( true );
        }
    }
    
    return( false );
}
