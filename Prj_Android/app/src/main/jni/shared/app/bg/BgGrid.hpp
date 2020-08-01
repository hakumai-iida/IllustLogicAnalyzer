/*+----------------------------------------------------------------+
  |	Title:		BgGrid.hpp [共通環境]
  |	Comment:	グリッド背景
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BG_GRID_HPP__
#define __BG_GRID_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "obj/Obj.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
enum eBG_GRID_TYPE{
    eBG_GRID_TYPE_INVALID = -1,         // 無効値
    
    eBG_GRID_TYPE_DEFAULT,              // 基本
    eBD_GRID_TYPE_BEZIER_CRITICAL,      // ベジェ編集：クリティカルボタン有効
    
    eBG_GRID_TYPE_BEZIER_XY,            // ベジェ編集：XY
    eBG_GRID_TYPE_BEZIER_XY_ADJ,        // ベジェ編集：XY（調整値）
    
    eBG_GRID_TYPE_BEZIER_IN_XY,         // ベジェ編集：進入XY
    eBG_GRID_TYPE_BEZIER_IN_XY_ADJ,     // ベジェ編集：進入XY（調整値）

    eBG_GRID_TYPE_BEZIER_OUT_XY,        // ベジェ編集：出立XY
    eBG_GRID_TYPE_BEZIER_OUT_XY_ADJ,    // ベジェ編集：出立XY（調整値）

    eBG_GRID_TYPE_MAX,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// グリッド背景：遠景
//----------------------------------------
class CBgGridB: public CObj{
protected:
    eBG_GRID_TYPE   m_eGridType;
    
    // 各色
    DWORD m_nDEBUG_DA_RGBA;
    DWORD m_nDEBUG_SAFE_AREA_LINE_RGBA;
    DWORD m_nDEBUG_MIN_AREA_LINE_RGBA;
    DWORD m_nDEBUG_GRID_C0_RGBA;
    DWORD m_nDEBUG_GRID_C1_RGBA;
    DWORD m_nDEBUG_GRID_RGBA;
    int   m_nDEBUG_GRID_W;
    int   m_nDEBUG_GRID_H;

public:
	// コンストラクタ＆デストラクタ
    CBgGridB( void );
	virtual ~CBgGridB( void ){}

    // クリア
    void clear( void );

    // タイプ設定
    void setGridType( eBG_GRID_TYPE type );

protected:
	virtual void onDraw0( void );
};

//----------------------------------------
// グリッド背景：近景
//----------------------------------------
class CBgGridF: public CObj{
public:
	// コンストラクタ＆デストラクタ
    CBgGridF( void );
	virtual ~CBgGridF( void ){}
    
    // クリア
    void clear( void );

    // タイプ設定
    void setGridType( eBG_GRID_TYPE type );

protected:
	virtual void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BG_GRID_HPP__
