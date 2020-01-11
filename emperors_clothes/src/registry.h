#pragma once


class Registry {
public:
    Registry();
    ~Registry();

    unsigned int ReadIdleTime() const;
    void WriteIdleTime(unsigned int idleTime) const;

private:
    HKEY _registryKey;
};