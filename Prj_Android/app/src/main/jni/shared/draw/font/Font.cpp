/*+----------------------------------------------------------------+
  |	Title:		Font.cpp [共通環境]
  |	Comment:	フォント
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Font.hpp"
#include "def/FontSprBinData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ASCIIオンリーの際のデータ配列長
#define	ASCII_LEN	128

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
CFont::CFont( void ){
	clear();
}

//------------------------
// デストラクタ
//------------------------
CFont::~CFont( void ){
	clear();
}

//-------------------
// クリア
//-------------------
void CFont::clear( void ){
    // 確保領域の解放
    SAFE_DELETE_ARRAY( m_oArrSprFont );
    SAFE_DELETE_ARRAY( m_cArrLabelBuf );
    SAFE_DELETE_ARRAY( m_pArrLabel );

    // クリア
	m_nCharNum = 0;

	m_nLineHeight = 0;
	m_nLineBaseY = 0;
	m_bAsciiOnly = false;
	m_bRenderAtOnce = false;
	m_eDrawSetType = (eDRAW_SET_TYPE)0;
}

//-----------------------------------------------------------------
// バイナリの適用
// [CFontSprBinData]の値は読み捨て（※保持しておいても使わない＆量が多いので）
//-----------------------------------------------------------------
bool CFont::applyBin( CFontBinData* pBin ){
    const char* pFileName = pBin->getFileName();

	// データ読み込み
	void* pBuf = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_DATA );
	uint32 size = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_DATA );
	uint32 readed;
	if( ! CFileMgr::LoadFile( pFileName, pBuf, size, &readed ) ){
		LOGE( "@ CFont::setFromFile: LOAD FAILED: %s\n", pFileName );

		char* log = CMemMgr::GetBufFromTempStr();
		sprintf( log, "[%s]の読み込みに失敗しました。\n", pFileName );
		env_push_error_log( log );
		return( false );	// FATAL ERROR
	}

	// 入力バッファ（※リトルエンディアン）
	CInputBuffer ib;
	ib.setBuffer( pBuf, readed );
	ib.setForLittleEndian();

	// ファイルヘッダ確認
	CFileHeader fh;
    fh.read( &ib );
    
    // 要素サイズ確認
    int dataSize = CFontSprBinData::GetBinSize();
    if( fh.getDataSize() != dataSize ){
        LOGE( "@ CFont: DATA SIZE ERROR( %d != %u)\n", fh.getDataSize(), dataSize );
        char* buf = CMemMgr::GetBufFromTempStr();
        sprintf( buf, "[%s]のデータサイズが不正です\n", pFileName );
        env_push_error_log( buf );
        return( false );    // FATAL ERROR
    }

    // 要素数（※各フォントの文字数はデータサイズから算出する）
    int num = (readed-fh.getFileHeaderSize()) / dataSize;
    
	CSpr* arrSpr;
	char* pLabelBuf;
	char** pArrLabel;
    CFontSprBinData binSpr;

	// アスキーだけかどうかで処理の分岐
	if( pBin->isAsciiOnly() ){
		// [Ascii]だけのデータではマッチング配列として[0x00～0x7F]分確保
		CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
		arrSpr = new CSpr[ASCII_LEN];
		pLabelBuf = new char[ASCII_LEN*FONT_SPR_LABEL_LEN];
		pArrLabel = new char*[ASCII_LEN+1];
		CMemMgr::PopTargetField();

		// ラベル枠の設定
		for( int i=0; i<ASCII_LEN; i++ ){
			pArrLabel[i] = &pLabelBuf[i*FONT_SPR_LABEL_LEN];
			sprintf( pArrLabel[i], "%d:", i );
		}

		// データ読み込み
		for( int i=0; i<num; i++ ){
            binSpr.read( &ib );
            unsigned int code = binSpr.getCode();
            const char* pLabel = binSpr.getLabel();

			// ラベル設定
			sprintf( pArrLabel[code], "%u:%s", code, pLabel );

			// スプライト設定
			if( ! arrSpr[code].applyBinForChar( pBin, &binSpr ) ){
				LOGE( "@ CFont::setFromFile(%s): INVALID CHAR=%d(%s)\n", pFileName, code, pLabel );

				char* log = CMemMgr::GetBufFromTempStr();
				sprintf( log, "フォントスプライト[%s]の設定に失敗しました。\n", pLabel );
				env_push_error_log( log );
				return( false );	// FATAL ERROR
			}
		}

		// 個数上書き
		num = ASCII_LEN;
	}else{
		CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
		arrSpr = new CSpr[num];
		pLabelBuf = new char[num*FONT_SPR_LABEL_LEN];
		pArrLabel = new char*[num+1];
		CMemMgr::PopTargetField();

		// データ読み込み
		for( int i=0; i<num; i++ ){
            binSpr.read( &ib );
            unsigned int code = binSpr.getCode();
            const char* pLabel = binSpr.getLabel();

			// ラベル設定
			pArrLabel[i] = &pLabelBuf[i*FONT_SPR_LABEL_LEN];
			sprintf( pArrLabel[i], "%u:%s", code, pLabel );

			// スプライト設定
			if( ! arrSpr[i].applyBinForChar( pBin, &binSpr ) ){
				LOGE( "@ CFont::setFromFile(%s): INVALID CHAR=%d(%s)\n", pFileName, code, pLabel );

				char* buf = CMemMgr::GetBufFromTempStr();
				sprintf( buf, "フォントスプライト[%s]の設定に失敗しました。\n", pLabel );
				env_push_error_log( buf );
				return( false );	// FATAL ERROR
			}
		}
	}

	// 終端指定
	pArrLabel[num] = NULL;

	// ここまできたら設定
	m_nCharNum = num;
	m_oArrSprFont = arrSpr;
	m_pArrLabel = pArrLabel;
	m_cArrLabelBuf = pLabelBuf;

	m_nLineHeight = pBin->getLineH();
	m_nLineBaseY = pBin->getLineY();
	m_bAsciiOnly = pBin->isAsciiOnly();
	m_bRenderAtOnce = pBin->isRenderAtOnce();
	m_eDrawSetType = pBin->getDrawSetType();;

	return( true );
}

//----------------------------
// フォント用スプライトの取得
//----------------------------
CSpr* CFont::getSprChar( int c ){
	// アスキー文字だけであれば対応表を参照
	if( m_bAsciiOnly ){
		if( c >= 0 && c < ASCII_LEN ){
			if( m_oArrSprFont[c].isValid() ){
				return( &m_oArrSprFont[c] );
			}
		}
	}
	// それ以外の文字を含む場合は総当たり
	else{
		for( int i=0; i<m_nCharNum; i++ ){
			if( m_oArrSprFont[i].getCharCode() == c ){
				return( &m_oArrSprFont[i] );
			}
		}
	}

	// ここまできたら無効文字（※０が無効文字であると想定）
	return( &m_oArrSprFont[0] );
}

//-----------------------------------
// フォント用スプライトの取得（直接）
//-----------------------------------
CSpr* CFont::getSprCharAt( int at ){
	if( at>=0 && at<m_nCharNum && m_oArrSprFont[at].isValid() ){
		return( &m_oArrSprFont[at] );
	}

	// ここまできたら無効文字（※０が無効文字であると想定）
	return( &m_oArrSprFont[0] );
}

//--------------------------
// デバッグ文字列の取得
//--------------------------
const char** CFont::getCharLabelArray( void ){
	return( (const char**) m_pArrLabel );
}
