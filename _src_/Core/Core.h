#ifndef CORE_PCH_H
#define CORE_PCH_H

#define MU_SEASON	16
#define MU_EP		1.1

#pragma warning(disable: 4018)
#pragma warning(disable: 4267)
#pragma warning(disable: 4244)
#pragma warning(disable: 4554)
#pragma warning(disable: 4996)

#include <assert.h>
#include <iostream>
#include <fstream>
#include <filesystem> // C++17
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#define NO_CORE_DEBUG_LOG

#ifndef NO_CORE_DEBUG_LOG
#define PRINT_DEBUG(msg) std::cout << msg << std::endl
#else
#define PRINT_DEBUG(msg) 
#endif

#define TRUE	1
#define FALSE	0
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned char byte;

namespace fs = std::filesystem;

#include "Utls.h"
#include "LazyStruct.h"
#include "SingletonInstance.h"
#include "MuCommon.h"
#include "ClientStructures.h"
#include "ZzzMathLib.h"

#include "MuCrypto.h"
#include "BaseIO.h"
#include "TxtConvertor.h"
#include "GraphicConvertor.h"
#include "ConvertorTemplate.h"

#endif // !CORE_PCH_H