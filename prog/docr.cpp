
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <iostream>
#include <string>

class DigitRecognizer
{
private:
  tesseract::TessBaseAPI* api;

public:
  DigitRecognizer() {
    api = new tesseract::TessBaseAPI();
  }

  ~DigitRecognizer() {
    if (api) {
      api->End();
      delete api;
    }
  }

  // 初始化Tesseract
  bool init(const std::string& lang = "eng",
    const std::string& tessdata_path = "")
  {
    // 初始化Tesseract，如果tessdata_path为空，使用默认路径
    if (api->Init(tessdata_path.empty() ? nullptr : tessdata_path.c_str(),
      lang.c_str()) != 0) {
      std::cerr << "无法初始化Tesseract!" << std::endl;
      return false;
    }

    // 设置只识别数字
    api->SetVariable("tessedit_char_whitelist", "0123456789");

    // 设置页面分割模式（Page Segmentation Mode）
    api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

    std::cout << "Tesseract初始化成功，语言: " << lang << std::endl;
    return true;
  }

  // 设置识别字符白名单（数字和字母）
  void setWhitelist(const std::string& chars) {
    api->SetVariable("tessedit_char_whitelist", chars.c_str());
  }

  // 设置识别字符黑名单
  void setBlacklist(const std::string& chars) {
    api->SetVariable("tessedit_char_blacklist", chars.c_str());
  }

  // 识别图片文件
  std::string recognizeFromFile(const std::string& image_path) {
    Pix* image = pixRead(image_path.c_str());
    if (!image) {
      std::cerr << "无法加载图片: " << image_path << std::endl;
      return "";
    }

    return recognizeFromPix(image);
  }

  // 识别Pix图像
  std::string recognizeFromPix(Pix* image) {
    if (!image) {
      return "";
    }

    // 设置图像
    api->SetImage(image);

    // 获取识别结果
    char* text = api->GetUTF8Text();
    std::string result(text ? text : "");

    // 清理
    if (text) delete[] text;
    pixDestroy(&image);

    // 去除换行和空格
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());

    return result;
  }

  // 识别灰度图像
  std::string recognizeGrayscaleImage(const std::string& image_path) {
    // 加载图像
    Pix* pix = pixRead(image_path.c_str());
    if (!pix) {
      std::cerr << "无法加载图像: " << image_path << std::endl;
      return "";
    }

    // 转换为灰度图（如果还不是）
    Pix* gray_pix = nullptr;
    if (pixGetDepth(pix) == 32 || pixGetDepth(pix) == 24) {
      gray_pix = pixConvertRGBToGray(pix, 0.0, 0.0, 0.0);
      pixDestroy(&pix);
    }
    else if (pixGetDepth(pix) == 8) {
      gray_pix = pix;  // 已经是灰度图
    }
    else {
      // 其他深度，转换为8位灰度
      gray_pix = pixConvertTo8(pix, 0);
      pixDestroy(&pix);
    }

    // 可选：增强对比度
    Pix* enhanced = pixContrastTRC(gray_pix, 1.5);
    pixDestroy(&gray_pix);

    std::string result = recognizeFromPix(enhanced);
    return result;
  }

  // 获取识别置信度
  int getConfidence() {
    return api->MeanTextConf();
  }

  // 获取替代结果
  void getAlternativeResults() {
    tesseract::ResultIterator* ri = api->GetIterator();
    if (ri != nullptr) {
      do {
        const char* symbol = ri->GetUTF8Text(tesseract::RIL_SYMBOL);
        if (symbol) {
          float conf = ri->Confidence(tesseract::RIL_SYMBOL);
          std::cout << "符号: " << symbol << " 置信度: " << conf << std::endl;
          delete[] symbol;
        }
      } while (ri->Next(tesseract::RIL_SYMBOL));
      delete ri;
    }
  }
};

// 使用示例
int main()
{
  DigitRecognizer recognizer;

  // 初始化，指定tessdata路径（如果不在默认位置）
  if (!recognizer.init("eng", "/usr/share/tesseract-ocr/4.00/tessdata")) {
    return 1;
  }

  // 只识别数字
  recognizer.setWhitelist("0123456789");

  // 识别图片
  std::string result = recognizer.recognizeGrayscaleImage("digits.png");

  std::cout << "识别结果: " << result << std::endl;
  std::cout << "置信度: " << recognizer.getConfidence() << std::endl;

  return 0;
}

