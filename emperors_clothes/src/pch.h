#pragma once

#include <codeanalysis\warnings.h>
#pragma warning(push)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS 26812)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <commctrl.h>
#include <cguid.h>
#include <initguid.h>

// baseclasses
#include <streams.h>

#pragma warning(pop)

#include "resource.h"
