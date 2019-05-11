#include "clothes.h"
#include "constants.h"


inline void ShowCursor() {
    while (ShowCursor(TRUE) < 0);
}

inline void HideCursor() {
    while (ShowCursor(FALSE) >= 0);
}

DWORD WINAPI ClothesThreadProc(LPVOID lpParameter) {
    Clothes *_clothes = static_cast<Clothes *>(lpParameter);

    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = _clothes->GetModule();
    wc.lpszClassName = CLOTHES_WINDOW_CLASS_NAME;

    if (RegisterClass(&wc) == FALSE) {
        return 1;
    }

    HWND hClothesWindow = CreateWindowEx(WS_EX_LAYERED, CLOTHES_WINDOW_CLASS_NAME, nullptr, WS_CHILD | WS_DISABLED,
        0, 0, 0, 0,
        _clothes->GetParentWindow(), nullptr, _clothes->GetModule(), nullptr);
    if (hClothesWindow == nullptr) {
        SetEvent(_clothes->GetCreateWindowEvent());
        return 1;
    }

    SetWindowLongPtr(hClothesWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(_clothes));
    _clothes->SetClothesWindow(hClothesWindow);
    SetEvent(_clothes->GetCreateWindowEvent());

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

void CALLBACK CheckLastInput(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    POINT currentPos;
    if (GetCursorPos(&currentPos) == FALSE) {
        return;
    }

    LASTINPUTINFO lastInput = { sizeof(lastInput) };
    GetLastInputInfo(&lastInput);
    DWORD idleTime = dwTime - lastInput.dwTime;

    Clothes *_clothes = reinterpret_cast<Clothes *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (idleTime > _clothes->GetIdleTimeValue()) {
        HideCursor();
    } else if (idleTime <= TIMER_INTERVAL) {
        ShowCursor();
    }
}

Clothes::Clothes(HMODULE hModule, const IIdleTime *idleTime)
    : _hModule(hModule)
    , _hCreateWindowEvent(nullptr)
    , _hParentWindow(nullptr)
    , _hClothesWindow(nullptr)
    , _idleTime(idleTime) {
}

HMODULE Clothes::GetModule() const {
    return _hModule;
}

HANDLE Clothes::GetCreateWindowEvent() const {
    return _hCreateWindowEvent;
}

HWND Clothes::GetParentWindow() const {
    return _hParentWindow;
}

unsigned int Clothes::GetIdleTimeValue() const {
    unsigned int _idleTimeValue;
    HRESULT hr = _idleTime->GetIdleTime(&_idleTimeValue);

    if (SUCCEEDED(hr)) {
        return _idleTimeValue;
    } else {
        return IDLE_TIME_DEFAULT;
    }
}

void Clothes::SetClothesWindow(HWND hClothesWindow) {
    _hClothesWindow = hClothesWindow;
}

void Clothes::Stop() {
    if (_hClothesWindow != nullptr) {
        DestroyWindow(_hClothesWindow);
    }
}

void Clothes::Pause() {
    if (_hClothesWindow != nullptr) {
        ShowCursor();
        ShowWindow(_hClothesWindow, SW_HIDE);
        KillTimer(_hClothesWindow, 0);
    }
}

void Clothes::Run() {
    HWND hParentWindow = GetForegroundWindow();
    if (hParentWindow == nullptr) {
        return;
    }

    RECT parentRect;
    if (GetWindowRect(hParentWindow, &parentRect) == FALSE) {
        return;
    }

    if (_hClothesWindow == nullptr) {
        _hCreateWindowEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        _hParentWindow = hParentWindow;

        CreateThread(nullptr, 0, ClothesThreadProc, this, 0, nullptr);
        WaitForSingleObject(_hCreateWindowEvent, INFINITE);
        CloseHandle(_hCreateWindowEvent);

        if (_hClothesWindow == nullptr) {
            return;
        }
    }

    int width = parentRect.right - parentRect.left;
    int height = parentRect.bottom - parentRect.top;
    SetWindowPos(_hClothesWindow, HWND_TOP,
        parentRect.left, parentRect.top, width, height,
        SWP_SHOWWINDOW);

    SetTimer(_hClothesWindow, TIMER_ID, TIMER_INTERVAL, CheckLastInput);
}