/*+----------------------------------------------------------------+
  |	Title:		AnchorPointData.hpp [共通環境]
  |	Comment:	アンカーポイントデータ（※[CLineObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ANCHOR_POINT_DATA_HPP__
#define __ANCHOR_POINT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/tex/adjust/AdjustablePoint.hpp"
#include "draw/tex/adjust/JointPoint.hpp"

#include "sys/data/ListNode.hpp"
#include "sys/data/DataCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eAPD_FLAG{
	eAPD_FLAG_INVALID = -1,                 // 無効値

    eAPD_FLAG_DISABLE,                      // 00:[*] 無効（※[CLineObjectData::CreateBezierAnchorPoint]で登録されない）
    eAPD_FLAG_TRANSPARENCY,                 // 01:[(T)] 透明（※塗りガイドを出力したいが線は描画したくない時）
    eAPD_FLAG_NO_FILL_GUIDE,                // 02:[nG] 線のみ（※線を描画したいが塗りガイドを出力したくない時）
    eAPD_FLAG_LINE_REPAIR_TEST,             // 03:[lrT] 線修復テストを行うか？（※[ClineObjectData::getTestPalOfsIdForRepair]に対して線を引くか？）
    eAPD_FLAG_JOINT_NO_HOOK_IF_OVERWRAPPED, // 04:[jnH] ジョイント時に角度が重なっていた場合に結合先にフックしない（※肘下の内側の線が食いこむ感じを出したい時用）
    eAPD_FLAG_STRIPE_FILL_FRONT_EDGE,       // 05:[sfF] ストライプ塗りの際、前方端を塗りつぶすか？（※足先等を塗らない時用）
    eAPD_FLAG_STRIPE_FILL_BACK_EDGE,        // 06:[sfB] ストライプ塗りの際、後方端を塗りつぶすか？（※足元等を塗らない時用）
    eAPD_FLAG_REVERSE_TOUCH_POINT,          // 07:[rvT] タッチ点の登録を逆に行うか？（※タッチ点の登録は根元から先端へ向けて行う）
    eAPD_FLAG_DOT_PUT_ON_FILL_OUT,          // 08:[onF] 直前の塗りで実際に出力された領域に対してストロークを描くか？（※後ろ髪に対するストローク描画時用）
    eAPD_FLAG_DOT_ERASE,                    // 09:[E] 消去ストロークか？
    eAPD_FLAG_JOINT_DIR_REVERSE_X,          // 10:[jrX] ジョイント時の方向線調整のX方向を反転
    eAPD_FLAG_JOINT_DIR_REVERSE_Y,          // 11:[jrY] ジョイント時の方向線調整のX方向を反転
    eAPD_FLAG_JOINT_DIR_SWAP,               // 12:[jdS] ジョイント時の方向線をスワップする（※INとOUTの入れ替え）

    eAPD_FLAG_MAX,		                    // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eAPD_FLAG_TEMP_SELECTED = 16,           // 編集対象として選択されている（※表示切り替え用）
    eAPD_FLAG_TEMP_ACTIVE,                  // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// アンカーポイントデータ
//----------------------------------------
class CAnchorPointData: public CListNode{
    // 各種実装
    IMPLEMENT_ADJUSTABLE_XY                             // 実装：調整XY座標
    IMPLEMENT_ADJUSTABLE_IN_XY                          // 実装：調整進入XY座標
    IMPLEMENT_ADJUSTABLE_OUT_XY                         // 実装：調整出立XY座標
    IMPLEMENT_NODE( CAnchorPointData )                  // 実装：リストノード
    IMPLEMENT_DATA( CAnchorPointData )                  // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eAPD_FLAG )                    // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eAPD_FLAG )                // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                               // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CAnchorPointData ) // 実装：クラスメソッドによるインスタンス確保／解放

public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                          // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CAdjustablePoint m_oAdjXY;            // [X]: 座標（※画像サイズに対する割合座標）
    //CAdjustablePoint m_oAdjInXY;          // [X]: 進入方向線座標（※アンカーポイントからの相対座標）
    //CAdjustablePoint m_oAdjOutXY;         // [X]: 出立方向線座標（※アンカーポイントからの相対座標）

    // IDの指定
    eJOINT_POINT m_eJointPointId;           // [enum]: 結合ポイントID（※[BDP]オーダーリスト経由でないと有効処理されない）
    eSTROKE_HOOK_TARGET m_eHookTargetId;    // [enum]: フック対象ID
    
    // ストローク強弱の指定
    int m_nStrokeSize;                      // [2]: ストローク：基本サイズ:[0〜300%]
    int m_nStrokeStartSize;                 // [2]: ストローク：開始サイズ:[0〜300%]（※開始範囲内で[StartSize〜StrokeSize]で変動）
    int m_nStrokeStartRange;                // [2]; ストローク：開始範囲:[0〜100%]（※開始サイズ適用範囲＝[0.0f〜StartRange]が範囲となる）
    int m_nStrokeEndSize;                   // [2]: ストローク：終了サイズ:[0〜300%]（※終了範囲内で[EndSize〜StrokeSize]で変動）
    int m_nStrokeEndRange;                  // [2]: ストローク：終了範囲:[0〜100%]（※終了サイズ適用範囲＝[(1.0-EndRange)〜1.0f]が範囲となる）
    int m_nStrokeEdgeFillSize;              // [2]: ストローク：エッジ塗りつぶしサイズ[0〜300%]（※基本サイズは[8*GM_P_RATE]）

    // タッチ＆フリル指定
    eSTROKE_TOUCH_TARGET m_eTouchTargetId;  // [enum]: タッチ：対象ID（※タッチ登録時／タッチ描画時の両方で指定）
    eSTROKE_FRILL m_eFrillId;               // [enum]: タッチ：フリルID（※フリル素材を指定）

    //------------------------------------------------------------------------------
    // タッチ詳細（※タッチ描画[CLineObjectData::eLOD_FLAG_TOUCHが立っている]時に参照される）
    //（※[eLOD_FLAG_TOUCH_IS_FRILL]が立っているとフリル向けの処理となる）
    //（※タッチ／フリルで値を共有するため値域は全て[0-10000]に限定している）
    //------------------------------------------------------------------------------
    // [2]: パラメータ０
    // 共通：基本位置:[0〜100%]（※描画を開始する起点：5000で中央開始）
    int m_nP00_TouchBasePos;
    
    // [2]: パラメータ１
    // 共通：基本サイズ:[0〜100%]（※基本描画サイズ：10000で100%）
    int m_nP01_TouchBaseSize;
    
    // [2]: パラメータ２
    // 共通：基本角度:[0〜10000]（※基本描画角度：１０倍精度の角度なので実質的には：0〜3599）
    int m_nP02_TouchBaseRot;
    
    // [2]: パラメータ３
    // 共通：前方（手前）エリア:[0〜100%]（※基本位置から手前の領域にかけてどこまで表示するかの割合：10000で前方の端まで）
    int m_nP03_TouchFrontArea;
    
    // [2]: パラメータ４
    // 共通：前方（手前）サイズ:[0〜100%]（※基本位置から手前の領域にかけての表示サイズ）
    int m_nP04_TouchFrontSize;

    // [2]: パラメータ５
    // 共通：前方（手前）回転:[0〜10000]（※基本位置から手前の領域にかけての角度：１０倍精度の角度なので実質的には：0〜3599）
    int m_nP05_TouchFrontRot;
    
    // [2]: パラメータ６
    // 共通：後方（奥）エリア:[0〜100%]（※基本位置から奥の領域にかけてどこまで表示するかの割合：10000で後方の端まで）
    int m_nP06_TouchBackArea;
    
    // [2]: パラメータ７
    // 共通：後方（奥）サイズ:[0〜100%]（※基本位置から奥の領域にかけての表示サイズ）
    int m_nP07_TouchBackSize;
    
    // [2]: パラメータ８
    // 共通：後方（奥）回転:[0〜10000]（※基本位置から奥の領域にかけての角度：１０倍精度の角度なので実質的には：0〜3599）
    int m_nP08_TouchBackRot;

    // [2]: パラメータ９
    // タッチ：前方への基本ステップ:[0〜100%]（※テクスチャのサイズに対する割合＆最低保証サイズ有り）
    // フリル：未使用
    int m_nP09_TouchFrontStep_FrillDummy;

    // [2]: パラメータ１０
    // タッチ：前方への一つおきのステップ:[0〜100%]（※二重線を引く場合の偶数描画時のステップ加算）
    // フリル：未使用
    int m_nP10_TouchFrontStep2_FrillDummy;

    // [2]: パラメータ１１
    // タッチ：前方へのランダムステップ:[0〜100%]
    // フリル：未使用
    int m_nP11_TouchFrontStepRnd_FrillDummy;

    // [2]: パラメータ１２
    // タッチ：後方への基本ステップ:[0〜100%]（※テクスチャのサイズに対する割合＆最低保証サイズ有り）
    // フリル：未使用
    int m_nP12_TouchBackStep_FrillDummy;
    
    // [2]: パラメータ１３
    // タッチ：後方への一つおきのステップ:[0〜100%]（※二重線を引く場合の偶数描画時のステップ加算）
    // フリル：未使用
    int m_nP13_TouchBackStep2_FrillDummy;
    
    // [2]: パラメータ１４
    // タッチ：後方へのランダムステップ:[0〜100%]
    // フリル：未使用
    int m_nP14_TouchBackStepRnd_FrillDummy;

    // ストライプ色の指定（※ストライプ指定をした際、領域を交互に塗りつぶす色）
    ePAL_OFS m_eStripeMainPalOfsId;         // [enum]: ストライプパレットオフセットID（※メイン色）
    ePAL_OFS m_eStripeSubPalOfsId;          // [enum]: ストライプパレットオフセットID（※サブ色）
    
    //--------------
    // ワーク
    //--------------
    bool m_bEditMenuForFrill;   // 編集メニュー設定をフリル向けにするか？（※メニュー設定メソッドの引数に渡せないので）
    
public:
    // コンストラクタ／デストラクタ
    CAnchorPointData( void );
    virtual ~CAnchorPointData( void ){}

    //----------------
    // 設定
    //----------------
    // 基本
    void set( int rX, int rY, int inRX, int inRY, int outRX, int outRY );

    // ID
    inline void setJointPointId( eJOINT_POINT jointPointId ){ m_eJointPointId = jointPointId; }
    inline void setHookTargetId( eSTROKE_HOOK_TARGET hookTargetId ){ m_eHookTargetId = hookTargetId; }

    // ストローク
    void setStrokeSize( int size, int startSize, int startRange, int endSize, int endRange );

    inline void setStrokeSize( int size ){ m_nStrokeSize = size; }
    inline void setStrokeStartSize( int size ){ m_nStrokeStartSize = size; }
    inline void setStrokeStartRange( int range ){ m_nStrokeStartRange = range; }
    inline void setStrokeEndSize( int size ){ m_nStrokeEndSize = size; }
    inline void setStrokeEndRange( int range ){ m_nStrokeEndRange = range; }
    inline void setStrokeEdgeFillSize( int size ){ m_nStrokeEdgeFillSize = size; }

    // タッチ＆フリル
    inline void setTouchTargetId( eSTROKE_TOUCH_TARGET touchTargetId ){ m_eTouchTargetId = touchTargetId; }
    inline void setFrillId( eSTROKE_FRILL frillId ){ m_eFrillId = frillId; }

    void setTouchBase( int pos, int size, int rot );
    void setTouchFront( int area, int size, int rot, int step, int step2, int stepRnd );
    void setTouchBack( int area, int size, int rot, int step, int step2, int stepRnd );

    void setFrillBase( int pos, int size, int rot );
    void setFrillFront( int area, int size, int rot );
    void setFrillBack( int area, int size, int rot );

    // ストライプ色
    inline void setStripeMainPalOfsId( ePAL_OFS palOfsId ){ m_eStripeMainPalOfsId = palOfsId; }
    inline void setStripeSubPalOfsId( ePAL_OFS palOfsId ){ m_eStripeSubPalOfsId = palOfsId; }
    
    // 編集メニュー設定をフリル向けにするか？（※メニュー設定メソッドの引数に渡せないので）
    inline void setEditMenuForFill( bool flag ){ m_bEditMenuForFrill = flag; }

	//----------------
	// 取得
	//----------------
    // ID
    eJOINT_POINT getJointPointId( int slotIndex=-1 );
    eSTROKE_HOOK_TARGET getHookTargetId( int slotIndex=-1 );

    // ストローク
    inline int getStrokeSize( void ){ return( m_nStrokeSize ); }
    inline int getStrokeStartSize( void ){ return( m_nStrokeStartSize ); }
    inline int getStrokeStartRange( void ){ return( m_nStrokeStartRange ); }
    inline int getStrokeEndSize( void ){ return( m_nStrokeEndSize ); }
    inline int getStrokeEndRange( void ){ return( m_nStrokeEndRange ); }
    inline int getStrokeEdgeFillSize( void ){ return( m_nStrokeEdgeFillSize );}

    // タッチ＆フリル
    eSTROKE_TOUCH_TARGET getTouchTargetId( int slotIndex=-1 );
    inline eSTROKE_FRILL getFrillId( void ){ return( m_eFrillId ); }

    // タッチ情報
    inline int getTouchBasePos( void ){ return( m_nP00_TouchBasePos ); }
    inline int getTouchBaseSize( void ){ return( m_nP01_TouchBaseSize ); }
    inline int getTouchBaseRot( void ){ return( m_nP02_TouchBaseRot ); }
    inline int getTouchFrontArea( void ){ return( m_nP03_TouchFrontArea ); }
    inline int getTouchFrontSize( void ){ return( m_nP04_TouchFrontSize ); }
    inline int getTouchFrontRot( void ){ return( m_nP05_TouchFrontRot ); }
    inline int getTouchBackArea( void ){ return( m_nP06_TouchBackArea ); }
    inline int getTouchBackSize( void ){ return( m_nP07_TouchBackSize ); }
    inline int getTouchBackRot( void ){ return( m_nP08_TouchBackRot ); }
    inline int getTouchFrontStep( void ){ return( m_nP09_TouchFrontStep_FrillDummy ); }
    inline int getTouchFrontStep2( void ){ return( m_nP10_TouchFrontStep2_FrillDummy ); }
    inline int getTouchFrontStepRnd( void ){ return( m_nP11_TouchFrontStepRnd_FrillDummy ); }
    inline int getTouchBackStep( void ){ return( m_nP12_TouchBackStep_FrillDummy ); }
    inline int getTouchBackStep2( void ){ return( m_nP13_TouchBackStep2_FrillDummy ); }
    inline int getTouchBackStepRnd( void ){ return( m_nP14_TouchBackStepRnd_FrillDummy ); }

    // フリル情報
    inline int getFrillBasePos( void ){ return( m_nP00_TouchBasePos ); }
    inline int getFrillBaseSize( void ){ return( m_nP01_TouchBaseSize ); }
    inline int getFrillBaseRot( void ){ return( m_nP02_TouchBaseRot ); }
    inline int getFrillFrontArea( void ){ return( m_nP03_TouchFrontArea ); }
    inline int getFrillFrontSize( void ){ return( m_nP04_TouchFrontSize ); }
    inline int getFrillFrontRot( void ){ return( m_nP05_TouchFrontRot ); }
    inline int getFrillBackArea( void ){ return( m_nP06_TouchBackArea ); }
    inline int getFrillBackSize( void ){ return( m_nP07_TouchBackSize ); }
    inline int getFrillBackRot( void ){ return( m_nP08_TouchBackRot ); }

    // ストライプ色
    inline ePAL_OFS getStripeMainPalOfsId( void ){ return( m_eStripeMainPalOfsId ); }
    inline ePAL_OFS getStripeSubPalOfsId( void ){ return( m_eStripeSubPalOfsId ); }

    //-------------------------------
    // 有効性の確認
    //-------------------------------
    bool isValid( void );

    //-------------------------------
    // [pPrev]と[pNext]の中間に配置
    //-------------------------------
    void setAtIntermediate( CAnchorPointData* pPrev, CAnchorPointData* pNext );
    
    //------------
    // 鏡面コピー
    //------------
    // ヨコ
    void fixForMirrorCopyH( void );
    void fixAsHeadForMirrorCopyH( void );
    void fixAsTailForMirrorCopyH( void );

    // タテ
    void fixForMirrorCopyV( void );
    void fixAsHeadForMirrorCopyV( void );
    void fixAsTailForMirrorCopyV( void );

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __ANCHOR_POINT_DATA_HPP__
