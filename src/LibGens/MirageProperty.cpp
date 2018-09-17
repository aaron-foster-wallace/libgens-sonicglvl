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

#include "MirageProperty.h"
#include "MirageNode.h"

namespace LibGens {
	MirageProperty::MirageProperty(string name_p, unsigned int value_p) {
		name = name_p;
		value = value_p;
	}

	MirageProperty::MirageProperty(MirageNode *mirage_node_p) {
		name = mirage_node_p->getName();
		value = mirage_node_p->getValue();
	}

	string MirageProperty::getName() {
		return name;
	}

	unsigned int MirageProperty::getValue() {
		return value;
	}

	void MirageProperty::setName(string v) {
		name = v;
	}

	void MirageProperty::setValue(unsigned int v) {
		value = v;
	}

	MirageNode *MirageProperty::toMirageNode() {
		return new MirageNode( name, value );
	}
}
