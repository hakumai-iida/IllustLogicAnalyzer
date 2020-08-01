/*+----------------------------------------------------------------+
  | Title:      Fill.cpp [共通環境]
  | Comment:    塗り（※対象はパレット画像）
  | Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Fill.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 下塗りセルサイズ（※とりあえず）
#define FILL_UNDERCOAT_CELL_MAX  4096

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// コンストラクタ
//------------------------
CFill::CFill( void ){
    clear();
}

//------------------------
// デストラクタ
//------------------------
CFill::~CFill( void ){}

//-------------------
// クリア
//-------------------
void CFill::clear( void ){
    m_eType = eFILL_TYPE_INVALID;
    
    m_pBuf = NULL;
    m_pBufUndercoat = NULL;
    m_pBufGuard = NULL;
    m_nDotW = m_nDotH = 0;

    m_nUndercoatL = m_nUndercoatR = 0;
    m_nUndercoatT = m_nUndercoatB = 0;
    
    m_stArrUndercoatCell = NULL;
    m_nUndercoatCellHeadAt = m_nUndercoatCellTailAt = 0;
}

//------------------------------------------------------------------
// 塗り設定
// [type=eFILL_TYPE_TEX]の場合は[pSrc,srcW,srcH]の指定が必須
//------------------------------------------------------------------
bool CFill::set( eFILL_TYPE type ){
    // 一旦クリア
    clear();
    
    // 無効は無視
    if( type < 0 || type >= eFILL_TYPE_MAX ){
        LOGE( "@ CFill::set: FAILED: INVALID TYPE=%d\n", type );
        return( false );
    }

    // 設定
    m_eType = type;
    
    return( true );
}

//------------------------
// 指定座標から塗りつぶす
//------------------------
bool CFill::fill0( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y, BYTE palGrp, BYTE palVal, BYTE testPalGrp,
                   bool isForAdd, int addVal ){
    // 用心：バッファが無効
    if( pBuf == NULL || pBufGuide == NULL || pBufGuard == NULL ){
        LOGE( "CFill::fill0: INVALID BUF: pBuf=%p, pBufGuide=%p, pBufGuard=%p\n", pBuf, pBufGuide, pBufGuard );
        return( false );
    }
    
    // 用心：サイズが無効
    if( w <= 0 || h <= 0 ){
        LOGE( "CFill::fill0: INVALID SIZE: w=%d, h=%d\n", w, h );
        return( false );
    }

    // 用心：座標が無効
    if( x < 0 || x >= w || y < 0 || y >= h ){
        LOGE( "CFill::fill0: INVALID POSITION: x=%d/%d, y=%d/%d\n", x, w, y, h );
        return( false );
    }

    if( isForAdd ){
        // 用心：加算値が０であれば無視
        if( addVal == 0 ){
            LOGE( "CFill::fill0: INVALID VAL FOR ADD: addVal=%d\n", addVal );
            return( false );
        }
    }else{
        // 用心：通常時：指定座標が出力済みであれば無視
        if( pBufGuide[w*y+x] != BEZIER_PAL_WITHOUT_COL ){
            LOGE( "CFill::fill0: INVALID FILL POSITION: x=%d, y=%d, flag=%X\n", x, y, pBufGuide[w*y+x] );
            return( false );
        }
    }

    //-------------------------------
    // 準備
    //-------------------------------
    m_pBuf = pBuf;
    m_pBufUndercoat = pBufGuide;
    m_pBufGuard = pBufGuard;
    m_nDotW = w;
    m_nDotH = h;

    m_nUndercoatL = w;
    m_nUndercoatR = -1;
    m_nUndercoatT = h;
    m_nUndercoatB = -1;

    readyForUndercoat();

    //---------------------------------------------------
    // 下塗り（※塗りガイドの[x,y]の位置から閉じた範囲を塗りつぶす）
    //---------------------------------------------------
    fillUndercoat( x, y );
    
    // 下塗り範囲が有効であれば
    if( m_nUndercoatL <= m_nUndercoatR && m_nUndercoatT <= m_nUndercoatB ){
        // 領域の調整（処理範囲から１ピクセル拡大させる＝塗りガイドの末端が含まれるように）
        if( m_nUndercoatL > 0 ){ m_nUndercoatL--; }
        if( m_nUndercoatR < (m_nDotW-1) ){ m_nUndercoatR++; }
        if( m_nUndercoatT > 0 ){ m_nUndercoatT--; }
        if( m_nUndercoatB < (m_nDotH-1) ){ m_nUndercoatB++; }

        //-------------------------------------------------------
        // 本塗り（※塗りガイドで下塗りされている画素を色バッファに反映させる）
        //-------------------------------------------------------
        // 広域加算／減算
        if( isForAdd ){
            if( addVal < 0 ){
                subForArea( testPalGrp, -addVal );
            }else if( addVal > 0 ){
                addForArea( testPalGrp, addVal );
            }
        }
        // 単色塗り
        else if( m_eType == eFILL_TYPE_MONO ){
            if( palVal == BEZIER_PAL_VAL_BASE ){
                // 基本色が指定された場合は画素の明暗を引き継ぐ塗りつぶし
                fillForMono( palGrp, testPalGrp );
            }else{
                // 基本色以外が指定された場合は指定色で塗りつぶす
                fillForMonoWithOfs( palGrp, palVal, testPalGrp );
            }
        }
        // オフ塗り
        else if( m_eType == eFILL_TYPE_OFF ){
            fillForOff();
        }
        // ここに来たら困る
        else{
            LOGE( "@ CFill::fill0: INVALID type=%d\n", m_eType );
            return( false );
        }
    }
    
    return( true );
}

//------------------------------
// オフセット（別画素参照）による加算
//------------------------------
bool CFill::fillAddWithOfs( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int ofsX, int ofsY, int addVal, bool isReserve ){
    // 用心：対象バッファが無効
    if( pBuf == NULL || pBufGuide == NULL || pBufGuard == NULL ){
        LOGE( "CFill::fillAddWithOfs: INVALID BUF: pBuf=%p, pBufGuide=%p, pBufGuard=%p\n", pBuf, pBufGuide, pBufGuard );
        return( false );
    }
    
    // 用心：サイズが無効
    if( w <= 0 || h <= 0 ){
        LOGE( "CFill::fillAddWithOfs: INVALID SIZE: w=%d, h=%d\n", w, h );
        return( false );
    }

    // 用心：オフセットが無効
    if( ofsX == 0 && ofsY == 0 ){
        LOGE( "CFill::fillAddWithOfs: INVALID OFFSET: ofsX=%d, ofsY=%d\n", ofsX, ofsY );
        return( false );
    }
    
    // 用心：広域／全色時：加算値が０であれば無視
    if( addVal == 0 ){
        LOGE( "CFill::fillAddWithOfs: INVALID VALN: addVal=%d\n", addVal );
        return( false );
    }
    
    //-------------------------------
    // 準備
    //-------------------------------
    m_pBuf = pBuf;
    m_pBufUndercoat = pBufGuide;
    m_pBufGuard = pBufGuard;
    m_nDotW = w;
    m_nDotH = h;
    
    // 下塗り領域はリセットしない（※直前の状態を参照するのでクリアは不可）

    if( isReserve ){
        if( addVal < 0 ){
            subForReserve( ofsX, ofsY, -addVal );
        }else if( addVal > 0 ){
            addForReserve( ofsX, ofsY, addVal );
        }
    }else{
        if( addVal < 0 ){
            subForColor( ofsX, ofsY, -addVal );
        }else if( addVal > 0 ){
            addForColor( ofsX, ofsY, addVal );
        }
    }
    
    return( true );
}

//--------------------------------------------------------------------------------------------------
// マスク
//--------------------------------------------------------------------------------------------------
bool CFill::fillMask( BYTE* pBuf, BYTE* pBufGuide, int w, int h, int x, int y ){
    // 用心：バッファが無効
    if( pBuf == NULL || pBufGuide == NULL ){
        LOGE( "CFill::fillMask: INVALID BUF: pBuf(Guard)=%p, pBufGuide=%p\n", pBuf, pBufGuide );
        return( false );
    }
    
    // 用心：サイズが無効
    if( w <= 0 || h <= 0 ){
        LOGE( "CFill::fillMask: INVALID SIZE: w=%d, h=%d\n", w, h );
        return( false );
    }

    // 用心：座標が無効
    if( x < 0 || x >= w || y < 0 || y >= h ){
        LOGE( "CFill::fillMask: INVALID POSITION: x=%d/%d, y=%d/%d\n", x, w, y, h );
        return( false );
    }

    //-------------------------------
    // 準備
    //-------------------------------
    m_pBuf = pBuf;      // ここに指定されるのがガードバッファとなる想定
    m_pBufUndercoat = pBufGuide;
    m_pBufGuard = NULL; // ここでは利用しない
    m_nDotW = w;
    m_nDotH = h;

    m_nUndercoatL = w;
    m_nUndercoatR = -1;
    m_nUndercoatT = h;
    m_nUndercoatB = -1;

    readyForUndercoat();

    //---------------------------------------------------
    // 下塗り（※塗りガイドの[x,y]の位置から閉じた範囲を塗りつぶす）
    //---------------------------------------------------
    fillUndercoat( x, y );
    
    // 下塗り範囲が有効であれば
    if( m_nUndercoatL <= m_nUndercoatR && m_nUndercoatT <= m_nUndercoatB ){
        // 領域の調整（処理範囲から１ピクセル拡大させる＝塗りガイドの末端が含まれるように）
        if( m_nUndercoatL > 0 ){ m_nUndercoatL--; }
        if( m_nUndercoatR < (m_nDotW-1) ){ m_nUndercoatR++; }
        if( m_nUndercoatT > 0 ){ m_nUndercoatT--; }
        if( m_nUndercoatB < (m_nDotH-1) ){ m_nUndercoatB++; }

        //-------------------------------------------------------
        // 本塗り（※塗りガイドで下塗りされている画素を色バッファに反映させる）
        //-------------------------------------------------------
        fillForGuard();
    }
    
    return( true );
}

//----------------------
// [下塗り]: 準備
//----------------------
void CFill::readyForUndercoat( void ){
    // ヘッダ位置のクリア
    m_nUndercoatCellHeadAt = m_nUndercoatCellTailAt = 0;

    // バッファ確保
    uint32 reqSize = (uint32)(FILL_UNDERCOAT_CELL_MAX * sizeof( stFILL_UNDERCOAT_CELL ));
    uint32 bufSize = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_FILL_CELL_OR_SS );
    m_stArrUndercoatCell = (stFILL_UNDERCOAT_CELL*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_FILL_CELL_OR_SS );

    // 領域不足なら（※ここに来たらバッファの拡張を検討）
    if( reqSize > bufSize ){
        LOGE( "@ CFill::readyForWork: BUF SHORTAGE: reqSize(%u) > bufSize(%u)\n", reqSize, bufSize );
        m_stArrUndercoatCell = NULL;
    }
}

//----------------------
// [下塗り]: セルが空か？
//----------------------
bool CFill::isUndercoatCellEmpty( void ){
    return( m_nUndercoatCellHeadAt == m_nUndercoatCellTailAt );
}

//----------------------
// [下塗り]: セルの追加
//----------------------
void CFill::addUndercoatCell( int hint, int xL, int xR, int y ){
    stFILL_UNDERCOAT_CELL* pCell = &m_stArrUndercoatCell[m_nUndercoatCellTailAt];
    pCell->hint = hint;
    pCell->xL = xL;
    pCell->xR = xR;
    pCell->y = y;

    // 次の要素へ（※バッファの終端へ来ていたら循環させる）
    m_nUndercoatCellTailAt++;
    if( m_nUndercoatCellTailAt >= FILL_UNDERCOAT_CELL_MAX ){
        m_nUndercoatCellTailAt = 0;
    }
}

//----------------------
// [下塗り]: セルの取得
//----------------------
stFILL_UNDERCOAT_CELL* CFill::getUndercoatCell( void ){
    stFILL_UNDERCOAT_CELL* pCell = &m_stArrUndercoatCell[m_nUndercoatCellHeadAt];

    // 次の要素へ（※バッファの終端へ来ていたら循環させる）
    m_nUndercoatCellHeadAt++;
    if( m_nUndercoatCellHeadAt >= FILL_UNDERCOAT_CELL_MAX ){
        m_nUndercoatCellHeadAt = 0;
    }

    return( pCell );
}

//-----------------------------------------------
// [下塗り]: ガイド領域を[x,y]位置から下塗り色で塗りつぶす
//-----------------------------------------------
void CFill::fillUndercoat( int x, int y ){
    BYTE* p;
    int xL, xR;

    // 指定座標を追加（※開始時はなんのヒントもないので値は０で）
    addUndercoatCell( 0, x, x, y );
    
    // セルが有効であれば
    while( ! isUndercoatCellEmpty() ){
        // 処理対象の取得
        stFILL_UNDERCOAT_CELL* pCell = getUndercoatCell();

        // 都度[m_nDotW*y+x]でアクセスするのもあれなので指定行を配列で取り出しておく
        y = pCell->y;
        p = &m_pBufUndercoat[m_nDotW*y];

        //------------------------------------------------------------------------------------------------
        // 指定位置が対象色でなくなっていたら無視
        //------------------------------------------------------------------------------------------------
        // [UNDERCOAT_COL]色の書き込みは[fillUndercoat]でのみ行われるが、この時、横方向に連なる領域が塗りつぶされる
        // また、ワークセルに登録されて処理される対象の区域は、登録時には[未出力＝抜き色:0x00]であったことが保証される
        // もし、[xL-xR]内のいずれかのドットが対象色で無くなっていた場合、先行するワークセルにより塗りつぶされたと判断できる
        //------------------------------------------------------------------------------------------------
        if( p[pCell->xL] != BEZIER_PAL_WITHOUT_COL ){
            continue;
        }
        
        // 左サイドの確認（※閉じていなければ指定値の先を走査）
        xL = pCell->xL;
        if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_CLOSE_LEFT) == 0 ){
            while( xL > 0 && p[xL-1] == BEZIER_PAL_WITHOUT_COL ){
                xL--;
            }
        }

        // 右サイドの確認（※閉じていなければ指定値の先を走査）
        xR = pCell->xR;
        if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_CLOSE_RIGHT) == 0 ){
            while( (xR+1) < m_nDotW && p[xR+1] == BEZIER_PAL_WITHOUT_COL ){
                xR++;
            }
        }

        // ここまできたら指定範囲の塗りつぶし
        if( xR >= xL ){
            memset( &p[xL], BEZIER_PAL_UNDERCOAT_COL, xR-xL+1 );
        }

        // 処理範囲の更新
        if( m_nUndercoatL > xL ){
            m_nUndercoatL = xL;
        }
        if( m_nUndercoatR < xR){
            m_nUndercoatR = xR;
        }
        if( m_nUndercoatT > y ){
            m_nUndercoatT = y;
        }
        if( m_nUndercoatB < y ){
            m_nUndercoatB = y;
        }
       
        // 一つ上の走査
        if( y > 0 ){
            // 上からきていた場合は調整（※処理済みの領域に対して逆走しない）
            if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_MOVE_DOWN) != 0 ){
                // 左側の未走査をチェック
                if( xL < pCell->xL ){
                    scanLineForUndercoat( xL, pCell->xL-1, y-1, true );
                }
                // 右側の未走査をチェック
                if( xR > pCell->xR ){
                    scanLineForUndercoat( pCell->xR+1, xR, y-1, true );
                }
            }else{
                scanLineForUndercoat( xL, xR, y-1, true );
            }
        }
        
        // 一つ下の走査
        if( y < (m_nDotH-1) ){
            // 下からきていた場合は調整（※処理済みの領域に対して逆走しない）
            if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_MOVE_UP) != 0 ){
                // 左側の未走査をチェック
                if( xL < pCell->xL ){
                    scanLineForUndercoat( xL, pCell->xL-1, y+1, false );
                }
                // 右側の未走査をチェック
                if( xR > pCell->xR ){
                    scanLineForUndercoat( pCell->xR+1, xR, y+1, false );
                }
            }else{
                scanLineForUndercoat( xL, xR, y+1, false );
            }
        }
    }
}

//------------------------------------------------
// [下塗り]: 下塗り領域を走査して塗り対象であればセルに登録
//------------------------------------------------
void CFill::scanLineForUndercoat( int xL, int xR, int y, bool isMoveUp ){
    BYTE* p = &m_pBufUndercoat[m_nDotW*y];
    int xStart = xL;
    while( xStart <= xR ){
        // 未出力以外は無視
        while( xStart <= xR && p[xStart] != BEZIER_PAL_WITHOUT_COL ){
            xStart++;
        }
        
        // 対象色が見つからなければ終了
        if( xStart > xR ){
            return;
        }
        
        // 未出力が途切れるまで走査
        int xEnd = xStart;
        while( (xEnd+1) <= xR && p[xEnd+1] == BEZIER_PAL_WITHOUT_COL ){
            xEnd++;
        }

        // ここまできたらヒントと座標の登録
        int hint = (isMoveUp)? FILL_UNDERCOAT_SCAN_HINT_MOVE_UP: FILL_UNDERCOAT_SCAN_HINT_MOVE_DOWN;
        
        // 左端が内側：左サイドが閉じている
        if( xStart > xL ){
            hint |= FILL_UNDERCOAT_SCAN_HINT_CLOSE_LEFT;
        }
        // 右端が内側：右サイドが閉じている
        if( xEnd < xR ){
            hint |= FILL_UNDERCOAT_SCAN_HINT_CLOSE_RIGHT;
        }

        // ここまできたら登録
        addUndercoatCell( hint, xStart, xEnd, y );
        xStart = xEnd + 1;
    }
}

//-----------------------------------------------------------------------------
// [本塗り]: オフ塗り（※出力フラグのみ書き込む）
//-----------------------------------------------------------------------------
void CFill::fillForOff( void ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        //BYTE* pOut = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            // 塗りつぶし完了フラグ
            *pUndercoat = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;

            // 出力フラグを立てるのみ（※色の出力は行わない）
            if( *pUndercoat == BEZIER_PAL_UNDERCOAT_COL ){
                if( *pGuard == 0 ){
                    *pUndercoat = BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG;
                }
            }
            
            //pOut++;
            pUndercoat++;
            pGuard++;
        }
    }
}

//-----------------------------------------------------------------------------
// [本塗り]: ガード塗り（※ガードフラグのみ書き込む）
//-----------------------------------------------------------------------------
void CFill::fillForGuard( void ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pOut = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y+m_nUndercoatL];
        //BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            // 下塗り色の部分を出力するのみ
            if( *pUndercoat == BEZIER_PAL_UNDERCOAT_COL ){
                // 塗りつぶし完了フラグ（※意味はないと思われるが一応立てておく）
                *pUndercoat = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;

                // 値の上書き（※出力先はガードを想定するので塗られているかどうかの判定にしか利用されない）
                // 画素の更新ではないのでベジェ管理への通知は不要
                *pOut = 1;
            }
            
            pOut++;
            pUndercoat++;
            //pGuard++;
        }
    }
}

//----------------------------------------------------------------------
// [本塗り]: 単色塗り（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//----------------------------------------------------------------------
void CFill::fillForMono( BYTE palGrp, BYTE testPalGrp ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pOut = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( *pUndercoat == BEZIER_PAL_UNDERCOAT_COL ){
                // 塗りつぶし完了フラグ
                *pUndercoat = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;

                if( *pGuard == 0 ){
                    // テストに通れば
                    if( BEZIER_CONV_DOT_TO_PAL_GRP( *pOut ) == testPalGrp ){
                        // 明暗を引き継いで出力
                        int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( *pOut );
                        *pOut = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                        
                        // ベジェに対して画素領域の更新（※新たに色が塗られた領域を通知）
                        CBezier::UpdateColorBufInfo( x, y );

                        // 出力したフラグ
                        *pUndercoat |= BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG;
                    }
                }
            }
            
            pOut++;
            pUndercoat++;
            pGuard++;
        }
    }
}

//-----------------------------------------------------------------------------------
// [本塗り]: オフセットによる単色塗り（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//-----------------------------------------------------------------------------------
void CFill::fillForMonoWithOfs( BYTE palGrp, int palValOfs, BYTE testPalGrp ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pOut = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( *pUndercoat == BEZIER_PAL_UNDERCOAT_COL ){
                // 塗りつぶし完了フラグ
                *pUndercoat = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;

                if( *pGuard == 0 ){
                    // テストに通れば
                    if( BEZIER_CONV_DOT_TO_PAL_GRP( *pOut ) == testPalGrp ){
                        // 指定のオフセットで出力
                        *pOut = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palValOfs );
                        
                        // ベジェに対して画素領域の更新（※新たに色が塗られた領域を通知）
                        CBezier::UpdateColorBufInfo( x, y );
                        
                        // 出力したフラグ
                        *pUndercoat |= BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG;
                    }
                }
            }

            pOut++;
            pUndercoat++;
            pGuard++;
        }
    }
}

//-------------------------
// [本塗り]: エリアに加算
//-------------------------
void CFill::addForArea( BYTE testPalGrp, int addVal ){
    // 有効な画素領域の取得
    int l = CBezier::GetLeftForColorBuf();
    int r = CBezier::GetRightForColorBuf();
    int t = CBezier::GetTopForColorBuf();
    int b = CBezier::GetBottomForColorBuf();
    
    // 下塗りエリアで制限（※下塗りエリア外は無視できる）
    if( l < m_nUndercoatL ){ l = m_nUndercoatL; }
    if( r > m_nUndercoatR ){ r = m_nUndercoatR; }
    if( t < m_nUndercoatT ){ t = m_nUndercoatT; }
    if( b > m_nUndercoatB ){ b = m_nUndercoatB; }

    for( int y=t; y<=b; y++ ){
        BYTE* pOut = &m_pBuf[m_nDotW*y + l];
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=l; x<=r; x++ ){
            if( *pUndercoat == BEZIER_PAL_UNDERCOAT_COL ){
                // 塗りつぶし完了フラグ
                *pUndercoat = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;

                if( *pGuard == 0 ){
                    // 有効な画素でテストに通れば
                    if( *pOut >= BEZIER_PAL_GRP_HEAD && (testPalGrp==0x00 ||BEZIER_CONV_DOT_TO_PAL_GRP( *pOut )==testPalGrp) ){
                        int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( *pOut );
                        int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( *pOut );

                        // パレット値を加算（暗く）
                        palVal += addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }
                        
                        // 画素の更新なのでベジェ管理への通知は不要
                        *pOut = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palVal );

                        // 出力したフラグ
                        *pUndercoat |= BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG;
                    }
                }
            }
            
            pOut++;
            pUndercoat++;
            pGuard++;
        }
    }
}

//-------------------------
// [本塗り]: エリアに減算
//-------------------------
void CFill::subForArea( BYTE testPalGrp, int subVal ){
    // 有効な画素領域の取得
    int l = CBezier::GetLeftForColorBuf();
    int r = CBezier::GetRightForColorBuf();
    int t = CBezier::GetTopForColorBuf();
    int b = CBezier::GetBottomForColorBuf();

    // 下塗りエリアで制限（※下塗りエリア外は無視できる）
    if( l < m_nUndercoatL ){ l = m_nUndercoatL; }
    if( r > m_nUndercoatR ){ r = m_nUndercoatR; }
    if( t < m_nUndercoatT ){ t = m_nUndercoatT; }
    if( b > m_nUndercoatB ){ b = m_nUndercoatB; }
    
    for( int y=t; y<=b; y++ ){
        BYTE* pOut = &m_pBuf[m_nDotW*y + l];
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=l; x<=r; x++ ){
            if( *pUndercoat == BEZIER_PAL_UNDERCOAT_COL ){
                // 塗りつぶし完了フラグ
                *pUndercoat = BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG;

                if( *pGuard == 0 ){
                    // 有効な画素でテストに通れば
                    if( *pOut >= BEZIER_PAL_GRP_HEAD && (testPalGrp==0x00||BEZIER_CONV_DOT_TO_PAL_GRP( *pOut )==testPalGrp) ){
                        int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( *pOut );
                        int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( *pOut );

                        // パレット値を減算（明るく）
                        palVal -= subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }
                        
                        // 画素の更新なのでベジェ管理への通知は不要
                        *pOut = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palVal );

                        // 出力したフラグ
                        *pUndercoat |= BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG;
                    }
                }
            }
            
            pOut++;
            pUndercoat++;
            pGuard++;
        }
    }
}

//-------------------------
// [本塗り]: 全色に加算
//-------------------------
void CFill::addForColor( int ofsX, int ofsY, int addVal ){
    // 出力済みの領域に対して確認
    int l = CBezier::GetLeftForColorBuf();
    int r = CBezier::GetRightForColorBuf();
    int t = CBezier::GetTopForColorBuf();
    int b = CBezier::GetBottomForColorBuf();
    for( int y=t; y<=b; y++ ){
        BYTE* pOut = &m_pBuf[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 有効な画素であれば
            if( *pOut >= BEZIER_PAL_GRP_HEAD ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( *pOut );
                    int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( *pOut );

                    // 加算（暗く）
                    palVal += addVal;
                    if( palVal > BEZIER_PAL_VAL_MAX ){
                        palVal = BEZIER_PAL_VAL_MAX;
                    }
                    
                    // 画素の更新なのでベジェ管理への通知は不要
                    *pOut = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                }
            }
            
            pOut++;
        }
    }
}

//-------------------------
// [本塗り]: 全色に減算
//-------------------------
void CFill::subForColor( int ofsX, int ofsY, int subVal ){
    // 出力済みの領域に対して確認
    int l = CBezier::GetLeftForColorBuf();
    int r = CBezier::GetRightForColorBuf();
    int t = CBezier::GetTopForColorBuf();
    int b = CBezier::GetBottomForColorBuf();
    for( int y=t; y<=b; y++ ){
        BYTE* pOut = &m_pBuf[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 有効な画素であれば
            if( *pOut >= BEZIER_PAL_GRP_HEAD  ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    int palGrp = BEZIER_CONV_DOT_TO_PAL_GRP( *pOut );
                    int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( *pOut );

                    // 減算（明るく）
                    palVal -= subVal;
                    if( palVal < BEZIER_PAL_VAL_MIN ){
                        palVal = BEZIER_PAL_VAL_MIN;
                    }

                    // 画素の更新なのでベジェ管理への通知は不要
                    *pOut = BEZIER_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                }
            }
            
            pOut++;
        }
    }
}

//-------------------------
// [本塗り]: 予約に加算
//-------------------------
void CFill::addForReserve( int ofsX, int ofsY, int addVal ){
    // 有効な画素領域の取得
    int l = CBezier::GetLeftForColorBuf();
    int r = CBezier::GetRightForColorBuf();
    int t = CBezier::GetTopForColorBuf();
    int b = CBezier::GetBottomForColorBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 塗り領域に含まれていたら
            if( (*pUndercoat&BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG) != 0 ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    // 加算（暗く）
                    int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( m_pBuf[m_nDotW*targetY+targetX] ) + addVal;
                    if( palVal > BEZIER_PAL_VAL_MAX ){
                        palVal = BEZIER_PAL_VAL_MAX;
                    }
                    
                    // 画素の更新なのでベジェ管理への通知は不要
                    m_pBuf[m_nDotW*targetY+targetX] = BEZIER_CONV_PAL_INFO_TO_DOT( 0x00, palVal );
                }
            }
            
            pUndercoat++;
        }
    }
}

//-------------------------
// [本塗り]: 予約に減算
//-------------------------
void CFill::subForReserve( int ofsX, int ofsY, int subVal ){
    // 有効な画素領域の取得
    int l = CBezier::GetLeftForColorBuf();
    int r = CBezier::GetRightForColorBuf();
    int t = CBezier::GetTopForColorBuf();
    int b = CBezier::GetBottomForColorBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pUndercoat = &m_pBufUndercoat[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 塗り領域に含まれていたら
            if( (*pUndercoat&BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG) != 0 ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    // 減算（明るく）
                    int palVal = BEZIER_CONV_DOT_TO_PAL_VAL( m_pBuf[m_nDotW*targetY+targetX] ) - subVal;
                    if( palVal < BEZIER_PAL_VAL_MIN ){
                        palVal = BEZIER_PAL_VAL_MIN;
                    }
                    
                    // 画素の更新なのでベジェ管理への通知は不要
                    m_pBuf[m_nDotW*targetY+targetX] = BEZIER_CONV_PAL_INFO_TO_DOT( 0x00, palVal );
                }
            }
            
            pUndercoat++;
        }
    }
}
