/*+----------------------------------------------------------------+
  |	Title:		LayerObject.hpp [共通環境]
  |	Comment:	レイヤーオブジェクト（※レイヤーに配置する要素の基底）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LINE_OBJECT_HPP__
#define __LINE_OBJECT_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sys/data/ListNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------------------------------
// レイヤーの種別
//-------------------------------------------------
enum eLAYER_OBJECT_TYPE{
    eLAYER_OBJECT_TYPE_INVALID = -1,    // 無効値
    
    eLAYER_OBJECT_TYPE_LINE,            // 線：CLineObjectData
    eLAYER_OBJECT_TYPE_PAINT,           // 塗り：CPaintObjectData
    eLAYER_OBJECT_TYPE_SLOT,            // スロット：CSlotObjectData
    
    eLAYER_OBJECT_TYPE_MAX,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
// 派生先のクラスのプロトタイプ宣言（※キャストして取り出す用）
class CLineObjectData;
class CPaintObjectData;
class CSlotObjectData;

//--------------------------------------------------------------
// レイヤーオブジェクト（※このクラスは器でしかないので保存要素はもたないこと）
//--------------------------------------------------------------
class CLayerObject: public CListNode{
    // 実装
    IMPLEMENT_NODE( CLayerObject )      // 実装：リストノード
    
public:
    // 解放（※対応するクラスの解放処理への受け渡し）
    static void Free( CLayerObject* pObj );

private:
    // 自身が[LineObject,PaintObject,SlotObject]であるかの判別用
    eLAYER_OBJECT_TYPE m_eObjectType;

protected:
    // コンストラクタ／デストラクタ
    CLayerObject( eLAYER_OBJECT_TYPE type );
    virtual ~CLayerObject( void ){}

public:
    // 判定
    inline bool isLineObject( void ){ return( m_eObjectType == eLAYER_OBJECT_TYPE_LINE ); }
    inline bool isPaintObject( void ){ return( m_eObjectType == eLAYER_OBJECT_TYPE_PAINT ); }
    inline bool isSlotObject( void ){ return( m_eObjectType == eLAYER_OBJECT_TYPE_SLOT ); }

    // 取得
    inline eLAYER_OBJECT_TYPE getObjectType( void ){ return( m_eObjectType ); }
    inline CLineObjectData* getLineObject( void ){ return( (isLineObject())? ((CLineObjectData*)this): NULL ); }
    inline CPaintObjectData* getPaintObject( void ){ return( (isPaintObject())? ((CPaintObjectData*)this): NULL ); }
    inline CSlotObjectData* getSlotObject( void ){ return( (isSlotObject())? ((CSlotObjectData*)this): NULL ); }
    
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
extern const char* g_pArrLabelLayerObjectType[];

#endif	// __LINE_OBJECT_HPP__
