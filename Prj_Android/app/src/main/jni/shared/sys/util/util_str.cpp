/*+----------------------------------------------------------------+
  |	Title:		util_str.cpp [共通環境]
  |	Comment:	文字列関連（※基本的に[NULL]は空文字[""]扱い）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "util_str.hpp"

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
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//----------------
// 文字列長の取得
//----------------
int util_str_len( const char* pStr ){
	if( pStr == NULL ){
		return( 0 );
	}

	return( (int)strlen( pStr ) );
}

//---------------------
// 文字列の比較
//---------------------
int util_str_cmp( const char* pStr0, const char* pStr1, int bufSize ){
	// 両者が[NULL]であれば等しい（※両者とも空文字）
	if( pStr0 == NULL && pStr1 == NULL ){
		return( 0 );
	}

	if( pStr0 == NULL ){
		if( pStr1[0] == '\0' ){ return( 0 );  }
		return( 1 );	// [pStr1]の方が大きい
	}

	if( pStr1 == NULL ){
		if( pStr0[0] == '\0' ){ return( 0 );  }
		return( -1 );	// [pStr0]の方が大きい
	}

	if( bufSize > 0 ){
		return( strncmp( pStr0, pStr1, bufSize ) );
	}
	return( strcmp( pStr0, pStr1 ) );
}

//------------------------------
// 文字列のコピー
//------------------------------
bool util_str_cpy( char* pBuf, const char* pSrc, int bufSize ){
	if( pBuf == NULL ){
		LOGE( "@ util_str_cpy: INVALID: pBuf\n" );
		return( false );
	}

	if( bufSize <= 0 ){
		LOGE( "@ util_str_cpy: INVALID: bufSize=%d\n", bufSize );
		return( false );
	}

	// [NULL]は空文字扱い
	if( pSrc == NULL ){
		pBuf[0] = '\0';
		return( true );	// 正常終了
	}

	// 文字数がバッファに収まらない場合
	int len = util_str_len( pSrc );
	if( len >= bufSize ){
		strncpy( pBuf, pSrc, bufSize-1 );
		pBuf[bufSize-1] = '\0';

		// 結果としては失敗終了（※後の取り扱いは呼び出し元にゆだねる）
		return( false );
	}

	strcpy( pBuf, pSrc );
	return( true );
}

//------------------------------------------------------------------------
// TSV処理した際の要素数と、その終端の確認（※想定よりも要素が少ない場合は[false]が返る）
//------------------------------------------------------------------------
bool util_str_tsv_check_element_end( const char* pSrc, int* pEnd, int supposedNum, char separator ){
    // 無効は無視
    if( pSrc == NULL ){
        LOGE( "@ util_str_tsv_check_element_end: INVALID: pSrc\n" );
        return( false );
    }
    
    if( pEnd == NULL ){
        LOGE( "@ util_str_tsv_check_element_end: INVALID: pEnd\n" );
        return( false );
    }

    if( supposedNum <= 0 ){
        LOGE( "@ util_str_tsv_check_element_end: INVALID: supposedNum=%d\n", supposedNum );
        return( false );
    }

    // work
    int at = 0;
    int check = 0;

    // 各要素の終端位置の検出
    while( pSrc[at] != '\0' ){
        if( pSrc[at] == separator ){
            pEnd[check++] = at;
            if( check >= supposedNum ){ break; }
        }
        at++;
    }
    
    // 要素数に達していない場合は文字列終端も検出
    if( check < supposedNum ){
        pEnd[check++] = at;
    }

    // 要素数が想定にみたなければ無効
    if( check < supposedNum ){
        LOGE( "@ util_str_tsv_check_element_end: not enough value check=%d/%d, pSrc=%s\n", check, supposedNum, pSrc  );
        return( false );
    }

    // ここまできたら想定するだけの要素がある
    return( true );
}

//-------------------------------------------------------------
// 有効な pEnd に対して、指定番目の要素をバッファに読み込む
//-------------------------------------------------------------
bool util_str_tsv_read_at( char* pBuf, const char* pSrc, int bufSize, int* pEnd, int elementAt, bool isOmittedIfOverflow ){
    // 無効は無視
    if( pBuf == NULL){
        LOGE( "@ util_str_tsv_read_at: INVALID: pBuf\n" );
        return( false );
    }
    
    if( pSrc == NULL ){
        LOGE( "@ util_str_tsv_read_at: INVALID: pSrc\n" );
        return( false );
    }
    
    if( pEnd == NULL ){
        LOGE( "@ util_str_tsv_read_at: INVALID: pEnd\n" );
        return( false );
    }

    if( elementAt < 0 ){
        LOGE( "@ util_str_tsv_read_at: INVALID: elementAt=%d\n", elementAt );
        return( false );
    }

    int from = 0;
    int len = 0;
    
    // コピー範囲の検出
    if( elementAt > 0 ){
        from = pEnd[elementAt-1] + 1;
        len = pEnd[elementAt] - from;
    }else{
        from = 0;
        len = pEnd[0];
    }
    
    // バッファの確認
    if( len < bufSize ){
        // バッファに余裕があればそのままコピー
        util_str_n_cpy( pBuf, &pSrc[from], len );
        pBuf[len] = '\0';
    }
    // バッファが足りない場合
    else{
        // 省略指定がなければエラー
        if( ! isOmittedIfOverflow ){
            LOGE( "@ util_str_tsv_read_at: BUF SHORTAGE: len=%d/%d\n", len, bufSize );
            return( false );
        }
        
        // 省略して表示できるほどのバッファがなければエラーとする
        if( bufSize < 4 ){
            LOGE( "@ util_str_tsv_read_at: CAN'T OMMITED: bufSize=%d\n", bufSize );
            return( false );
        }

        // 末尾を[...]で省略して設定
        util_str_n_cpy( pBuf, &pSrc[from], bufSize-(3+1) );
        pBuf[bufSize-4] = '.';
        pBuf[bufSize-3] = '.';
        pBuf[bufSize-2] = '.';
        pBuf[bufSize-1] = '\0';
    }
    
    // ここまできたら成功
    return( true );
}

//-------------------------------------------------------------
// TSVバッファに文字列を追加する（※返値は追加後の文字列長）
//-------------------------------------------------------------
int util_str_tsv_add_str( char* pBuf, const char* pStr, int ofs, int bufSize, char separator ){
    // 無効は無視
    if( pBuf == NULL ){
        LOGE( "@ util_str_tsv_add_str: INVALID: pBuf\n" );
        return( -1 );
    }
    
    if( ofs < 0 ){
        LOGE( "@ util_str_tsv_add_str: INVALID: ofs=%d\n", ofs );
        return( -1 );
    }

    // 要素が無効なら空文字扱い
    if( pStr == NULL ){
        pStr = "";
    }

    // 追加後のサイズの確認
    int len = ofs + util_str_len( pStr ) + 1;
    if( len >= bufSize ){
        LOGE( "@ util_str_tsv_add_str: BUF OVERFLOW: len=%d/%d\n", len, bufSize );
        return( -1 );
    }
    
    // 文字列の書き込み
    sprintf( &pBuf[ofs], "%s%c", pStr, separator );
    
    return( len );
}

//-------------------------------------------------------------------------------
// 指定の区切り文字を検索して '\0' に置き換える（※返値は要素数）
//-------------------------------------------------------------------------------
int util_str_csv_split( char* pBuf, char separator ){
    // 無効は無視
    if( pBuf == NULL ){
        LOGE( "@ util_str_csv_split: INVALID pBuf\n" );
        return( 0 );
    }
    
    int num = 0;

    // 区切り文字の数だけ部分文字列（文字終端）が作成されることになる
    while( *pBuf != '\0' ){
        if( *pBuf == separator ){
            *pBuf = '\0';
            num++;
        }
        
        pBuf++;
    }

    // 終端要素分＋１
    num++;
    return( num );
}

//-------------------------------------------------------------------------------
// 文字列終端の次の位置を返す（※[util_str_csv_split]後の文字列に対して逐次部分文字列を取得する）
//-------------------------------------------------------------------------------
char* util_str_csv_get_next( char* pBuf ){
    if( pBuf == NULL ){
        LOGE( "@ util_str_csv_get_next: INVALID pBuf\n" );
        return( NULL );
    }
    
    // 文字終端の次の位置まで参照を進める
    while( *pBuf++ != '\0' );
    
    return( pBuf );
}

//-------------------------------------------------------------------------------
// 文字列を出力して区切り文字を挿入した位置を返す（※文字列のCSV出力用）
//-------------------------------------------------------------------------------
int util_str_csv_print_str( char* pBuf, const char* pStr, int ofs, int bufSize, char separator ){
    if( pBuf == NULL ){
        LOGE( "@ util_str_csv_print_str: INVALID pBuf\n" );
        return( -1 );
    }
    
    // pStr の長さの確認（※NULLも許容しておく＝NULLの場合は空文字となる）
    int len = util_str_len( pStr );
    
    // サイズ確認：「既存の文字列＋区切り文字＋追加文字列＋終端文字」分の空きなければ失敗
    if( bufSize < (ofs + 1 + len + 1) ){
        LOGE( "@ util_str_csv_print_str: BUF OVERFLOW needed=(%d+1+%d+1)/%d\n", ofs, len, bufSize );
        return( -1 );
    }
    
    // 要素の出力
    for( int i=0; i<len; i++ ){
        pBuf[ofs++] = pStr[i];
    }
    
    // 区切り文字の出力
    pBuf[ofs++] = separator;

    // 終端文字の出力（※返値のオフセットはここを指す＝次に同じ処理が呼ばれた際の文字列挿入開始位置となる）
    pBuf[ofs] = '\0';

    return( ofs );
}

//-------------------------------------------------------------------------------
// 区切り文字の変換（※返値は変換数）
//-------------------------------------------------------------------------------
int util_str_csv_change_separator( char* pBuf, char separatorFrom, char separatorTo ){
    // 無効は返す
    if( pBuf == NULL ){
        return( 0 );
    }
    
    // 無意味は返す
    if( separatorFrom == separatorTo ){
        return( 0 );
    }

    int num = 0;

    int at = 0;
    while( pBuf[at] != '\0' ){
        if( pBuf[at] == separatorFrom ){
            pBuf[at] = separatorTo;
            num++;
        }
        at++;
    }

    return( num );
}


//-------------------------------------------------------------------------------
// [uint64]の値を10進数で出力して区切り文字を挿入した位置を返す（※数字のCSV出力用）
//-------------------------------------------------------------------------------
int util_str_csv_print_decimal_uint64( char* pBuf, uint64 val, int ofs, int bufSize, char separator ){
    if( pBuf == NULL ){
        LOGE( "@ util_str_csv_print_decimal_uint64: INVALID pBuf\n" );
        return( -1 );
    }

    char buf[32];   // uint64の収まるバッファ（※２１＋１バイトで大丈夫だとは思うが多めに）
    sprintf( buf, "%llu", val );

    // [str]版を呼び出す
    return( util_str_csv_print_str( pBuf, buf, ofs, bufSize, separator ) );
}

//-------------------------------------------------------------------------------
// 10進数表記の文字列からビットサイズで指定する値域で値を読み込む
//-------------------------------------------------------------------------------
uint64 util_str_read_decimal_uint64_with_bit_range( const char* pSrc, int bitNum ){
    // 無効は[0]扱い
    if( pSrc == NULL ){
        LOGE( "@ util_str_read_decimal_uint64_with_bit_range: INVALID pSrc\n" );
        return( 0 );
    }

    // 読み込みビットサイズが無効なら無視
    if( bitNum <= 0 ){
        LOGE( "@ util_str_read_decimal_uint64_with_bit_range: INVALID bitNum=%d\n", bitNum );
        return( 0 );
    }
    
    // 指定ビットサイズにおける最大値の算出
    uint64 max;
    if( bitNum == 64 ){
        max = 0xFFFFFFFFFFFFFFFF;
    }else if( /*bitNum > 0 &&*/ bitNum < 64 ){
        if( bitNum < 32 ){
            max = (1 << bitNum) - 1;
        }else{
            max = (1 << (bitNum-32)) - 1;
            max <<= 32;
            max |= 0xFFFFFFFF;
        }
    }else{
        LOGE( "@ util_str_read_decimal_uint64_with_bit_range: too big bitNum=%d\n", bitNum );
        return( 0 );
    }

    // 読み込み
    uint64 val;
    if( ! CMath::StrToUnsignedLong( pSrc, &val ) ){
        LOGE( "@ util_str_read_decimal_uint64_with_bit_range: invalid num=%s\n", pSrc );
        return( 0 );
    }

    // オーバーフローしていたら失敗
    if( max < val ){
        LOGE( "@ util_str_read_decimal_uint64_with_bit_range: overflow val=%llu/%llu(%d), pSrc=%s\n", val, max, bitNum, pSrc );
        return( 0 );
    }

    return( val );
}

//-------------------------------------------------------------------------------
// ブロックチェーンワード(16進数のuint256文字列)から文字単位で読み込む（※ビッグエンディアン）
//-------------------------------------------------------------------------------
extern uint64 util_str_read_blockchain_word_with_char_range( const char* pSrc, int strLen, int readNum, int readOfs ){
    // 無効は[0]扱い
    if( pSrc == NULL ){
        LOGE( "@ util_str_read_blockchain_word_with_char_range: INVALID pSrc\n" );
        return( 0 );
    }
    
    // uint64で収まらない場合は無視（※最大で１６文字までしか読み込めない＝１文字で４ビット、１６文字で６４ビット）
    if( readNum > 16 ){
        LOGE( "@ util_str_read_blockchain_word_with_char_range: too big readNum=%d\n", readNum );
        return( 0 );
    }
    
    // オフセットが無効(有効な範囲外)であれば無視
    if( readOfs < 0 || readOfs > (64-readNum) ){
        LOGE( "@ util_str_read_blockchain_word_with_char_range: invalid readOfs=%d\n", readOfs );
        return( 0 );
    }
    
    uint64 val = 0;

    // 指定文字数だけ読み込む（※ビッグエンディアン）
    while( readNum-- > 0 ){
        // 読み込み既の値を１文字分（４ビット）シフト
        val <<= 4;
        
        // 読み込み範囲内であれば（※６４文字に満たない文字列の欠けている先頭領域は０として扱う）
        if( readOfs >= (64-strLen) ){
            // 文字に応じた値を足しこむ
            char c = pSrc[readOfs-(64-strLen)];
            if( c >= '0' && c <= '9' ){
                val += c - '0';
            }else if( c >= 'a' && c <= 'f' ){
                val += 0xa + c - 'a';
            }else if( c >= 'A' && c <= 'F' ){
                val += 0xA + c - 'A';
            }
            // 無効な文字がきたら返す
            else{
                LOGE( "@ util_str_read_blockchain_word_with_char_range: INVALID c=%c\n", c );
                return( 0 );
            }
        }

        readOfs++;
    }
    
    return( val );
}

//-------------------------------------------------------------------------------
// ブロックチェーンのワード(16進数のuint256文字列)を出力する（※ビッグエンディアン）
//-------------------------------------------------------------------------------
extern bool util_str_print_blockchain_word_with_separator( char*pBuf, int bufSize, BYTE* pWords, int numWords, char separator ){
    // 無効確認
    if( pBuf == NULL ){
        LOGE( "@ util_str_print_blockchain_word_with_separator: INVALID pBuf\n" );
        return( false );
    }
    
    if( pWords == NULL ){
        LOGE( "@ util_str_print_blockchain_word_with_separator: INVALID pWords\n" );
        return( false );
    }
    
    if( numWords <= 0 ){
        LOGE( "@ util_str_print_blockchain_word_with_separator: INVALID numWords=%d\n", numWords );
        return( false );
    }
    
    // 必要な文字数の算出
    int checkSize = (64+1) * numWords;
    if( bufSize < checkSize ){
        LOGE( "@ util_str_print_blockchain_word_with_separator: short bufSize=%d/%d\n", bufSize, checkSize );
        return( false );
    }

    int at = 0;
    for( int i=0; i<numWords; i++ ){
        // バイナリの内容を16進数で出力（※ビッグエンディアン）
        for( int j=0; j<32; j++ ){
            sprintf( &pBuf[at], "%02X", pWords[32*i+j] );
            at += 2;
        }

        // 区切り文字の挿入
        pBuf[at++] = separator;
    }
    
    // 最後の区切り文字を終端指定に置き換え
    pBuf[at-1] = '\0';

    return( true );
}
