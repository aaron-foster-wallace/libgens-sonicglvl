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

#include "Havok.h"
#include "HavokEnviroment.h"
#include "HavokPhysicsCache.h"
#include "HavokSkeletonCache.h"
#include "HavokAnimationCache.h"

namespace LibGens {
	void HavokEnviroment::addFolder(string folder) {
		bool found = false;
		for (list<string>::iterator it=search_paths.begin(); it!=search_paths.end(); it++) {
			if ((*it) == folder) {
				found = true;
				break;
			}
		}

		if (!found) {
			search_paths.push_back(folder);
		}
	}

	HavokEnviroment::HavokEnviroment(int bufferSize) {
		//hkMemoryRouter * memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(bufferSize));
		//hkBaseSystem::init(memoryRouter, HavokErrorReport);
	}

	
	/*HavokPhysicsCache *HavokEnviroment::getPhysics(string physics_name) {
		//todo: hat if contains the extension , fix for absolute paths

		for (list<HavokPhysicsCache *>::iterator it=physics_cache.begin(); it!=physics_cache.end(); it++) {
			if ((*it)->getName() == physics_name) {
				return (*it);
			}
		}

		for (list<string>::iterator it=search_paths.begin(); it!=search_paths.end(); it++) {
			string filename=(*it) + physics_name + LIBGENS_HAVOK_PHYSICS_EXTENSION;

			if (File::check(filename)) {
				cout << "detected:" << filename << "\n";
				IhkPackFile* data = IhkPackFile::Create(filename.c_str());

				if (!data)
				{
					Error::addMessage(Error::FILE_NOT_FOUND, "Couldn't load " + filename + " Havok file. "
					);
					return NULL;
				}

				hkRootLevelContainer* container = data->GetRootLevelContainer();

				if (!container)
				{
					Error::addMessage(Error::EXCEPTION, "Couldn't find hkRootLevelContainer!");
					continue;
				}
				if (container) {
					for (size_t i = 0; i < container->GetNumVariants(); i++)
					{
						auto  variant= container->GetVariant(i);
						//Todo: check the tipe of a classs
						//if (dynamic_cast<hkpPhysicsData*>(&variant) == nullptr) continue;

							const hkpPhysicsData* constphysics = variant;
							hkpPhysicsData* physics = const_cast<hkpPhysicsData*>(constphysics);
							HavokPhysicsCache* physics_cache_entry = 
								new HavokPhysicsCache(data, filename, 
									physics_name, physics);
							physics_cache.push_back(physics_cache_entry);
							return physics_cache_entry;
						 
					}				
				}
			}
		}

		return NULL;
	}*/



	HavokSkeletonCache *HavokEnviroment::getSkeleton(string skeleton_name) {
		//todo: hat if contains the extension , fix for absolute paths

		for (list<HavokSkeletonCache *>::iterator it=skeleton_cache.begin(); it!=skeleton_cache.end(); it++) {
			if ((*it)->getName() == skeleton_name) {
				return (*it);
			}
		}

		for (list<string>::iterator it=search_paths.begin(); it!=search_paths.end(); it++) {
			string filename=(*it) + skeleton_name + LIBGENS_HAVOK_SKELETON_EXTENSION;
			cout << "testing:"<< filename << "\n";			
			if (File::check(filename)) {
				cout << "detected:" << filename << "\n";
				IhkPackFile* data = IhkPackFile::Create(filename.c_str());

				if (!data)
				{
					Error::addMessage(Error::FILE_NOT_FOUND, "Couldn't load " + filename + " Havok file. "
					);
					return NULL;
				}

				hkRootLevelContainer* container = data->GetRootLevelContainer();

				if (!container)
				{
					Error::addMessage(Error::EXCEPTION, "Couldn't find hkRootLevelContainer!");
					continue;
				}
				if (container) {
					cout << "detected:" << filename << "\n";

					for (size_t j = 0; j < container->GetNumVariants(); j++)
					{

						const hkaAnimationContainer* aniCont = container->GetVariant(j);//TODO: Check class
						if (aniCont != nullptr) {
							for (size_t i = 0; i < aniCont->GetNumSkeletons(); i++)
							{
								hkaSkeleton* skeleton = const_cast<hkaSkeleton*>(aniCont->GetSkeleton(i));

								if (skeleton) {
									HavokSkeletonCache* skeleton_cache_entry = new HavokSkeletonCache(data, filename, skeleton_name, skeleton);
									skeleton_cache.push_back(skeleton_cache_entry);
									return skeleton_cache_entry;
								}
							}
							break;
						}
					}
				}
			}
		}

		return NULL;
	}

	
	HavokAnimationCache *HavokEnviroment::getAnimation(string animation_name) {
		for (list<HavokAnimationCache *>::iterator it=animation_cache.begin(); it!=animation_cache.end(); it++) {
			if ((*it)->getName() == animation_name) {
				return (*it);
			}
		}

		for (list<string>::iterator it=search_paths.begin(); it!=search_paths.end(); it++) {
			string filename=(*it) + animation_name + LIBGENS_HAVOK_ANIMATION_EXTENSION;
			cout << "testing:" << filename << "\n";
			if (File::check(filename)) {
				cout << "detected:" << filename << "\n";
				IhkPackFile* data = IhkPackFile::Create(filename.c_str());

				if (!data)
				{
					Error::addMessage(Error::FILE_NOT_FOUND, "Couldn't load " + filename + " Havok file. "
						);
					continue;
				}

				hkRootLevelContainer* container = data->GetRootLevelContainer();

				if (!container)
				{
					Error::addMessage(Error::EXCEPTION, "Couldn't find hkRootLevelContainer!");
					continue;
				}
 				if (container) {
					
						for (size_t j = 0; j < container->GetNumVariants(); j++)
						{
						
							const hkaAnimationContainer* aniCont = container->GetVariant(j);//TODO: Check class
							if (aniCont != nullptr){
								for (size_t i = 0; i < aniCont->GetNumBindings(); i++)
								{
									hkaAnimationBinding* animation_binding = const_cast<hkaAnimationBinding*>(aniCont->GetBinding(i));
									hkaAnimation* m_animation = const_cast<hkaAnimation*>( animation_binding->GetAnimation());
									HavokAnimationCache* animation_cache_entry =
										new HavokAnimationCache(data, filename, animation_name,
											animation_binding, m_animation);
									animation_cache.push_back(animation_cache_entry);
									return animation_cache_entry;
								}
								break;
							}
						}
				}
			}
		}

		return NULL;
	}
	/*
	bool HavokEnviroment::deletePhysicsEntry(string physics_name) {
		for (list<HavokPhysicsCache *>::iterator it=physics_cache.begin(); it!=physics_cache.end(); it++) {
			if ((*it)->getName() == physics_name) {
				delete (*it);
				physics_cache.erase(it);
				return true;
			}
		}

		return false;
	}*/
};