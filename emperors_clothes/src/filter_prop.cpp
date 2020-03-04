#include "pch.h"
#include "filter_prop.h"
#include "g_constants.h"


CUnknown *WINAPI CEmperorsProp::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr) {
    return new CEmperorsProp(pUnk, phr);
}

CEmperorsProp::CEmperorsProp(LPUNKNOWN pUnk, HRESULT *phr)
    : CBasePropertyPage(PROPERTY_PAGE_NAME, pUnk, IDD_PROPPAGE, IDS_TITLE)
    , _idleTimeInterface(nullptr) {
}

HRESULT CEmperorsProp::OnConnect(IUnknown *pUnk) {
    if (pUnk == nullptr) {
        return E_POINTER;
    }

    ASSERT(_idleTimeInterface == nullptr);
    return pUnk->QueryInterface(IID_IIdleTime, reinterpret_cast<void **>(&_idleTimeInterface));
}

HRESULT CEmperorsProp::OnDisconnect() {
    if (_idleTimeInterface != nullptr) {
        _idleTimeInterface->Release();
        _idleTimeInterface = nullptr;
    }
    return S_OK;
}

HRESULT CEmperorsProp::OnActivate() {
    ASSERT(_idleTimeInterface != nullptr);

    HRESULT hr = _idleTimeInterface->GetIdleTime(&_idleTimeValue);
    if (SUCCEEDED(hr)) {
        UDACCEL accels = { 0, 500 };
        SendDlgItemMessage(m_Dlg, IDC_SPIN_IDLE_TIME, UDM_SETRANGE32, IDLE_TIME_MIN, IDLE_TIME_MAX);
        SendDlgItemMessage(m_Dlg, IDC_SPIN_IDLE_TIME, UDM_SETPOS32, 0, _idleTimeValue);
        SendDlgItemMessage(m_Dlg, IDC_SPIN_IDLE_TIME, UDM_SETACCEL, 1, reinterpret_cast<LPARAM>(&accels));
    }

    return hr;
}

HRESULT CEmperorsProp::OnApplyChanges() {
    _idleTimeInterface->UpdateIdleTime(_idleTimeValue);
    return S_OK;
}

INT_PTR CEmperorsProp::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDC_EDIT_IDLE_TIME && HIWORD(wParam) == EN_CHANGE) {
            unsigned int newValue = GetDlgItemInt(hwnd, IDC_EDIT_IDLE_TIME, nullptr, FALSE);
            if (newValue < IDLE_TIME_MIN || newValue > IDLE_TIME_MAX) {
                SendDlgItemMessage(m_Dlg, IDC_SPIN_IDLE_TIME, UDM_SETPOS32, 0, newValue);
            } else if (_idleTimeValue != newValue) {
                _idleTimeValue = newValue;
                SetDirty();
            }
            return static_cast<LRESULT>(true);
        }
        break;
    }
    case WM_NOTIFY:
    {
        LPNMUPDOWN upDown = reinterpret_cast<LPNMUPDOWN>(lParam);
        if (upDown->hdr.idFrom == IDC_SPIN_IDLE_TIME && upDown->hdr.code == UDN_DELTAPOS) {
            unsigned int newValue = upDown->iPos + upDown->iDelta;
            if (newValue >= IDLE_TIME_MIN && newValue <= IDLE_TIME_MAX && _idleTimeValue != newValue) {
                _idleTimeValue = newValue;
                SetDirty();
            }
            return static_cast<LRESULT>(true);
        }
    }
    }

    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

void CEmperorsProp::SetDirty() {
    m_bDirty = TRUE;
    if (m_pPageSite) {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
}
