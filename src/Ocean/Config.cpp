// Config.cpp
#include "stdafx.h"
#include "Config.h"

std::string ToLower(const std::string& str) {
    std::string s;
    s.resize(str.size());

    std::transform(str.begin(), str.end(),
        s.begin(), ::tolower);

    return s;
}

void Config::Clear() {
    data.clear();
}

bool Config::Load(const std::string& file) {
    std::ifstream inFile(file.c_str());

    if (!inFile.good()) {
        std::cout << "Cannot read Config file " << file << std::endl;
        return false;
    }

    while (inFile.good() && !inFile.eof()) {
        std::string line;
        std::getline(inFile, line);

        // filter out comments
        constexpr char CommentSymbol = '#';
        if (!line.empty()) {
            auto pos = line.find(CommentSymbol);

            if (pos != std::string::npos) {
                line = line.substr(0, pos);
            }
        }

        // split line into key and value
        constexpr char EqSymbol = '=';
        if (!line.empty()) {
            auto pos = line.find(EqSymbol);

            if (pos != std::string::npos) {
                std::string key     = Trim(line.substr(0, pos));
                std::string value   = Trim(line.substr(pos + 1));

                if (!key.empty() && !value.empty()) {
                    data[key] = value;
                }
            }
        }
    }

    return true;
}

bool Config::Contains(const std::string& key) const {
    return data.find(key) != data.end();
}

std::optional<std::string> Config::GetStringInternal(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator iter = data.find(key);

    if (iter != data.end()) {
        return iter->second;
    }

    return {};
}

std::optional<std::string> Config::GetString(const std::string& key) const {
    try {
        if (auto str = GetStringInternal(key)) {
            return *str;
        }
    }
    catch (const std::exception& ex) {
        LOGE << "Cannot get string value: " << ex.what();
    }

    return {};
}

std::optional<int> Config::GetInt(const std::string& key) const {
    try {
        if (auto str = GetStringInternal(key)) {
            return std::atoi(str->c_str());
        }
    }
    catch (const std::exception& ex) {
        LOGE << "Cannot parse int: " << ex.what();
    }

    return {};
}

std::optional<long> Config::GetLong(const std::string& key) const {
    try {
        if (auto str = GetStringInternal(key)) {
            return std::atol(str->c_str());
        }
    }
    catch (const std::exception& ex) {
        LOGE << "Cannot parse long: " << ex.what();
    }

    return {};
}

std::optional<float> Config::GetFloat(const std::string& key) const {
    try {
        if (auto str = GetStringInternal(key)) {
            return std::atof(str->c_str());
        }
    }
    catch (const std::exception& ex) {
        LOGE << "Cannot parse float: " << ex.what();
    }

    return {};
}

std::optional<bool> Config::GetBool(const std::string& key) const {
    try {
        if (auto str = GetStringInternal(key)) {
            return StringToBool(*str);
        }
    }
    catch (const std::exception& ex) {
        LOGE << "Cannot parse bool: " << ex.what();
    }

    return {};
}

std::string Config::Trim(const std::string& str) {
    static const std::string WhitespaceSymbols{ " \t" };

    auto first = str.find_first_not_of(WhitespaceSymbols);

    if (first != std::string::npos) {
        auto last = str.find_last_not_of(WhitespaceSymbols);

        return str.substr(first, last - first + 1);
    }

    return {};
}

std::optional<bool> Config::StringToBool(const std::string& value) {
    std::string s{ ToLower(value) };

    if (s == "true" || s == "yes" || s == "on") {
        return true;
    }
    else if (s == "false" || s == "no" || s == "off") {
        return false;
    }

    return {};
}
