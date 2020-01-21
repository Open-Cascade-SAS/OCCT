//! Calculates transformation from tangent space and apply it to value from normal map to get normal in object space
vec3 TangentSpaceNormal (in mat2 theDeltaUVMatrix,
                         in mat2x3 theDeltaVectorMatrix,
                         in vec3 theNormalMapValue,
                         in vec3 theNormal,
                         in bool theIsInverse)
{
  theNormalMapValue = normalize(theNormalMapValue * 2.0 - vec3(1.0));
  // Inverse matrix
  theDeltaUVMatrix = mat2 (theDeltaUVMatrix[1][1], -theDeltaUVMatrix[0][1], -theDeltaUVMatrix[1][0], theDeltaUVMatrix[0][0]);
  theDeltaVectorMatrix = theDeltaVectorMatrix * theDeltaUVMatrix;
  // Gram-Schmidt orthogonalization
  theDeltaVectorMatrix[1] = normalize(theDeltaVectorMatrix[1] - dot(theNormal, theDeltaVectorMatrix[1]) * theNormal);
  theDeltaVectorMatrix[0] = cross(theDeltaVectorMatrix[1], theNormal);
  float aDirection = theIsInverse ? -1.0 : 1.0;
  return mat3 (aDirection * theDeltaVectorMatrix[0], aDirection * theDeltaVectorMatrix[1], theNormal) * theNormalMapValue;
}
