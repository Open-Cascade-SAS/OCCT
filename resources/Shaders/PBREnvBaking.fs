THE_SHADER_IN vec3 ViewDirection; //!< direction of fetching from environment cubemap

#if (__VERSION__ >= 120)
uniform int uSamplesNum;     //!< number of samples in Monte-Carlo integration
#else
const int uSamplesNum = 256;
#endif
uniform samplerCube uEnvMap; //!< source of baking (environment cubemap)

#ifdef THE_TO_BAKE_DIFFUSE
uniform int uYCoeff; //!< coefficient of Y controlling horizontal flip of cubemap
uniform int uZCoeff; //!< coefficient of Z controlling vertical flip of cubemap
#endif

#ifdef THE_TO_BAKE_SPECULAR
uniform int   uCurrentLevel;        //!< current level of specular IBL map (ignored in case of diffuse map's processing)
uniform float uEnvSolidAngleSource; //!< source solid angle sample computed from one edge's size of source environment map's zero mipmap level
#endif

//! Returns coordinates of point theNumber from Hammersley point set having size theSize.
vec2 hammersley (in int theNumber,
                 in int theSize)
{
  int aDenominator = 2;
  int aNumber = theNumber;
  float aVanDerCorput = 0.0;
  for (int i = 0; i < 32; ++i)
  {
    if (aNumber > 0)
    {
      aVanDerCorput += mod(float(aNumber), 2.0) / float(aDenominator);
      aNumber /= 2;
      aDenominator *= 2;
    }
  }
  return vec2(float(theNumber) / float(theSize), aVanDerCorput);
}

//! This function does importance sampling on hemisphere surface using GGX normal distribution function
//! in tangent space (positive z axis is surface normal direction).
vec3 importanceSample (in vec2  theHammersleyPoint,
                       in float theRoughness)
{
  float aPhi = PI_2 * theHammersleyPoint.x;
  theRoughness *= theRoughness;
  theRoughness *= theRoughness;
  float aCosTheta = sqrt((1.0 - theHammersleyPoint.y) / (1.0 + (theRoughness - 1.0) * theHammersleyPoint.y));
  float aSinTheta = sqrt(1.0 - aCosTheta * aCosTheta);
  return vec3(aSinTheta * cos(aPhi),
              aSinTheta * sin(aPhi),
              aCosTheta);
}

//! This function uniformly generates samples on whole sphere.
vec3 sphereUniformSample (in vec2 theHammersleyPoint)
{
  float aPhi = PI_2 * theHammersleyPoint.x;
  float aCosTheta = 2.0 * theHammersleyPoint.y - 1.0;
  float aSinTheta = sqrt(1.0 - aCosTheta * aCosTheta);
  return vec3(aSinTheta * cos(aPhi),
              aSinTheta * sin(aPhi),
              aCosTheta);
}

//! Transforms resulted sampled direction from tangent space to world space considering the surface normal.
vec3 fromTangentSpace (in vec3 theVector,
                       in vec3 theNormal)
{
  vec3 anUp = (abs(theNormal.z) < 0.999) ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 anX = normalize(cross(anUp, theNormal));
  vec3 anY = cross(theNormal, anX);
  return anX * theVector.x + anY * theVector.y + theNormal * theVector.z;
}

#ifdef THE_TO_BAKE_DIFFUSE
#if (__VERSION__ >= 120)
const float aSHBasisFuncCoeffs[9] = float[9]
(
  0.282095 * 0.282095,
  0.488603 * 0.488603,
  0.488603 * 0.488603,
  0.488603 * 0.488603,
  1.092548 * 1.092548,
  1.092548 * 1.092548,
  1.092548 * 1.092548,
  0.315392 * 0.315392,
  0.546274 * 0.546274
);
const float aSHCosCoeffs[9] = float[9]
(
  3.141593,
  2.094395,
  2.094395,
  2.094395,
  0.785398,
  0.785398,
  0.785398,
  0.785398,
  0.785398
);
#else
uniform float aSHBasisFuncCoeffs[9];
uniform float aSHCosCoeffs[9];
#endif

//! Bakes diffuse IBL map's spherical harmonics coefficients.
vec3 bakeDiffuseSH()
{
  int anId = int(gl_FragCoord.x);
  float aCoef;
#if (__VERSION__ >= 120)
  aCoef = aSHCosCoeffs[anId] * aSHBasisFuncCoeffs[anId];
#else
  if      (anId == 0) { aCoef = aSHCosCoeffs[0] * aSHBasisFuncCoeffs[0]; }
  else if (anId == 1) { aCoef = aSHCosCoeffs[1] * aSHBasisFuncCoeffs[1]; }
  else if (anId == 2) { aCoef = aSHCosCoeffs[2] * aSHBasisFuncCoeffs[2]; }
  else if (anId == 3) { aCoef = aSHCosCoeffs[3] * aSHBasisFuncCoeffs[3]; }
  else if (anId == 4) { aCoef = aSHCosCoeffs[4] * aSHBasisFuncCoeffs[4]; }
  else if (anId == 5) { aCoef = aSHCosCoeffs[5] * aSHBasisFuncCoeffs[5]; }
  else if (anId == 6) { aCoef = aSHCosCoeffs[6] * aSHBasisFuncCoeffs[6]; }
  else if (anId == 7) { aCoef = aSHCosCoeffs[7] * aSHBasisFuncCoeffs[7]; }
  else                { aCoef = aSHCosCoeffs[8] * aSHBasisFuncCoeffs[8]; }
#endif
  vec3 aRes = vec3 (0.0);
  for (int aSampleIter = 0; aSampleIter < uSamplesNum; ++aSampleIter)
  {
    vec2 aHammersleyPoint = hammersley (aSampleIter, uSamplesNum);
    vec3 aDir = sphereUniformSample (aHammersleyPoint);

    vec3 aVal = occTextureCube (uEnvMap, cubemapVectorTransform (aDir, uYCoeff, uZCoeff)).rgb;
  #if (__VERSION__ >= 120)
    float aFunc[9];
    aFunc[0] = 1.0;

    aFunc[1] = aDir.x;
    aFunc[2] = aDir.y;
    aFunc[3] = aDir.z;

    aFunc[4] = aDir.x * aDir.z;
    aFunc[5] = aDir.y * aDir.z;
    aFunc[6] = aDir.x * aDir.y;

    aFunc[7] = 3.0 * aDir.z * aDir.z - 1.0;
    aFunc[8] = aDir.x * aDir.x - aDir.y * aDir.y;

    aRes += aVal * aFunc[anId];
  #else
    if      (anId == 0) { aRes += aVal * 1.0; }
    else if (anId == 1) { aRes += aVal * aDir.x; }
    else if (anId == 2) { aRes += aVal * aDir.y; }
    else if (anId == 3) { aRes += aVal * aDir.z; }
    else if (anId == 4) { aRes += aVal * (aDir.x * aDir.z); }
    else if (anId == 5) { aRes += aVal * (aDir.y * aDir.z); }
    else if (anId == 6) { aRes += aVal * (aDir.x * aDir.y); }
    else if (anId == 7) { aRes += aVal * (3.0 * aDir.z * aDir.z - 1.0); }
    else                { aRes += aVal * (aDir.x * aDir.x - aDir.y * aDir.y); }
  #endif
  }

  return 4.0 * aRes * aCoef / float(uSamplesNum);
}
#endif

#ifdef THE_TO_BAKE_SPECULAR
//! Computes a single sample for specular IBL map.
vec4 specularMapSample (in vec3  theNormal,
                        in float theRoughness,
                        in int   theNumber,
                        in int   theSize)
{
  vec2 aHammersleyPoint = hammersley (theNumber, theSize);
  vec3 aHalf = importanceSample (aHammersleyPoint, occRoughness (theRoughness));
  float aHdotV = aHalf.z;
  aHalf = fromTangentSpace (aHalf, theNormal);
  vec3  aLight = -reflect (theNormal, aHalf);
  float aNdotL = dot (aLight, theNormal);
  if (aNdotL > 0.0)
  {
    float aSolidAngleSample = 1.0 / (float(theSize) * (occPBRDistribution (aHdotV, theRoughness) * 0.25 + 0.0001) + 0.0001);
    float aLod = (theRoughness == 0.0) ? 0.0 : 0.5 * log2 (aSolidAngleSample / uEnvSolidAngleSource);
    return vec4 (occTextureCubeLod (uEnvMap, aLight, aLod).rgb * aNdotL, aNdotL);
  }
  return vec4 (0.0);
}

//! Bakes specular IBL map.
vec3 bakeSpecularMap (in vec3  theNormal,
                      in float theRoughness)
{
  vec4 aResult = vec4(0.0);
  if (theRoughness == 0.0)
  {
    aResult = specularMapSample (theNormal, theRoughness, 0, 1);
  }
  else
  {
    for (int aSampleIter = 0; aSampleIter < uSamplesNum; ++aSampleIter)
    {
      aResult += specularMapSample (theNormal, theRoughness, aSampleIter, uSamplesNum);
    }
  }
  return aResult.xyz / aResult.w;
}
#endif

void main()
{
  vec3 aViewDirection = normalize (ViewDirection);
#ifdef THE_TO_BAKE_DIFFUSE
  vec4 aRes = vec4 (bakeDiffuseSH(), 1.0);
#ifdef THE_TO_PACK_FLOAT
  int aCompIndex = int(gl_FragCoord.y);
  float aComp = aCompIndex == 0 ? aRes.x : (aCompIndex == 1 ? aRes.y : aRes.z);
  int aFixedPrec = int(aComp * 2147483647.0);
  int aFixedDiv1 = aFixedPrec / 256;
  int aFixedDiv2 = aFixedDiv1 / 256;
  int aFixedDiv3 = aFixedDiv2 / 256;
  vec4 aPacked = vec4(float(aFixedPrec), float(aFixedDiv1), float(aFixedDiv2), float(aFixedDiv3));
  aRes = fract (aPacked * (1.0 / 256.0));
#endif
  occFragColor = aRes;
#else
  float aRoughness = float(uCurrentLevel) / float(occNbSpecIBLLevels - 1);
  occFragColor = vec4 (bakeSpecularMap (aViewDirection, aRoughness), 1.0);
#endif
}
