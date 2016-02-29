//! Input image.
uniform sampler2D uInputTexture;

//! Ray tracing depth image.
uniform sampler2D uDepthTexture;

//! Output pixel color.
out vec4 OutColor;

void main (void)
{
  vec4 aColor = texelFetch (uInputTexture, ivec2 (gl_FragCoord.xy), 0);

  float aDepth = texelFetch (uDepthTexture, ivec2 (gl_FragCoord.xy), 0).r;
  gl_FragDepth = aDepth;

  // apply gamma correction (we use gamma = 2)
  OutColor = vec4 (sqrt (aColor.rgb), aColor.a);
}
