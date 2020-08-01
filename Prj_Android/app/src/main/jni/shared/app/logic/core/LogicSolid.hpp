/*+----------------------------------------------------------------+
  |	Title:		LogicSolid.hpp [共通環境]
  |	Comment:	ロジック（コア）：マス（単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_SOLID_HPP__
#define __LOGIC_SOLID_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "app/logic/LogicConst.hpp"

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
// ロジック（コア）：マス（単位）
//----------------------------------------
class CLogicSolid {
protected:
    eLOGIC_SOLID_STATUS     m_eStatus;  // 状態
    eLOGIC_SOLID_TYPE       m_eType;    // タイプ
    int m_nAssumedLevel;                // 仮定レベル

public:
	CLogicSolid( void );
	virtual ~CLogicSolid( void );

    // クリア
	void clear( void );
    
    // リセット
    void reset( void );
    
    // コピー
    void copy( CLogicSolid* pData );
    
    // 設定（※[reset]で破棄される流動的な値）
    bool set( eLOGIC_SOLID_STATUS status, eLOGIC_SOLID_TYPE type, int assumedLevel=0, bool isForce=false );

    // スワップ（※開発用：黒→白→空の順番でタイプをスラップする）
    bool swapForDev( eLOGIC_SOLID_STATUS status, int assumedLevel=0 );

    // 仮定の確定
    bool fixForAssumedDetermined( int assumedLevel );
    bool fixForAssumedImcpompatible( int assumedLevel );
    bool fixForAssumedAborted( int assumedLeel  );

    // 取得
    inline eLOGIC_SOLID_STATUS getStatus( void ){ return( m_eStatus ); }
    inline eLOGIC_SOLID_TYPE getType( void ){ return( m_eType ); }
    inline int getAssumedLevel( void ){ return( m_nAssumedLevel ); }

    // 判定：タイプ
    inline bool isBlank( void ){ return( m_eStatus == eLOGIC_SOLID_STATUS_INVALID ); }
    inline bool isBlack( void ){ return( m_eType == eLOGIC_SOLID_TYPE_BLACK && m_eStatus != eLOGIC_SOLID_STATUS_INVALID ); }
    inline bool isWhite( void ){ return( m_eType == eLOGIC_SOLID_TYPE_WHITE && m_eStatus != eLOGIC_SOLID_STATUS_INVALID ); }
    
    // 判定：状態
    inline bool isAssumedStart( void ){ return( m_eStatus == eLOGIC_SOLID_STATUS_ASSUMED_START ); }

    inline bool isAssumed( bool isStartIncluded=true ){
        return( m_eStatus == eLOGIC_SOLID_STATUS_ASSUMED ||
                (isStartIncluded && m_eStatus==eLOGIC_SOLID_STATUS_ASSUMED_START) );
    }

    inline bool isDetermined( void ){ return( m_eStatus == eLOGIC_SOLID_STATUS_DETERMINED ); }

    inline bool isDeterminedOrAssumed( bool isStartIncluded=true ){
        return( m_eStatus == eLOGIC_SOLID_STATUS_DETERMINED ||
                m_eStatus == eLOGIC_SOLID_STATUS_ASSUMED ||
                (isStartIncluded && m_eStatus==eLOGIC_SOLID_STATUS_ASSUMED_START) );
    }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_SOLID_HPP__
