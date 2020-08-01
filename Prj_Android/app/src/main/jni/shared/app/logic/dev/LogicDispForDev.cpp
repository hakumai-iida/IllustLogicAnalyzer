/*+----------------------------------------------------------------+
  |	Title:		LogicDispForDev.cpp [共通環境]
  |	Comment:	ロジック表示（※[CLogicGrid]の内容を直接表示する開発用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LogicDispForDev.hpp"

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
//-----------------------
// コンストラクタ
//-----------------------
CLogicDispForDev::CLogicDispForDev( void ){
    clear();
}

//-----------------------
// デストラクタ
//-----------------------
CLogicDispForDev::~CLogicDispForDev( void ){
    clear();
}

//-----------------------
// クリア
//-----------------------
void CLogicDispForDev::clear( void ){
    m_pGrid = NULL;
    m_fOriginalCenterX = m_fOriginalCenterY = 0.0f;
    m_fOriginalW = m_fOriginalH = 0.0f;
    
    m_fOfsX = m_fOfsY = 0.0f;

    m_fGridScale = 0.0f;
    m_fGridX = m_fGridY = 0.0f;
    m_fGridW = m_fGridH = 0.0f;
    m_fGridLineSize = 0.0f;
    m_fClueX = m_fClueY = 0.0f;
    m_fClueW = m_fClueH = 0.0f;
    m_fSolidW = m_fSolidH = 0.0f;
    m_fSolidTexW = m_fSolidTexH = 0.0f;
    m_fClueNumW1 = 0.0f;
    m_fClueNumW2 = 0.0f;
    m_fClueNumMarginX = 0.0f;
    m_fClueNumH = 0.0f;
    m_fClueNumMarginY = 0.0f;

    // 解放
    SAFE_DELETE( m_pTexLineForCross );
    SAFE_DELETE( m_pTexColorForCross );
}

//-----------------------
// 初期化
//-----------------------
void CLogicDispForDev::init( CLogicGrid* pGrid, float centerX, float centerY, float w, float h ){
    // 用心にクリア
    clear();
    
    // 無効は無視
    if( pGrid == NULL || w <= 0 || h <= 0 ){
        LOGE( "@ CLogicDispForDev::init: INVALID: pGrid=%p, centerX=%f, centerY=%f, w=%f, h=%f\n", pGrid, centerX, centerY, w, h );
        return;
    }
    
    //----------------------
    // サイズ確定
    //----------------------
    m_pGrid = pGrid;
    m_fOriginalCenterX = centerX;
    m_fOriginalCenterY = centerY;
    m_fOriginalW = w;
    m_fOriginalH = h;
    
    //----------------------------------------------
    // 表示準備（※派生先で上書きされているであろう処理を呼ぶ）
    //----------------------------------------------
    readyForDisp();
}

//----------------------
// タッチ確認：ヒント（ヨコ）
//----------------------
bool CLogicDispForDev::checkTouchForColClue( eTOUCH_CHECK check, int *pSelectCol ){
    // 無効は無視
    if( ! isValid() ){
        return( false );
    }

    // ヒント（ヨコ）を操作していたら
    float clueX = m_fClueX + m_fOfsX;
    float gridY = m_fGridY + m_fOfsY;
    CTouchStatusCell* pTouchStatus = CTouchMgr::CheckRect( check, clueX, gridY, m_fClueW, m_fGridH );
    if( pTouchStatus != NULL ){
        int touchY = (int)(pTouchStatus->getY() - gridY);
        int atY = touchY / m_fSolidH;

        // 用心
        if( atY < 0 ){
            atY = 0;
        }if( atY >= m_pGrid->getColumn() ){
            atY = m_pGrid->getColumn();
        }

        if( pSelectCol != NULL ){
            *pSelectCol = atY;
        }

        return( true );
    }

    return( false );
}

//----------------------
// タッチ確認：ヒント（タテ）
//----------------------
bool CLogicDispForDev::checkTouchForRowClue( eTOUCH_CHECK check, int *pSelectRow ){
    // 無効は無視
    if( ! isValid() ){
        return( false );
    }

    // ヒント（タテ）をタッチしていたら
    float gridX = m_fGridX + m_fOfsX;
    float clueY = m_fClueY + m_fOfsY;
    CTouchStatusCell* pTouchStatus = CTouchMgr::CheckRect( check, gridX, clueY, m_fGridW, m_fClueH );
    if( pTouchStatus != NULL ){
        int touchX = (int)(pTouchStatus->getX() - gridX);
        int atX = touchX / m_fSolidW;
        
        // 用心
        if( atX < 0 ){
            atX = 0;
        }if( atX >= m_pGrid->getRow() ){
            atX = m_pGrid->getRow();
        }

        if( pSelectRow != NULL ){
            *pSelectRow = atX;
        }
            
        return( true );
    }
    
    return( false );
}

//-----------------------
// 描画
//-----------------------
void CLogicDispForDev::draw( bool isZeroVisible,
                             int focusColAt, int focusRowAt,
                             int focusColClueAt, int focusRowClueAt, int focusColBlockAt, int focusRowBlockAt ){
    // 無効は無視
    if( ! isValid() ){
        return;
    }
    
    CDrawCell* pDC = CDrawCell::GetFreeCell();
    
    int row = m_pGrid->getRow();
    int col = m_pGrid->getColumn();
    
    DWORD rgba;
    float tempW, tempH, scale;
    float ofsX, ofsY;
    int blockAt;
    
    float gridX = m_fGridX + m_fOfsX;
    float gridY = m_fGridY + m_fOfsY;
    float clueX = m_fClueX + m_fOfsX;
    float clueY = m_fClueY + m_fOfsY;
    
    float iconX = gridX - (row+4);
    float iconY = gridY - (col+4);

    //-------------------------------------------
    // 下地
    //-------------------------------------------
    rgba = 0xCCCCCCFF;
    
    // 盤面
    pDC->clear();
    pDC->setRect( m_fGridW, m_fGridH );
    pDC->setRGBA( rgba );
    pDC->draw( gridX, gridY );
    
    // ヒント（ヨコ）
    pDC->clear();
    pDC->setRect( m_fGridW, m_fClueH );
    pDC->setRGBA( rgba );
    pDC->draw( gridX, clueY );
    
    // ヒント（タテ）
    pDC->clear();
    pDC->setRect( m_fClueW, m_fGridH );
    pDC->setRGBA( rgba );
    pDC->draw( clueX, gridY );
    
    //-------------------------------------------
    // マス
    //-------------------------------------------
    for( int i=0; i<col; i++ ){
        for( int j=0; j<row; j++ ){
            CLogicSolid* pSolid = m_pGrid->getSolid( i, j );
            eLOGIC_SOLID_STATUS status = pSolid->getStatus();
            eLOGIC_SOLID_TYPE type = pSolid->getType();
            int assumedLevel = pSolid->getAssumedLevel();
            
            // 確定
            if( status == eLOGIC_SOLID_STATUS_DETERMINED ){
                rgba = (type == eLOGIC_SOLID_TYPE_BLACK)? 0x000000FF: 0xFFFFFFFF;
            }
            // 仮定
            else if( status == eLOGIC_SOLID_STATUS_ASSUMED ){
                rgba = (type == eLOGIC_SOLID_TYPE_BLACK)? 0x600080FF: 0xEEDDFFFF;
            }
            // 仮定開始
            else if( status == eLOGIC_SOLID_STATUS_ASSUMED_START ){
                rgba = (type == eLOGIC_SOLID_TYPE_BLACK)? 0x800060FF: 0xFFDDEEFF;
            }
            // その他は無視
            else{
                continue;
            }

            // 仮定状態は色分けで十分そうなのでサイズ調整は一旦オフ
            tempW = m_fSolidW;
            tempH = m_fSolidH;
            
            float x = gridX + m_fSolidW*j + m_fSolidW/2;
            float y = gridY + m_fSolidH*i + m_fSolidH/2;
            
            // タイプにより表示わけ
            if( type == eLOGIC_SOLID_TYPE_BLACK ){
                // 黒マス
                pDC->clear();
                pDC->setRect( m_fSolidW, m_fSolidH );
                pDC->setRGBA( rgba );
                pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
                pDC->draw( x, y );
            }else if( type == eLOGIC_SOLID_TYPE_WHITE ){
                // 白マス
                pDC->clear();
                pDC->setRect( m_fSolidW, m_fSolidH );
                pDC->setRGBA( rgba );
                pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
                pDC->draw( x, y );
                
                // 確定していたら×テクスチャ（※仮定時は数字が乗るので汚く見える）
                if( assumedLevel <= 0 ){
                    pDC->clear();
                    pDC->setTex( m_pTexLineForCross, NULL );
                    pDC->setRGBA( rgba );
                    pDC->setWidth( 4*tempW );
                    pDC->setHeight( 4*tempH );
                    pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
                    pDC->setBlendAlpha();
                    pDC->draw( x, y );
                }
            }

            // アイコンを描いておく
            if( type == eLOGIC_SOLID_TYPE_BLACK || type == eLOGIC_SOLID_TYPE_WHITE ){
                pDC->clear();
                pDC->setRect( 1, 1 );
                pDC->setRGBA( rgba );
                pDC->draw( iconX+j, iconY+i );
            }
            
            // 仮定状態であれば仮定レベルの表示
            if( assumedLevel > 0 ){
                pDC->clear();
                pDC->setNum( eNUM_DEBUG, assumedLevel );
                pDC->setRGBA( 0xFFFFFFFF );
                if( assumedLevel >= 10 ){
                    pDC->setWidth( m_fClueNumW2 );
                }else{
                    pDC->setWidth( m_fClueNumW1 );
                }
                pDC->setHeight( m_fClueNumH );
                pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
                pDC->draw( x, y );
            }
        }
    }

    //-------------------------------------------
    // 枠線
    //-------------------------------------------
    // ヨコ
    for( int i=0; i<=col; i++ ){
        float size = ( (i%5)==0 || i==col )? 2.0f*m_fGridLineSize: m_fGridLineSize;
        
        pDC->clear();
        pDC->setRect( m_fGridW+m_fClueW+size/2.0f, size );
        pDC->setRGBA( 0x404040FF );
        pDC->draw( clueX, gridY+i*m_fSolidH-size/2.0f );
    }
    
    // タテ
    for( int i=0; i<=row; i++ ){
        float size = ( (i%5)==0 || i==row )? 2.0f*m_fGridLineSize: m_fGridLineSize;
        
        pDC->clear();
        pDC->setRect( size, m_fGridH+m_fClueH+size/2.0f );
        pDC->setRGBA( 0x404040FF );
        pDC->draw( gridX+i*m_fSolidW-size/2.0f, clueY );
    }
    
    //-------------------------------------------
    // 選択カーソル（マス）
    //-------------------------------------------
    // ヨコ
    if( focusColAt >= 0 ){
        float size = 2.0f * m_fGridLineSize;
        
        pDC->clear();
        pDC->setRect( m_fGridW+m_fClueW+size/2.0f, size );
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( clueX, gridY+focusColAt*m_fSolidH-size/2.0f );
        pDC->draw( clueX, gridY+(focusColAt+1)*m_fSolidH-size/2.0f );
        
        // フタ
        pDC->setRect( size, m_fSolidH +size/2);
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( clueX, gridY+focusColAt*m_fSolidH );
        pDC->draw( gridX+m_fGridW-size/2.0f, gridY+focusColAt*m_fSolidH );
    }
    
    // タテ
    if( focusRowAt >= 0 ){
        float size = 2.0f * m_fGridLineSize;
        
        pDC->clear();
        pDC->setRect( size, m_fGridH+m_fClueH+size/2.0f );
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( gridX+focusRowAt*m_fSolidW-size/2.0f, clueY );
        pDC->draw( gridX+(focusRowAt+1)*m_fSolidW-size/2.0f, clueY );
        
        // フタ
        pDC->setRect( m_fSolidW+size/2.0f, size );
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( gridX+focusRowAt*m_fSolidW, clueY );
        pDC->draw( gridX+focusRowAt*m_fSolidW, gridY+m_fGridH-size/2.0f );
    }
    
    //-------------------------------------------
    // 選択カーソル（ヒント）
    //-------------------------------------------
    // ヨコ
    if( focusColClueAt >= 0 ){
        float size = 2.0f * m_fGridLineSize;
        
        pDC->clear();
        pDC->setRect( m_fClueW+size/2.0f, size );
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( clueX, gridY+focusColClueAt*m_fSolidH-size/2.0f );
        pDC->draw( clueX, gridY+(focusColClueAt+1)*m_fSolidH-size/2.0f );
        
        // フタ
        pDC->setRect( size, m_fSolidH +size/2);
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( clueX, gridY+focusColClueAt*m_fSolidH );
        pDC->draw( clueX+m_fClueW-size/2.0f, gridY+focusColClueAt*m_fSolidH );
    }
    
    // タテ
    if( focusRowClueAt >= 0 ){
        float size = 2.0f * m_fGridLineSize;
        
        pDC->clear();
        pDC->setRect( size, m_fClueH+size/2.0f );
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( gridX+focusRowClueAt*m_fSolidW-size/2.0f, clueY );
        pDC->draw( gridX+(focusRowClueAt+1)*m_fSolidW-size/2.0f, clueY );
        
        // フタ
        pDC->setRect( m_fSolidW+size/2.0f, size );
        pDC->setRGBA( 0xFF4040FF );
        pDC->draw( gridX+focusRowClueAt*m_fSolidW, clueY );
        pDC->draw( gridX+focusRowClueAt*m_fSolidW, clueY+m_fClueH-size/2.0f );
    }
    
    //-------------------------------------------
    // ヒントの数字
    //-------------------------------------------
    // ヨコ
    for( int i=0; i<col; i++ ){
        int numClue = m_pGrid->getClueNumForColumn( i );
        if( numClue <= 0 && isZeroVisible ){
            numClue = 1;
        }
        
        ofsX = m_fClueNumMarginX;
        for( int j=0; j<numClue; j++ ){
            blockAt = numClue-(j+1);
            CLogicClue* pClue = m_pGrid->getClueForColumn( i, blockAt );
            eLOGIC_CLUE_STATUS status = pClue->getStatus();
            int val = pClue->getVal();
            
            float x = clueX + m_fClueW - ofsX;
            float y = gridY + m_fSolidH*i + m_fSolidH/2;
            
            if( pClue->isError() ){
                rgba = 0x8080FFFF;
            }else if( status == eLOGIC_CLUE_STATUS_DETERMINED ){
                rgba = 0x808080FF;
            }else if( status == eLOGIC_CLUE_STATUS_ASSUMED ){
                rgba = 0xCC00FFFF;
            }else if( status == eLOGIC_CLUE_STATUS_IMCOMPATIBLE ){
                rgba = 0xFF4040FF;
            }else/* if( status == eLOGIC_CLUE_STATUS_INVALID )*/{
                rgba = 0xFFFFFFFF;
            }
            
            // 選択対象であれば
            if( i == focusColClueAt && blockAt == focusColBlockAt ){
                rgba = 0xFF8080FF;
                scale = (val<10)? 1.5f: 1.3f;
            }else{
                scale = 1.0f;
            }
            
            pDC->clear();
            pDC->setNum( eNUM_DEBUG, val);
            pDC->setRGBA( rgba );
            pDC->setWidth( ((val<10)? m_fClueNumW1: m_fClueNumW2) * scale );
            pDC->setHeight( m_fClueNumH * scale );
            pDC->setAlignXY( eDRAW_ALIGN_X_R, eDRAW_ALIGN_Y_M );
            pDC->draw( x, y );
            
            ofsX += pDC->getWidth() + ((val<10)? m_fClueNumMarginX: (m_fClueNumMarginX-(m_fClueNumW2-m_fClueNumW1)));
        }
    }
    
    // タテ
    for( int i=0; i<row; i++ ){
        int numClue = m_pGrid->getClueNumForRow( i );
        
        ofsY = m_fClueNumMarginY;
        for( int j=0; j<numClue; j++ ){
            blockAt = numClue-(j+1);
            CLogicClue* pClue = m_pGrid->getClueForRow( i, blockAt );
            eLOGIC_CLUE_STATUS status = pClue->getStatus();
            int val = pClue->getVal();
            
            float x = gridX + m_fSolidW*i + m_fSolidW/2;
            float y = clueY + m_fClueH - ofsY;
            
            if( pClue->isError() ){
                rgba = 0x8080FFFF;
            }else if( status == eLOGIC_CLUE_STATUS_DETERMINED ){
                rgba = 0x808080FF;
            }else if( status == eLOGIC_CLUE_STATUS_ASSUMED ){
                rgba = 0xCC00FFFF;
            }else if( status == eLOGIC_CLUE_STATUS_IMCOMPATIBLE ){
                rgba = 0xFF8080FF;
            }else/* if( status == eLOGIC_CLUE_STATUS_INVALID )*/{
                rgba = 0xFFFFFFFF;
            }
            
            // 選択対象であれば
            if( i == focusRowClueAt && blockAt == focusRowBlockAt ){
                rgba = 0xFF8080FF;
                scale = 1.5f;
            }else{
                scale = 1.0f;
            }
            
            pDC->clear();
            pDC->setNum( eNUM_DEBUG, val );
            pDC->setRGBA( rgba );
            pDC->setWidth( m_fClueNumW1 * scale );
            pDC->setHeight( m_fClueNumH * scale );
            pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_B );
            pDC->draw( x, y );
            
            ofsY += pDC->getHeight() + m_fClueNumMarginY;
        }
    }
}

//-----------------------
// 座標取得
//-----------------------
void CLogicDispForDev::getXYForClue( int focusColClueAt, int focusRowClueAt, float* pX0, float*pY0, float* pX1, float* pY1 ){
    int gridX = m_fGridX + m_fOfsX;
    int gridY = m_fGridY + m_fOfsY;
    int clueX = m_fClueX + m_fOfsX;
    int clueY = m_fClueY + m_fOfsY;

    if( focusColClueAt >= 0 ){
        float size = 2.0f * m_fGridLineSize;

        *pX0 = clueX;
        *pY0 = gridY + m_fSolidH/2.0f + focusColClueAt*m_fSolidH;
        *pX1 = clueX + m_fClueW-size/2.0f;
        *pY1 = gridY + m_fSolidH/2.0f + focusColClueAt*m_fSolidH;
    }else if( focusRowClueAt >= 0 ){
        float size = 2.0f * m_fGridLineSize;
        
        *pX0 = gridX + m_fSolidW/2.0f + focusRowClueAt*m_fSolidW;
        *pY0 = clueY;
        *pX1 = gridX + m_fSolidW/2.0f + focusRowClueAt*m_fSolidW;
        *pY1 = clueY + m_fClueH-size/2.0f;
    }
}

//---------------------
// 表示準備
//---------------------
void CLogicDispForDev::readyForDisp( void ){
    // 無効は無視
    if( ! isValid() ){
        return;
    }
    
    // 列(タテ)と行(ヨコ)の取得
    int row = m_pGrid->getRow();
    int col = m_pGrid->getColumn();
    
    // ヒントの最大数：タテ
    int maxClueRow = 0;
    for( int i=0; i<row; i++ ){
        if( maxClueRow < m_pGrid->getClueNumForRow( i ) ){
            maxClueRow = m_pGrid->getClueNumForRow( i );
        }
    }
    
    // ヒントの最大数：ヨコ
    int maxClueCol = 0;
    for( int i=0; i<col; i++ ){
        if( maxClueCol < m_pGrid->getClueNumForColumn( i ) ){
            maxClueCol = m_pGrid->getClueNumForColumn( i );
        }
    }
    
    //----------------------------
    // 基本サイズ
    //----------------------------
    // ヒント数字
    m_fClueNumW1 = 6.0f;        // 横幅：一桁
    m_fClueNumW2 = 9.0f;        // 横幅：二桁
    m_fClueNumMarginX = 4.0f;   // 横の余白
    m_fClueNumH = 11.0f;        // 縦幅
    m_fClueNumMarginY = 0.0f;   // 縦の余白
    
    // ヒントの領域（タテ、ヨコのヒントの最大数分）
    m_fClueW = maxClueCol*(m_fClueNumW1+m_fClueNumMarginX) + 6.0f;
    m_fClueH = maxClueRow*(m_fClueNumH+m_fClueNumMarginY) + 6.0f;
    
    // マスの１大きさ
    m_fSolidW = 16.0f;
    m_fSolidH = 16.0f;
    
    // マスの領域
    m_fGridW = row * m_fSolidW;         // 盤面の大きさ：ヨコ
    m_fGridH = col * m_fSolidH;         // 盤面の大きさ：タテ
    m_fGridLineSize = 1.0f;             // ラインの太さ
    
    //----------------------------
    // スケール判定
    //----------------------------
    float checkScaleW = m_fOriginalW/(m_fGridW + m_fClueW);
    float checkScaleH = m_fOriginalH/(m_fGridH + m_fClueH);
    m_fGridScale = ( checkScaleW > checkScaleH )? checkScaleH: checkScaleW;
    
    // スケールの適用
    m_fClueNumW1 *= m_fGridScale;
    m_fClueNumW2 *= m_fGridScale;
    m_fClueNumMarginX *= m_fGridScale;
    m_fClueNumH *= m_fGridScale;
    m_fClueNumMarginY *= m_fGridScale;
    
    // ヒントの領域
    m_fClueW *= m_fGridScale;
    m_fClueH *= m_fGridScale;
    
    // マスの１単位
    m_fSolidW *= m_fGridScale;
    m_fSolidH *= m_fGridScale;
    
    // テクスチャサイズの調整（マスのサイズの解像度倍を２の階乗にして指定）
    int sizeBase = (int)(GM_P_RATE*m_fSolidW);
    int texSize = 16;
    while( texSize < sizeBase ){
        texSize *= 2;
    }
    m_fSolidTexW = texSize;
    m_fSolidTexH = texSize;
    
    // マスの領域
    m_fGridW *= m_fGridScale;
    m_fGridH *= m_fGridScale;
    //m_fGridLineSize *= m_fGridScale;      // 枠線はサイズ適用しない
    
    //----------------------------
    // 座標
    //----------------------------
    m_fClueX = m_fOriginalCenterX - (m_fGridW+m_fClueW)/2;
    m_fClueY = m_fOriginalCenterY - (m_fGridH+m_fClueH)/2;
    
    m_fGridX = m_fClueX + m_fClueW;
    m_fGridY = m_fClueY + m_fClueH;
    
    //----------------------
    // 領域確保
    //----------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_LOGIC );
    m_pTexLineForCross = new CTex();
    m_pTexColorForCross = new CTex();
    CMemMgr::PopTargetField();
    
    //----------------------
    // テクスチャ作成
    //----------------------
    createTexForCross();
}

//----------------------------
// 白マステクスチャの作成：[×]
//----------------------------
void CLogicDispForDev::createTexForCross( void ){
    // レイヤー確保
    CLayerData* pLayer = CLayerData::Alloc();
    
    // 線ワーク
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    
    eBRUSH brush = eBRUSH_CIRCLE_L;
    
    //-------------------------
    // オブジェクト確保＆設定： ＼
    //-------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLayer->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500+125, -2500+125, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500-125, 2500-125, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    //-------------------------
    // オブジェクト確保＆設定： ／
    //-------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLayer->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500-125, -2500+125, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500+125, 2500-125, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    //-----------------------------
    // テクスチャの描画
    //-----------------------------
    CBmpGenerator::SetBaseTexSize( m_fSolidTexW, m_fSolidTexH );

    stBMP_GEN_CREATE_PARAM createParam;
    CLEAR_BMP_GEN_CREATE_PARAM( &createParam );

    CList list;
    list.add( pLayer );
    CBmpGenerator::CreateTexWithLayerList( m_pTexLineForCross, m_pTexColorForCross, &list, &createParam, false );
    
    // レイヤー解放
    CLayerData::Free( pLayer );
}
