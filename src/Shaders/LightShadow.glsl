#if (__VERSION__ >= 120)
//! Coefficients for gathering close samples for antialiasing.
//! Use only with decent OpenGL (array constants cannot be initialized with GLSL 1.1 / GLSL ES 1.1)
const vec2 occPoissonDisk16[16] = vec2[](
 vec2(-0.94201624,-0.39906216), vec2( 0.94558609,-0.76890725), vec2(-0.09418410,-0.92938870), vec2( 0.34495938, 0.29387760),
 vec2(-0.91588581, 0.45771432), vec2(-0.81544232,-0.87912464), vec2(-0.38277543, 0.27676845), vec2( 0.97484398, 0.75648379),
 vec2( 0.44323325,-0.97511554), vec2( 0.53742981,-0.47373420), vec2(-0.26496911,-0.41893023), vec2( 0.79197514, 0.19090188),
 vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590), vec2( 0.19984126, 0.78641367), vec2( 0.14383161,-0.14100790)
);
#endif

//! Function computes directional and spot light shadow attenuation (1.0 means no shadow).
float occLightShadow (in sampler2D theShadow,
                      in int  theId,
                      in vec3 theNormal)
{
  vec4 aPosLightSpace = PosLightSpace[occLight_Index(theId)];
  vec3 aLightDir = occLight_Position (theId);
  vec3 aProjCoords = (aPosLightSpace.xyz / aPosLightSpace.w);
#ifdef THE_ZERO_TO_ONE_DEPTH
  aProjCoords.xy = aProjCoords.xy * 0.5 + vec2 (0.5);
#else
  aProjCoords = aProjCoords * 0.5 + vec3 (0.5);
#endif
  float aCurrentDepth = aProjCoords.z;
  if (aProjCoords.x < 0.0 || aProjCoords.x > 1.0
   || aProjCoords.y < 0.0 || aProjCoords.y > 1.0
   || aCurrentDepth > 1.0)
  {
    return 1.0;
  }

  vec2 aTexelSize = vec2 (occShadowMapSizeBias.x);
  float aBias = max (occShadowMapSizeBias.y * (1.0 - dot (theNormal, aLightDir)), occShadowMapSizeBias.y * 0.1);
#if (__VERSION__ >= 120)
  float aShadow = 0.0;
  for (int aPosIter = 0; aPosIter < 16; ++aPosIter)
  {
    float aClosestDepth = occTexture2D (theShadow, aProjCoords.xy + occPoissonDisk16[aPosIter] * aTexelSize).r;
    aShadow += (aCurrentDepth - aBias) > aClosestDepth ? 1.0 : 0.0;
  }
  return 1.0 - aShadow / 16.0;
#else
  float aClosestDepth = occTexture2D (theShadow, aProjCoords.xy).r;
  float aShadow = (aCurrentDepth - aBias) > aClosestDepth ? 1.0 : 0.0;
  return 1.0 - aShadow;
#endif
}
