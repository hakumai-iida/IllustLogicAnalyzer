/*+----------------------------------------------------------------+
  |	Title:		LogicAnalyzer.hpp [共通環境]
  |	Comment:	ロジック（コア）：解析（※実際にマスを埋める処理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_ANALYZER_HPP__
#define __LOGIC_ANALYZER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicGrid.hpp"

#include "app/logic/core/Logic.hpp"
#include "app/logic/log/LogicLog.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------
// 解析種別（※ログ登録用）
//-------------------------
enum eLOGIC_ANALYZE{
    eLOGIC_ANALYZE_INVALID = -1,
    
    //[◎][00/黒マスの特定]:ブロックの両端からヒントの数だけマスをのばして重った部分は黒マス
    eLOGIC_ANALYZE_00_CHECK_HINT_LEN_THEN_SET_BLACK,
    
    //[◎][01/白マスの特定]:どのヒントにも所属しないマスは白マス
    eLOGIC_ANALYZE_01_CHECK_ISOLATED_SOLID_THEN_SET_WHITE,
    
    //[◎][02/矛盾の特定]:空マスを白マスにした場合、矛盾するようなら黒マス
    eLOGIC_ANALYZE_02_CHECK_IMCOMPATIBLE_FOR_WHITE_THEN_SET_BLACK,
    
    //[◎][03/矛盾の特定]:空マスを黒マスにした場合、矛盾するようなら白マス
    eLOGIC_ANALYZE_03_CHECK_IMCOMPATIBLE_FOR_BLACK_THEN_SET_WHITE,
    
    //[◎][04/白マスの特定]:黒マスの長さがヒントの最大と等しければ両端は白マス
    eLOGIC_ANALYZE_04_CHECK_MAX_CLUE_FOR_BLACK_THEN_SET_WHITE,
    
    eLOGIC_ANALYZE_MAX,
};

#define IS_LOGIC_ANALYZE_VALID( _t )    ((_t)>=(eLOGIC_ANALYZE)0 && (_t)<eLOGIC_ANALYZE_MAX)

// パラメータセット（※型をあわせたいのでマクロで定義しておく）
#define LOGIC_ANALYZER_PARAM        \
    CLogicGrid* pGrid,              \
    bool isRow,                     \
    int targetIndex,                \
    CLogicLog* pLog

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジック（コア）：解析
//----------------------------------------
class CLogicAnalyzer {
protected:
    // 解析情報
    static int s_nTargetColumn;
    static int s_nTargetRow;

    // 解析スコア（※確認用）
    static int s_nTotalTry;
    static int s_nAnalyzeScore_00;
    static int s_nAnalyzeScore_01;
    static int s_nAnalyzeScore_02;
    static int s_nAnalyzeScore_03;
    static int s_nAnalyzeScore_04;
    
public:
    // スコアのリセット
    static void ResetAnalyzeScore( void );
    
    // スコアの取得
    static inline int GetTotalTry( void ){ return( s_nTotalTry ); }
    static inline int GetAnalyzeScore_00( void ){ return( s_nAnalyzeScore_00 ); }
    static inline int GetAnalyzeScore_01( void ){ return( s_nAnalyzeScore_01 ); }
    static inline int GetAnalyzeScore_02( void ){ return( s_nAnalyzeScore_02 ); }
    static inline int GetAnalyzeScore_03( void ){ return( s_nAnalyzeScore_03 ); }
    static inline int GetAnalyzeScore_04( void ){ return( s_nAnalyzeScore_04 ); }

    // 解析：指定のマス[targetColumn,targetRow]を中心に縦横のラインを変化がなくなるまで処理する
    static int Analyze( CLogic* pLogic, int targetColumn, int targetRow,
                        CLogicLog* pLog=NULL, int* pNumBlack=NULL, int* pNumWhite=NULL );
    
protected:
    //-----------------------------------
    // 各種処理
    //-----------------------------------
    // [00/黒マスの特定]:ブロックの両端からヒントの数だけマスをのばして重った部分は黒マス
    static int _checkHintLenThenSetBlack( LOGIC_ANALYZER_PARAM );

    // [01/白マスの特定]:どのヒントにも所属しないマスは白マス
    static int _checkIsolatedSolidThenSetWhite( LOGIC_ANALYZER_PARAM );

    // [02/矛盾の特定]:空マスを白マスにした場合、矛盾するようなら黒マス
    static int _checkImcompatibleForWhiteThenSetBlack( LOGIC_ANALYZER_PARAM );

    // [03/矛盾の特定]:空マスを黒マスにした場合、矛盾するようなら白マス
    static int _checkImcompatibleForBlackThenSetWhite( LOGIC_ANALYZER_PARAM );

    // [04/白マスの特定]:黒マスの長さがヒントの最大と等しければ両端は白マス
    static int _checkcMaxClueForBlockThenSetWhite( LOGIC_ANALYZER_PARAM );
    
private:
    CLogicAnalyzer( void ){}
    virtual ~CLogicAnalyzer( void ){}

    //===================================================================
    // 以下は[LogicAnalyzerForSearchArray.cpp]にて実装
    // 検索配列：対象のマスを中心に一定のルールで全てのマスのインデックスを格納したもの
    //===================================================================
public:
    // 検索配列：十字の直線上に検索
    static void SearchArraySetForLineUDLR( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );
    static void SearchArraySetForLineDURL( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );
    static void SearchArraySetForLineLRUD( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );
    static void SearchArraySetForLineRLDU( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );

    // 検索配列：時計回りに検索
    static void SearchArraySetForClockwiseU( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );
    static void SearchArraySetForClockwiseD( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );

    // 検索配列：反時計回りに検索
    static void SearchArraySetForCounterClockwiseU( int* pArr, int numCol, int numRow, int targetCol, int targetRow,
                                                    int logicId, int cycle );
    static void SearchArraySetForCounterClockwiseD( int* pArr, int numCol, int numRow, int targetCol, int targetRow,
                                                    int logicId, int cycle );

    // 検索配列：ランダムに検索
    static void SearchArraySetForRandomN( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );
    static void SearchArraySetForRandomM( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );
    static void SearchArraySetForRandomF( int* pArr, int numCol, int numRow, int targetCol, int targetRow, int logicId, int cycle );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_ANALYZER_HPP__
