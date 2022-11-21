//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <cstdio>
#include <cstdlib>

template<typename... Args>
void DebugLog(const char *format, Args... args) {
    fprintf(stderr, format, args...);
    fputc('\n', stderr);
}

template<typename... Args>
bool DebugCheck(const bool succeeded, const char *failMessage, Args... args) {
    if (succeeded) return true;

    DebugLog(failMessage, args...);
    return false;
}

template<typename... Args>
void DebugCheckCritical(const bool succeeded, const char *failMessage, Args... args) {
    if (DebugCheck(succeeded, failMessage, args...)) return;

    exit(EXIT_FAILURE);
}