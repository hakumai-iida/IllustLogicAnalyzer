/*+----------------------------------------------------------------+
  |	Title:		DefBinTable.hpp [共通環境]
  |	Comment:	定義バイナリテーグル（テンプレート）
  |             [CDefTable]クラスで管理される列挙子で参照されるデータテーブル
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DEF_BIN_TABLE_HPP__
#define __DEF_BIN_TABLE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

// ヘッダの読み込みタイミングの関係で[env_push_error_log]を宣言
extern void env_push_error_log( const char* pLog );

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
// [DATA] は [void read( CInputBuffer* )] と [static int GetBinSize( void] を持つクラスを想定
template<class DATA>

//----------------------------------------
// 定義バイナリテーブル（テンプレート）
//----------------------------------------
class CDefBinTable{
protected:
    //----------------
    // メンバー
    //----------------
    CFileHeader*    m_pFileHeader;                      // ファイルヘッダ
    DATA*           m_oArrData;                         // データ配列
    int             m_nTableSize;                       // テーブルサイズ（※対応する列挙子の最大値を想定）
    char            m_bufTableName[COMMON_NAME_LEN];    // テーブル名（※ログ用）

public:
    //----------------
    // コンストラクタ
    //----------------
    CDefBinTable( eMEM_FIELD memField, int tableSize, const char* pTableName ){
        // 領域確保
        CMemMgr::PushTargetField( memField );
        m_pFileHeader = new CFileHeader();
        m_oArrData = new DATA[tableSize];
        CMemMgr::PopTargetField();
        
        m_nTableSize = tableSize;
        util_str_cpy( m_bufTableName, pTableName, COMMON_NAME_LEN );
    }
    
    //----------------
    // デストラクタ
    //----------------
    virtual ~CDefBinTable(){
        SAFE_DELETE( m_pFileHeader );
        SAFE_DELETE_ARRAY( m_oArrData );
    }

    //----------------------------------------------------------------------------------
    // データ取得（※この処理の前には対応する列挙子の有効判定[IS_***_VALID(at)]が呼ばれている想定
    //----------------------------------------------------------------------------------
    DATA* getDataAt( int at ){
        if( at >= 0 && at < m_nTableSize ){
            return( &m_oArrData[at] );
        }

        // ここまで来たら困る
        LOGE( "@ [CDefBinTable:%s].getDataAt: INVALID at=%d/%d\n",  m_bufTableName, at, m_nTableSize );
        return( NULL );
    }

    //----------------------------------------------------
    // 読み込み（※定義データの読み込み失敗は致命的エラーとする）
    //----------------------------------------------------
    bool read( const char* pFilePath ){
        // ワーク確保
        uint32 size = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_DATA );
        void* pBuf = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_DATA );

        // データ読み込み
        uint32 readed;
        if( ! CFileMgr::LoadFile( pFilePath, pBuf, size, &readed ) ){
            LOGE( "@ [CDefBinTable:%s].read: LOAD FAILED %s\n", m_bufTableName, pFilePath );
            
            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "[%s]の読み込みに失敗しました\n", pFilePath );
            env_push_error_log( buf );
            return( false );
        }
        
        // 入力バッファ（※リトルエンディアン）
        CInputBuffer ib;
        ib.setBuffer( pBuf, readed );
        ib.setForLittleEndian();

        // ファイルヘッダ読み込み
        m_pFileHeader->read( &ib );
        
        // ファイルサイズと読み込みサイズの確認
        if( m_pFileHeader->getFileSize() != readed ){
            LOGE( "@ [CDefBinTable:%s].read: FILE SIZE ERROR(%d != %d)\n", m_bufTableName, m_pFileHeader->getFileSize(), readed );

            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "[%s]のファイルサイズが不正です\n", pFilePath );
            env_push_error_log( buf );
            return( false );
        }

        // データ数の確認
        if( m_pFileHeader->getDataNum() != m_nTableSize ){
            LOGE( "@ [CDefBinTable:%s].read: DATA NUM ERROR (%d != %u)\n", m_bufTableName, m_pFileHeader->getDataNum(), m_nTableSize );

            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "[%s]のデータ数が不正です\n", pFilePath );
            env_push_error_log( buf );
            return( false );
        }

        // データサイズの確認
        int sizeData = DATA::GetBinSize();
        if( m_pFileHeader->getDataSize() != sizeData  ){
            LOGE( "@ [CDefBinTable:%s].read: DATA SIZE ERROR (%d != %u)\n", m_bufTableName, m_pFileHeader->getDataSize(), sizeData );
            
            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "[%s]のデータサイズが不正です\n", pFilePath );
            env_push_error_log( buf );
            return( false );
        }

        // ボディサイズの確認
        if( m_pFileHeader->getFileBodySize() != (m_nTableSize*sizeData) ){
            LOGE( "@ [CDefBinTable:%s].read: BODY SIZE ERROR (%d != %u)\n", m_bufTableName, m_pFileHeader->getFileBodySize(), m_nTableSize*sizeData );

            char* buf = CMemMgr::GetBufFromTempStr();
            sprintf( buf, "[%s]のボディサイズが不正です\n", pFilePath );
            env_push_error_log( buf );
            return( false );
        }
        
        // データの読み込み
        for( int i=0; i<m_nTableSize; i++ ){
            m_oArrData[i].read( &ib );
        }

        return( true );
    }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __DEF_BIN_TABLE_HPP__
