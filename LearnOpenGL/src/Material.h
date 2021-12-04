#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include "Shader.h"



struct Material 
{
	std::string MatName;
	glm::vec3 DiffuseColor = glm::vec3(0.2f);
	glm::vec3 AmbientColor = glm::vec3(1.0f);
	glm::vec3 SpecularColor = glm::vec3(1.0f);
    glm::vec3 EmissiveColor = glm::vec3(0.0f);

    float shininess = 0.0f;
    bool IsEmissive = false;
    bool IsTransparent = false;
    bool IsLit = false;


    static Material Copper() 
    {
        Material mat;
        mat.AmbientColor = glm::vec3(0.19125f, 0.0735f, 0.0225f);
        mat.DiffuseColor = glm::vec3(0.7038f, 0.27048f, 0.0828f);
        mat.SpecularColor = glm::vec3(0.256777f,	0.137622f, 0.086014f);
        mat.shininess = 0.1f;
        return mat;
    }

    static Material Gold()
    {
        Material mat;
        mat.AmbientColor = glm::vec3(0.24725, 0.1995, 0.0745);
        mat.DiffuseColor = glm::vec3(0.75164, 0.60648, 0.22648);
        mat.SpecularColor = glm::vec3(0.628281, 0.555802, 0.366065);
        mat.shininess = 0.4f;
        return mat;
    }
    static Material Emerald()
    {
        Material mat;
        mat.AmbientColor = glm::vec3(0.0215, 0.1745, 0.0215);
        mat.DiffuseColor = glm::vec3(0.07568, 0.61424, 0.07568);
        mat.SpecularColor = glm::vec3(0.633, 0.727811, 0.633);
        mat.shininess = 0.6f;
        return mat;
    }

    static Material Default()
    {
        Material mat;
        mat.AmbientColor = glm::vec3(0.0f);
        mat.DiffuseColor = glm::vec3(1.0f);
        mat.SpecularColor = glm::vec3(1.0f);
        mat.EmissiveColor = glm::vec3(0.0f);
        mat.shininess = 1.0f;
        return mat;
    }

    static void ApplyMaterial(Material& material, std::shared_ptr<Shader>& shader) 
    {
        shader->SetVec3("material.ambient", material.AmbientColor);
        shader->SetVec3("material.diffuse", material.DiffuseColor);
        shader->SetVec3("material.specular", material.SpecularColor);
        shader->SetVec3("material.emissive", material.EmissiveColor);
        shader->SetFloat("material.shininess", material.shininess);
    }
};