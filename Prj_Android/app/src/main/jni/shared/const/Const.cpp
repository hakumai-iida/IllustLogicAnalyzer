/*+----------------------------------------------------------------+
  |	Title:		Const.cpp [共通環境]
  |	Comment:	定型処理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Const.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 一時ファイル接頭子
#define TEMPORARY_FILE_HEAD		"~"

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
const char* g_pArrLabelAsyncType[] = {
	"LINEAR",
	"UP",
	"DOWN",
	"UP_DOWN",
	"DOWN_UP",

	NULL,
};

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------
// 同期率算出
//-------------------
float CConst::ApplyAsyncType( eASYNC_TYPE type, float rateSrc, float pow ){
	// 用心
	if( rateSrc < 0.0f ){ rateSrc = 0.0f; }
	else if( rateSrc > 1.0f ){ rateSrc = 1.0f; }

	float rate = rateSrc;

	// 適用
	switch( type ){
	// 拡散
	case eASYNC_TYPE_UP:
		rate =  rate * rate * rate;
		break;

	// 収束
	case eASYNC_TYPE_DOWN:
		rate = 1.0f - (1.0f-rate) * (1.0f-rate) * (1.0f-rate);
		break;

	// 拡散→収束
	case eASYNC_TYPE_UP_DOWN:
		if( rate > 0.5f ){
			rate = 2.0f*(rate-0.5f);
			rate = 1.0f - (1.0f-rate) * (1.0f-rate) * (1.0f-rate);
			rate = 0.5f + rate/2.0f;
		}else{
			rate *= 2.0f;
			rate = rate * rate * rate;
			rate /= 2.0f;
		}
		break;

	// 収束→拡散
	case eASYNC_TYPE_DOWN_UP:
		if( rate > 0.5f ){
			rate = 2.0f*(rate-0.5f);
			rate = rate * rate * rate;
			rate = 0.5f + rate/2.0f;
		}else{
			rate *= 2.0f;
			rate = 1.0f - (1.0f-rate) * (1.0f-rate) * (1.0f-rate);
			rate /= 2.0f;
		}
		break;

	// その他（※線形）
	default:
		// そのまま返す
		return( rate );
	}

	// 強さの適用
	if( pow < 0.0f ){ pow = 0.0f; }
	else if( pow > 1.0f ){ pow = 1.0f; }
	rate = rateSrc*(1.0f-pow) + rate*pow;

	return( rate );
}

//-------------------
// 同期：Int
//-------------------
int CConst::CalcAsyncI( float rate, int vFrom, int vTo ){
	if( rate <= 0 ){ return( vFrom ); }
	if( rate >= 1.0f ){ return( vTo ); }
	return( (int)((vFrom*(1.0f-rate) + vTo*rate)) );
}

//-------------------
// 同期：Float
//-------------------
float CConst::CalcAsyncF( float rate, float fFrom, float fTo ){
	if( rate <= 0 ){ return( fFrom ); }
	if( rate >= 1.0f ){ return( fTo ); }
	return( fFrom*(1.0f-rate) + fTo*rate );
}

//-------------------
// 同期：BYTE
//-------------------
BYTE CConst::CalcAsyncB( float rate, BYTE aFrom, BYTE aTo ){
    if( rate <= 0 ){ return( aFrom ); }
    if( rate >= 1.0f ){ return( aTo ); }
    return( (BYTE)(aFrom*(1.0f-rate) + aTo*rate) );
}

//---------------------------------
// 同期：RGBA
//---------------------------------
uint32 CConst::CalcAsyncRGBA( float rate, uint32 rgbaFrom, uint32 rgbaDest ){
	if( rate <= 0.0f ){ return( rgbaFrom ); }
	else if( rate >= 1.0f ){ return( rgbaDest ); }

	uint32 rF = (rgbaFrom>>24) & 0xFF;
	uint32 gF = (rgbaFrom>>16) & 0xFF;
	uint32 bF = (rgbaFrom>> 8) & 0xFF;
	uint32 aF = (rgbaFrom>> 0) & 0xFF;

	uint32 rD = (rgbaDest>>24) & 0xFF;
	uint32 gD = (rgbaDest>>16) & 0xFF;
	uint32 bD = (rgbaDest>> 8) & 0xFF;
	uint32 aD = (rgbaDest>> 0) & 0xFF;

	uint32 r = (uint32)(rF*(1.0f-rate) + rD*rate);
	uint32 g = (uint32)(gF*(1.0f-rate) + gD*rate);
	uint32 b = (uint32)(bF*(1.0f-rate) + bD*rate);
	uint32 a = (uint32)(aF*(1.0f-rate) + aD*rate);

	return( (r<<24) | (g<<16) | (b<<8) | (a<<0) );
}

//------------------------
// 平均：RGBA
//------------------------
uint32 CConst::CalcAvgForRGBA( uint32 rgba0, uint32 rgba1 ){
	uint32 r0 = (rgba0>>24) & 0xFF;
	uint32 g0 = (rgba0>>16) & 0xFF;
	uint32 b0 = (rgba0>> 8) & 0xFF;
	uint32 a0 = (rgba0>> 0) & 0xFF;

	uint32 r1 = (rgba1>>24) & 0xFF;
	uint32 g1 = (rgba1>>16) & 0xFF;
	uint32 b1 = (rgba1>> 8) & 0xFF;
	uint32 a1 = (rgba1>> 0) & 0xFF;

	uint32 r = (r0 + r1)/2;
	uint32 g = (g0 + g1)/2;
	uint32 b = (b0 + b1)/2;
	uint32 a = (a0 + a1)/2;

	return( (r<<24) | (g<<16) | (b<<8) | (a<<0) );
}

//------------------------
// 乗算：RGBA
//------------------------
uint32 CConst::CalcMultiForRGBA( uint32 rgba, uint32 rgbaM ){
	// 計算が不要であれば
	if( rgba == 0x00000000 || rgbaM == 0x00000000 ){ return( 0x00000000 ); }
	if( rgba == 0xFFFFFFFF ){ return( rgbaM ); }
	if( rgbaM == 0xFFFFFFFF ){ return( rgba ); }

	uint32 r = (rgba>>24) & 0xFF;
	uint32 g = (rgba>>16) & 0xFF;
	uint32 b = (rgba>> 8) & 0xFF;
	uint32 a = (rgba>> 0) & 0xFF;

	uint32 rM = (rgbaM>>24) & 0xFF;
	uint32 gM = (rgbaM>>16) & 0xFF;
	uint32 bM = (rgbaM>> 8) & 0xFF;
	uint32 aM = (rgbaM>> 0) & 0xFF;

	r = r*rM/0xFF;
	g = g*gM/0xFF;
	b = b*bM/0xFF;
	a = a*aM/0xFF;

	return( (r<<24) | (g<<16) | (b<<8) | (a<<0) );
}

//---------------------------------
// RGBAの計算：カウンタによるアニメ
//---------------------------------
uint32 CConst::CalcMoveForRGBA( uint32 rgbaFrom, uint32 rgbaDest, int count, int countMax ){
    if( count <= 0 ){ return( rgbaFrom ); }
    else if( count >= countMax ){ return( rgbaDest ); }
    
    uint32 rF = (rgbaFrom>>24) & 0xFF;
    uint32 gF = (rgbaFrom>>16) & 0xFF;
    uint32 bF = (rgbaFrom>> 8) & 0xFF;
    uint32 aF = (rgbaFrom>> 0) & 0xFF;
    
    uint32 rD = (rgbaDest>>24) & 0xFF;
    uint32 gD = (rgbaDest>>16) & 0xFF;
    uint32 bD = (rgbaDest>> 8) & 0xFF;
    uint32 aD = (rgbaDest>> 0) & 0xFF;
    
    uint32 r = (rF*(countMax-count) + rD*count)/countMax;
    uint32 g = (gF*(countMax-count) + gD*count)/countMax;
    uint32 b = (bF*(countMax-count) + bD*count)/countMax;
    uint32 a = (aF*(countMax-count) + aD*count)/countMax;
    
    return( (r<<24) | (g<<16) | (b<<8) | (a<<0) );
}

//--------------------------------
// 最大までインクリメント
//--------------------------------
bool CConst::IncrementToMax( float *pVal, float max, float delta ){
	if( *pVal < max ){
		*pVal += delta;
		if( *pVal >= max ){
			*pVal = max;
		}
	}

	// 最大であれば通知
	return( *pVal >= max );
}

//--------------------------------
// ０までデクリメント
//--------------------------------
bool CConst::DecrementToZero( float *pVal, float delta ){
	if( *pVal > 0.0f ){
		*pVal -= delta;
		if( *pVal <= 0.0f ){
			*pVal = 0.0f;
		}
	}

	// ０であれば通知
	return( *pVal <= 0.0f );
}

//--------------------------------
// フラグ：判定
//--------------------------------
bool CConst::CheckFlag( int flag, int bit ){
	int mask = 0x01 << bit;
	return( (mask&flag) != 0 );
}

//--------------------------------
// フラグ：オン
//--------------------------------
void CConst::SetFlagOn( int* pFlag, int bit ){
	int mask = 0x01 << bit;
	*pFlag = mask | (*pFlag);
}

//--------------------------------
// フラグ：オフ
//--------------------------------
void CConst::SetFlagOff( int* pFlag, int bit ){
	int mask = ~(0x01 << bit);
	*pFlag = mask & (*pFlag);
}

//--------------------------------
// フラグ：スワップ
//--------------------------------
void CConst::SwapFlag( int *pFlag, int bit ){
	if( CheckFlag( *pFlag, bit ) ){ SetFlagOff( pFlag, bit ); }
	else{ SetFlagOn( pFlag, bit ); }
}

//-----------------------------------------------------------------
// 文字列置換（※パラメータ付き）
// [pStrSrc]の[0],[1],[2]...の文字を、[pArrParam]の要素で置き換える
//-----------------------------------------------------------------
bool CConst::ReplaceString( char* buf, const char* pStrSrc, const char** pArrParam ){
	// 無効は無視
	if( buf == NULL || pStrSrc == NULL ){
		return( false );
	}

	// パラメータ個数の確認
	int numParam = 0;
	if( pArrParam != NULL ){
		while( pArrParam[numParam] != NULL ){
			numParam++;
		}
	}

	// 文字列の出力
	int at = 0;
	int out = 0;
	while( pStrSrc[at] != '\0' ){
		// 置き換え指定の開始文字があれば
		if( pStrSrc[at] == '[' ){
			// 添え字が有効なら（※とりあえず１桁まで）
			if( pStrSrc[at+1] >= '0' && pStrSrc[at+1] <= '9' ){
				int index = pStrSrc[at+1] - '0';

				// 終了文字があれば
				if( pStrSrc[at+2] == ']' ){
					// パラメータ枠に収まっていれば
					if( index < numParam ){
						const char* pParam = pArrParam[index];

						while( *pParam != '\0' ){
							buf[out++] = *pParam++;
						}

						// 指定をスキップ
						at += 3;
						continue;
					}
				}
			}
		}

		// ここまできたら1文字出力
		buf[out++] = pStrSrc[at++];
	}

	// 終端をつけて終了
	buf[out] = '\0';
	return( true );
}

// パラメータ×１
bool CConst::ReplaceString1( char* buf, const char* pStrSrc, const char* pParam0 ){
	const char* arrParam[2];
	arrParam[0] = pParam0;
	arrParam[1] = NULL;
	return( ReplaceString( buf, pStrSrc, arrParam ) );
}

// パラメータ×２
bool CConst::ReplaceString2( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1 ){
	const char* arrParam[3];
	arrParam[0] = pParam0;
	arrParam[1] = pParam1;
	arrParam[2] = NULL;
	return( ReplaceString( buf, pStrSrc, arrParam ) );

}

// パラメータ×３
bool CConst::ReplaceString3( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1, const char* pParam2 ){
	const char* arrParam[4];
	arrParam[0] = pParam0;
	arrParam[1] = pParam1;
	arrParam[2] = pParam2;
	arrParam[3] = NULL;
	return( ReplaceString( buf, pStrSrc, arrParam ) );

}

// パラメータ×４
bool CConst::ReplaceString4( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1, const char* pParam2, const char* pParam3 ){
	const char* arrParam[5];
	arrParam[0] = pParam0;
	arrParam[1] = pParam1;
	arrParam[2] = pParam2;
	arrParam[3] = pParam3;
	arrParam[4] = NULL;
	return( ReplaceString( buf, pStrSrc, arrParam ) );
}

// パラメータ×５
bool CConst::ReplaceString5( char* buf, const char* pStrSrc, const char* pParam0, const char* pParam1, const char* pParam2, const char* pParam3, const char* pParam4 ){
	const char* arrParam[6];
	arrParam[0] = pParam0;
	arrParam[1] = pParam1;
	arrParam[2] = pParam2;
	arrParam[3] = pParam3;
	arrParam[4] = pParam4;
	arrParam[5] = NULL;
	return( ReplaceString( buf, pStrSrc, arrParam ) );
}

//-----------------------------------------------------------------
// 一時ファイル名設定
//-----------------------------------------------------------------
bool CConst::SetFileName( char* buf, const char* fileName, const char* defaultFileName, bool isTemporary ){
	// 無効は無視
	if( buf == NULL ){
		LOGE( "CConst::SetFileName: INVALID buf\n" );
		return( false );
	}

	// ファイル名が無効ならデフォルト名を利用
	if( fileName == NULL || fileName[0] == '\0' ){
		if( defaultFileName == NULL || defaultFileName[0] == '\0'){
			LOGE( "CConst::SetFileName: INVALID defaultName\n" );
			return( false );
		}

		fileName = defaultFileName;
	}

	// テンポラリであれば接頭子をつける
	if( isTemporary ){
		sprintf( buf, "%s%s", TEMPORARY_FILE_HEAD, fileName );
	}else{
		sprintf( buf, "%s", fileName );
	}

	return( true );
}
