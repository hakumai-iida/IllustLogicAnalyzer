/*+----------------------------------------------------------------+
  |	Title:		StrokeMgr.hpp [共通環境]
  |	Comment:	ストローク管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __STROKE_MGR_HPP__
#define __STROKE_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Stroke.hpp"
#include "StrokeDot.hpp"

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
// ストローク管理
//----------------------------------------
class CStrokeMgr{
private:
    // work（※設定を反映して呼び出し元へ返すオブジェクト）
    static CStroke s_oCurStroke;

    // 基本のストローク画素
    static CStrokeDot* s_oArrStrokeDotForSquare;    // for [eSTROKE_TYPE_SQUARE]
    static CStrokeDot* s_oArrStrokeDotForCircle;    // for [eSTROKE_TYPE_CIRCLE]

public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSizeK( void );

	// 開始＆終了
    static bool OnCreate( void );
    static void OnDestroy( void );

    // 取得
    static CStroke* GetStroke( eSTROKE_TYPE type, float size, float range );

    // ストローク画素取得
    inline static CStrokeDot* GetStrokeDotForSquare( float size0 ){ return( GetStrokeDot( size0, s_oArrStrokeDotForSquare ) ); }
    inline static CStrokeDot* GetStrokeDotForCircle( float size0 ){ return( GetStrokeDot( size0, s_oArrStrokeDotForCircle ) ); }

private:
    // ストローク画素取得（実体）
    static CStrokeDot* GetStrokeDot( float size0, CStrokeDot* pArrStrokeDot );
    
    // ストローク画素ソースの生成
    static int SetDotSrcForSquare( BYTE* pBuf, float size0 );
    static int SetDotSrcForCircle( BYTE* pBuf, float size0 );
    
    // インスタンス作成は不可
    CStrokeMgr( void ){}
    virtual ~CStrokeMgr( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __STROKE_MGR_HPP__
