//! Input ray-traced image.
uniform sampler2D uFSAAInputTexture;

//! Number of accumulated FSAA samples.
uniform int uSamples;

//! Output pixel color.
out vec4 OutColor;

#define LUM_DIFFERENCE 0.085f

// =======================================================================
// function : main
// purpose  :
// =======================================================================
void main (void)
{
#ifndef PATH_TRACING

  int aPixelX = int (gl_FragCoord.x);
  int aPixelY = int (gl_FragCoord.y);

  // Adjust FLIPTRI pattern used for adaptive FSAA
  float anOffsetX = mix (uOffsetX, -uOffsetX, float (aPixelX % 2));
  float anOffsetY = mix (uOffsetY, -uOffsetY, float (aPixelY % 2));

  vec4 aClr0 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX + 0, aPixelY + 0), 0);
  vec4 aClr1 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX + 0, aPixelY - 1), 0);
  vec4 aClr2 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX + 0, aPixelY + 1), 0);

  vec4 aClr3 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX + 1, aPixelY + 0), 0);
  vec4 aClr4 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX + 1, aPixelY - 1), 0);
  vec4 aClr5 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX + 1, aPixelY + 1), 0);

  vec4 aClr6 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX - 1, aPixelY + 0), 0);
  vec4 aClr7 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX - 1, aPixelY - 1), 0);
  vec4 aClr8 = texelFetch (uFSAAInputTexture, ivec2 (aPixelX - 1, aPixelY + 1), 0);

  float aLum = dot (LUMA, aClr0.xyz);

  bool aRender = abs (aClr1.w - aClr0.w) > LUM_DIFFERENCE ||
                 abs (aClr2.w - aClr0.w) > LUM_DIFFERENCE ||
                 abs (aClr3.w - aClr0.w) > LUM_DIFFERENCE ||
                 abs (aClr4.w - aClr0.w) > LUM_DIFFERENCE ||
                 abs (aClr5.w - aClr0.w) > LUM_DIFFERENCE ||
                 abs (aClr6.w - aClr0.w) > LUM_DIFFERENCE ||
                 abs (aClr7.w - aClr0.w) > LUM_DIFFERENCE ||
                 abs (aClr8.w - aClr0.w) > LUM_DIFFERENCE;

  if (!aRender)
  {
    aRender = abs (dot (LUMA, aClr1.xyz) - aLum) > LUM_DIFFERENCE ||
              abs (dot (LUMA, aClr2.xyz) - aLum) > LUM_DIFFERENCE ||
              abs (dot (LUMA, aClr3.xyz) - aLum) > LUM_DIFFERENCE ||
              abs (dot (LUMA, aClr4.xyz) - aLum) > LUM_DIFFERENCE ||
              abs (dot (LUMA, aClr5.xyz) - aLum) > LUM_DIFFERENCE ||
              abs (dot (LUMA, aClr6.xyz) - aLum) > LUM_DIFFERENCE ||
              abs (dot (LUMA, aClr7.xyz) - aLum) > LUM_DIFFERENCE ||
              abs (dot (LUMA, aClr8.xyz) - aLum) > LUM_DIFFERENCE;
  }

  vec4 aColor = aClr0;

  if (aRender)
  {
    SRay aRay = GenerateRay (vPixel + vec2 (anOffsetX, anOffsetY));

    vec3 aInvDirect = 1.f / max (abs (aRay.Direct), SMALL);

    aInvDirect = vec3 (aRay.Direct.x < 0.f ? -aInvDirect.x : aInvDirect.x,
                       aRay.Direct.y < 0.f ? -aInvDirect.y : aInvDirect.y,
                       aRay.Direct.z < 0.f ? -aInvDirect.z : aInvDirect.z);

    aColor = mix (aClr0, clamp (Radiance (aRay, aInvDirect), 0.f, 1.f), 1.f / uSamples);
  }

  OutColor = aColor;

#endif
}
