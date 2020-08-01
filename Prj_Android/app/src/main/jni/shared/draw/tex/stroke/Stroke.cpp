/*+----------------------------------------------------------------+
  | Title:      Stroke.hpp [共通環境]
  | Comment:    ストローク（※アンカーポイントを結ぶ線の見た目部分）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Stroke.hpp"
#include "StrokeDot.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
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
//-------------------
// フックターゲット調整
//-------------------
eSTROKE_HOOK_TARGET CStroke::AdjustHookTargetForSlotIndex( eSTROKE_HOOK_TARGET target, int slotIndex ){
    // 対象が有効
    if( IS_STROKE_HOOK_TARGET_VALID( target ) ){
        // 対象がテンポラリ枠であればそのまま返す
        if( target >= eSTROKE_HOOK_TARGET_TEMP_START && target <= eSTROKE_HOOK_TARGET_TEMP_END ){
            return( target );
        }
        
        // スロットインデックスが有効（※左右の概念のある要素の右側[1]からアクセスされた）
        // 基本的に同一要素からのみのアクセスを想定（※脚が腕をアクセスしたり、逆サイドの要素にアクセスすることはない前提）
        if( slotIndex > 0 ){
            // 顔＆頭
            if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_HEAD && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_HEAD ){
                // 基準値へ戻してからスロット補正
                target = (eSTROKE_HOOK_TARGET)(eSTROKE_HOOK_TARGET_CHECK_START_FOR_HEAD + (2*(target-eSTROKE_HOOK_TARGET_CHECK_START_FOR_HEAD)/2));
                target = (eSTROKE_HOOK_TARGET)(target+1);   // 右枠しかないので＋１
            }
            // 特殊
            else if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_ETC && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_ETC ){
                // 基準値へ戻してからスロット補正
                target = (eSTROKE_HOOK_TARGET)(eSTROKE_HOOK_TARGET_CHECK_START_FOR_ETC + (2*(target-eSTROKE_HOOK_TARGET_CHECK_START_FOR_ETC)/2));
                target = (eSTROKE_HOOK_TARGET)(target+1);   // 右枠しかないので＋１
            }
            // 腕
            else if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_ARM ){
                // 基準値へ戻してからスロット補正
                target = (eSTROKE_HOOK_TARGET)(eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM + (2*(target-eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM)/2));
                target = (eSTROKE_HOOK_TARGET)(target+1);   // 右枠しかないので＋１
            }
            // 脚
            else if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_LEG ){
                // 基準値へ戻してからスロット補正
                target = (eSTROKE_HOOK_TARGET)(eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG + (2*(target-eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG)/2));
                target = (eSTROKE_HOOK_TARGET)(target+1);   // 右枠しかないので＋１
            }
        }
    }
    
    return( target );
}

//-------------------
// タッチターゲット調整
//-------------------
eSTROKE_TOUCH_TARGET CStroke::AdjustTouchTargetForSlotIndex( eSTROKE_TOUCH_TARGET target, int slotIndex ){
    // 対象が有効
    if( target >= 0 && target < eSTROKE_TOUCH_TARGET_MAX ){
        // 対象がテンポラリ枠でない
        if( target < eSTROKE_TOUCH_TARGET_TEMP_START || target > eSTROKE_TOUCH_TARGET_TEMP_END ){
            
            // スロットインデックスが右であれば
            if( slotIndex > 0 ){
                // 対象をインデックスで補正する
                target = (eSTROKE_TOUCH_TARGET)(target + STROKE_TOUCH_TARGET_OFS_FOR_SLOT_INDEX*slotIndex);
            }
        }
    }
    
    return( target );
}

//-------------------
// コンストラクタ
//-------------------
CStroke::CStroke( void ){
    clear();
}

//-------------------
// クリア
//-------------------
void CStroke::clear( void ){
    m_eType = eSTROKE_TYPE_INVALID;
    m_fSize = 0.0f;
    m_fOfsRange = 0.0f;
}

//-------------------
// ストローク設定
//-------------------
bool CStroke::set( eSTROKE_TYPE type, float size, float range ){
    // 一旦クリア
    clear();

    // 無効は無視
    if( type < 0 || type >= eSTROKE_TYPE_MAX ){
        LOGE( "@ CStroke::set: FAILED: INVALID TYPE: type=%d, size=%f, range=%f\n", type, size, range );
        return( false );
    }

    // サイズが０以下の場合は無効
    if( size <= 0.0f ){
        LOGE( "@ CStroke::set: FAILED: TOO SMALL SIZE: type=%d, size=%f, range=%f\n", type, size, range );
        return( false );
    }

    // サイズが大きすぎる場合は制限
    if( size > STROKE_DOT_ALLOC_SIZE_MAX ){
        LOGE( "@ CStroke::set: FAILED: TOO HUGE SIZE: type=%d, size=%f, range=%f\n", type, size, range );
        size = STROKE_DOT_ALLOC_SIZE_MAX;
    }

    // 用心（※レンジが負なら反転しておく）
    if( range < 0.0f ){
        range *= -1.0f;
    }

    // 設定を保持して完了
    m_eType = type;
    m_fSize = size;
    m_fOfsRange = range;
    
    return( true );
}

//-----------------------------------------------------------------
// ストロークの出力セルの取得
//-----------------------------------------------------------------
stSTROKE_OUTPUT_CELL* CStroke::getStrokeCell( float rateOutX, float rateOutY, float scale ){
    CStrokeDot* pStrokeDot = NULL;

    if( m_eType == eSTROKE_TYPE_SQUARE ){
        pStrokeDot = CStrokeMgr::GetStrokeDotForSquare( scale*m_fSize );
    }else if ( m_eType == eSTROKE_TYPE_CIRCLE ){
        pStrokeDot = CStrokeMgr::GetStrokeDotForCircle( scale*m_fSize );
    }

    if( pStrokeDot == NULL ){
        return( NULL );
    }

    // レンジが有効ならブレさせる
    if( m_fOfsRange > 0.0f ){
        // Rangeの値で半径を設定して適当に回転させる
        float r = CRand::GetRandF( m_fOfsRange );
        float deg = CRand::GetRandF( 360.0f );
        float oX = r * CMath::GetCosLite( deg ) + rateOutX;
        float oY = r * CMath::GetSinLite( deg ) + rateOutY;
        
        // 整数分のブレはオフセットでずらす
        m_stOutputCell.ofsX = (int)oX;
        m_stOutputCell.ofsY = (int)oY;
        
        // 負の端数の調整
        float outX = oX - m_stOutputCell.ofsX;
        float outY = oY - m_stOutputCell.ofsY;
        if( outX < 0.0f ){
            m_stOutputCell.ofsX -= 1;
            outX += 1.0f;
        }
        if( outY < 0.0f ){
            m_stOutputCell.ofsY -= 1;
            outY += 1.0f;
        }

        // バッファとサイズ設定
        m_stOutputCell.pBuf = pStrokeDot->getDot( outX, outY );
        m_stOutputCell.sizeW = m_stOutputCell.sizeH = pStrokeDot->getSizeDot();
    }
    // レンジの指定がなければそのまま
    else{
        // バッファとサイズ設定
        m_stOutputCell.pBuf = pStrokeDot->getDot( rateOutX, rateOutY );
        m_stOutputCell.sizeW = m_stOutputCell.sizeH = pStrokeDot->getSizeDot();
        
        // オフセットは不要（ブレないので）
        m_stOutputCell.ofsX = m_stOutputCell.ofsY = 0;
    }

    return( &m_stOutputCell );
}
