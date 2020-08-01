/*+----------------------------------------------------------------+
  |	Title:		LogicLog.hpp [共通環境]
  |	Comment:	ロジック：ログ（※解析で確定したマスやヒントの情報）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_LOG_HPP__
#define __LOGIC_LOG_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicLogCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// ロジックログ：ターン中の解析処理に先立って初期化される想定なので配列形式で管理
//-----------------------------------------------------------------
class CLogicLog {
protected:
    //---------------------------
    // セル管理
    //---------------------------
    CLogicLogCell m_oArrCell[LOGIC_LOG_CELL_MAX];   // セル配列
    int m_nLogNum;                                  // ログ数
    int m_nNextLogAt;                               // 次のログの参照位置

public:
    // コンストラクタ／デストラクタ
	CLogicLog( void );
	virtual ~CLogicLog( void );
    
    // クリア
    void clear( void );
    
    // リセット
    void reset( void );
    
    // 空か？
    inline bool isEmpty( void ){ return( m_nLogNum <= 0 ); }

    // ログの取り出し（※古い順番／ログがなければ[NULL]が返る）
    CLogicLogCell* getNextLog( void );

    //----------------------------------------------
    // 情報区分
    //----------------------------------------------
    // 情報：開始：[p0=情報タイプ],[p1=オプション]…
    inline void addLog_INFO_START( eLLC_INFO info, int op0=0, int op1=0, int op2=0 ){
        addLog( eLLC_TYPE_INFO_START, (int)info, op0, op1, op2 );
    }

    // 情報：終了：[p0=情報タイプ]、[p1=オプション]…
    inline void addLog_INFO_END( eLLC_INFO info, int op0=0, int op1=0, int op2=0 ){
        // 直近のログが開始であれば削除（※[開始]〜[終了]の間に何の登録もなかった）
        CLogicLogCell* pLastLog = getLastLog();
        if( pLastLog != NULL ){
            if( pLastLog->getType() == eLLC_TYPE_INFO_START && pLastLog->getParamAt(0) == (int)info ){
                removeLastLog();
                return;
            }
        }
        
        // ここまできたら登録
        addLog( eLLC_TYPE_INFO_END, (int)info, op0, op1, op2 );
    }

    //----------------------------------------------
    // 状況関連
    //----------------------------------------------
    // 状況：行のヒントが確定した：[p0=行]、[p1=ヒント番号]、[p2=仮定レベル]、[p3=ヒントの状態]
    inline void addLog_STATUS_CLUE_IN_COLUMN( int col, int no, int lv, eLOGIC_CLUE_STATUS stat ){
        addLog( eLLC_TYPE_STATUS_CLUE_IN_COLUMN, col, no, lv, (int)stat );
    }
    
    // 状況：列のヒントが確定した：[p0=列]、[p1=ヒント番号]、[p2=仮定レベル]、[p3=ヒントの状態]
    inline void addLog_STATUS_CLUE_IN_ROW( int row, int no, int lv, eLOGIC_CLUE_STATUS stat ){
        addLog( eLLC_TYPE_STATUS_CLUE_IN_ROW, row, no, lv, (int)stat );
    }
    
    // 状況：行のラインが確定した：[p0=行]、[p1=仮定レベル]、[p2=ラインの状態]
    inline void addLog_STATUS_LINE_IN_COLUMN( int col, int lv, eLOGIC_LINE_STATUS stat ){
        addLog( eLLC_TYPE_STATUS_LINE_IN_COLUMN, col, lv, (int)stat );
    }
    
    // 状況：列のラインが確定した：[p0=列]、[p1=仮定レベル]、[p2=ラインの状態]
    inline void addLog_STATUS_LINE_IN_ROW( int row, int lv, eLOGIC_LINE_STATUS stat ){
        addLog( eLLC_TYPE_STATUS_LINE_IN_ROW, row, lv, (int)stat );
    }
    
    // 状況：ロジックをクリアした：[p0=仮定レベル]
    inline void addLog_STATUS_LOGIC_CLEAR( int lv ){
        addLog( eLLC_TYPE_STATUS_LOGIC_CLEAR, lv );
    }
    
    // 状況：ロジックが矛盾した：[p0=仮定レベル]
    inline void addLog_STATUS_LOGIC_IMCOMPATIBLE( int lv ){
        addLog( eLLC_TYPE_STATUS_LOGIC_IMCOMPATIBLE, lv );
    }
    
    // 状況：ロジックの解析が行き詰まった（※オート時）：[p0=仮定レベル]
    inline void addLog_STATUS_LOGIC_GIVE_UP( int lv ){
        addLog( eLLC_TYPE_STATUS_LOGIC_GIVE_UP, lv );
    }
    
    //----------------------------------------------
    // マニュアル関連
    //----------------------------------------------
    // 手動：処理対象：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種類]
    inline void addLog_MANUAL_TARGET( int col, int row, int lv, eLOGIC_SOLID_TYPE solid ){
        addLog( eLLC_TYPE_MANUAL_TARGET, col, row, lv, (int)solid );
    }

    //----------------------------------------------
    // 解析関連
    //----------------------------------------------
    // 解析：ターゲット情報：[p0=行]、[p1=列]
    inline void addLog_ANALYZE_TARGET_INFO( int col, int row ){
        addLog( eLLC_TYPE_ANALYZE_TARGET_INFO, col, row );
    }

    // 解析：ターゲットが確定した：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    inline void addLog_ANALYZE_TARGET_DETERMINED( int col, int row, int lv, eLOGIC_SOLID_TYPE solid ){
        addLog( eLLC_TYPE_ANALYZE_TARGET_DETERMINED, col, row, lv, (int)solid );
    }

    // 解析：マスを埋めた：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    inline void addLog_ANALYZE_SOLID( int col, int row, int lv, eLOGIC_SOLID_TYPE solid ){
        addLog( eLLC_TYPE_ANALYZE_SOLID, col, row, lv, (int)solid );
    }

    //----------------------------------------------
    // 仮定関連
    //----------------------------------------------
    // 仮定：開始情報：[p0＝行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    inline void addLog_ASSUME_START_INFO( int col, int row, int lv, eLOGIC_SOLID_TYPE solid ){
        addLog( eLLC_TYPE_ASSUME_START_INFO, col, row, lv, (int)solid );
    }

    // 仮定：確定した：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    inline void addLog_ASSUME_DETERMINED( int col, int row, int lv, eLOGIC_SOLID_TYPE solid ){
        addLog( eLLC_TYPE_ANALYZE_SOLID, col, row, lv, (int)solid );
    }
    
    // 仮定：矛盾した：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    inline void addLog_ASSUME_IMCOMPATIBLE( int col, int row, int lv, eLOGIC_SOLID_TYPE solid ){
        addLog( eLLC_TYPE_ASSUME_IMCOMPATIBLE, col, row, lv, (int)solid );
    }
    
    // 仮定：解除された：[p0=行]、[p1=列]、[p2=仮定レベル]、[p3=マスの種別]
    inline void addLog_ASSUME_OFF( int col, int row, int lv, eLOGIC_SOLID_TYPE solid ){
        addLog( eLLC_TYPE_ASSUME_OFF, col, row, lv, (int)solid );
    }
    
protected:
    // セル登録
    CLogicLogCell* addLog( eLLC_TYPE type, int p0=-1, int p1=-1, int p2=-2, int p3=-1 );
    
    // 直近のログの確認
    CLogicLogCell* getLastLog( void );
    
    // 直近のログを削除する（※開始登録したログを途中失敗により削除したい場合）
    void removeLastLog( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_LOG_HPP__
