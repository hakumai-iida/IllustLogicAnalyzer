/*+----------------------------------------------------------------+
  |	Title:		DefConst.cpp [共通環境]
  |	Comment:	定義定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DefConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//------------------------
// ラベル
//------------------------
// for System
const char* g_pArrLabelShader[] = {
#include "inc/ShaderLabel.inc"
    NULL,
};

const char* g_pArrLabelFrameBuffer[] = {
#include "inc/FrameBufferLabel.inc"
    NULL,
};

// for Image
const char* g_pArrLabelImg[] = {
#include "inc/ImgLabel.inc"
    NULL,
};

// for Draw
const char* g_pArrLabelSpr[] = {
#include "inc/SprLabel.inc"
    NULL,
};

const char* g_pArrLabelGrp[] = {
#include "inc/GrpLabel.inc"
    NULL,
};

const char* g_pArrLabelFont[] = {
#include "inc/FontLabel.inc"
    NULL,
};

const char* g_pArrLabelNum[] = {
#include "inc/NumLabel.inc"
    NULL,
};

// for Bezier
const char* g_pArrLabelPalOfs[] = {
#include "inc/PalOfsLabel.inc"
    NULL,
};

const char* g_pArrLabelBrush[] = {
#include "inc/BrushLabel.inc"
    NULL,
};

const char* g_pArrLabelBucket[] = {
#include "inc/BucketLabel.inc"
    NULL,
};

/*+----------------------------------------------------------------+
  |	Prototype	プロト宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
