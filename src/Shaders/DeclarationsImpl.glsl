// This file includes implementation of common functions and properties accessors

// arrays of light sources
uniform ivec2 occLightSourcesTypes[THE_MAX_LIGHTS]; //!< packed light sources types
uniform vec4  occLightSources[THE_MAX_LIGHTS * 4];  //!< packed light sources parameters

// light source properties accessors
int   occLight_Type              (in int theId) { return occLightSourcesTypes[theId].x; }
int   occLight_IsHeadlight       (in int theId) { return occLightSourcesTypes[theId].y; }
vec4  occLight_Diffuse           (in int theId) { return occLightSources[theId * 4 + 0]; }
vec4  occLight_Specular          (in int theId) { return occLightSources[theId * 4 + 0]; }
vec4  occLight_Position          (in int theId) { return occLightSources[theId * 4 + 1]; }
vec4  occLight_SpotDirection     (in int theId) { return occLightSources[theId * 4 + 2]; }
float occLight_ConstAttenuation  (in int theId) { return occLightSources[theId * 4 + 3].x; }
float occLight_LinearAttenuation (in int theId) { return occLightSources[theId * 4 + 3].y; }
float occLight_SpotCutOff        (in int theId) { return occLightSources[theId * 4 + 3].z; }
float occLight_SpotExponent      (in int theId) { return occLightSources[theId * 4 + 3].w; }

// material state
uniform vec4 occFrontMaterial[5];
uniform vec4 occBackMaterial[5];

// front material properties accessors
vec4  occFrontMaterial_Ambient(void)      { return occFrontMaterial[0]; }
vec4  occFrontMaterial_Diffuse(void)      { return occFrontMaterial[1]; }
vec4  occFrontMaterial_Specular(void)     { return occFrontMaterial[2]; }
vec4  occFrontMaterial_Emission(void)     { return occFrontMaterial[3]; }
float occFrontMaterial_Shininess(void)    { return occFrontMaterial[4].x; }
float occFrontMaterial_Transparency(void) { return occFrontMaterial[4].y; }

// back material properties accessors
vec4  occBackMaterial_Ambient(void)       { return occBackMaterial[0]; }
vec4  occBackMaterial_Diffuse(void)       { return occBackMaterial[1]; }
vec4  occBackMaterial_Specular(void)      { return occBackMaterial[2]; }
vec4  occBackMaterial_Emission(void)      { return occBackMaterial[3]; }
float occBackMaterial_Shininess(void)     { return occBackMaterial[4].x; }
float occBackMaterial_Transparency(void)  { return occBackMaterial[4].y; }
