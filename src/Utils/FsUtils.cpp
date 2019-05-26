#include "stdafx.h"
#include "FsUtils.h"

std::string FsUtils::GetSeparator()
{
    return "/";
}

std::string FsUtils::PathJoin(const std::string& path1, const std::string& path2)
{
    return path1 + FsUtils::GetSeparator() + path2;
}

std::string FsUtils::GetModuleDirectory(const std::string& modulePath)
{
    std::string s(modulePath);
    std::replace(s.begin(), s.end(), '\\', '/');
    return s.substr(0, s.find_last_of(FsUtils::GetSeparator()));
}
