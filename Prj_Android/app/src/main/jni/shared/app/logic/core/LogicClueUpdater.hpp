/*+----------------------------------------------------------------+
  |	Title:		LogicClueUpdater.hpp [共通環境]
  |	Comment:	ロジック（コア）：ヒント更新（※ヒントの候補マスを更新する処理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_CLUE_UPDATER_HPP__
#define __LOGIC_CLUE_UPDATER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicGrid.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------
// ヒント更新種別（※ログ登録用）
//-------------------------
enum eLOGIC_CLUE_UPDATE{
    eLOGIC_CLUE_UPDATE_INVALID = -1,
    
    //[◎][BASE]: 基本設定
    eLOGIC_CLUE_UPDATE_BASE,
    
    //[◎][00]: 届かないマスを除外
    eLOGIC_CLUE_UPDATE_00_UNREACHABLE,
    
    //[◎][01]: 前後の範囲内による調整
    eLOGIC_CLUE_UPDATE_01_BEFORE_AND_AFTER,
    
    //[◎][02]: トリミング（※前後の無効マスを除外）
    eLOGIC_CLUE_UPDATE_02_TRIM,
    
    //[◎][03]: 黒マスの分布による調整（※ヒント配列を分割して再帰処理を施す）
    eLOGIC_CLUE_UPDATE_03_BLACK_DISTRIBUTION,
    
    //[◎][04]: 黒マスの数がヒントの範囲内で確定していたら調整（※[03]でもれた限定状況への対応）
    eLOGIC_CLUE_UPDATE_04_BALCK_NUM,
    
    //[◎][05]: 白マスで分断された黒マスを含むブロックの数がヒントと等しければ調整（※[03]でもれた限定状況への対応＆矛盾判定にかかることが多い）
    eLOGIC_CLUE_UPDATE_05_SEPARATED_BLOCK,
    
    eLOGIC_CLUE_UPDATE_MAX,
};

#define IS_LOGIC_CLUE_UPDATE_VALID( _t )    ((_t)>=(eLOGIC_CLUE_UPDATE)0 && (_t)<eLOGIC_CLUE_UPDATE_MAX)

// パラメータセット（※型をあわせたいのでマクロで定義しておく）
#define LOGIC_CLUE_UPDATER_PARAM_DEF    \
    CLogicGrid* pGrid,                  \
    CLogicClue* arrClue,                \
    int sizeClue,                       \
    bool* isImcompatible,               \
    bool isRow,                         \
    int targetIndex,                    \
    int solidOfs,                       \
    int solidNum

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジック：ヒント更新
//----------------------------------------
class CLogicClueUpdater {
protected:
    // 各種スコア（※確認用）
    static int s_nTotalTry;
    static int s_nFixSolidScoreBase, s_nImcompatibleScoreBase;
    static int s_nFixSolidScore_00, s_nImcompatibleScore_00;
    static int s_nFixSolidScore_01, s_nImcompatibleScore_01;
    static int s_nFixSolidScore_02, s_nImcompatibleScore_02;
    static int s_nFixSolidScore_03, s_nImcompatibleScore_03;
    static int s_nFixSolidScore_04, s_nImcompatibleScore_04;
    static int s_nFixSolidScore_05, s_nImcompatibleScore_05;

public:
    // スコアのリセット
    static void ResetFixScore( void );
    
    // スコア取得
    static inline int GetTotalTry( void ){ return( s_nTotalTry ); }

    static inline int GetFixSolidScoreBase( void ){ return( s_nFixSolidScoreBase ); }
    static inline int GetFixSolidScore_00( void ){ return( s_nFixSolidScore_00 ); }
    static inline int GetFixSolidScore_01( void ){ return( s_nFixSolidScore_01 ); }
    static inline int GetFixSolidScore_02( void ){ return( s_nFixSolidScore_02 ); }
    static inline int GetFixSolidScore_03( void ){ return( s_nFixSolidScore_03 ); }
    static inline int GetFixSolidScore_04( void ){ return( s_nFixSolidScore_04 ); }
    static inline int GetFixSolidScore_05( void ){ return( s_nFixSolidScore_05 ); }

    static inline int GetImcompatibleScoreBase( void ){ return( s_nImcompatibleScoreBase ); }
    static inline int GetImcompatibleScore_00( void ){ return( s_nImcompatibleScore_00 ); }
    static inline int GetImcompatibleScore_01( void ){ return( s_nImcompatibleScore_01 ); }
    static inline int GetImcompatibleScore_02( void ){ return( s_nImcompatibleScore_02 ); }
    static inline int GetImcompatibleScore_03( void ){ return( s_nImcompatibleScore_03 ); }
    static inline int GetImcompatibleScore_04( void ){ return( s_nImcompatibleScore_04 ); }
    static inline int GetImcompatibleScore_05( void ){ return( s_nImcompatibleScore_05 ); }

    //--------------------------------------------------------------------------------
    // ヒントの更新
    //--------------------------------------------------------------------------------
    // 確定ヒントをピボットとして分割された、ヒントの部分配列に対する設定（※確定済みヒントはこない想定）
    //（※内部で[pGrid->checkSolid***]を呼び出しす際は、オフセットである[solidOfs]を足しこむこと）
    //--------------------------------------------------------------------------------
    static bool UpdateClue( CLogicGrid* pGrid, CLogicClue* arrClue, int sizeClue, int blockAt,
                            bool isRow, int targetIndex, int solidOfs, int solidNum );

protected:
    //--------------------------------------------------------------------------------
    // 基本更新（※ヒント単体から設定できる調整）（※返値は正常終了したかのフラグ＝[false]で矛盾終了）
    //--------------------------------------------------------------------------------
    static bool _setBaseSolidInfoForClue( CLogicGrid* pGrid, CLogicClue* arrClue, int sizeClue, int blockAt,
                                          bool isRow, int targetIndex, int solidOfs, int solidNum );
    
    //--------------------------------------------------------------------------------
    // マスブロック調整（※他のヒントとの相互調整）
    //--------------------------------------------------------------------------------
    // [00]: 届かないマスを除外
    static int _fixSolidBlocksForUnreachable( LOGIC_CLUE_UPDATER_PARAM_DEF );

    // [01]: 前後の範囲内による調整（※複数ヒントが対象）
    static int _fixSolidBlocksForBeforeAndAfter( LOGIC_CLUE_UPDATER_PARAM_DEF );

    // [02]: トリミング（※前後の無効マスを除外）
    static int _fixSolidBlocksForTrim( LOGIC_CLUE_UPDATER_PARAM_DEF );

    // [03]: 黒マスの分布による調整（※複数ヒントが対象）
    static int _fixSolidBlocksForBlackDistribution( LOGIC_CLUE_UPDATER_PARAM_DEF );
    static int _fixSolidBlocksForBlackDistributionSub( LOGIC_CLUE_UPDATER_PARAM_DEF, int solidStart, int solidEnd );

    // [04]: 黒マスの数がヒントの範囲内で確定していたら調整
    static int _fixSolidBlocksForBlackNum( LOGIC_CLUE_UPDATER_PARAM_DEF );

    // [05]: マスの塊の数が確定していたら調整
    static int _fixSolidBlocksForSeparatedBlock( LOGIC_CLUE_UPDATER_PARAM_DEF );

private:
    CLogicClueUpdater( void ){}
    virtual ~CLogicClueUpdater( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_CLUE_UPDATER_HPP__
