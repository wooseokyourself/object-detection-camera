#include "../include/Common.hpp"

/**
 * @brief Get timestamp as an ISO format.
 * @return Format is like "2021-01-08T13:41:21.046Z".
 */
std::string getISOCurrentTimestamp () {
    return date::format("%FT%TZ", date::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()));
}