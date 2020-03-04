#pragma once

#include "idle_time.h"


class CEmperorsProp : public CBasePropertyPage {
public:
    static CUnknown *WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
    CEmperorsProp(LPUNKNOWN pUnk, HRESULT *phr);

private:
    HRESULT OnConnect(IUnknown *pUnk);
    HRESULT OnDisconnect();
    HRESULT OnActivate();
    HRESULT OnApplyChanges();
    INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void SetDirty();

    IIdleTime *_idleTimeInterface;
    unsigned int _idleTimeValue;
};
