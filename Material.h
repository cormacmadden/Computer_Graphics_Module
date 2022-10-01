#pragma once
#include "Texture.h"
#include "maths_funcs.h"

class Material {

public:
    vec3 AmbientColor = vec3(0.0f, 0.0f, 0.0f);
    vec3 DiffuseColor = vec3(0.0f, 0.0f, 0.0f);
    vec3 SpecularColor = vec3(0.0f, 0.0f, 0.0f);

    // TODO: need to deallocate  these
    Texture* pDiffuse = NULL; // base color of the material
    Texture* pSpecularExponent = NULL;
};
