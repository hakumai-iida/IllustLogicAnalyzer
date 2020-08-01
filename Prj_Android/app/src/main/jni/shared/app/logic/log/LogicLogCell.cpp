/*+----------------------------------------------------------------+
  |	Title:		LogicLogCell.cpp [共通環境]
  |	Comment:	ロジック：ログ単位
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicLogCell.hpp"

#include "app/logic/LogicConst.hpp"

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
//--------------------------
// ラベル：情報種別
//--------------------------
const char* g_pArrLabelLogicLogCellInfo[] = {
    // 基本
    "STATUS_FIX",

    "MANUAL_PLAY",
    "AUTO_PLAY",
    
    "ANALYZE",
    "ASSUME_ON",
    "ASSUME_OFF",
    
    "ANALYZE_METHOD",

    NULL,
};

//--------------------------
// ラベル：タイプ
//--------------------------
const char* g_pArrLabelLogicLogCellType[] = {
    "INFO_START",                       // 情報区分：開始
    "INFO_END",                         // 情報区分：終了

    "STATUS_CLUE_IN_COLUMN",            // 行のヒントが確定した
    "STATUS_CLUE_IN_ROW",               // 列のヒントが確定した
    "STATUS_LINE_IN_COLUMN",            // 行のラインが確定した
    "STATUS_LINE_IN_ROW",               // 列のラインが確定した
    "STATUS_LOGIC_CLEAR",               // ロジックをクリアした
    "STATUS_LOGIC_IMCOMPATIBLE",        // ロジックが矛盾した
    "STATUS_LOGIC_GIVE_UP",             // ロジックの解析が行き詰まった（※オート時）

    "MANUAL_TARGET",                    // マニュアル：処理対象
    
    "ANALYZE_TARGET_INFO",              // 解析：ターゲット情報
    "ANALYZE_TARGET_DETERMINED",        // 解析：ターゲットが確定した
    "ANALYZE_SOLID",                    // 解析：マスを埋めた

    "ASSUME_START_INFO",                // 仮定：開始情報
    "ASSUME_DETERMINED",                // 仮定：確定した
    "ASSUME_IMCOMPATIBLE",              // 仮定：矛盾した
    "ASSUME_OFF",                       // 仮定：解除された

    NULL,
};

//------------------------------
// パラメータ数（※ログ表示用）
//------------------------------
static int _arr_cell_param_num[] = {
    4,          // eLLC_TYPE_INFO_START
    4,          // eLLC_TYPE_INFO_END
    
    4,          // eLLC_TYPE_STATUS_CLUE_IN_COLUMN
    4,          // eLLC_TYPE_STATUS_CLUE_IN_ROW
    3,          // eLLC_TYPE_STATUS_LINE_IN_COLUMN
    3,          // eLLC_TYPE_STATUS_LINE_IN_ROW
    1,          // eLLC_TYPE_STATUS_LOGIC_CLEAR
    1,          // eLLC_TYPE_STATUS_LOGIC_IMCOMPATIBLE
    1,          // eLLC_TYPE_STATUS_LOGIC_GIVE_UP

    4,          // eLLC_TYPE_MANUAL_TARGET
    
    2,          // eLLC_TYPE_ANALYZE_TARGET_INFO
    4,          // eLLC_TYPE_ANALYZE_TARGET_DETERMINED
    4,          // eLLC_TYPE_ANALYZE_SOLID
    
    4,          // eLLC_TYPE_ASSUME_START_INFO
    4,          // eLLC_TYPE_ASSUME_DETERMINED
    4,          // eLLC_TYPE_ASSUME_IMCOMPATIBLE
    4,          // eLLC_TYPE_ASSUME_OFF
};

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------
// コンストラクタ
//-----------------------
CLogicLogCell::CLogicLogCell( void ){
    clear();
}

//-----------------------
// デストラクタ
//-----------------------
CLogicLogCell::~CLogicLogCell( void ){
}

//-----------------------
// クリア
//-----------------------
void CLogicLogCell::clear( void ){
    m_eType = eLLC_TYPE_INVALID;
    for( int i=0; i<LOGIC_LOG_CELL_PARAM_MAX; i++ ){
        m_nArrParam[i] = -1;
    }
}

//-----------------------
// 設定
//-----------------------
void CLogicLogCell::set( eLLC_TYPE type, int p0, int p1, int p2, int p3 ){
    m_eType = type;
    m_nArrParam[0] = p0;
    m_nArrParam[1] = p1;
    m_nArrParam[2] = p2;
    m_nArrParam[3] = p3;
}

//-----------------------
// ログ文言設定（※開発用）
//-----------------------
void CLogicLogCell::setLogForBuf( char* buf ){
    // 無効は無視
    if( buf == NULL ){
        return;
    }
    
    // ラベルとパラメータ数
    const char* pLabel;
    int numParam = LOGIC_LOG_CELL_PARAM_MAX;
    if( m_eType >= (eLLC_TYPE)0 && m_eType < eLLC_TYPE_MAX ){
        pLabel = g_pArrLabelLogicLogCellType[m_eType];
        numParam = _arr_cell_param_num[m_eType];
    }else if( m_eType == eLLC_TYPE_INVALID ){
        pLabel = ENUM_INVALID_LABEL;
    }else{
        pLabel = ENUM_NOT_FOUND_LABEL;
    }

    // [INFO_START/END]例外
    if( m_eType == eLLC_TYPE_INFO_START || m_eType == eLLC_TYPE_INFO_END ){
        const char* pInfo = "UNKNOWN";
        if( m_nArrParam[0] >= 0 && m_nArrParam[0] < eLLC_INFO_MAX ){
            pInfo = g_pArrLabelLogicLogCellInfo[m_nArrParam[0]];
        }
        sprintf( buf, "*LOG[%s]: INFO=%s(%d), p0=%d, p1=%d, p2=%d", pLabel, pInfo, m_nArrParam[0],
                 m_nArrParam[1], m_nArrParam[2], m_nArrParam[3] );
    }
    // その他
    else{
        // 文字列設定
        switch( numParam ){
        case 0:
            sprintf( buf, " LOG[%s]:", pLabel );
            break;

        case 1:
            sprintf( buf, " LOG[%s]: p0=%d", pLabel,
                     m_nArrParam[0] );
            break;
            
        case 2:
            sprintf( buf, " LOG[%s]: p0=%d, p1=%d", pLabel,
                     m_nArrParam[0], m_nArrParam[1] );
            break;
            
        case 3:
            sprintf( buf, " LOG[%s]: p0=%d, p1=%d, p2=%d", pLabel,
                     m_nArrParam[0], m_nArrParam[1], m_nArrParam[2] );
            break;
            
        case 4:
        default:
            sprintf( buf, " LOG[%s]: p0=%d, p1=%d, p2=%d, p3=%d", pLabel,
                     m_nArrParam[0], m_nArrParam[1], m_nArrParam[2], m_nArrParam[3] );
            break;
        }
    }
}
