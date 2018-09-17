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

#include "MirageNode.h"
#include "Model.h"
#include "Material.h"

namespace LibGens {
	MirageNode::MirageNode() {
		name = "";
		value = 0;
		data = NULL;
		data_address = 0;
		nodes.clear();
	}

	MirageNode::MirageNode(string name_p, unsigned int value_p) {
		name = name_p;
		value = value_p;
		data = NULL;
		data_address = 0;
		nodes.clear();
	}

	MirageNode::~MirageNode() {
		for (vector<MirageNode *>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			delete (*it);
		}

		nodes.clear();
	}

	bool MirageNode::read(File *file) {
		size_t header_address = file->getCurrentAddress();

		unsigned int size = 0;
		file->readInt32BE(&size);
		file->readInt32BE(&value);
		file->readString(&name, 8);

		int space_index = name.find_last_not_of(" ");
		name = name.substr(0, space_index + 1);

		unsigned int flags = size >> 29;
		size &= 0x1FFFFFFF;

		if (~flags & 1) {
			while (true) {
				MirageNode *mirage_node = new MirageNode();
				bool last_node = mirage_node->read(file);
				nodes.push_back(mirage_node);
				
				if (last_node) {
					break;
				}
			}
		}

		data_address = file->getCurrentAddress();
		file->goToAddress(header_address + size);
		return flags & 2;
	}

	void MirageNode::write(File *file, bool last_node) {
		size_t header_address = file->getCurrentAddress();

		file->writeNull(4);
		file->writeInt32BE(&value);

		unsigned int name_size = name.size();
		if (name_size > 8) {
			name_size = 8;
		}

		file->write((char *)name.c_str(), name_size);
		if (name_size < 8) {
			unsigned char filler = 32;
			unsigned int filler_count = 8 - name_size;
			for (unsigned int i = 0; i < filler_count; i++) {
				file->writeUChar(&filler);
			}
		}

		unsigned int flags = last_node ? 2 : 0;
		if (nodes.size()) {
			for (unsigned int i = 0; i < nodes.size(); i++) {
				nodes[i]->write(file, i == (nodes.size() - 1));
			}
		}

		else {
			flags |= 1;
		}

		if (data) {
			switch (value) {
				case LIBGENS_MODEL_ROOT_DYNAMIC_GENERATIONS:
					{
						Model *model = (Model *)data;
						model->writeRootNodeDynamicGenerations(file);
						break;
					}

				case LIBGENS_MATERIAL_ROOT_GENERATIONS:
					{
						Material *material = (Material *)data;
						material->writeRootNodeGenerations(file);
						break;
					}
			}
		}

		size_t end_address = file->getCurrentAddress();
		file->goToAddress(header_address);
		unsigned int size = (end_address - header_address) | (flags << 29);
		file->writeInt32BE(&size);
		file->goToAddress(end_address);
	}

	string MirageNode::getName() {
		return name;
	}

	unsigned int MirageNode::getValue() {
		return value;
	}

	size_t MirageNode::getDataAddress() {
		return data_address;
	}

	void MirageNode::setName(string v) {
		name = v;
	}

	void MirageNode::setValue(unsigned int v) {
		value = v;
	}

	void MirageNode::setData(void *data_p, int data_type) {
		data = data_p;
		value = data_type;
	}

	vector<MirageNode *> MirageNode::getNodes() {
		return nodes;
	}

	void MirageNode::addNode(MirageNode *node) {
		nodes.push_back(node);
	}

	MirageNode *MirageNode::newNode(string name, unsigned int value) {
		MirageNode *node = new MirageNode(name, value);
		nodes.push_back(node);
		return node;
	}

	MirageNode *MirageNode::find(string name, bool search_children) {
		for (vector<MirageNode *>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if ((*it)->getName() == name) {
				return (*it);
			}

			MirageNode *node;
			if (search_children && (node = (*it)->find(name))) {
				return node;
			}
		}

		return NULL;
	}
}