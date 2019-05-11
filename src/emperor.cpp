#include "emperor.h"
#include "constants.h"
#include "emperors_prop.h"


HMODULE moduleHandle = nullptr;

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
    : _inputPin(NAME("Input Pin"), this, phr, L"Input")
    , _outputPin(NAME("Output Pin"), this, phr, L"Output")
    , _allocator(nullptr)
    , _clothes(moduleHandle, this)
    , _idleTimeValue(_registry.ReadIdleTime())
    , CBaseFilter(FILTER_NAME, pUnk, this, CLSID_EmperorsClothes) {
}

STDMETHODIMP CEmperorFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
    CheckPointer(ppv, E_POINTER);

    if (riid == IID_IIdleTime) {
        return GetInterface(static_cast<IIdleTime *>(this), ppv);
    } else if (riid == IID_ISpecifyPropertyPages) {
        return GetInterface(static_cast<ISpecifyPropertyPages*>(this), ppv);
    } else {
        return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
    }
}

int CEmperorFilter::GetPinCount() {
    return 2;
}

CBasePin *CEmperorFilter::GetPin(int n) {
    switch (n) {
    case 0:
        return &_inputPin;
    case 1:
        return &_outputPin;
    default:
        return nullptr;
    }
}

STDMETHODIMP CEmperorFilter::Stop() {
    HRESULT hr = CBaseFilter::Stop();
    
    //_clothes.Stop();
    
    return hr;
}

STDMETHODIMP CEmperorFilter::Pause() {
    CAutoLock cObjectLock(m_pLock);
    HRESULT hr = CBaseFilter::Pause();

    if (_inputPin.IsConnected() == FALSE) {
        _inputPin.EndOfStream();
    }

    _clothes.Pause();

    return hr;
}

STDMETHODIMP CEmperorFilter::Run(REFERENCE_TIME tStart) {
    CAutoLock cObjectLock(m_pLock);
    HRESULT hr = CBaseFilter::Run(tStart);

    if (_inputPin.IsConnected() == FALSE) {
        _inputPin.EndOfStream();
    }

    _clothes.Run();

    return hr;
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

CEmperorInputPin::CEmperorInputPin(TCHAR *pObjectName, CEmperorFilter *pFilter, HRESULT *phr, LPCWSTR pPinName)
    : CBaseInputPin(pObjectName, pFilter, pFilter, phr, pPinName)
    , _filter(pFilter)
    , _insideCheckMediaType(false) {
    ASSERT(pFilter);
}

STDMETHODIMP CEmperorInputPin::CheckMediaType(const CMediaType *pmt) {
    CAutoLock lock(m_pLock);

    // If we are already inside CheckMediaType() for this pin, return NOERROR
    // It is possble to hookup two of the tee filters and some other filter
    // like the video effects sample to get into this situation. If we don't
    // detect this situation, we will carry on looping till we blow the stack

    if (_insideCheckMediaType) {
        return NOERROR;
    }

    _insideCheckMediaType = true;

    // The media types that we can support are entirely dependent on the
    // downstream connections. If we have downstream connection, we should check with it

    if (_filter->_outputPin.m_Connected != nullptr) {
        // The pin is connected, check its peer
        HRESULT hr = _filter->_outputPin.m_Connected->QueryAccept(pmt);
        if (FAILED(hr)) {
            _insideCheckMediaType = false;
            return VFW_E_TYPE_NOT_ACCEPTED;
        }
    }

    _insideCheckMediaType = false;
    return NOERROR;
}

STDMETHODIMP CEmperorInputPin::BreakConnect() {
    if (_filter->_allocator) {
        _filter->_allocator->Release();
        _filter->_allocator = nullptr;
    }

    return CBaseInputPin::BreakConnect();
}

STDMETHODIMP CEmperorInputPin::CompleteConnect(IPin *pReceivePin) {
    ASSERT(pReceivePin);

    HRESULT hr = CBaseInputPin::CompleteConnect(pReceivePin);
    if (FAILED(hr)) {
        return hr;
    }

    if (_filter->_outputPin.m_Connected != nullptr) {
        if (m_mt != _filter->_outputPin.m_mt) {
            _filter->ReconnectPin(&_filter->_outputPin, &m_mt);
        }
    }

    return NOERROR;
}

STDMETHODIMP CEmperorInputPin::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly) {
    CheckPointer(pAllocator, E_FAIL);
    CAutoLock lock(m_pLock);

    if (_filter->_allocator) {
        _filter->_allocator->Release();
    }

    pAllocator->AddRef();
    _filter->_allocator = pAllocator;

    return CBaseInputPin::NotifyAllocator(pAllocator, bReadOnly);
}

STDMETHODIMP CEmperorInputPin::Receive(IMediaSample *pSample) {
    ASSERT(pSample);
    CAutoLock lock(m_pLock);

    HRESULT hr = CBaseInputPin::Receive(pSample);
    if (FAILED(hr)) {
        return hr;
    }

    return _filter->_outputPin.Deliver(pSample);
}

STDMETHODIMP CEmperorInputPin::EndOfStream() {
    CAutoLock lock(m_pLock);
    return _filter->_outputPin.DeliverEndOfStream();
}

STDMETHODIMP CEmperorInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
    CAutoLock lock(m_pLock);
    return _filter->_outputPin.DeliverNewSegment(tStart, tStop, dRate);
}

STDMETHODIMP CEmperorInputPin::BeginFlush() {
    CAutoLock lock(m_pLock);
    return _filter->_outputPin.DeliverBeginFlush();
}

STDMETHODIMP CEmperorInputPin::EndFlush() {
    CAutoLock lock(m_pLock);
    return _filter->_outputPin.DeliverEndFlush();
}

CEmperorOutputPin::CEmperorOutputPin(TCHAR *pObjectName, CEmperorFilter *pFilter, HRESULT *phr, LPCWSTR pPinName)
    : CBaseOutputPin(pObjectName, pFilter, pFilter, phr, pPinName)
    , _filter(pFilter)
    , _insideCheckMediaType(false) {
    ASSERT(pFilter);
}

STDMETHODIMP CEmperorOutputPin::EnumMediaTypes(IEnumMediaTypes **ppEnum) {
    CAutoLock lock(m_pLock);
    ASSERT(ppEnum);

    if (_filter->_inputPin.m_Connected == nullptr) {
        return VFW_E_NOT_CONNECTED;
    }

    return _filter->_inputPin.m_Connected->EnumMediaTypes(ppEnum);
}

STDMETHODIMP CEmperorOutputPin::CheckMediaType(const CMediaType *pmt) {
    CAutoLock lock(m_pLock);

    // If we are already inside CheckMediaType() for this pin, return NOERROR
    // It is possble to hookup two of the filters and some other filter
    // like the video effects sample to get into this situation. If we
    // do not detect this, we will loop till we blow the stack

    if (_insideCheckMediaType) {
        return NOERROR;
    }

    _insideCheckMediaType = true;

    // The input needs to have been conneced first
    if (_filter->_inputPin.m_Connected == nullptr) {
        _insideCheckMediaType = false;
        return VFW_E_NOT_CONNECTED;
    }

    // Make sure that our input pin peer is happy with this
    HRESULT hr = _filter->_inputPin.m_Connected->QueryAccept(pmt);
    if (FAILED(hr)) {
        _insideCheckMediaType = false;
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    if (_filter->_outputPin.m_Connected != nullptr) {
        // The pin is connected, check its peer
        hr = _filter->_outputPin.m_Connected->QueryAccept(pmt);
        if (FAILED(hr)) {
            _insideCheckMediaType = false;
            return VFW_E_TYPE_NOT_ACCEPTED;
        }
    }

    _insideCheckMediaType = false;
    return NOERROR;
}

STDMETHODIMP CEmperorOutputPin::CompleteConnect(IPin *pReceivePin) {
    CAutoLock lock(m_pLock);
    ASSERT(m_Connected == pReceivePin);

    HRESULT hr = CBaseOutputPin::CompleteConnect(pReceivePin);
    if (FAILED(hr)) {
        return hr;
    }

    if (m_mt != _filter->_inputPin.m_mt) {
        hr = _filter->ReconnectPin(_filter->_inputPin.m_Connected, &m_mt);
        if (FAILED(hr)) {
            return hr;
        }
    }

    return NOERROR;
}

STDMETHODIMP CEmperorOutputPin::DecideAllocator(IMemInputPin *pPin, IMemAllocator **pAlloc) {
    CheckPointer(pPin, E_POINTER);
    CheckPointer(pAlloc, E_POINTER);
    ASSERT(_filter->_allocator != nullptr);

    *pAlloc = nullptr;

    HRESULT hr = pPin->NotifyAllocator(_filter->_allocator, TRUE);
    if (FAILED(hr)) {
        return hr;
    }

    *pAlloc = _filter->_allocator;
    _filter->_allocator->AddRef();
    return NOERROR;
}

STDMETHODIMP CEmperorOutputPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest) {
    return NOERROR;
}

STDMETHODIMP_(HRESULT __stdcall) CEmperorOutputPin::Notify(IBaseFilter * pSender, Quality q) {
    // We pass the message on, which means that we find the quality sink
    // for our input pin and send it there

    if (_filter->_inputPin.m_pQSink != nullptr) {
        return _filter->_inputPin.m_pQSink->Notify(_filter, q);
    } else {
        // No sink set, so pass it upstream
        IQualityControl *pIQC;

        HRESULT hr = VFW_E_NOT_FOUND;
        if (_filter->_inputPin.m_Connected) {
            _filter->_inputPin.m_Connected->QueryInterface(IID_IQualityControl, (void**)&pIQC);

            if (pIQC != nullptr) {
                hr = pIQC->Notify(_filter, q);
                pIQC->Release();
            }
        }
        return hr;
    }

    // Quality management is too hard to do
    return NOERROR;
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
    if (moduleHandle == nullptr) {
        moduleHandle = hModule;
    }

    return DllEntryPoint(hModule, ul_reason_for_call, lpReserved);
}