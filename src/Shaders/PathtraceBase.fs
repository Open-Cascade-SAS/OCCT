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
struct SBSDF
{
  //! Weight of coat specular/glossy BRDF.
  vec4 Kc;

  //! Weight of base diffuse BRDF.
  vec4 Kd;

  //! Weight of base specular/glossy BRDF.
  vec4 Ks;

  //! Weight of base specular/glossy BTDF.
  vec3 Kt;

  //! Fresnel coefficients of coat layer.
  vec3 FresnelCoat;

  //! Fresnel coefficients of base layer.
  vec3 FresnelBase;
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
  float aFresnel = 1.f;

  float anEtaI = theCosI > 0.f ? 1.f : theIndex;
  float anEtaT = theCosI > 0.f ? theIndex : 1.f;

  float aSinT2 = (anEtaI * anEtaI) / (anEtaT * anEtaT) * (1.f - theCosI * theCosI);

  if (aSinT2 < 1.f)
  {
    aFresnel = fresnelDielectric (abs (theCosI), sqrt (1.f - aSinT2), anEtaI, anEtaT);
  }

  return aFresnel;
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
  vec3 aFresnel;

  if (theFresnel.x > FRESNEL_SCHLICK)
  {
    aFresnel = fresnelSchlick (abs (theCosI), theFresnel);
  }
  else if (theFresnel.x > FRESNEL_CONSTANT)
  {
    aFresnel = vec3 (theFresnel.z);
  }
  else if (theFresnel.x > FRESNEL_CONDUCTOR)
  {
    aFresnel = vec3 (fresnelConductor (abs (theCosI), theFresnel.y, theFresnel.z));
  }
  else
  {
    aFresnel = vec3 (fresnelDielectric (theCosI, theFresnel.y));
  }

  return aFresnel;
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
// function : EvalLambertianReflection
// purpose  : Evaluates Lambertian BRDF, with cos(N, PSI)
//=======================================================================
float EvalLambertianReflection (in vec3 theWi, in vec3 theWo)
{
  return (theWi.z <= 0.f || theWo.z <= 0.f) ? 0.f : theWi.z * (1.f / M_PI);
}

#define FLT_EPSILON 1.0e-5f

//=======================================================================
// function : SmithG1
// purpose  :
//=======================================================================
float SmithG1 (in vec3 theDirection, in vec3 theM, in float theRoughness)
{
  float aResult = 0.f;

  if (dot (theDirection, theM) * theDirection.z > 0.f)
  {
    float aTanThetaM = sqrt (1.f - theDirection.z * theDirection.z) / theDirection.z;

    if (aTanThetaM == 0.f)
    {
      aResult = 1.f;
    }
    else
    {
      float aVal = 1.f / (theRoughness * aTanThetaM);

      // Use rational approximation to shadowing-masking function (from Mitsuba)
      aResult = (3.535f + 2.181f * aVal) / (1.f / aVal + 2.276f + 2.577f * aVal);
    }
  }

  return min (aResult, 1.f);
}

//=======================================================================
// function : EvalBlinnReflection
// purpose  : Evaluates Blinn glossy BRDF, with cos(N, PSI)
//=======================================================================
vec3 EvalBlinnReflection (in vec3 theWi, in vec3 theWo, in vec3 theFresnel, in float theRoughness)
{
  // calculate the reflection half-vec
  vec3 aH = normalize (theWi + theWo);

  // roughness value -> Blinn exponent
  float aPower = max (2.f / (theRoughness * theRoughness) - 2.f, 0.f);

  // calculate microfacet distribution
  float aD = (aPower + 2.f) * (1.f / M_2_PI) * pow (aH.z, aPower);

  // calculate shadow-masking function
  float aG = SmithG1 (theWo, aH, theRoughness) *
             SmithG1 (theWi, aH, theRoughness);

  // return total amount of reflection
  return (theWi.z <= 0.f || theWo.z <= 0.f) ? ZERO :
    aD * aG / (4.f * theWo.z) * fresnelMedia (dot (theWo, aH), theFresnel);
}

//=======================================================================
// function : EvalBsdfLayered
// purpose  : Evaluates BSDF for specified material, with cos(N, PSI)
//=======================================================================
vec3 EvalBsdfLayered (in SBSDF theBSDF, in vec3 theWi, in vec3 theWo)
{
#ifdef TWO_SIDED_BXDF
  theWi.z *= sign (theWi.z);
  theWo.z *= sign (theWo.z);
#endif

  vec3 aBxDF = theBSDF.Kd.rgb * EvalLambertianReflection (theWi, theWo);

  if (theBSDF.Ks.w > FLT_EPSILON)
  {
    aBxDF += theBSDF.Ks.rgb * EvalBlinnReflection (theWi, theWo, theBSDF.FresnelBase, theBSDF.Ks.w);
  }

  aBxDF *= UNIT - fresnelMedia (theWo.z, theBSDF.FresnelCoat);

  if (theBSDF.Kc.w > FLT_EPSILON)
  {
    aBxDF += theBSDF.Kc.rgb * EvalBlinnReflection (theWi, theWo, theBSDF.FresnelCoat, theBSDF.Kc.w);
  }

  return aBxDF;
}

//=======================================================================
// function : SampleLambertianReflection
// purpose  : Samples Lambertian BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//=======================================================================
vec3 SampleLambertianReflection (in vec3 theWo, out vec3 theWi, inout float thePDF)
{
  float aKsi1 = RandFloat();
  float aKsi2 = RandFloat();

  theWi = vec3 (cos (M_2_PI * aKsi1),
                sin (M_2_PI * aKsi1),
                sqrt (1.f - aKsi2));

  theWi.xy *= sqrt (aKsi2);

#ifdef TWO_SIDED_BXDF
  theWi.z *= sign (theWo.z);
#endif

  thePDF *= theWi.z * (1.f / M_PI);

#ifdef TWO_SIDED_BXDF
  return UNIT;
#else
  return UNIT * step (0.f, theWo.z);
#endif
}

//=======================================================================
// function : SampleGlossyBlinnReflection
// purpose  : Samples Blinn BRDF, W = BRDF * cos(N, PSI) / PDF(PSI)
//            The BRDF is a product of three main terms, D, G, and F,
//            which is then divided by two cosine terms. Here we perform
//            importance sample the D part of the Blinn model; trying to
//            develop a sampling procedure that accounted for all of the
//            terms would be complex, and it is the D term that accounts
//            for most of the variation.
//=======================================================================
vec3 SampleGlossyBlinnReflection (in vec3 theWo, out vec3 theWi, in vec3 theFresnel, in float theRoughness, inout float thePDF)
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

#ifdef TWO_SIDED_BXDF
  bool toFlip = theWo.z < 0.f;

  if (toFlip)
    theWo.z = -theWo.z;
#endif

  float aCosDelta = dot (theWo, aM);

  // pick input based on half direction
  theWi = -theWo + 2.f * aCosDelta * aM;

  if (theWi.z <= 0.f || theWo.z <= 0.f)
  {
    return ZERO;
  }

  // Jacobian of half-direction mapping
  thePDF /= 4.f * aCosDelta;

  // compute shadow-masking coefficient
  float aG = SmithG1 (theWo, aM, theRoughness) *
             SmithG1 (theWi, aM, theRoughness);

#ifdef TWO_SIDED_BXDF
  if (toFlip)
    theWi.z = -theWi.z;
#endif

  return (aG * aCosDelta) / (theWo.z * aM.z) * fresnelMedia (aCosDelta, theFresnel);
}

//=======================================================================
// function : BsdfPdfLayered
// purpose  : Calculates BSDF of sampling input knowing output
//=======================================================================
float BsdfPdfLayered (in SBSDF theBSDF, in vec3 theWo, in vec3 theWi, in vec3 theWeight)
{
  float aPDF = 0.f; // PDF of sampling input direction

  // We choose whether the light is reflected or transmitted
  // by the coating layer according to the Fresnel equations
  vec3 aCoatF = fresnelMedia (theWo.z, theBSDF.FresnelCoat);

  // Coat BRDF is scaled by its Fresnel reflectance term. For
  // reasons of simplicity we scale base BxDFs only by coat's
  // Fresnel transmittance term
  vec3 aCoatT = UNIT - aCoatF;

  float aPc = dot (theBSDF.Kc.rgb * aCoatF, theWeight);
  float aPd = dot (theBSDF.Kd.rgb * aCoatT, theWeight);
  float aPs = dot (theBSDF.Ks.rgb * aCoatT, theWeight);
  float aPt = dot (theBSDF.Kt.rgb * aCoatT, theWeight);

  if (theWi.z * theWo.z > 0.f)
  {
    vec3 aH = normalize (theWi + theWo);

    aPDF = aPd * abs (theWi.z / M_PI);

    if (theBSDF.Kc.w > FLT_EPSILON)
    {
      float aPower = max (2.f / (theBSDF.Kc.w * theBSDF.Kc.w) - 2.f, 0.f); // roughness --> exponent

      aPDF += aPc * (aPower + 2.f) * (0.25f / M_2_PI) * pow (abs (aH.z), aPower + 1.f) / dot (theWi, aH);
    }

    if (theBSDF.Ks.w > FLT_EPSILON)
    {
      float aPower = max (2.f / (theBSDF.Ks.w * theBSDF.Ks.w) - 2.f, 0.f); // roughness --> exponent

      aPDF += aPs * (aPower + 2.f) * (0.25f / M_2_PI) * pow (abs (aH.z), aPower + 1.f) / dot (theWi, aH);
    }
  }

  return aPDF / (aPc + aPd + aPs + aPt);
}

//! Tool macro to handle sampling of particular BxDF
#define PICK_BXDF_LAYER(p, k) aPDF = p / aTotalR; theWeight *= k / aPDF;

//=======================================================================
// function : SampleBsdfLayered
// purpose  : Samples specified composite material (BSDF)
//=======================================================================
float SampleBsdfLayered (in SBSDF theBSDF, in vec3 theWo, out vec3 theWi, inout vec3 theWeight, inout bool theInside)
{
  // NOTE: OCCT uses two-layer material model. We have base diffuse, glossy, or transmissive
  // layer, covered by one glossy/specular coat. In the current model, the layers themselves
  // have no thickness; they can simply reflect light or transmits it to the layer under it.
  // We use actual BRDF model only for direct reflection by the coat layer. For transmission
  // through this layer, we approximate it as a flat specular surface.

  float aPDF = 0.f; // PDF of sampled direction

  // We choose whether the light is reflected or transmitted
  // by the coating layer according to the Fresnel equations
  vec3 aCoatF = fresnelMedia (theWo.z, theBSDF.FresnelCoat);

  // Coat BRDF is scaled by its Fresnel term. According to
  // Wilkie-Weidlich layered BSDF model, transmission term
  // for light passing through the coat at direction I and
  // leaving it in O is T = ( 1 - F (O) ) x ( 1 - F (I) ).
  // For reasons of simplicity, we discard the second term
  // and scale base BxDFs only by the first term.
  vec3 aCoatT = UNIT - aCoatF;

  float aPc = dot (theBSDF.Kc.rgb * aCoatF, theWeight);
  float aPd = dot (theBSDF.Kd.rgb * aCoatT, theWeight);
  float aPs = dot (theBSDF.Ks.rgb * aCoatT, theWeight);
  float aPt = dot (theBSDF.Kt.rgb * aCoatT, theWeight);

  // Calculate total reflection probability
  float aTotalR = (aPc + aPd) + (aPs + aPt);

  // Generate random variable to select BxDF
  float aKsi = aTotalR * RandFloat();

  if (aKsi < aPc) // REFLECTION FROM COAT
  {
    PICK_BXDF_LAYER (aPc, theBSDF.Kc.rgb)

    if (theBSDF.Kc.w < FLT_EPSILON)
    {
      theWeight *= aCoatF;

      theWi = vec3 (-theWo.x,
                    -theWo.y,
                     theWo.z);
    }
    else
    {
      theWeight *= SampleGlossyBlinnReflection (theWo, theWi, theBSDF.FresnelCoat, theBSDF.Kc.w, aPDF);
    }

    aPDF = mix (aPDF, MAXFLOAT, theBSDF.Kc.w < FLT_EPSILON);
  }
  else if (aKsi < aTotalR) // REFLECTION FROM BASE
  {
    theWeight *= aCoatT;

    if (aKsi < aPc + aPd) // diffuse BRDF
    {
      PICK_BXDF_LAYER (aPd, theBSDF.Kd.rgb)

      theWeight *= SampleLambertianReflection (theWo, theWi, aPDF);
    }
    else if (aKsi < (aPc + aPd) + aPs) // specular/glossy BRDF
    {
      PICK_BXDF_LAYER (aPs, theBSDF.Ks.rgb)

      if (theBSDF.Ks.w < FLT_EPSILON)
      {
        theWeight *= fresnelMedia (theWo.z, theBSDF.FresnelBase);

        theWi = vec3 (-theWo.x,
                      -theWo.y,
                       theWo.z);
      }
      else
      {
        theWeight *= SampleGlossyBlinnReflection (theWo, theWi, theBSDF.FresnelBase, theBSDF.Ks.w, aPDF);
      }

      aPDF = mix (aPDF, MAXFLOAT, theBSDF.Ks.w < FLT_EPSILON);
    }
    else // specular transmission
    {
      PICK_BXDF_LAYER (aPt, theBSDF.Kt.rgb)

      // refracted direction should exist if we are here
      transmitted (theBSDF.FresnelCoat.y, theWo, theWi);

      theInside = !theInside; aPDF = MAXFLOAT;
    }
  }

  // path termination for extra small weights
  theWeight = mix (ZERO, theWeight, step (FLT_EPSILON, aTotalR));

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

#define MATERIAL_KC(index)           (19 * index + 11)
#define MATERIAL_KD(index)           (19 * index + 12)
#define MATERIAL_KS(index)           (19 * index + 13)
#define MATERIAL_KT(index)           (19 * index + 14)
#define MATERIAL_LE(index)           (19 * index + 15)
#define MATERIAL_FRESNEL_COAT(index) (19 * index + 16)
#define MATERIAL_FRESNEL_BASE(index) (19 * index + 17)
#define MATERIAL_ABSORPT_BASE(index) (19 * index + 18)

//! Enables experimental Russian roulette sampling path termination.
//! In most cases, it provides faster image convergence with minimal
//! bias, so it is enabled by default.
#define RUSSIAN_ROULETTE

//! Frame step to increase number of bounces. This mode is used
//! for interaction with the model, when path length is limited
//! for the first samples, and gradually increasing when camera
//! is stabilizing.
#ifdef ADAPTIVE_SAMPLING
  #define FRAME_STEP 4
#else
  #define FRAME_STEP 5
#endif

//=======================================================================
// function : IsNotZero
// purpose  : Checks whether BSDF reflects direct light
//=======================================================================
bool IsNotZero (in SBSDF theBSDF, in vec3 theThroughput)
{
  vec3 aGlossy = theBSDF.Kc.rgb * step (FLT_EPSILON, theBSDF.Kc.w) +
                 theBSDF.Ks.rgb * step (FLT_EPSILON, theBSDF.Ks.w);

  return convolve (theBSDF.Kd.rgb + aGlossy, theThroughput) > FLT_EPSILON;
}

//=======================================================================
// function : PathTrace
// purpose  : Calculates radiance along the given ray
//=======================================================================
vec4 PathTrace (in SRay theRay, in vec3 theInverse, in int theNbSamples)
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

    SBSDF aBSDF;

    // fetch BxDF weights
    aBSDF.Kc = texelFetch (uRaytraceMaterialTexture, MATERIAL_KC (aTriIndex.w));
    aBSDF.Kd = texelFetch (uRaytraceMaterialTexture, MATERIAL_KD (aTriIndex.w));
    aBSDF.Ks = texelFetch (uRaytraceMaterialTexture, MATERIAL_KS (aTriIndex.w));
    aBSDF.Kt = texelFetch (uRaytraceMaterialTexture, MATERIAL_KT (aTriIndex.w)).rgb;

    // compute smooth normal (in parallel with fetch)
    vec3 aNormal = SmoothNormal (aHit.UV, aTriIndex);

    aNormal = normalize (vec3 (dot (aInvTransf0, aNormal),
                               dot (aInvTransf1, aNormal),
                               dot (aInvTransf2, aNormal)));

    SLocalSpace aSpace = buildLocalSpace (aNormal);

#ifdef USE_TEXTURES
    if (aBSDF.Kd.w >= 0.f)
    {
      vec4 aTexCoord = vec4 (SmoothUV (aHit.UV, aTriIndex), 0.f, 1.f);

      vec4 aTrsfRow1 = texelFetch (
        uRaytraceMaterialTexture, MATERIAL_TRS1 (aTriIndex.w));
      vec4 aTrsfRow2 = texelFetch (
        uRaytraceMaterialTexture, MATERIAL_TRS2 (aTriIndex.w));

      aTexCoord.st = vec2 (dot (aTrsfRow1, aTexCoord),
                           dot (aTrsfRow2, aTexCoord));

      vec4 aTexColor = textureLod (
        sampler2D (uTextureSamplers[int (aBSDF.Kd.w)]), aTexCoord.st, 0.f);

      aBSDF.Kd.rgb *= (aTexColor.rgb * aTexColor.rgb) * aTexColor.w; // de-gamma correction (for gamma = 2)

      if (aTexColor.w != 1.0f)
      {
        // mix transparency BTDF with texture alpha-channel
        aBSDF.Kt = (UNIT - aTexColor.www) + aTexColor.w * aBSDF.Kt;
      }
    }
#endif

    // fetch Fresnel reflectance for both layers
    aBSDF.FresnelCoat = texelFetch (uRaytraceMaterialTexture, MATERIAL_FRESNEL_COAT (aTriIndex.w)).xyz;
    aBSDF.FresnelBase = texelFetch (uRaytraceMaterialTexture, MATERIAL_FRESNEL_BASE (aTriIndex.w)).xyz;

    if (uLightCount > 0 && IsNotZero (aBSDF, aThroughput))
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

      aImpPDF = BsdfPdfLayered (aBSDF,
        toLocalSpace (-theRay.Direct, aSpace), toLocalSpace (aLight.xyz, aSpace), aThroughput);

      // MIS weight including division by explicit PDF
      float aMIS = (aExpPDF == MAXFLOAT) ? 1.f : aExpPDF / (aExpPDF * aExpPDF + aImpPDF * aImpPDF);

      vec3 aContrib = aMIS * aParam.rgb /* Le */ * EvalBsdfLayered (
          aBSDF, toLocalSpace (aLight.xyz, aSpace), toLocalSpace (-theRay.Direct, aSpace));

      if (any (greaterThan (aContrib, MIN_CONTRIBUTION))) // check if light source is important
      {
        SRay aShadow = SRay (theRay.Origin + aLight.xyz * uSceneEpsilon, aLight.xyz);

        aShadow.Origin += aHit.Normal * mix (
          -uSceneEpsilon, uSceneEpsilon, step (0.f, dot (aHit.Normal, aLight.xyz)));

        float aVisibility = SceneAnyHit (aShadow,
          InverseDirection (aLight.xyz), aLight.w == 0.f ? MAXFLOAT : aDistance);

        aRadiance += aVisibility * (aThroughput * aContrib);
      }
    }

    // account for self-emission
    aRadiance += aThroughput * texelFetch (uRaytraceMaterialTexture, MATERIAL_LE (aTriIndex.w)).rgb;

    if (aInMedium) // handle attenuation
    {
      vec4 aScattering = texelFetch (uRaytraceMaterialTexture, MATERIAL_ABSORPT_BASE (aTriIndex.w));

      aThroughput *= exp (-aHit.Time * aScattering.w * (UNIT - aScattering.rgb));
    }

    vec3 anInput = UNIT; // sampled input direction

    aImpPDF = SampleBsdfLayered (aBSDF,
      toLocalSpace (-theRay.Direct, aSpace), anInput, aThroughput, aInMedium);

    float aSurvive = float (any (greaterThan (aThroughput, MIN_THROUGHPUT)));

#ifdef RUSSIAN_ROULETTE
    aSurvive = aDepth < 3 ? aSurvive : min (dot (LUMA, aThroughput), 0.95f);
#endif

    // here, we additionally increase path length for non-diffuse bounces
    if (RandFloat() > aSurvive || all (lessThan (aThroughput, MIN_THROUGHPUT)) || aDepth >= theNbSamples / FRAME_STEP + step (1.f / M_PI, aImpPDF))
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
