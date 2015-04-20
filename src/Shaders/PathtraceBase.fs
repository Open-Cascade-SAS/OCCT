#ifdef PATH_TRACING

///////////////////////////////////////////////////////////////////////////////////////
// Specific data types

//! Describes local space at the hit point (visualization space).
struct SLocalSpace
{
  //! Local X axis.
  vec3 AxisX;

  //! Local Y axis.
  vec3 AxisY;

  //! Local Z axis.
  vec3 AxisZ;
};

//! Describes material properties (BSDF).
struct SMaterial
{
  //! Weight of the Lambertian BRDF.
  vec4 Kd;

  //! Weight of the reflection BRDF.
  vec3 Kr;

  //! Weight of the transmission BTDF.
  vec3 Kt;

  //! Weight of the Blinn BRDF (and roughness).
  vec4 Ks;

  //! Fresnel coefficients.
  vec3 Fresnel;

  //! Absorption color and intensity of the media.
  vec4 Absorption;
};

///////////////////////////////////////////////////////////////////////////////////////
// Support subroutines

//=======================================================================
// function : LocalSpace
// purpose  : Generates local space for the given normal
//=======================================================================
SLocalSpace LocalSpace (in vec3 theNormal)
{
  vec3 anAxisX = cross (vec3 (0.f, 1.f, 0.f), theNormal);
  vec3 anAxisY = cross (vec3 (1.f, 0.f, 0.f), theNormal);

  float aSqrLenX = dot (anAxisX, anAxisX);
  float aSqrLenY = dot (anAxisY, anAxisY);

  if (aSqrLenX > aSqrLenY)
  {
    anAxisX *= inversesqrt (aSqrLenX);
    anAxisY = cross (anAxisX, theNormal);
  }
  else
  {
    anAxisY *= inversesqrt (aSqrLenY);
    anAxisX = cross (anAxisY, theNormal);
  }

  return SLocalSpace (anAxisX, anAxisY, theNormal);
}

//=======================================================================
// function : toLocalSpace
// purpose  : Transforms the vector to local space from world space
//=======================================================================
vec3 toLocalSpace (in vec3 theVector, in SLocalSpace theSpace)
{
  return vec3 (dot (theVector, theSpace.AxisX),
               dot (theVector, theSpace.AxisY),
               dot (theVector, theSpace.AxisZ));
}

//=======================================================================
// function : fromLocalSpace
// purpose  : Transforms the vector from local space to world space
//=======================================================================
vec3 fromLocalSpace (in vec3 theVector, in SLocalSpace theSpace)
{
  return theVector.x * theSpace.AxisX +
         theVector.y * theSpace.AxisY +
         theVector.z * theSpace.AxisZ;
}

//=======================================================================
// function : convolve
// purpose  : Performs a linear convolution of the vector components
//=======================================================================
float convolve (in vec3 theVector, in vec3 theFactor)
{
  return dot (theVector, theFactor) * (1.f / max (theFactor.x + theFactor.y + theFactor.z, 1e-15f));
}

//=======================================================================
// function : sphericalDirection
// purpose  : Constructs vector from spherical coordinates
//=======================================================================
vec3 sphericalDirection (in float theCosTheta, in float thePhi)
{
  float aSinTheta = sqrt (1.f - theCosTheta * theCosTheta);

  return vec3 (aSinTheta * cos (thePhi),
               aSinTheta * sin (thePhi),
               theCosTheta);
}

//=======================================================================
// function : fresnelSchlick
// purpose  : Computes the Fresnel reflection formula using
//            Schlick's approximation.
//=======================================================================
vec3 fresnelSchlick (in float theCosI, in vec3 theSpecularColor)
{
  return theSpecularColor + (UNIT - theSpecularColor) * pow (1.f - theCosI, 5.f);
}

//=======================================================================
// function : fresnelDielectric
// purpose  : Computes the Fresnel reflection formula for dielectric in
//            case of circularly polarized light (Based on PBRT code).
//=======================================================================
float fresnelDielectric (in float theCosI,
                         in float theCosT,
                         in float theEtaI,
                         in float theEtaT)
{
  float aParl = (theEtaT * theCosI - theEtaI * theCosT) /
                (theEtaT * theCosI + theEtaI * theCosT);

  float aPerp = (theEtaI * theCosI - theEtaT * theCosT) /
                (theEtaI * theCosI + theEtaT * theCosT);

  return (aParl * aParl + aPerp * aPerp) * 0.5f;
}

#define ENVIRONMENT_IOR 1.f

//=======================================================================
// function : fresnelDielectric
// purpose  : Computes the Fresnel reflection formula for dielectric in
//            case of circularly polarized light (based on PBRT code)
//=======================================================================
float fresnelDielectric (in float theCosI, in float theIndex)
{
  float anEtaI = theCosI > 0.f ? 1.f : theIndex;
  float anEtaT = theCosI > 0.f ? theIndex : 1.f;

  float aSinT = (anEtaI / anEtaT) * sqrt (1.f - theCosI * theCosI);

  if (aSinT >= 1.f)
  {
    return 1.f;
  }

  float aCosT = sqrt (1.f - aSinT * aSinT);

  return fresnelDielectric (abs (theCosI), aCosT, anEtaI, anEtaT);
}

//=======================================================================
// function : fresnelConductor
// purpose  : Computes the Fresnel reflection formula for conductor in case
//            of circularly polarized light (based on PBRT source code)
//=======================================================================
float fresnelConductor (in float theCosI, in float theEta, in float theK)
{
  float aTmp = 2.f * theEta * theCosI;

  float aTmp1 = theEta * theEta + theK * theK;

  float aSPerp = (aTmp1 - aTmp + theCosI * theCosI) /
                 (aTmp1 + aTmp + theCosI * theCosI);

  float aTmp2 = aTmp1 * theCosI * theCosI;

  float aSParl = (aTmp2 - aTmp + 1.f) /
                 (aTmp2 + aTmp + 1.f);

  return (aSPerp + aSParl) * 0.5f;
}

#define FRESNEL_SCHLICK    -0.5f
#define FRESNEL_CONSTANT   -1.5f
#define FRESNEL_CONDUCTOR  -2.5f
#define FRESNEL_DIELECTRIC -3.5f

//=======================================================================
// function : fresnelMedia
// purpose  : Computes the Fresnel reflection formula for general medium
//            in case of circularly polarized light.
//=======================================================================
vec3 fresnelMedia (in float theCosI, in vec3 theFresnelCoeffs)
{
  if (theFresnelCoeffs.x > FRESNEL_SCHLICK)
  {
    return fresnelSchlick (abs (theCosI), theFresnelCoeffs);
  }

  if (theFresnelCoeffs.x > FRESNEL_CONSTANT)
  {
    return vec3 (theFresnelCoeffs.z);
  }

  if (theFresnelCoeffs.x > FRESNEL_CONDUCTOR)
  {
    return vec3 (fresnelConductor (abs (theCosI), theFresnelCoeffs.y, theFresnelCoeffs.z));
  }

  return vec3 (fresnelDielectric (theCosI, theFresnelCoeffs.y));
}

//=======================================================================
// function : transmitted
// purpose  : Computes transmitted direction in tangent space
//            (in case of TIR returned result is undefined!)
//=======================================================================
void transmitted (in float theIndex, in vec3 theIncident, out vec3 theTransmit)
{
  // Compute relative index of refraction
  float anEta = (theIncident.z > 0.f) ? 1.f / theIndex : theIndex;

  // Handle total internal reflection for transmission
  float aSinT2 = anEta * anEta * (1.f - theIncident.z * theIncident.z);

  // Compute transmitted ray direction
  float aCosT = sqrt (1.f - min (aSinT2, 1.f)) * (theIncident.z > 0.f ? -1.f : 1.f);

  theTransmit = normalize (vec3 (-anEta * theIncident.x,
                                 -anEta * theIncident.y,
                                  aCosT));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Handlers and samplers for materials
//////////////////////////////////////////////////////////////////////////////////////////////

//=======================================================================
// function : handleLambertianReflection
// purpose  : Handles Lambertian BRDF, with cos(N, PSI)
//=======================================================================
float handleLambertianReflection (in vec3 theInput, in vec3 theOutput)
{
  return max (0.f, theInput.z) * (1.f / M_PI);
}

//=======================================================================
// function : handleBlinnReflection
// purpose  : Handles Blinn glossy BRDF, with cos(N, PSI)
//=======================================================================
vec3 handleBlinnReflection (in vec3 theInput, in vec3 theOutput, in vec3 theFresnelCoeffs, in float theExponent)
{
  vec3 aWeight = ZERO;

  // Compute half-angle vector
  vec3 aHalf = theInput + theOutput;

  if (aHalf.z < 0.f)
    aHalf = -aHalf;

  float aLength = dot (aHalf, aHalf);

  if (aLength <= 0.f)
    return ZERO;

  aHalf *= inversesqrt (aLength);

  // Compute Fresnel reflectance
  float aCosDelta = dot (theOutput, aHalf);

  vec3 aFresnel = fresnelMedia (aCosDelta, theFresnelCoeffs);

  // Compute fraction of microfacets that reflect light
  float aCosThetaH = max (0.f, aHalf.z);

  float aFraction = (theExponent + 2.f) * (M_PI / 2.f) * pow (aCosThetaH, theExponent);

  // Compute geometry attenuation term (already includes cos)
  float aCosThetaI = max (0.f, theInput.z);
  float aCosThetaO = max (0.f, theOutput.z);

  float aGeom = min (1.f, 2.f * aCosThetaH / max (0.f, aCosDelta) * min (aCosThetaO, aCosThetaI));

  return aCosThetaO < 1.0e-3f ? ZERO :
    aFraction * aGeom / (4.f * aCosThetaO) * aFresnel;
}

//=======================================================================
// function : handleMaterial
// purpose  : Returns BSDF value for specified material, with cos(N, PSI)
//=======================================================================
vec3 handleMaterial (in SMaterial theMaterial, in vec3 theInput, in vec3 theOutput)
{
  return theMaterial.Kd.rgb * handleLambertianReflection (theInput, theOutput) +
    theMaterial.Ks.rgb * handleBlinnReflection (theInput, theOutput, theMaterial.Fresnel, theMaterial.Ks.w);
}

//=======================================================================
// function : sampleSpecularReflection
// purpose  : Samples specular BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
void sampleSpecularReflection (in vec3 theOutput, out vec3 theInput)
{
  theInput = vec3 (-theOutput.x,
                   -theOutput.y,
                    theOutput.z);
}

//=======================================================================
// function : sampleLambertianReflection
// purpose  : Samples Lambertian BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
void sampleLambertianReflection (in vec3 theOutput, out vec3 theInput)
{
  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  float aTemp = sqrt (aKsi2);

  theInput = vec3 (aTemp * cos (2.f * M_PI * aKsi1),
                   aTemp * sin (2.f * M_PI * aKsi1),
                   sqrt (1.f - aKsi2));

  if (theOutput.z < 0.f)
    theInput.z = -theInput.z;
}

//=======================================================================
// function : sampleSpecularTransmission
// purpose  : Samples specular BTDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
vec3 sampleSpecularTransmission (in vec3 theOutput, out vec3 theInput,
  out bool isTransmit, in vec3 theThroughput, in vec3 theFresnelCoeffs)
{
  vec3 aFresnel = fresnelMedia (theOutput.z, theFresnelCoeffs);

  float aProbability = convolve (aFresnel, theThroughput);

  // Sample input direction
  if (RandFloat() <= aProbability)
  {
    theInput = vec3 (-theOutput.x,
                     -theOutput.y,
                      theOutput.z);

    isTransmit = false;

    return aFresnel * (1.f / aProbability);
  }

  transmitted (theFresnelCoeffs.y, theOutput, theInput);

  isTransmit = true;

  return (UNIT - aFresnel) * (1.f / (1.f - aProbability));
}

//=======================================================================
// function : sampleSpecularReflection
// purpose  : Samples specular BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
vec3 sampleSpecularReflection (in vec3 theOutput, out vec3 theInput, in vec3 theFresnelCoeffs)
{
  // Sample input direction
  theInput = vec3 (-theOutput.x,
                   -theOutput.y,
                    theOutput.z);

  return fresnelMedia (theOutput.z, theFresnelCoeffs);
}

#define MIN_COS 1.0e-20f

//=======================================================================
// function : sampleBlinnReflection
// purpose  : Samples Blinn BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//            The BRDF is a product of three main terms, D, G, and F,
//            which is then divided by two cosine terms. Here we perform
//            importance sample the D part of the Blinn model; trying to
//            develop a sampling procedure that accounted for all of the
//            terms would be complex, and it is the D term that accounts
//            for most of the variation.
//=======================================================================
vec3 sampleBlinnReflection (in vec3 theOutput, out vec3 theInput, in vec3 theFresnelCoeffs, in float theExponent)
{
  vec3 aWeight = ZERO;

  // Generate two random variables
  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  // Compute sampled half-angle vector for Blinn distribution
  float aCosThetaH = pow (aKsi1, 1.f / (theExponent + 1.f));

  vec3 aHalf = sphericalDirection (aCosThetaH, aKsi2 * 2.f * M_PI);

  if (aHalf.z < 0)
  {
    aHalf = -aHalf;
  }

  // Compute incident direction by reflecting about half-vector
  float aCosDelta = dot (theOutput, aHalf);

  vec3 anInput = 2.f * aCosDelta * aHalf - theOutput;

  if (theOutput.z * anInput.z <= 0.f)
  {
    return ZERO;
  }

  theInput = anInput;

  // Compute Fresnel reflectance
  vec3 aFresnel = fresnelMedia (aCosDelta, theFresnelCoeffs);

  // Compute geometry attenuation term
  float aCosThetaI = max (MIN_COS, theInput.z);
  float aCosThetaO = max (MIN_COS, theOutput.z);

  float aGeom = min (max (MIN_COS, aCosDelta), 2.f * aCosThetaH * min (aCosThetaO, aCosThetaI));

  // Compute weight of the ray sample
  return aFresnel * ((theExponent + 2.f) / (theExponent + 1.f) * aGeom / aCosThetaO);
}

// Enables expiremental russian roulette sampling
// #define RUSSIAN_ROULETTE

//=======================================================================
// function : sampleMaterial
// purpose  : Samples specified composite material (BSDF)
//=======================================================================
bool sampleMaterial (in SMaterial theMaterial,
                     in vec3      theOutput,
                     in vec3      theFactor,
                     out vec3     theInput,
                     out vec3     theWeight,
                     inout bool   isTransmit)
{
  theWeight = ZERO;

  // Compute the probability of ray reflection
  float aPd = convolve (theMaterial.Kd.rgb, theFactor);
  float aPs = convolve (theMaterial.Ks.rgb, theFactor);
  float aPr = convolve (theMaterial.Kr.rgb, theFactor);
  float aPt = convolve (theMaterial.Kt.rgb, theFactor);

  float aReflection = aPd + aPs + aPr + aPt;

#ifndef RUSSIAN_ROULETTE
  if (aReflection < 1e-2f)
  {
    return false; // path termination
  }
#else
  float aSurvival = max (dot (theFactor, LUMA), 0.1f);

  if (RandFloat() > aSurvival)
  {
    return false; // path termination
  }
#endif

  isTransmit = false;

  // Choose BSDF component to sample
  float aKsi = aReflection * RandFloat();

  if (aKsi < aPd) // diffuse reflection
  {
    sampleLambertianReflection (theOutput, theInput);

#ifndef RUSSIAN_ROULETTE
    theWeight = theMaterial.Kd.rgb * (aReflection / aPd);
#else
    theWeight = theMaterial.Kd.rgb * (aReflection / aPd / aSurvival);
#endif

    return false; // non-specular bounce
  }
  else if (aKsi < aPd + aPs) //  glossy reflection
  {
    theWeight = sampleBlinnReflection (theOutput, theInput, theMaterial.Fresnel, theMaterial.Ks.w);

#ifndef RUSSIAN_ROULETTE
    theWeight *= theMaterial.Ks.rgb * (aReflection / aPs);
#else
    theWeight *= theMaterial.Ks.rgb * (aReflection / aPs / aSurvival);
#endif

    return false; // non-specular bounce
  }
  else if (aKsi < aPd + aPs + aPr) //  specular reflection
  {
    theWeight = sampleSpecularReflection (theOutput, theInput, theMaterial.Fresnel);

#ifndef RUSSIAN_ROULETTE
    theWeight *= theMaterial.Kr.rgb * (aReflection / aPr);
#else
    theWeight *= theMaterial.Kr.rgb * (aReflection / aPr / aSurvival);
#endif

    return true; // specular bounce
  }
  else //  specular transmission
  {
    theWeight = sampleSpecularTransmission (theOutput, theInput,
      isTransmit, theFactor, theMaterial.Fresnel);

#ifndef RUSSIAN_ROULETTE
    theWeight *= theMaterial.Kt.rgb * (aReflection / aPt);
#else
    theWeight *= theMaterial.Kt.rgb * (aReflection / aPt / aSurvival);
#endif

    return true; // specular bounce
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Handlers and samplers for light sources
//////////////////////////////////////////////////////////////////////////////////////////////

//=======================================================================
// function : handlePointLight
// purpose  :
//=======================================================================
float handlePointLight (in vec3 theInput, in vec3 theToLight, in float theRadius)
{
  float aCosMax = sqrt (1.f - theRadius * theRadius / dot (theToLight, theToLight));

  return step (aCosMax, dot (theInput, theToLight));
}

//=======================================================================
// function : handleDirectLight
// purpose  :
//=======================================================================
float handleDirectLight (in vec3 theInput, in vec3 theToLight, in float theCosMax)
{
  return step (theCosMax, dot (theInput, theToLight));
}

//=======================================================================
// function : samplePointLight
// purpose  :
//=======================================================================
vec3 samplePointLight (in vec3 theToLight, in float theRadius, inout float thePDF)
{
  SLocalSpace aSpace = LocalSpace (theToLight);

  float aCosMax = sqrt (1.f - theRadius * theRadius / dot (theToLight, theToLight));

  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  float aTmp = 1.f - aKsi2 * (1.f - aCosMax);

  vec3 anInput = vec3 (sqrt (1.f - aTmp * aTmp) * cos (2.f * M_PI * aKsi1),
                       sqrt (1.f - aTmp * aTmp) * sin (2.f * M_PI * aKsi1),
                       aTmp);

  thePDF *= (theRadius > 0.f) ? 1.f / (2.f * M_PI) / (1.f - aCosMax) : 1.f;

  return normalize (fromLocalSpace (anInput, aSpace));
}

//=======================================================================
// function : sampleDirectLight
// purpose  :
//=======================================================================
vec3 sampleDirectLight (in vec3 theToLight, in float theCosMax, inout float thePDF)
{
  SLocalSpace aSpace = LocalSpace (theToLight);

  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  float aTmp = 1.f - aKsi2 * (1.f - theCosMax);

  vec3 anInput = vec3 (sqrt (1.f - aTmp * aTmp) * cos (2.f * M_PI * aKsi1),
                       sqrt (1.f - aTmp * aTmp) * sin (2.f * M_PI * aKsi1),
                       aTmp);

  thePDF *= (theCosMax < 1.f) ? 1.f / (2.f * M_PI) / (1.f - theCosMax) : 1.f;

  return normalize (fromLocalSpace (anInput, aSpace));
}

// =======================================================================
// function : Latlong
// purpose  : Converts world direction to environment texture coordinates
// =======================================================================
vec2 Latlong (in vec3 thePoint)
{
  float aPsi = acos (-thePoint.z);

  float aPhi = atan (thePoint.y, thePoint.x) + M_PI;

  return vec2 (aPhi * 0.1591549f,
               aPsi * 0.3183098f);
}

// =======================================================================
// function : EnvironmentRadiance
// purpose  :
// =======================================================================
vec3 EnvironmentRadiance (in SRay theRay, in bool isSpecular, in bool isBackground)
{
  vec3 aRadiance = ZERO;

  if (uSphereMapForBack != 0 || !isBackground)
  {
    aRadiance += FetchEnvironment (Latlong (theRay.Direct)).xyz;
  }
  else
  {
    aRadiance += BackgroundColor().xyz;
  }

  // Apply gamma correction (gamma is 2)
  aRadiance *= aRadiance;

  for (int aLightIdx = 0; aLightIdx < uLightCount && isSpecular; ++aLightIdx)
  {
    vec4 aLight = texelFetch (
      uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));
    vec4 aParam = texelFetch (
      uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx));

    if (aLight.w != 0.f) // point light source
    {
      aRadiance += aParam.rgb * handlePointLight (theRay.Direct, aLight.xyz - theRay.Origin, aParam.w /* radius */);
    }
    else // directional light source
    {
      aRadiance += aParam.rgb * handleDirectLight (theRay.Direct, aLight.xyz, aParam.w /* angle cosine */);
    }
  }

  return aRadiance;
}

#define MIN_THROUGHPUT   vec3 (0.02f)
#define MIN_CONTRIBUTION vec3 (0.01f)

#define MATERIAL_KD(index)      (18 * index + 11)
#define MATERIAL_KR(index)      (18 * index + 12)
#define MATERIAL_KT(index)      (18 * index + 13)
#define MATERIAL_KS(index)      (18 * index + 14)
#define MATERIAL_LE(index)      (18 * index + 15)
#define MATERIAL_FRESNEL(index) (18 * index + 16)
#define MATERIAL_ABSORPT(index) (18 * index + 17)

//=======================================================================
// function : PathTrace
// purpose  : Calculates radiance along the given ray
//=======================================================================
vec4 PathTrace (in SRay theRay, in vec3 theInverse)
{
  float anOpenGlDepth = ComputeOpenGlDepth (theRay);

  vec3 aRadiance   = ZERO;
  vec3 aThroughput = UNIT;

  bool isInMedium = false;
  bool isSpecular = false;
  bool isTransmit = false;

  int anObjectId; // ID of intersected triangle

  for (int aDepth = 0; aDepth < NB_BOUNCES; ++aDepth)
  {
    SIntersect aHit = SIntersect (MAXFLOAT, vec2 (ZERO), ZERO);

    ivec4 aTriIndex = SceneNearestHit (theRay, theInverse, aHit, anObjectId);

    if (aTriIndex.x == -1)
    {
      return vec4 (aRadiance + aThroughput *
        EnvironmentRadiance (theRay, isSpecular, aDepth == 0 || isTransmit), 0.f);
    }

    vec3 aInvTransf0 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 0).xyz;
    vec3 aInvTransf1 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 1).xyz;
    vec3 aInvTransf2 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 2).xyz;

    aHit.Normal = normalize (vec3 (dot (aInvTransf0, aHit.Normal),
                                   dot (aInvTransf1, aHit.Normal),
                                   dot (aInvTransf2, aHit.Normal)));

    // For polygons that are parallel to the screen plane, the depth slope
    // is equal to 1, resulting in small polygon offset. For polygons that
    // that are at a large angle to the screen, the depth slope tends to 1,
    // resulting in a larger polygon offset
    float aPolygonOffset = uSceneEpsilon * EPS_SCALE /
      max (abs (dot (theRay.Direct, aHit.Normal)), MIN_SLOPE);

    if (anOpenGlDepth < aHit.Time + aPolygonOffset)
    {
      vec4 aSrcColorRGBA = ComputeOpenGlColor();

      aRadiance   += aThroughput.xyz * aSrcColorRGBA.xyz;
      aThroughput *= aSrcColorRGBA.w;
    }

    theRay.Origin += theRay.Direct * aHit.Time; // intersection point

    // Fetch material (BSDF)
    SMaterial aMaterial = SMaterial (
      vec4 (texelFetch (uRaytraceMaterialTexture, MATERIAL_KD      (aTriIndex.w))),
      vec3 (texelFetch (uRaytraceMaterialTexture, MATERIAL_KR      (aTriIndex.w))),
      vec3 (texelFetch (uRaytraceMaterialTexture, MATERIAL_KT      (aTriIndex.w))),
      vec4 (texelFetch (uRaytraceMaterialTexture, MATERIAL_KS      (aTriIndex.w))),
      vec3 (texelFetch (uRaytraceMaterialTexture, MATERIAL_FRESNEL (aTriIndex.w))),
      vec4 (texelFetch (uRaytraceMaterialTexture, MATERIAL_ABSORPT (aTriIndex.w))));

#ifdef USE_TEXTURES
    if (aMaterial.Kd.w >= 0.f)
    {
      vec4 aTexCoord = vec4 (SmoothUV (aHit.UV, aTriIndex), 0.f, 1.f);

      vec4 aTrsfRow1 = texelFetch (
        uRaytraceMaterialTexture, MATERIAL_TRS1 (aTriIndex.w));
      vec4 aTrsfRow2 = texelFetch (
        uRaytraceMaterialTexture, MATERIAL_TRS2 (aTriIndex.w));

      aTexCoord.st = vec2 (dot (aTrsfRow1, aTexCoord),
                           dot (aTrsfRow2, aTexCoord));

      vec3 aTexColor = textureLod (
        uTextureSamplers[int (aMaterial.Kd.w)], aTexCoord.st, 0.f).rgb;

      aMaterial.Kd.rgb *= aTexColor;
    }
#endif

    vec3 aNormal = SmoothNormal (aHit.UV, aTriIndex);

    aNormal = normalize (vec3 (dot (aInvTransf0, aNormal),
                               dot (aInvTransf1, aNormal),
                               dot (aInvTransf2, aNormal)));

    SLocalSpace aSpace = LocalSpace (aNormal);

    // Account for self-emission (not stored in the material)
    aRadiance += aThroughput * texelFetch (
      uRaytraceMaterialTexture, MATERIAL_LE (aTriIndex.w)).rgb;

    if (uLightCount > 0 && convolve (aMaterial.Kd.rgb + aMaterial.Ks.rgb, aThroughput) > 0.f)
    {
      int aLightIdx = min (int (floor (RandFloat() * uLightCount)), uLightCount - 1);

      vec4 aLight = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));
      vec4 aParam = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx));

      float aPDF = 1.f / uLightCount, aDistance = MAXFLOAT;

      if (aLight.w != 0.f) // point light source
      {
        aDistance = length (aLight.xyz -= theRay.Origin);

        aLight.xyz = samplePointLight (aLight.xyz, aParam.w /* radius */, aPDF);
      }
      else // directional light source
      {
        aLight.xyz = sampleDirectLight (aLight.xyz, aParam.w /* angle cosine */, aPDF);
      }

      vec3 aContrib = (1.f / aPDF) * aParam.rgb /* Le */ * handleMaterial (
          aMaterial, toLocalSpace (aLight.xyz, aSpace), toLocalSpace (-theRay.Direct, aSpace));

      if (any (greaterThan (aContrib, MIN_CONTRIBUTION))) // first check if light source is important
      {
        SRay aShadow = SRay (theRay.Origin + aLight.xyz * uSceneEpsilon, aLight.xyz);

        aShadow.Origin += aHit.Normal * mix (
          -uSceneEpsilon, uSceneEpsilon, step (0.f, dot (aHit.Normal, aLight.xyz)));

        float aVisibility = SceneAnyHit (aShadow,
          InverseDirection (aLight.xyz), aDistance);

        aRadiance += aVisibility * aThroughput * aContrib;
      }
    }

    vec3 anInput;
    vec3 aWeight;

    isSpecular = sampleMaterial (aMaterial,
      toLocalSpace (-theRay.Direct, aSpace), aThroughput, anInput, aWeight, isTransmit);

    if (isInMedium)
    {
      aThroughput *= exp (-aHit.Time *
        aMaterial.Absorption.w * (UNIT - aMaterial.Absorption.rgb));
    }

    isInMedium = isTransmit ? !isInMedium : isInMedium;

    aThroughput *= aWeight;

    if (all (lessThan (aThroughput, MIN_THROUGHPUT)))
    {
      return vec4 (aRadiance, 0.f);
    }

    anInput = normalize (fromLocalSpace (anInput, aSpace));

    theRay = SRay (theRay.Origin + anInput * uSceneEpsilon +
      aHit.Normal * mix (-uSceneEpsilon, uSceneEpsilon, step (0.f, dot (aHit.Normal, anInput))), anInput);

    theInverse = InverseDirection (anInput);

    anOpenGlDepth = MAXFLOAT; // disable combining image with OpenGL output
  }

  return vec4 (aRadiance, 0.f);
}

#endif
