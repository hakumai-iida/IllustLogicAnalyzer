/*+----------------------------------------------------------------+
  |	Title:		Fill.hpp [共通環境]
  |	Comment:	塗り（※対象はパレット画像）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FILL_HPP__
#define __FILL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FillConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
 +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//----------------------------
// 下塗り処理単位
//----------------------------
struct stFILL_UNDERCOAT_CELL{
    int hint;   // 処理ヒント
    int xL, xR; // X座標
    int y;      // Y座標
};

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 塗り
//-----------------------------------------------------------------
class CFill{
protected:
    //-------------------------------------------
    // 設定値
    //-------------------------------------------
    eFILL_TYPE m_eType;         // タイプ
    
    //-------------------------------------------
    // 塗りつぶし対象
    //-------------------------------------------
    BYTE* m_pBuf;               // 塗りつぶしバッファ（※実際の出力先）
    BYTE* m_pBufUndercoat;      // 下塗りバッファ（※指定座標を内側にもつ閉じた領域が渡される想定）
    BYTE* m_pBufGuard;          // ガードバッファ
    int m_nDotW, m_nDotH;       // バッファサイズ

    //-------------------------------------------
    // 下塗りをした左右上下端（※本塗りの処理範囲）
    //-------------------------------------------
    int m_nUndercoatL, m_nUndercoatR;
    int m_nUndercoatT, m_nUndercoatB;

    //---------------------------------------------------
    // 下塗りワーク（※循環管理＝[Head]と[Tail]による追いかけっこ）
    //---------------------------------------------------
    stFILL_UNDERCOAT_CELL* m_stArrUndercoatCell;
    int m_nUndercoatCellHeadAt;
    int m_nUndercoatCellTailAt;
    
public:
    // コンストラクタ／デストラクタ
    CFill( void );
    virtual ~CFill( void );
    
    // クリア
    void clear( void );

    //---------------------------------------------------
    // 塗り設定
    //---------------------------------------------------
    bool set( eFILL_TYPE type );
    
    // 判定
    inline bool isValid( void ){ return( m_eType != eFILL_TYPE_INVALID ); }
    
    // 取得
    inline eFILL_TYPE getType( void ){ return( m_eType ); }

    //--------------------------------------------------------------------------------------------------
    // 塗りつぶし（※フラグにより[広域／全色]処理へ切り替え＆[addVal]の値で加算／減算の切り替え）
    //--------------------------------------------------------------------------------------------------
    // ガイドバッファ[pBufGuide]を[x,y]を始点に下塗り色で塗りつぶす
    // その後、下塗りの結果を用いて[pBuf]の領域を塗り設定で塗りつぶす
    //（※この処理の後は[pBufGuide]が破壊されている点に注意すること＝塗りガイドを再利用したい場合は退避＆復旧する必要がある）
    // [pBufGuard]で出力を制限する
    //--------------------------------------------------------------------------------------------------
    bool fill0( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y, BYTE palGrp, BYTE palVal, BYTE testPalGrp,
                bool isForAdd, int addVal );

    // 通常塗り
    inline bool fill( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y, BYTE palGrp, BYTE palVal, BYTE testPalGrp ){
        return( fill0( pBuf, pBufGuide, pBufGuard, w, h, x, y, palGrp, palVal, testPalGrp, false, 0 ) );
    }

    // 領域加算
    inline bool fillAddForAdd( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y, BYTE testPalGrp, int addVal ){
        return( fill0( pBuf, pBufGuide, pBufGuard, w, h, x, y, 0, 0, testPalGrp, true, addVal ) );
    }

    //--------------------------------------------------------------------------------------------------
    // オフセット（別画素参照）による加算（※フラグにより[全色／予約]処理へ切り替え＆[addVal]の値で加算／減算の切り替え）
    //--------------------------------------------------------------------------------------------------
    bool fillAddWithOfs( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int ofsX, int ofsY, int addVal, bool isReserve );
    
    // 全色加算
    inline bool fillAddForColor( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int ofsX, int ofsY, int addVal ){
        return( fillAddWithOfs( pBuf, pBufGuide, pBufGuard, w, h, ofsX, ofsY, addVal, false ) );
    }

    // パレット値予約
    inline bool fillAddForReserve( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int ofsX, int ofsY, int addVal ){
        return( fillAddWithOfs( pBuf, pBufGuide, pBufGuard, w, h, ofsX, ofsY, addVal, true ) );
    }
    
    //--------------------------------------------------------------------------------------------------
    // マスク
    //--------------------------------------------------------------------------------------------------
    bool fillMask( BYTE* pBuf, BYTE* pBufGuide, int w, int h, int x, int y );

protected:
    // [下塗り]: ワーク関連
    void readyForUndercoat( void );
    bool isUndercoatCellEmpty( void );
    void addUndercoatCell( int hint, int xL, int xR, int y );
    stFILL_UNDERCOAT_CELL* getUndercoatCell( void );
    
    // [下塗り]: ラインのスキャン
    void scanLineForUndercoat( int xL, int xR, int y, bool isMoveUp );

    // [下塗り]: 塗りつぶし
    void fillUndercoat( int x, int y );
    
    // [本塗り]: 指定タイプ毎の処理（※下塗り色による色バッファへの処理）
    void fillForOff( void );
    void fillForGuard( void );
    void fillForMono( BYTE palGrp, BYTE testPalGrp );
    void fillForMonoWithOfs( BYTE palGrp, int palValOfs, BYTE testPalGrp );
    void addForArea( BYTE testPalGrp, int addVal );
    void subForArea( BYTE testPalGrp, int subVal );
    void addForColor( int ofsX, int ofsY, int addVal );
    void subForColor( int ofsX, int ofsY, int subVal );
    void addForReserve( int ofsX, int ofsY, int addVal );
    void subForReserve( int ofsX, int ofsY, int subVal );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelFillType[];
extern const char* g_pArrLabelFillOption[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __FILL_HPP__ */
