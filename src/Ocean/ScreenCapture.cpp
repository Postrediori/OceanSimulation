#include "stdafx.h"
#include "ScreenCapture.h"

std::string ScreenCapture::GetNextFileName(ScreenCaptureFormat format) {
    static int counter = 0;
    std::stringstream s;
    s << "capture" << (counter++);
    if (format == ScreenCaptureFormat::Png) {
        s << ".png";
    }
    else if (format == ScreenCaptureFormat::Jpg) {
        s << ".jpg";
    }
    return s.str();
}

bool ScreenCapture::SaveToFile(ScreenCaptureFormat format, int width, int height) {
    static const size_t ScreenCaptureBpp = 3;
    static const GLenum ScreenCaptureFormat = GL_RGB;

    std::vector<uint8_t> pixelBuffer(width * height * ScreenCaptureBpp);
    glReadPixels(0, 0, width, height, ScreenCaptureFormat, GL_UNSIGNED_BYTE, pixelBuffer.data());

    stbi_flip_vertically_on_write(1); // flag is non-zero to flip data vertically

    std::string fileName = ScreenCapture::GetNextFileName(format);

    static const int ScreenCaptureComp = ScreenCaptureBpp; // 1=Y, 2=YA, 3=RGB, 4=RGBA
    int result = 0;

    if (format == ScreenCaptureFormat::Png) {
        int screenCaptureStride = width * ScreenCaptureBpp;
        result = stbi_write_png(fileName.c_str(), width, height, ScreenCaptureComp, pixelBuffer.data(), screenCaptureStride);
    }
    else if (format == ScreenCaptureFormat::Jpg) {
        static const int ScreenCaptureQuality = 90;
        result = stbi_write_jpg(fileName.c_str(), width, height, ScreenCaptureComp, pixelBuffer.data(), ScreenCaptureQuality);
    }

    if (!result) {
        LOGE << "Unable to capture screen to file " << fileName;
        return false;
    }

    LOGI << "Saved screen capture to file " << fileName << " (" << width << "x" << height << " pixels)";

    return true;
}
