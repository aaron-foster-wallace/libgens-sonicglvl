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
#include "LibGens.h"
#include "FBX.h"
#include "FBXManager.h"
#include "HavokEnviroment.h"
#include "MaterialLibrary.h"
#include "Model.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//#define BUFSIZE 4096
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}


string getFileName(const string& s) {

	char sep = '\\';


	size_t i = s.rfind(sep, s.length());
	if (i != string::npos) {
		return(s.substr(i + 1, s.length() - i));
	}

	return(s);
}
string getDirName(const string& s) {

	char sep = '\\';


	size_t i = s.rfind(sep, s.length());
	if (i != string::npos) {
		return(s.substr(0,i+1));
	}
	return("");
}
int main(int argc, char** argv) {
    if (argc < 2) {
		printf("Usage: modelfbx [mesh.model] [skeleton.skl.hkx] [animation.anm.hkx] [animation1.anm.hkx]  [animation2.anm.hkx].. [animation_n.anm.hkx] [output.fbx]\n - Parameters can be in any order. You can omit any parameter for excluding elements from the output.\n");
		printf(" Also for anim you can use  wildcards like *.anm.hkx)\n");
		getchar();
        return 1;
    }
	LibGens::initialize();
	LibGens::Error::setLogging(true);
	
	string source_model="";
	string source_skeleton="";
	string auxsource_animation="";
	list<string> source_animations;
	string output_file="";
	size_t pos=0;
//	char *auxbuf = new char[BUFSIZE];
	for (int i=1; i<argc; i++) {
		string parameter = argv[i];
		std::transform(parameter.begin(), parameter.end(), parameter.begin(), ::tolower);

		if ((parameter.find(".model") != string::npos) && (!source_model.size())) {
			source_model = ToString(argv[i]);
		}

		if ((parameter.find(".terrain-model") != string::npos) && (!source_model.size())) {
			source_model = ToString(argv[i]);
		}

		if (((pos=parameter.find(".skl.hkx")) != string::npos) && (!source_skeleton.size())) {
			source_skeleton = ToString(argv[i]);
			source_skeleton.resize(pos);
		}

		
		if (((pos = parameter.find(LIBGENS_HAVOK_ANIMATION_EXTENSION)) != string::npos)) {
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;
			hFind = FindFirstFile(argv[i], &FindFileData);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do {
					source_animations.push_back(FindFileData.cFileName);
 				} while (FindNextFile(hFind, &FindFileData) != 0);

				FindClose(hFind);
			}
		}

		if ((parameter.find(".fbx") != string::npos) && (!output_file.size())) {
			output_file = ToString(argv[i]);
		}
	}

	if (source_model.size() && !output_file.size()) {
		output_file = source_model + ".fbx";
	}

	if (source_skeleton.size() && !output_file.size()) {
		output_file = source_skeleton + ".fbx";
	}

	// Initialize FBX Package
	LibGens::FBXManager *fbx_manager = new LibGens::FBXManager();
	LibGens::FBX *fbx_pack = new LibGens::FBX(fbx_manager->getManager());
	LibGens::HavokEnviroment havok_enviroment(100 * 1024 * 1024);

	havok_enviroment.addFolder("./");
	
	LibGens::Model *model = NULL;

	if (source_model.size()) {
		model = new LibGens::Model(source_model);

		string folder="";
		size_t sep = source_model.find_last_of("\\/");
		if (sep != std::string::npos) {
			folder = source_model.substr(0, sep+1);
		}

		LibGens::MaterialLibrary *material_library = new LibGens::MaterialLibrary(folder);
		fbx_pack->setMaterialLibrary(material_library);
	}

	LibGens::HavokSkeletonCache *havok_skeleton_cache = NULL;
	LibGens::HavokAnimationCache *auxhavok_animation_cache = NULL;
	list <LibGens::HavokAnimationCache*> havok_animation_caches;

	if (source_skeleton.size()) {
 		string dir = getDirName(source_skeleton);
		if (dir.size()) {
			havok_enviroment.addFolder(dir);
		}
		printf("Added %s.skl.hkx from %s \n", getFileName(source_skeleton).c_str(), getDirName(source_skeleton).c_str());
		havok_skeleton_cache = havok_enviroment.getSkeleton(getFileName(source_skeleton));
	}

	//for (std::list<string>::iterator it = source_animations.begin(); it != source_animations.end(); ++it) 
	for (string str : source_animations)
	{
		if (str.size()) {
			string dir = getDirName(str);
			if(dir.size()){
				havok_enviroment.addFolder(dir);
			}
			
			string auxsource_animation = getFileName(str);			

 			int pos = auxsource_animation.find(LIBGENS_HAVOK_ANIMATION_EXTENSION);
			auxsource_animation.resize(pos);
			printf("%s\n", str.c_str());

			auxhavok_animation_cache = havok_enviroment.getAnimation(auxsource_animation);
			havok_animation_caches.push_back(auxhavok_animation_cache);
		}
	}
	 

	FbxMesh *model_node=fbx_pack->addNodeV2(model, havok_skeleton_cache, havok_animation_caches);
	fbx_manager->exportFBX(fbx_pack, output_file);
    return 0;
}

