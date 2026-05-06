#pragma once
#include <string>
#include <windows.h>

#include <string>
#include <windows.h>

inline std::wstring to_wstring(const std::string& utf8)
{
    if (utf8.empty()) return {};

    int size_needed = MultiByteToWideChar(
        CP_UTF8, 0,
        utf8.data(), (int)utf8.size(),
        nullptr, 0
    );

    std::wstring result(size_needed, 0);

    MultiByteToWideChar(
        CP_UTF8, 0,
        utf8.data(), (int)utf8.size(),
        result.data(), size_needed
    );

    return result;
}
