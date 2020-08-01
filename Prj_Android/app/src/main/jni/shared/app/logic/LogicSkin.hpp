/*+----------------------------------------------------------------+
  |	Title:		LogicSkin.hpp [共通環境]
  |	Comment:	ロジック：スキン要素基底（※画面の見た目を差し替える場合に継承）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_SKIN_HPP__
#define __LOGIC_SKIN_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------
// スキンタイプ（※盤面表示の切替用）
//----------------------------
enum eLOGIC_SKIN_TYPE{
    eLOGIC_SKIN_TYPE_INVALID = -1,          // 無効
    
    eLOGIC_SKIN_TYPE_NORMAL,                //
    
    eLOGIC_SKIN_TYPE_MAX
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ロジック：スキン
//----------------------------------------
class CLogicSkin{
protected:
    // スキンタイプ
    eLOGIC_SKIN_TYPE m_eSkinType;
    
public:
    // コンストラクタ／デストラクタ
	CLogicSkin( void );
    virtual ~CLogicSkin( void ){}
    
    // クリア
    void clearLogicSkin( void );

    // スキン
    virtual void setLogicSkinType( eLOGIC_SKIN_TYPE skinType );
    virtual eLOGIC_SKIN_TYPE getLogicSkinType( void );
    
protected:
    // スキンに対する確定
    virtual void fixForSkinType( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelLogicSkinType[];

#endif	// __LOGIC_SKIN_HPP__
