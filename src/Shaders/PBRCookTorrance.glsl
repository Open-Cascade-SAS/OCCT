//! Calculates Cook-Torrance BRDF.
vec3 occPBRCookTorrance (in vec3  theView,
                         in vec3  theLight,
                         in vec3  theNormal,
                         in vec3  theBaseColor,
                         in float theMetallic,
                         in float theRoughness,
                         in float theIOR)
{
  vec3 aHalf = normalize (theView + theLight);
  float aCosV = max(dot(theView, theNormal), 0.0);
  float aCosL = max(dot(theLight, theNormal), 0.0);
  float aCosH = max(dot(aHalf, theNormal), 0.0);
  float aCosVH = max(dot(aHalf, theView), 0.0);
  vec3 aCookTorrance = occPBRDistribution (aCosH, theRoughness)
                     * occPBRGeometry     (aCosV, aCosL, theRoughness)
                     * occPBRFresnel      (theBaseColor, theMetallic, theIOR, aCosVH);
  aCookTorrance /= 4.0;
  return aCookTorrance;
}
