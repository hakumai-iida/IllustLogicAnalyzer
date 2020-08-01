/*+----------------------------------------------------------------+
  |	Title:		BrushBinData.hpp [共通環境]
  |	Comment:	ブラシバイナリデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BRUSH_BIN_DATA_HPP__
#define __BRUSH_BIN_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/tex/stroke/StrokeConst.hpp"

// ヘッダの読み込みタイミングの関係で[CEditValueMenu]を宣言
class CEditValueMenu;

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ブラシバイナリデータ
//----------------------------------------
class CBrushBinData{
public:
    //------------------------
    // バイナリサイズ取得
    //------------------------
    inline static int GetBinSize( void ){
        return( 3*2 + 4*1 + 2*2 + 2*1 );
    }

protected:
    //------------------------
    // メンバー
    //------------------------
    eSTROKE_TYPE m_eStrokeType;         // [enum/2]: ストロークタイプ
    int16 m_nStrokeSize1000;            // [2]: ストロークサイズ（※千分率）
    int16 m_nStrokeRange1000;           // [2]: ストローク出力範囲（※千分率）
    
    uint8 m_nDotPutMax;                 // [1]: ドット出力最大
    uint8 m_nDotPutRateBase;            // [1]: ドット出力割合：基本（※百分率）
    uint8 m_nDotPutRateStrong;          // [1]: ドット出力割合：濃い方（※百分率）
    uint8 m_nDotPutRateWeak;            // [1]: ドット出力割合：薄い方（※百分率）

    int16 m_nShakePosScale1000;         // [2]: 位置ブレ率（※千分率）
    int16 m_nShakeSizeScale1000;        // [2]: サイズブレ率（※千分率）
    
    bool m_bDotPutForce;                // [1]: 必ずドットを出力するか？
    bool m_bDotPutWhetherMax;           // [1]: ドット出力先が最大値であろうとドットを出力するか？

public:
    //------------------------
    // コンストラクタ／デストラクタ
    //------------------------
    CBrushBinData(){}
    virtual ~CBrushBinData(){}

    //------------------------
    // 取得
    //------------------------
    inline eSTROKE_TYPE getStrokeType( void ){ return( m_eStrokeType ); }
    inline int getStrokeSize1000( void ){ return( m_nStrokeSize1000 ); }
    inline float getStrokeSize( void ){ return( m_nStrokeSize1000/1000.0f ); }
    inline int getStrokeRange1000( void ){ return( m_nStrokeRange1000 ); }
    inline float getStrokeRange( void ){ return( m_nStrokeRange1000/1000.0f ); }

    inline int getDotPutMax( void ){ return( m_nDotPutMax ); }
    inline int getDotPutRateBase( void ){ return( m_nDotPutRateBase ); }
    inline int getDotPutRateStrong( void ){ return( m_nDotPutRateStrong ); }
    inline int getDotPutRateWeak( void ){ return( m_nDotPutRateWeak ); }
    
    inline int getShakePosScale1000( void ){ return( m_nShakePosScale1000 ); }
    inline float getShakePosScale( void ){ return( m_nShakePosScale1000/1000.0f ); }
    inline int getShakeSizeScale1000( void ){ return( m_nShakeSizeScale1000 ); }
    inline float getShakeSizeScale( void ){ return( m_nShakeSizeScale1000/1000.0f ); }
    
    inline bool isDotPutForce( void ){ return( m_bDotPutForce ); }
    inline bool isDotPutWhetherMax( void ){ return( m_bDotPutWhetherMax ); }
    
    //--------------------------------------------
    // 設定（※ストロークチェックループでのエディット用）
    //--------------------------------------------
    inline void setStrokeType( eSTROKE_TYPE type ){ m_eStrokeType = type; }
    inline void setStrokeSize1000( int size1000 ){ m_nStrokeSize1000 = (int16)size1000; }
    inline void setStrokeRange1000( int range1000 ){ m_nStrokeRange1000 = (int16)range1000; }

    inline void setDotPutMax( int max ){ m_nDotPutMax = (uint8)max; }
    inline void setDotPutRateBase( int rate ){ m_nDotPutRateBase = (uint8)rate; }
    inline void setDotPutRateStrong( int rate ){ m_nDotPutRateStrong = (uint8)rate; }
    inline void setDotPutRateWeak( int rate ){ m_nDotPutRateWeak = (uint8)rate; }
    
    inline void setShakePosScale1000( int scale1000 ){ m_nShakePosScale1000 = (int16)scale1000; }
    inline void setShakeSizeScale1000( int scale1000 ){ m_nShakeSizeScale1000 = (int16)scale1000; }

    inline void setDotPutForce( bool flag ){ m_bDotPutForce = flag; }
    inline void setDotPutWhetherMax( bool flag ){ m_bDotPutWhetherMax = flag; }

    //-----------------------------------------------------
    // 読み込み（※[pIB]は有効である前提＆値の整合性等の判断はしない）
    //-----------------------------------------------------
    inline void read( CInputBuffer* pIB ){
        m_eStrokeType = (eSTROKE_TYPE)pIB->readInt16();
        m_nStrokeSize1000 = pIB->readInt16();
        m_nStrokeRange1000 = pIB->readInt16();
        
        m_nDotPutMax = pIB->readUint8();
        m_nDotPutRateBase = pIB->readUint8();
        m_nDotPutRateStrong = pIB->readUint8();
        m_nDotPutRateWeak = pIB->readUint8();
        
        m_nShakePosScale1000 = pIB->readInt16();
        m_nShakeSizeScale1000 = pIB->readInt16();
        
        m_bDotPutForce = ( pIB->readUint8() != 0 );
        m_bDotPutWhetherMax = ( pIB->readUint8() != 0 );
    }
    
    //---------------------------------------------
    // データ編集（※ブラシチェックループでのエディット用）
    //---------------------------------------------
    void setEditValueMenu( CEditValueMenu* pMenu );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BRUSH_BIN_DATA_HPP__
