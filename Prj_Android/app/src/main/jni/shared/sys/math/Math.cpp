/*+----------------------------------------------------------------+
  |	Title:		Math.cpp [共通環境]
  |	Comment:	算数関連
  |             角度の処理は[度＝360で一周]＆[１２時を０度]として行なっている
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Math.hpp"

#include <math.h>

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 円周率
#define PI	3.14159265359f

#define DEG_TO_RAD( _deg )	(PI*(_deg)/180.0f)
#define RAD_TO_DEG( _rad )	(180.0f*(_rad)/PI)

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 値は[0.5f]刻み（※要素数７２０）
static float __val_sin[] = {
#include "inc/val_sin.inc"
};

// 値は[0.5f]刻み（※要素数７２０）
static float __val_cos[] = {
#include "inc/val_cos.inc"
};

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------------
// サイン取得（※実処理）
//------------------------------
float CMath::GetSin( float deg ){
	float rad = DEG_TO_RAD( deg );
	return( sinf( rad ) );
}

//------------------------------
// コサイン取得（※実処理）
//------------------------------
float CMath::GetCos( float deg ){
	float rad = DEG_TO_RAD( deg );
	return( cos( rad ) );
}

//-------------------------------------------------------------------
// サイン取得（※低精度：[0.5f]刻み）
// 引数は[0.0f]以上[360.0f]未満を想定（※呼び出し時は値の有効性に気をつけること）
//-------------------------------------------------------------------
float CMath::GetSinLite( float deg ){
    return( __val_sin[(int)(2*deg)] );
}

//-------------------------------------------------------------------
// サイン取得（※低精度：[0.5f]刻み）
// 引数は[0.0f]以上[360.0f]未満を想定（※呼び出し時は値の有効性に気をつけること）
//-------------------------------------------------------------------
float CMath::GetCosLite( float deg ){
    return( __val_cos[(int)(2*deg)] );
}

//------------------------------
// 回転：Ｘ座標
//------------------------------
float CMath::RotX( float x0, float y0, float deg ){
	float sin = GetSin( deg );
	float cos = GetCos( deg );
	return( x0*cos - y0*sin );
}

//------------------------------
// 回転：Ｙ座標
//------------------------------
float CMath::RotY( float x0, float y0, float deg ){
	float sin = GetSin( deg );
	float cos = GetCos( deg );
	return( x0*sin + y0*cos );
}

//------------------------------
// 角度取得
//------------------------------
float CMath::GetDeg( float x, float y ){
	// 横に傾いていない場合
	if( x == 0.0f ){
		if( y > 0.0f ){
			return( 180.0f );
		}
		return( 0.0f );
	}

	// 縦に傾いていない場合
	if( y == 0.0f  ){
		if( x < 0.0f ){
			return( 270.0f );
		}
		return( 90.0f );
	}

	// ここまで来たら計算
	float rad = atanf( y/x );
	float deg = RAD_TO_DEG( rad );

	// 上向きが０度になるように調整
	if( x < 0 ){ deg -= 90; }
	else{ deg += 90; }

	while( deg < 0.0f ){ deg += 360; }
	while( deg > 360.0f ){ deg -= 360; }

	return( deg );
}

//------------------------------
// 二点間の角度取得
//------------------------------
float CMath::GetDeg( float x0, float y0, float x1, float y1 ){
	float x = x0 - x1;
	float y = y0 - y1;
	return( GetDeg( x, y ) );
}

//------------------------------
// 角度の反転：ヨコ
//------------------------------
float CMath::FlipRotH( float rot ){
    rot = 360 - rot;
    
    while( rot >= 360.0f ){
        rot -= 360.0f;
    }

    while( rot < 0.0f ){
        rot += 360.0f;
    }

    return( rot );
}

//------------------------------
// 角度の反転：タテ
//------------------------------
float CMath::FlipRotV( float rot ){
    rot = 180 - rot;

    while( rot >= 360.0f ){
        rot -= 360.0f;
    }
    
    while( rot < 0.0f ){
        rot += 360.0f;
    }

    return( rot );
}

//------------------------------
// 二点間の距離
//------------------------------
float CMath::Dist( float x0, float y0, float x1, float y1 ){
	float x = x0 - x1;
	float y = y0 - y1;
	return( Len( x, y ) );
}

//------------------------------
// 長さ
//------------------------------
float CMath::Len( float x, float y ){
	return( (float)sqrt( x*x + y*y ) );
}

//------------------------------
// 切り上げ
//------------------------------
int CMath::RoundUp( float vF ){
	int vI = (int)vF;

	if( vI < vF ){
		vI++;
	}

	return( vI );
}

//--------------------------------
// 文字列を符号付整数に(atoi)
//--------------------------------
bool CMath::StrToLong( const char* pStr, int64* pVal ){
	// 用心
	if( pStr == NULL || pVal == NULL){
        return( false );
    }

	int64 val = 0;
	int sign = 1;

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }

	// マイナス符号の確認
	if( *pStr == '-' ){
		sign = -1;
		pStr++;
	}
	// プラス符号の確認（※一応）
	else if( *pStr == '+' ){
		sign = 1;
		pStr++;
	}

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }

    // この時点で終端文字であればおかしい
    if( *pStr == '\0' ){
        return( false );
    }

	// 整数部分の読み込み
	while( *pStr != '\0' ){
		if( *pStr >= '0' && *pStr <= '9' ){
			val *= 10;
			val += *pStr - '0';
		}else{
            // 数字以外が来たら抜ける
            break;
		}

		pStr++;
 	}
    
    // 少数の指定であれば無視しておく
    if( *pStr == '.' ){
        pStr++;
        
        // この時点で終端文字であればおかしい
        if( *pStr == '\0' ){
            return( false );
        }

        while( *pStr != '\0' ){
            if( *pStr >= '0' && *pStr <= '9' ){
            }else{
                // 数字以外が来たら抜ける
                break;
            }

            pStr++;
        }
    }
    
    // 基数の指定であれば無視しておく
    if( *pStr == 'E' || *pStr == 'e' ){
        pStr++;
        
        // マイナス符号の確認
        if( *pStr == '-' ){
            pStr++;
        }
        // プラス符号の確認（※一応）
        else if( *pStr == '+' ){
            pStr++;
        }

        // この時点で終端文字であればおかしい
        if( *pStr == '\0' ){
            return( false );
        }

        while( *pStr != '\0' ){
            if( *pStr >= '0' && *pStr <= '9' ){
            }else{
                // 数字以外が来たら抜ける
                break;
            }

            pStr++;
        }
    }

    // フロート指定は無視
    if( *pStr == 'f' || *pStr == 'F' ){
        pStr++;
    }

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }

    // この時点で終端文字になっていなければおかしい
    if( *pStr != '\0' ){
        return( false );
    }

    // ここまで来たら成功
    *pVal = sign * val;
	return( true );
}

//--------------------------------
// 文字列を符号無し整数に(atoi)
//--------------------------------
bool CMath::StrToUnsignedLong( const char* pStr, uint64* pVal ){
    // 用心
    if( pStr == NULL || pVal == NULL ){
        return( false );
    }

    uint64 val = 0;

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }

    // プラス符号の確認（※一応）
    if( *pStr == '+' ){
        pStr++;
    }

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }

    // この時点で終端文字であればおかしい
    if( *pStr == '\0' ){
        return( false );
    }

    // 整数部分の読み込み
    while( *pStr != '\0' ){
        if( *pStr >= '0' && *pStr <= '9' ){
            val *= 10;
            val += *pStr - '0';
        }else{
            // 数字以外が来たら抜ける
            break;
        }

        pStr++;
     }
    
    // 少数の指定であれば無視しておく
    if( *pStr == '.' ){
        pStr++;
        
        // この時点で終端文字であればおかしい
        if( *pStr == '\0' ){
            return( false );
        }

        while( *pStr != '\0' ){
            if( *pStr >= '0' && *pStr <= '9' ){
            }else{
                // 数字以外が来たら抜ける
                break;
            }

            pStr++;
        }
    }
    
    // 基数の指定であれば無視しておく
    if( *pStr == 'E' || *pStr == 'e' ){
        pStr++;
        
        // マイナス符号の確認
        if( *pStr == '-' ){
            pStr++;
        }
        // プラス符号の確認（※一応）
        else if( *pStr == '+' ){
            pStr++;
        }

        // この時点で終端文字であればおかしい
        if( *pStr == '\0' ){
            return( false );
        }

        while( *pStr != '\0' ){
            if( *pStr >= '0' && *pStr <= '9' ){
            }else{
                // 数字以外が来たら抜ける
                break;
            }

            pStr++;
        }
    }

    // フロート指定は無視
    if( *pStr == 'f' || *pStr == 'F' ){
        pStr++;
    }

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }

    // この時点で終端文字になっていなければおかしい
    if( *pStr != '\0' ){
        return( false );
    }

    // ここまで来たら成功
    *pVal = val;
    return( true );
}

//--------------------------------
// 文字列を浮動小数に(atof)
//--------------------------------
bool CMath::StrToDouble( const char* pStr, double* pVal ){
	// 用心
	if( pStr == NULL || pVal == NULL){
        return( false );
    }

	double val  = 0.0;
	double base = 1.0;
	double sign = 1.0;

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }
    
	// マイナス符号の確認
	if( *pStr == '-' ){
		sign = -1.0;
		pStr++;
	}
	// プラス符号の確認（※一応）
	else if( *pStr == '+' ){
		sign = 1.0;
		pStr++;
	}
    
    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }
    
    // この時点で終端文字であればおかしい
    if( *pStr == '\0' ){
        return( false );
    }

	// 左側（整数部分）の確認
	while( *pStr != '\0' ){
		if( *pStr >= '0' && *pStr <= '9' ){
			val *= 10.0;
			val += *pStr - '0';
		}else{
            // 数字以外が来たら抜ける
            break;
		}

		pStr++;
 	}

	// 小数点の確認
	if( *pStr == '.' ){
        pStr++;

        // 右側（小数部分）の読み込み
        while( *pStr != '\0' ){
            if( *pStr >= '0' && *pStr <= '9' ){
                base /= 10.0;
                val += (*pStr - '0') * base;
            }else{
                // 数字以外が来たら抜ける
                break;
            }

            pStr++;
        }
    }

	// [e/E]の確認
	if( *pStr == 'e' || *pStr == 'E' ){
		pStr++;

		double step = 10.0;
        
		if( *pStr == '-' ){
            pStr++;
			step = 0.1;
		}else if( *pStr == '+' ){
            pStr++;
			step = 10.0;
		}

        int num = 0;
        while( *pStr != '\0' ){
            if( *pStr >= '0' && *pStr <= '9' ){
                num = 10*num + (*pStr - '0');
            }else{
                // 数字以外が来たら抜ける
                break;
            }
            pStr++;
        }

        for( int i=0; i<num; i++ ){
            val *= step;
        }
    }

    // フロート指定の確認
    if( *pStr == 'f' || *pStr == 'F' ){
        pStr++;
    }

    // スペースは無視
    while( *pStr == ' ' ){
        pStr++;
    }

    // この時点で終端文字になっていなければおかしい
    if( *pStr != '\0' ){
        return( false );
    }

    // ここまで来たら成功
    *pVal = sign * val;
	return( true );
}
