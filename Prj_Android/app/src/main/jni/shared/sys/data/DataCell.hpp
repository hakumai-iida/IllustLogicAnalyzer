/*+----------------------------------------------------------------+
  |	Title:		DataCell.hpp [共通環境]
  |	Comment:	データ要素（※編集ループ等で読み／書き／調整するデータ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DATA_CELL_HPP__
#define __DATA_CELL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "loop/node/EditValueMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//===============================================================================================
// 基本実装：基本データ処理（※アプリ上で編集管理されるデータ）（※クラス定義内に記載）
//（※[.cpp]にて各メソッドを実装すること）
//===============================================================================================
#define IMPLEMENT_DATA( _CLASS_NAME )                                                           \
    public:                                                                                     \
        void clear( void );                                                                     \
        void copy( _CLASS_NAME* pData );                                                        \
        void read( CInputBuffer* pIB );                                                         \
        void write( COutputBuffer* pOB );

//===============================================================================================
// 追加実装：フラグ（※クラス定義内に記載）
//===============================================================================================
#define IMPLEMENT_DATA_FLAG( _ENUM_TYPE )                                                       \
    protected:                                                                                  \
        int m_nFlag;                                                                            \
    public:                                                                                     \
        inline bool checkFlag( _ENUM_TYPE id ){ return( CConst::CheckFlag( m_nFlag, id ) ); }   \
        inline void setFlagOn( _ENUM_TYPE id ){ CConst::SetFlagOn( &m_nFlag, id ); }            \
        inline void setFlagOff( _ENUM_TYPE id ){ CConst::SetFlagOff( &m_nFlag, id ); }          \
        inline void swapFlag( _ENUM_TYPE id ){ CConst::SwapFlag( &m_nFlag, id ); }

//===============================================================================================
// 追加実装：データの編集（※クラス定義内に記載）
//（※[.cpp]にて各メソッドを実装すること）
//===============================================================================================
#define IMPLEMENT_DATA_EDIT()                                                                   \
    public:                                                                                     \
        void setEditValueMenu( CEditValueMenu* pMenu );

//===============================================================================================
// 要素確保管理（※インスタンス枠をあらかじめ確保しておき、要素の確保／解放によるメモリ変動を避けたい場合の利用を想定）
//===============================================================================================
// クラス定義内(.hpp)に記載
#define IMPLEMENT_DATA_ALLOCATOR_IN_HPP( _CLASS_NAME )                      \
    protected:                                                              \
        static _CLASS_NAME* s_oArrAllocData;                                \
        bool m_bAllocated;                                                  \
    public:                                                                 \
        static bool Init( void );                                           \
        static void Exit( void );                                           \
        static void Reset( void );                                          \
        static _CLASS_NAME* Alloc( void );                                  \
        static void Free( _CLASS_NAME* pData );                             \
        static int GetAllocDataNumMax( void );                              \
        static int GetAllocDataNumUse( void );                              \
        inline bool isAllocated( void ){ return( m_bAllocated ); }          \
        inline void setAllocated( bool flag ){ m_bAllocated = flag; }

// クラス実装内(.cpp)に記載
#define IMPLEMENT_DATA_ALLOCATOR_IN_CPP( _CLASS_NAME, _MEMORY_FIELD, _MAX ) \
    _CLASS_NAME* _CLASS_NAME::s_oArrAllocData;                              \
    bool _CLASS_NAME::Init( void ){                                         \
        CMemMgr::PushTargetField( _MEMORY_FIELD );                          \
        s_oArrAllocData = new _CLASS_NAME[_MAX];                            \
        CMemMgr::PopTargetField();                                          \
        Reset();                                                            \
        return( true );                                                     \
    }                                                                       \
    void _CLASS_NAME::Exit( void ){                                         \
        SAFE_DELETE_ARRAY( s_oArrAllocData );                               \
    }                                                                       \
    void _CLASS_NAME::Reset( void ){                                        \
        for( int i=0; i<_MAX; i++ ){                                        \
            s_oArrAllocData[i].clear();                                     \
            s_oArrAllocData[i].setAllocated( false );                       \
        }                                                                   \
    }                                                                       \
    _CLASS_NAME* _CLASS_NAME::Alloc( void ){                                \
        for( int i=0; i<_MAX; i++ ){                                        \
            if( !s_oArrAllocData[i].isAllocated() ){                        \
                s_oArrAllocData[i].setAllocated( true );                    \
                return( &s_oArrAllocData[i] );                              \
            }                                                               \
        }                                                                   \
        LOGE( "@ _CLASS_NAME::ALLOC BUF SHORTAGE\n" );                      \
        return( NULL );                                                     \
    }                                                                       \
    void _CLASS_NAME::Free( _CLASS_NAME* pData ){                           \
        if( pData != NULL ){                                                \
            pData->clear();                                                 \
            pData->setAllocated( false );                                   \
        }                                                                   \
    }                                                                       \
    int _CLASS_NAME::GetAllocDataNumMax( void ){                            \
        return( _MAX );                                                     \
    }                                                                       \
    int _CLASS_NAME::GetAllocDataNumUse( void ){                            \
        int num = 0;                                                        \
        for( int i=0; i<_MAX; i++ ){                                        \
            if( s_oArrAllocData[i].isAllocated() ){                         \
                num++;                                                      \
            }                                                               \
        }                                                                   \
        return( num );                                                      \
    }

//===========================================================================
// 一括フラグ設定
// [IMPLEMENT_DATA_FLAG]と[IMPLEMENT_DATA_ALLOCATOR_IN_HPP/CPP]の実装る前提
//===========================================================================
#define IMPLEMENT_DATA_FLAG_ALL( _ENUM_TYPE )                               \
    inline static void SetFlagOnAll( _ENUM_TYPE flag ){                     \
        int max = GetAllocDataNumMax();                                     \
        for( int i=0; i<max; i++ ){                                         \
            s_oArrAllocData[i].setFlagOn( flag );                           \
        }                                                                   \
    }                                                                       \
    inline static void SetFlagOffAll( _ENUM_TYPE flag ){                    \
        int max = GetAllocDataNumMax();                                     \
        for( int i=0; i<max; i++ ){                                         \
            s_oArrAllocData[i].setFlagOff( flag );                          \
        }                                                                   \
    }

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
// 基底クラス[CDataCell]から各クラスを派生させて
// 取り扱いを共通化する需要はなさそうなのでクラス定義はしないでおく

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __DATA_CELL_HPP__
