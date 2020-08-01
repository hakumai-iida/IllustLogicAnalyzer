/*+----------------------------------------------------------------+
  |	Title:		LogicDispForDev.hpp [共通環境]
  |	Comment:	ロジック表示（※[CLogicGrid]の内容を直接表示する開発用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_DISP_FOR_DEV_HPP__
#define __LOGIC_DISP_FOR_DEV_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "app/logic/core/LogicGrid.hpp"

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
// ロジック：表示（※開発用の簡易なもの）
//----------------------------------------
class CLogicDispForDev{
protected:
    //-----------------------------
    // 設定値（※初期化でもらう値）
    //-----------------------------
    // 表示対象
    CLogicGrid* m_pGrid;
    
    // 表示位置（中心座標）＆最大サイズ
    float m_fOriginalCenterX, m_fOriginalCenterY;
    float m_fOriginalW, m_fOriginalH;

    // 表示オフセット
    float m_fOfsX, m_fOfsY;

    //-----------------------------
    // 表示関連（※初期化で設定する値）
    //-----------------------------
    float m_fGridScale;                 // 盤面のスケール
    float m_fGridX, m_fGridY;           // 盤面の左上座標
    float m_fGridW, m_fGridH;           // 盤面の大きさ
    float m_fGridLineSize;              // 盤面のラインサイズ
    
    float m_fClueX, m_fClueY;           // ヒント領域の左上座標
    float m_fClueW, m_fClueH;           // ヒント領域の大きさ
    
    float m_fSolidW, m_fSolidH;         // １マスの大きさ
    float m_fSolidTexW, m_fSolidTexH;   // マステクスチャの大きさ
    
    float m_fClueNumW1;                 // ヒント数字のヨコ幅（１桁）
    float m_fClueNumW2;                 // ヒント数字のヨコ幅（２桁）
    float m_fClueNumMarginX;            // ヒント数字間の余白：ヨコ
    
    float m_fClueNumH;                  // ヒント数字のタテ幅
    float m_fClueNumMarginY;            // ヒント数字間の余白：タテ
    
    //-----------------------------------
    // テクスチャ（※白マス（×）はベジェで準備）
    //-----------------------------------
    CTex* m_pTexLineForCross;
    CTex* m_pTexColorForCross;

public:
    // コンストラクタ／デストラクタ
	CLogicDispForDev( void );
	virtual ~CLogicDispForDev( void );

    // クリア
    void clear( void );
    
    // 座標基準関連
    inline float getClueX( void ){ return( m_fClueX ); }
    inline float getClueY( void ){ return( m_fClueY ); }
    
    inline void setOfsXY( float x, float y ){ m_fOfsX = x; m_fOfsY = y; }

    // 有効性の確認
    inline bool isValid( void ){ return( m_pGrid != NULL ); }

    // 初期化
    void init( CLogicGrid* pGrid, float centerX, float centerY, float w, float h );
    
    // タッチ確認
    bool checkTouchForColClue( eTOUCH_CHECK check, int *pSelectCol );
    bool checkTouchForRowClue( eTOUCH_CHECK check, int *pSelectRow );

    // 描画（※フォーカス情報は引数で受け取る）
    void draw( bool isZeroVisible,
               int focusColAt, int focusRowAt,
               int focusColClueAt, int focusRowClueAt,
               int focusColBlockAt, int focusRowBlockAt );
    
    // 座標取得
    void getXYForClue(  int focusColClueAt, int focusRowClueAt, float* pX0, float*pY0, float* pX1, float* pY1 );

protected:
    // 表示準備
    void readyForDisp( void );
    
    // テクスチャ作成
    void createTexForCross( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOGIC_DISP_FOR_DEV_HPP__
