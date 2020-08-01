/*+----------------------------------------------------------------+
  |	Title:		JointPoint.hpp [共通環境]
  |	Comment:	結合点（※ベジェ描画時のワーク）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __JOINT_POINT_HPP__
#define __JOINT_POINT_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------------------------------------------------------------------------------
// 結合対象
//-------------------------------------------------------------------------------------------------
// ・ジョイントとなる[BmpDot]にて接続元と接続先の情報を登録しておき、その値を別の[BmpDot]のAPから参照するための枠組み
//（※腕などの幅の概念をもつパーツの二点を円で定義し、ジョイントの接続元と接続先から参照できるようにラベルを指定する）
//（※オーダー段階で大まかな座標が登録された後、描画AP変換時に相互参照が行われ、システム[HOOK]により座標が同期される）
//-------------------------------------------------------------------------------------------------
// ・データは[接続元の左側の点、それに対応する接続先の点]と[接続元の右側の点、それに対応する接続先の点]で１セットとなる
//（※[接続先の値]ラベルから[１]を引いた値が、[接続元の値]ラベルとなる）
//（※[接続元の値]ラベルに[２]を足した値が、二点間の対になる値となる＝ある領域の左側の点に対する右側の点等）
//（※スロットイインデックスの値に対して[４]を足した値が、逆側の値となる＝左腕に対する右腕等）
//--------------------------------------------------------------------------------------------------
enum eJOINT_POINT{
    eJOINT_POINT_INVALID = -1,  // 無効値
    
    //--------------------------------------------------
    // 一時枠（※確認／検証用）
    //--------------------------------------------------
    eJOINT_POINT_TEMP_A_0_FROM,     eJOINT_POINT_TEMP_A_0_TO,
    eJOINT_POINT_TEMP_A_1_FROM,     eJOINT_POINT_TEMP_A_1_TO,

    eJOINT_POINT_TEMP_B_0_FROM,     eJOINT_POINT_TEMP_B_0_TO,
    eJOINT_POINT_TEMP_B_1_FROM,     eJOINT_POINT_TEMP_B_1_TO,

    eJOINT_POINT_TEMP_C_0_FROM,     eJOINT_POINT_TEMP_C_0_TO,
    eJOINT_POINT_TEMP_C_1_FROM,     eJOINT_POINT_TEMP_C_1_TO,

    eJOINT_POINT_TEMP_D_0_FROM,     eJOINT_POINT_TEMP_D_0_TO,
    eJOINT_POINT_TEMP_D_1_FROM,     eJOINT_POINT_TEMP_D_1_TO,

    //--------------------------------------------------
    // 用途別固定枠
    //--------------------------------------------------
    // [UP_BODY]：腹から上半身へのつなぎ（※[BlJointUpBody]で設定され[From:BlBase]、[To:UpBase]となる）
    eJOINT_POINT_UP_BODY_0_FROM,    eJOINT_POINT_UP_BODY_0_TO,
    eJOINT_POINT_UP_BODY_1_FROM,    eJOINT_POINT_UP_BODY_1_TO,

    // [LOW_BODY]：腹から下半身へのつなぎ（※[BlJointLowBody]で設定され[From:BlBase]、[To:LwBase]となる）
    eJOINT_POINT_LOW_BODY_0_FROM,   eJOINT_POINT_LOW_BODY_0_TO,
    eJOINT_POINT_LOW_BODY_1_FROM,   eJOINT_POINT_LOW_BODY_1_TO,

    // [NECK]：上半身から首へのつなぎ（※[BrJointNeck]で設定され[From:UpBase]、[To:NkBase]となる）
    eJOINT_POINT_NECK_0_FROM,       eJOINT_POINT_NECK_0_TO,
    eJOINT_POINT_NECK_1_FROM,       eJOINT_POINT_NECK_1_TO,

    // [SHOLDER]：上半身から腕へのつなぎ（※[BrJointSholder]で設定され[From:UpBase]、[To:AmBase]となる）
    eJOINT_POINT_SHOLDER_L0_FROM,   eJOINT_POINT_SHOLDER_L0_TO,
    eJOINT_POINT_SHOLDER_L1_FROM,   eJOINT_POINT_SHOLDER_L1_TO,
    
    //（※腕は左右があるので２セットもつ）
    eJOINT_POINT_SHOLDER_R0_FROM,   eJOINT_POINT_SHOLDER_R0_TO,
    eJOINT_POINT_SHOLDER_R1_FROM,   eJOINT_POINT_SHOLDER_R1_TO,

    // [ELBOW]：腕から肘下へのつなぎ（※[AmJointElbow]で設定され[From:AmBase]、[To:ElBase]となる）
    eJOINT_POINT_ELBOW_L0_FROM,    eJOINT_POINT_ELBOW_L0_TO,
    eJOINT_POINT_ELBOW_L1_FROM,    eJOINT_POINT_ELBOW_L1_TO,
    
    //（※肘下は左右があるので２セットもつ）
    eJOINT_POINT_ELBOW_R0_FROM,    eJOINT_POINT_ELBOW_R0_TO,
    eJOINT_POINT_ELBOW_R1_FROM,    eJOINT_POINT_ELBOW_R1_TO,

    // [WRIST]：肘下から手へのつなぎ（※[ElJointWrist]で設定され[From:ElBase]、[To:HnBase]となる）
    eJOINT_POINT_WRIST_L0_FROM,    eJOINT_POINT_WRIST_L0_TO,
    eJOINT_POINT_WRIST_L1_FROM,    eJOINT_POINT_WRIST_L1_TO,

    //（※手は左右があるので２セットもつ）
    eJOINT_POINT_WRIST_R0_FROM,    eJOINT_POINT_WRIST_R0_TO,
    eJOINT_POINT_WRIST_R1_FROM,    eJOINT_POINT_WRIST_R1_TO,

    // [LEG]：下半身から脚へのつなぎ（※[LwJointLeg]で設定され[From:LwBase]、[To:LgBase]となる）
    eJOINT_POINT_LEG_L0_FROM,       eJOINT_POINT_LEG_L0_TO,
    eJOINT_POINT_LEG_L1_FROM,       eJOINT_POINT_LEG_L1_TO,
    
    //（※脚は左右があるので２セットもつ）
    eJOINT_POINT_LEG_R0_FROM,       eJOINT_POINT_LEG_R0_TO,
    eJOINT_POINT_LEG_R1_FROM,       eJOINT_POINT_LEG_R1_TO,

    // [NNEE]：脚から膝下へのつなぎ（※[LgJointKnee]で設定され[From:LgBase]、[To:KnBase]となる）
    eJOINT_POINT_KNEE_L0_FROM,      eJOINT_POINT_KNEE_L0_TO,
    eJOINT_POINT_KNEE_L1_FROM,      eJOINT_POINT_KNEE_L1_TO,
    
    //（※膝下は左右があるので２セットもつ）
    eJOINT_POINT_KNEE_R0_FROM,      eJOINT_POINT_KNEE_R0_TO,
    eJOINT_POINT_KNEE_R1_FROM,      eJOINT_POINT_KNEE_R1_TO,

    // [ANKLE]：膝下から足先へのつなぎ（※[LgJointAnkle]で設定され[From:KnBase]、[To:FoBase]となる）
    eJOINT_POINT_ANKLE_L0_FROM,    eJOINT_POINT_ANKLE_L0_TO,
    eJOINT_POINT_ANKLE_L1_FROM,    eJOINT_POINT_ANKLE_L1_TO,

    //（※足先は左右があるので２セットもつ）
    eJOINT_POINT_ANKLE_R0_FROM,    eJOINT_POINT_ANKLE_R0_TO,
    eJOINT_POINT_ANKLE_R1_FROM,    eJOINT_POINT_ANKLE_R1_TO,
    
    eJOINT_POINT_MAX,
};

// ジョイントインデックスに対するオフセット
#define JOINT_POINT_OFS_FOR_FROM_TO         1
#define JOINT_POINT_OFS_FOR_OTHER_SIDE      2
#define JOINT_POINT_OFS_FOR_SLOT_INDEX      4

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//---------------------------------------------
// 登録情報の登録単位（※オーダー解釈時の情報を保持する）
//---------------------------------------------
typedef struct{
    bool  bValid;                   // 有効か？
    
    bool  bTo;                      // 接続先[TO]か？
    bool  bFlipH, bFlipV;           // 横／縦に反転しているか？

    int   nRateX, nRateY;           // 割合座標（※結合用＝角度調整後の座標）
    int   nRateX0, nRateY0;         // 割合座標（※元々の値＝角度調整前の座標）
    int   nRateOfsX, nRateOfsY;     // 割合座標（※表示のオフセット）
    int   dirRateX, dirRateY;       // 方向線調整値

    float fScaleAbs;                // 表示スケール（※絶対値＝符号は[bFlipX/bFlipV]で判断）
    float fRot0;                    // 表示角度（※重なり判定のために使う調整前の値）

    float openRateForDir;           // 結合部分が開く際の方向線割合
    float closeRateForDir;          // 結合部分が閉じる際の方向線割合
} stJOINT_POINT_CELL;

// クリア
#define CLEAR_JOINT_POINT_CELL( _pCell )                        \
    memset( (_pCell), 0, sizeof(stJOINT_POINT_CELL) );          \
    (_pCell)->fScaleAbs = 1.0f

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 結合点
//----------------------------------------
class CJointPoint{
protected:
    // 登録データ
    stJOINT_POINT_CELL m_stArrCell[eJOINT_POINT_MAX];

public:
    // コンストラクタ／デストラクタ
    CJointPoint( void );
    virtual ~CJointPoint( void ){}
    
    // クリア
    void clear( void );

    // 登録情報の取得（※無効時は[NULL]を返す)
    stJOINT_POINT_CELL* getJointPoint( eJOINT_POINT id );

    //-----------------------------
    // [FROM]向けの登録
    //-----------------------------
    inline void registJointPointFrom( eJOINT_POINT id, int rateX, int rateY, int rateX0, int rateY0, int ofsRX, int ofsRY,
                                      float scaleAbs, float rot0, bool isFlipH, bool isFlipV ){
        registJointPoint( id, rateX, rateY, rateX0, rateY0, ofsRX, ofsRY, scaleAbs, rot0, isFlipH, isFlipV, false );
    }

    //-----------------------------
    // [TO]向けの登録
    //-----------------------------
    inline void registJointPointTo( eJOINT_POINT id, int rateX, int rateY, int rateX0, int rateY0, int ofsRX, int ofsRY,
                                    float scaleAbs, float rot0, bool isFlipH, bool isFlipV ){
        registJointPoint( id, rateX, rateY, rateX0, rateY0, ofsRX, ofsRY, scaleAbs, rot0, isFlipH, isFlipV, true );
    }
    
    //---------------------------------
    // 方向線調整値の設定（※[FROM/TO]で共通
    //---------------------------------
    void setDirRateXY( eJOINT_POINT id, int dirRX, int dirRY, float openRate, float closeRate );
    
protected:
    // 座標の登録（実体）
    void registJointPoint( eJOINT_POINT id, int rateX, int rateY, int rateX0, int rateY0, int ofsRX, int ofsRY,
                           float scaleAbs, float rot, bool isFlipH, bool isFlipV, bool isTo );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelJointPoint[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __JOINT_POINT_HPP__
