#pragma once

namespace ScreenCapture {

enum class Format {
    Png,
    Jpg,
};

bool SaveToFile(Format format, int width, int height);

std::string GetNextFileName(Format format);

}
