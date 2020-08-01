/*+----------------------------------------------------------------+
  |	Title:		BgMgr.cpp [共通環境]
  |	Comment:	背景管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BgMgr.hpp"

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
eBG   CBgMgr::s_eNextBgForNormal;
eBG   CBgMgr::s_eCurBgForNormal;
CObj* CBgMgr::s_pObjBackForNormal;
CObj* CBgMgr::s_pObjFrontForNormal;

eBG   CBgMgr::s_eNextBgForExtra;
eBG   CBgMgr::s_eCurBgForExtra;
CObj* CBgMgr::s_pObjBackForExtra;
CObj* CBgMgr::s_pObjFrontForExtra;

eBG*   CBgMgr::s_pNextBg;
eBG*   CBgMgr::s_pCurBg;
CObj** CBgMgr::s_ppCurBufBack;
CObj** CBgMgr::s_ppCurBufFront;

float CBgMgr::s_fQuakeX;
float CBgMgr::s_fQuakeY;

BYTE CBgMgr::s_nAlphaForFront;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CBgMgr::OnAppStart( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CBgMgr::OnAppStart()\n" );
#endif

	s_eNextBgForNormal = eBG_GRID;
	s_eCurBgForNormal = eBG_INVALID;
	s_pObjBackForNormal = NULL;
	s_pObjFrontForNormal = NULL;

	s_eNextBgForExtra = eBG_GRID;
	s_eCurBgForExtra = eBG_INVALID;
	s_pObjBackForExtra = NULL;
	s_pObjFrontForExtra = NULL;

    s_nAlphaForFront = 0xFF;

	// 通常に設定
	SwitchBufForNormal();

	return( true );
}

//------------------------
// 終了
//------------------------
void CBgMgr::OnAppEnd( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CBgMgr::OnAppEnd()\n" );
#endif

	// 解放
	SAFE_DELETE( s_pObjBackForNormal );
	SAFE_DELETE( s_pObjFrontForNormal );
	SAFE_DELETE( s_pObjBackForExtra );
	SAFE_DELETE( s_pObjFrontForExtra );
}

//--------------------------
// 切り替え
//--------------------------
// Normal
void CBgMgr::SwitchBufForNormal( void ){
	s_pNextBg = &s_eNextBgForNormal;
	s_pCurBg = &s_eCurBgForNormal;
	s_ppCurBufBack = &s_pObjBackForNormal;
	s_ppCurBufFront = &s_pObjFrontForNormal;
}

// Extra
void CBgMgr::SwitchBufForExtra( void ){
	s_pNextBg = &s_eNextBgForExtra;
	s_pCurBg = &s_eCurBgForExtra;
	s_ppCurBufBack = &s_pObjBackForExtra;
	s_ppCurBufFront = &s_pObjFrontForExtra;
}

//----------------------
// Extra判定
//----------------------
bool CBgMgr::IsCurBuffForExtra( void ){
	return( s_ppCurBufBack == &s_pObjBackForExtra );
}

//--------------------------
// 背景を次に（デバッグ用）
//--------------------------
void CBgMgr::SetNextBg( void ){
	int nextBg = (*s_pCurBg) + 1;
	if( nextBg >= eBG_MAX ){
		nextBg = 0;
	}
	(*s_pNextBg) = (eBG) nextBg;
}

//----------------------------------
// 個別指定：グリッド背景のタイプ指定
//----------------------------------
void CBgMgr::SetBgGridType( eBG_GRID_TYPE type ){
    if( *s_pCurBg == eBG_GRID ){
        ((CBgGridB*)(*s_ppCurBufBack))->setGridType( type );
    }
}

//--------------------------
// 更新
//--------------------------
void CBgMgr::Update( void ){
	// 背景が切り替わっていたら設定
	if( (*s_pCurBg) != (*s_pNextBg) ){
		FixForChange();
	}
}

//--------------------------
// 描画登録
//--------------------------
void CBgMgr::RegistForDraw( void ){
	// 登録
	if( (*s_ppCurBufFront) != NULL ){ (*s_ppCurBufFront)->registForDraw(); }
	if( (*s_ppCurBufBack) != NULL ){ (*s_ppCurBufBack)->registForDraw(); }
}

//--------------------------
// 背景切り替えの確定
//--------------------------
void CBgMgr::FixForChange( void ){
    // 現在の遠景＆近景を解放
    SAFE_DELETE( (*s_ppCurBufBack) );
    SAFE_DELETE( (*s_ppCurBufFront) );

    // 指定が有効なら背景の作成
    if( (*s_pNextBg) != eBG_INVALID ){
        CMemMgr::PushTargetField( eMEM_FIELD_D_BG );
        switch( (*s_pNextBg) ){
        case eBG_GRID:
            (*s_ppCurBufBack) = new CBgGridB();
            (*s_ppCurBufFront) = new CBgGridF();
            break;
                
        default:
            break;
        }
        CMemMgr::PopTargetField();

        (*s_pCurBg) = (*s_pNextBg);

        // ブランク解除
        if( (*s_ppCurBufBack) != NULL ){ (*s_ppCurBufBack)->setBlank( false ); }
        if( (*s_ppCurBufFront) != NULL ){ (*s_ppCurBufFront)->setBlank( false ); }
    }
}
