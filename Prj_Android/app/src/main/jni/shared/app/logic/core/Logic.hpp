/*+----------------------------------------------------------------+
  |	Title:		Logic.hpp [共通環境]
  |	Comment:	ロジック：コアデータ[CLogicGrid]の管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_HPP__
#define __LOGIC_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "app/logic/core/LogicGrid.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジック（コア）：データ処理の窓口
//----------------------------------------
class CLogic{
protected:
    //--------------------------------------
    // ロジックコア関連（※データの実態）
    //--------------------------------------
    CLogicGrid m_oGrid;                 // 盤面データ
    
    //--------------------------------------
    // 状況関連
    //--------------------------------------
    eLOGIC_STATUS m_eStatus;            // プレイステータス

    //--------------------------------------
    // 操作管理
    //--------------------------------------
    int m_nSelectColumn;                // 選択中の行（ヨコ）
    int m_nSelectRow;                   // 選択中の列（タテ）
    int m_nSelectClueForColumn;         // 選択中のヒント行（ヨコ）
    int m_nSelectClueForRow;            // 選択中のヒント列（タテ）
    int m_nFocusClueForColumn;          // 選択中のヒント数字（ヨコ）（※選択済みのヒントをタップすることで切り替える）
    int m_nFocusClueForRow;             // 選択中のヒント数字（タテ）（※選択済みのヒントをタップすることで切り替える）

    //-----------------------------
    // 解析関連（※確認用）
    //-----------------------------
    int m_nSolidDetermined;             // 確定したマス数（※仮定状態のマスは計上されない）
    
    //--------------------------------------
    // 仮定関連（※確認用）
    //--------------------------------------
    int m_nAssumedTry;                  // 仮定回数
    int m_nAssumedLevelMax;             // 仮定の最大レベル
    int m_nAssumedDeterminedSolid;      // 仮定で確定したマス数
    int m_nAssumedImcompatibleSolid;    // 仮定で矛盾したマス数

    //--------------------------------------
    // 自動プレイ管理
    //--------------------------------------
    bool m_bAutoPlay;                   // 自動プレイ中か？
    bool m_bAutoForRow;                 // 自動プレイで列を処理中か？
    bool m_bAutoSelectBlackForAssume;   // 自動プレイにおける仮置きを黒で行うか？
    int  m_nAutoPointLast;              // 直前のポイント（※状況の変化検出用）
    int  m_nAutoPointCur;               // 現在のポイント
    int  m_nAutoColumnCur;              // 現在の処理対象：行（ヨコ）
    int  m_nAutoRowCur;                 // 現在の処理対象：列（タテ）
    
public:
    // コンストラクタ／デストラクタ
	CLogic( void );
	virtual ~CLogic( void );

    // クリア
    void clear( void );
    
    // リセット
    void reset( void );
    
    // 取得
    inline CLogicGrid* getGrid( void ){ return( &m_oGrid ); }
    inline int getColumn( void ){ return( m_oGrid.getColumn() ); }
    inline int getRow( void ){ return( m_oGrid.getRow() ); }

    inline eLOGIC_STATUS getStatus( void ){ return( m_eStatus ); }
    
    inline int getSelectColumn( void ){ return( m_nSelectColumn ); }
    inline int getSelectRow( void ){ return( m_nSelectRow ); }
    inline int getSelectClueForColumn( void ){ return( m_nSelectClueForColumn ); }
    inline int getSelectClueForRow( void ){ return( m_nSelectClueForRow ); }
    inline int getFocusClueForColumn( void ){ return( m_nFocusClueForColumn ); }
    inline int getFocusClueForRow( void ){ return( m_nFocusClueForRow ); }
    
    inline int getSolidDetermined( void ){ return( m_nSolidDetermined ); }

    inline int getAssumedTry( void ){ return( m_nAssumedTry ); }
    inline int getAssumedLevelMax( void ){ return( m_nAssumedLevelMax ); }
    inline int getAssumedDeterminedSolid( void ){ return( m_nAssumedDeterminedSolid ); }
    inline int getAssumedImcompatibleSolid( void ){ return( m_nAssumedImcompatibleSolid ); }

    // 初期化
    bool initWithDataStr( const char* pDataStr, char* bufError=NULL );                      // 文字列データによる初期化
    bool initWithFile( const char* pFileName, char* bufError=NULL, bool isLocal=false );    // ファイルによる初期化
    bool initForSize( int col, int row );   // サイズ確認用初期化
    void fixForSize( void );                // サイズ確認用初期化後の調整
    
    // [盤面／ヒント(ヨコ)／ヒント(タテ)]の選択
    bool selectSolid( int selectColumn, int selectRow );
    bool selectClueForColumn( int selectColumn, bool isTapped );
    bool selectClueForRow( int selectRow, bool isTapped );
    
    // 直接選択
    inline void setSelectClueForColumn( int select, int focusAt ){
        m_nSelectClueForColumn = select;
        m_nFocusClueForColumn = focusAt;
    }

    inline void setSelectClueForRow( int select, int focusAt ){
        m_nSelectClueForRow = select;
        m_nFocusClueForRow = focusAt;
    }

    //---------------------------------------------------------------------
    // なんらかの盤面変化が伴う処理では[CLogicLog]を渡し、内容の変更をログ経由で受け取る
    //---------------------------------------------------------------------
    // 状況確定（※盤面の状況が逆戻りする場合はステータスのリセットを指定すること）
    void fixStatusWithLog( CLogicLog* pLog, bool isResetStatusAll=false );

    // 直接操作（※返値はマスが変化したかのフラグ）
    bool playManualWithLog( CLogicLog* pLog );

    // 解析（※返値は確定したマス数）
    int analyzeWithLog( CLogicLog* pLog, bool* arrFlag=NULL, int size=0 );

    //------------------------------------------------------------------
    // その他
    //------------------------------------------------------------------
    // ログ表示
    void drawLog( float x, float y );
    
    //---------------------------------------------------
    // 編集機能
    //---------------------------------------------------
    bool prevClue( void );
    bool nextClue( void );
    bool addClue( void );
    bool deleteClue( void );
    bool setClue( int val );

    //======================================================
    // 以下は[LogicForAssume.cpp]にて実装
    //======================================================
public:
    // 判定
    inline bool isAssumedOn( void ){ return( m_oGrid.isAssumed() ); }

    // 仮定オン／オフ（※返値は変化があったかのフラグ）
    bool assumeOnWithLog( CLogicLog* pLog, bool isBlack );
    bool assumeOffWithLog( CLogicLog* pLog );
    
protected:
    // 仮定リセット
    void resetAssume( void );

    //======================================================
    // 以下は[LogicForAuto.cpp]にて実装
    //======================================================
public:
    // 判定
    inline bool isAutoOn( void ){ return( m_bAutoPlay ); }
    
    // 仮定のマス色指定
    inline void setAutoSelectBlackForAssume( bool flag ){ m_bAutoSelectBlackForAssume = flag; }

    // オートオン／オフ（※返値は変化があったかのフラグ）
    bool setAutoOn( void );
    bool setAutoOff( void );

    // 自動操作（※返値はオートプレイが終了したかのフラグ）
    bool playAutoWithLog( CLogicLog* pLog );

protected:
    // オートリセット
    void resetAuto( void );

    // オートプレイ実体
    bool updateForAutoWithLog( CLogicLog* pLog );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __LOGIC_HPP__ */
