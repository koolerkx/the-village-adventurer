module;
#include <windows.h>

export module game.utils.encode;
import std;

export namespace encode {
  inline std::wstring Utf8ToUtf16(std::string_view s) {
    if (s.empty()) return L"";
    int len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                    s.data(), static_cast<int>(s.size()),
                                    nullptr, 0);
    if (len <= 0) throw std::runtime_error("Utf8ToUtf16: invalid UTF-8");

    std::wstring out(static_cast<size_t>(len), L'\0');
    int written = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        s.data(), static_cast<int>(s.size()),
                                        out.data(), len);
    if (written != len) throw std::runtime_error("Utf8ToUtf16: conversion failed");
    return out;
  }
}
