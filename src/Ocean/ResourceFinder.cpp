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

std::optional<std::filesystem::path> Utils::ResourceFinder::LookForDataDir(
        const Utils::ResourceFinder::DirectoryList& paths_for_lookup) {

    for (const auto& p : paths_for_lookup) {
        std::filesystem::path data_path = p / DataDirName;
        if (std::filesystem::exists(data_path) &&
            std::filesystem::is_directory(data_path)) {
            return data_path;
        }
    }

    return {}; // empty path
}

std::optional<std::filesystem::path> Utils::ResourceFinder::GetDataDirectory(const std::string& argv_path) {
    auto path_list = Utils::ResourceFinder::GetDataDirectoryList(argv_path);
    return Utils::ResourceFinder::LookForDataDir(path_list);
}
