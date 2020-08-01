/*+----------------------------------------------------------------+
 |	Title:		MainView.hpp [iOS環境]
 |	Comment:	メインビュー（※ループ制御）
 |	Author:		K.Takayanagi
 +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
 |	Header Define	ヘッダ定義
 +----------------------------------------------------------------+*/
#ifndef __MAIN_VIEW_HPP__
#define __MAIN_VIEW_HPP__

/*+----------------------------------------------------------------+
 |	Include		インクルードヘッダ
 +----------------------------------------------------------------+*/
#include "env.hpp"

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
@interface MainView : GLKView{
    CADisplayLink* m_pDisplayLink;
    CFTimeInterval m_fLastUpdate;
    
    // 制御用
    bool m_bFinishAppAfterDialog;
    
    // [OPEN GL]関連
    EAGLContext* m_pContext;
    uint32 m_nRenderbuffer;
    uint32 m_nFramebuffer;
    
    // [DIALOG]関連
    bool m_bDialogActive;
    long m_nDialogSelectedIndex;
    NSString* m_pStrDialogInput;
}

@property(readonly) EAGLContext* glContext;
@property(readonly) uint32 glRenderbuffer;
@property(readonly) uint32 glFramebuffer;
@property(readonly) bool dialogActive;
@property(readonly) long dialogSelectedIndex;
@property(readonly) NSString* dialogInput;

+ (Class) layerClass;

- (id) initWithFrame: (CGRect) frame;

- (void) updateWithDisplayLink: (CADisplayLink*) displayLink;

- (bool) onSurfaceCreate;
- (bool) onSuspend;
- (bool) onResume;
- (bool) onExit;

// 入力関連
- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event;

// [DIALOG]関連
- (void) callDialog: (NSString*) strTitle withMess: (NSString*) strMess withPositiveButton: (NSString*) strPositive withNeutralButton: (NSString*) strNeutral withNegativeButton: (NSString*) strNegative withDefaultText: (NSString*) strDefault;

@end

#endif  /* __MAIN_VIEW_HPP__ */
