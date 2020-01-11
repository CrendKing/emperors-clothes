#include "pch.h"
#include "emperor.h"
#include "constants.h"
#include "emperors_prop.h"


const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,        // Major CLSID
    &MEDIASUBTYPE_NULL       // Minor type
};

const AMOVIESETUP_PIN psudPins[] =
{
    { nullptr,               // Pin's string name (obsolete)
      FALSE,                 // Is it rendered
      FALSE,                 // Is it an output
      FALSE,                 // Allowed none
      FALSE,                 // Allowed many
      &CLSID_NULL,           // Connects to filter (obsolete)
      nullptr,               // Connects to pin (obsolete)
      1,                     // Number of types
      &sudPinTypes },        // Pin information
    { nullptr,               // Pin's string name (obsolete)
      FALSE,                 // Is it rendered
      TRUE,                  // Is it an output
      FALSE,                 // Allowed none
      FALSE,                 // Allowed many
      &CLSID_NULL,           // Connects to filter (obsolete)
      nullptr,               // Connects to pin (obsolete)
      1,                     // Number of types
      &sudPinTypes }         // Pin information
};

const AMOVIESETUP_FILTER sudEmperorsClothes =
{
    &CLSID_EmperorsClothes,  // CLSID of filter
    FILTER_NAME_WIDE,        // Filter's name
    MERIT_DO_NOT_USE,        // Filter merit
    2,                       // Number of pins
    psudPins                 // Pin information
};

CFactoryTemplate g_Templates[2] = {
    { FILTER_NAME_WIDE
    , &CLSID_EmperorsClothes
    , CEmperorFilter::CreateInstance
    , nullptr
    , &sudEmperorsClothes }
    ,
    { PROPERTY_PAGE_NAME_WIDE
    , &CLSID_PropertyPage
    , CEmperorsProp::CreateInstance
    , nullptr
    , nullptr }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

CUnknown *WINAPI CEmperorFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr) {
    return new CEmperorFilter(pUnk, phr);
}

CEmperorFilter::CEmperorFilter(LPUNKNOWN pUnk, HRESULT *phr)
    : CTransInPlaceFilter(FILTER_NAME, pUnk, CLSID_EmperorsClothes, phr)
    , _clothes(this)
    , _idleTimeValue(_registry.ReadIdleTime()) {
}

STDMETHODIMP CEmperorFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
    CheckPointer(ppv, E_POINTER);

    if (riid == IID_IIdleTime) {
        return GetInterface(static_cast<IIdleTime *>(this), ppv);
    } else if (riid == IID_ISpecifyPropertyPages) {
        return GetInterface(static_cast<ISpecifyPropertyPages *>(this), ppv);
    } else {
        return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
    }
}

HRESULT CEmperorFilter::Transform(IMediaSample *pSample) {
    return S_OK;
}

HRESULT CEmperorFilter::CheckInputType(const CMediaType *mtIn) {
    return S_OK;
}

STDMETHODIMP CEmperorFilter::Pause() {
    CAutoLock lock(m_pLock);

    _clothes.Pause();

    return CTransInPlaceFilter::Pause();
}

STDMETHODIMP CEmperorFilter::Run(REFERENCE_TIME tStart) {
    CAutoLock lock(m_pLock);

    _clothes.Run();

    return CTransInPlaceFilter::Run(tStart);
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

////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration
//
////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterServer() {
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer() {
    return AMovieDllRegisterServer2(FALSE);
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return DllEntryPoint(hModule, ul_reason_for_call, lpReserved);
}