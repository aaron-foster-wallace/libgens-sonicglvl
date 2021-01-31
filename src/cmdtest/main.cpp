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
#include "Material.h"
#include "Texture.h"
#include "Model.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Submesh.h"
#include "S06XNFile.h"
#include "Parameter.h"
#include "VertexFormat.h"

using namespace LibGens;

int main(int argc, char** argv) {
    if (ToString(argv[1]).find(".model") != string::npos || ToString(argv[1]).find(".terrain-model") != string::npos) {
        LibGens::Model model(argv[1]);

        bool isTerrain = ToString(argv[1]).find(".terrain-model") != string::npos;

        uint32_t offset = 0;

        VertexFormat format;
        format.addElement(VertexFormatElement(offset, FLOAT3, POSITION, 0));
        offset += 12;
		format.addElement(VertexFormatElement(offset, DEC3N, NORMAL, 0));
        offset += 4;
		format.addElement(VertexFormatElement(offset, DEC3N, TANGENT, 0));
        offset += 4;
		format.addElement(VertexFormatElement(offset, DEC3N, BINORMAL, 0));
        offset += 4;
		format.addElement(VertexFormatElement(offset, FLOAT2, UV, 0));
        offset += 8;
        if (isTerrain)
        {
		    format.addElement(VertexFormatElement(offset, FLOAT2, UV, 1));
            offset += 8;
        }
		format.addElement(VertexFormatElement(offset, UBYTE4N, COLOR, 0));
        offset += 4;

        if (!isTerrain)
        {
            format.addElement(VertexFormatElement(offset, UBYTE4, BONE_INDICES, 0));
            offset += 4;
            format.addElement(VertexFormatElement(offset, UBYTE4N, BONE_WEIGHTS, 0));
            offset += 4;
        }
        format.setSize(offset);
        printf("Vertex Format Size: %d\n", offset);

        vector<LibGens::Mesh*> meshes = model.getMeshes();
        for (size_t i = 0; i < meshes.size(); i++)
        {
            vector<LibGens::Submesh*> submeshes = meshes[i]->getSubmeshes();
            for (size_t j = 0; j < submeshes.size(); j++)
            {
                submeshes[j]->setVertexFormat(new LibGens::VertexFormat(format));
            }
        }
        model.save(argv[1]);
        return 0;
    }

    LibGens::Material material(argv[1]);

    vector<LibGens::Texture*> units = material.getTextureUnits();

    //LibGens::Texture* texture = material.getTextureByUnit("IBL");
    //if (!texture) material.addTextureUnit(new LibGens::Texture(material.getName() + "-000" + ToString(units.size()), "IBL", "defaultibl"));
    //else texture->setName("defaultibl");

    bool hasDiffuse = false;
    bool hasSpecular = false;
    bool hasNormal = false;
    bool hasTransparency = false;
    bool hasBlend = false;
    bool hasDiffuseBlend = false;
    bool hasSpecularBlend = false;
    bool hasNormalBlend = false;
    bool hasEmission = false;
    bool hasCurvature = false;
    bool hasFalloff = false;
    bool hasReflection = false;

    string shader = material.getShader();
    if (shader.find("Sky_") != string::npos)
        return 1;

    for (auto it = units.begin(); it != units.end(); it++) {
        string name = (*it)->getName();
        string unit = (*it)->getUnit();

        if (name.find("_fal") != string::npos) {
            (*it)->setUnit("falloff");
            hasFalloff = true;
        }
        else if (name.find("_cdr") != string::npos) {
            (*it)->setUnit("curvature");
            hasCurvature = true;
        }
        //else if (name.find("_nrm") != string::npos || unit == "normal") {
        //    (*it)->setUnit("normal");
        //    if (!hasNormal)
        //        hasNormal = true;
        //    else
        //        hasNormalBlend = true;
        //}
        //else if (name.find("_prm") != string::npos || unit == "specular" || unit == "gloss") {
        //    (*it)->setUnit("specular");
        //    if (!hasSpecular)
        //        hasSpecular = true;
        //    else if (unit != "gloss")
        //        hasSpecularBlend = true;
        //}
        //else if (name.find("_alp") != string::npos) {
        //    (*it)->setUnit("opacity");
        //    hasBlend = true;
        //}
        //else if (name.find("_abd_a") != string::npos || unit == "transparency" || unit == "opacity") {
        //    (*it)->setUnit("transparency");
        //    hasTransparency = true;
        //}
        //else if (name.find("_abd") != string::npos || unit == "diffuse") {
        //    (*it)->setUnit("diffuse");
        //    if (!hasDiffuse)
        //        hasDiffuse = true;
        //    else
        //        hasDiffuseBlend = true;
        //}
        //else if (name.find("_ems") != string::npos || unit == "displacement" || unit == "emission") {
        //    (*it)->setUnit("emission");
        //    hasEmission = true;
        //}
        //else if (name.find("_spt") != string::npos || unit == "reflection") {
        //    (*it)->setUnit("reflection");
        //    hasReflection = true;
        //}
    }

    //if (hasReflection && hasCurvature) {
    //    material.setShader("ChrEyeCDRF");
    //
    //    if (!material.getParameterByName("PBRFactor"))
    //        material.addParameter(new LibGens::Parameter("PBRFactor", LibGens::Color(0.1, 0.8, 0, 0)));
    //    if (!material.getParameterByName("ChrEye1"))
    //        material.addParameter(new LibGens::Parameter("ChrEye1", LibGens::Color(0.03, -0.05, 0.01, 0.01)));
    //    if (!material.getParameterByName("ChrEye2"))
    //        material.addParameter(new LibGens::Parameter("ChrEye2", LibGens::Color(0.02, 0.09, 0.12, 0.07)));
    //    if (!material.getParameterByName("ChrEye3"))
    //        material.addParameter(new LibGens::Parameter("ChrEye3", LibGens::Color(0, 0, 0.1, 0.1)));
    //}
    //else {
    //    string suffix = "";
    //
    //    if (hasFalloff && hasCurvature)
    //    {
    //        if (hasDiffuse)
    //            suffix += "d";
    //        if (hasSpecular)
    //            suffix += "p";
    //        if (hasNormal)
    //            suffix += "n";
    //
    //        suffix += "cf";
    //
    //         material.setShader("ChrSkinCDRF_" + suffix);
    //
    //        if (!material.getParameterByName("PBRFactor") && !hasSpecular)
    //            material.addParameter(new LibGens::Parameter("PBRFactor", LibGens::Color(0.1, 0.8, 0, 0)));
    //        if (!material.getParameterByName("FalloffFactor"))
    //            material.addParameter(new LibGens::Parameter("FalloffFactor", LibGens::Color(1, 5, 0.15, 1)));
    //    }
    //    else if (hasEmission)
    //    {
    //        if (hasDiffuse)
    //            suffix += "d";
    //        if (hasSpecular)
    //            suffix += "p";
    //        if (hasNormal)
    //            suffix += "n";
    //
    //        suffix += "E";
    //
    //        if (hasTransparency)
    //            suffix += "a";
    //
    //        if (shader.find("MEmission") != std::string::npos)
    //            material.setShader("MEmission_" + suffix);
    //        else
    //            material.setShader("Emission_" + suffix);
    //
    //        if (!material.getParameterByName("PBRFactor") && !hasSpecular)
    //            material.addParameter(new LibGens::Parameter("PBRFactor", LibGens::Color(0.1, 0.8, 0, 0)));
    //
    //        if (!material.getParameterByName("Luminance"))
    //            material.addParameter(new LibGens::Parameter("Luminance", LibGens::Color(1, 0, 0, 0)));
    //    }
    //    else if (hasDiffuseBlend || hasNormalBlend || hasSpecularBlend || hasBlend)
    //    {
    //        if (hasDiffuse)
    //            suffix += "d";
    //        if (hasSpecular)
    //            suffix += "p";
    //        if (hasNormal)
    //            suffix += "n";
    //        if (hasBlend)
    //            suffix += "b";
    //        if (hasDiffuseBlend)
    //            suffix += "d";
    //        if (hasSpecularBlend)
    //            suffix += "p";
    //        if (hasNormalBlend)
    //            suffix += "n";
    //
    //
    //        if (shader.find("MBlend") != std::string::npos)
    //            material.setShader("MBlend_" + suffix);
    //        else
    //            material.setShader("Blend2_" + suffix);
    //
    //        if (!material.getParameterByName("PBRFactor") && !(hasSpecular || hasSpecularBlend))
    //            material.addParameter(new LibGens::Parameter("PBRFactor", LibGens::Color(0.1, 0.8, 0, 0)));
    //    }
    //    else
    //    {
    //        if (hasDiffuse)
    //            suffix += "d";
    //        if (hasSpecular)
    //            suffix += "p";
    //        if (hasNormal)
    //            suffix += "n";
    //        if (hasTransparency)
    //            suffix += "a";
    //
    //        if (shader.find("MCommon") != std::string::npos)
    //            material.setShader("MCommon_" + suffix);
    //        else
    //            material.setShader("Common2_" + suffix);
    //
    //        if (!material.getParameterByName("PBRFactor") && !hasSpecular)
    //            material.addParameter(new LibGens::Parameter("PBRFactor", LibGens::Color(0.1, 0.8, 0, 0)));
    //    }
    //}

    size_t pos = shader.find_last_of('_');
    if (pos != string::npos)
    {
        string actual_shader = shader.substr(0, pos);
        string textures = shader.substr(pos);
        if (actual_shader == "Common" || actual_shader == "Blend")
            material.setShader(actual_shader + "2" + textures);
    }

    material.save(argv[1]);//, LIBGENS_MATERIAL_ROOT_UNLEASHED);

    return 0;
}
