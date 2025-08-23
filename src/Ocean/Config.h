// Config.h
#ifndef CONFIG_H
#define CONFIG_H

class Config {
public:
    Config() = default;

    // clear all values
    void Clear();

    // load a Config file
    bool Load(const std::string& file);

    // check if value associated with given key exists
    bool Contains(const std::string& key) const;

    // get value associated with given key
    std::optional<std::string> GetString(const std::string& key) const;
    std::optional<int> GetInt(const std::string& key) const;
    std::optional<long> GetLong(const std::string& key) const;
    std::optional<float> GetFloat(const std::string& key) const;
    std::optional<bool> GetBool(const std::string& key) const;

private:
    std::optional<std::string> GetStringInternal(const std::string& key) const;

    static std::optional<bool> StringToBool(const std::string& value);

    // remove leading and trailing tabs and spaces
    static std::string Trim(const std::string& str);

private:
    // the container
    std::map<std::string, std::string> data;

};

#endif
