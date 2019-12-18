
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
uniform               vec4 occLightSources[THE_MAX_LIGHTS * 4]; //!< packed light sources parameters
uniform THE_PREC_ENUM int occLightSourcesTypes[THE_MAX_LIGHTS]; //!< packed light sources types
#endif

#if defined(THE_IS_PBR)
vec3 occDiffIBLMap (in vec3 theNormal)
{
  vec3 aSHCoeffs[9];
  for (int i = 0; i < 9; ++i)
  {
    aSHCoeffs[i] = occTexture2D (occDiffIBLMapSHCoeffs, vec2 ((float(i) + 0.5) / 9.0, 0.0)).rgb;
  }
  return aSHCoeffs[0]

       + aSHCoeffs[1] * theNormal.x
	   + aSHCoeffs[2] * theNormal.y
	   + aSHCoeffs[3] * theNormal.z

	   + aSHCoeffs[4] * theNormal.x * theNormal.z
	   + aSHCoeffs[5] * theNormal.y * theNormal.z
	   + aSHCoeffs[6] * theNormal.x * theNormal.y

	   + aSHCoeffs[7] * (3.0 * theNormal.z * theNormal.z - 1.0)
	   + aSHCoeffs[8] * (theNormal.x * theNormal.x - theNormal.y * theNormal.y);
}
#endif

// front and back material properties accessors
#if defined(THE_IS_PBR)
uniform vec4 occPbrFrontMaterial[3];
uniform vec4 occPbrBackMaterial[3];

#define MIN_ROUGHNESS 0.01
float occRoughness (in float theNormalizedRoughness) { return theNormalizedRoughness * (1.0 - MIN_ROUGHNESS) + MIN_ROUGHNESS; }
vec4  occPBRMaterial_Color(in bool theIsFront)     { return theIsFront ? occPbrFrontMaterial[0]     : occPbrBackMaterial[0]; }
vec3  occPBRMaterial_Emission(in bool theIsFront)  { return theIsFront ? occPbrFrontMaterial[1].rgb : occPbrBackMaterial[1].rgb; }
float occPBRMaterial_IOR(in bool theIsFront)       { return theIsFront ? occPbrFrontMaterial[1].w   : occPbrBackMaterial[1].w; }
float occPBRMaterial_Metallic(in bool theIsFront)  { return theIsFront ? occPbrFrontMaterial[2].b   : occPbrBackMaterial[2].b; }
float occPBRMaterial_NormalizedRoughness(in bool theIsFront) { return theIsFront ? occPbrFrontMaterial[2].g : occPbrBackMaterial[2].g; }
#else
uniform vec4 occFrontMaterial[5];
uniform vec4 occBackMaterial[5];

vec4  occFrontMaterial_Ambient(void)      { return occFrontMaterial[0]; }
vec4  occFrontMaterial_Diffuse(void)      { return occFrontMaterial[1]; }
vec4  occFrontMaterial_Specular(void)     { return occFrontMaterial[2]; }
vec4  occFrontMaterial_Emission(void)     { return occFrontMaterial[3]; }
float occFrontMaterial_Shininess(void)    { return occFrontMaterial[4].x; }
float occFrontMaterial_Transparency(void) { return occFrontMaterial[4].y; }

vec4  occBackMaterial_Ambient(void)       { return occBackMaterial[0]; }
vec4  occBackMaterial_Diffuse(void)       { return occBackMaterial[1]; }
vec4  occBackMaterial_Specular(void)      { return occBackMaterial[2]; }
vec4  occBackMaterial_Emission(void)      { return occBackMaterial[3]; }
float occBackMaterial_Shininess(void)     { return occBackMaterial[4].x; }
float occBackMaterial_Transparency(void)  { return occBackMaterial[4].y; }
#endif

// 2D texture coordinates transformation
vec2  occTextureTrsf_Translation(void) { return occTexTrsf2d[0].xy; }
vec2  occTextureTrsf_Scale(void)       { return occTexTrsf2d[0].zw; }
float occTextureTrsf_RotationSin(void) { return occTexTrsf2d[1].x; }
float occTextureTrsf_RotationCos(void) { return occTexTrsf2d[1].y; }
//! @endfile DeclarationsImpl.glsl
