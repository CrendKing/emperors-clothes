#include "pch.h"
#include "registry.h"
#include "g_constants.h"


Registry::Registry() : _registryKey(nullptr) {
    RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY_NAME, 0, nullptr, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, nullptr, &_registryKey, nullptr);
}

Registry::~Registry() {
    if (_registryKey) {
        RegCloseKey(_registryKey);
    }
}

unsigned int Registry::ReadIdleTime() const {
    if (_registryKey) {
        DWORD idleTime;
        DWORD valueSize = sizeof(idleTime);

        const LSTATUS registryStatus = RegGetValue(_registryKey, nullptr, REGISTRY_IDLE_TIME_VALUE_NAME, RRF_RT_REG_DWORD, nullptr, &idleTime, &valueSize);
        if (registryStatus == ERROR_SUCCESS) {
            if (idleTime < IDLE_TIME_MIN) {
                idleTime = IDLE_TIME_MIN;
            } else if (idleTime > IDLE_TIME_MAX) {
                idleTime = IDLE_TIME_MAX;
            }

            return idleTime;
        }
    }

    return IDLE_TIME_DEFAULT;
}

void Registry::WriteIdleTime(unsigned int idleTime) const {
    if (_registryKey) {
        RegSetValueEx(_registryKey, REGISTRY_IDLE_TIME_VALUE_NAME, 0, REG_DWORD, reinterpret_cast<BYTE *>(&idleTime), sizeof(idleTime));
    }
}
