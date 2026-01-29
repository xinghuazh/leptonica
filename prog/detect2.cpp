

#include <windows.h>

#include <iostream>
#include <vector>
#include <string>

#ifdef HAVE_CONFIG_H
#include <config_auto.h>
#endif  /* HAVE_CONFIG_H */

#include "allheaders.h"
#include "pix_internal.h"

static std::string outputPrefix = "f:/test/ocr/defect";

//-----------------------------------------------------------------------------
static void dumpPix(PIX *pix, const char *name)
{
  pixWrite((outputPrefix + name + ".png").c_str(), pix, IFF_PNG);
  printf("write: %s\n", name);
}
static void dumpPix(const char *name, PIX *pix)
{
  pixWrite((outputPrefix + name + ".png").c_str(), pix, IFF_PNG);
  printf("write: %s\n", name);
}
static void dumpBox(const char *name, PIX* pix, BOXA* boxa)
{
  BOXA* boxaExpanded = boxaAdjustSides(boxa, 1, 1, -2, 2);
  PIX *pix2 = pixConvertTo8(pix, 0);

  PIX *pixWithBox = pixDrawBoxaRandom(pix2, boxaExpanded, 1); // 红色边框
  //PIX *pixWithBox = pixDrawBoxa(pix2, boxaExpanded, 1, color);

  dumpPix(name, pixWithBox);

  pixDestroy(&pix2);
  pixDestroy(&pixWithBox);
  boxaDestroy(&boxaExpanded);
}
static void dumpBox(const char *name, PIX* pix, BOX* box)
{
  BOXA *boxa = 0;

  boxa = boxaCreate(1);
  boxaAddBox(boxa, box, L_INSERT);

  dumpBox(name, pix, boxa);
  
  boxaDestroy(&boxa);
}

//-----------------------------------------------------------------------------
int detectBlack2(int white=0)
{
  PIX* pixs = NULL;        // 原图
  PIX* pixg = NULL;        // 灰度图
  PIX* pixb = NULL;        // 二值图
  PIX* pix_v = NULL;       // 提取的竖线
  PIX* pix_no_v = NULL;    // 去除竖线后的图
  PIX* pix_h = NULL;       // 提取的水平线
  BOXA* boxa = NULL;

  // 1. 读入图像
  pixs = pixRead("d:/defect1.png");

  // 2. 检测白线时反向，同时需要精确的区域
  if (white)
  {
    PIX* pixs2 = NULL;
    PIX* pixs3 = NULL;

    BOX *box1 = boxCreate(40, 60, 606-40, 190-60);
    pixs2 = pixClipRectangle(pixs, box1, NULL);
  
    pixs3 = pixInvert(NULL, pixs2);

    boxDestroy(&box1);
    pixDestroy(&pixs);
    pixDestroy(&pixs2);

    pixs = pixs3;
  }

  // 3. 转 8bit 灰度
  pixg = pixConvertTo8(pixs, 0);
  dumpPix("1", pixg);

  // 4. 二值化（黑色为前景）
  pixb = pixThresholdToBinary(pixg, 150);
  dumpPix("2", pixb);

  // 5. 高40，宽1（针对条码横线）
  pix_v = pixMorphSequence(
    pixb,
    "o40.1",
    0
  );
  dumpPix("3", pix_v);

  // 6. 联通区域
  //boxa = pixConnComp(pix_v, NULL, 4);
  boxa = pixConnCompBB(pix_v, 4);
  //boxa = pixConnCompPixa(pix_v, NULL, 4);
  dumpBox("4", pixs, boxa);

  // 7. 资源释放
  pixDestroy(&pixs);
  pixDestroy(&pixg);
  pixDestroy(&pixb);
  pixDestroy(&pix_v);
  pixDestroy(&pix_no_v);
  pixDestroy(&pix_h);
  boxaDestroy(&boxa);

  return 0;
}

int main()
{
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetBreakAlloc(0);

  SetCurrentDirectory("D:/mv3/3rdParty/leptonica/prog");

  detectBlack2(1);
  return 0;
}
