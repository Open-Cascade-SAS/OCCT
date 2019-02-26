
//! @file DeclarationsImpl.glsl includes implementation of common functions and properties accessors
#if defined(FRAGMENT_SHADER)
//! Output color (and coverage for accumulation by OIT algorithm).
void occSetFragColor (in vec4 theColor)
{
#if defined(OCC_ALPHA_TEST)
  if (theColor.a < occAlphaCutoff) discard;
#endif
#if defined(OCC_WRITE_WEIGHT_OIT_COVERAGE)
  float aWeight     = theColor.a * clamp (1e+2 * pow (1.0 - gl_FragCoord.z * occOitDepthFactor, 3.0), 1e-2, 1e+2);
  occFragCoverage.r = theColor.a * aWeight;
  occFragColor      = vec4 (theColor.rgb * theColor.a * aWeight, theColor.a);
#else
  occFragColor = theColor;
#endif
}
#endif

#if defined(THE_MAX_LIGHTS) && (THE_MAX_LIGHTS > 0)
// arrays of light sources
uniform THE_PREC_ENUM ivec2 occLightSourcesTypes[THE_MAX_LIGHTS]; //!< packed light sources types
uniform               vec4  occLightSources[THE_MAX_LIGHTS * 4];  //!< packed light sources parameters

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
#endif

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

// 2D texture coordinates transformation
vec2  occTextureTrsf_Translation(void) { return occTexTrsf2d[0].xy; }
vec2  occTextureTrsf_Scale(void)       { return occTexTrsf2d[0].zw; }
float occTextureTrsf_RotationSin(void) { return occTexTrsf2d[1].x; }
float occTextureTrsf_RotationCos(void) { return occTexTrsf2d[1].y; }
//! @endfile DeclarationsImpl.glsl
