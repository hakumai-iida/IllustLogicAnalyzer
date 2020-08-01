/*+----------------------------------------------------------------+
  |	Title:		Win.hpp [共通環境]
  |	Comment:	窓（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __WIN_HPP__
#define __WIN_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 窓
//----------------------------------------
class CWin: public CTouchNode{
protected:
	// メンバー
	eGRP m_eGrpId;                      // 窓の見た目（※ほぼ[eGRP_DEBUG_WINDOW]固定）
	eIMG m_ePalId;                      // パレットID（※デフォルトは[m_eGrpId]の先頭要素パレット）
	char m_cArrTitle[COMMON_NAME_LEN];  // 窓のタイトル（※タイトルバーに表示する文字列）

	// 窓全体の幅／高さ
	float m_fWinW;
	float m_fWinH;

	// 窓の中央の幅／高さ（※表示／操作が可能な領域）
	float m_fCenterX;
	float m_fCenterY;
	float m_fCenterW;
	float m_fCenterH;

	// 窓の内部的な幅／高さ（※窓が保持する項目の領域）
	float m_fInternalW;
	float m_fInternalH;

	// 窓の内部的なオフセット値（※[Center]に対して[Internal]が大きい場合のオフセット値）
	float m_fInternalOfsX;
	float m_fInternalOfsY;

	// 移動速度（※フリック操作用）
	int m_nFlickCount;
	float m_fFlickAX;
	float m_fFlickAY;
	float m_fFlickVX;
	float m_fFlickVY;

	// フリック係数（※フリックによる移動値への係数）
	float m_fFlickAdjX;
	float m_fFlickAdjY;

public:
    // コンストラクタ／デストラクタ（※窓の見た目に関わる設定はコンストラクタでのみ行う想定）
	CWin( eGRP grpId, float cW, float cH );
	virtual ~CWin( void ){}

	// 設定
	virtual void setTitle( const char* pTitle );
	virtual void setCenterWH( float w, float h );
	virtual void setInternalWH( float w, float h );
	virtual void setInternalOfsX( float x );
	virtual void setInternalOfsY( float y );
	virtual inline void setPalId( eIMG palId ){ m_ePalId = palId; }
	virtual inline void setFlickAdjX( float adjX ){ m_fFlickAdjX = adjX; }
	virtual inline void setFlickAdjY( float adjY ){ m_fFlickAdjY = adjY; }

	// 取得
	virtual inline float getCenterX( void ){ return( m_fCenterX ); }
	virtual inline float getCenterY( void ){ return( m_fCenterY ); }
	virtual inline float getInternalW( void ){ return( m_fInternalW ); }
	virtual inline float getInternalH( void ){ return( m_fInternalH ); }
	virtual inline float getInternalOfsX( void ){ return( m_fInternalOfsX ); }
	virtual inline float getInternalOfsY( void ){ return( m_fInternalOfsY ); }

	virtual inline float getW( void ){ return( m_fWinW ); }
	virtual inline float getH( void ){ return( m_fWinH ); }

protected:
	virtual void onUpdate0( void );

	virtual void limitInternalOfsX( void );
	virtual void limitInternalOfsY( void );
	virtual void setFlick( float flickX, float flickY );

	virtual void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __WIN_HPP__ */
