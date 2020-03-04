#pragma once

#include "idle_time.h"


class Clothes {
public:
    Clothes(const IIdleTime *idleTime);

    unsigned int GetIdleTimeValue() const;

    void Start();
    void Stop() const;

private:
    HWND _hClothesWindow;
    const IIdleTime *_idleTimeInterface;
};