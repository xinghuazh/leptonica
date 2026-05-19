
#include <windows.h>

//-----------------------------------------------------------------------------
//static void dumpPix(PIX *pix, const char *name)
//{
//  char path[260];
//  snprintf(path, sizeof(path), "f:/test/ocr/t%s.png", name);
//
//  pixWrite(path, pix, IFF_PNG);
//  printf("write: %s\n", name);
//}
static void dumpPix(const char *name, PIX *pix)
{
  char path[260];
  snprintf(path, sizeof(path), "f:/test/ocr/t%s.png", name);
  SetCurrentDirectory("D:/mv3/3rdParty/leptonica/prog");

  pixWrite(path, pix, IFF_PNG);
  printf("write: %s\n", name);
}
static void dumpBoxa(const char *name, PIX* pix, BOXA* boxa)
{
  BOXA* boxaExpanded = boxaAdjustSides(boxa, 1, 1, -2, 2);
  PIX *pix2 = pixConvertTo8(pix, 0);

  PIX *pixWithBox = pixDrawBoxaRandom(pix2, boxaExpanded, 1); // ºìÉ«±ß¿ò
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

  dumpBoxa(name, pix, boxa);
  
  boxaDestroy(&boxa);
}
