/*+----------------------------------------------------------------+
  |	Title:		TexString.cpp [GLES2.0環境]
  |	Comment:	テクスチャ文字列
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TexString.hpp"

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
//---------------------------
// コンストラクタ
//---------------------------
CTexString::CTexString( void ): CTexCacheBase(){}

//---------------------------
// キャッシュ判定
//---------------------------
bool CTexString::isCacheHit( const char* pStr, int type, int size ){
	if( isValid() ){
		if( m_nFontType != type ){ return( false ); }
		if( m_nFontSize != size ){ return( false ); }

		// 用心にバッファサイズ指定で比較（※コピー時に切り捨てられている可能性がある）
		return( util_str_cmp( m_cArrString, pStr, TEX_STRING_LEN ) == 0 );
	}

	return( false );
}

//---------------------------
// 設定
//---------------------------
CTex* CTexString::cacheTex( const char* pStr, int fontType, int fontSize ){
	// 設定のコピー（※文字列がバッファに収まらない場合は切り捨て：比較時にはサイズ制限をしているので問題はないとしておく）
	util_str_cpy( m_cArrString, pStr, TEX_STRING_LEN );
	m_nFontType = fontType;
	m_nFontSize = fontSize;

	// 生データの取得（※システムフォントをＢＭＰデータで受け取る）
	stINFO_FOR_STRING_TEXTURE info;
	BYTE* pBuf = CAppMgr::CreateStringTexture( m_cArrString, m_nFontType, m_nFontSize, &info );

	// テクスチャに設定
	m_oTex.create( pBuf, eTEX_DATA_TYPE_GRAY, info.texWidth, info.texHeight );
	m_oTex.setFocusX( info.left );
	m_oTex.setFocusY( info.top );
	resetUnusedCount();

	return( &m_oTex );
}
