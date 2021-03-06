/*+----------------------------------------------------------------+
 |	Title:		AppDelegate.hpp [iOS環境]
 |	Comment:	アプリケーションデリゲート（※アプリ制御）
 |	Author:		K.Takayanagi
 +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
 |	Header Define	ヘッダ定義
 +----------------------------------------------------------------+*/
#ifndef __APP_DELEGATE_HPP__
#define __APP_DELEGATE_HPP__

/*+----------------------------------------------------------------+
 |	Include		インクルードヘッダ
 +----------------------------------------------------------------+*/
#include "env.hpp"
#include "MainView.hpp"
#include "MainViewController.hpp"

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
 |	Interface	インターフェイス定義
 +----------------------------------------------------------------+*/
@interface AppDelegate : UIResponder <UIApplicationDelegate>{
    UIWindow* m_pWindow;
    MainView* m_pView;
    MainViewController* m_pViewController;
}

@end

#endif  /* __APP_DELEGATE_HPP__ */
