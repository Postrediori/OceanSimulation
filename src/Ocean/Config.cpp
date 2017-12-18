// Config.cpp
#include "stdafx.h"
#include "Config.h"

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
        if (!line.empty()) {
            int pos = line.find('#');
 
            if (pos != std::string::npos) {
                line = line.substr(0, pos);
            }
        }
 
        // split line into key and value
        if (!line.empty()) {
            int pos = line.find('=');
 
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
 
bool Config::Get(const std::string& key, std::string& value) const {
    std::map<std::string, std::string>::const_iterator iter = data.find(key);
 
    if (iter != data.end()) {
        value = iter->second;
        return true;
    } else {
        return false;
    }
}
 
bool Config::Get(const std::string& key, int& value) const {
    std::string str;
 
    if (Get(key, str)) {
        value = atoi(str.c_str());
        return true;
    } else {
        return false;
    }
}
 
bool Config::Get(const std::string& key, long& value) const {
    std::string str;
 
    if (Get(key, str)) {
        value = atol(str.c_str());
        return true;
    } else {
        return false;
    }
}
 
bool Config::Get(const std::string& key, float& value) const {
    std::string str;
 
    if (Get(key, str)) {
        value = atof(str.c_str());
        return true;
    } else {
        return false;
    }
}
 
bool Config::Get(const std::string& key, bool& value) const {
    std::string str;
 
    if (Get(key, str)) {
        value = (str == "true");
        return true;
    } else {
        return false;
    }
}
 
std::string Config::Trim(const std::string& str) {
    int first = str.find_first_not_of(" \t");
 
    if (first != std::string::npos) {
        int last = str.find_last_not_of(" \t");
 
        return str.substr(first, last - first + 1);
    } else {
        return "";
    }
}
