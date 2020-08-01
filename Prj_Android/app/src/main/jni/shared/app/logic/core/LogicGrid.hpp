/*+----------------------------------------------------------------+
  |	Title:		LogicGrid.hpp [共通環境]
  |	Comment:	ロジック（コア）：グリッド（盤面全体）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_GRID_HPP__
#define __LOGIC_GRID_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicClue.hpp"
#include "LogicSolid.hpp"

#include "app/logic/log/LogicLog.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジック（コア）：グリッド（盤面全体）
//----------------------------------------
class CLogicGrid {
protected:
    //-------------------------------------------------------------
    // ロジック構成データ
    //-------------------------------------------------------------
    int m_nColumn;      // 行数（※何行あるか＝Y＝上下のサイズ）
    int m_nRow;         // 列数（※何列あるか＝X＝左右のサイズ）
    
    // マス：[m_nColumn][m_nRow]
    CLogicSolid m_oArrArrSolid[LOGIC_SOLID_WH_MAX][LOGIC_SOLID_WH_MAX];
    
    // ヒント：ヨコ方向に並ぶ数字
    int         m_nArrNumClueForColumn[LOGIC_SOLID_WH_MAX];
    CLogicClue  m_oArrArrClueForColumn[LOGIC_SOLID_WH_MAX][LOGIC_CLUE_WH_MAX];
    
    // ヒント：タテ方向に並ぶ数字
    int         m_nArrNumClueForRow[LOGIC_SOLID_WH_MAX];
    CLogicClue  m_oArrArrClueForRow[LOGIC_SOLID_WH_MAX][LOGIC_CLUE_WH_MAX];
    
    //-------------------------------------------------------------
    // ワーク
    //-------------------------------------------------------------
    // 仮定レベル（※仮定を重ねている度合い）
    int m_nAssumedLevel;

    // ヒントの参照ブロック更新管理用（※対象の行／列内の１マスでも変われば更新する＆ヒントが確定したら更新する）
    bool m_bArrUpdateClueSolidInfoForColumn[LOGIC_SOLID_WH_MAX];
    bool m_bArrUpdateClueSolidInfoForRow[LOGIC_SOLID_WH_MAX];
    
    //-------------------------------------------------
    // ヒント／ラインの確定検出用の仮定レベル（※面倒なので力づく）
    //-------------------------------------------------
    int m_nArrClueDeterminedForRow[LOGIC_SOLID_WH_MAX][LOGIC_CLUE_WH_MAX];
    int m_nArrClueDeterminedForCol[LOGIC_SOLID_WH_MAX][LOGIC_CLUE_WH_MAX];
    int m_nArrLineDeterminedForRow[LOGIC_SOLID_WH_MAX];
    int m_nArrLineDeterminedForCol[LOGIC_SOLID_WH_MAX];

public:
	CLogicGrid( void );
	virtual ~CLogicGrid( void );

    // クリア
	void clear( void );
    
    // リセット
    void reset( void );
    
    // コピー
    void copy( CLogicGrid* pData );
    
    //--------------------------------------
    // 基本サイズ
    //--------------------------------------
    inline int getColumn( void ){ return( m_nColumn ); }
    inline int getRow( void ){ return( m_nRow ); }
    int getSolidNum( bool isRow );      // タテ／ヨコのマス数の取得（※[isRow]で切り替える）

    //--------------------------------------
    // マスの取得
    //--------------------------------------
    CLogicSolid* getSolid( int column, int row );
    CLogicSolid* getSolid( bool isRow, int targetIndex, int solidAt );

    //--------------------------------------
    // マスの判定（※[仮定／確定]状態共に検出される）
    //--------------------------------------
    bool checkSolidBlank( bool isRow, int targetIndex, int solidAt, int solidOfs=0 );
    bool checkSolidBlack( bool isRow, int targetIndex, int solidAt, int solidOfs=0 );
    bool checkSolidWhite( bool isRow, int targetIndex, int solidAt, int solidOfs=0 );

    //---------------------------------------
    // マスの操作（※内部でヒントの更新が予約される）
    //---------------------------------------
    bool setSolid( bool isRow, int targetIndex, int solidAt, eLOGIC_SOLID_STATUS status, eLOGIC_SOLID_TYPE type,
                   int assumedLevel=0, bool isForce=false );
    bool setSolidBlank( bool isRow, int targetIndex, int solidAt );
    bool setSolidBlack( bool isRow, int targetIndex, int solidAt, bool isForce=false );
    bool setSolidWhite( bool isRow, int targetIndex, int solidAt, bool isForce=false );

    // 開発用の直接操作（※マスの切替）
    bool swapSolidForDev( int column, int row );
    
    //---------------------------------------
    // 判定（※[仮定／確定]状態共に検出される）
    //（※基本的に[fixClueAll]後に呼ばれる想定）
    //---------------------------------------
    // 指定のラインが完成しているか？
    bool checkLineCompleted( bool isRow, int targetIndex );
    
    // 指定のマスが一意となるヒントがある場合にそのインデックスを返す（※一意ではない場合は負の値が返る）
    int checkUniqueBlockIndexWithSolidAt( bool isRow, int targetIndex, int solidAt );
    
    // 指定のマスが孤立（※どのブロックにも含まれない）しているか？
    bool checkSolidIsolatedAt( bool isRow, int targetIndex, int solidAt );
    
    // 指定のラインが矛盾しているか？
    bool checkImcompatibleAt( bool isRow, int targetIndex );
    
    // クリア判定
    bool checkClear( void );
    
    // 矛盾判定
    bool checkImcompatible( void );
    
    //-------------------------------------------
    // 仮定関連
    //-------------------------------------------
    // 取得
    inline bool isAssumed( void ){ return( m_nAssumedLevel > 0 ); }
    inline int getAssumedLevel( void ){ return( m_nAssumedLevel ); }
 
    // 仮定開始（※内部でヒントの更新が予約される）
    bool setAssumedOnWithLog( CLogicLog* pLog, bool isRow, int targetIndex, int solidAt, bool isBlack );
    
    // 仮定終了（※内部でヒントの更新が予約される）
    bool setAssumedOffForDeterminedWithLog( CLogicLog* pLog, int* pScore );
    bool setAssumedOffForImcompatibleWithLog( CLogicLog* pLog, int* pScore );
    bool setAssumedOffForAbortedWithLog( CLogicLog* pLog, int* pScore );

    //===============================================================
    // 以下は[CLogicGridForClue.cpp]にて実装
    //===============================================================
public:
    // 取得：ヨコ
    int getClueNumForColumn( int targetIndex );
    CLogicClue* getClueForColumn( int targetIndex, int clueAt );
    
    // 取得：タテ
    int getClueNumForRow( int targetIndex );
    CLogicClue* getClueForRow( int targetIndex, int clueAt );
    
    // サイズと配列の取得（※これで取得したヒントのブロック情報が最新である保証はない点に注意）
    bool getClueArray( bool isRow, int targetIndex, CLogicClue** ppClue, int* pSize );
    
    //-------------------------------------------
    // ヒント状態の確定（※内部でヒントの更新が予約される）
    //-------------------------------------------
    void fixClueAll( bool isResetStatusAll );
    
    //-------------------------------------------
    // ヒント／ライン確定検出
    //-------------------------------------------
    void fixClueAndLineDeterminedWithLog( CLogicLog* pLog );

    //-----------------------------------------------------------------------------
    // 最新ヒントの取得（※ヒントが取りうるマスの範囲を参照する場合は必ずこのメソッド経由で行うこと）
    //-----------------------------------------------------------------------------
    CLogicClue* getUpdatedClue( bool isRow, int targetIndex, int clueAt );
    
    //--------------
    // 編集関連
    //--------------
    bool addClueAt( bool isRow, int targetIndex );
    bool deleteClueAt( bool isRow, int targetIndex, int clueAt );
    bool setClueAt( bool isRow, int targetIndex, int clueAt, int val );
    
    // データの出力
    bool dumpToBuf( char* buf );

    // 整合性確認
    bool checkVerification( void );
    
protected:
    // ヒントの更新予約（※これを呼ぶことで[getUpdatedClue]でヒントを取得する際に更新されるようになる）
    void updateClueSolidInfoForAll( void );
    void updateClueSolidInfoForColumnAt( int at );
    void updateClueSolidInfoForRowAt( int at );

    // ヒントの更新（実体）（※ヒントの確定判定は、[仮定]を含む場合は[仮定]として設定される）
    int fixClueDeterminedAt( bool isRow, int targetIndex );
    bool fixClueImcompatibleAt( bool isRow, int targetIndex );
   
    //===============================================================
    // 以下は[CLogicGridForInit.cpp]にて実装
    //===============================================================
public:
    // エラー解除
    void clearError( void );
    
    // 初期化（※文字列データから）
    bool init( const char* pSrcStr, char* bufError );
    
    // 初期化（※サイズ確認用）
    bool initForSize( int col, int row );
    void fixForSize( void );

protected:
    // 初期化データの整合性確認
    bool checkVerificationForInit( char* bufError );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_GRID_HPP__
