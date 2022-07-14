
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#endif

#include "stdafx.h"
#include "GraphicsLogger.h"
#include "ScreenCapture.h"

std::string ScreenCapture::GetNextFileName(Format format) {
    static int counter = 0;
    std::stringstream s;
    s << "capture" << (counter++);

    auto const formatExtension = [format]() {
        switch (format) {
        case Format::Png: return ".png";
        case Format::Jpg: return ".jpg";
        default: return "";
        }
    }();
    s << formatExtension;

    return s.str();
}

bool ScreenCapture::SaveToFile(Format format, int width, int height) {
    constexpr size_t ScreenCaptureBpp = 3;
    constexpr GLenum ScreenCaptureFormat = GL_RGB;

    std::vector<uint8_t> pixelBuffer(width * height * ScreenCaptureBpp);
    glReadPixels(0, 0, width, height, ScreenCaptureFormat, GL_UNSIGNED_BYTE, pixelBuffer.data()); LOGOPENGLERROR();

    stbi_flip_vertically_on_write(1); // flag is non-zero to flip data vertically

    std::string fileName = ScreenCapture::GetNextFileName(format);

    auto result = [=]() {
        constexpr int JpegQuality = 90;
        const int comp = ScreenCaptureBpp; // 1=Y, 2=YA, 3=RGB, 4=RGBA
        const int stride = width * ScreenCaptureBpp;

        switch (format) {
        case Format::Png:
            return stbi_write_png(fileName.c_str(), width, height, comp, pixelBuffer.data(), stride);
        case Format::Jpg:
            return stbi_write_jpg(fileName.c_str(), width, height, comp, pixelBuffer.data(), JpegQuality);
        default:
            return 0;
        }
    }();

    if (!result) {
        LOGE << "Unable to capture screen to file " << fileName;
        return false;
    }

    LOGI << "Saved screen capture to file " << fileName << " (" << width << "x" << height << " pixels)";

    return true;
}
