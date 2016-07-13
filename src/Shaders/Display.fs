#ifdef ADAPTIVE_SAMPLING

  #extension GL_ARB_shader_image_load_store : require

  //! OpenGL image used for accumulating rendering result.
  volatile restrict layout(size1x32) uniform image2D uRenderImage;

  //! OpenGL image storing variance of sampled pixels blocks.
  volatile restrict layout(size1x32) uniform iimage2D uVarianceImage;

#else // ADAPTIVE_SAMPLING

  //! Input image.
  uniform sampler2D uInputTexture;

  //! Ray tracing depth image.
  uniform sampler2D uDepthTexture;

#endif // ADAPTIVE_SAMPLING

//! Number of accumulated frames.
uniform int uAccumFrames;

//! Is debug mode enabled for importance screen sampling.
uniform int uDebugAdaptive;

//! Output pixel color.
out vec4 OutColor;

//! RGB weight factors to calculate luminance.
#define LUMA vec3 (0.2126f, 0.7152f, 0.0722f)

//! Scale factor used to quantize visual error.
#define SCALE_FACTOR 1.0e6f

// =======================================================================
// function : main
// purpose  :
// =======================================================================
void main (void)
{
#ifndef ADAPTIVE_SAMPLING

  vec4 aColor = texelFetch (uInputTexture, ivec2 (gl_FragCoord.xy), 0);

#ifdef PATH_TRACING
  float aDepth = aColor.w; // path tracing uses averaged depth
#else
  float aDepth = texelFetch (uDepthTexture, ivec2 (gl_FragCoord.xy), 0).r;
#endif

  gl_FragDepth = aDepth;

#else // ADAPTIVE_SAMPLING

  ivec2 aPixel = ivec2 (gl_FragCoord.xy);

  vec4 aColor = vec4 (0.0);

  // fetch accumulated color and total number of samples
  aColor.x = imageLoad (uRenderImage, ivec2 (3 * aPixel.x + 0,
                                             2 * aPixel.y + 0)).x;
  aColor.y = imageLoad (uRenderImage, ivec2 (3 * aPixel.x + 1,
                                             2 * aPixel.y + 0)).x;
  aColor.z = imageLoad (uRenderImage, ivec2 (3 * aPixel.x + 1,
                                             2 * aPixel.y + 1)).x;
  aColor.w = imageLoad (uRenderImage, ivec2 (3 * aPixel.x + 0,
                                             2 * aPixel.y + 1)).x;

  // calculate normalization factor
  float aSampleWeight = 1.f / max (1.0, aColor.w);

  // calculate averaged depth value
  gl_FragDepth = imageLoad (uRenderImage, ivec2 (3 * aPixel.x + 2,
                                                 2 * aPixel.y + 1)).x * aSampleWeight;

  // calculate averaged radiance for all samples and even samples only
  float aHalfRad = imageLoad (uRenderImage, ivec2 (3 * aPixel.x + 2,
                                                   2 * aPixel.y + 0)).x * aSampleWeight * 2.f;

  float aAverRad = dot (aColor.rgb, LUMA) * aSampleWeight;

  // apply our 'tone mapping' operator (gamma correction and clamping)
  aHalfRad = min (1.f, sqrt (aHalfRad));
  aAverRad = min (1.f, sqrt (aAverRad));

  // calculate visual error
  float anError = (aAverRad - aHalfRad) * (aAverRad - aHalfRad);

  // accumulate visual error to current block
  imageAtomicAdd (uVarianceImage, ivec2 (aPixel / vec2 (BLOCK_SIZE)), int (anError * SCALE_FACTOR));

  if (uDebugAdaptive == 0) // normal rendering
  {
    aColor = vec4 (aColor.rgb * aSampleWeight, 1.0);
  }
  else // showing number of samples
  {
    aColor = vec4 (0.5f * aColor.rgb * aSampleWeight + vec3 (0.f, aColor.w / uAccumFrames * 0.35f, 0.f), 1.0);
  }

#endif // ADAPTIVE_SAMPLING

#ifdef PATH_TRACING

   // apply gamma correction (we use gamma = 2)
   OutColor = vec4 (sqrt (aColor.rgb), 0.f);

#else // not PATH_TRACING

   OutColor = aColor;

#endif
}
