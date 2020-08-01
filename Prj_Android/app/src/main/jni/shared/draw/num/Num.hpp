/*+----------------------------------------------------------------+
  |	Title:		Num.hpp [共通環境]
  |	Comment:	数字
  |             指定するグループデータには、
  |             [0～9,-,+]のスプライトが設定されている想定
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __NUM_HPP__
#define __NUM_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "def/NumBinData.hpp"
#include "draw/grp/GrpMgr.hpp"

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
// 数字
//----------------------------------------
class CNum{
protected:
	// データ
	eGRP	m_eGrpId;			    // 利用するグループID
	int		m_nMarginX;			    // 横方向のマージン
	bool	m_bRenderAtOnce;	    // 一括レンダー可能か？
	eDRAW_SET_TYPE m_eDrawSetType;  // 描画設定

public:
    // コンストラクタ／デストラクタ
	CNum( void );
	virtual ~CNum( void );

    // クリア
	void clear( void );

	// バイナリの適用
	bool applyBin( CNumBinData* pBin );

	// 取得
	inline eGRP getGrpId( void ){ return ( m_eGrpId ); }
    inline CGrp* getGrp( void ){ return( CGrpMgr::GetGrp( m_eGrpId ) ); }
	inline int getMarginX( void ){ return ( m_nMarginX ); }
	inline bool isRenderAtOnce( void ){ return( m_bRenderAtOnce ); }
	inline eDRAW_SET_TYPE getDrawSetType( void ){ return( m_eDrawSetType ); }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __NUM_HPP__