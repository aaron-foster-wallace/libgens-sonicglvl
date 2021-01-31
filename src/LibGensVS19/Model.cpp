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

#include "Bone.h"
#include "Mesh.h"
#include "Model.h"
#include "Morph.h"
#include "TerrainGroup.h"
#include "MirageNode.h"
#include "MirageProperty.h"

namespace LibGens {
	Model::Model() {
		meshes.clear();
		bones.clear();
		properties.clear();
		name="";
		filename="";
		terrain_mode = false;
		has_instances = true;
	}

	Model::Model(string filename_p) {
		File file(filename_p, LIBGENS_FILE_READ_BINARY);
		filename=filename_p;
		
		name = filename;
		size_t sep = name.find_last_of("\\/");
		if (sep != std::string::npos) {
			name = name.substr(sep + 1, name.size() - sep - 1);
		}
	
		size_t dot = name.find_last_of(".");
		if (dot != string::npos) {
			name = name.substr(0, dot);
		}

		terrain_mode = (filename.find(LIBGENS_TERRAIN_MODEL_EXTENSION) != string::npos);

		if (file.valid()) {
			file.readHeader();
			readRootNode(&file);
			file.close();
		}
	}

	Model::Model(File *file, bool terrain_mode_p) {
		if (!file) return;

		terrain_mode = terrain_mode_p;
		file->readHeader();
		readRootNode(file);
	}

	void Model::readRootNode(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_NULL_FILE);
			return;
		}

		switch (file->getRootNodeType()) {
			case LIBGENS_MODEL_ROOT_DYNAMIC_UNLEASHED_2:
				readRootNodeDynamicUnleashed2(file);
				break;

			case LIBGENS_MODEL_ROOT_DYNAMIC_GENERATIONS:
				readRootNodeDynamicGenerations(file);
				break;

			case LIBGENS_FILE_HEADER_ROOT_TYPE_LOST_WORLD:
				readRootNodeDynamicLostWorld(file);
				break;
		}
	}
	
	void Model::readRootNodeDynamicUnleashed2(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_NULL_FILE);
			return;
		}

		size_t header_address=file->getCurrentAddress();

		// FIXME: Implement Terrain Mode support for this root type

		if (!terrain_mode) {
			// Read Submeshes
			Mesh *mesh = new Mesh();
			mesh->read(file);
			mesh->buildAABB();
			meshes.push_back(mesh);

			// Go to Skeleton
			file->goToAddress(header_address + 24);
			readSkeleton(file);
		}

		if (terrain_mode) {
			buildAABB();
		}
	}

	void Model::readRootNodeDynamicGenerations(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_NULL_FILE);
			return;
		}

		size_t header_address=file->getCurrentAddress();

		// Read Meshes
		unsigned int mesh_count=0;
		size_t mesh_table_address=0;
		size_t mesh_name_address=0;

		
		file->readInt32BE(&mesh_count);
		file->readInt32BEA(&mesh_table_address);

		if (terrain_mode) {
			file->readInt32BEA(&mesh_name_address);
			file->readInt32BE(&has_instances);

			// Read Name
			file->goToAddress(mesh_name_address);
			file->readString(&name);
		}
		else {
			readSkeleton(file);
		}

		meshes.reserve(mesh_count);
		for (size_t i=0; i<mesh_count; i++) {
			size_t mesh_address=0;
			file->goToAddress(mesh_table_address + i*4);
			file->readInt32BEA(&mesh_address);
			file->goToAddress(mesh_address);

			Mesh *mesh = new Mesh();
			mesh->read(file);
			mesh->buildAABB();
			meshes.push_back(mesh);
		}

		if (terrain_mode) {
			buildAABB();
		}
	}

	
	void Model::readRootNodeDynamicLostWorld(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_NULL_FILE);
			return;
		}

		MirageNode *root_node = new MirageNode();
		root_node->read(file);

		MirageNode *property_node = root_node->find("SCAParam");
		if (property_node) {
			vector<MirageNode *> property_nodes = property_node->getNodes();
			for (vector<MirageNode *>::iterator it = property_nodes.begin(); it != property_nodes.end(); it++) {
				properties.push_back(new MirageProperty(*it));
			}
		}

		MirageNode *contexts_node = root_node->find("Contexts", false);
		if (contexts_node) {
			file->goToAddress(contexts_node->getDataAddress());
			readRootNodeDynamicGenerations(file);
		}

		delete root_node;
	}

	
	void Model::readSkeleton(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_NULL_FILE);
			return;
		}

		size_t header_address=file->getCurrentAddress();

		unsigned int morph_set_count=0;
		size_t morph_sets_address=0;
		file->readInt32BE(&morph_set_count);
		file->readInt32BEA(&morph_sets_address);

		unsigned int bone_total=0;
		size_t bone_definition_table_address=0;
		size_t bone_matrix_address=0;
		size_t global_aabb_address=0;

		file->readInt32BE(&bone_total);
		file->readInt32BEA(&bone_definition_table_address);
		file->readInt32BEA(&bone_matrix_address);
		file->readInt32BEA(&global_aabb_address);

		for (size_t i = 0; i < morph_set_count; i++) {
			file->goToAddress(morph_sets_address + i * 4);
			size_t address = 0;
			file->readInt32BEA(&address);
			file->goToAddress(address);

			MorphSet* morph_set = new MorphSet();
			morph_set->read(file);
			morph_sets.push_back(morph_set);
		}

		bones.reserve(bone_total);
		for (size_t i=0; i<bone_total; i++) {
			file->goToAddress(bone_definition_table_address + i*4);
			size_t address=0;
			file->readInt32BEA(&address);
			file->goToAddress(address);

			Bone *bone=new Bone();
			bone->readDescription(file);
			file->goToAddress(bone_matrix_address + i*64);
			bone->readMatrix(file);
			bones.push_back(bone);
		}

		file->goToAddress(global_aabb_address);
		global_aabb.read(file);
	}


	void Model::save(string filename_p, int root_type) {
		File file(filename_p, LIBGENS_FILE_WRITE_BINARY);

		if (file.valid()) {
			file.prepareHeader(root_type);
			write(&file);
			file.writeHeader(true);
			file.close();
		}
	}

	void Model::write(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_WRITE_NULL_FILE);
			return;
		}

		switch (file->getRootNodeType()) {
			case LIBGENS_MODEL_ROOT_DYNAMIC_GENERATIONS:
				writeRootNodeDynamicGenerations(file);
				break;
			case LIBGENS_MODEL_ROOT_DYNAMIC_UNLEASHED_2:
				writeRootNodeDynamicUnleashed2(file);
				break;
			case LIBGENS_FILE_HEADER_ROOT_TYPE_LOST_WORLD:
				writeRootNodeDynamicLostWorld(file);
				break;
		}
	}

	void Model::writeRootNodeDynamicGenerations(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_WRITE_NULL_FILE);
			return;
		}

		size_t table_address=file->getCurrentAddress();
		unsigned int mesh_count=meshes.size();
		size_t model_table_address=0;
		size_t model_name_address=0;

		size_t morph_sets_address=0;
		unsigned int morph_set_count = morph_sets.size();
		unsigned int bone_count=bones.size();
		size_t bone_definition_table_address=0;
		size_t bone_matrix_address=0;
		size_t global_aabb_address=0;

		file->writeInt32BE(&mesh_count);
		file->writeNull(4);

		if (terrain_mode) {
			file->writeNull(4);
			file->writeInt32BE(&has_instances);
		}
		else {
			file->writeInt32BE(&morph_set_count);
			file->writeNull(4);
			file->writeInt32BE(&bone_count);
			file->writeNull(12);
		}

		// Write meshes
		model_table_address=file->getCurrentAddress();
		vector<unsigned int> mesh_addresses;
		file->writeNull(mesh_count*4);

		for (size_t i=0; i<mesh_count; i++) {
			mesh_addresses.push_back(file->getCurrentAddress());
			meshes[i]->write(file);
		}

		file->goToAddress(model_table_address);
		for (size_t i=0; i<mesh_count; i++) {
			file->writeInt32BEA(&mesh_addresses[i]);
		}

		file->goToEnd();
		file->fixPadding();

		if (terrain_mode) {
			model_name_address=file->getCurrentAddress();
			file->writeString(&name);
			file->fixPadding();
		}
		else {
			morph_sets_address = file->getCurrentAddress();

			vector<size_t> morph_set_addresses;
			file->writeNull(morph_set_count * 4);

			for (size_t i = 0; i < morph_set_count; i++) {
				morph_set_addresses.push_back(file->getCurrentAddress());
				morph_sets[i]->write(file);
			}

			for (size_t i = 0; i < morph_set_count; i++) {
				file->goToAddress(morph_sets_address + i * 4);
				file->writeInt32BEA(&morph_set_addresses[i]);
			}
			file->goToEnd();

			bone_definition_table_address = file->getCurrentAddress();
			vector<unsigned int> bone_definition_addresses;
			file->writeNull(bone_count*4);

			for (size_t i=0; i<bone_count; i++) {
				bone_definition_addresses.push_back(file->getCurrentAddress());
				bones[i]->writeDescription(file);
			}

			for (size_t i=0; i<bone_count; i++) {
				file->goToAddress(bone_definition_table_address + i*4);
				file->writeInt32BEA(&bone_definition_addresses[i]);
			}
			file->goToEnd();

			bone_matrix_address = file->getCurrentAddress();
			for (size_t i=0; i<bone_count; i++) {
				bones[i]->writeMatrix(file);
			}

			global_aabb_address = file->getCurrentAddress();
			global_aabb.write(file);
		}

		file->goToAddress(table_address+4);
		file->writeInt32BEA(&model_table_address);

		if (terrain_mode) {
			file->writeInt32BEA(&model_name_address);
		}
		else {
			file->moveAddress(4);
			file->writeInt32BEA(&morph_sets_address);
			file->moveAddress(4);
			file->writeInt32BEA(&bone_definition_table_address);
			file->writeInt32BEA(&bone_matrix_address);
			file->writeInt32BEA(&global_aabb_address);
		}

		file->goToEnd();
	}

	
	void Model::writeRootNodeDynamicUnleashed2(File *file) {
		if (!file) {
			Error::addMessage(Error::NULL_REFERENCE, LIBGENS_MODEL_ERROR_MESSAGE_WRITE_NULL_FILE);
			return;
		}

		size_t table_address=file->getCurrentAddress();
		unsigned int mesh_count=meshes.size();
		size_t model_table_address=0;
		size_t model_name_address=0;

		size_t unknown_address=0;
		unsigned int bone_count=bones.size();
		size_t bone_definition_table_address=0;
		size_t bone_matrix_address=0;
		size_t global_aabb_address=0;

		meshes[0]->write(file, true);

		file->goToEnd();
		file->fixPadding();

		unknown_address = file->getCurrentAddress();

		bone_definition_table_address = file->getCurrentAddress();
		vector<unsigned int> bone_definition_addresses;
		file->writeNull(bone_count*4);

		for (size_t i=0; i<bone_count; i++) {
			bone_definition_addresses.push_back(file->getCurrentAddress());
			bones[i]->writeDescription(file);
		}

		for (size_t i=0; i<bone_count; i++) {
			file->goToAddress(bone_definition_table_address + i*4);
			file->writeInt32BEA(&bone_definition_addresses[i]);
		}
		file->goToEnd();

		bone_matrix_address = file->getCurrentAddress();
		for (size_t i=0; i<bone_count; i++) {
			bones[i]->writeMatrix(file);
		}

		global_aabb_address = file->getCurrentAddress();
		global_aabb.write(file);

		file->goToAddress(table_address + 28);
		file->writeInt32BEA(&unknown_address);
		file->writeInt32BE(&bone_count);
		file->writeInt32BEA(&bone_definition_table_address);
		file->writeInt32BEA(&bone_matrix_address);
		file->writeInt32BEA(&global_aabb_address);

		file->goToEnd();
	}

	void Model::writeRootNodeDynamicLostWorld(File *file) {
		MirageNode *root_node = new MirageNode("Model", 1);
		
		if (properties.size()) {
			MirageNode *property_node = root_node->newNode("NodesExt", 1)->newNode("NodePrms", 0)->newNode("SCAParam", 1);
			for (vector<MirageProperty *>::iterator it = properties.begin(); it != properties.end(); it++) {
				property_node->addNode((*it)->toMirageNode());
			}
		}

		MirageNode *contexts_node = new MirageNode();
		contexts_node->setName("Contexts");
		contexts_node->setData(this, LIBGENS_MODEL_ROOT_DYNAMIC_GENERATIONS);
		root_node->addNode(contexts_node);

		root_node->write(file, true);
		delete root_node;
	}

	list<Vertex *> Model::getVertexList() {
		list<Vertex *> vertices;
		list<Vertex *> mesh_vertices;

		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			mesh_vertices=(*it)->getVertexList();

			for (list<Vertex *>::iterator it_v=mesh_vertices.begin(); it_v!=mesh_vertices.end(); it_v++) {
				vertices.push_back(*it_v);
			}
		}

		return vertices;
	}

	list<unsigned int> Model::getFaceList() {
		list<unsigned int> faces;
		list<unsigned int> mesh_faces;
		unsigned int face_offset=0;

		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			mesh_faces=(*it)->getFaceList();

			for (list<unsigned int>::iterator it_f=mesh_faces.begin(); it_f!=mesh_faces.end(); it_f++) {
				faces.push_back((*it_f) + face_offset);
			}

			face_offset += (*it)->getVertexList().size();
		}

		return faces;
	}

	void Model::createSamplePoints(list<VRMapSample *> *list, Matrix4 &matrix, Bitmap *bitmap, float unit_size, float saturation_multiplier, float brightness_offset) {
		for (std::vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			(*it)->createSamplePoints(list, matrix, bitmap, unit_size, saturation_multiplier, brightness_offset);
		}
	}

	void Model::addMesh(Mesh *mesh) {
		if (mesh) {
			meshes.push_back(mesh);
		}
	}

	void Model::mergeModel(Model *model, LibGens::Matrix4 transform, float uv2_left, float uv2_right, float uv2_top, float uv2_bottom) {
		vector<Mesh *> merge_meshes = model->getMeshes();
		for (std::vector<Mesh *>::iterator it=merge_meshes.begin(); it!=merge_meshes.end(); it++) {
			cloneMesh(*it, transform, uv2_left, uv2_right, uv2_top, uv2_bottom);
		}

		buildAABB();
	}

	void Model::cloneMesh(Mesh *mesh, LibGens::Matrix4 transform, float uv2_left, float uv2_right, float uv2_top, float uv2_bottom) {
		Mesh *clone_mesh = new Mesh(mesh, transform, uv2_left, uv2_right, uv2_top, uv2_bottom);
		meshes.push_back(clone_mesh);
	}

	Model::~Model() {
		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			delete (*it);
		}
		meshes.clear();

		for (vector<Bone *>::iterator it=bones.begin(); it!=bones.end(); it++) {
			delete (*it);
		}
		bones.clear();
	}

	list<string> Model::getMaterialNames() {
		list<string> material_names;
		list<string> mesh_material_names;

		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			mesh_material_names=(*it)->getMaterialNames();

			for (list<string>::iterator it_s=mesh_material_names.begin(); it_s!=mesh_material_names.end(); it_s++) {
				bool found=false;

				for (list<string>::iterator it_m=material_names.begin(); it_m!=material_names.end(); it_m++) {
					if ((*it_m) == (*it_s)) {
						found = true;
						break;
					}
				}

				if (!found) material_names.push_back(*it_s);
			}
		}

		return material_names;
	}

	vector<unsigned int> Model::getMaterialMappings(list<string> &material_names) {
		vector<unsigned int> material_mappings;
		vector<unsigned int> mesh_material_mappings;

		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			mesh_material_mappings=(*it)->getMaterialMappings(material_names);

			for (size_t i=0; i<mesh_material_mappings.size(); i++) {
				material_mappings.push_back(mesh_material_mappings[i]);
			}
		}

		return material_mappings;
	}


	void Model::getTotalData(list<Vertex *> &vertex_list, list<unsigned int> &face_list, list<string> &material_names, vector<unsigned int> &material_mappings) {
		vertex_list = getVertexList();
		face_list = getFaceList();
		material_names = getMaterialNames();
		material_mappings = getMaterialMappings(material_names);
	}


	void Model::fixVertexFormatForPC() {
		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			(*it)->fixVertexFormatForPC();
		}

		for (auto it = morph_sets.begin(); it != morph_sets.end(); ++it)
			(*it)->fixVertexFormatForPC();
	}

	void Model::buildAABB() {
		global_aabb.reset();

		for (std::vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			global_aabb.merge((*it)->getAABB());
		}
	}

	AABB Model::getAABB() {
		return global_aabb;
	}

	void Model::setTerrainMode(bool v) {
		terrain_mode = v;
		has_instances = v;
	}

	vector<Bone *> Model::getBones() {
		return bones;
	}

	int Model::getBoneIndexByName(string bone_name) {
		for (size_t i=0; i<bones.size(); i++) {
			if (bones[i]->getName() == bone_name) {
				return i;
			}
		}

		return -1;
	}

	vector<Mesh *> Model::getMeshes() {
		return meshes;
	}

	void Model::setName(string v) {
		name=v;
	}

	string Model::getName() {
		return name;
	}

	string Model::getFilename() {
		return filename;
	}

	unsigned int Model::getEstimatedMemorySize() {
		unsigned int memory_size = 0;
		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			memory_size += (*it)->getEstimatedMemorySize();
		}
		return memory_size;
	}

	void Model::changeVertexFormat(int format) {
		for (vector<Mesh *>::iterator it=meshes.begin(); it!=meshes.end(); it++) {
			(*it)->changeVertexFormat(format);
		}
	}

	void Model::setPropertyValue(string name, unsigned int value) {
		if (name.size() > 8) name = name.substr(0, 8);
		
		for (vector<MirageProperty *>::iterator it = properties.begin(); it != properties.end(); it++) {
			if ((*it)->getName() == name) {
				(*it)->setValue(value);
				return;
			}
		}

		properties.push_back(new MirageProperty(name, value));
	}

	vector<MorphSet*> Model::getMorphSets() {
		return morph_sets;
	}

	void Model::addMorphSet(MorphSet* morpher_set) {
		morph_sets.push_back(morpher_set);
	}
};

