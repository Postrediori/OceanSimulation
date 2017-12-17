// Config.h
#ifndef CONFIG_H
#define CONFIG_H
 
class Config {
public:
    // clear all values
    void Clear();
 
    // load a Config file
    bool Load(const std::string& File);
 
    // check if value associated with given key exists
    bool Contains(const std::string& key) const;
 
    // get value associated with given key
    bool Get(const std::string& key, std::string& value) const;
    bool Get(const std::string& key, int&    value) const;
    bool Get(const std::string& key, long&   value) const;
    bool Get(const std::string& key, float& value) const;
    bool Get(const std::string& key, bool&   value) const;
 
private:
    // the container
    std::map<std::string, std::string> data;
 
    // remove leading and trailing tabs and spaces
    static std::string Trim(const std::string& str);
};

#endif
