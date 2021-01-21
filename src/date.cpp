#include "../include/date.hpp"

std::string
getISOCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    return date::format("%FT%TZ", date::floor<chrono::milliseconds>(now));
}

std::string
getISOCurrentDate() {
    return now = floor<chrono::days>(system_clock::now());
}