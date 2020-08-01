/*+----------------------------------------------------------------+
  |	Title:		DefTable.cpp [共通環境]
  |	Comment:	定義テーブル
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DefTable.hpp"

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
CDefBinTable<CShaderBinData>* CDefTable::s_pShaderTable;
CDefBinTable<CFrameBufferBinData>* CDefTable::s_pFrameBufferTable;
CDefBinTable<CImgBinData>* CDefTable::s_pImgTable;
CDefBinTable<CSprBinData>* CDefTable::s_pSprTable;
CDefBinTable<CGrpBinData>* CDefTable::s_pGrpTable;
CDefBinTable<CNumBinData>* CDefTable::s_pNumTable;
CDefBinTable<CFontBinData>* CDefTable::s_pFontTable;
CDefBinTable<CPalOfsBinData>* CDefTable::s_pPalOfsTable;
CDefBinTable<CBrushBinData>* CDefTable::s_pBrushTable;
CDefBinTable<CBucketBinData>* CDefTable::s_pBucketTable;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CDefTable::Init( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CDefTable::Init()\n" );
#endif
    
    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_CSV );
    s_pShaderTable = new CDefBinTable<CShaderBinData>( eMEM_FIELD_D_CSV, eSHADER_MAX, "ShaderTable" );
    s_pFrameBufferTable = new CDefBinTable<CFrameBufferBinData>( eMEM_FIELD_D_CSV, eFRAME_BUFFER_MAX, "FrameBufferTable" );
    s_pImgTable = new CDefBinTable<CImgBinData>( eMEM_FIELD_D_CSV, eIMG_MAX, "ImgTable" );
    s_pSprTable = new CDefBinTable<CSprBinData>( eMEM_FIELD_D_CSV, eSPR_MAX, "SprTable" );
    s_pGrpTable = new CDefBinTable<CGrpBinData>( eMEM_FIELD_D_CSV, eGRP_MAX, "GrpTable" );
    s_pNumTable = new CDefBinTable<CNumBinData>( eMEM_FIELD_D_CSV, eNUM_MAX, "NumTable" );
    s_pFontTable = new CDefBinTable<CFontBinData>( eMEM_FIELD_D_CSV, eFONT_MAX, "FontTable" );
    s_pPalOfsTable = new CDefBinTable<CPalOfsBinData>( eMEM_FIELD_D_CSV, ePAL_OFS_MAX, "PalOfsTable" );
    s_pBrushTable = new CDefBinTable<CBrushBinData>( eMEM_FIELD_D_CSV, eBRUSH_MAX, "BrushTable" );
    s_pBucketTable = new CDefBinTable<CBucketBinData>( eMEM_FIELD_D_CSV, eBUCKET_MAX, "BucketTable" );
    CMemMgr::PopTargetField();
    
    return( ReadAll() );
}

//------------------------
// 終了
//------------------------
void CDefTable::Exit( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CDefTable::Exit()\n" );
#endif

    SAFE_DELETE( s_pShaderTable );
    SAFE_DELETE( s_pFrameBufferTable );
    SAFE_DELETE( s_pImgTable );
    SAFE_DELETE( s_pSprTable );
    SAFE_DELETE( s_pGrpTable );
    SAFE_DELETE( s_pNumTable );
    SAFE_DELETE( s_pFontTable );
    SAFE_DELETE( s_pPalOfsTable );
    SAFE_DELETE( s_pBrushTable );
    SAFE_DELETE( s_pBucketTable );
}

//------------------------
// 全読み込み
//------------------------
bool CDefTable::ReadAll( void ){
    //-----------------------------
    // 各テーブルの読み込み
    //-----------------------------
    // for [Shader]
    if( ! s_pShaderTable->read( "bin/ShaderData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [FrameBuffer]
    if( ! s_pFrameBufferTable->read( "bin/FrameBufferData.bin" ) ){
        return( false );    // FATAL ERROR
    }
    
    // for [Img]
    if( ! s_pImgTable->read( "bin/ImgData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [Spr]
    if( ! s_pSprTable->read( "bin/SprData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [Grp]
    if( ! s_pGrpTable->read( "bin/GrpData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [Num]
    if( ! s_pNumTable->read( "bin/NumData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [Font]
    if( ! s_pFontTable->read( "bin/FontData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [PalOfs]
    if( ! s_pPalOfsTable->read( "bin/PalOfsData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [Brush]
    if( ! s_pBrushTable->read( "bin/BrushData.bin" ) ){
        return( false );    // FATAL ERROR
    }

    // for [Bucket]
    if( ! s_pBucketTable->read( "bin/BucketData.bin" ) ){
        return( false );    // FATAL ERROR
    }
   
    return( true );
}

//------------------------
// パレット値の取得
//------------------------
BYTE CDefTable::GetPalOfsValue( ePAL_OFS id ){
    if( IS_PAL_OFS_VALID( id ) ){
        CPalOfsBinData* pBin = s_pPalOfsTable->getDataAt( id );
        if( pBin != NULL ){
            return( pBin->getPalOfs() );
        }
    }
    return( 0 );
}
