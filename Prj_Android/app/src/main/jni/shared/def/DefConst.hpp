/*+----------------------------------------------------------------+
  |	Title:		DefConst.hpp [共通環境]
  |	Comment:	定義定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DEF_CONST_HPP__
#define __DEF_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// シェーダーID: [System.xls/Shader]シート
//-----------------------------------------------------------------
enum eSHADER{
    eSHADER_INVALID = -1,
#include "inc/ShaderId.inc"
    eSHADER_MAX,
};

#define IS_SHADER_VALID( _id )      ((_id)>=(eSHADER)0 && (_id)<eSHADER_MAX)

//-----------------------------------------------------------------
// フレームバッファID: [System.xls/FrameBuffer]シート
//-----------------------------------------------------------------
enum eFRAME_BUFFER{
    eFRAME_BUFFER_INVALID = -1,
#include "inc/FrameBufferId.inc"
    eFRAME_BUFFER_MAX,
};

#define IS_FRAME_BUFFER_VALID( _id )    ((_id)>=(eFRAME_BUFFER)0 && (_id)<eFRAME_BUFFER_MAX)

//-----------------------------------------------------------------
// 画像ID: [Img.xls/image]シート
//-----------------------------------------------------------------
enum eIMG{
    eIMG_INVALID = -1,
#include "inc/ImgId.inc"
    eIMG_MAX,
};

#define IS_IMG_VALID( _id )     ((_id)>=(eIMG)0 && (_id)<eIMG_MAX)

//-----------------------------------------------------------------
// スプライトID: [Draw.xls/Spr]シート
//-----------------------------------------------------------------
enum eSPR{
    eSPR_INVALID = -1,
#include "inc/SprId.inc"
    eSPR_MAX,
};

#define IS_SPR_VALID( _id )     ((_id)>=(eSPR)0 && (_id)<eSPR_MAX)

//-----------------------------------------------------------------
// スプライトグループID: [Draw.xls/Grp]シート
//-----------------------------------------------------------------
enum eGRP{
    eGRP_INVALID = -1,
#include "inc/GrpId.inc"
    eGRP_MAX,
};

#define IS_GRP_VALID( _id )     ((_id)>=(eGRP)0 && (_id)<eGRP_MAX)

//-----------------------------------------------------------------
// eFONT: フォントID: [Draw.xls/Font]シート
//-----------------------------------------------------------------
enum eFONT{
    eFONT_INVALID = -1,
#include "inc/FontId.inc"
    eFONT_MAX,
};

#define IS_FONT_VALID( _id )     ((_id)>=(eFONT)0 && (_id)<eFONT_MAX)

//-----------------------------------------------------------------
// eNUM: 数字ID: [Draw.xls/Num]シート
//-----------------------------------------------------------------
enum eNUM{
    eNUM_INVALID = -1,
#include "inc/NumId.inc"
    eNUM_MAX
};

#define IS_NUM_VALID( _id )     ((_id)>=(eNUM)0 && (_id)<eNUM_MAX)

//-----------------------------------------------------------------
// ePAL_OFS: [Bezier.xls/PalOfs]シート
//-----------------------------------------------------------------
enum ePAL_OFS{
    ePAL_OFS_INVALID = -1,
#include "inc/PalOfsId.inc"
    ePAL_OFS_MAX,
};

#define IS_PAL_OFS_VALID( _id )     ((_id)>=(ePAL_OFS)0 && (_id)<ePAL_OFS_MAX)

//-----------------------------------------------------------------
// eBRUSH: [Bezier.xls/Brush]シート
//-----------------------------------------------------------------
enum eBRUSH{
    eBRUSH_INVALID = -1,
#include "inc/BrushId.inc"
    eBRUSH_MAX,
};

#define IS_BRUSH_VALID( _id )     ((_id)>=(eBRUSH)0 && (_id)<eBRUSH_MAX)

//-----------------------------------------------------------------
// eBUCKET: [Bezier.xls/Bucket]シート
//-----------------------------------------------------------------
enum eBUCKET{
    eBUCKET_INVALID = -1,
#include "inc/BucketId.inc"
    eBUCKET_MAX,
};

#define IS_BUCKET_VALID( _id )     ((_id)>=(eBUCKET)0 && (_id)<eBUCKET_MAX)

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelBgm[];
extern const char* g_pArrLabelSe[];
extern const char* g_pArrLabelShader[];
extern const char* g_pArrLabelFrameBuffer[];
extern const char* g_pArrLabelImg[];
extern const char* g_pArrLabelSpr[];
extern const char* g_pArrLabelGrp[];
extern const char* g_pArrLabelFont[];
extern const char* g_pArrLabelNum[];
extern const char* g_pArrLabelPalOfs[];
extern const char* g_pArrLabelBrush[];
extern const char* g_pArrLabelBucket[];

#endif /* __DEF_CONST_HPP__ */
