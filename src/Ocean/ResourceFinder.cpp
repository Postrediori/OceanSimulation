#include "stdafx.h"
#include "ResourceFinder.h"

const std::filesystem::path DataDirName = "data";

Utils::ResourceFinder::DirectoryList Utils::ResourceFinder::GetDataDirectoryList(const std::string& argv_path) {
    const auto current_directory{ std::filesystem::current_path() };

    const auto proc_path = std::filesystem::canonical(argv_path);
    const auto proc_directory = proc_path.parent_path();

    return Utils::ResourceFinder::DirectoryList{
        current_directory,
        proc_directory
    };
}

bool Utils::ResourceFinder::LookForDataDir(const Utils::ResourceFinder::DirectoryList& paths_for_lookup,
        std::filesystem::path& found_path) {

    for (const auto& p : paths_for_lookup) {
        std::filesystem::path data_path = p / DataDirName;
        if (std::filesystem::exists(data_path) &&
            std::filesystem::is_directory(data_path)) {
            found_path = data_path;
            return true;
        }
    }

    found_path = std::filesystem::path(); // empty path
    return false;
}

bool Utils::ResourceFinder::GetDataDirectory(const std::string& argv_path,
        std::filesystem::path& found_path) {
    auto path_list = Utils::ResourceFinder::GetDataDirectoryList(argv_path);
    return Utils::ResourceFinder::LookForDataDir(path_list, found_path);
}
