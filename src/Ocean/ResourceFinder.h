#pragma once

namespace Utils
{
    namespace ResourceFinder
    {
        using DirectoryList = std::vector<std::filesystem::path>;

        DirectoryList GetDataDirectoryList(const std::string& argv_path);
        std::optional<std::filesystem::path> LookForDataDir(const DirectoryList& dirs_for_lookup);

        std::optional<std::filesystem::path> GetDataDirectory(const std::string& argv_path);
    }
}
