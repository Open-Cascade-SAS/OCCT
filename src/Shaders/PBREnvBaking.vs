THE_SHADER_OUT vec3 ViewDirection; //!< direction of fetching from environment cubemap

uniform int uCurrentSide; //!< current side of cubemap
uniform int uYCoeff;      //!< coefficient of Y controlling horizontal flip of cubemap
uniform int uZCoeff;      //!< coefficient of Z controlling vertical flip of cubemap

const mat2 cubemapDirectionMatrices[6] = mat2[]
(
  mat2 ( 0, -1, -1,  0),
  mat2 ( 0,  1, -1,  0),
  mat2 ( 0,  1,  1,  0),
  mat2 ( 0,  1, -1,  0),
  mat2 ( 1,  0,  0, -1),
  mat2 (-1,  0,  0, -1)
);

//! Generates environment map fetching direction considering current index of side.
vec3 cubemapBakingViewDirection (in int theSide,
                                 in vec2 theScreenCoord)
{
  int anAxis = theSide / 2;
  vec3 aDirection = vec3(0.0);
  aDirection[anAxis] = float(-(int(theSide) % 2) * 2 + 1);
  theScreenCoord = cubemapDirectionMatrices[theSide] * theScreenCoord;
  aDirection[(anAxis + 1) % 3] = theScreenCoord.x;
  aDirection[(anAxis + 2) % 3] = theScreenCoord.y;
  return aDirection;
}

void main()
{
  ViewDirection = cubemapBakingViewDirection (uCurrentSide, occVertex.xy);
  ViewDirection = cubemapVectorTransform (ViewDirection, uYCoeff, uZCoeff);
  gl_Position = vec4 (occVertex.xy, 0.0, 1.0);
}
