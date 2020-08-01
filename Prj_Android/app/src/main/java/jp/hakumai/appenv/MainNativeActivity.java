/*+----------------------------------------------------------------+
  |	Title:		MainNativeActivity.java [Android環境]
  |	Comment:	メインアクティビティー
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
package jp.hakumai.appenv;

/*+----------------------------------------------------------------+
  |	Import		インポート
  +----------------------------------------------------------------+*/
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Locale;

import android.app.NativeActivity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.content.DialogInterface.OnClickListener;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

/*+----------------------------------------------------------------+
 |	Implement	実装
 +----------------------------------------------------------------+*/
public class MainNativeActivity extends NativeActivity {
	//----------------------------------------------------
	// デバッグフラグ（※本番ビルドでは[false]にすること）
	//----------------------------------------------------
	private static boolean s_bDebug = true/*false*/;

	//----------------------------------------------------
	// 定数
	//----------------------------------------------------
	private static final String TAG = "AppEnv";

	//----------------------------------------------------
	// メンバー
	//----------------------------------------------------
	// [DIALOG] 関連
	private boolean	m_bDialogBusy;
	private boolean	m_bDialogClickedPositive;
	private boolean	m_bDialogClickedNeutral;
	private boolean	m_bDialogClickedNegative;
	private String	m_strDialogInput;
	private View	m_oAdditionalViewForDialog;

	//------------------------------
	// ログ出力（デバッグ）
	//------------------------------
	static public void LOGD( String str ){
		if( s_bDebug ){
			Log.i( TAG, str );
		}
	}

	//------------------------------
	// ログ出力（エラー）
	//------------------------------
	static public void LOGE( String str ){
		if( s_bDebug ){
			Log.e( TAG, str );
		}
	}

	//---------------------------------------------
	// クラスが読み込まれた際に１度だけ呼ばれる処理
	//---------------------------------------------
	static{
		LOGD( "% [static] START" );

		// DLLの読み込み
		System.loadLibrary( "AppEnv" );

		LOGD( "% [static] END" );
	}

	//--------------------------------------------------
	// アクティビティライフサイクル
	//--------------------------------------------------
	// アクティビティが作成された
	@Override
	protected void onCreate( Bundle savedInstanceState ){
		LOGD( "% [onCreate] START" );

		//-------------------------------------------
		// 親の[onCreate]内で[android_main]が呼ばれる
		//-------------------------------------------
		super.onCreate( savedInstanceState );

		LOGD( "% [onCreate] END" );
	}

	//-----------------------------------------------
	// アクティビティが再表示されようとしている
	//-----------------------------------------------
    @Override
    protected void onRestart() {
   		LOGD( "% [onRestart] START" );

        super.onRestart();

   		LOGD( "% [onRestart] END" );
    }

    //-----------------------------------------------
    // アクティビティが表示された
    //-----------------------------------------------
    @Override
    protected void onStart(){
   		LOGD( "% [onStart] START" );

    	super.onStart();

   		LOGD( "% [onStart] END" );
    }

    //-----------------------------------------------
    // アクティビティが開始された
    //-----------------------------------------------
    @Override
    protected void onResume(){
   		LOGD( "% [onResume] START" );

        super.onResume();

        // バッテリー監視
        IntentFilter filter = new IntentFilter();
        filter.addAction( Intent.ACTION_BATTERY_CHANGED );
        registerReceiver( m_oBroadcastReceiver, filter );

		// ナビゲーションバーを非表示に設定
		hideSystemUI();

		LOGD( "% [onResume] END" );
    }

	//===========================================================================
	// システムUIを非表示に設定
	//===========================================================================
	private void hideSystemUI() {
		// Set the IMMERSIVE flag.
		// Set the content to appear under the system bars so that the content
		// doesn't resize when the system bars hide and show.
		this.getWindow().getDecorView().setSystemUiVisibility(
				View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
						| View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
//						| View.SYSTEM_UI_FLAG_IMMERSIVE);
						| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		LOGD( "% [hideSystemUI]" );
	}

	//-----------------------------------------------------
	// アクティビティが停止（バックグラウンドへ移動）された
    //-----------------------------------------------------
    @Override
    protected void onPause() {
   		LOGD( "% [onPause] START" );

        super.onPause();

        // バッテリー監視終了
        unregisterReceiver( m_oBroadcastReceiver );

   		LOGD( "% [onPause] END" );
    }

    //-----------------------------------------------
    // アクティビティが表示されなくなった
    //-----------------------------------------------
	@Override
	protected void onStop(){
		LOGD( "% [onStop] START" );

		super.onStop();

		LOGD( "% [onStop] END" );
	}

	//-----------------------------------------------
	// アクティビティが破棄されようとしている
	//-----------------------------------------------
	@Override
	protected void onDestroy() {
		LOGD( "% [onDestroy] START" );

		super.onDestroy();

		LOGD( "% [onDestroy] END" );
	}

	//-----------------------------------------------
	// アクティビティの復元用情報の取得
	//-----------------------------------------------
	@Override
	protected void onSaveInstanceState( Bundle outState ){
		LOGD( "% [onSaveInstanceState] START" );
		LOGD( "% outState=" + outState );

		super.onSaveInstanceState( outState );

		LOGD( "% [onSaveInstanceState] END" );
	}

	//-----------------------------------------------
	// 画面のコンテントが変化した
	//-----------------------------------------------
	@Override
	public void onContentChanged(){
		LOGD( "% [onContentChanged] START" );

		super.onContentChanged();

		LOGD( "% [onContentChanged] END" );
	}

	//---------------------------------------------------------
	// アクティビティの窓がウィンドウマネージャに関連付けされた
	//---------------------------------------------------------
	@Override
	public void onAttachedToWindow(){
		LOGD( "% [onAttachedToWindow] START" );

		super.onAttachedToWindow();

		LOGD( "% [onAttachedToWindow] END" );
	}

	//---------------------------------------------------------
	// アクティビティの窓がウィンドウマネージャから切り離された
	//---------------------------------------------------------
	@Override
	public void onDetachedFromWindow(){
		LOGD( "% [onDetachedFromWindow] START" );

		super.onDetachedFromWindow();

		LOGD( "% [onDetachedFromWindow] END" );
	}

	//---------------------------------------------------------
	// アクティビティの窓のフォーカスが変化した
	//---------------------------------------------------------
	@Override
	public void onWindowFocusChanged( boolean hasFocus ){
		LOGD( "% [onWindowFocusChanged] START );" );
		LOGD( "% hasFocus=" + hasFocus );

		super.onWindowFocusChanged( hasFocus );

		// ナビゲーションバーを非表示に設定
		if ( hasFocus ){
			hideSystemUIForWindowFocusChanged();
		}

		LOGD( "% [onWindowFocusChanged] END" );
	}

	//===========================================================================
	// システムUIを非表示に設定(onWindowFocusChanged 用)
	//===========================================================================
	private void hideSystemUIForWindowFocusChanged() {
		// Set the IMMERSIVE flag.
		// Set the content to appear under the system bars so that the content
		// doesn't resize when the system bars hide and show.
		this.getWindow().getDecorView().setSystemUiVisibility(
				View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
						| View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
						| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		LOGD( "% [hideSystemUIForWindowFocusChanged]" );
	}

	//---------------------------------------------------------
	// メモリが不足している
	//---------------------------------------------------------
	@Override
	public void onLowMemory(){
		LOGD( "% [onLowMemory] START" );

		super.onLowMemory();

		LOGD( "% [onLowMemory] END" );
	}

	//---------------------------------------------------------
	// [BACK]ボタンが押された
	//---------------------------------------------------------
	@Override
	public void onBackPressed(){
		// 何もしない
		//super.onBackPressed();
	}

	//---------------------------------------------------------
	// 起動先アクティビティから戻った時
	//---------------------------------------------------------
	@Override
	protected void onActivityResult( int requestCode, int resultCode, Intent data ){
	}

	//-----------------------------
	// 強制終了
	//-----------------------------
	public final void exitForce(){
		LOGD( "% [exitForce]" );

		System.exit( RESULT_OK );
	}

	//-----------------------------------
	// サイトを開く（※ブラウザ呼び出し）
	//-----------------------------------
	public final void openSite( String strURL ){
		LOGD( "% [openSite]" );
		LOGD( "% strURL=" + strURL );

		if( strURL != null && strURL.length() > 0 ){
			Uri uri = Uri.parse( strURL );
			Intent i = new Intent( Intent.ACTION_VIEW, uri );
			startActivity(i);
		}
	}

	//-----------------------------
	// バッテリー残量監視用
	//-----------------------------
	private int m_nBatteryLevel;
	private BroadcastReceiver m_oBroadcastReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive( Context context, Intent intent ){
			String action = intent.getAction();
			if( action.equals( Intent.ACTION_BATTERY_CHANGED ) ){
				m_nBatteryLevel = intent.getIntExtra("level", 0);
			}
		}
	};

	//-----------------------------
	// バッテリー残量取得
	//-----------------------------
	public final int getBatteryRest(){
		return( m_nBatteryLevel);
	}

	//-----------------------------
	// 端末の設定言語取得
	//-----------------------------
	public final String getLang(){
		Locale locale = Locale.getDefault();
		return locale.getLanguage();
	}

	//-----------------------------
	// ビルドバージョン取得
	//-----------------------------
	public final String getVersionName(){
		PackageManager pm = this.getPackageManager();
		String versionName = "?.?.?";
		try{
			PackageInfo packageInfo = pm.getPackageInfo( this.getPackageName(), 0 );
			versionName = packageInfo.versionName;
		}catch( Exception e ){
		}

		return( versionName );
	}

	//-----------------------------
	// ビルド番号取得
	//-----------------------------
	public int getVersionCode(){
		PackageManager pm = this.getPackageManager();
		int versionCode = 0;
		try{
			PackageInfo packageInfo = pm.getPackageInfo( this.getPackageName(), 0 );
			versionCode = packageInfo.versionCode;
		}catch( Exception e ){
		}

		return( versionCode );
	}

	//-----------------------------
	// 文字列テクスチャ作成
	//-----------------------------
	public void createStringTexture( String text, ByteBuffer bb ){
		//LOGD( "% [createStringTexture]" );

		// リトルエンディアンで処理
		bb.order( ByteOrder.LITTLE_ENDIAN );

		String str = text;
		float fontSize = 38;//48;
		int a = 0xFF;
		int r = 0xFF;
		int g = 0xFF;
		int b = 0xFF;

		try{
			Rect rect = new Rect();
			Paint paint = new Paint( Paint.FILTER_BITMAP_FLAG );

			paint.setTextSize(fontSize);
			paint.setAntiAlias( true );
			paint.setFilterBitmap( true );
			paint.setDither( true );
			paint.setTextAlign( Align.LEFT );
			paint.setARGB( a, r, g, b );
			paint.getTextBounds( str, 0, str.length(), rect );

			int width		= rect.width();
			int height		= rect.height();
			int tex_width	= 16;
			int tex_height	= 16;

			// ４の倍数に
			while( width > tex_width ){ tex_width += 4; }
			while( height > tex_height ){ tex_height += 4; }

			Bitmap bitmap = Bitmap.createBitmap( tex_width, tex_height, Config.ARGB_8888 );
			Canvas canvas = new Canvas(bitmap);
			canvas.drawText( str, 0-rect.left, 0-rect.top, paint );

			// サイズの出力
			bb.putInt( width );
			bb.putInt( height );
			bb.putInt( tex_width );
			bb.putInt( tex_height );
			bb.putInt( rect.left );
			bb.putInt( rect.top );
			bb.putInt( rect.right );
			bb.putInt( rect.bottom );

			//LOGD( "% w = " + width + ", h = " + height + ", left = " + rect.left + ", top = " + rect.top + ", right = " + rect.right + ", bottom = " + rect.bottom );

			// 画素の出力
			bitmap.copyPixelsToBuffer( bb );
			bitmap.recycle();
			//LOGD( "% getStringTextureData end");
			//LOGD( "% bb.capacity:"+ bb.capacity() );
			//LOGD( "% bb.position:"+ bb.position() );
			//LOGD( "% getStringTextureData end, bb width:"+ tex_width +", height:"+tex_height);
		}catch(Exception e){
			LOGE( "% % [createStringTexture] FAILED: e=" + e.toString() );
		}
	}

	//------------------------------------------------------
	// [DIALOG] 呼び出し
	//------------------------------------------------------
	// ダイアログ処理の流れは以下の通り
	//  1.[callDialog]によりダイアログを表示
	//  2.[isDialogBusy]によりダイアログの終了待ちを監視
	//  3.[isDialogBusy]が[false]を返したら、
	//    [isDialogClicked***]によりユーザーの入力を受け取る
	//------------------------------------------------------
	synchronized public final void callDialog(
			final String strTitle,		// タイトル
			final String strMess,		// 表示文言
			final String strPositive,	// 肯定ボタン文言（※不要の場合は[null]）
			final String strNeutral,	// 中立ボタン文言（※不要の場合は[null]）
			final String strNegative	// 否定ボタン文言（※不要の場合は[null]）
	){
		// ステータスクリア
		clearDialogStatus();

		// 作業検出フラグを立てる
		m_bDialogBusy = true;

		// スレッドを作成してダイアログ呼び出し
		final HandlerThread ht = new HandlerThread( "DialogThread" );
		ht.start();

		Handler h = new Handler( ht.getLooper() );
		h.post( new Runnable(){
			@Override
			public void run(){
				AlertDialog.Builder builder = createBaseDialog( strTitle, strMess, strPositive, strNeutral, strNegative );
				builder.show();
			}
		});
	}

	//----------------------------------------------------------
	// [DIALOG] 入力呼び出し
	//----------------------------------------------------------
	// 使い方は[callDialog]と同じ
	// [getDialogInput]によりユーザーの入力した文字を取得する
	//----------------------------------------------------------
	synchronized public final void callDialogInputString(
			final String strTitle,		// タイトル
			final String strMess,		// 表示文言
			final String strPositive,	// 肯定ボタン文言（※不要の場合は[null]）
			final String strNeutral,	// 中立ボタン文言（※不要の場合は[null]）
			final String strNegative,	// 否定ボタン文言（※不要の場合は[null]）
			final String strDefault		// デフォルト文言
	){
		// ステータスクリア
		clearDialogStatus();

		// 作業検出フラグを立てる
		m_bDialogBusy = true;

		// スレッドを作成してダイアログ呼び出し
		final HandlerThread ht = new HandlerThread( "DialogThread" );
		ht.start();

		Handler h = new Handler( ht.getLooper() );
		h.post( new Runnable(){
			@Override
			public void run(){
				AlertDialog.Builder builder = createBaseDialog( strTitle, strMess, strPositive, strNeutral, strNegative );

				// テキスト編集用のビューを設定
				LayoutInflater layout = (LayoutInflater) getSystemService( LAYOUT_INFLATER_SERVICE );
				final ViewGroup nullParent = null;
				m_oAdditionalViewForDialog = layout.inflate( R.layout.dialog_input_string, nullParent );

				// テキスト編集が有効なら
				EditText et = (EditText) m_oAdditionalViewForDialog.findViewById( R.id.dialogInputStringEditText );
				if( et != null && strDefault != null ){
					et.setText( strDefault );
				}

				// ビューに設定して表示
				builder.setView( m_oAdditionalViewForDialog );
				builder.show();
			}
		});
	}

	//-------------------------------------------------------------------
	// [DIALOG] 処理中か？（※モーダルとして使う際に利用）
	// [m_bDialogBusy]には複数スレッドから同時にアクセスされる可能性あり
	//-------------------------------------------------------------------
	synchronized public final boolean isDialogBusy(){
		return( m_bDialogBusy );
	}

	//--------------------------------------------------------------
	// [DIALOG] 取得関連（※[isDialogBusy]確認後にアクセスすること）
	//--------------------------------------------------------------
	public final boolean isDialogClickedPositive(){ return( m_bDialogClickedPositive ); }
	public final boolean isDialogClickedNeutral(){ return( m_bDialogClickedNeutral ); }
	public final boolean isDialogClickedNegative(){ return( m_bDialogClickedNegative ); }
	public final String getStrDialogInput(){ return( m_strDialogInput ); }

	//---------------------------------------
	// [DIALOG] 状態クリア
	//---------------------------------------
	private final void clearDialogStatus(){
		m_bDialogBusy				= false;
		m_bDialogClickedPositive	= false;
		m_bDialogClickedNeutral		= false;
		m_bDialogClickedNegative	= false;
		m_strDialogInput			= null;
		m_oAdditionalViewForDialog	= null;
	}

	//---------------------------------------
	// [DIALOG] 基本ダイアログ作成
	//---------------------------------------
	private final AlertDialog.Builder createBaseDialog(
			String strTitle,
			String strMess,
			String strPositive,
			String strNeutral,
			String strNegative
	){
		AlertDialog.Builder builder = new AlertDialog.Builder( this );

		// タイトル設定
		if( strTitle != null && strTitle.length() > 0 ){
			builder.setTitle( strTitle );
		}else{
			strTitle = null;
		}

		// メッセージ設定
		if( strMess != null && strMess.length() > 0 ){
			builder.setMessage( strMess );
		}else{
			strMess = null;
		}

		// 肯定ボタン設定
		if( strPositive != null && strPositive.length() > 0 ){
			builder.setPositiveButton( strPositive, new OnClickListener(){
				@Override
				public void onClick( DialogInterface dialog, int width ){
					m_bDialogClickedPositive = true;
					m_bDialogBusy = false;

					// 追加ビューが有効なら内部の確認
					if( m_oAdditionalViewForDialog != null ){
						// テキスト編集が有効なら
						EditText et = (EditText) m_oAdditionalViewForDialog.findViewById( R.id.dialogInputStringEditText );
						if( et != null ){
							m_strDialogInput = "" + et.getText();
						}
					}
				}
			});
		}else{
			strPositive = null;
		}

		// 中間ボタン設定
		if( strNeutral != null && strNeutral.length() > 0 ){
			builder.setNeutralButton( strNeutral, new OnClickListener(){
				@Override
				public void onClick( DialogInterface dialog, int width ){
					m_bDialogClickedNeutral = true;
					m_bDialogBusy = false;
				}
			});
		}else{
			strNeutral = null;
		}

		// 否定ボタン設定
		if( strNegative != null && strNegative.length() > 0 ){
			builder.setNegativeButton( strNegative, new OnClickListener(){
				@Override
				public void onClick( DialogInterface dialog, int width ){
					m_bDialogClickedNegative = true;
					m_bDialogBusy = false;
				}
			});
		}else{
			strNegative = null;
		}

		// ボタンが設定されていなければ画面外タップでキャンセル（※否定ボタンと同じ扱い）
		if( strPositive == null && strNeutral == null && strNegative == null ){
			// 画面外タップでのキャンセル
			builder.setOnCancelListener( new OnCancelListener(){
				@Override
				public void onCancel( DialogInterface arg0 ) {
					m_bDialogClickedNegative = true;
					m_bDialogBusy = false;
				}
			});
			builder.setCancelable( true );
		}else{
			builder.setCancelable( false );
		}

		return( builder );
	}
}
