out vec4 OutColor;

// Seed for random number generator
uniform int uFrameRndSeed;

// Weight of current frame related to accumulated frames.
uniform float uSampleWeight;

//! Input accumulated image.
uniform sampler2D uAccumTexture;

// =======================================================================
// function : main
// purpose  :
// =======================================================================
void main (void)
{
#ifndef PATH_TRACING
  SRay aRay = GenerateRay (vPixel);
#else
  ivec2 aWinSize = textureSize (uAccumTexture, 0);

  SeedRand (uFrameRndSeed, aWinSize.x);

  SRay aRay = GenerateRay (vPixel +
    vec2 (RandFloat() + 1.f, RandFloat() + 1.f) / vec2 (aWinSize));
#endif

  vec3 aInvDirect = 1.f / max (abs (aRay.Direct), SMALL);

  aInvDirect = vec3 (aRay.Direct.x < 0.f ? -aInvDirect.x : aInvDirect.x,
                     aRay.Direct.y < 0.f ? -aInvDirect.y : aInvDirect.y,
                     aRay.Direct.z < 0.f ? -aInvDirect.z : aInvDirect.z);

#ifdef PATH_TRACING

  vec4 aColor = PathTrace (aRay, aInvDirect);

  if (any (isnan (aColor.xyz)))
  {
    aColor.xyz = ZERO;
  }

  OutColor = mix (texture2D (uAccumTexture, vPixel), aColor, uSampleWeight);

#else

  OutColor = clamp (Radiance (aRay, aInvDirect), 0.f, 1.f);

#endif
}