THE_SHADER_OUT vec3 ViewDirection; //!< direction of fetching from environment cubemap

uniform int uCurrentSide; //!< current side of cubemap
uniform int uYCoeff;      //!< coefficient of Y controlling horizontal flip of cubemap
uniform int uZCoeff;      //!< coefficient of Z controlling vertical flip of cubemap

void main()
{
  vec3 aDir;
  vec2 aCoord;
  if (uCurrentSide == 0)
  {
    aCoord = mat2( 0,-1,-1, 0) * occVertex.xy;
    aDir.x = 1.0;
    aDir.y = aCoord.x;
    aDir.z = aCoord.y;
  }
  else if (uCurrentSide == 1)
  {
    aCoord = mat2( 0, 1,-1, 0) * occVertex.xy;
    aDir.x = -1.0;
    aDir.y = aCoord.x;
    aDir.z = aCoord.y;
  }
  else if (uCurrentSide == 2)
  {
    aCoord = mat2( 0, 1, 1, 0) * occVertex.xy;
    aDir.x = aCoord.y;
    aDir.y = 1.0;
    aDir.z = aCoord.x;
  }
  else if (uCurrentSide == 3)
  {
    aCoord = mat2( 0, 1,-1, 0) * occVertex.xy;
    aDir.x = aCoord.y;
    aDir.y = -1.0;
    aDir.z = aCoord.x;
  }
  else if (uCurrentSide == 4)
  {
    aCoord = mat2( 1, 0, 0,-1) * occVertex.xy;
    aDir.x = aCoord.x;
    aDir.y = aCoord.y;
    aDir.z = 1.0;
  }
  else //if (uCurrentSide == 5)
  {
    aCoord = mat2(-1, 0, 0,-1) * occVertex.xy;
    aDir.x = aCoord.x;
    aDir.y = aCoord.y;
    aDir.z = -1.0;
  }
  ViewDirection = cubemapVectorTransform (aDir, uYCoeff, uZCoeff);
  gl_Position = vec4 (occVertex.xy, 0.0, 1.0);
}
