#ifdef _MSC_VER
  #define PATH_TRACING // just for editing in MS VS

  #define in
  #define out
  #define inout

  typedef struct { float x; float y; } vec2;
  typedef struct { float x; float y; float z; } vec3;
  typedef struct { float x; float y; float z; float w; } vec4;
#endif

#ifdef PATH_TRACING

//! Number of previously rendered frames.
uniform int uAccumSamples;

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
// function : buildLocalSpace
// purpose  : Generates local space for the given normal
//=======================================================================
SLocalSpace buildLocalSpace (in vec3 theNormal)
{
  vec3 anAxisX = vec3 (theNormal.z, 0.f, -theNormal.x);
  vec3 anAxisY = vec3 (0.f, -theNormal.z, theNormal.y);

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
vec3 fresnelMedia (in float theCosI, in vec3 theFresnel)
{
  if (theFresnel.x > FRESNEL_SCHLICK)
  {
    return fresnelSchlick (abs (theCosI), theFresnel);
  }

  if (theFresnel.x > FRESNEL_CONSTANT)
  {
    return vec3 (theFresnel.z);
  }

  if (theFresnel.x > FRESNEL_CONDUCTOR)
  {
    return vec3 (fresnelConductor (abs (theCosI), theFresnel.y, theFresnel.z));
  }

  return vec3 (fresnelDielectric (theCosI, theFresnel.y));
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

  // Handle total internal reflection (TIR)
  float aSinT2 = anEta * anEta * (1.f - theIncident.z * theIncident.z);

  // Compute direction of transmitted ray
  float aCosT = sqrt (1.f - min (aSinT2, 1.f)) * sign (-theIncident.z);

  theTransmit = normalize (vec3 (-anEta * theIncident.x,
                                 -anEta * theIncident.y,
                                  aCosT));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Handlers and samplers for materials
//////////////////////////////////////////////////////////////////////////////////////////////

//=======================================================================
// function : HandleLambertianReflection
// purpose  : Handles Lambertian BRDF, with cos(N, PSI)
//=======================================================================
float HandleLambertianReflection (in vec3 theInput, in vec3 theOutput)
{
  return (theInput.z <= 0.f || theOutput.z <= 0.f) ? 0.f : theInput.z * (1.f / M_PI);
}

//=======================================================================
// function : SmithG1
// purpose  :
//=======================================================================
float SmithG1 (in vec3 theDirection, in vec3 theM, in float theRoughness)
{
  if (dot (theDirection, theM) * theDirection.z <= 0.f)
  {
    return 0.f;
  }

  float aTanThetaM = sqrt (1.f - theDirection.z * theDirection.z) / theDirection.z;

  if (aTanThetaM == 0.0f)
  {
    return 1.f;
  }

  float aVal = 1.f / (theRoughness * aTanThetaM);

  if (aVal >= 1.6f)
  {
    return 1.f;
  }

  // Use fast and accurate rational approximation to the
  // shadowing-masking function (from Mitsuba renderer)
  float aSqr = aVal * aVal;

  return (3.535f * aVal + 2.181f * aSqr) / (1.f + 2.276f * aVal + 2.577f * aSqr);
}

//=======================================================================
// function : HandleBlinnReflection
// purpose  : Handles Blinn glossy BRDF, with cos(N, PSI)
//=======================================================================
vec3 HandleBlinnReflection (in vec3 theInput, in vec3 theOutput, in vec3 theFresnel, in float theRoughness)
{
  // calculate the reflection half-vec
  vec3 aH = normalize (theInput + theOutput);

  // roughness value -> Blinn exponent
  float aPower = max (2.f / (theRoughness * theRoughness) - 2.f, 0.f);

  // calculate microfacet distribution
  float aD = (aPower + 2.f) * (1.f / M_2_PI) * pow (aH.z, aPower);

  // calculate shadow-masking function
  float aG = SmithG1 (theOutput, aH, theRoughness) * SmithG1 (theInput, aH, theRoughness);

  // return total amount of reflection
  return (theInput.z <= 0.f || theOutput.z <= 0.f) ? ZERO :
    aD * aG / (4.f * theOutput.z) * fresnelMedia (dot (theOutput, aH), theFresnel);
}

//=======================================================================
// function : HandleMaterial
// purpose  : Returns BSDF value for specified material, with cos(N, PSI)
//=======================================================================
vec3 HandleMaterial (in SMaterial theBSDF, in vec3 theInput, in vec3 theOutput)
{
  return theBSDF.Kd.rgb * HandleLambertianReflection (theInput, theOutput) +
    theBSDF.Ks.rgb * HandleBlinnReflection (theInput, theOutput, theBSDF.Fresnel, theBSDF.Ks.w);
}

//=======================================================================
// function : SampleLambertianReflection
// purpose  : Samples Lambertian BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
vec3 SampleLambertianReflection (in vec3 theOutput, out vec3 theInput, inout float thePDF)
{
  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  float aTemp = sqrt (aKsi2);

  theInput = vec3 (aTemp * cos (M_2_PI * aKsi1),
                   aTemp * sin (M_2_PI * aKsi1),
                   sqrt (1.f - aKsi2));

  thePDF *= abs (theInput.z) * (1.f / M_PI);

  return (theInput.z <= 0.f || theOutput.z <= 0.f) ? ZERO : UNIT;
}

//=======================================================================
// function : SampleBlinnReflection
// purpose  : Samples Blinn BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//            The BRDF is a product of three main terms, D, G, and F,
//            which is then divided by two cosine terms. Here we perform
//            importance sample the D part of the Blinn model; trying to
//            develop a sampling procedure that accounted for all of the
//            terms would be complex, and it is the D term that accounts
//            for most of the variation.
//=======================================================================
vec3 SampleBlinnReflection (in vec3 theOutput, out vec3 theInput, in vec3 theFresnel, in float theRoughness, inout float thePDF)
{
  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  // roughness value --> Blinn exponent
  float aPower = max (2.f / (theRoughness * theRoughness) - 2.f, 0.f);

  // normal from microface distribution
  float aCosThetaM = pow (aKsi1, 1.f / (aPower + 2.f));

  vec3 aM = vec3 (cos (M_2_PI * aKsi2),
                  sin (M_2_PI * aKsi2),
                  aCosThetaM);

  aM.xy *= sqrt (1.f - aCosThetaM * aCosThetaM);

  // calculate PDF of sampled direction
  thePDF *= (aPower + 2.f) * (1.f / M_2_PI) * pow (aCosThetaM, aPower + 1.f);

  float aCosDelta = dot (theOutput, aM);

  // pick input based on half direction
  theInput = -theOutput + 2.f * aCosDelta * aM;

  if (theInput.z <= 0.f || theOutput.z <= 0.f)
  {
    return ZERO;
  }

  // Jacobian of half-direction mapping
  thePDF /= 4.f * dot (theInput, aM);

  // compute shadow-masking coefficient
  float aG = SmithG1 (theOutput, aM, theRoughness) * SmithG1 (theInput, aM, theRoughness);

  return aG * aCosDelta / (theOutput.z * aM.z) * fresnelMedia (aCosDelta, theFresnel);
}

//=======================================================================
// function : SampleSpecularReflection
// purpose  : Samples specular BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
vec3 SampleSpecularReflection (in vec3 theOutput, out vec3 theInput, in vec3 theFresnel)
{
  // Sample input direction
  theInput = vec3 (-theOutput.x,
                   -theOutput.y,
                    theOutput.z);

  return fresnelMedia (theOutput.z, theFresnel);
}

//=======================================================================
// function : SampleSpecularTransmission
// purpose  : Samples specular BTDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
vec3 SampleSpecularTransmission (in vec3 theOutput,
  out vec3 theInput, in vec3 theWeight, in vec3 theFresnel, inout bool theInside)
{
  vec3 aFactor = fresnelMedia (theOutput.z, theFresnel);

  float aReflection = convolve (aFactor, theWeight);

  // sample specular BRDF/BTDF
  if (RandFloat() <= aReflection)
  {
    theInput = vec3 (-theOutput.x,
                     -theOutput.y,
                      theOutput.z);

    theWeight = aFactor * (1.f / aReflection);
  }
  else
  {
    theInside = !theInside;

    transmitted (theFresnel.y, theOutput, theInput);

    theWeight = (UNIT - aFactor) * (1.f / (1.f - aReflection));
  }

  return theWeight;
}

#define FLT_EPSILON 1.0e-5F

//=======================================================================
// function : BsdfPdf
// purpose  : Calculates BSDF of sampling input knowing output
//=======================================================================
float BsdfPdf (in SMaterial theBSDF,
               in vec3      theOutput,
               in vec3      theInput,
               in vec3      theWeight)
{
  float aPd = convolve (theBSDF.Kd.rgb, theWeight);
  float aPs = convolve (theBSDF.Ks.rgb, theWeight);
  float aPr = convolve (theBSDF.Kr.rgb, theWeight);
  float aPt = convolve (theBSDF.Kt.rgb, theWeight);

  float aReflection = aPd + aPs + aPr + aPt;

  float aPDF = 0.f; // PDF of sampling input direction

  if (theInput.z * theOutput.z > 0.f)
  {
    vec3 aHalf = normalize (theInput + theOutput);

    // roughness value --> Blinn exponent
    float aPower = max (2.f / (theBSDF.Ks.w * theBSDF.Ks.w) - 2.f, 0.f);

    aPDF = aPd * abs (theInput.z / M_PI) +
      aPs * (aPower + 2.f) * (1.f / M_2_PI) * pow (aHalf.z, aPower + 1.f) / (4.f * dot (theInput, aHalf));
  }

  return aPDF / aReflection;
}

//! Tool macro to handle sampling of particular BxDF
#define PICK_BXDF(p, k) aPDF = p / aReflection; theWeight *= k / aPDF;

//=======================================================================
// function : SampleBsdf
// purpose  : Samples specified composite material (BSDF)
//=======================================================================
float SampleBsdf (in SMaterial theBSDF,
                  in vec3      theOutput,
                  out vec3     theInput,
                  inout vec3   theWeight,
                  inout bool   theInside)
{
  // compute probability of each reflection type (BxDF)
  float aPd = convolve (theBSDF.Kd.rgb, theWeight);
  float aPs = convolve (theBSDF.Ks.rgb, theWeight);
  float aPr = convolve (theBSDF.Kr.rgb, theWeight);
  float aPt = convolve (theBSDF.Kt.rgb, theWeight);

  float aReflection = aPd + aPs + aPr + aPt;

  // choose BxDF component to sample
  float aKsi = aReflection * RandFloat();

  // BxDF's PDF of sampled direction
  float aPDF = 0.f;

  if (aKsi < aPd) // diffuse reflection
  {
    PICK_BXDF (aPd, theBSDF.Kd.rgb);

    theWeight *= SampleLambertianReflection (theOutput, theInput, aPDF);
  }
  else if (aKsi < aPd + aPs) // glossy reflection
  {
    PICK_BXDF (aPs, theBSDF.Ks.rgb);

    theWeight *= SampleBlinnReflection (theOutput, theInput, theBSDF.Fresnel, theBSDF.Ks.w, aPDF);
  }
  else if (aKsi < aPd + aPs + aPr) // specular reflection
  {
    PICK_BXDF (aPr, theBSDF.Kr.rgb);

    aPDF = MAXFLOAT;

    theWeight *= SampleSpecularReflection (theOutput, theInput, theBSDF.Fresnel);
  }
  else if (aKsi < aReflection) // specular transmission
  {
    PICK_BXDF (aPt, theBSDF.Kt.rgb);

    aPDF = MAXFLOAT;

    theWeight *= SampleSpecularTransmission (theOutput, theInput, theWeight, theBSDF.Fresnel, theInside);
  }

  // path termination for extra small weights
  theWeight = mix (ZERO, theWeight, step (FLT_EPSILON, aReflection));

  return aPDF;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Handlers and samplers for light sources
//////////////////////////////////////////////////////////////////////////////////////////////

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

//=======================================================================
// function : SampleLight
// purpose  : General sampling function for directional and point lights
//=======================================================================
vec3 SampleLight (in vec3 theToLight, inout float theDistance, in bool isInfinite, in float theSmoothness, inout float thePDF)
{
  SLocalSpace aSpace = buildLocalSpace (theToLight * (1.f / theDistance));

  // for point lights smoothness defines radius
  float aCosMax = isInfinite ? theSmoothness :
    inversesqrt (1.f + theSmoothness * theSmoothness / (theDistance * theDistance));

  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  float aTmp = 1.f - aKsi2 * (1.f - aCosMax);

  vec3 anInput = vec3 (cos (M_2_PI * aKsi1),
                       sin (M_2_PI * aKsi1),
                       aTmp);

  anInput.xy *= sqrt (1.f - aTmp * aTmp);

  thePDF = (aCosMax < 1.f) ? (thePDF / M_2_PI) / (1.f - aCosMax) : MAXFLOAT;

  return normalize (fromLocalSpace (anInput, aSpace));
}

//=======================================================================
// function : HandlePointLight
// purpose  :
//=======================================================================
float HandlePointLight (in vec3 theInput, in vec3 theToLight, in float theRadius, in float theDistance, inout float thePDF)
{
  float aCosMax = inversesqrt (1.f + theRadius * theRadius / (theDistance * theDistance));

  float aVisibility = step (aCosMax, dot (theInput, theToLight));

  thePDF *= step (-1.f, -aCosMax) * aVisibility * (1.f / M_2_PI) / (1.f - aCosMax);

  return aVisibility;
}

//=======================================================================
// function : HandleDistantLight
// purpose  :
//=======================================================================
float HandleDistantLight (in vec3 theInput, in vec3 theToLight, in float theCosMax, inout float thePDF)
{
  float aVisibility = step (theCosMax, dot (theInput, theToLight));

  thePDF *= step (-1.f, -theCosMax) * aVisibility * (1.f / M_2_PI) / (1.f - theCosMax);

  return aVisibility;
}

// =======================================================================
// function: IntersectLight
// purpose : Checks intersections with light sources
// =======================================================================
vec3 IntersectLight (in SRay theRay, in int theDepth, in float theHitDistance, out float thePDF)
{
  vec3 aTotalRadiance = ZERO;

  thePDF = 0.f; // PDF of sampling light sources

  for (int aLightIdx = 0; aLightIdx < uLightCount; ++aLightIdx)
  {
    vec4 aLight = texelFetch (
      uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));
    vec4 aParam = texelFetch (
      uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx));

    // W component: 0 for infinite light and 1 for point light
    aLight.xyz -= mix (ZERO, theRay.Origin, aLight.w);

    float aPDF = 1.f / uLightCount;

    if (aLight.w != 0.f) // point light source
    {
      float aCenterDst = length (aLight.xyz);

      if (aCenterDst < theHitDistance)
      {
        float aVisibility = HandlePointLight (
          theRay.Direct, normalize (aLight.xyz), aParam.w /* radius */, aCenterDst, aPDF);

        if (aVisibility > 0.f)
        {
          theHitDistance = aCenterDst;
          aTotalRadiance = aParam.rgb;

          thePDF = aPDF;
        }
      }
    }
    else if (theHitDistance == MAXFLOAT) // directional light source
    {
      aTotalRadiance += aParam.rgb * HandleDistantLight (
        theRay.Direct, aLight.xyz, aParam.w /* angle cosine */, aPDF);

      thePDF += aPDF;
    }
  }

  if (thePDF == 0.f && theHitDistance == MAXFLOAT) // light source not found
  {
    if (theDepth + uSphereMapForBack == 0) // view ray and map is hidden
    {
      aTotalRadiance = pow (BackgroundColor().rgb, vec3 (2.f));
    }
    else
    {
      aTotalRadiance = pow (FetchEnvironment (Latlong (theRay.Direct)).rgb, vec3 (2.f));
    }
  }
  
  return aTotalRadiance;
}

#define MIN_THROUGHPUT   vec3 (1.0e-3f)
#define MIN_CONTRIBUTION vec3 (1.0e-2f)

#define MATERIAL_KD(index)      (18 * index + 11)
#define MATERIAL_KR(index)      (18 * index + 12)
#define MATERIAL_KT(index)      (18 * index + 13)
#define MATERIAL_KS(index)      (18 * index + 14)
#define MATERIAL_LE(index)      (18 * index + 15)
#define MATERIAL_FRESNEL(index) (18 * index + 16)
#define MATERIAL_ABSORPT(index) (18 * index + 17)

// Enables expiremental russian roulette sampling
#define RUSSIAN_ROULETTE

//! Frame step to increase number of bounces
#define FRAME_STEP 5

//=======================================================================
// function : PathTrace
// purpose  : Calculates radiance along the given ray
//=======================================================================
vec4 PathTrace (in SRay theRay, in vec3 theInverse)
{
  float aRaytraceDepth = MAXFLOAT;

  vec3 aRadiance   = ZERO;
  vec3 aThroughput = UNIT;

  int  aTransfID = 0;     // ID of object transformation
  bool aInMedium = false; // is the ray inside an object

  float aExpPDF = 1.f;
  float aImpPDF = 1.f;

  for (int aDepth = 0; aDepth < NB_BOUNCES; ++aDepth)
  {
    SIntersect aHit = SIntersect (MAXFLOAT, vec2 (ZERO), ZERO);

    ivec4 aTriIndex = SceneNearestHit (theRay, theInverse, aHit, aTransfID);

    // check implicit path
    vec3 aLe = IntersectLight (theRay, aDepth, aHit.Time, aExpPDF);

    if (any (greaterThan (aLe, ZERO)) || aTriIndex.x == -1)
    {
      float aMIS = (aDepth == 0 || aImpPDF == MAXFLOAT) ? 1.f :
        aImpPDF * aImpPDF / (aExpPDF * aExpPDF + aImpPDF * aImpPDF);

      aRadiance += aThroughput * aLe * aMIS; break; // terminate path
    }

    vec3 aInvTransf0 = texelFetch (uSceneTransformTexture, aTransfID + 0).xyz;
    vec3 aInvTransf1 = texelFetch (uSceneTransformTexture, aTransfID + 1).xyz;
    vec3 aInvTransf2 = texelFetch (uSceneTransformTexture, aTransfID + 2).xyz;

    // compute geometrical normal
    aHit.Normal = normalize (vec3 (dot (aInvTransf0, aHit.Normal),
                                   dot (aInvTransf1, aHit.Normal),
                                   dot (aInvTransf2, aHit.Normal)));

    theRay.Origin += theRay.Direct * aHit.Time; // get new intersection point

    // evaluate depth on first hit
    if (aDepth == 0)
    {
      vec4 aNDCPoint = uViewMat * vec4 (theRay.Origin, 1.f);

      float aPolygonOffset = PolygonOffset (aHit.Normal, theRay.Origin);
      aRaytraceDepth = (aNDCPoint.z / aNDCPoint.w + aPolygonOffset * POLYGON_OFFSET_SCALE) * 0.5f + 0.5f;
    }

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

      vec4 aTexColor = textureLod (
        sampler2D (uTextureSamplers[int (aMaterial.Kd.w)]), aTexCoord.st, 0.f);

      aMaterial.Kd.rgb *= (aTexColor.rgb, aTexColor.rgb) * aTexColor.w; // de-gamma correction (for gamma = 2)

      if (aTexColor.w != 1.0f)
      {
        // mix transparency BTDF with texture alpha-channel
        aMaterial.Kt = (UNIT - aTexColor.www) + aTexColor.w * aMaterial.Kt;
      }
    }
#endif

    // compute smooth normal
    vec3 aNormal = SmoothNormal (aHit.UV, aTriIndex);

    aNormal = normalize (vec3 (dot (aInvTransf0, aNormal),
                               dot (aInvTransf1, aNormal),
                               dot (aInvTransf2, aNormal)));

    SLocalSpace aSpace = buildLocalSpace (aNormal);

    // account for self-emission (not stored in the material)
    aRadiance += aThroughput * texelFetch (
      uRaytraceMaterialTexture, MATERIAL_LE (aTriIndex.w)).rgb;

    if (uLightCount > 0 && convolve (aMaterial.Kd.rgb + aMaterial.Ks.rgb, aThroughput) > 0.f)
    {
      aExpPDF = 1.f / uLightCount;

      int aLightIdx = min (int (floor (RandFloat() * uLightCount)), uLightCount - 1);

      vec4 aLight = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));
      vec4 aParam = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx));

      // 'w' component is 0 for infinite light and 1 for point light
      aLight.xyz -= mix (ZERO, theRay.Origin, aLight.w);

      float aDistance = length (aLight.xyz);

      aLight.xyz = SampleLight (aLight.xyz, aDistance,
        aLight.w == 0.f /* is infinite */, aParam.w /* max cos or radius */, aExpPDF);

      aImpPDF = BsdfPdf (aMaterial,
        toLocalSpace (-theRay.Direct, aSpace), toLocalSpace (aLight.xyz, aSpace), aThroughput);

      // MIS weight including division by explicit PDF
      float aMIS = (aExpPDF == MAXFLOAT) ? 1.f : aExpPDF / (aExpPDF * aExpPDF + aImpPDF * aImpPDF);

      vec3 aContrib = aMIS * aParam.rgb /* Le */ * HandleMaterial (
          aMaterial, toLocalSpace (aLight.xyz, aSpace), toLocalSpace (-theRay.Direct, aSpace));

      if (any (greaterThan (aContrib, MIN_CONTRIBUTION))) // check if light source is important
      {
        SRay aShadow = SRay (theRay.Origin + aLight.xyz * uSceneEpsilon, aLight.xyz);

        aShadow.Origin += aHit.Normal * mix (
          -uSceneEpsilon, uSceneEpsilon, step (0.f, dot (aHit.Normal, aLight.xyz)));

        float aVisibility = SceneAnyHit (aShadow,
          InverseDirection (aLight.xyz), aLight.w == 0.f ? MAXFLOAT : aDistance);

        aRadiance += aVisibility * aThroughput * aContrib;
      }
    }

    if (aInMedium) // handle attenuation
    {
      aThroughput *= exp (-aHit.Time *
        aMaterial.Absorption.w * (UNIT - aMaterial.Absorption.rgb));
    }

    vec3 anInput = UNIT; // sampled input direction

    aImpPDF = SampleBsdf (aMaterial,
      toLocalSpace (-theRay.Direct, aSpace), anInput, aThroughput, aInMedium);

    float aSurvive = 1.f;

#ifdef RUSSIAN_ROULETTE
    aSurvive = aDepth < 3 ? 1.f : min (dot (LUMA, aThroughput), 0.95f);
#endif

    if (RandFloat() > aSurvive || all (lessThan (aThroughput, MIN_THROUGHPUT)) || aDepth >= uAccumSamples / FRAME_STEP + step (1.f / M_PI, aImpPDF))
    {
      aDepth = INVALID_BOUNCES; // terminate path
    }

#ifdef RUSSIAN_ROULETTE
    aThroughput /= aSurvive;
#endif

    anInput = normalize (fromLocalSpace (anInput, aSpace));

    theRay = SRay (theRay.Origin + anInput * uSceneEpsilon +
      aHit.Normal * mix (-uSceneEpsilon, uSceneEpsilon, step (0.f, dot (aHit.Normal, anInput))), anInput);

    theInverse = InverseDirection (anInput);
  }

  gl_FragDepth = aRaytraceDepth;

  return vec4 (aRadiance, aRaytraceDepth);
}

#endif
