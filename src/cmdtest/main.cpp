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
#include "Model.h"
#include "Mesh.h"
#include "Morph.h"
#include <unordered_map>
#include "Submesh.h"
#include "Bone.h"

int main(int argc, char** argv) {
    LibGens::Model m1(argv[1]);
    m1.fixVertexFormatForPC();
    m1.save(argv[1], LIBGENS_MODEL_ROOT_DYNAMIC_UNLEASHED_2);

    //LibGens::Model m1(argv[1]);
    //LibGens::Model m2(argv[2]);

    //vector<LibGens::Bone*> bones = m1.getBones();
    //vector<LibGens::MorphSet*> sets = m1.getMorpherSets();

    //for (auto it = sets.begin(); it != sets.end(); ++it)
    //{
    //    LibGens::Mesh* mesh = (*it)->getMesh();
    //    vector<LibGens::Submesh*> submeshes = mesh->getSubmeshes();
    //    for (auto it2 = submeshes.begin(); it2 != submeshes.end(); ++it2)
    //    {
    //        vector<unsigned char> boneTable = (*it2)->getBoneTable();
    //        (*it2)->clearBoneTable();
    //        for (auto it3 = boneTable.begin(); it3 != boneTable.end(); ++it3)
    //        {
    //            (*it2)->addBone(m2.getBoneIndexByName(bones[(*it3)]->getName()));
    //        }
    //    }
    //    m2.addMorpherSet(*it);
    //}

    //m2.fixVertexFormatForPC();
    //m2.save(argv[3]);

    return 0;
}
