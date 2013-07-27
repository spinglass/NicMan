// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#define NOMINMAX                // Allow std::min/max to be used
#include <windows.h>

#include <cassert>

#include "SFML\Audio.hpp"
#include "SFML\Graphics.hpp"

#include "tinyxml2.h"
