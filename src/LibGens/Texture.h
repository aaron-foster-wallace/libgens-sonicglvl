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

#pragma once

#define LIBGENS_TEXSET_EXTENSION          ".texset"
#define LIBGENS_TEXTURE_EXTENSION         ".texture"
#define LIBGENS_TEXSET_ROOT_UNLEASHED     0
#define LIBGENS_TEXTURE_ROOT_UNLEASHED    1

namespace LibGens {
	class Material;

	class Texture {
		protected:
			string internal_name;
			string unit;
			string name;
		    unsigned int flags;
		public:
			Texture();
			Texture(string filename, string internal_name_p);
			Texture(string internal_name_p, string unit_p, string name_p);
			void read(File *file, string id);
			void write(File *file);
			void save(string filename);
			void setName(string v);
			void setUnit(string v);
			void setInternalName(string v);
			string getName();
			string getUnit();
			string getTexset();
	};
};