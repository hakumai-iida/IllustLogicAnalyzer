/*+----------------------------------------------------------------+
  |	Title:		Bezier.hpp [共通環境]
  |	Comment:	ベジェ曲線（※対象はパレット画像[P8D]）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BEZIER_HPP__
#define __BEZIER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BezierStruct.hpp"
#include "LayerData.hpp"
#include "LineObjectData.hpp"
#include "PaintObjectData.hpp"
#include "SlotObjectData.hpp"
#include "obj/Spring.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//------------------------------
// 分割点（※アンカーポイント間の補完）
//------------------------------
typedef struct{
    float x, y;                     // 座標：テクスチャの左上を原点とした位置（※傾きがある場合は適用済みの値となる）
    float x0, y0;                   // 元座標（※傾き適用前の値＝タッチの処理で利用する）
    float strokeSize;               // ストロークサイズ
    bool isEdgeFillCheck;           // エッジ埋めの確認をするか？
    stBEZIER_ANCHOR_POINT* pAP;     // 分割元のアンカーポイント（※描画情報参照用）
} stBEZIER_DIVISION_POINT;

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ベジェ曲線
//----------------------------------------
class CBezier{
public:
    // 領域確保情報（※端末の解像度によってサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSizeK( void );
    
private:
    // 管理フラグ
    static bool s_bDispDirVectorAlways;         // 方向線を常に表示する
    static bool s_bDispOffDirVectorAlways;      // 方向線を常に表示しない
    static bool s_bDispPathActiveAlways;        // 全てのパスの表示をアクティブにする（一律の色味で表示する）
    static bool s_bDispOnlyAnchorPoint;         // アンカーポイントのみ憑依ｚ
    
public:
    // 管理
    inline static void SetDispDirVectorAlways( bool flag ){ s_bDispDirVectorAlways = flag; }
    inline static void SetDispOffDirVectorAlways( bool flag ){ s_bDispOffDirVectorAlways = flag; }
    inline static void SetDispPathActiveAlways( bool flag ){ s_bDispPathActiveAlways = flag; }
    inline static void SetDispOnlyAnchorPoint( bool flag ){ s_bDispOnlyAnchorPoint = flag;}

    inline static bool IsDispDirVectorAlways( void ){ return( s_bDispDirVectorAlways ); }
    inline static bool IsDispOffDirVectorAlways( void ){ return( s_bDispOffDirVectorAlways ); }
    inline static bool IsDispPathActiveAlways( void ){ return( s_bDispPathActiveAlways ); }
    inline static bool IsDispOnlyAnchorPoint( void ){ return( s_bDispOnlyAnchorPoint );}

private:
    //-------------------------------------
    // ワーク
    //-------------------------------------
    // 分割点（※ストロークはＡＰ間の距離に応じて分割点に分けられて処理される）
    static stBEZIER_DIVISION_POINT* s_stArrDivisionPoint;
    static int s_nUseDivisionPoint;         // 現在の分割点利用数
    static int s_nUseDivisionPointMax;      // 最大の分割点利用数（※ログ用）
    
    // テンポラリ（※ドット出力処理内での[if]判定を極力省くため、座標とストロークサイズを事前計算してバッファに登録しておき、後でまとめて処理する用）
    static float* s_fArrTempPointX;         // テンポラリ：ポイントＸ
    static float* s_fArrTempPointY;         // テンポラリ：ポイントＹ
    static float* s_fArrTempPointX0;        // テンポラリ：ポイントＸ（※傾き適用前）
    static float* s_fArrTempPointY0;        // テンポラリ：ポイントＹ（※傾き適用前）
    static float* s_fArrTempStrokeSize;     // ストロークサイズ
    
    // フック（※データ指定／システム指定の２パターンで指定される）
    static bool* s_bArrHookPoint;          // 結合点有効性
    static float* s_fArrHookPointX;        // 結合点位置Ｘ
    static float* s_fArrHookPointY;        // 結合点位置Ｙ

    // タッチ（※タッチ描画の起点となる座標関連）
    static int* s_nArrTouchPointNum;        // タッチ登録数
    static float** s_fArrArrTouchPointX;    // タッチ位置Ｘ
    static float** s_fArrArrTouchPointY;    // タッチ位置Ｙ
    static float* s_fArrTouchPoint;         // タッチ位置（※バッファ実体）
    static float** s_fArrArrTouchPointX0;   // タッチ位置Ｘ（※傾き適用前）
    static float** s_fArrArrTouchPointY0;   // タッチ位置Ｙ（※傾き適用前）
    static float* s_fArrTouchPoint0;        // タッチ位置（※傾き適用前）（※バッファ実体）
    static float* s_fArrTouchWorkForLen0;   // タッチワーク（長さ）

    static int s_nTouchStrokePointCur;      // タッチストロークポイント現在値
    static int s_nTouchStrokePointMax;      // タッチストロークポイント最大数
    static int s_nTouchStrokePointLimit;    // タッチストロークポイント出力制限

    // ストライプ塗りつぶし情報
    static int s_nStripePointNum;           // ストライプ塗りつぶし点登録数
    static BYTE* s_nArrStripePalGrp;        // ストライプ塗りつぶしパレットグループ値
    static float* s_fArrStripePointX;       // ストライプ塗りつぶし点位置Ｘ
    static float* s_fArrStripePointY;       // ストライプ塗りつぶし点位置Ｙ

    // ガイド（※塗りガイド座標参照用／ストライプの塗りつぶし位置の算出用）
    static int s_nNumStripeGuidePoint;      // ドット数
    static int* s_nArrStripeGuidePointX;    // ドットのＸ座標
    static int* s_nArrStripeGuidePointY;    // ドットのＹ座標

    // エッジ塗り
    static int s_nBufSizeForEdgeFillGuide;                  // エッジ塗りガイドサイズ
    static BYTE* s_pBufForEdgeFillGuide;                    // エッジ塗りガイド
    static int s_nOfsXForEdgeFillGuide;                     // エッジ塗り：オフセットX（※簡便のため／固定的な値を引数で渡すのを避けるため）
    static int s_nOfsYForEdgeFillGuide;                     // エッジ塗り：オフセットY（※簡便のため／固定的な値を引数で渡すのを避けるため）
    static stBEZIER_ANCHOR_POINT* s_pApForEdgeFillGuide;    // エッジ塗り：AP（※簡便のため／固定的な値を引数で渡すのを避けるため）
    
    // バッファの利用状況（※有効領域判定用）
    static int s_nLeftForStrokeBuf;     // ストロークバッファの左端
    static int s_nRightForStrokeBuf;    // ストロークバッファの右端
    static int s_nTopForStrokeBuf;      // ストロークバッファの上端
    static int s_nBottomForStrokeBuf;   // ストロークバッファの下端
    static int s_nLeftForColorBuf;      // カラーバッファの左端
    static int s_nRightForColorBuf;     // カラーバッファの右端
    static int s_nTopForColorBuf;       // カラーバッファの上端
    static int s_nBottomForColorBuf;    // カラーバッファの下端

    //-------------------------------------
    // 入力データ
    //-------------------------------------
    static stBEZIER_BASE_PARAM*  s_pBaseParam;
    static stBEZIER_LAYER_PARAM* s_pLayerParam;

    //-------------------------------------
    // 処理設定
    //-------------------------------------
    static BYTE* s_pBuf;                    // メインバッファ（※ストローク時は不透明色出力先）
    static BYTE* s_pBufFillGuide;           // 塗りつぶしガイド（※ここに描かれた閉領域が塗りつぶしの対象となる）
    static BYTE* s_pBufFillGuard;           // バッファ：塗りガード（マスク）
    static BYTE* s_pBufTest;                // テストバッファ（※ストロークの出力可能判定用）
    static int s_nBufW, s_nBufH;            // バッファサイズ
    static BYTE s_nTestPalGrp;              // テスト値（※[s_pBufTest]内の、この値と同じパレットグループの領域のみが出力対象となる）
    static BYTE s_nTestPalGrpForRepair;     // ライン修復時のテスト値
    static bool s_bTouch;                   // タッチの描画
    static bool s_bStripe;                  // ストライプの描画
    static bool s_bFrill;                   // フリルの描画
    static bool s_bWorkPath;                // ワークパス出力指定
    
    //---------------------------------------------------
    // ワークパス表示用（※アンカーポイント作業用の小さいマーク各種）
    //---------------------------------------------------
    static int  s_nSizeForWorkPathLine;         // パス画素サイズ（ストローク）
    static int  s_nSizeForWorkPathPoint;        // ポイント画素サイズ（アンカーポイント）
    static int  s_nSizeForWorkPathSelected;     // 選択中のポイント画素サイズ（アンカーポイント）
    static int  s_nSizeForWorkPathTriangle;     // 三角画素サイズ（方向線の向き）
    static int  s_nSizeForWorkPathCross;        // バツ画素サイズ（塗りつぶしポイント）
    static int  s_nSizeForWorkPathPlus;         // プラス画素サイズ（スロットポイント／基準点）
    static BYTE* s_pDotForWorkPathTriangle;     // 三角画素（方向線の向き）
    static BYTE* s_pDotForWorkPathCross;        // バツ画素（塗りつぶしポイント）
    static BYTE* s_pDotForWorkPathPlus;         // プラス画素（スロットポイント／基準点）
   
    //---------------------------------------------------
    // フリルデータ（※処理が固まったらMgrで管理するか？）
    //---------------------------------------------------
    static CLayerData* s_pArrLayerDataForFrill[eSTROKE_FRILL_MAX];
    
    //---------------------------------------------------
    // 線のブレ用
    //---------------------------------------------------
    static CSpring s_oSpringForBrushPos;        // バネ：ブラシの位置ブレ
    static CSpring s_oSpringForBrushSize;       // バネ：ブライのサイズブレ
    static int m_nResetCountForBrushPos;        // リセットカウント：ブラシの位置ずれ
    static int m_nResetCountForBrushSize;       // リセットカウント：ブラシのサイズブレ
    static float m_nUpdateFrameForBrushPos;     // 更新カウント：ブラシの位置ずれ
    static float m_nUpdateFrameForBrushSize;    // 更新カウント：ブラシの位置ずれ

public:
    // 初期化／終了
	static bool OnCreate( void );
	static void OnDestroy( void );
    
    // 情報取得
    static int GetDivisionPointNumMax( void );
    static int GetDivisionPointNumUseMax( void );
    
    // 領域関連（※画像の利用状況＝四隅の位置）
    static void ResetBufInfo( int w, int h );
    static int GetLeftForStrokeBuf( void ){ return( s_nLeftForStrokeBuf ); }
    static int GetRightForStrokeBuf( void ){ return( s_nRightForStrokeBuf ); }
    static int GetTopForStrokeBuf( void ){ return( s_nTopForStrokeBuf ); }
    static int GetBottomForStrokeBuf( void ){ return( s_nBottomForStrokeBuf ); }
    static int GetLeftForColorBuf( void ){ return( s_nLeftForColorBuf ); }
    static int GetRightForColorBuf( void ){ return( s_nRightForColorBuf ); }
    static int GetTopForColorBuf( void ){ return( s_nTopForColorBuf ); }
    static int GetBottomForColorBuf( void ){ return( s_nBottomForColorBuf ); }

    inline static void UpdateStrokeBufInfo( int x, int y ){
        if( s_nLeftForStrokeBuf > x ){ s_nLeftForStrokeBuf = x; }
        if( s_nRightForStrokeBuf < x ){ s_nRightForStrokeBuf = x; }
        if( s_nTopForStrokeBuf > y ){ s_nTopForStrokeBuf = y; }
        if( s_nBottomForStrokeBuf < y ){ s_nBottomForStrokeBuf = y; }
    }

    inline static void UpdateColorBufInfo( int x, int y ){
        if( s_nLeftForColorBuf > x ){ s_nLeftForColorBuf = x; }
        if( s_nRightForColorBuf < x ){ s_nRightForColorBuf = x; }
        if( s_nTopForColorBuf > y ){ s_nTopForColorBuf = y; }
        if( s_nBottomForColorBuf < y ){ s_nBottomForColorBuf = y; }
    }

    // フック登録リセット
    static void ResetHookPoint( bool isAll=true );

    // タッチ登録リセット
    static void ResetTouchPoint( bool isAll=true );

    // 全フラグ解除（※開発用）
    static void ResetAllFlagForDev( void );
    
    //------------------------------------------------------------------------------------------
    // ストローク（線）の描画
    //------------------------------------------------------------------------------------------
    // [stBEZIER_ANCHOR_POINT* pHead]で指定される線を描画する
    // [pBuf]に画素が出力される
    // [pBufFillGuide]には塗りつぶし用の領域が１ドットのラインとして出力される（※クローズパスが前提）
    // [pBufFillGuard]により出力が制限される
    // [pBufColor]により出力が制御される（※[pBufColor]の[testVal]に相当する位置の画素しか出力されなくなる）
    // [isDot]によりアンカーポイント間の線の代わりにアンカーポイント上に点を描く
    //------------------------------------------------------------------------------------------
    static int Stroke( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                       BYTE testPalGrp, BYTE testPalGrpForRepair, bool isDot, bool isWorkPath );
    
    //------------------------------------------------------------------------------------------
    // ストローク（タッチ）の描画（※引数の意味は[Stroke]と同じ）
    //（※ワークパス、フックの塗り返し、ドット描画には対応しない）
    //------------------------------------------------------------------------------------------
    static int Touch( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                      BYTE testPalGrp, bool isStripe, bool isWorkPath );
    
    // フリル（※内部でフリルタイプに対応した[CLayerData]を描画する）
    static int TouchForFrill( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                              BYTE testPalGrp, bool isWorkPath );

    //------------------------------------------------------------------------------------------
    // 塗りつぶし
    //------------------------------------------------------------------------------------------
    // [stBEZIER_FILL_POINT* pHead]で指定される情報で[pBufColor]を塗りつぶす
    // [pHead]で指定される点から[pBufFillGuide]で指定される閉領域が塗りつぶされる
    // 塗りつぶしは「下塗り(pBufFillGuide)→本塗り(pBufColor)」の流れで行われる
    //（※下塗り時の[testVal]の参照対象は[pBufColor]となる）
    // [pBufFillGuard]により出力が制限される
    //------------------------------------------------------------------------------------------
    static int Fill( stBEZIER_FILL_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                     BYTE testPalGrp, bool isWorkPath );

    //------------------------------------------------------------------------------------------
    // 情報：スロット（※開発用：スロット位置に印を出力）
    //------------------------------------------------------------------------------------------
    static void SlotForDev( stBEZIER_SLOT_POINT* pHead, stBEZIER_BASE_PARAM* pParam );

    //------------------------------------------------------------------------------------------
    // 情報：基準点（※開発用：基準点に印を出力）
    //------------------------------------------------------------------------------------------
    static void BasePointForDev( float x, float y, stBEZIER_BASE_PARAM* pParam, bool isActive, bool isSelected );

private:
    // ワーククリア
    static void ClearWork( void );
    static void ClearWorkSetting( void );
    static void ClearWorkPoint( void );
    static void ClearWorkStripePoint( void );
    
    // 出力先設定
    static bool SetBufForStroke( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath );
    static bool SetBufForFill( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath );
    static bool SetBufForDev( stBEZIER_BASE_PARAM* pParam );

    //-----------------------
    // 分割点関連
    //-----------------------
    // AP間の分割点の登録
    static void RegistDivisionPointList( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 );
    
    // AP間の分割点数の算出
    static int  CalcDivisionNumForSegment( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 );
    
    // 分割点の追加
    static void AddDivisionPoint( float x, float y, float strokeSize, bool isEdgeFillCheck, stBEZIER_ANCHOR_POINT* pAP );
    
    //-----------------------
    // ストロークの描画
    //-----------------------
    // 指定の分割点間を線形補間しながら描画（※描画設定は[pAP]から参照する）
    static void PutDotLinear( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP );

    // 分割点間の座標を算出して一時バッファに格納
    static int CalcThenSetPointForTempBuf( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP );

    // ２点間のドット数の算出
    static int  CalcPointNum( float x0, float y0, float x1, float y1 );
    
    // ドットを置く
    static void PutDotAt( float x0, float y0, float strokeSize, BYTE testPalGrp, CStroke* pStroke, stBEZIER_ANCHOR_POINT* pAP );
    
    // フックの登録／設定
    static void resolveHookPoint( stBEZIER_ANCHOR_POINT* pBAP );

    // ストライプ塗りポイントの追加
    static void AddStripePoint( BYTE palOfsGrp, float x, float y );
    
    //====================================================================================
    // 以下は、[BezierSub.cpp]で実装
    //====================================================================================
    //----------------------------------------------
    // ワークパス用
    //----------------------------------------------
    static void ReadyDotForWorkPath( void );
    static void PutTexForWorkPath( float x0, float y0, int size, BYTE* pTex, BYTE pal, int cut=0 );
    
    static void PutLineDotForWorkPath( float x0, float y0, BYTE pal );
    static void PutPointForWorkPath( float x0, float y0, BYTE pal );
    static void PutTriangleForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal, bool isFlip );
    static void PutCrossForWorkPath( float x0, float y0, BYTE pal );
    static void PutPlusForWorkPath( float x0, float y0, BYTE pal );
    static void PutPlusForWorkPathS( float x0, float y0, BYTE pal );
    
    static void PutLinearForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal );

    //----------------------------------------------
    // 描画（※各種実体）
    //----------------------------------------------
    static void DrawStroke( stBEZIER_ANCHOR_POINT* pHead, float limitRate=1.0f );
    static void ReverseTouchPoint( eSTROKE_TOUCH_TARGET targetTouchId, int atFrom, int atTo );
    static void FillStrokeEdge( int targetDivisionPoint );
    static void FillStrokeAt( int from, int x, int y );
    
    static void DrawDot( stBEZIER_ANCHOR_POINT* pHead );
    
    //----------------------------------------------
    // ブラシのブレ用
    //----------------------------------------------
    static void ResetBrushShake( void );
    static void SetBrushShake( void );
    static void UpdateBrushShake( void );
    static float GetBrushShakeForPosX( void );
    static float GetBrushShakeForPosY( void );
    static float GetBrushShakeForSize( void );

    //====================================================================================
    // 以下は、[BezierForFill.cpp]で実装
    //====================================================================================
    static void DrawFill( stBEZIER_FILL_POINT* pHead );
    static void DrawFillPointBright( stBEZIER_FILL_POINT* pFP, float x, float y, BYTE targetPalGrp );
    static void DrawFillPointDark( stBEZIER_FILL_POINT* pFP, float x, float y, BYTE targetPalGrp );
    static void DrawFillAntiForTouch( stBEZIER_FILL_POINT* pHead );
    static void DrawFillAntiBrightForTouch( stBEZIER_FILL_POINT* pFP, eSTROKE_TOUCH_TARGET touchId, BYTE targetPalGrp );
    static void DrawFillAntiDarkForTouch( stBEZIER_FILL_POINT* pFP, eSTROKE_TOUCH_TARGET touchId, BYTE targetPalGrp );

    //=================================================
    // 以下は、[BezierForTouch.cpp]で実装
    //=================================================
public:
    // タッチ対象によるガイド描画
    static bool DrawFillGuideForTouch( stBEZIER_BASE_PARAM* pParam );

private:
    static void DrawTouch( stBEZIER_ANCHOR_POINT* pHead );
    static void DrawTouchAt( stBEZIER_ANCHOR_POINT* pPairHead, float x, float y, float size, float rot, float* pCenterX=NULL, float* pCenterY=NULL );
    static float CalcTouchWorkForLen0( int target );
    static int SearchTouchIndexForLen0( int target, float len, int from, bool isBack );

    //=================================================
    // 以下は、[BezierForFrill.cpp]で実装
    //=================================================
private:
    static void DrawFrill( stBEZIER_ANCHOR_POINT* pHead );
    static void DrawFrillAt( stBEZIER_ANCHOR_POINT* pBAP, CLayerData* pLD, float x, float y, float size, float rot );

    //=================================================
    // 以下は、[BezierForFrillLayerData.cpp]で実装
    //=================================================
public:
    // フリルデータの取得（※開発用に外部からも見えるようにしておく）
    static CLayerData* GetLayerDataForFrill( eSTROKE_FRILL frillId );

private:
    static bool AllocFrill( void );
    static void ReleaseFrill( void );

    static bool AllocFrillForTestCircle( void );
    static bool AllocFrillForTestTriangle( void );
    static bool AllocFrillForTestSquare( void );

private:
    // インスタンス作成は不可
    CBezier( void ){}
    virtual ~CBezier( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __BEZIER_HPP__ */
