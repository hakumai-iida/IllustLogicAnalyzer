/*+----------------------------------------------------------------+
  |	Title:		Img.cpp [共通環境]
  |	Comment:	画像
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Img.hpp"

#include "sys/util/util_endian.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 遅延時間の変換（※アニメGIFの時間単位なので1==0.01秒となる）
#define IMG_DELAY_TO_TIME( _d )		((_d)*0.01f)
#define IMG_DELAY_TO_FRAME( _d )	((FPS*(_d)+99)/100)

#define IMG_LOAD_INFO_DUMP

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
//---------------------------
// コンストラクタ
//---------------------------
CImg::CImg( void ){
	clear();
}

//---------------------------
// デストラクタ
//---------------------------
CImg::~CImg( void ){
	clear();
}

//---------------------------
// クリア
//---------------------------
void CImg::clear( void ){
    // 確保領域の解放
    SAFE_DELETE_ARRAY( m_pArrPal );
    SAFE_DELETE_ARRAY( m_pArrDot );
    SAFE_DELETE_ARRAY( m_arrDotDelay );
    SAFE_DELETE_ARRAY( m_oArrTex );

    // クリア
	m_nNumPal = 0;
	m_nNumDot = 0;
	m_nNumTex = 0;

	m_bufFileName[0] = '\0';
	m_nVramSize = 0;
}

//-------------------------------------------
// パレット取得
//-------------------------------------------
CTex* CImg::getPal( int at ){
	// パレットが無い場合は無視
	if( m_nNumPal <= 0 ){
		return( NULL );
	}

	if( at >= 0 && at < m_nNumPal ){
		return( m_pArrPal[at] );
	}

	// ここに来たら困る
	LOGE( "CImg:getPal: ERROR at=%d(%s)%d\n", at, m_bufFileName, m_nNumPal );
	return( NULL );
}

//-------------------------------------------
// 画素取得
//-------------------------------------------
CTex* CImg::getDot( int at ){
	if( at >= 0 && at < m_nNumDot ){
		return( m_pArrDot[at] );
	}

	// ここに来たら困る
	LOGE( "CImg:getDot: ERROR at=%d(%s)/%d\n", at, m_bufFileName, m_nNumDot );
	return( NULL );
}

//-------------------------------------------
// 画素の表示時間取得
//-------------------------------------------
float CImg::getDotDelay( int at ){
	if( at >= 0 && at < m_nNumDot ){
		return( IMG_DELAY_TO_TIME( m_arrDotDelay[at] ) );
	}

	// ここに来たら困る
	LOGE( "CImg:getDotDelay: ERROR at=%d: %s\n", at, m_bufFileName );
	return( 0.0f );
}

//-------------------------------------------
// トータルの表示時間の取得
//-------------------------------------------
float CImg::getDotDelayTotal( void ){
	float total = 0.0f;
	for( int i=0; i<m_nNumDot; i++ ){
		total += IMG_DELAY_TO_TIME( m_arrDotDelay[i] );
	}

	return( total );
}

//-------------------------------------------
// 指定時間から画素番号を取得する
//-------------------------------------------
int CImg::getDotAtByDelay( float time, bool isLoop ){
	// 終端に到達している場合
	float total = getDotDelayTotal();
	if( total > 0.0f && time >= total ){
		// ループ指定がなければ最終フレームを返す
		if( !isLoop ){ return( m_nNumDot -1 ); }

		// ループの場合は端数にする
		int loop = (int)(time/total);
		time -= loop*total;
	}

	float temp;
	for( int i=0; i<m_nNumDot; i++ ){
		temp = IMG_DELAY_TO_TIME( m_arrDotDelay[i] );
		if( time < temp ){
			return( i );
		}
		time -= temp;
	}

	// ここまで来たら最終フレームを返しておく
	return( m_nNumDot - 1 );
}

//-------------------------------------------
// 画素の表示フレーム数の取得
//-------------------------------------------
int CImg::getDotDelayAsFrame( int at ){
	if( at >= 0 && at < m_nNumDot ){
		int frame = IMG_DELAY_TO_FRAME( m_arrDotDelay[at] );
		return( frame );
	}

	// ここに来たら困る
	LOGE( "CImg:getDotDelayAsFrame: ERROR at=%d: %s\n", at, m_bufFileName );
	return( 0 );
}

//-------------------------------------------
// 総表示フレーム数の取得
//-------------------------------------------
int CImg::getDotDelayTotalAsFrame( void ){
	int total = 0;
	for( int i=0; i<m_nNumDot; i++ ){
		total += IMG_DELAY_TO_FRAME( m_arrDotDelay[i] );
	}

	return( total );
}

//-------------------------------------------
// 指定フレームから画像番号を取得する
//-------------------------------------------
int CImg::getDotAtByDelayAsFrame( int frame, bool isLoop ){
	// 終端に到達している場合
	int total = getDotDelayTotalAsFrame();
	if( total > 0 && frame >= total ){
		// ループ指定がなければ最終フレームを返す
		if( !isLoop ){ return( m_nNumDot -1 ); }

		// ループの場合は端数にする
		frame %= total;
	}

	int temp;
	for( int i=0; i<m_nNumDot; i++ ){
		temp = IMG_DELAY_TO_FRAME( m_arrDotDelay[i] );
		if( frame < temp ){
			return( i );
		}
		frame -= temp;
	}

	// ここまで来たら最終フレームを返しておく
	return( m_nNumDot - 1 );
}

//---------------------------
// バイナリの適用
//---------------------------
bool CImg::applyBinWithFpack( CImgBinData* pBin, stFPACK* pPack ){
    // 用心
    if( pBin == NULL ){
        return( false );
    }
    
    return( setFromFpack( pPack, pBin->getFileName(), pBin->getOption() ) );
}

//---------------------------
// ファイルから設定
//---------------------------
bool CImg::setFromFile( const char* pFileName, DWORD opt ){
	// 無効は無視
	if( util_str_len(pFileName) <= 0 ){
		return( false );
	}

	// データ読み込み
	void* pBufIn = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_IN );
	uint32 sizeIn = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_IN );
	void* pBufOut = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_OUT );
	uint32 sizeOut = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SDARC_OUT );
	uint32 readed;

	if( ! CFileMgr::LoadFileSdarc( pFileName, pBufIn, sizeIn, pBufOut, sizeOut, &readed ) ){
		LOGE( "@ CImg::setFromFile: [%s] LOAD FAILED\n", pFileName );
		return( false );
	}

	// 名前を控えておく（※バッファに収まらない場合は切り捨て）
	util_str_cpy( m_bufFileName, pFileName, IMG_FILE_NAME_LEN );

	return( setFromBuf( (BYTE*)pBufOut, readed, opt ) );
}

//---------------------------------------
// ファイルパックから設定（※名前で指定）
//---------------------------------------
bool CImg::setFromFpack( stFPACK* pPack, const char* pFileName, DWORD opt ){
	// 名前が無効なら無視
	if( util_str_len(pFileName) <= 0 ){
		LOGE( "@ CImg::setFromFpack: INVALID FILE NAME\n" );
		return( false );
	}

	// 対象の検索
	int target = fpack_search_file( pPack, pFileName, true );
	if( target < 0 ){
		LOGE( "@ CImg::setFromFpack: [%s] NOT FOUND\n", pFileName );
		return( false );
	}

#ifdef IMG_LOAD_INFO_DUMP
	LOGD( "@ LOAD IMG file=%s, opt=0x%08X\n", pFileName, opt );
#endif

	if( setFromFpack( pPack, target, opt ) ){
		// 名前を控えておく（※バッファに収まらない場合は切り捨て）
		util_str_cpy( m_bufFileName, pFileName, IMG_FILE_NAME_LEN );

		return( true );
	}

	return( false );
}

//-------------------------------------
// ファイルパックから設定（番号で指定）
//-------------------------------------
bool CImg::setFromFpack( stFPACK* pPack, int target, DWORD opt ){
	if( target < 0 || target >= fpack_get_num(pPack) ){
		LOGE( "@ CImg::setFromFpack: INVALID TARGET: target=%d\n", target );
		return( false );
	}

	// データ取得
	uint32 size;
	BYTE* pBuf = (BYTE*) fpack_get_data( pPack, target, &size, true );
	if( pBuf == NULL ){
		LOGE( "@ CImg::setFromFpack: LOAD FAILED: target=%d\n", target );
		return( false );
	}

	return( setFromBuf( pBuf, size, opt ) );
}

//---------------------------
// バッファから設定
//---------------------------
bool CImg::setFromBuf( BYTE* pBuf, uint32 sizeBuf, DWORD opt ){
	stHIF_HEADER header;
	stHIF_BLOCK_INFO info;

	// [HIF]ファイルとしての有効性確認
	if( ! CImgFormat::CheckHifHeaderThenSet( &header, pBuf ) ){
		LOGE( "CImg::setFromBuf: INVALID HIF(%s)\n", m_bufFileName );
		return( false );
	}

	//----------------------------------------
	// テクスチャ読み込み
	//----------------------------------------
	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	m_oArrTex = new CTex[header.numBlock];
	CMemMgr::PopTargetField();

	if( m_oArrTex == NULL ){
		LOGE( "CImg::setFromBuf: ALLOC ERROR (%s)\n", m_bufFileName );
		return( false );
	}

	m_nNumTex = header.numBlock;
	m_nNumPal = 0;
	m_nNumDot = 0;
	for( int i=0; i<m_nNumTex; i++ ){
		if( CImgFormat::GetHifBlockInfoFromHifHeaderAt( &info, &header, i ) ){
			// テクスチャ設定
			if( setTexWithHifBlockInfo( &m_oArrTex[i], &info, opt ) ){
#ifdef IMG_LOAD_INFO_DUMP
				LOGD( "@ LOAD IMG BLOCK[%d]: type=%02X, w=%d, h=%d, size=%u\n", i, info.type, info.width, info.height, info.size );
#endif
				if( info.type == eHIF_IMG_TYPE_P8P ){
					m_nNumPal++;
				}else{
					m_nNumDot++;
				}
			}else{
				LOGE( "CImg::setFromBuf: TEX ERROR: i=%d(%s)\n", i, m_bufFileName );
				return( false );
			}
		}else{
			LOGE( "CImg::setFromBuf: BLOCK ERROR: i=%d(%s)\n", i, m_bufFileName );
			return( false );
		}
	}

	//----------------------------------------
	// 画素とパレットを管理配列に割り振り
	//----------------------------------------
	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_DRAW );
	if( m_nNumPal > 0 ){
		m_pArrPal = new CTex*[m_nNumPal];
	}
	if( m_nNumDot > 0 ){
		m_pArrDot = new CTex*[m_nNumDot];
		m_arrDotDelay = new int[m_nNumDot];
	}
	CMemMgr::PopTargetField();

	if( (m_nNumPal > 0 && m_pArrPal == NULL) || (m_nNumDot > 0 && m_pArrDot == NULL) ){
		LOGE( "CImg::setFromBuf: ALLOC ERROR (%s)\n", m_bufFileName );
		return( false );
	}

	// 割り振り
	int palAt = 0;
	int dotAt = 0;
	for( int i=0; i<m_nNumTex; i++ ){
		CImgFormat::GetHifBlockInfoFromHifHeaderAt( &info, &header, i );

		if( info.type == eHIF_IMG_TYPE_P8P ){
			m_pArrPal[palAt++] = &m_oArrTex[i];
		}else{
			m_pArrDot[dotAt] = &m_oArrTex[i];
			m_arrDotDelay[dotAt] = info.delay;
            dotAt++;
		}
	}

	return( true );
}

//-------------------------------------------
// ブロックデータからテクスチャを設定
//-------------------------------------------
bool CImg::setTexWithHifBlockInfo( CTex* pTex, stHIF_BLOCK_INFO* pInfo, DWORD opt ){
	// work
	BYTE bufTemp[256*4];
	eTEX_DATA_TYPE type = eTEX_DATA_TYPE_INVALID;
	BYTE* pBuf = pInfo->pBuf;
	uint32 size = pInfo->size;
	int w = pInfo->width;
	int h = pInfo->height;
    
	switch( pInfo->type ){
	//----------------------------------------------
	// フルカラー：１画素４バイト
	//----------------------------------------------
	case eHIF_IMG_TYPE_RGBA:
		type = eTEX_DATA_TYPE_RGBA;
		break;

	//----------------------------------------------
	// フルカラー（α無し）：１画素３バイト
	//----------------------------------------------
	case eHIF_IMG_TYPE_RGB:
		type = eTEX_DATA_TYPE_RGB;
		break;

	//----------------------------------------------
	// グレースケール：１画素１バイト
	//----------------------------------------------
	case eHIF_IMG_TYPE_GRAY:
		type = eTEX_DATA_TYPE_GRAY;
		break;

	//----------------------------------------------
	// ２５６色パレット：パレット部：１画素４バイト
	//----------------------------------------------
	case eHIF_IMG_TYPE_P8P:
		type = eTEX_DATA_TYPE_P8P;

		// [HIF]のパレットは使用色しか格納されないので256色分の配列へ設定
		memcpy( bufTemp, pInfo->pBuf, pInfo->size );
		if( pInfo->size < 256*4 ){
			memset( &bufTemp[pInfo->size], 0, 256*4-pInfo->size );
		}
		pBuf = bufTemp;
		size = 256*4;
		w = 256;
		h = 1;
		break;

	//----------------------------------------------
	// ２５６色パレット：ドット部：１画素１バイト
	//----------------------------------------------
	case eHIF_IMG_TYPE_P8D:
		type = eTEX_DATA_TYPE_P8D;
            
        // リニアの指定があればグレースケール扱いとしておく
        if( (opt&TEX_CREATE_OPT_LINEAR) != 0 ){
            type = eTEX_DATA_TYPE_GRAY;
        }
		break;

	default:
		break;
	}

	// この時点でタイプが無効であれば返す
	if( type == eTEX_DATA_TYPE_INVALID ){
		LOGE( "@ CImg::setTexWithHifBlockInfo: INVALID DATA: pInfo->type=0x%02X\n", pInfo->type );
		return( false );
	}

	// VRRAM上のサイズ
	if( type == eTEX_DATA_TYPE_P8P ){
		m_nVramSize += TEX_PAL_EXPAND_RATE*size;	// パレットサイズ調整される
	}else{
		m_nVramSize += size;
	}

	// テクスチャの作成
	if( pTex->create( pBuf, type, w, h, opt ) ){
		pTex->setOfsL( pInfo->ofsLeft );
		pTex->setOfsT( pInfo->ofsTop );
		pTex->setOfsR( pInfo->ofsRight );
		pTex->setOfsB( pInfo->ofsBottom );
		pTex->setBaseX( pInfo->baseX );
		pTex->setBaseY( pInfo->baseY );
		return( true );
	}

	LOGE( "@ CImg::setTexWithHifBlockInfo: CREATE FAILED: type=%d, w=%d, h=%d, opt=%08X\n", type, w, h, opt );
	return( false );
}
