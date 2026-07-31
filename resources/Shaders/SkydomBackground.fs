// Constants
const float M_PI = 3.1415926535;
const float THE_EARTH_RADIUS = 6360e3;
const vec3  THE_EARTH_CENTER = vec3 (0.0, -THE_EARTH_RADIUS, 0.0);
const float THE_ATMO_RADIUS  = 6380e3;  // atmosphere radius (6420e3?)
const float THE_G            = 0.76;    // anisotropy of the medium (papers use 0.76)
const float THE_G2           = THE_G * THE_G;
const float THE_HR           = 8000.0;  // Thickness of the atmosphere
const float THE_HM           = 1000.0;  // Same as above but for Mie
const vec3  THE_BETA_R       = vec3 (5.8e-6, 13.5e-6, 33.1e-6); // Reyleigh scattering normal earth
const vec3  THE_BETA_M       = vec3 (21e-6); // Normal Mie scattering

// Parameters
const float THE_SunAttenuation = 1.0;   // sun intensity
const float THE_EyeHeight      = 100.0; // viewer height
const float THE_HorizonWidth   = 0.002;
const int   THE_NbSamples      = 8;
const int   THE_NbSamplesLight = 8;     // integral sampling rate (might highly hit performance)
const float THE_SunPower       = 5.0;
const float THE_StarTreshold   = 0.98;

// Uniforms
uniform vec3  uSunDir;
uniform float uTime;
uniform float uCloudy;
uniform float uFog;

float hash13 (in vec3 p3)
{
  p3  = fract (p3 * 0.1031);
  p3 += dot (p3, p3.zyx + 31.32);
  return fract ((p3.x + p3.y) * p3.z);
}

float hash12 (in vec2 p)
{
  vec3 p3  = fract (vec3(p.xyx) * .1031);
  p3 += dot (p3, p3.yzx + 33.33);
  return fract ((p3.x + p3.y) * p3.z);
}

float smoothStarField (in vec2 theSamplePos)
{
  vec2 aFract = fract (theSamplePos);
  vec2 aFloorSample = floor (theSamplePos);
  float v1 = hash12 (aFloorSample);
  float v2 = hash12 (aFloorSample + vec2( 0.0, 1.0 ));
  float v3 = hash12 (aFloorSample + vec2( 1.0, 0.0 ));
  float v4 = hash12 (aFloorSample + vec2( 1.0, 1.0 ));

  vec2 u = aFract * aFract * (3.0 - 2.0 * aFract);

  return mix(v1, v2, u.x) +
            (v3 - v1) * u.y * (1.0 - u.x) +
            (v4 - v2) * u.x * u.y;
}

float noisyStarField (in vec2 theSamplePos)
{
  float aStarVal = smoothStarField (theSamplePos);
  if (aStarVal >= THE_StarTreshold)
  {
    aStarVal = pow ((aStarVal - THE_StarTreshold) / (1.0 - THE_StarTreshold), 6.0);
  }
  else
  {
    aStarVal = 0.0;
  }
  return aStarVal;
}

float smoothNoise (in vec3 theCoord)
{
  vec3 anInt   = floor (theCoord);
  vec3 anFract = fract (theCoord);
  anFract = anFract * anFract * (3.0 - (2.0 * anFract));
  return mix(mix(mix(hash13(anInt                      ),
                     hash13(anInt + vec3(1.0, 0.0, 0.0)), anFract.x),
                 mix(hash13(anInt + vec3(0.0, 1.0, 0.0)),
                     hash13(anInt + vec3(1.0, 1.0, 0.0)), anFract.x), anFract.y),
             mix(mix(hash13(anInt + vec3(0.0, 0.0, 1.0)),
                     hash13(anInt + vec3(1.0, 0.0, 1.0)), anFract.x),
                 mix(hash13(anInt + vec3(0.0, 1.0, 1.0)),
                     hash13(anInt + vec3(1.0, 1.0, 1.0)), anFract.x), anFract.y), anFract.z);
}

float fnoise (in vec3 theCoord, in float theTime)
{
  theCoord *= .25;
  float aNoise;

  aNoise =  0.5000 * smoothNoise (theCoord);
  theCoord = theCoord * 3.02; theCoord.y -= theTime * 0.2;
  aNoise += 0.2500 * smoothNoise (theCoord);
  theCoord = theCoord * 3.03; theCoord.y += theTime * 0.06;
  aNoise += 0.1250 * smoothNoise (theCoord);
  theCoord = theCoord * 3.01;
  aNoise += 0.0625 * smoothNoise (theCoord);
  theCoord = theCoord * 3.03;
  aNoise += 0.03125 * smoothNoise (theCoord);
  theCoord = theCoord * 3.02;
  aNoise += 0.015625 * smoothNoise (theCoord);
  return aNoise;
}

float clouds (in vec3 theTs, in float theTime)
{
  float aCloud = fnoise (theTs * 2e-4, theTime) + uCloudy * 0.1;
  aCloud = smoothstep (0.44, 0.64, aCloud);
  aCloud *= 70.0;
  return aCloud + uFog;
}

void densities (in vec3 thePos, out float theRayleigh, out float theMie, in float theTime)
{
  float aHeight = length (thePos - THE_EARTH_CENTER) - THE_EARTH_RADIUS;
  theRayleigh = exp (-aHeight / THE_HR);

  float aCloud = 0.0;
  if (aHeight > 5000.0 && aHeight < 8000.0)
  {
    aCloud  = clouds (thePos + vec3 (0.0, 0.,-theTime*3e3), theTime);
    aCloud *= sin (M_PI*(aHeight - 5e3) / 5e3) * uCloudy;
  }

  float aCloud2 = 0.0;
  if (aHeight > 12e3 && aHeight < 15.5e3)
  {
    aCloud2 = fnoise (thePos * 3e-4, theTime) * clouds (thePos * 32.0, theTime);
    aCloud2 *= sin (M_PI * (aHeight - 12e3) / 12e3) * 0.05;
    aCloud2 = clamp (aCloud2, 0.0, 1.0);
  }

  theMie = exp (-aHeight / THE_HM) + aCloud + uFog;
  theMie += aCloud2;
}

// ray with sphere intersection problem is reduced to solving the equation
// (P - C)^2 = r^2             <--- sphere equation
// where P is P(t) = A + t*B   <--- point on ray
// t^2*dot(B, B) + t*2*dot(B, A-C) + dot(A-C, A-C) - r^2 = 0
//     [   A   ]     [     B     ]   [        C        ]
// We just need to solve the above quadratic equation
float raySphereIntersect (in vec3 theOrig, in vec3 theDir, in float theRadius)
{
  theOrig = theOrig - THE_EARTH_CENTER;
  // A coefficient will be always 1 (theDir is normalized)
  float B = dot (theOrig, theDir);
  float C = dot (theOrig, theOrig) - theRadius * theRadius;
  // optimized version of classic (-b +- sqrt(b^2 - 4ac)) / 2a
  float aDet2 = B * B - C;
  if (aDet2 < 0.0) { return -1.0; }
  float aDet = sqrt (aDet2);
  float aT1 = -B - aDet;
  float aT2 = -B + aDet;
  return aT1 >= 0.0 ? aT1 : aT2;
}

void scatter (in vec3 theEye, in vec3 theRay, in vec3 theSun,
              out vec3 theCol, out float theScat, in float theTime)
{
  float aRayLen = raySphereIntersect (theEye, theRay, THE_ATMO_RADIUS);
  float aMu     = dot (theRay, theSun);
  float aMu2    = 1.0 + aMu*aMu;
  // The Raleigh phase function looks like this:
  float aPhaseR = 3.0/(16.0 * M_PI) * aMu2;
  // And the Mie phase function equation is:
  float aPhaseM = (3.0 / (8.0 * M_PI) * (1.0 - THE_G2) * aMu2)
                / ((2.0 + THE_G2) * pow (1.0 + THE_G2 - 2.0 * THE_G * aMu, 1.5));

  float anOpticalDepthR = 0.0;
  float anOpticalDepthM = 0.0;
  vec3 aSumR = vec3 (0.0);
  vec3 aSumM = vec3 (0.0); // Mie and Rayleigh contribution

  float dl = aRayLen / float (THE_NbSamples);
  for (int i = 0; i < THE_NbSamples; ++i)
  {
    float l = float(i) * dl;
    vec3 aSamplePos = theEye + theRay * l;

    float dR, dM;
    densities (aSamplePos, dR, dM, theTime);
    dR *= dl;
    dM *= dl;
    anOpticalDepthR += dR;
    anOpticalDepthM += dM;

    float aSegmentLengthLight = raySphereIntersect (aSamplePos, theSun, THE_ATMO_RADIUS);
    if (aSegmentLengthLight > 0.0)
    {
      float dls = aSegmentLengthLight / float (THE_NbSamplesLight);
      float anOpticalDepthRs = 0.0;
      float anOpticalDepthMs = 0.0;
      for (int j = 0; j < THE_NbSamplesLight; ++j)
      {
        float ls = float (j) * dls;
        vec3 aSamplePosS = aSamplePos + theSun * ls;
        float dRs, dMs;
        densities (aSamplePosS, dRs, dMs, theTime);
        anOpticalDepthRs += dRs * dls;
        anOpticalDepthMs += dMs * dls;
      }

      vec3 anAttenuation = exp (-(THE_BETA_R * (anOpticalDepthR + anOpticalDepthRs)
                                + THE_BETA_M * (anOpticalDepthM + anOpticalDepthMs)));
      aSumR += anAttenuation * dR;
      aSumM += anAttenuation * dM;
    }
  }

  theCol = THE_SunPower * (aSumR * THE_BETA_R * aPhaseR + aSumM * THE_BETA_M * aPhaseM);
  theScat = 1.0 - clamp (anOpticalDepthM*1e-5, 0.0, 1.0);
}

// This is where all the magic happens. We first raymarch along the primary ray
// (from the camera origin to the point where the ray exits the atmosphere).
// For each sample along the primary ray,
// we then "cast" a light ray and raymarch along that ray as well.
// We basically shoot a ray in the direction of the sun.
vec4 computeIncidentLight (in vec3 theRayDirection, in vec2 theUv, in float theTime)
{
  float aSunAttenuation = THE_SunAttenuation;
  vec3 aSunDir = uSunDir;
  // conversion to moon
  float aStarAttenuation = 0.0;
  if (aSunDir.y < 0.0)
  {
    aSunDir *= -1.0;
    aSunAttenuation = aSunAttenuation * 0.1;
    aStarAttenuation = sqrt (aSunDir.y);
  }

  vec3 anEyePosition = vec3(0.0, THE_EyeHeight, 0.0);

  // draw a water surface horizontally symmetrically to the sky
  if (theRayDirection.y <= -THE_HorizonWidth / 2.0)
  {
    theRayDirection.y = -THE_HorizonWidth - theRayDirection.y;
  }

  float aScattering = 0.0;
  vec3  aColor = vec3 (0.0);

  scatter (anEyePosition, theRayDirection, aSunDir, aColor, aScattering, theTime);
  aColor *= aSunAttenuation;
  float aStarIntensity = noisyStarField (theUv * 2048.0);
  vec3 aStarColor = vec3 (aScattering * aStarIntensity * aStarAttenuation);
  aColor += aStarColor;

  return vec4 (1.18 * pow (aColor, vec3(0.7)), 1.0);
}

uniform int uSide;

void main()
{
  vec2 anUv = vec2 (2.0 * TexCoord.x - 1.0,
                    2.0 * TexCoord.y - 1.0);
  vec3 aPlanes[6];
  aPlanes[0] = vec3 (+1.0, 0.0, 0.0);
  aPlanes[1] = vec3 (-1.0, 0.0, 0.0);
  aPlanes[2] = vec3 ( 0.0,+1.0, 0.0);
  aPlanes[3] = vec3 ( 0.0,-1.0, 0.0);
  aPlanes[4] = vec3 ( 0.0, 0.0,+1.0);
  aPlanes[5] = vec3 ( 0.0, 0.0,-1.0);
  vec3 aRayDirection;
  if (uSide == 0)
  {
    // Positive X side
    aRayDirection = aPlanes[0] + vec3 (0.0, +anUv.y, -anUv.x);
  }
  else if (uSide == 1)
  {
    // Negative X side
    aRayDirection = aPlanes[1] + vec3 (0.0, +anUv.y, +anUv.x);
  }
  else if (uSide == 2)
  {
    // Positive Y side
    aRayDirection = aPlanes[2] + vec3 (+anUv.x, 0.0, +anUv.y);
  }
  else if (uSide == 3)
  {
    // Negative Y side
    aRayDirection = aPlanes[3] + vec3 (+anUv.x, 0.0, -anUv.y);
  }
  else if (uSide == 4)
  {
    // Positive Z side
    aRayDirection = aPlanes[4] + vec3 (+anUv.x, +anUv.y, 0.0);
  }
  else if (uSide == 5)
  {
    // Negative Z side
    aRayDirection = aPlanes[5] + vec3 (-anUv.x, +anUv.y, 0.0);
  }

  occFragColor = computeIncidentLight (normalize (aRayDirection), anUv, uTime);
}
