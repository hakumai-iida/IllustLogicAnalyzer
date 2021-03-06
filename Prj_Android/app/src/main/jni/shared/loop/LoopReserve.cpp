/*+----------------------------------------------------------------+
  |	Title:		LoopReserve.cpp [共通環境]
  |	Comment:	ループ予約
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LoopReserve.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// コンストラクタ
//------------------------
CLoopReserve::CLoopReserve( void ){
	clear();
}

//------------------------
// クリア
//------------------------
void CLoopReserve::clear( void ){
	m_pLoop = NULL;
	m_eReserveType = eLOOP_RESERVE_INVALID;
	m_nLoopId = eLOOP_ID_INVALID;
	m_nOption = 0;
}

//------------------------
// 設定
//------------------------
void CLoopReserve::set( CLoop* pLoop, eLOOP_RESERVE reserveType, int loopId, int option ){
	m_pLoop = pLoop;
	m_eReserveType = reserveType;
	m_nLoopId = loopId;
	m_nOption = option;
}

//------------------------
// 有効性判定
//------------------------
bool CLoopReserve::isValid( void ){
	return( m_eReserveType != eLOOP_RESERVE_INVALID );
}
