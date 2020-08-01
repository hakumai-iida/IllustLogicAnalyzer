/*+----------------------------------------------------------------+
  |	Title:		TouchMgr.cpp [共通環境]
  |	Comment:	タッチ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ステータス管理数（※同時に検出できる指の本数）
#define MAX_STATUS	 32

// 情報管理数（※同時に検出できる状況検出数）
#define MAX_INFO	256

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
int CTouchMgr::s_nNumInfo;
CTouchStatusCell* CTouchMgr::s_oArrStatusCell;
CTouchInfoCell* CTouchMgr::s_oArrInfoCell;

bool CTouchMgr::s_bIgnoreDuplicated;

bool CTouchMgr::s_bBackButtonDown;
bool CTouchMgr::s_bBackButtonUp;
bool CTouchMgr::s_bBackButtonDown0;
bool CTouchMgr::s_bBackButtonUp0;

bool CTouchMgr::s_bTouchEffect;
bool CTouchMgr::s_bTouchEffectSkip;
bool CTouchMgr::s_bTouchEffectSe;
bool CTouchMgr::s_bTouchEffectSeSkip;
int  CTouchMgr::s_nTouchEffectNum;
int* CTouchMgr::s_nArrTouchEffectPosX;
int* CTouchMgr::s_nArrTouchEffectPosY;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CTouchMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTouchMgr::OnCreate()\n" );
#endif

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_BASE );
	s_oArrStatusCell = new CTouchStatusCell[MAX_STATUS];
	s_oArrInfoCell = new CTouchInfoCell[MAX_INFO];
	s_nArrTouchEffectPosX = new int[MAX_STATUS];
	s_nArrTouchEffectPosY = new int[MAX_STATUS];
	CMemMgr::PopTargetField();

	return( true );
}

//------------------------
// 終了
//------------------------
void CTouchMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CTouchMgr::OnDestroy()\n" );
#endif

	// 解放
	SAFE_DELETE_ARRAY( s_oArrStatusCell );
	SAFE_DELETE_ARRAY( s_oArrInfoCell );
	SAFE_DELETE_ARRAY( s_nArrTouchEffectPosX );
	SAFE_DELETE_ARRAY( s_nArrTouchEffectPosY );
}

//--------------------
// 更新
//--------------------
void CTouchMgr::Update( void ){
	// ログの確認
	for( int i=0; i<s_nNumInfo; i++ ){
		CTouchInfoCell* pInfo = &s_oArrInfoCell[i];

		int action = pInfo->getAction();
		int length = pInfo->getLength();

#ifdef EXE_ANDROID
        // [ANDROID] ＩＤが付与されたアクションであれば調整
		if( action > AMOTION_EVENT_ACTION_POINTER_DOWN ){
			int act = action & AMOTION_EVENT_ACTION_MASK;
			int id = action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

			for( int j=0; j<length; j++ ){
				if( id == j ){	// [INDEX_SHIFT]により指定されるのは配列に対するID
					CheckLog( pInfo->getIdAt(j), act, pInfo->getXAt(j), pInfo->getYAt(j) );
				}else{
					CheckLog( pInfo->getIdAt(j), AMOTION_EVENT_ACTION_MOVE, pInfo->getXAt(j), pInfo->getYAt(j) );
				}
			}
		}
		else{
			for( int j=0; j<length; j++ ){
				CheckLog( pInfo->getIdAt(j), action, pInfo->getXAt(j), pInfo->getYAt(j) );
			}
		}
#endif
#ifdef EXE_IOS
        // [IOS] そのまま渡す
        for( int j=0; j<length; j++ ){
            CheckLog( pInfo->getIdAt(j), action, pInfo->getXAt(j), pInfo->getYAt(j) );
        }
#endif
    }

    // ログを無効化
    s_nNumInfo = 0;

	// タッチ更新
	for( int i=0; i<MAX_STATUS; i++ ){
		s_oArrStatusCell[i].update();
	}

	// [BACK]ボタン更新
	s_bBackButtonDown = s_bBackButtonDown0;
	s_bBackButtonUp = s_bBackButtonUp0;
	s_bBackButtonDown0 = false;
	s_bBackButtonUp0 = false;

	//-----------------------------
	// タッチ効果関連
	//-----------------------------
	// スキップフラグは都度クリア
	s_bTouchEffectSkip = false;
	s_bTouchEffectSeSkip = false;

	// 押下判定リスト作成
	s_nTouchEffectNum = 0;
	for( int i=0; i<MAX_STATUS; i++ ){
		if( s_oArrStatusCell[i].isTouchedDown() ){
			s_nArrTouchEffectPosX[s_nTouchEffectNum] = s_oArrStatusCell[i].getBaseX();
			s_nArrTouchEffectPosY[s_nTouchEffectNum] = s_oArrStatusCell[i].getBaseY();
			s_nTouchEffectNum++;
		}
	}
}

//------------------------------
// ゲーム画面用の更新
//------------------------------
void CTouchMgr::UpdateForGame( void ){
	// 無視フラグが立っていたら無視
	if( s_bTouchEffectSkip ){
		return;
	}
}

//-------------------------------------------------
// ステータスの全クリア
//-------------------------------------------------
void CTouchMgr::ClearStatusAll( void ){
    LOGD( "@ ClearStatusAll!!\n" );

    for( int i=0; i<MAX_STATUS; i++ ){
        s_oArrStatusCell[i].clear();
    }

    s_nNumInfo = 0;
    s_bBackButtonDown = s_bBackButtonDown0 = false;
    s_bBackButtonUp = s_bBackButtonUp0 = false;
}

//-------------------------------------------------
// ログの追加
//-------------------------------------------------
bool CTouchMgr::PushLog( stTOUCH_EVENT_DESCRIPTOR* pTED ){
    //CTouchInfoCell::DumpLog( pTED );

#ifdef EXE_ANDROID
    // [ANDROID] 入力イベントはタッチイベント以外も含まれる点に注意
    AInputEvent* pEvent = pTED->pInputEvent;

    int type = AInputEvent_getType( pEvent );

#ifdef CHECK_BACK_BUTTON
    // [ANDROID] [BACK]ボタンの検出
    if( type == AINPUT_EVENT_TYPE_KEY ){
        int code = AKeyEvent_getKeyCode( pEvent );
        if( code == AKEYCODE_BACK ){
            int action = AKeyEvent_getAction( pEvent );

            // フラグで管理
            if( action == AKEY_EVENT_ACTION_DOWN ){
                s_bBackButtonDown0 = true;
            }else if( action == AKEY_EVENT_ACTION_UP ){
                s_bBackButtonUp0 = true;
            }

            return( true );
        }
    }
#endif

    // [ANDROID] タッチイベントでなければ無視
    if( type != AINPUT_EVENT_TYPE_MOTION ){
        return( false );
    }
#endif

    if( s_nNumInfo < MAX_INFO ){
        s_oArrInfoCell[s_nNumInfo++].setMotionEvent( pTED );
    }else{
        // とりあえず無視しておく（※何カ所もタッチされているメチャクチャな状況のはず）
        LOGE( "@ CTouchMgr::PushLog: s_oArrInfoCell SHORTAGE\n" );
    }
    return( true );
}

//------------------------------
// どこかがタッチされたか？
//------------------------------
bool CTouchMgr::IsTouchedAnywhere( void ){
    // 無視状態であれば無視
    if( s_bIgnoreDuplicated  ){
        return( false );
    }

    return( s_nTouchEffectNum > 0 );
}

//--------------
// 確認：矩形
//--------------
CTouchStatusCell* CTouchMgr::CheckRect( int check, float x, float y, float w, float h, int at ){
	// 無視状態であれば無視
	if( s_bIgnoreDuplicated  ){
		return( NULL );
	}

	// 更新
	for( int i=0; i<MAX_STATUS; i++ ){
		CTouchStatusCell* pStat = &s_oArrStatusCell[i];
		CTouchStatusCell* pHit = NULL;

		// 活動していなければ無視
		if( !pStat->isActive() ){
			continue;
		}

		// アクションの確認
		switch( check ){
        // 押されている：現ポイントで判定
		case eTOUCH_CHECK_PRESSED:
			if( pStat->isCurPointInRect( x, y, w, h) ){
				if( pStat->isPressed() ){ pHit = pStat; }
			}
			break;

        // 移動：開始ポイントで判定
		case eTOUCH_CHECK_MOVED:
			if( pStat->isStartPointInRect( x, y, w, h) ){
				if( pStat->isMoved() ){ pHit = pStat; }
			}
			break;

        // 押下：現ポイントで判定
		case eTOUCH_CHECK_TOUCHED_DOWN:
			if( pStat->isCurPointInRect( x, y, w, h) ){
				if( pStat->isTouchedDown() ){ pHit = pStat; }
			}
			break;

        // 押上：現ポイントで判定
		case eTOUCH_CHECK_TOUCHED_UP:
			if( pStat->isCurPointInRect( x, y, w, h) ){
				if( pStat->isTouchedUp() ){ pHit = pStat; }
			}
			break;

        // タップ：開始ポイントで判定
		case eTOUCH_CHECK_TAPPED:
			if( pStat->isStartPointInRect( x, y, w, h) ){
				if( pStat->isTapped() ){ pHit = pStat; }
			}
			break;

        // フリック：開始ポイントで判定
		case eTOUCH_CHECK_FLICKED:
			if( pStat->isStartPointInRect( x, y, w, h) ){
				if( pStat->isFlicked() ){ pHit = pStat; }
			}
			break;
		}

		if( pHit != NULL ){
			if( at <= 0 ){
				return( pHit );
			}
			at--;
		}
	}

	return( NULL );
}

//--------------
// 確認：矩形
//--------------
CTouchStatusCell* CTouchMgr::CheckOval( int check, float x, float y, float w, float h, int at ){
	// 無視状態であれば無視
	if( s_bIgnoreDuplicated  ){
		return( NULL );
	}

	// 更新
	for( int i=0; i<MAX_STATUS; i++ ){
		CTouchStatusCell* pStat = &s_oArrStatusCell[i];
		CTouchStatusCell* pHit = NULL;

		// 活動していなければ無視
		if( !pStat->isActive() ){
			continue;
		}

		// アクションの確認
		switch( check ){
        // 押されている：現ポイントで判定
		case eTOUCH_CHECK_PRESSED:
			if( pStat->isCurPointInOval( x, y, w, h ) ){
				if( pStat->isPressed() ){ pHit = pStat; }
			}
			break;
                
        // 移動：開始ポイントで判定
		case eTOUCH_CHECK_MOVED:
			if( pStat->isStartPointInOval( x, y, w, h ) ){
				if( pStat->isMoved() ){ pHit = pStat; }
			}
			break;

        // 押下：現ポイントで判定
		case eTOUCH_CHECK_TOUCHED_DOWN:
			if( pStat->isCurPointInOval( x, y, w, h ) ){
				if( pStat->isTouchedDown() ){ pHit = pStat; }
			}
			break;

        // 押上：ポイントで判定
		case eTOUCH_CHECK_TOUCHED_UP:
			if( pStat->isCurPointInOval( x, y, w, h ) ){
				if( pStat->isTouchedUp() ){ pHit = pStat; }
			}
			break;

        // タップ：開始ポイントで判定
		case eTOUCH_CHECK_TAPPED:
			if( pStat->isStartPointInOval( x, y, w, h ) ){
				if( pStat->isTapped() ){ pHit = pStat; }
			}
			break;

        // フリック：開始ポイントで判定
		case eTOUCH_CHECK_FLICKED:
			if( pStat->isStartPointInOval( x, y, w, h ) ){
				if( pStat->isFlicked() ){ pHit = pStat; }
			}
			break;
		}

        // 検出要素が対象であるなら返す
		if( pHit != NULL ){
			if( at <= 0 ){
				return( pHit );
			}
			at--;
		}
	}

	return( NULL );
}

//-------------------------------------------------
// ピンチ（※２点が押されているか？）
//-------------------------------------------------
bool CTouchMgr::IsPinched( float x, float y, float w, float h, float* pLen ){
    CTouchStatusCell* pStat0 = NULL;
    CTouchStatusCell* pStat1 = NULL;

    for( int i=0; i<MAX_STATUS; i++ ){
        CTouchStatusCell* pStat = &s_oArrStatusCell[i];
        
        // 活動していなければ無視
        if( !pStat->isActive() ){
            continue;
        }
        
        // 指定位置から開始している＆押下中か？
        if( pStat->isStartPointInOval( x, y, w, h ) && pStat->isPressed() ){
            if( pStat0 == NULL ){
                pStat0 = pStat;
            }else{
                pStat1 = pStat;
                break;
            }
        }
    }

    // 二点が有効なら
    if( pStat0 != NULL && pStat1 != NULL ){
        if( pLen != NULL ){
            *pLen = CMath::Dist( pStat0->getX(), pStat0->getY(), pStat1->getX(), pStat1->getY());
        }
        
        return( true );
    }
    
    if( pLen != NULL ){
        *pLen = 0.0f;
    }
    return( false );
}

//------------------------------------------
// IDによるセルの取得（※入力の継続検出用）
//------------------------------------------
CTouchStatusCell* CTouchMgr::GetCellById( int id ){
    for( int i=0; i<MAX_STATUS; i++ ){
        CTouchStatusCell* pStat = &s_oArrStatusCell[i];
        
        // 活動していなければ無視
        if( !pStat->isActive() ){
            continue;
        }

        // IDが一致したら返す
        if( pStat->getId() == id ){
            return( pStat );
        }
    }

    // ここまできたらミス
    return( NULL );
}

//-------------------------------------------------
// バックボタン検出
//-------------------------------------------------
// バックボタンが押下されたか？
bool CTouchMgr::CheckBackButtonDown( void ){
    if( ! s_bIgnoreDuplicated ){
        return( s_bBackButtonDown );
    }
    return( false );
}

// バックボタンが押上されたか？
bool CTouchMgr::CheckBackButtonUp( void ){
	if( ! s_bIgnoreDuplicated  ){
		return( s_bBackButtonUp );
	}
	return( false );
}

// バックボタンが入力されたか？
bool CTouchMgr::CheckBackButton( void ){
	if( ! s_bIgnoreDuplicated ){
		return( s_bBackButtonUp );
	}
	return( false );
}

//-----------
// ログの処理
//-----------
void CTouchMgr::CheckLog( int id, int act, float x, float y ){
    //LOGD( "@ CheckLog: id=%d, act=%d, x0=%f, y0=%f\n", id, act, x, y );
   
    // デバイスピクセル率の適用
    x *= CMainRenderer::GetDisplayDevicePixelRate();
    y *= CMainRenderer::GetDisplayDevicePixelRate();
    
    // ピクセル率で割ってゲーム座標へ
    x /= GM_P_RATE;
    y /= GM_P_RATE;

    // セーフエリアの左上を原点とする
    x -= SA_OFS_X;
    y -= SA_OFS_Y;
    
    //LOGD( "@ x=%d, y=%d\n", x, y );

#ifdef EXE_ANDROID
    // [ANDOID] [id]はそのまま配列のインデックスとなる
    if( id < 0 || id >= MAX_STATUS ){
        return;
    }

    CTouchStatusCell* pStat = &s_oArrStatusCell[id];
    pStat->setId( id );

    // 割り振り
    switch( act ){
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            pStat->onTouchedDown( x, y );
            break;

        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_OUTSIDE:	// とりあえず
        case AMOTION_EVENT_ACTION_CANCEL:	// とりあえず
            pStat->onTouchedUp( x, y );
            break;

        case AMOTION_EVENT_ACTION_MOVE:
            pStat->onMoved( x, y );
            break;
    }
#endif

#ifdef EXE_IOS
    // [IOS] 登録済みのIDを探す
    int target = -1;
    for( int i=0; i<MAX_STATUS; i++ ){
        if( s_oArrStatusCell[i].getId() == id ){
            target = i;
            break;
        }
    }

    // [IOS] 登録済みがいなければ空きを探す
    if( target == -1 ){
        for( int i=0; i<MAX_STATUS; i++ ){
            if( s_oArrStatusCell[i].getId() < 0 ){
                target = i;
                break;
            }
        }
    }

    // [IOS] 見つからなければ無視
    if( target == -1 ){
        return;
    }

    CTouchStatusCell* pStat = &s_oArrStatusCell[target];
    pStat->setId( id );

    // [IOS] 割り振り
    switch( act ){
    case UITouchPhaseBegan:
        //LOGD( "@  BEGAN[%d]: ADJ_X=%f, ADJ_Y=%f\n", target, x, y );
        pStat->onTouchedDown( x, y );
        break;

    case UITouchPhaseEnded:
    case UITouchPhaseCancelled:
        //LOGD( "@  TOUCHED UP[%d]: ADJ_X=%f, ADJ_Y=%f\n", target, x, y );
        pStat->onTouchedUp( x, y );
        break;

    case UITouchPhaseMoved:
        //LOGD( "@  MOVED[%d]: ADJ_X=%f, ADJ_Y=%f\n", target, x, y );
        pStat->onMoved( x, y );
        break;
    }
#endif
}
