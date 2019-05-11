#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


class Registry {
    HKEY _registryKey;

public:
    Registry();
    ~Registry();

    unsigned int ReadIdleTime() const;
    void WriteIdleTime(unsigned int idleTime) const;
};