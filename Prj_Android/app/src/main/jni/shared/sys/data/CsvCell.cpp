/*+----------------------------------------------------------------+
  |	Title:		CsvCell.cpp [共通環境]
  |	Comment:	CSVセル
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "CsvCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------
// CSV読み込み準備
//------------------
bool csv_cell_ready_with_error_log( const char* pFilePath, CInputBuffer* pIB, CFileHeader* pFH,
                                    int numData, int sizeData, const char* pLabelForError ){
    
    uint32 size = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_DATA );
    void* pBuf = CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_DATA );

    // データ読み込み
    uint32 readed;
    if( ! CFileMgr::LoadFile( pFilePath, pBuf, size, &readed ) ){
        LOGE( "@ %s: LOAD FAILED: %s\n", pLabelForError, pFilePath );
        
        char* buf = CMemMgr::GetBufFromTempStr();
        sprintf( buf, "[%s]の読み込みに失敗しました\n", pFilePath );
        env_push_error_log( buf );
        return( false );
    }
    
    // 入力バッファ（※リトルエンディアン）
    pIB->setBuffer( pBuf, readed );
    pIB->setForLittleEndian();

    // ファイルヘッダ読み込み
    pFH->read( pIB );
    
    // ファイルサイズと読み込みサイズの確認
    if( pFH->getFileSize() != readed ){
        LOGE( "@ [CsvCell]::%s: FILE SIZE ERROR(%d != %d)\n", pLabelForError, pFH->getFileSize(), readed );

        char* buf = CMemMgr::GetBufFromTempStr();
        sprintf( buf, "[%s]のファイルサイズが不正です\n", pFilePath );
        env_push_error_log( buf );
        return( false );
    }

    // データ数の確認
    if( numData >= 0 && pFH->getDataNum() != numData ){
        LOGE( "@ [CsvCell]::%s: DATA NUM ERROR: (%d != %u)\n", pLabelForError, pFH->getDataNum(), numData );

        char* buf = CMemMgr::GetBufFromTempStr();
        sprintf( buf, "[%s]のデータ数が不正です\n", pFilePath );
        env_push_error_log( buf );
        return( false );
    }

    // データサイズの確認（※実データ数が０なら無視）
    if( pFH->getDataNum() > 0 && sizeData >= 0 && pFH->getDataSize() != sizeData  ){
        LOGE( "@ [CsvCell]::%s: DATA SIZE ERROR: (%d != %u)\n", pLabelForError, pFH->getDataSize(), sizeData );
        
        char* buf = CMemMgr::GetBufFromTempStr();
        sprintf( buf, "[%s]のデータサイズが不正です\n", pFilePath );
        env_push_error_log( buf );
        return( false );
    }

    // ボディサイズの確認
    if( numData >= 0 && sizeData >= 0 && pFH->getFileBodySize() != (numData*sizeData) ){
        LOGE( "@ [CsvCell]::%s: BODY SIZE ERROR: (%d != %u)\n", pLabelForError, pFH->getFileBodySize(), numData*sizeData );

        char* buf = CMemMgr::GetBufFromTempStr();
        sprintf( buf, "[%s]のサイズが不正です\n", pFilePath );
        env_push_error_log( buf );
        return( false );
    }
    
    // ここまで来たら正常
    return( true );
}
