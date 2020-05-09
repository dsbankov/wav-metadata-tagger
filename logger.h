#pragma once

#include <iostream>

namespace LOGGER
{
    inline void log_info(const std::string& msg)
    {
        std::cout << msg << std::endl;
    }

    inline void log_error(const std::string& msg)
    {
        std::cerr << msg << std::endl;
    }

    inline void log_debug(const std::string& msg)
    {
#ifdef _DEBUG
        std::cout << msg << std::endl;
#endif
    }
};

