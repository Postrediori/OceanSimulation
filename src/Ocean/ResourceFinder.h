#pragma once

namespace Utils
{
    namespace ResourceFinder
    {
        using DirectoryList = std::vector<std::filesystem::path>;

        DirectoryList GetDataDirectoryList(const std::string& argv_path);
        bool LookForDataDir(const DirectoryList& dirs_for_lookup, std::filesystem::path& found_dir);

        bool GetDataDirectory(const std::string& argv_path, std::filesystem::path& found_dir);
    }
}
