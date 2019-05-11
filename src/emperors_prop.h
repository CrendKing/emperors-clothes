#pragma once

#include <streams.h>
#include "idle_time.h"


class CEmperorsProp : public CBasePropertyPage {
    IIdleTime *_idleTime;
    unsigned int _idleTimeValue;

    HRESULT OnConnect(IUnknown *pUnk);
    HRESULT OnDisconnect();
    HRESULT OnActivate();
    HRESULT OnApplyChanges();
    INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void SetDirty();

public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
    CEmperorsProp(LPUNKNOWN pUnk, HRESULT *phr);
};