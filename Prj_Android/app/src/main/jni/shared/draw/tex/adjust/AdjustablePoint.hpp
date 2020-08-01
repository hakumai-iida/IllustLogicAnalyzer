/*+----------------------------------------------------------------+
  |	Title:		AdjustablePoint.hpp [共通環境]
  |	Comment:	調整可能ポイント（※外部指定のデータにより調整可能な座標）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ADJUSTABLE_POINT_HPP__
#define __ADJUSTABLE_POINT_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sys/data/DataCell.hpp"
#include "draw/tex/adjust/DelayLog.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//===========================================================================================
// XY調整値定義（※[CAdjustablePoint]をメンバーとしてもたせたいクラス定義内に記載）
//===========================================================================================
// 基本XY座標
#define IMPLEMENT_ADJUSTABLE_XY                                                             \
    protected:                                                                              \
        CAdjustablePoint m_oAdjXY;                                                          \
    public:                                                                                 \
        inline void setAdjustXYForH( int minRX, int minRY, int maxRX, int maxRY ){          \
            m_oAdjXY.setOfsMinRateXYForH( minRX, minRY );                                   \
            m_oAdjXY.setOfsMaxRateXYForH( maxRX, maxRY );                                   \
        }                                                                                   \
        inline void setAdjustXYForV( int minRX, int minRY, int maxRX, int maxRY ){          \
            m_oAdjXY.setOfsMinRateXYForV( minRX, minRY );                                   \
            m_oAdjXY.setOfsMaxRateXYForV( maxRX, maxRY );                                   \
        }                                                                                   \
        inline void setAdjustXYDelay( eDELAY_LOG type, int powX, int powY ){                \
            m_oAdjXY.setDelayType( type );                                                  \
            m_oAdjXY.setDelayPowerRate( powX, powY );                                       \
        }                                                                                   \
        inline CAdjustablePoint* getAdjXY( void ){ return( &m_oAdjXY ); }

// 進入XY座標
#define IMPLEMENT_ADJUSTABLE_IN_XY                                                          \
    protected:                                                                              \
        CAdjustablePoint m_oAdjInXY;                                                        \
    public:                                                                                 \
        inline void setAdjustInXYForH( int minRX, int minRY, int maxRX, int maxRY ){        \
            m_oAdjInXY.setOfsMinRateXYForH( minRX, minRY );                                 \
            m_oAdjInXY.setOfsMaxRateXYForH( maxRX, maxRY );                                 \
        }                                                                                   \
        inline void setAdjustInXYForV( int minRX, int minRY, int maxRX, int maxRY ){        \
            m_oAdjInXY.setOfsMinRateXYForV( minRX, minRY );                                 \
            m_oAdjInXY.setOfsMaxRateXYForV( maxRX, maxRY );                                 \
        }                                                                                   \
        inline void setAdjustInXYDelay( eDELAY_LOG type, int powX, int powY ){              \
            m_oAdjInXY.setDelayType( type );                                                \
            m_oAdjInXY.setDelayPowerRate( powX, powY );                                     \
        }                                                                                   \
        inline CAdjustablePoint* getAdjInXY( void ){ return( &m_oAdjInXY ); }

// 出立XY座標
#define IMPLEMENT_ADJUSTABLE_OUT_XY                                                         \
    protected:                                                                              \
        CAdjustablePoint m_oAdjOutXY;                                                       \
    public:                                                                                 \
        inline void setAdjustOutXYForH( int minRX, int minRY, int maxRX, int maxRY ){       \
            m_oAdjOutXY.setOfsMinRateXYForH( minRX, minRY );                                \
            m_oAdjOutXY.setOfsMaxRateXYForH( maxRX, maxRY );                                \
        }                                                                                   \
        inline void setAdjustOutXYForV( int minRX, int minRY, int maxRX, int maxRY ){       \
            m_oAdjOutXY.setOfsMinRateXYForV( minRX, minRY );                                \
            m_oAdjOutXY.setOfsMaxRateXYForV( maxRX, maxRY );                                \
        }                                                                                   \
        inline void setAdjustOutXYDelay( eDELAY_LOG type, int powX, int powY ){             \
            m_oAdjOutXY.setDelayType( type );                                               \
            m_oAdjOutXY.setDelayPowerRate( powX, powY );                                    \
        }                                                                                   \
        inline CAdjustablePoint* getAdjOutXY( void ){ return( &m_oAdjOutXY ); }

//===========================================================================================
// 一時しのぎ調整値定義（※保持するベジェデータに対して一時的なオフセット＆拡縮したいクラス定義内に記載）
//（※[AdjustablePoint]で行われる傾向値等の調整が生じない値であることに注意）
//（※この値はデータとして保存されない：一時調整値をデータに反映させたい場合は[applyTempAdjust]により行う）
//===========================================================================================
#define IMPLEMENT_TEMP_ADJUST                                                               \
    protected:                                                                              \
        int m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY;                                   \
        int m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY;                               \
    public:                                                                                 \
        inline void clearTempAdjust( void ){                                                \
            m_nTempAdjustRateOfsX = 0;                                                      \
            m_nTempAdjustRateOfsY = 0;                                                      \
            m_nTempAdjustRateScaleX = BEZIER_SCALE_RATE;                                    \
            m_nTempAdjustRateScaleY = BEZIER_SCALE_RATE;                                    \
        }                                                                                   \
        inline void setTempAdjust( int oRX, int oRY, int sRX, int sRY ){                    \
            m_nTempAdjustRateOfsX = oRX;                                                    \
            m_nTempAdjustRateOfsY = oRY;                                                    \
            m_nTempAdjustRateScaleX = sRX;                                                  \
            m_nTempAdjustRateScaleY = sRY;                                                  \
        }                                                                                   \
        inline bool isTempAdjustValid( void ){                                              \
            if( m_nTempAdjustRateOfsX != 0 ){ return( true ); }                             \
            if( m_nTempAdjustRateOfsY != 0 ){ return( true ); }                             \
            if( m_nTempAdjustRateScaleX != BEZIER_SCALE_RATE ){ return( true ); }           \
            if( m_nTempAdjustRateScaleY != BEZIER_SCALE_RATE ){ return( true ); }           \
            return( false );                                                                \
        }                                                                                   \
        inline int getTempAdjustRateOfsX( void ){ return( m_nTempAdjustRateOfsX ); }        \
        inline int getTempAdjustRateOfsY( void ){ return( m_nTempAdjustRateOfsY ); }        \
        inline int getTempAdjustRateScaleX( void ){ return( m_nTempAdjustRateScaleX ); }    \
        inline int getTempAdjustRateScaleY( void ){ return( m_nTempAdjustRateScaleY ); }    \
        void applyTempAdjust( void );                                                       \
        void applyTempAdjustForMaxH( void );                                                \
        void applyTempAdjustForMinH( void );                                                \
        void applyTempAdjustForMaxV( void );                                                \
        void applyTempAdjustForMinV( void );

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 調整可能ポイント
//----------------------------------------
class CAdjustablePoint{
    // 各種実装
    IMPLEMENT_DATA( CAdjustablePoint )               // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_EDIT()                            // 実装：データ編集

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    // 基本座標
    int m_nRateX, m_nRateY;                             // [2x2]: 割合座標（※画像中心からの座標）

    // ヨコ調整：[-1.0f:min=細い]〜[0.0f:基本]〜[+1.0f:max=太い]（※痩せている／太っている的な調整用）
    int m_nOfsMinRateXForH, m_nOfsMinRateYForH;         // [2x2]: ヨコ：最小オフセット
    int m_nOfsMaxRateXForH, m_nOfsMaxRateYForH;         // [2x2]: ヨコ：最大オフセット

    // タテ調整：[-1.0f:min=低い]〜[0.0f:基本]〜[+1.0f:max=高い]（※背が低い／背が高い的な調整用）
    int m_nOfsMinRateXForV, m_nOfsMinRateYForV;         // [2x2]: タテ：最小オフセット
    int m_nOfsMaxRateXForV, m_nOfsMaxRateYForV;         // [2x2]: タテ：最大オフセット

    // 遅延調整：動きに遅れて着いてくるイメージのオフセット値（※慣性的な調整用）
    eDELAY_LOG m_eDelayType;                            // [enum]: 遅延タイプ（※遅延配列への参照先）
    int m_nDelayDepth;                                  // [2]: 遅延ログ震度
    int m_nDelayPowerRateX, m_nDelayPowerRateY;         // [2x2]: 遅延の強さ（※移動値への係数）

public:
    // コンストラクタ／デストラクタ
    CAdjustablePoint( void );
    virtual ~CAdjustablePoint( void ){}

    //----------------
    // 設定
    //----------------
    inline void setRateXY( int rX, int rY ){ m_nRateX = rX; m_nRateY = rY; }
    
    inline void setOfsMinRateXYForH( int ofsRX, int ofsRY ){ m_nOfsMinRateXForH = ofsRX; m_nOfsMinRateYForH = ofsRY; }
    inline void setOfsMaxRateXYForH( int ofsRX, int ofsRY ){ m_nOfsMaxRateXForH = ofsRX; m_nOfsMaxRateYForH = ofsRY; }

    inline void setOfsMinRateXYForV( int ofsRX, int ofsRY ){ m_nOfsMinRateXForV = ofsRX; m_nOfsMinRateYForV = ofsRY; }
    inline void setOfsMaxRateXYForV( int ofsRX, int ofsRY ){ m_nOfsMaxRateXForV = ofsRX; m_nOfsMaxRateYForV = ofsRY; }
    
    inline void setDelayType( eDELAY_LOG type, int depth=0 ){ m_eDelayType = type; m_nDelayDepth = depth; }
    inline void setDelayPowerRate( int rateX, int rateY ){ m_nDelayPowerRateX = rateX; m_nDelayPowerRateY = rateY; }

	//----------------
	// 取得
	//----------------
    inline int getRateX( void ){ return( m_nRateX ); }
    inline int getRateY( void ){ return( m_nRateY ); }
    
    inline int getOfsMinRateXForH( void ){ return( m_nOfsMinRateXForH ); }
    inline int getOfsMinRateYForH( void ){ return( m_nOfsMinRateYForH ); }
    inline int getOfsMaxRateXForH( void ){ return( m_nOfsMaxRateXForH ); }
    inline int getOfsMaxRateYForH( void ){ return( m_nOfsMaxRateYForH ); }

    inline int getOfsMinRateXForV( void ){ return( m_nOfsMinRateXForV ); }
    inline int getOfsMinRateYForV( void ){ return( m_nOfsMinRateYForV ); }
    inline int getOfsMaxRateXForV( void ){ return( m_nOfsMaxRateXForV ); }
    inline int getOfsMaxRateYForV( void ){ return( m_nOfsMaxRateYForV ); }
    
    inline eDELAY_LOG getDelayType( void ){ return( m_eDelayType ); }
    inline int getDelayDepth( void ){ return( m_nDelayDepth ); }
    inline int getDelayPowerRateX( void ){ return( m_nDelayPowerRateX ); }
    inline int getDelayPowerRateY( void ){ return( m_nDelayPowerRateY ); }

    //---------------------------------------
    // 増減（※エディタ用）
    //---------------------------------------
    void addRateXY( int addX, int addY );
    void addMinRateXYForH( int addX, int addY );
    void addMaxRateXYForH( int addX, int addY );
    void addMinRateXYForV( int addX, int addY );
    void addMaxRateXYForV( int addX, int addY );
    
    //---------------------------------------
    // 調整値適用（※一時調整の値を取り込む）
    //---------------------------------------
    void applyOfs( int rateOfsX, int rateOfsY );
    void applyScale( int rateScaleX, int rateScaleY );
    void applyScaleThenOfs( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY );
    
    void applyOfsForVal( int rateOfsX, int rateOfsY, int* pX, int* pY );
    void applyScaleForVal( int rateScaleX, int rateScaleY, int* pX, int* pY );
    void applyScaleThenOfsForVal( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY, int* pX, int* pY );

    // for MinH
    inline void applyOfsForMinH( int rateOfsX, int rateOfsY ){
        applyOfsForVal( rateOfsX, rateOfsY, &m_nOfsMinRateXForH, &m_nOfsMinRateYForH );
    }
    inline void applyScaleForMinH( int rateScaleX, int rateScaleY ){
        applyScaleForVal( rateScaleX, rateScaleY, &m_nOfsMinRateXForH, &m_nOfsMinRateYForH );
    }
    inline void applyScaleThenOfsForMinH( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY ){
        applyScaleThenOfsForVal( rateScaleX, rateScaleY, rateOfsX, rateOfsY, &m_nOfsMinRateXForH, &m_nOfsMinRateYForH );
    }

    // for MaxH
    inline void applyOfsForMaxH( int rateOfsX, int rateOfsY ){
        applyOfsForVal( rateOfsX, rateOfsY, &m_nOfsMaxRateXForH, &m_nOfsMaxRateYForH );
    }
    inline void applyScaleForMaxH( int rateScaleX, int rateScaleY ){
        applyScaleForVal( rateScaleX, rateScaleY, &m_nOfsMaxRateXForH, &m_nOfsMaxRateYForH );
    }
    inline void applyScaleThenOfsForMaxH( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY ){
        applyScaleThenOfsForVal( rateScaleX, rateScaleY, rateOfsX, rateOfsY, &m_nOfsMaxRateXForH, &m_nOfsMaxRateYForH );
    }

    // for MinV
    void applyOfsForMinV( int rateOfsX, int rateOfsY ){
        applyOfsForVal( rateOfsX, rateOfsY, &m_nOfsMinRateXForV, &m_nOfsMinRateYForV );
    }
    void applyScaleForMinV( int rateScaleX, int rateScaleY ){
        applyScaleForVal( rateScaleX, rateScaleY, &m_nOfsMinRateXForV, &m_nOfsMinRateYForV );
    }
    void applyScaleThenOfsForMinV( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY ){
        applyScaleThenOfsForVal( rateScaleX, rateScaleY, rateOfsX, rateOfsY, &m_nOfsMinRateXForV, &m_nOfsMinRateYForV );
    }
    
    // for MaxV
    void applyOfsForMaxV( int rateOfsX, int rateOfsY ){
        applyOfsForVal( rateOfsX, rateOfsY, &m_nOfsMaxRateXForV, &m_nOfsMaxRateYForV );
    }
    void applyScaleForMaxV( int rateScaleX, int rateScaleY ){
        applyScaleForVal( rateScaleX, rateScaleY, &m_nOfsMaxRateXForV, &m_nOfsMaxRateYForV );
    }
    void applyScaleThenOfsForMaxV( int rateScaleX, int rateScaleY, int rateOfsX, int rateOfsY ){
        applyScaleThenOfsForVal( rateScaleX, rateScaleY, rateOfsX, rateOfsY, &m_nOfsMaxRateXForV, &m_nOfsMaxRateYForV );
    }

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
#endif	// __ADJUSTABLE_POINT_HPP__
