/*+----------------------------------------------------------------+
  |	Title:		MemMgr.hpp　[共通環境]
  |	Comment:	メモリ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __MEM_MGR_HPP__
#define __MEM_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "mcell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// eMEM_FIELD, eMEM_FIELD_SHARE は [config_app.hpp]で定義される

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//------------------------------------------------
// フィールド管理構造体
//------------------------------------------------
struct stMEM_FIELD{
	void*			pAddr;			// 先頭アドレス
	uint32			size;			// サイズ
	bool			isStatic;		// 静的か？

	// 管理データ
	stMCELL_FIELD	field;			// MCELLフィールド
	uint32			sizePool;		// セルプールサイズ
	void*			pCellPool;		// セルプール領域
};

//------------------------------------------------
// 共有フィールド管理構造体
//------------------------------------------------
struct stMEM_FIELD_SHARE{
	uint32	ofs;	// 共有メモリ内のオフセット
	uint32	size;	// 必要サイズ
};

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// メモリ管理クラス
//----------------------------------------
class CMemMgr{
private:
	// マネージャに割り当てられたメモリ（※初期化時に設定）
	static void*	s_pAddr;		// アドレス
	static uint32	s_nSize;		// サイズ
	static uint32	s_nUsed;		// 使用済サイズ
	static int32	s_nCurField;	// 確保するフィールド
	static bool		s_bAllocEnable;	// 確保可能か？

	// フィールド管理
	static stMEM_FIELD			s_stArrField[eMEM_FIELD_MAX];

	// 共有フィールド管理
	static stMEM_FIELD_SHARE	s_stArrFieldShare[eMEM_FIELD_SHARE_MAX];

	// テンポラリ文字列を使いまわす細工
	static uint32	s_nTempStrAt;

	// 確保先プッシュ／ポップ管理用
	static uint32	s_nFieldStackUse;
	static int32*	s_nArrFieldStack;

public:
	// 初期化／終了
	static bool Init( void );
	static void Exit( void );

	//------------------------------------------------------------
	// グローバル[new]/[delete]制御用
	//------------------------------------------------------------
	inline static void SetAllocEnable( bool flag ){ s_bAllocEnable = flag; }
#ifdef EXE_IOS
	// [iOS] 64bit環境で動作が不安定のため[FREE]領域に対する確保/解放は管理しない
	inline static bool IsAllocEnable( void ){ return( s_bAllocEnable && s_nCurField != eMEM_FIELD_D_FREE ); }
#else
	// [その他]
	inline static bool IsAllocEnable( void ){ return( s_bAllocEnable ); }
#endif

	// アドレス判定（指定アドレスがマネージャ管理下にあるか？）
	inline static bool IsManaged( void* pAddr ){
		ADDR target = (ADDR) pAddr;
		ADDR start = (ADDR) s_pAddr;
		ADDR end = (ADDR) s_pAddr + s_nSize;
		return( target >= start && target < end );
	}

	//---------------------------------------------------------------
	// 確保先設定
	//（※管理メモリを確保する場合は必ず[Push]し、終了したら[Pop]すること）
	//---------------------------------------------------------------
	static void PushTargetField( eMEM_FIELD fieldId );
	static void PopTargetField( void );

	//---------------------------------------------------------------
	// メモリの取得（※フィールドの領域を全て使う場合）
	//---------------------------------------------------------------
	static void* GetAddr( eMEM_FIELD fieldId );		// アドレス取得
	static uint32 GetSize( eMEM_FIELD fieldId );	// サイズ取得

	//---------------------------------------------------------------
	// 共有メモリの取得
	//---------------------------------------------------------------
	static void* GetAddrFromShare( eMEM_FIELD_SHARE fieldShareId );		// アドレス取得
	static uint32 GetSizeFromShare( eMEM_FIELD_SHARE fieldShareId );	// サイズ取得

	// 一時文字列領域(※取得したその場で使う場合にのみ領域が破壊されないことを保証する)
	static char* GetBufFromTempStr( int32 target=-1 );
	static uint32 GetSizeFromTempStr( void );

	//---------------------------------------------------------------
	// 動的メモリの確保／解放（※領域を細切れで使う場合）
	//---------------------------------------------------------------
	// 確保（※実際にはマネージャ経由ではなく[new],[MALLOC]として利用される：[util.h]）
	static void* Alloc( uint32 size );

	// 解放（※実際にはマネージャ経由ではなく[SAFE_FREE]等として利用される：[util.h]）
	static bool Free( void* pAddr );

	// アライン設定
	static void SetFieldAlignment( eMEM_FIELD fieldId, uint32 alignment );

	static void ReportField( eMEM_FIELD fieldId, bool isDetail=false );		// 指定フィールドのログを出力する
	static void DrawLog( void );	// ログ表示

	// フィールド情報の取得
	static const stMEM_FIELD* GetFieldInfo( eMEM_FIELD fieldId );

private:
	// フィールド関連
	static stMEM_FIELD* get_field( eMEM_FIELD fieldId ){
		if( (int)fieldId < 0 || fieldId >= eMEM_FIELD_MAX ){ return( NULL ); }
		return( &s_stArrField[fieldId] );
	}
	static bool set_field( eMEM_FIELD fieldId, uint32 size, bool isStatic, uint32 numCell, const char* pName, bool isDumpLog );

	// 共有フィールド関連
	static stMEM_FIELD_SHARE* get_field_share( eMEM_FIELD_SHARE fieldShareId ){
		if( (int)fieldShareId < 0 || fieldShareId >= eMEM_FIELD_SHARE_MAX ){ return( NULL ); }
		return( &s_stArrFieldShare[fieldShareId] );
	}
	static bool set_field_share( eMEM_FIELD_SHARE fieldShareId, uint32 ofs, uint32 size );
	static uint32 calc_share_size( void );
  
	// インスタンス生成は不可
	CMemMgr( void ){};
	virtual ~CMemMgr( void ){};
    
    // アプリ依存メモリ確保
    static bool AllocForApp( void );
};
#endif	// __MEM_MGR_HPP__
