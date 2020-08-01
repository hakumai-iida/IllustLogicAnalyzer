/*+----------------------------------------------------------------+
  |	Title:		SlotAdjuster.hpp [共通環境]
  |	Comment:	スロット調整（※外部指定のデータによりスロットを調整）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SLOT_ADJUSTER_HPP__
#define __SLOT_ADJUSTER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sys/data/DataCell.hpp"

#include "JointPoint.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//==============================================================================
// スロット調整定義（※[CSlotAdjuster]をメンバーとしてもたせたいクラス定義内に記載）
//==============================================================================
#define IMPLEMENT_SLOT_ADJUSTER                                                 \
    protected:                                                                  \
        CSlotAdjuster m_oSlotAdj;                                               \
    public:                                                                     \
        inline CSlotAdjuster* getSlotAdjuster( void ){ return( &m_oSlotAdj ); }

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// スロット調整調整
//----------------------------------------
class CSlotAdjuster{
    // 各種実装
    IMPLEMENT_DATA( CSlotAdjuster )             // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_EDIT()                       // 実装：データ編集

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    // テンション関連
    int m_nMinRateForT;                     // [2]: テンション調整：最小オフセット（※角度を下げる指定）
    int m_nMaxRateForT;                     // [2]: テンション調整：最大オフセット（※角度をあげる指定）

    // サイズ関連
    int m_nMinRateForS;                     // [2]: サイズ調整：最小オフセット（※小さくする指定）
    int m_nMaxRateForS;                     // [2]: サイズ調整：最大オフセット（※大きくする指定）
    
    //----------------------------------------------------------------------
    // ジョイント関連（※スロットの中心を基準に、接続元の２点と接続先の２点をつなげる仕組み）
    // 半径が腕や足の太さに相当する値なのでＨ調整値に影響される（※Ｖ調整値には影響されない）
    //----------------------------------------------------------------------
    // 座標登録先（※IDが有効な場合に処理される）
    eJOINT_POINT m_eJointPointIdForFrom;    // [enum]: 接続元（呼び出し元）に対する座標登録ID
    eJOINT_POINT m_eJointPointIdForTo;      // [enum]: 接続先（呼び出し先）に対する座標登録ID
    
    int m_nJointRateRotR;                   // [2]: 可動半径（※接続する二点間の幅）
    int m_nJointMinRateOfsRForH;            // [2]: ヨコ調整：最小オフセット（※半径を細くする指定）
    int m_nJointMaxRateOfsRForH;            // [2]: ヨコ調整：最大オフセット（※半径を太くする指定）
    int m_nJointRateRotOfs;                 // [2]: 基本角度（この角度を中心に前後９０度の二点を[TargetIdForFrom/To]へ登録する）
    int m_nJointRateOfsForRotCenter;        // [2]: 回転時にずらす中心割合オフセット（※円の中心から見て、[+]で上、[-]で下に中心がずれる）
       
    int m_nJointRateDirForFrom;             // [2]: 方向線調整：呼び出し元の強さ
    int m_nJointRateDirForTo;               // [2]: 方向線調整：呼び出し先の強さ
    int m_nJointRateDirForOpen;             // [2]: 方向線調整：二点間が開いた際の方向線の強さ
    int m_nJointRateDirForClose;            // [2]: 方向線調整：二点間が閉じた際の方向線の強さ

public:
    // コンストラクタ／デストラクタ
    CSlotAdjuster( void );
    virtual ~CSlotAdjuster( void ){}

    //----------------
    // 設定
    //----------------
    // for T
    inline void setMinRateForT( int rate ){ m_nMinRateForT = rate; }
    inline void setMaxRateForT( int rate ){ m_nMaxRateForT = rate; }

    // for S
    inline void setMinRateForS( int rate ){ m_nMinRateForS = rate; }
    inline void setMaxRateForS( int rate ){ m_nMaxRateForS = rate; }

    // joint
    inline void setJointPointIdForFrom( eJOINT_POINT targetId ){ m_eJointPointIdForFrom = targetId; }
    inline void setJointPointIdForTo( eJOINT_POINT targetId ){ m_eJointPointIdForTo = targetId; }

    inline void setJointRateRotR( int JointRateRotR ){ m_nJointRateRotR = JointRateRotR; }
    inline void setJointMinRateOfsRForH( int rateOfsR ){ m_nJointMinRateOfsRForH = rateOfsR; }
    inline void setJointMaxRateOfsRForH( int rateOfsR ){ m_nJointMaxRateOfsRForH = rateOfsR; }
    inline void setJointRateRotOfs( int JointRateRotOfs ){ m_nJointRateRotOfs = JointRateRotOfs; }
    inline void setJointRateOfsForRotCenter( int JointRateOfsForRotCenter ){ m_nJointRateOfsForRotCenter = JointRateOfsForRotCenter; }

    inline void setJointRateDirForFrom( int rate ){ m_nJointRateDirForClose = rate; }
    inline void setJointRateDirForTo( int rate ){ m_nJointRateDirForTo = rate; }
    inline void setJointRateDirForOpen( int rate ){ m_nJointRateDirForOpen = rate; }
    inline void setJointRateDirForClose( int rate ){ m_nJointRateDirForClose = rate; }

    //----------------
	// 取得
	//----------------
    // for T
    inline int getMinRateForT( void ){ return( m_nMinRateForT ); }
    inline int getMaxRateForT( void ){ return( m_nMaxRateForT ); }

    // for S
    inline int getMinRateForS( void ){ return( m_nMinRateForS ); }
    inline int getMaxRateForS( void ){ return( m_nMaxRateForS ); }

    // joint
    eJOINT_POINT getJointPointIdForFrom( int slotIndex=-1 );
    eJOINT_POINT getJointPointIdForTo( int slotIndex=-1 );

    inline int getJointRateRotR( void ){ return( m_nJointRateRotR ); }
    inline int getJointMinRateOfsRForH( void ){ return( m_nJointMinRateOfsRForH ); }
    inline int getJointMaxRateOfsRForH( void ){ return( m_nJointMaxRateOfsRForH ); }
    inline int getJointRateRotOfs( void ){ return( m_nJointRateRotOfs ); }
    inline int getJointRateOfsForRotCenter( void ){ return( m_nJointRateOfsForRotCenter ); }

    inline int getJointRateDirForFrom( void ){ return( m_nJointRateDirForFrom ); }
    inline int getJointRateDirForTo( void ){ return( m_nJointRateDirForTo ); }
    inline int getJointRateDirForOpen( void ){ return( m_nJointRateDirForOpen ); }
    inline int getJointRateDirForClose( void ){ return( m_nJointRateDirForClose ); }
    
    inline float getJointDirRateForFrom( void ){ return( CConst::ConvBezierRateScale( m_nJointRateDirForFrom ) ); }
    inline float getJointDirRateForTo( void ){ return( CConst::ConvBezierRateScale( m_nJointRateDirForTo ) ); }
    inline float getJointDirRateForOpen( void ){ return( CConst::ConvBezierRateScale( m_nJointRateDirForOpen ) ); }
    inline float getJointDirRateForClose( void ){ return( CConst::ConvBezierRateScale( m_nJointRateDirForClose ) ); }

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
#endif	// __SLOT_ADJUSTER_HPP__
