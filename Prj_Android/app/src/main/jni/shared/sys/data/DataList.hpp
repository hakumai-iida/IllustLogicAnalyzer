/*+----------------------------------------------------------------+
  |	Title:		DataList.hpp [共通環境]
  |	Comment:	データリスト（※リスト操作を備えたデータ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DATA_LIST_HPP__
#define __DATA_LIST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "List.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// データ操作実装
#define IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( _DATA_CLASS_NAME )                                                          \
    public:                                                                                                                     \
        inline _DATA_CLASS_NAME* getData( int select ){ return( (_DATA_CLASS_NAME*) getData0( select ) ); }                     \
        inline _DATA_CLASS_NAME* getSelectedData( void ){ return( (_DATA_CLASS_NAME*) getSelectedData0() ); }                   \
        inline _DATA_CLASS_NAME* getDataHead( void ){ return( (_DATA_CLASS_NAME*) m_oDataList.getHead() ); }                    \
        inline _DATA_CLASS_NAME* getDataTail( void ){ return( (_DATA_CLASS_NAME*) m_oDataList.getTail() ); }                    \
        inline _DATA_CLASS_NAME* addData( _DATA_CLASS_NAME* pNewData ){ return( (_DATA_CLASS_NAME*)addData0( pNewData ) ); }    \
        inline _DATA_CLASS_NAME* addData( void ){                                                                               \
            _DATA_CLASS_NAME* pNew = _DATA_CLASS_NAME::Alloc();                                                                 \
            pNew->copy( (_DATA_CLASS_NAME*)getSelectedData0() );                                                                \
            return( (_DATA_CLASS_NAME*)addData0( pNew ) );                                                                      \
        }                                                                                                                       \
        inline _DATA_CLASS_NAME* moveUpData( void ){ return( (_DATA_CLASS_NAME*) moveUpData0() ); }                             \
        inline _DATA_CLASS_NAME* moveDownData( void ){ return( (_DATA_CLASS_NAME*) moveDownData0() ); }                         \
        inline void deleteData( void ){ _DATA_CLASS_NAME::Free( (_DATA_CLASS_NAME*)deleteData0() ); }                           \
        inline _DATA_CLASS_NAME* copyData( void ){                                                                              \
            _DATA_CLASS_NAME* pData = getSelectedData();                                                                        \
            if( pData != NULL ){                                                                                                \
                _DATA_CLASS_NAME* pNew = _DATA_CLASS_NAME::Alloc();                                                             \
                pNew->copy( pData );                                                                                            \
                return( pNew );                                                                                                 \
            }                                                                                                                   \
            return( NULL );                                                                                                     \
        }                                                                                                                       \
        inline _DATA_CLASS_NAME* pasteData( _DATA_CLASS_NAME* pDataCopy ){                                                      \
            _DATA_CLASS_NAME* pData = getSelectedData();                                                                        \
            if( pData != NULL ){                                                                                                \
                pData->copy( pDataCopy );                                                                                       \
            }                                                                                                                   \
            return( pData );                                                                                                    \
        }

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// データリスト
//----------------------------------------
class CDataList{
protected:
	CList m_oDataList;          // 実体でもつ
    int m_nSelect;              // 選択位置
    bool m_bNodeDisconnectOnly; // 項目削除時にリストから切り離すだけか？（※削除対象のメモリを解放しない）

public:
	// コンストラクタ＆デストラクタ
	CDataList( void );
    CDataList( bool isDisconnectOnly );
	virtual ~CDataList( void );

	// 選択設定
	int setSelect( int select );
	int addSelect( int add );

	// 取得関連
	inline int getSelect( void ){ return( m_nSelect ); }
	inline int getDataSize( void ){ return( m_oDataList.getSize() ); }
	inline CList* getDataList( void ){ return( &m_oDataList ); }

protected:
	// リスト操作（※派生先で利用するデータ型にあわせてラップして使用することを前提）
	CListNode* getData0( int select );			// 指定位置のデータを取得
	CListNode* getSelectedData0( void );		// 選択中のデータを取得
	CListNode* addData0( CListNode* pNewData );	// 選択中の要素の後ろに指定データを追加
	CListNode* moveUpData0( void );				// 選択中のデータを一つ上(前)に
	CListNode* moveDownData0( void );			// 選択中のデータを一つ下(後)に
    CListNode* deleteData0( void );             // 選択中のデータを削除
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __DATA_LIST_HPP__
