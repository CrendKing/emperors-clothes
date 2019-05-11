#pragma once

#include "idle_time.h"


class Clothes {
    HMODULE _hModule;
    HANDLE _hCreateWindowEvent;
    HWND _hParentWindow;
    HWND _hClothesWindow;
    const IIdleTime *_idleTime;

public:
    Clothes(HMODULE hModule, const IIdleTime *idleTime);

    HMODULE GetModule() const;
    HANDLE GetCreateWindowEvent() const;
    HWND GetParentWindow() const;
    unsigned int GetIdleTimeValue() const;

    void SetClothesWindow(HWND hClothesWindow);

    void Stop();
    void Pause();
    void Run();
};