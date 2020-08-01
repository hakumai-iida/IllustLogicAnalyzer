/*+----------------------------------------------------------------+
  |	Title:		Spring.hpp [共通環境]
  |	Comment:	バネの移動系
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SPRING_HPP__
#define __SPRING_HPP__

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
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// Spring
//-----------------------------------------------------------------
class CSpring{
protected:
	// 設定値
	float m_fLength;		// バネの長さ（※振幅の距離）
	float m_fStiffness;		// バネの強さ（※バネの伸びが逆向きの速度になる強さ：1.0fで100%反発）
	float m_fDamping;		// バネの減衰（※速度を維持する強さ：1.0fで100%維持＝減衰しない）
    float m_fRotation;      // バネの向き

	bool  m_bAutoStop;		// 自動で停止するか？
	float m_fBorderDist;	// 座標の判定値（※絶対値がこれ未満になったら収束とする）
	float m_fBorderV;		// 速度の判定値（※絶対値がこれ未満になったら収束とする）
    
	// ワーク
    bool m_bMoving;         // 移動中か？
    bool m_bUpdateSkip;     // 更新をスキップするか？
	float m_fPos;			// 座標
	float m_fV;				// 速度

public:
	// コンストラクタ＆デストラクタ（※基本的に何もしない）
	CSpring( void );
	virtual ~CSpring( void ){}

	// クリア（※初期化用）
    void clearSpring( void );

	// 設定
	void set( float len, float stiff, float damp, float rot=0.0f );
    
    inline void setLength( float len ){ m_fLength = len; }
    inline void setStiffness( float stiff ){ m_fStiffness = stiff; }
    inline void setDamping( float damp ){ m_fDamping = damp; }
    inline void setRotation( float rot ){ m_fRotation = rot; }
    
	inline void setAutoStop( bool flag ){ m_bAutoStop = flag; }
	inline void setBorderV( float borderV ){ m_fBorderV = borderV; }
	inline void setBorderDist( float borderDist ){ m_fBorderDist = borderDist; }
    
	inline void setPos( float pos ){ m_fPos = pos; }
	inline void setV( float v ){ m_fV = v; }

	// 開始／停止
    void start( void );
    void stop( bool isPosReset=false );

	// 判定
	bool isMoving( void ){ return( m_bMoving ); }
	bool isConvergenced( void );

	// 差分取得（※中央[m_fLength]を０とした時の位置）
	float getPosDelta( void );
    
    // 差分に対して回転を適用した値
    float getX( void );
    float getY( void );
    
    // 速度合成（※現在の状況に指定の速度を加える）
    void addV( float vX, float vY );

	// 更新
	void updateWithFrame( float deltaFrame );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __SPRING_HPP__
