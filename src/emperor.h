#pragma once

#include <streams.h>
#include "clothes.h"
#include "registry.h"
#include "idle_time.h"


class CEmperorFilter;
class CEmperorOutputPin;

class CEmperorInputPin : public CBaseInputPin {
    friend class CEmperorOutputPin;

    CEmperorFilter *_filter;
    bool _insideCheckMediaType;

public:
    CEmperorInputPin(TCHAR *pObjectName, CEmperorFilter *pFilter, HRESULT *phr, LPCWSTR pPinName);

    STDMETHODIMP CheckMediaType(const CMediaType *pmt);
    STDMETHODIMP BreakConnect();
    STDMETHODIMP CompleteConnect(IPin *pReceivePin);

    STDMETHODIMP NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly);
    STDMETHODIMP Receive(IMediaSample *pSample);

    STDMETHODIMP EndOfStream();
    STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    STDMETHODIMP BeginFlush();
    STDMETHODIMP EndFlush();
};

class CEmperorOutputPin : public CBaseOutputPin {
    friend class CEmperorInputPin;

    CEmperorFilter *_filter;
    IUnknown *_position;
    bool _holdsSeek;
    bool _insideCheckMediaType;

public:
    CEmperorOutputPin(TCHAR *pObjectName, CEmperorFilter *pFilter, HRESULT *phr, LPCWSTR pPinName);

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
    STDMETHODIMP_(ULONG) NonDelegatingRelease();

    STDMETHODIMP EnumMediaTypes(IEnumMediaTypes **ppEnum);
    STDMETHODIMP CheckMediaType(const CMediaType *pmt);

    STDMETHODIMP CompleteConnect(IPin *pReceivePin);
    STDMETHODIMP DecideAllocator(IMemInputPin *pPin, IMemAllocator **pAlloc);
    STDMETHODIMP DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES * ppropInputRequest);

    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
};

class CEmperorFilter
    : public CBaseFilter
    , public CCritSec
    , public IIdleTime
    , public ISpecifyPropertyPages {
    friend class CEmperorInputPin;
    friend class CEmperorOutputPin;

    CEmperorInputPin _inputPin;
    CEmperorOutputPin _outputPin;
    LONG _canSeek;              // Seekable output pin
    IMemAllocator *_allocator;  // Allocator from the input pin

    Clothes _clothes;
    Registry _registry;
    unsigned int _idleTimeValue;

public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);

    DECLARE_IUNKNOWN

    CEmperorFilter(LPUNKNOWN pUnk, HRESULT *phr);

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

    CBasePin *GetPin(int n);
    int GetPinCount();

    STDMETHODIMP Pause();
    STDMETHODIMP Run(REFERENCE_TIME tStart);

    STDMETHODIMP GetPages(CAUUID *pPages);
    STDMETHODIMP GetIdleTime(unsigned int *idleTime) const;
    STDMETHODIMP UpdateIdleTime(unsigned int idleTime);
};