#include "pch.h"
#include "clothes.h"
#include "g_constants.h"


static POINT lastCursorPos = {};
static DWORD lastCursorTime = 0;

inline void ShowCursor() {
    while (ShowCursor(TRUE) < 0);
}

inline void HideCursor() {
    while (ShowCursor(FALSE) >= 0);
}

void CALLBACK CheckLastInput(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    POINT curCursorPos;
    if (GetCursorPos(&curCursorPos) == FALSE) {
        return;
    }

    if (curCursorPos.x != lastCursorPos.x || curCursorPos.y != lastCursorPos.y) {
        lastCursorPos = curCursorPos;
        lastCursorTime = dwTime;
    }
    DWORD idleTime = dwTime - lastCursorTime;

    const Clothes *_clothes = reinterpret_cast<const Clothes *>(GetProp(hwnd, CLOTHES_WINDOW_PROP_NAME));
    if (idleTime > _clothes->GetIdleTimeValue()) {
        HideCursor();
    } else if (idleTime <= TIMER_INTERVAL) {
        ShowCursor();
    }
}

Clothes::Clothes(const IIdleTime *idleTime)
    : _hClothesWindow(nullptr)
    , _idleTimeInterface(idleTime) {
}

unsigned int Clothes::GetIdleTimeValue() const {
    unsigned int _idleTimeValue;
    HRESULT hr = _idleTimeInterface->GetIdleTime(&_idleTimeValue);

    if (SUCCEEDED(hr)) {
        return _idleTimeValue;
    } else {
        return IDLE_TIME_DEFAULT;
    }
}

void Clothes::Start() {
    _hClothesWindow = GetForegroundWindow();
    if (_hClothesWindow == nullptr) {
        return;
    }

    // check existing attribute to avoid conflict
    if (GetProp(_hClothesWindow, CLOTHES_WINDOW_PROP_NAME) == nullptr) {
        SetProp(_hClothesWindow, CLOTHES_WINDOW_PROP_NAME, this);
        SetTimer(_hClothesWindow, TIMER_ID, TIMER_INTERVAL, CheckLastInput);
    }
}

void Clothes::Stop() const {
    if (_hClothesWindow != nullptr) {
        ShowCursor();
        RemoveProp(_hClothesWindow, CLOTHES_WINDOW_PROP_NAME);
        KillTimer(_hClothesWindow, TIMER_ID);
    }
}