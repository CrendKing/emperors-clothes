#include "pch.h"
#include "emperors_prop.h"
#include "constants.h"


CUnknown *WINAPI CEmperorsProp::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr) {
    return new CEmperorsProp(pUnk, phr);
}

CEmperorsProp::CEmperorsProp(LPUNKNOWN pUnk, HRESULT *phr)
    : CBasePropertyPage(PROPERTY_PAGE_NAME, pUnk, IDD_PROPPAGE, IDS_TITLE)
    , _idleTime(nullptr) {
}

HRESULT CEmperorsProp::OnConnect(IUnknown *pUnk) {
    if (pUnk == NULL) {
        return E_POINTER;
    }

    ASSERT(_idleTime == nullptr);
    return pUnk->QueryInterface(IID_IIdleTime, reinterpret_cast<void **>(&_idleTime));
}

HRESULT CEmperorsProp::OnDisconnect() {
    if (_idleTime) {
        _idleTime->Release();
        _idleTime = nullptr;
    }
    return S_OK;
}

HRESULT CEmperorsProp::OnActivate() {
    ASSERT(_idleTime != nullptr);

    HRESULT hr = _idleTime->GetIdleTime(&_idleTimeValue);
    if (SUCCEEDED(hr)) {
        UDACCEL accels = { 0, 500 };
        SendDlgItemMessage(m_Dlg, IDC_SPIN_IDLE_TIME, UDM_SETRANGE32, IDLE_TIME_MIN, IDLE_TIME_MAX);
        SendDlgItemMessage(m_Dlg, IDC_SPIN_IDLE_TIME, UDM_SETPOS32, 0, _idleTimeValue);
        SendDlgItemMessage(m_Dlg, IDC_SPIN_IDLE_TIME, UDM_SETACCEL, 1, reinterpret_cast<LPARAM>(&accels));
    }

    return hr;
}

HRESULT CEmperorsProp::OnApplyChanges() {
    _idleTime->UpdateIdleTime(_idleTimeValue);
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
