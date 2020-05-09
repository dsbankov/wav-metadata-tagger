#include "command_line_parser.h"
#include "logger.h"

CommandLineParser::CommandLineParser(int argc, char* argv[])
{
    m_is_valid = true;
    for (int i = 1; i < argc; i++)
    {
        std::string argv_str = argv[i];
        if (argv_str == "--path")
        {
            i++;
            if (i < argc)
            {
                m_path = argv[i];
            }
            else
            {
                LOGGER::log_error("No path argument found.");
                m_is_valid = false;
                break;
            }
        }
        else if (argv_str == "--metadata")
        {
            i++;
            if (i < argc)
            {
                std::string key_value = argv[i];
                size_t delimiter_idx = key_value.find('=');
                if (delimiter_idx != std::string::npos)
                {
                    std::string key = key_value.substr(0, delimiter_idx);
                    std::string value = key_value.substr(delimiter_idx + 1);
                    m_metadata[key] = value;
                }
            }
            else
            {
                LOGGER::log_error("No metadata argument found.");
                m_is_valid = false;
                break;
            }
        }
        else
        {
            LOGGER::log_error("Unsupported '" + argv_str + "' argument found.");
            m_is_valid = false;
            break;
        }
    }
};

bool CommandLineParser::is_valid() const
{
    return m_is_valid;
}

std::string CommandLineParser::get_path() const
{
    return m_path;
}

const std::map<std::string, std::string> CommandLineParser::get_metadata() const
{
    return m_metadata;
}