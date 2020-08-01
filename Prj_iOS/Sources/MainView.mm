/*+----------------------------------------------------------------+
 |	Title:		MainView.mm [iOS環境]
 |	Comment:	メインビュー（※ループ制御）
 |	Author:		K.Takayanagi
 +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
 |	Include		インクルードヘッダ
 +----------------------------------------------------------------+*/
#include "env.hpp"
#include "MainView.hpp"

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
 |	Implementation		実装
 +----------------------------------------------------------------+*/
@implementation MainView

@synthesize glContext = m_pContext;
@synthesize glRenderbuffer = m_nRenderbuffer;
@synthesize glFramebuffer = m_nFramebuffer;

@synthesize dialogActive = m_bDialogActive;
@synthesize dialogSelectedIndex = m_nDialogSelectedIndex;
@synthesize dialogInput = m_pStrDialogInput;

//------------------------------------
// レイヤークラスの取得
//------------------------------------
+ (Class) layerClass{
    return( [CAEAGLLayer class] );
}

//------------------------------------
// 初期化
//------------------------------------
- (id) initWithFrame: (CGRect) frame{
    LOGD( "@ [MAIN VIEW] initWithFrame:\n" );
    
    self = [super initWithFrame:frame];
    if( self != nil ){
        self.multipleTouchEnabled = YES;
        self.contentMode = UIViewContentModeCenter;
        self.backgroundColor = [UIColor blackColor];
        
        //------------------------------------------------------------------------------------
        // CADisplayLinkは、アプリがディスプレイのリフレッシュレートに描画を同期させるタイマーオブジェクト
        //
        // アプリは新しいディスプレイリンクを生成し、画面が更新された時に呼び出されるセレクタと
        // ターゲットオブジェクトを指定することで、実行ループにディスプレイリンクを追加する
        //
        // 一度ディスプレイリンクが実行ループに関連付けされると、
        // ターゲット上のセレクタは画面のコンテンツを更新する必要がある時に呼び出される
        //
        // ターゲットは、以前のフレームが表示された時刻を取得するために、
        // ディスプレイリンクのtimestampプロパティ読み取ることができる
        //
        // アプリケーションはpausedプロパティをYESに設定することによって通知を無効化できる
        //
        // frameIntervalプロパティを変更することによって、
        // （見かけ上のフレームレートを減少させ）フレーム間の時間を増大させることができる
        //
        // アプリケーションがディスプレイリンクを終了させる際、
        // 全ての実行ループから削除しターゲットからの関連付けを解除するためにinvalidateを呼び出す必要がある
        //
        // CADisplayLinkはサブクラス化してはならない
        //------------------------------------------------------------------------------------
        m_pDisplayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(updateWithDisplayLink:)];
        [m_pDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        
        // 時間を更新しておく
        m_fLastUpdate = CACurrentMediaTime();
    }
    
    return( self );
}

//----------------------------------------
// ここが実質的なゲームループ
//----------------------------------------
- (void) updateWithDisplayLink: (CADisplayLink*) displayLink{
    if( displayLink != nil ){
#if 0
        if( ((int)m_fLastUpdate) != ((int)displayLink.timestamp) ){
            LOGD( "@ [MAIN VIEW] updateWithDisplayLink: time=%f\n", displayLink.timestamp );
        }
#endif
        m_fLastUpdate = displayLink.timestamp;
    }
    
    // エラーダイアログ表示中なら、ユーザーの入力を待ってアプリ終了
    if( m_bFinishAppAfterDialog ){
        if( ! env_native_isDialogBusy() ){
            env_native_exitApp( true );
        }
    }else{
        // メモリ管理有効
        CMemMgr::SetAllocEnable( true );
        
        // コールバック確認（※制御系のエラーはここで発生する）
        env_check_callback();
        
        // 致命的エラーが発生していたらダイアログ表示
        if( env_check_fatal_error() ){
            m_bFinishAppAfterDialog = true;
            env_native_callDialog( "エラーが発生しました", env_get_error_log(), NULL, NULL, "OK" );
        }else{
            // 更新
            env_update();
        }
        
        // メモリ管理無効
        CMemMgr::SetAllocEnable( false );
    }
}

//----------------------------------
// 作成
//----------------------------------
- (bool) onSurfaceCreate{
    LOGD( "@ [MAIN VIEW] onSurfaceCreate:\n" );
    
    // [ES 2.0]としてコンテキストの作成
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    m_pContext = [[EAGLContext alloc] initWithAPI: api];
    
    if( ![EAGLContext setCurrentContext: m_pContext] ){
        LOGE( "@ [EAGLContext setCurrentContext]: FAILED\n" );
        return( false );
    }
 
    // レンダーバッファの作成
    glGenRenderbuffers( 1, &m_nRenderbuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, m_nRenderbuffer );
    
    // フレームバッファの作成
    glGenFramebuffers( 1, &m_nFramebuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, m_nFramebuffer );

    // フレームバッファのアタッチ
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_nRenderbuffer );

    // レイヤーに関連付け
    CAEAGLLayer* pLayer = (CAEAGLLayer*) super.layer;
    
    // Construct a dictionary with our configurations.
    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                          [NSNumber numberWithBool:YES],
                          kEAGLColorFormatRGBA8,
                          nil];
    
    pLayer.drawableProperties = dict;
    pLayer.opaque = YES;
    
    if( ![m_pContext renderbufferStorage: GL_RENDERBUFFER fromDrawable: pLayer] ){
        LOGE( "@ [m_pContext renderbufferStorage]: FAILED\n" );
        return( false );
    }
    
    return( true );
}

//----------------------------------
// 中断
//----------------------------------
- (bool) onSuspend{
    LOGD( "@ [MAIN VIEW] onSuspend:\n" );

    // ディスプレイリンク中断
    m_pDisplayLink.paused = YES;
    
    return( true );
}

//----------------------------------
// 再開
//----------------------------------
- (bool) onResume{
    LOGD( "@ [MAIN VIEW] onResume:\n" );

    // ディスプレイリンク再開
    m_pDisplayLink.paused = NO;
    
    return( true );
}

//----------------------------------
// 終了
//----------------------------------
- (bool) onExit{
    LOGD( "@ [MAIN VIEW] onExit:\n" );
    
    if( m_nRenderbuffer > 0 ){
        glDeleteRenderbuffers( 1, &m_nRenderbuffer );
    }
    
    if( m_nFramebuffer > 0 ){
        glDeleteFramebuffers( 1, &m_nFramebuffer );
    }
    
    // コンテキストの関連付けを外す
    if( [EAGLContext currentContext] == m_pContext ){
        [EAGLContext setCurrentContext: nil];
    }

    // ディスプレイリンクの無効化
    [m_pDisplayLink invalidate];

    return( true );
}

//-----------------------------
// タッチ開始（指が触れた）
//-----------------------------
- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event{
    stTOUCH_EVENT_DESCRIPTOR ted;
    ted.nEventType = UITouchPhaseBegan;
    ted.pEvent = event;
    
    env_push_input_event( &ted );
}

//-----------------------------
// タッチ終了（指が離れた）
//-----------------------------
- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event{
    stTOUCH_EVENT_DESCRIPTOR ted;
    ted.nEventType = UITouchPhaseEnded;
    ted.pEvent = event;
    
    env_push_input_event( &ted );
}

//-----------------------------
// タッチ移動（指が動いた）
//-----------------------------
- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event{
    stTOUCH_EVENT_DESCRIPTOR ted;
    ted.nEventType = UITouchPhaseMoved;
    ted.pEvent = event;
    
    env_push_input_event( &ted );
}

//-------------------------------
// タッチキャンセル（電話等で中断された）
//-------------------------------
- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event{
    stTOUCH_EVENT_DESCRIPTOR ted;
    ted.nEventType = UITouchPhaseCancelled;
    ted.pEvent = event;
    
    env_push_input_event( &ted );
}

//-----------------------------
// [DIALOG] 関連
//-----------------------------
// ダイアログ呼び出し
- (void) callDialog: (NSString*) strTitle withMess: (NSString*) strMess withPositiveButton: (NSString*) strPositive withNeutralButton: (NSString*) strNeutral withNegativeButton: (NSString*) strNegative withDefaultText: (NSString*) strDefault{

    // 状態設定
    m_bDialogActive = true;
    m_nDialogSelectedIndex = -1;
    
    if( m_pStrDialogInput != nil ){
        [m_pStrDialogInput release];
        m_pStrDialogInput = nil;
    }

    // ダイアログ確保
    UIAlertController* ac = [UIAlertController alertControllerWithTitle: strTitle
                                message: strMess
                                preferredStyle: UIAlertControllerStyleAlert];
   
    if( strDefault != nil ){
        [ac addTextFieldWithConfigurationHandler:^(UITextField *textField){
            textField.text = strDefault;
        }];
    }

    
    if( strPositive != nil && [strPositive length] > 0 ){
        [ac addAction:[UIAlertAction actionWithTitle:strPositive style:UIAlertActionStyleDefault
            handler:^(UIAlertAction *action) {
                m_nDialogSelectedIndex = 0;
                m_bDialogActive = false;
            
                if( strDefault != nil ){
                    UITextField *textField = ac.textFields.firstObject;
                    m_pStrDialogInput = [[NSString alloc] initWithString: textField.text];
                }
            }
        ]];
    }
        
    if( strNeutral != nil && [strNeutral length] > 0 ){
        [ac addAction:[UIAlertAction actionWithTitle:strNeutral style:UIAlertActionStyleDestructive
            handler:^(UIAlertAction *action) {
                m_nDialogSelectedIndex = 1;
                m_bDialogActive = false;
            }
        ]];
    }
    
    if( strNegative != nil && [strNegative length] > 0 ){
        [ac addAction:[UIAlertAction actionWithTitle:strNegative style:UIAlertActionStyleCancel
            handler:^(UIAlertAction *action) {
                m_nDialogSelectedIndex = 2;
                m_bDialogActive = false;
            }
        ]];
    }

    [g_stEnvNative.pViewController presentViewController:ac animated:YES completion:nil];
}

@end
