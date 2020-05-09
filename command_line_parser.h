#pragma once

#include <string>
#include <map>

class CommandLineParser
{
public:
    CommandLineParser(int argc, char* argv[]);

    bool is_valid() const;
    std::string get_path() const;
    const std::map<std::string, std::string> get_metadata() const;

private:
    std::string m_path;
    std::map<std::string, std::string> m_metadata;
    bool m_is_valid;
};

