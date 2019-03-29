out vec4 OutColor;

// Seed for random number generator (generated on CPU).
uniform int uFrameRndSeed;

//! Enables/disables using of single RNG seed for 16x16 image
//! blocks. Increases performance up to 4x, but the noise has
//! become structured. Can be used fo final rendering.
uniform int uBlockedRngEnabled;

//! Number of previously rendered frames (used in non-ISS mode).
uniform int uAccumSamples;

#ifndef ADAPTIVE_SAMPLING
  //! Input image with previously accumulated samples.
  uniform sampler2D uAccumTexture;
#endif

//! Maximum radiance that can be added to the pixel.
//! Decreases noise level, but introduces some bias.
uniform float uMaxRadiance = 50.f;

#ifdef ADAPTIVE_SAMPLING
//! Wrapper over imageLoad()+imageStore() having similar syntax as imageAtomicAdd().
//! Modifies one component of 3Wx2H uRenderImage:
//! |RGL| Red, Green, Luminance
//! |SBH| Samples, Blue, Hit time transformed into OpenGL NDC space
//! Returns previous value of the component.
float addRenderImageComp (in ivec2 theFrag, in ivec2 theComp, in float theVal)
{
  ivec2 aCoord = ivec2 (3 * theFrag.x + theComp.x,
                        2 * theFrag.y + theComp.y);
#ifdef ADAPTIVE_SAMPLING_ATOMIC
  return imageAtomicAdd (uRenderImage, aCoord, theVal);
#else
  float aVal = imageLoad (uRenderImage, aCoord).x;
  imageStore (uRenderImage, aCoord, vec4 (aVal + theVal));
  return aVal;
#endif
}
#endif

// =======================================================================
// function : main
// purpose  :
// =======================================================================
void main (void)
{
  SeedRand (uFrameRndSeed, uWinSizeX, uBlockedRngEnabled == 0 ? 1 : 16);

#ifndef PATH_TRACING

  SRay aRay = GenerateRay (vPixel);

#else

  ivec2 aFragCoord = ivec2 (gl_FragCoord.xy);

#ifdef ADAPTIVE_SAMPLING

#ifdef ADAPTIVE_SAMPLING_ATOMIC
  ivec2 aTileXY = imageLoad (uOffsetImage, aFragCoord / uTileSize).xy * uTileSize;
  if (aTileXY.x < 0) { discard; }

  ivec2 aRealBlockSize = ivec2 (min (uWinSizeX - aTileXY.x, uTileSize.x),
                                min (uWinSizeY - aTileXY.y, uTileSize.y));

  aFragCoord.x = aTileXY.x + (aFragCoord.x % aRealBlockSize.x);
  aFragCoord.y = aTileXY.y + (aFragCoord.y % aRealBlockSize.y);
#else
  int aNbTileSamples = imageAtomicAdd (uTilesImage, aFragCoord / uTileSize, int(-1));
  if (aNbTileSamples <= 0)
  {
    discard;
  }
#endif

#endif // ADAPTIVE_SAMPLING

  vec2 aPnt = vec2 (aFragCoord.x + RandFloat(),
                    aFragCoord.y + RandFloat());

  SRay aRay = GenerateRay (aPnt / vec2 (uWinSizeX, uWinSizeY));

#endif // PATH_TRACING

  vec3 aInvDirect = InverseDirection (aRay.Direct);

#ifdef PATH_TRACING

#ifndef ADAPTIVE_SAMPLING

  vec4 aColor = PathTrace (aRay, aInvDirect, uAccumSamples);

#else

  float aNbSamples = addRenderImageComp (aFragCoord, ivec2 (0, 1), 1.0);
  vec4 aColor = PathTrace (aRay, aInvDirect, int (aNbSamples));

#endif

  if (any (isnan (aColor.rgb)))
  {
    aColor.rgb = ZERO;
  }

  aColor.rgb = min (aColor.rgb, vec3 (uMaxRadiance));

#ifdef ADAPTIVE_SAMPLING

  // accumulate RGB color and depth
  addRenderImageComp (aFragCoord, ivec2 (0, 0), aColor.r);
  addRenderImageComp (aFragCoord, ivec2 (1, 0), aColor.g);
  addRenderImageComp (aFragCoord, ivec2 (1, 1), aColor.b);
  addRenderImageComp (aFragCoord, ivec2 (2, 1), aColor.w);

  if (int (aNbSamples) % 2 == 0) // accumulate luminance for even samples only
  {
    addRenderImageComp (aFragCoord, ivec2 (2, 0), dot (LUMA, aColor.rgb));
  }

#else

  if (uAccumSamples == 0)
  {
    OutColor = aColor;
  }
  else
  {
    OutColor = mix (texture (uAccumTexture, vPixel), aColor, 1.f / (uAccumSamples + 1));
  }

#endif // ADAPTIVE_SAMPLING

#else

  OutColor = clamp (Radiance (aRay, aInvDirect), 0.f, 1.f);

#endif // PATH_TRACING
}
