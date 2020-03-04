#pragma once

#include "clothes.h"
#include "registry.h"
#include "idle_time.h"

class CEmperorFilter
    : public CTransInPlaceFilter
    , public IIdleTime
    , public ISpecifyPropertyPages {
public:
    static CUnknown *WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);

    DECLARE_IUNKNOWN

    CEmperorFilter(LPUNKNOWN pUnk, HRESULT *phr);

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT Transform(IMediaSample *pSample);

    STDMETHODIMP Run(REFERENCE_TIME tStart);
    STDMETHODIMP Pause();

    STDMETHODIMP GetPages(CAUUID *pPages);
    STDMETHODIMP GetIdleTime(unsigned int *idleTime) const;
    STDMETHODIMP UpdateIdleTime(unsigned int idleTime);

private:
    Clothes _clothes;
    Registry _registry;
    unsigned int _idleTimeValue;
};
