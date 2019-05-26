#pragma once

namespace FsUtils
{
    std::string GetSeparator();
    std::string PathJoin(const std::string& path1, const std::string& path2);
    std::string GetModuleDirectory(const std::string& modulePath);
}
