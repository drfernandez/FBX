// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#if DEBUG
	#include <vld.h>
#endif

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Windowsx.h>

// reference additional headers your program requires here
#include <DirectXMath.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <fstream>
#include <string>
#include <codecvt>
#include <vector>
#include <unordered_map>
#include "Structures.h"
