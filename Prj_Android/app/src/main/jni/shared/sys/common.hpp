/*+----------------------------------------------------------------+
  |	Title:		common.hpp [共通環境]
  |	Comment:	共通定義
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef	__COMMON_HPP__
#define	__COMMON_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// NULL
#ifndef NULL
#define NULL					((void*)0)
#endif

// 無効値
#define INVALID_VAL				(-1)

// バイトサイズ
#define KB					    (1024)
#define KBf						(1024.0f)
#define	MB						(1024*1024)
#define	MBf						(1024.0f*1024.0f)
#define KB_ADJ( _s )			(((_s)+(KB-1))/KB)
#define MB_ADJ( _s )			(((_s)+(MB-1))/MB)
#define ALIGN_ADJ( _s, _a )     ((((_s)+((_a)-1))/(_a))*(_a))

// 文字列長
#define COMMON_NAME_LEN     128		// 名前、ファイル名等用

#endif /* __COMMON_HPP__ */