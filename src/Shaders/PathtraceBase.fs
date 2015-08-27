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

  theInput.z = mix (-theInput.z, theInput.z, step (0.f, theOutput.z));
}

// Types of bounces
#define NON_SPECULAR_BOUNCE 0
#define SPEC_REFLECT_BOUNCE 1
#define SPEC_REFRACT_BOUNCE 2

#define IS_NON_SPEC_BOUNCE(theBounce) (theBounce == 0)
#define IS_ANY_SPEC_BOUNCE(theBounce) (theBounce != 0)
#define IS_REFL_SPEC_BOUNCE(theBounce) (theBounce == 1)
#define IS_REFR_SPEC_BOUNCE(theBounce) (theBounce == 2)

//=======================================================================
// function : sampleSpecularTransmission
// purpose  : Samples specular BTDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
vec3 sampleSpecularTransmission (in vec3 theOutput, out vec3 theInput,
  out int theBounce, in vec3 theWeight, in vec3 theFresnelCoeffs)
{
  vec3 aFresnel = fresnelMedia (theOutput.z, theFresnelCoeffs);

  float aProbability = convolve (aFresnel, theWeight);

  // Check if transmission takes place
  theBounce = RandFloat() <= aProbability ?
    SPEC_REFLECT_BOUNCE : SPEC_REFRACT_BOUNCE;

  // Sample input direction
  if (theBounce == SPEC_REFLECT_BOUNCE)
  {
    theInput = vec3 (-theOutput.x,
                     -theOutput.y,
                      theOutput.z);

    theWeight = aFresnel * (1.f / aProbability);
  }
  else
  {
    transmitted (theFresnelCoeffs.y, theOutput, theInput);

    theWeight = (UNIT - aFresnel) * (1.f / (1.f - aProbability));
  }

  return theWeight;
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

//=======================================================================
// function : sampleMaterial
// purpose  : Samples specified composite material (BSDF)
//=======================================================================
void sampleMaterial (in SMaterial theMaterial,
                     in vec3      theOutput,
                     out vec3     theInput,
                     inout vec3   theWeight,
                     inout int    theBounce)
{
  // Compute the probability of ray reflection
  float aPd = convolve (theMaterial.Kd.rgb, theWeight);
  float aPs = convolve (theMaterial.Ks.rgb, theWeight);
  float aPr = convolve (theMaterial.Kr.rgb, theWeight);
  float aPt = convolve (theMaterial.Kt.rgb, theWeight);

  float aReflection = aPd + aPs + aPr + aPt;

  // Choose BSDF component to sample
  float aKsi = aReflection * RandFloat();

  theBounce = NON_SPECULAR_BOUNCE;

  if (aKsi < aPd) // diffuse reflection
  {
    sampleLambertianReflection (theOutput, theInput);

    theWeight *= theMaterial.Kd.rgb * (aReflection / aPd);
  }
  else if (aKsi < aPd + aPs) //  glossy reflection
  {
    theWeight *= theMaterial.Ks.rgb * (aReflection / aPs) *
      sampleBlinnReflection (theOutput, theInput, theMaterial.Fresnel, theMaterial.Ks.w);
  }
  else if (aKsi < aPd + aPs + aPr) //  specular reflection
  {
    theWeight *= theMaterial.Kr.rgb * (aReflection / aPr) *
      sampleSpecularReflection (theOutput, theInput, theMaterial.Fresnel);

    theBounce = SPEC_REFLECT_BOUNCE; // specular bounce
  }
  else //  specular transmission
  {
    theWeight *= theMaterial.Kt.rgb * (aReflection / aPt) *
      sampleSpecularTransmission (theOutput, theInput, theBounce, theWeight, theMaterial.Fresnel);
  }

  // path termination for extra small weights
  theWeight = mix (theWeight, ZERO, float (aReflection < 1e-3f));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Handlers and samplers for light sources
//////////////////////////////////////////////////////////////////////////////////////////////

//=======================================================================
// function : handlePointLight
// purpose  :
//=======================================================================
float handlePointLight (in vec3 theInput, in vec3 theToLight, in float theRadius, in float theDistance)
{
  float aDistance = dot (theToLight, theToLight);

  float aCosMax = inversesqrt (1.f + theRadius * theRadius / aDistance);

  return float (aDistance < theDistance * theDistance) *
    step (aCosMax, dot (theToLight, theInput) * inversesqrt (aDistance));
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
// function : sampleLight
// purpose  : general sampling function for directional and point lights
//=======================================================================
vec3 sampleLight (in vec3 theToLight, in bool isDirectional, in float theSmoothness, inout float thePDF)
{
  SLocalSpace aSpace = LocalSpace (theToLight);

  // for point lights smoothness defines radius
  float aCosMax = isDirectional ? theSmoothness :
    inversesqrt (1.f + theSmoothness * theSmoothness / dot (theToLight, theToLight));

  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  float aTmp = 1.f - aKsi2 * (1.f - aCosMax);

  vec3 anInput = vec3 (cos (2.f * M_PI * aKsi1),
                       sin (2.f * M_PI * aKsi1),
                       aTmp);

  anInput.xy *= sqrt (1.f - aTmp * aTmp);

  thePDF *= (aCosMax < 1.f) ? 1.f / (2.f * M_PI) / (1.f - aCosMax) : 1.f;

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
// function: intersectLight
// purpose : Checks intersections with light sources
// =======================================================================
vec3 intersectLight (in SRay theRay, in bool isViewRay, in int theBounce, in float theDistance)
{
  vec3 aRadiance = ZERO;

  if ((isViewRay || IS_REFR_SPEC_BOUNCE(theBounce)) && uSphereMapForBack == 0)
  {
    aRadiance = BackgroundColor().xyz;
  }
  else
  {
    aRadiance = FetchEnvironment (Latlong (theRay.Direct)).xyz;
  }

  // Apply gamma correction (gamma is 2)
  aRadiance = aRadiance * aRadiance * float (theDistance == MAXFLOAT);

  for (int aLightIdx = 0; aLightIdx < uLightCount && (isViewRay || IS_ANY_SPEC_BOUNCE(theBounce)); ++aLightIdx)
  {
    vec4 aLight = texelFetch (
      uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));
    vec4 aParam = texelFetch (
      uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx));

    if (aLight.w != 0.f) // point light source
    {
      aRadiance += aParam.rgb * handlePointLight (
        theRay.Direct, aLight.xyz - theRay.Origin, aParam.w /* radius */, theDistance);
    }
    else if (theDistance == MAXFLOAT) // directional light source
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

// Enables expiremental russian roulette sampling
#define RUSSIAN_ROULETTE

//=======================================================================
// function : PathTrace
// purpose  : Calculates radiance along the given ray
//=======================================================================
vec4 PathTrace (in SRay theRay, in vec3 theInverse)
{
  float anOpenGlDepth = ComputeOpenGlDepth (theRay);

  vec3 aRadiance   = ZERO;
  vec3 aThroughput = UNIT;

  int aBounce = 0; // type of previous hit point
  int aTrsfId = 0; // offset of object transform

  bool isInMedium = false;

  for (int aDepth = 0; aDepth < NB_BOUNCES; ++aDepth)
  {
    SIntersect aHit = SIntersect (MAXFLOAT, vec2 (ZERO), ZERO);

    ivec4 aTriIndex = SceneNearestHit (theRay, theInverse, aHit, aTrsfId);

    // check implicit path
    vec3 aLe = intersectLight (theRay,
      aDepth == 0 /* is view ray */, aBounce, aHit.Time);

    if (any (greaterThan (aLe, ZERO)) || aTriIndex.x == -1)
    {
      aRadiance += aThroughput * aLe; break; // terminate path
    }

    vec3 aInvTransf0 = texelFetch (uSceneTransformTexture, aTrsfId + 0).xyz;
    vec3 aInvTransf1 = texelFetch (uSceneTransformTexture, aTrsfId + 1).xyz;
    vec3 aInvTransf2 = texelFetch (uSceneTransformTexture, aTrsfId + 2).xyz;

    // compute geometrical normal
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

    theRay.Origin += theRay.Direct * aHit.Time; // get new intersection point

    // fetch material (BSDF)
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
        sampler2D (uTextureSamplers[int (aMaterial.Kd.w)]), aTexCoord.st, 0.f).rgb;

      aMaterial.Kd.rgb *= aTexColor;
    }
#endif

    // compute smooth normal
    vec3 aNormal = SmoothNormal (aHit.UV, aTriIndex);

    aNormal = normalize (vec3 (dot (aInvTransf0, aNormal),
                               dot (aInvTransf1, aNormal),
                               dot (aInvTransf2, aNormal)));

    SLocalSpace aSpace = LocalSpace (aNormal);

    // account for self-emission (not stored in the material)
    aRadiance += aThroughput * texelFetch (
      uRaytraceMaterialTexture, MATERIAL_LE (aTriIndex.w)).rgb;

    if (uLightCount > 0 && convolve (aMaterial.Kd.rgb + aMaterial.Ks.rgb, aThroughput) > 0.f)
    {
      int aLightIdx = min (int (floor (RandFloat() * uLightCount)), uLightCount - 1);

      vec4 aLight = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));
      vec4 aParam = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx));

      // 'w' component is 0 for infinite light and 1 for point light
      aLight.xyz -= mix (ZERO, theRay.Origin, aLight.w);

      float aPDF = 1.f / uLightCount, aDistance = length (aLight.xyz);

      aLight.xyz = sampleLight (aLight.xyz * (1.f / aDistance),
        aLight.w == 0.f /* is infinite */, aParam.w /* angle cosine */, aPDF);

      vec3 aContrib = (1.f / aPDF) * aParam.rgb /* Le */ * handleMaterial (
          aMaterial, toLocalSpace (aLight.xyz, aSpace), toLocalSpace (-theRay.Direct, aSpace));

      if (any (greaterThan (aContrib, MIN_CONTRIBUTION))) // first check if light source is important
      {
        SRay aShadow = SRay (theRay.Origin + aLight.xyz * uSceneEpsilon, aLight.xyz);

        aShadow.Origin += aHit.Normal * mix (
          -uSceneEpsilon, uSceneEpsilon, step (0.f, dot (aHit.Normal, aLight.xyz)));

        float aVisibility = SceneAnyHit (aShadow,
          InverseDirection (aLight.xyz), aLight.w == 0.f ? MAXFLOAT : aDistance);

        aRadiance += aVisibility * aThroughput * aContrib;
      }
    }

    vec3 anInput;

    sampleMaterial (aMaterial,
      toLocalSpace (-theRay.Direct, aSpace), anInput, aThroughput, aBounce);

    if (isInMedium)
    {
      aThroughput *= exp (-aHit.Time *
        aMaterial.Absorption.w * (UNIT - aMaterial.Absorption.rgb));
    }

    isInMedium = IS_REFR_SPEC_BOUNCE(aBounce) ? !isInMedium : isInMedium;

#ifndef RUSSIAN_ROULETTE
    if (all (lessThan (aThroughput, MIN_THROUGHPUT)))
    {
      aDepth = INVALID_BOUNCES; // terminate path
    }
#else
    float aSurvive = aDepth < 3 ? 1.f : min (dot (LUMA, aThroughput), 0.95f);

    if (RandFloat() > aSurvive)
    {
      aDepth = INVALID_BOUNCES; // terminate path
    }

    aThroughput /= aSurvive;
#endif

    anInput = normalize (fromLocalSpace (anInput, aSpace));

    theRay = SRay (theRay.Origin + anInput * uSceneEpsilon +
      aHit.Normal * mix (-uSceneEpsilon, uSceneEpsilon, step (0.f, dot (aHit.Normal, anInput))), anInput);

    theInverse = InverseDirection (anInput);

    anOpenGlDepth = MAXFLOAT; // disable combining image with OpenGL output
  }

  return vec4 (aRadiance, 0.f);
}

#endif
