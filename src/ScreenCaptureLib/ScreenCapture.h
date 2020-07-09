#pragma once

enum class ScreenCaptureFormat {
    Png,
    Jpg,
};

class ScreenCapture {
public:
    static bool SaveToFile(ScreenCaptureFormat format, int width, int height);

private:
    static std::string GetNextFileName(ScreenCaptureFormat format);
};
