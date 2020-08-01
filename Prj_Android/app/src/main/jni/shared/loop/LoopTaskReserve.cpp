/*+----------------------------------------------------------------+
  |	Title:		LoopTaskReserve.hpp [共通環境]
  | Comment:    ループタスク予約：ヘルパー系の処理をタスクとして取り扱う
  |             （※マネージャで管理するほどではないシンプルな制御系の管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LoopTaskReserve.hpp"

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
//------------------------
// コンストラクタ
//------------------------
CLoopTaskReserve::CLoopTaskReserve( void ){
	clear();
}

//------------------------
// クリア
//------------------------
void CLoopTaskReserve::clear( void ){
	m_eTask = eLOOP_TASK_RESERVE_INVALID;
	m_pLoop = NULL;
	m_nTaskId = 0;

    m_nNumParam = 0;
    m_nBufUse = 0;
}

//------------------------
// 設定（※通常ダイアログ）
//------------------------
bool CLoopTaskReserve::setDialog( CLoop*pLoop, int taskId, const char*pTitle, const char*pText, const char*pPositive, const char*pNegative ){
	m_eTask = eLOOP_TASK_RESERVE_DIALOG;
	m_pLoop = pLoop;
	m_nTaskId = taskId;
    
    // パラメータの追加
    if( ! addParam( pTitle ) ){ return( false ); }
    if( ! addParam( pText ) ){ return( false ); }
    if( ! addParam( pPositive ) ){ return( false ); }
    if( ! addParam( pNegative ) ){ return( false ); }

    return( true );
}

//-------------------------
// 設定（※入力ダイアログ）
//-------------------------
bool CLoopTaskReserve::setDialogInput( CLoop*pLoop, int taskId, const char*pTitle, const char*pText, const char*pPositive, const char*pNegative, const char*pDefault ){
	m_eTask = eLOOP_TASK_RESERVE_DIALOG_INPUT;
	m_pLoop = pLoop;
	m_nTaskId = taskId;

    // パラメータの追加
    if( ! addParam( pTitle ) ){ return( false ); }
    if( ! addParam( pText ) ){ return( false ); }
    if( ! addParam( pPositive ) ){ return( false ); }
    if( ! addParam( pNegative ) ){ return( false ); }
    if( ! addParam( pDefault ) ){ return( false ); }

    return( true );
}

//------------------------
// 有効性判定
//------------------------
bool CLoopTaskReserve::isValid( void ){
	return( m_eTask != eLOOP_TASK_RESERVE_INVALID );
}

//------------------------
// パラメータ取得
//------------------------
const char* CLoopTaskReserve::getParamAt( int at ){
    // 無効は無視
    if( at < 0 || m_nNumParam <= at ){
        LOGE( "@ CLoopTaskReserve::getParamAt: INVALID PARAM: at=%d/%d\n", at, m_nNumParam );
        return( "" );
    }
    
    return( m_arrParam[at] );
}

//------------------------
// パラメータの追加
//------------------------
bool CLoopTaskReserve::addParam( const char* pParam ){
    if( pParam == NULL ){
        pParam = "";
    }
    
    // バッファサイズ確認
    int len = m_nBufUse + util_str_len( pParam );
    if( len >= LOOP_TASK_PARAM_BUF_LEN ){
        LOGE( "@ CLoopTaskReserve::addParam: BUF SHORTAGE: len=%d/%d, pParam=%s\n", len, LOOP_TASK_PARAM_BUF_LEN, pParam );
        return( false );
    }
    
    // パラメータ枠確認
    if( m_nNumParam >= LOOP_TASK_PARAM_MAX ){
        LOGE( "@ CLoopTaskReserve::addParam: PARAM SHORTAGE: num=%d/%d, pParam=%s\n", m_nNumParam, LOOP_TASK_PARAM_MAX, pParam );
        return( false );
    }

    // 文字列の出力
    sprintf( &m_bufParam[m_nBufUse], "%s", pParam );
    
    // 参照先の保持＆パラメータインクリメント
    m_arrParam[m_nNumParam++] = &m_bufParam[m_nBufUse];

    // バッファ使用量の更新
    m_nBufUse = len + 1; // 終端文字分＋１
    
    return( true );
}
