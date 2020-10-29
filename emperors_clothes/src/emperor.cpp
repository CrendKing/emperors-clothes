#include "pch.h"
#include "emperor.h"
#include "g_constants.h"
#include "filter_prop.h"


#ifdef _DEBUG
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "strmbase.lib")
#endif
#pragma comment(lib, "winmm.lib")

static constexpr REGPINTYPES PIN_TYPE_REG[] = {
    { &MEDIATYPE_Video, &MEDIASUBTYPE_NULL },
};
static constexpr UINT PIN_TYPE_COUNT = sizeof(PIN_TYPE_REG) / sizeof(PIN_TYPE_REG[0]);

static constexpr REGFILTERPINS PIN_REG[] = {
    { nullptr          // pin name (obsolete)
    , FALSE            // is pin rendered?
    , FALSE            // is this output pin?
    , FALSE            // Can the filter create zero instances?
    , FALSE            // Does the filter create multiple instances?
    , &CLSID_NULL      // filter CLSID the pin connects to (obsolete)
    , nullptr          // pin name the pin connects to (obsolete)
    , PIN_TYPE_COUNT   // pin media type count
    , PIN_TYPE_REG },  // pin media types

    { nullptr          // pin name (obsolete)
    , FALSE            // is pin rendered?
    , TRUE            // is this output pin?
    , FALSE            // Can the filter create zero instances?
    , FALSE            // Does the filter create multiple instances?
    , &CLSID_NULL      // filter CLSID the pin connects to (obsolete)
    , nullptr          // pin name the pin connects to (obsolete)
    , PIN_TYPE_COUNT   // pin media type count
    , PIN_TYPE_REG },  // pin media types
};

static constexpr ULONG PIN_COUNT = sizeof(PIN_REG) / sizeof(PIN_REG[0]);

static constexpr AMOVIESETUP_FILTER FILTER_REG = {
    &CLSID_EmperorsClothes,  // filter CLSID
    FILTER_NAME_WIDE,       // filter name
    MERIT_DO_NOT_USE,       // filter merit
    PIN_COUNT,              // pin count
    PIN_REG                 // pin information
};

CFactoryTemplate g_Templates[] = {
    { FILTER_NAME_WIDE
    , &CLSID_EmperorsClothes
    , CEmperorFilter::CreateInstance
    , nullptr
    , &FILTER_REG },

    { PROPERTY_PAGE_NAME_WIDE
    , &CLSID_PropertyPage
    , CEmperorsProp::CreateInstance
    , nullptr
    , nullptr },
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

STDAPI DllRegisterServer() {
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer() {
    return AMovieDllRegisterServer2(FALSE);
}

extern "C" DECLSPEC_NOINLINE BOOL WINAPI DllEntryPoint(HINSTANCE hInstance, ULONG ulReason, __inout_opt LPVOID pv);

auto APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) -> BOOL {
    return DllEntryPoint(hModule, ul_reason_for_call, lpReserved);
}

CUnknown *WINAPI CEmperorFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr) {
    return new CEmperorFilter(pUnk, phr);
}

CEmperorFilter::CEmperorFilter(LPUNKNOWN pUnk, HRESULT *phr)
    : CTransInPlaceFilter(FILTER_NAME, pUnk, CLSID_EmperorsClothes, phr, false)
    , _clothes(this)
    , _idleTimeValue(_registry.ReadIdleTime()) {
}

STDMETHODIMP CEmperorFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
    CheckPointer(ppv, E_POINTER);

    if (riid == IID_IIdleTime) {
        return GetInterface(static_cast<IIdleTime *>(this), ppv);
    } else if (riid == IID_ISpecifyPropertyPages) {
        return GetInterface(static_cast<ISpecifyPropertyPages *>(this), ppv);
    }

    return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CEmperorFilter::CheckInputType(const CMediaType *mtIn) {
    return S_OK;
}

HRESULT CEmperorFilter::Transform(IMediaSample *pSample) {
    return S_OK;
}

STDMETHODIMP CEmperorFilter::Run(REFERENCE_TIME tStart) {
    CAutoLock lock(m_pLock);

    _clothes.Start();

    return CTransInPlaceFilter::Run(tStart);
}

STDMETHODIMP CEmperorFilter::Pause() {
    CAutoLock lock(m_pLock);

    _clothes.Stop();

    return CTransInPlaceFilter::Pause();
}

STDMETHODIMP CEmperorFilter::GetPages(CAUUID *pPages) {
    if (pPages == nullptr) {
        return E_POINTER;
    }

    pPages->cElems = 1;
    pPages->pElems = static_cast<GUID *>(CoTaskMemAlloc(sizeof(GUID)));
    if (pPages->pElems == nullptr) {
        return E_OUTOFMEMORY;
    }

    pPages->pElems[0] = CLSID_PropertyPage;
    return S_OK;
}


STDMETHODIMP CEmperorFilter::GetIdleTime(unsigned int *idleTime) const {
    if (idleTime == nullptr) {
        return E_POINTER;
    }
    CAutoLock lock(m_pLock);

    *idleTime = _idleTimeValue;

    return S_OK;
}

STDMETHODIMP CEmperorFilter::UpdateIdleTime(unsigned int idleTime) {
    CAutoLock lock(m_pLock);

    if (idleTime < IDLE_TIME_MIN || idleTime > IDLE_TIME_MAX) {
        return E_INVALIDARG;
    }

    _idleTimeValue = idleTime;
    _registry.WriteIdleTime(idleTime);

    return S_OK;
}
