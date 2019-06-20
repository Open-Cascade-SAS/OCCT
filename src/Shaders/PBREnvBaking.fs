THE_SHADER_IN vec3 ViewDirection; //!< direction of fetching from environment cubemap

uniform int uSamplesNum;     //!< number of samples in Monte-Carlo integration
uniform int uCurrentLevel;   //!< current level of specular IBL map (ignored in case of diffuse map's processing)
uniform int uEnvMapSize;     //!< one edge's size of source environtment map's zero mipmap level
uniform int uYCoeff;         //!< coefficient of Y controlling horizontal flip of cubemap
uniform int uZCoeff;         //!< coefficient of Z controlling vertical flip of cubemap
uniform samplerCube uEnvMap; //!< source of baking (environment cubemap)

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
      aVanDerCorput += float(aNumber % 2) / float(aDenominator);
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

//! Bakes diffuse IBL map's spherical harmonics coefficients.
vec3 bakeDiffuseSH()
{
  int anIndex = int(gl_FragCoord.x);
  vec3 aResult = vec3 (0.0);
  for (int aSampleIter = 0; aSampleIter < uSamplesNum; ++aSampleIter)
  {
    vec2 aHammersleyPoint = hammersley (aSampleIter, uSamplesNum);
    vec3 aDirection = sphereUniformSample (aHammersleyPoint);

    vec3 aValue = occTextureCube (uEnvMap, cubemapVectorTransform (aDirection, uYCoeff, uZCoeff)).rgb;

    float aBasisFunc[9];
    aBasisFunc[0] = 1.0;

    aBasisFunc[1] = aDirection.x;
    aBasisFunc[2] = aDirection.y;
    aBasisFunc[3] = aDirection.z;

    aBasisFunc[4] = aDirection.x * aDirection.z;
    aBasisFunc[5] = aDirection.y * aDirection.z;
    aBasisFunc[6] = aDirection.x * aDirection.y;

    aBasisFunc[7] = 3.0 * aDirection.z * aDirection.z - 1.0;
    aBasisFunc[8] = aDirection.x * aDirection.x - aDirection.y * aDirection.y;

    aResult += aValue * aBasisFunc[anIndex];
  }

  aResult *= 4.0 * aSHCosCoeffs[anIndex] * aSHBasisFuncCoeffs[anIndex] / float(uSamplesNum);
  return aResult;
}

//! Bakes specular IBL map.
vec3 bakeSpecularMap (in vec3  theNormal,
                      in float theRoughness)
{
  vec3 aResult = vec3(0.0);
  float aWeightSum = 0.0;
  int aSamplesNum = (theRoughness == 0.0) ? 1 : uSamplesNum;
  float aSolidAngleSource = 4.0 * PI / (6.0 * float(uEnvMapSize * uEnvMapSize));
  for (int aSampleIter = 0; aSampleIter < aSamplesNum; ++aSampleIter)
  {
    vec2 aHammersleyPoint = hammersley (aSampleIter, aSamplesNum);
    vec3 aHalf = importanceSample (aHammersleyPoint, occRoughness (theRoughness));
    float aHdotV = aHalf.z;
    aHalf = fromTangentSpace (aHalf, theNormal);
    vec3  aLight = -reflect (theNormal, aHalf);
    float aNdotL = dot (aLight, theNormal);
    if (aNdotL > 0.0)
    {
      float aSolidAngleSample = 1.0 / (float(aSamplesNum) * (occPBRDistribution (aHdotV, theRoughness) * 0.25 + 0.0001) + 0.0001);
      float aLod = (theRoughness == 0.0) ? 0.0 : 0.5 * log2 (aSolidAngleSample / aSolidAngleSource);
      aResult += occTextureCubeLod (uEnvMap, aLight, aLod).rgb * aNdotL;
      aWeightSum += aNdotL;
    }
  }
  return aResult / aWeightSum;
}

void main()
{
  vec3 aViewDirection = normalize (ViewDirection);
  if (occNbSpecIBLLevels == 0)
  {
    occSetFragColor (vec4 (bakeDiffuseSH (), 1.0));
  }
  else
  {
    occSetFragColor (vec4 (bakeSpecularMap (aViewDirection, float(uCurrentLevel) / float(occNbSpecIBLLevels - 1)), 1.0));
  }
}
