//=========================================================================
//	  Copyright (c) 2016 SonicGLvl
//
//    This file is part of SonicGLvl, a community-created free level editor 
//    for the PC version of Sonic Generations.
//
//    SonicGLvl is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    SonicGLvl is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    
//
//    Read AUTHORS.txt, LICENSE.txt and COPYRIGHT.txt for more details.
//=========================================================================

using namespace std;
// Configuration Properties -> Linker -> Input -> Additional Dependencies.
#pragma comment (lib, "legacy_stdio_definitions.lib")
#pragma comment (lib, "zlib-md.lib")
#pragma comment (lib, "libxml2-md.lib")

#include <stack>
#include <list>
#include <set>
#include <iterator>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <ctype.h>
#include <pthread.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#if defined(_WIN32) || defined(WIN32)
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#endif

// Common Headers only should be pre-compiled
#include "half/half.h"
#include "sha1/sha1.h"
#include "tinyxml/tinyxml.h"
#include "MathGens.h"
#include "Bitmap.h"
#include "Error.h"
#include "Endian.h"
#include "File.h"

#undef max
#undef min
#include "tristripper/tri_stripper.h"