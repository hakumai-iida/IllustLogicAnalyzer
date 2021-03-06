/*+----------------------------------------------------------------+
  |	Title:		LogicLogCell.hpp [共通環境]
  |	Comment:	ロジック：ログ単位
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_LOG_CELL_HPP__
#define __LOGIC_LOG_CELL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "app/logic/LogicConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------------------------------
// ログの情報タイプ（※ロジックの盤面に影響する処理の判別用）
//-------------------------------------------------
enum eLLC_INFO{
    eLLC_INFO_INVALID = -1,
    
    //-------------------------------------------------
    // 基本操作：[CLogic]上の処理（※イラストロジック的な処理）
    //-------------------------------------------------
    eLLC_INFO_STATUS_FIX,           // 状況確定：[CLogic::fixStatusWithLog]
    
    eLLC_INFO_MANUAL_PLAY,          // 手動プレイ: [CLogic::playManualWithLog]
    eLLC_INFO_AUTO_PLAY,            // 自動プレイ: [CLogic::playAutoWithLog]

    eLLC_INFO_ANALYZE,              // 解析: [CLogic::analyzeWithLog]
    eLLC_INFO_ASSUME_ON,            // 仮定開始: [CLogic::assumeOnWithLog]
    eLLC_INFO_ASSUME_OFF,           // 仮定終了: [CLogic::assumeOffWithLog]
    
    eLLC_INFO_ANALYZE_METHOD,       // 解析メソッド指定: [CLogicAnalyzer::Analyze]

    eLLC_INFO_MAX,
};

//-------------------------------------------------
// ログタイプ
//-------------------------------------------------
enum eLLC_TYPE{
    eLLC_TYPE_INVALID = -1,
    
    //-------------------------------------------------------------
    // 情報区分（※全てのログ登録は[INFO_START]で始まり[INFO_END]で終わる）
    //（※情報区分間に１つもログがない場合、[INFO_START]の登録が削除される）
    //-------------------------------------------------------------
    eLLC_TYPE_INFO_START,                   // 情報区分：開始：[p0=情報タイプ]、[p1=オプション]…
    eLLC_TYPE_INFO_END,                     // 情報区分：終了：[p0=情報タイプ]、[p1=オプション]…

    //-------------------------------------------------------------
    // 基本操作ログ：状況確定
    //-------------------------------------------------------------
    eLLC_TYPE_STATUS_CLUE_IN_COLUMN,        // 行のヒントが確定した：[p0=行]、[p1=ヒント番号]、[p2=仮定レベル]、[p3=ヒントの状態]
    eLLC_TYPE_STATUS_CLUE_IN_ROW,           // 列のヒントが確定した：[p0=列]、[p1=ヒント番号]、[p2=仮定レベル]、[p3=ヒントの状態]
    eLLC_TYPE_STATUS_LINE_IN_COLUMN,        // 行のラインが確定した：[p0=行]、[p1=仮定レベル]、[p2=ラインの状態]
    eLLC_TYPE_STATUS_LINE_IN_ROW,           // 列のラインが確定した：[p0=列]、[p1=仮定レベル]、[p2=ラインの状態]
    eLLC_TYPE_STATUS_LOGIC_CLEAR,           // ロジックをクリアした：[p0=仮定レベル]
    eLLC_TYPE_STATUS_LOGIC_IMCOMPATIBLE,    // ロジックが矛盾した：[p0=仮定レベル]
    eLLC_TYPE_STATUS_LOGIC_GIVE_UP,         // ロジックの解析が行き詰まった（※オート時）：[p0=仮定レベル]

    //-------------------------------------------------------------
    // 基本操作ログ：マニュアル関連
    //-------------------------------------------------------------
    eLLC_TYPE_MANUAL_TARGET,                // マニュアル：処理対象：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種類]

    //-------------------------------------------------------------
    // 基本操作ログ：解析関連
    //-------------------------------------------------------------
    eLLC_TYPE_ANALYZE_TARGET_INFO,          // 解析：ターゲット情報：[p0=行]、[p1=列]
    eLLC_TYPE_ANALYZE_TARGET_DETERMINED,    // 解析：ターゲットが確定した：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    eLLC_TYPE_ANALYZE_SOLID,                // 解析：マスを埋めた：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]

    //-------------------------------------------------------------
    // 基本操作ログ：仮定関連
    //-------------------------------------------------------------
    eLLC_TYPE_ASSUME_START_INFO,            // 仮定：開始情報：[p0＝行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    eLLC_TYPE_ASSUME_DETERMINED,            // 仮定：確定した：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    eLLC_TYPE_ASSUME_IMCOMPATIBLE,          // 仮定：矛盾した：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    eLLC_TYPE_ASSUME_OFF,                   // 仮定：解除された：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]

    eLLC_TYPE_MAX,
};

//------------------
// パラメータ数
//------------------
#define LOGIC_LOG_CELL_PARAM_MAX    4

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジックログセル
//----------------------------------------
class CLogicLogCell {
protected:
    eLLC_TYPE m_eType;                          // タイプ
    int m_nArrParam[LOGIC_LOG_CELL_PARAM_MAX];  // パラメータ

public:
	CLogicLogCell( void );
	virtual ~CLogicLogCell( void );

    // クリア
	void clear( void );

    //--------------------------
    // 設定
    //--------------------------
    void set( eLLC_TYPE type, int p0, int p1, int p2, int p3 );
    
    //--------------------------
    // 取得
    //--------------------------
    inline eLLC_TYPE getType( void ){ return( m_eType ); }
    inline int getParamAt( int at ){
        if( at >= 0 && at < LOGIC_LOG_CELL_PARAM_MAX ){
            return( m_nArrParam[at] );
        }
        return( -1 );
    }

    // ログ文言設定（※開発用）
    void setLogForBuf( char* buf );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelLogicLogCellInfo[];
extern const char* g_pArrLabelLogicLogCellType[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_LOG_CELL_HPP__
