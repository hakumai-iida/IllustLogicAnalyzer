/*+----------------------------------------------------------------+
  |	Title:		DefTable.hpp [共通環境]
  |	Comment:	定義テーブル
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DEF_TABLE_HPP__
#define __DEF_TABLE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "DefBinTable.hpp"

#include "ShaderBinData.hpp"
#include "FrameBufferBinData.hpp"
#include "ImgBinData.hpp"
#include "SprBinData.hpp"
#include "GrpBinData.hpp"
#include "NumBinData.hpp"
#include "FontBinData.hpp"
//#include "FontSprBinDatav.hpp"     // [CFontSprBinData]は[CFont::applyBin]内のみで読み捨て利用される
#include "PalOfsBinData.hpp"
#include "BrushBinData.hpp"
#include "BucketBinData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 定義テーブル
//-----------------------------------------------------------------
class CDefTable{
private:
    static CDefBinTable<CShaderBinData>* s_pShaderTable;
    static CDefBinTable<CFrameBufferBinData>* s_pFrameBufferTable;
    static CDefBinTable<CImgBinData>* s_pImgTable;
    static CDefBinTable<CSprBinData>* s_pSprTable;
    static CDefBinTable<CGrpBinData>* s_pGrpTable;
    static CDefBinTable<CNumBinData>* s_pNumTable;
    static CDefBinTable<CFontBinData>* s_pFontTable;
    static CDefBinTable<CPalOfsBinData>* s_pPalOfsTable;
    static CDefBinTable<CBrushBinData>* s_pBrushTable;
    static CDefBinTable<CBucketBinData>* s_pBucketTable;

public:
    // 初期化／終了
    static bool Init( void );
    static void Exit( void );
    
    // 全読み込み
    static bool ReadAll( void );
    
    // テーブル取得
    inline static CDefBinTable<CShaderBinData>* GetShaderTable( void ){ return( s_pShaderTable ); }
    inline static CDefBinTable<CFrameBufferBinData>* GetFrameBufferTable( void ){ return( s_pFrameBufferTable ); }
    inline static CDefBinTable<CImgBinData>* GetImgTable( void ){ return( s_pImgTable ); }
    inline static CDefBinTable<CSprBinData>* GetSprTable( void ){ return( s_pSprTable ); }
    inline static CDefBinTable<CGrpBinData>* GetGrpTable( void ){ return( s_pGrpTable ); }
    inline static CDefBinTable<CNumBinData>* GetNumTable( void ){ return( s_pNumTable ); }
    inline static CDefBinTable<CFontBinData>* GetFontTable( void ){ return( s_pFontTable ); }
    inline static CDefBinTable<CPalOfsBinData>* GetPalOfsTable( void ){ return( s_pPalOfsTable ); }
    inline static CDefBinTable<CBrushBinData>* GetBrushTable( void ){ return( s_pBrushTable ); }
    inline static CDefBinTable<CBucketBinData>* GetBucketTable( void ){ return( s_pBucketTable ); }
    
    // 簡便のため
    static BYTE GetPalOfsValue( ePAL_OFS id );

private:
	// インスタンス作成は不可
	CDefTable( void ){}
	virtual ~CDefTable( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __DEF_TABLE_HPP__
