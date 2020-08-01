/*+----------------------------------------------------------------+
  |	Title:		LogicClue.hpp [共通環境]
  |	Comment:	ロジック（コア）：ヒント数字（単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_CLUE_HPP__
#define __LOGIC_CLUE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "app/logic/log/LogicClueLogCell.hpp"

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
// ロジック（コア）：ヒント（単位）
//----------------------------------------
class CLogicClue {
protected:
    eLOGIC_CLUE_STATUS  m_eStatus;      // 状態
    int m_nVal;                         // ヒントの数字
    int m_nAssumedLevel;                // 仮定レベル

    // ヒントの数字に対するマス目の参照範囲情報
    int m_nSolidStartAt;                // 参照開始位置
    int m_nSolidNum;                    // 参照数
    
    // ヒントのログ（※実体でもつ）
    int m_nCurLogAt;
    CLogicClueLogCell m_oArrLogCell[LOGIC_CLUE_LOG_CELL_MAX];
    
    // エラー表示用
    bool m_bError;

public:
	CLogicClue( void );
	virtual ~CLogicClue( void );
    
    // クリア
	void clear( void );
    
    // リセット
    void reset( void );
    
    // コピー
    void copy( CLogicClue* pData );
    
    // 値の設定（※[clear]でのみ破棄される＝[reset]では保持される）
    inline void setVal( int val ){ m_nVal = val; }

    // 状態設定
    bool setStatus( eLOGIC_CLUE_STATUS status, int assumedLevel=0, bool isForce=false );
    
    // 状態のリセット（※ブロック情報を保持したままステータスをリセットしたい場合）
    void resetStatus( void );
    
    // 取得
    inline eLOGIC_CLUE_STATUS getStatus( void ){ return( m_eStatus ); }
    inline int getVal( void ){ return( m_nVal ); }

    // 判定
    inline bool isUnsettled( void ){ return( m_eStatus == eLOGIC_CLUE_STATUS_INVALID ); }
    inline bool isDetermined( void ){ return( m_eStatus == eLOGIC_CLUE_STATUS_DETERMINED ); }
    inline bool isAssumed( void ){ return( m_eStatus == eLOGIC_CLUE_STATUS_ASSUMED ); }
    inline bool isDeterminedOrAssumed( void ){ return( m_eStatus == eLOGIC_CLUE_STATUS_DETERMINED ||
                                                       m_eStatus == eLOGIC_CLUE_STATUS_ASSUMED ); }
    inline bool isImcompatible( void ){ return( m_eStatus == eLOGIC_CLUE_STATUS_IMCOMPATIBLE ); } 

    //------------------------------------------
    // マスの参照範囲関連
    //------------------------------------------
    // 設定
    void setSolidBlock( int start, int num );
    inline void setSolidStartAt( int at ){ m_nSolidStartAt = at; }
    inline void setSolidNum( int num ){ m_nSolidNum = num; }

    // 調整
    void addSolidStartAt( int add );
    void addSolidNum( int add );

    // 取得
    inline int getSolidStartAt( void ){ return( m_nSolidStartAt ); }
    inline int getSolidEndAt( void ){ return( m_nSolidStartAt + m_nSolidNum - 1 ); }
    inline int getSolidNum( void ){ return( (m_nVal>0)? m_nSolidNum: 0 ); }
    
    // 判定
    inline bool isSolidContained( int solidAt ){ return( solidAt >= m_nSolidStartAt &&
                                                         solidAt < (m_nSolidStartAt+m_nSolidNum) ); }
    
    // 整合性確認（※指定のマス数がヒントに収まるか？）
    bool checkVerification( int solidNum );
    
    // エラー
    inline void setError( bool flag ){ m_bError = flag; }
    inline bool isError( void ){ return( m_bError ); }

    //----------------------------------
    // ログ関連
    //----------------------------------
    // リセット
    void resetLog( void );
    
    // 追加＆調整
    bool addLogWithCurVal( int type );
    void fixLog( int addForStartAt );
    
    // 取得
    inline int getLogNum( void ){ return( m_nCurLogAt ); }
    CLogicClueLogCell* getLogCellAt( int at );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_CLUE_HPP__
