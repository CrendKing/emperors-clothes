#pragma once

#include "idle_time.h"


class Clothes {
    HWND _hClothesWindow;
    const IIdleTime *_idleTime;

public:
    Clothes(const IIdleTime *idleTime);

    unsigned int GetIdleTimeValue() const;

    void Pause();
    void Run();
};