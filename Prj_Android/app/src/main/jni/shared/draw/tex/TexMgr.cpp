/*+----------------------------------------------------------------+
  |	Title:		CTexMgr.cpp [共通環境]
  |	Comment:	テクスチャ管理（※ベジェで作成したテクスチャの管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TexMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// データフォルダ名
#define BDD_FOLDER_PATH			"bdd"

// BMP名のカット位置
#define BMP_NAME_HEADER_PIVOT	6	// [bmpXX_]

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// バンクテクスチャ
CTex** CTexMgr::s_oArrArrBankTex;
char*** CTexMgr::s_pArrArrBankTexName;
char** CTexMgr::s_pArrBankTexNameBuf;
int** CTexMgr::s_nArrArrBankTexType;
int* CTexMgr::s_nArrBankTexNum;

// 登録テクスチャ
CTex** CTexMgr::s_oArrArrRegistTex;

// キャラパレット：羊から設定
CTex** CTexMgr::s_pArrCharaColorPalColor;
CTex** CTexMgr::s_pArrCharaColorPalLine;

// 各色
uint32* CTexMgr::s_nArrCharaColorForWordTop;
uint32* CTexMgr::s_nArrCharaColorForWordTopOL;
uint32* CTexMgr::s_nArrCharaColorForWord;
uint32* CTexMgr::s_nArrCharaColorForWordOL;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CTexMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexMgr::OnCreate()\n" );
#endif

	if( !CreateAllForBank() ){
		LOGE( "@ CTexMgr::CreateAllForBank(): FAILED!!\n" );
		env_push_error_log( "[CTexMgr::CreateAllForBank()]に失敗しました\n" );
		return( false );	// FATAL ERROR
	}

	if( !CreateAllForRegist() ){
		LOGE( "@ CTexMgr::CreateAllForRegist(): FAILED!!\n" );
		env_push_error_log( "[CTexMgr::CreateAllForRegist()]に失敗しました\n" );
		return( false );	// FATAL ERROR
	}

	if( !CreateAllForCharaColor() ){
		LOGE( "@ CTexMgr::CreateAllForCharaColor(): FAILED!!\n" );
		env_push_error_log( "[CTexMgr::CreateAllForCharaColor()]に失敗しました\n" );
		return( false );	// FATAL ERROR
	}

	return( true );
}

//------------------------
// 終了
//------------------------
void CTexMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexMgr::OnDestroy()\n" );
#endif

	// バンクテクスチャデータ解放
	if( s_oArrArrBankTex != NULL ){
		for( int i=0; i<eBANK_MAX; i++ ){
			SAFE_DELETE_ARRAY( s_oArrArrBankTex[i] );
		}
		SAFE_DELETE_ARRAY( s_oArrArrBankTex );
	}

	if( s_pArrArrBankTexName != NULL ){
		for( int i=0; i<eBANK_MAX; i++ ){
			// バッファの実体は[s_pArrBankTexNameBuf]
			SAFE_DELETE_ARRAY( s_pArrArrBankTexName[i] );
		}
		SAFE_DELETE_ARRAY( s_pArrArrBankTexName );
	}

	if( s_pArrBankTexNameBuf != NULL ){
		for( int i=0; i<eBANK_MAX; i++ ){
			SAFE_DELETE_ARRAY( s_pArrBankTexNameBuf[i] );
		}
		SAFE_DELETE_ARRAY( s_pArrBankTexNameBuf );
	}

	if( s_nArrArrBankTexType != NULL ){
		for( int i=0; i<eBANK_MAX; i++ ){
			SAFE_DELETE_ARRAY( s_nArrArrBankTexType[i] );
		}
		SAFE_DELETE_ARRAY( s_nArrArrBankTexType );
	}

	SAFE_DELETE_ARRAY( s_nArrBankTexNum );

	// 登録テクスチャデータ解放
	if( s_oArrArrRegistTex != NULL ){
		for( int i=0; i<eREGIST_TEX_BANK_MAX; i++ ){
			SAFE_DELETE_ARRAY( s_oArrArrRegistTex[i] );
		}
		SAFE_DELETE_ARRAY( s_oArrArrRegistTex );
	}

	// キャラパレット解放
	SAFE_DELETE_ARRAY( s_pArrCharaColorPalColor );
	SAFE_DELETE_ARRAY( s_pArrCharaColorPalLine );

	// 色解放
	SAFE_DELETE_ARRAY( s_nArrCharaColorForWordTop );
	SAFE_DELETE_ARRAY( s_nArrCharaColorForWordTopOL );
	SAFE_DELETE_ARRAY( s_nArrCharaColorForWord );
	SAFE_DELETE_ARRAY( s_nArrCharaColorForWordOL );
}

//---------------------------
// バンクテクスチャデータ作成
//---------------------------
bool CTexMgr::CreateAllForBank( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexMgr::CreateAllForBank()\n" );
#endif

	// バンク管理データ枠の確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
	s_oArrArrBankTex = new CTex*[eBANK_MAX];
	s_pArrArrBankTexName = new char**[eBANK_MAX];
	s_pArrBankTexNameBuf = new char*[eBANK_MAX];
	s_nArrArrBankTexType = new int*[eBANK_MAX];
	s_nArrBankTexNum = new int[eBANK_MAX];
	CMemMgr::PopTargetField();

	// テクスチャ準備
	for( int i=0; i<eBANK_MAX; i++ ){
#if 0
		if( !ReadBankTexAt( (eBANK)i, s_pArrBddPackName[i] ) ){
			LOGE( "@ CTexMgr::CreateAllForBank: INVALID TEX: %s\n", g_pArrLabelBank[i] );

			char* log = CMemMgr::GetBufFromTempStr();
			sprintf( log, "バンクテクスチャ[%s]の設定に失敗しました\n", g_pArrLabelBank[i] );
			env_push_error_log( log );
			return( false );	// FATAL ERROR
		}
#endif
	}

	return( true );
}

//---------------------------------
// 指定バンクにテクスチャの読み込み
//---------------------------------
bool CTexMgr::ReadBankTexAt( eBANK bank, const char* pFileName ){
#if 0
	stFPACK fp;
	fpack_init( &fp );

	// パックファイル読み込み（一括）
	if( !fpack_set( &fp, pFileName, true ) ){
		LOGE( "@ CTexMgr::ReadBankTexAt(%d, %s) FILE NOT FOUND\n", bank, pFileName );

		char* log = CMemMgr::GetBufFromTempStr();
		sprintf( log, "パックファイル[%s]が開けません\n", pFileName );
		env_push_error_log( log );
		return( false );	// FATAL ERROR
	}

	// データ読み込み
	int num = fpack_get_num( &fp );

	// 個数設定
	s_nArrBankTexNum[bank] = num;

	// ファイル名バッファサイズ算出
	int bufSize = 0;
	for( int i=0; i<num; i++ ){
		const char* pName = fpack_get_name( &fp, i );

		// 接頭子[bmpXX_]と拡張子[.bdd]を削除した長さが必要なサイズ
		int len = (int)strlen( pName );
		len -= BMP_NAME_HEADER_PIVOT + BMP_NAME_EXTENSION_PIVOT;
		bufSize += len + 1;	// [+1]は終端文字
	}

	// ファイル名枠確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
	s_pArrArrBankTexName[bank] = new char*[num+1];
	s_pArrBankTexNameBuf[bank] = new char[bufSize];
	s_nArrArrBankTexType[bank] = new int[num];
	CMemMgr::PopTargetField();

	// ファイル名設定
	int bufAt = 0;
	for( int i=0; i<num; i++ ){
		const char* pName = fpack_get_name( &fp, i );

		// 接頭子[bmpXX_]と拡張子[.bdd]を削除してコピー
		int len = (int)strlen( pName );
		len -= BMP_NAME_HEADER_PIVOT + BMP_NAME_EXTENSION_PIVOT;
		s_pArrArrBankTexName[bank][i] = &s_pArrBankTexNameBuf[bank][bufAt];
		strncpy( s_pArrArrBankTexName[bank][i], &pName[BMP_NAME_HEADER_PIVOT], len );
		s_pArrArrBankTexName[bank][i][len] = '\0';

		bufAt += len + 1;

		// タイプ検出
		const char* pTemp = &s_pArrArrBankTexName[bank][i][len-BMP_NAME_PART_OUTLINE_LEN];
		if( strcmp( pTemp, BMP_NAME_PART_OUTLINE ) == 0 ){
			s_nArrArrBankTexType[bank][i] = eBMP_TYPE_OUTLINE;
			continue;
		}

		pTemp = &s_pArrArrBankTexName[bank][i][len-BMP_NAME_PART_LINE_LEN];
		if( strcmp( pTemp, BMP_NAME_PART_LINE ) == 0 ){
			s_nArrArrBankTexType[bank][i] = eBMP_TYPE_LINE;
			continue;
		}

		s_nArrArrBankTexType[bank][i] = eBMP_TYPE_COLOR;
	}

	// 終端指定
	s_pArrArrBankTexName[bank][num] = NULL;

	// データ枠確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
	s_oArrArrBankTex[bank] = new CTex[num];
	CMemMgr::PopTargetField();

	// テクスチャ読み込み
	BYTE* pBuf;
	const char* pBmpName;
	int checkAt;
	bool ret;
	for( int i=0; i<num; i++ ){
		pBuf = (BYTE*) fpack_get_data( &fp, i );
		pBmpName = fpack_get_name( &fp, i );

		// ファイル名末尾で外枠かを判定
		checkAt = (int)strlen( pBmpName ) - (BMP_NAME_PART_OUTLINE_LEN+BMP_NAME_EXTENSION_PIVOT);
		ret = false;
		if( strncmp( &pBmpName[checkAt], BMP_NAME_PART_OUTLINE, BMP_NAME_PART_OUTLINE_LEN ) == 0 ){
			ret = s_oArrArrBankTex[bank][i].setFromBuf( pBuf, 0, eTEX_DATA_BDD_OL, 0, 0 );
		}else{
			eTEX_DATA texType = eTEX_DATA_BDD;

#if 1
			// TODO:エフェクト素材の指定をどうするか？
			// エフェクト素材をＯＬ扱いにしてみる（※力づく）
			if( bank == eBANK_COMMON ){
				if( strncmp( pBmpName, "bmp99_300", 9 ) == 0 ){
					texType = eTEX_DATA_BDD_OL;
				}
			}
#endif

			ret = s_oArrArrBankTex[bank][i].setFromBuf( pBuf, 0, texType, 0, 0 );

			// フキダシはキャッシュ時にαを有効（※フキダシのリンクの重ね合わせで線が目立たないように）
			if( bank == eBANK_BALLOON ){
				if( s_nArrArrBankTexType[bank][i] == eBMP_TYPE_COLOR ){
					s_oArrArrBankTex[bank][i].setUseAlphaForTexCache( true );
				}
			}
		}

		if( !ret ){
			LOGE( "@ CTexMgr::ReadBankTexAt: INVALID TEX:%s\n", pBmpName );

			char* log = CMemMgr::GetBufFromTempStr();
			sprintf( log, "テクスチャ[%s]の設定に失敗しました\n", pBmpName );
			env_push_error_log( log );
			return( false );	// FATAL ERROR
		}
	}

	// パックファイルを閉じる
	fpack_release( &fp );
#endif
	return( true );
}

//-------------------------
// 登録テクスチャデータ作成
//-------------------------
bool CTexMgr::CreateAllForRegist( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexMgr::CreateAllForRegist()\n" );
#endif

	// バンク管理データ枠の確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
	s_oArrArrRegistTex = new CTex*[eREGIST_TEX_BANK_MAX];
	CMemMgr::PopTargetField();

	// 各バンク枠確保
	int num = eREGIST_TEX_MAX * eBMP_TYPE_MAX;
	for( int i=0; i<eREGIST_TEX_BANK_MAX; i++ ){

		CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
		s_oArrArrRegistTex[i] = new CTex[num];
		CMemMgr::PopTargetField();
	}

	return( true );
}

//-------------------------------
// キャラ色作成
//-------------------------------
bool CTexMgr::CreateAllForCharaColor( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTexMgr::CreateAllForCharaColor()\n" );
#endif

	//-----------------------------------
	// 領域確保
	//-----------------------------------
	CMemMgr::PushTargetField( eMEM_FIELD_D_TEX );
	s_pArrCharaColorPalColor     = new CTex*[eCHARA_COLOR_MAX];
	s_pArrCharaColorPalLine      = new CTex*[eCHARA_COLOR_MAX];
	s_nArrCharaColorForWordTop   = new uint32[eCHARA_COLOR_MAX];
	s_nArrCharaColorForWordTopOL = new uint32[eCHARA_COLOR_MAX];
	s_nArrCharaColorForWord      = new uint32[eCHARA_COLOR_MAX];
	s_nArrCharaColorForWordOL    = new uint32[eCHARA_COLOR_MAX];
	CMemMgr::PopTargetField();

	return( true );
}

//---------------------------------------
// タイプ指定バンクテクスチャ取得
//---------------------------------------
CTex* CTexMgr::GetBankTexWithType( eBANK bank, const char*pName, eBMP_TYPE type, eREGIST_TEX_BANK registBank ){
	// 名前長の取得
	int len = (int)strlen( pName );

	// 接頭子を削除して照会
	len -= BMP_NAME_HEADER_PIVOT;

	if( len > 0 ){
		pName = &pName[BMP_NAME_HEADER_PIVOT];

		// タイプと指定文字部分が合致したらヒット
		for( int i=0; i<s_nArrBankTexNum[bank]; i++ ){
			if( type == s_nArrArrBankTexType[bank][i] ){
				if( strncmp( s_pArrArrBankTexName[bank][i], pName, len ) == 0 ){
					return( &s_oArrArrBankTex[bank][i] );
				}
			}
		}

		// 有効な文字でヒットしないのはおかしい
		LOGE( "@ CTexMgr::GetBankTexWithType( %d, %s, %d ) NOT FOUND\n", bank, pName, type );
	}

	// ここまで来たら無効画像を返しておく
    CImg* pImg = CImgMgr::GetImg( (eIMG) 0 );
    return( pImg->getDot() );
}

//---------------------
// バンクテクスチャ取得
//---------------------
CTex* CTexMgr::GetBankTexAt( eBANK bank, int at ){
	return( &s_oArrArrBankTex[bank][at] );
}

//---------------------
// バンクテクスチャ取得
//---------------------
int CTexMgr::GetBankTexTypeAt( eBANK bank, int at ){
	return( s_nArrArrBankTexType[bank][at] );
}

//---------------------
// バンクテクスチャ名配列取得
//---------------------
const char** CTexMgr::GetBankTexNameArray( eBANK bank ){
	return( (const char**)s_pArrArrBankTexName[bank] );
}

//---------------------
// バンクテクスチャ名取得
//---------------------
const char* CTexMgr::GetBankTexNameAt( eBANK bank, int at ){
	return( s_pArrArrBankTexName[bank][at] );
}

//--------------------------------------------------------
// 登録テクスチャの取得（※無効時は[NULL]を返す）
//--------------------------------------------------------
CTex* CTexMgr::GetRegistTexWithType( eREGIST_TEX_BANK registBank, eREGIST_TEX registTexId, eBMP_TYPE type ){
	// 無効は無視
	if( registBank < 0 || registBank >= eREGIST_TEX_BANK_MAX ){ return( NULL ); }
	if( registTexId < 0 || registTexId >= eREGIST_TEX_MAX ){ return( NULL ); }

	// 対象を返す
	int target = eBMP_TYPE_MAX*registTexId + type;
	return( &s_oArrArrRegistTex[registBank][target] );
}

//--------------------------------------------------------
// 登録パレット設定
//--------------------------------------------------------
void CTexMgr::SetCharaColorPalColor( eCHARA_COLOR id, CTex* pTexPal ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return; }
	s_pArrCharaColorPalColor[id] = pTexPal;
}

void CTexMgr::SetCharaColorPalLine( eCHARA_COLOR id, CTex* pTexPal ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return; }
	s_pArrCharaColorPalLine[id] = pTexPal;
}

//--------------------------------------------------------
// 登録パレット取得
//--------------------------------------------------------
CTex* CTexMgr::GetCharaColorPalColor( eCHARA_COLOR id ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return( NULL ); }
	return( s_pArrCharaColorPalColor[id] );
}

CTex* CTexMgr::GetCharaColorPalLine( eCHARA_COLOR id ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return( NULL ); }
	return( s_pArrCharaColorPalLine[id] );
}

//-------------------
// 各色設定
//-------------------
void CTexMgr::SetCharaColorForWordTop( eCHARA_COLOR id, uint32 rgba ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return; }
	s_nArrCharaColorForWordTop[id] = rgba;
}

void CTexMgr::SetCharaColorForWordTopOL( eCHARA_COLOR id, uint32 rgba ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return; }
	s_nArrCharaColorForWordTopOL[id] = rgba;
}

void CTexMgr::SetCharaColorForWord( eCHARA_COLOR id, uint32 rgba ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return; }
	s_nArrCharaColorForWord[id] = rgba;
}

void CTexMgr::SetCharaColorForWordOL( eCHARA_COLOR id, uint32 rgba ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return; }
	s_nArrCharaColorForWordOL[id] = rgba;
}

//-------------------
// 各色取得
//-------------------
uint32 CTexMgr::GetCharaColorForWordTop( eCHARA_COLOR id ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return( 0xFFFFFFFF ); }
	return( s_nArrCharaColorForWordTop[id] );
}

uint32 CTexMgr::GetCharaColorForWordTopOL( eCHARA_COLOR id ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return( 0x000000FF ); }
	return( s_nArrCharaColorForWordTopOL[id] );
}

uint32 CTexMgr::GetCharaColorForWord( eCHARA_COLOR id ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return( 0xFFFFFFFF ); }
	return( s_nArrCharaColorForWord[id] );
}

uint32 CTexMgr::GetCharaColorForWordOL( eCHARA_COLOR id ){
	if( id < 0 || id >= eCHARA_COLOR_MAX ){ return( 0x000000FF ); }
	return( s_nArrCharaColorForWordOL[id] );
}

//--------------------
// ID変換
//--------------------
eCHARA_COLOR CTexMgr::ConvCharaColorId( eBANK bank, eREGIST_TEX_BANK registBank, bool isBurning ){
    return( eCHARA_COLOR_DUMMY );
}

