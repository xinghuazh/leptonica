

#include <windows.h>

#include <zxing/BarcodeFormat.h>
//#include <zxing/MultiFormatReader.h>
#include <zxing/Barcode.h>
#include <zxing/ReaderOptions.h>
//#include <zxing/LuminanceSource.h>
//#include <zxing/common/GreyscaleLuminanceSource.h>
#include <leptonica/allheaders.h>
#include "pix_internal.h"


#pragma comment(lib, "ZXing.lib")

static std::string outputPrefix = "f:/test/ocr/bar";

std::unique_ptr<ZXing::ImageView> pixToImageView(PIX* pix) {
  if (!pix) return nullptr;

  PIX* gray = nullptr;
  if (pix->d == 32) {
    gray = pixConvertRGBToLuminance(pix);
  }
  else if (pix->d == 8) {
    gray = pixClone(pix);
  }
  else {
    gray = pixConvertTo8(pix, false);
  }
  pixWrite((outputPrefix + "1.gray.png").c_str(), gray, IFF_PNG);

  auto imageView = std::make_unique<ZXing::ImageView>(
    reinterpret_cast<const uint8_t*>(gray->data),
    gray->w,
    gray->h,
    ZXing::ImageFormat::Lum,
    gray->wpl * 4,
    1
  );

  //pixDestroy(&gray);
  return imageView;
}

std::string decodeBarcode(const std::string& imagePath) {
  PIX* pix = pixRead(imagePath.c_str());
  if (!pix) {
    throw std::runtime_error("22: " + imagePath);
  }
  auto imageView = pixToImageView(pix);
  if (!imageView) {
    pixDestroy(&pix);
    throw std::runtime_error("11");
  }

  ZXing::ReaderOptions hints;
  hints.setFormats(ZXing::BarcodeFormat::Any);
  //hints.setTryHarder(true);
  //hints.setTryRotate(true);

  auto results = ZXing::ReadBarcodes(*imageView, hints);
  //ZXing::MultiFormatReader reader;

  for (const auto& barcodeResult : results) {
    if (barcodeResult.isValid()) {

      if (barcodeResult.isValid()) {
        std::string text = barcodeResult.text();
        pixDestroy(&pix);
        return text;
      }
    }
  }

  return "";
}


int main21()
{
  SetCurrentDirectory("D:/mv3/3rdParty/leptonica/prog");

  // std::string imagePath = "barcode-2of5-300.png"; // "defect1.png";
  //std::string imagePath = "barcode-39-300.png";
  std::string imagePath = "barcode-128-300.png";
  std::string result = decodeBarcode(imagePath);

  return 0;
}


