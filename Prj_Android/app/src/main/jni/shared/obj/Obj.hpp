/*+----------------------------------------------------------------+
  |	Title:		Obj.hpp [共通環境]
  |	Comment:	ゲーム要素基底（※更新と描画を行う要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __OBJ_HPP__
#define __OBJ_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Updater.hpp"
#include "Drawer.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 状態メソッド実装（※[.hpp]に記載）
#define IMPLEMENT_OBJ_STATUS( _STAT_LABEL )                 \
    public:                                                 \
        inline bool setStatus( _STAT_LABEL status ){        \
            return( CObj::setStatus( status ) );            \
        }                                                   \
        inline bool checkStatus( _STAT_LABEL status ){      \
            return( CObj::checkStatus( status ) );          \
        }

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// ゲーム要素
// ・位置と状態を持つ（※移動したければ[CMover]等で行う）
// ・更新をする
// ・描画をする
//-----------------------------------------------------------------
// ループ処理の中での[CObj]動作の流れ
//  1.ループのステップ切り替えにより[CObj]の状態を設定
//  3.ループの更新後に[CObj]の更新処理
//-----------------------------------------------------------------
class CObj: public CUpdater, public CDrawer{
	//-------------------------
	// 管理データ
	//-------------------------
protected:
	// 地震(一律振動)
	static float s_fQuakeX;			// Ｘ揺れ
	static float s_fQuakeY;			// Ｙ揺れ

public:
	static void ResetQuake( void ){ s_fQuakeX = s_fQuakeY = 0.0f; }
	static void SetQuakeX( float x ){ s_fQuakeX = x; }
	static void SetQuakeY( float y ){ s_fQuakeY = y; }

	//-------------------------
	// メンバー
	//-------------------------
protected:
	// 親（※座標を同期させたい場合に設定）
	CObj* m_pParent;

	// 座標系（※左上座標を指定する）
    // 基本的にこの変数への直接参照はNG（※親座標との兼ね合いがあるので[getX,getY]で取得すること）
	float m_fX, m_fY;
    
    // サイズ（※中央や端の座標算出用）
    float m_fW, m_fH;

	// 管理用
	bool m_bBlank;	// 無効（非動作＆非表示）か？

	// 状態
	int  m_nStatus;
	int  m_nStatus0;

	// 情報（※演出終了待ち等、タッチ以外で外部へ状態変化を通知するために利用）
	bool m_bInfoValid;
	int  m_nInfoId;

    // 地震を無視するか？
    bool m_bQuakeIgnore;

    // 一時避難用（※一時的に表示を消す際に利用）
	bool  m_bExitTemporary;
	float m_fCountForExitTemporary;
	float m_fCountMaxForExitTemporary;
	float m_fXForExitTemporary;
	float m_fYForExitTemporary;

public:
	// コンストラクタ＆デストラクタ
	CObj( void );
	virtual ~CObj( void ){}

	// クリア（※初期化用）
	void clearObj( void );

    // 親設定
	inline void setParent( CObj* pParent ){ m_pParent = pParent; }

	// 座標系
	inline void setXY( float x, float y ){ m_fX = x; m_fY = y; }
	inline void setX( float x ){ m_fX = x; }
	inline void setY( float y ){ m_fY = y; }
    
    virtual float getX( void );
	virtual float getY( void );
	inline float getX0( void ){ return( m_fX ); }
	inline float getY0( void ){ return( m_fY ); }
    inline float getLeftX( void ){ return( getX() ); }
    inline float getCenterX( void ){ return( getX() + m_fW/2.0f ); }
    inline float getRightX( void ){ return( getX() + m_fW ); }
    inline float getTopY( void ){ return( getY() ); }
    inline float getMiddleY( void ){ return( getY() + m_fH/2.0f ); }
    inline float getBottomY( void ){ return( getY() + m_fH ); }

    // サイズ
    inline void setWH( float w, float h ){ m_fW = w; m_fH = h; }
    inline void setW( float w ){ m_fW = w; }
    inline void setH( float h ){ m_fH = h; }

    inline float getW( void ){ return( m_fW ); }
    inline float getH( void ){ return( m_fH ); }

	// ブランク関連（非動作＆非表示）
	virtual inline void setBlank( bool flag ){ m_bBlank = flag; }
	virtual inline bool swapBlank( void ){ return( m_bBlank = !m_bBlank ); }
	virtual inline bool isBlank( void ){ return( m_bBlank ); }

	// 状態関連
	bool setStatus( int status );
    bool setStatusForce( int status );
	bool checkStatus( int status );
	inline int getStatus( void ){ return( m_nStatus ); }
	inline int getStatus0( void ){ return( m_nStatus0 ); }

	// 情報関連
	inline void setInfoValid( bool flag, int id=0 ){ m_bInfoValid = flag; m_nInfoId = id; }
	inline bool isInfoValid( void ){ return( m_bInfoValid ); }
	inline int  getInfoId( void ){ return( m_nInfoId ); }

    // 地震影響無視
    inline void setQuakeIgnore( bool flag ){ m_bQuakeIgnore = flag; }

    // 一時避難関連
	void resetExitTemporary( void );
	void setExitTemporary( float count, float x, float y );
	void recoverExitTemporary( void );

	// 更新：[CUpdater]を上書き
	virtual void update( void );

protected:
	// 状態関連
	inline bool isStatusChanged( void ){ return( m_nStatus != m_nStatus0 ); }
	inline void fixStatus( void ){ m_nStatus0 = m_nStatus; }
    
    // [CObj]に関する更新（※事前のみ）
    bool preUpdateForObj( void );

	// 描画：[CDrawer]を上書き
	virtual void onDraw( void );
    
    // [CObj]に関する描画（※事前／事後）
    bool preDrawForObj( void );
    void postDrawForObj( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __OBJ_HPP__
