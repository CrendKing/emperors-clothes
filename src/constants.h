#pragma once

#include <initguid.h>


// {7797A4EA-AD10-4A5E-B511-20F84BBDD67D}
DEFINE_GUID(CLSID_EmperorsClothes,
    0x7797a4ea, 0xad10, 0x4a5e, 0xb5, 0x11, 0x20, 0xf8, 0x4b, 0xbd, 0xd6, 0x7d);

// {A01B4996-BA1A-438F-AB8B-63653571FE1D}
DEFINE_GUID(CLSID_PropertyPage,
    0xa01b4996, 0xba1a, 0x438f, 0xab, 0x8b, 0x63, 0x65, 0x35, 0x71, 0xfe, 0x1d);

// {53834EFB-9087-4C67-A211-C899BFD4161A}
DEFINE_GUID(IID_IIdleTime,
    0x53834efb, 0x9087, 0x4c67, 0xa2, 0x11, 0xc8, 0x99, 0xbf, 0xd4, 0x16, 0x1a);

#define WidenHelper(str)  L##str
#define Widen(str)        WidenHelper(str)

#ifdef DEBUG
#define FILTER_NAME_SUFFIX " [Debug]"
#else
#define FILTER_NAME_SUFFIX
#endif // DEBUG
#define PROPERTY_PAGE_SUFFIX " Property Page"

#define FILTER_NAME_BASE "Emperor's Clothes"
#define FILTER_NAME_FULL FILTER_NAME_BASE FILTER_NAME_SUFFIX
#define PROPERTY_PAGE_FULL FILTER_NAME_BASE PROPERTY_PAGE_SUFFIX FILTER_NAME_SUFFIX

constexpr char *FILTER_NAME = FILTER_NAME_FULL;
constexpr wchar_t *FILTER_NAME_WIDE = Widen(FILTER_NAME_FULL);

constexpr char *PROPERTY_PAGE_NAME = PROPERTY_PAGE_FULL;
constexpr wchar_t *PROPERTY_PAGE_NAME_WIDE = Widen(PROPERTY_PAGE_FULL);

constexpr char *CLOTHES_WINDOW_CLASS_NAME = "Clothes";

constexpr char *REGISTRY_KEY_NAME = "Software\\EmperorsClothes";
constexpr char *REGISTRY_IDLE_TIME_VALUE_NAME = "IdleTime";

constexpr unsigned int IDLE_TIME_MIN = 500;
constexpr unsigned int IDLE_TIME_MAX = 60000;
constexpr unsigned int IDLE_TIME_DEFAULT = 3000;

constexpr unsigned int TIMER_ID = 0;
constexpr unsigned int TIMER_INTERVAL = 100;