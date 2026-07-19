//! Calculates direct illumination using Cook-Torrance BRDF.
vec3 occPBRIllumination (in vec3  theView,
                         in vec3  theLight,
                         in vec3  theNormal,
                         in vec4  theBaseColor,
                         in float theMetallic,
                         in float theRoughness,
                         in float theIOR,
                         in vec3  theLightColor,
                         in float theLightIntensity)
{
  vec3 aHalf = normalize (theView + theLight);
  float aCosVH = max(dot(theView, aHalf), 0.0);
  vec3 aFresnel = occPBRFresnel (theBaseColor.rgb, theMetallic, theIOR, aCosVH);
  vec3 aSpecular = occPBRCookTorrance (theView,
                                       theLight,
                                       theNormal,
                                       theBaseColor.rgb,
                                       theMetallic,
                                       theRoughness,
                                       theIOR);
  vec3 aDiffuse = vec3(1.0) - aFresnel;
  aDiffuse *= 1.0 - theMetallic;
  aDiffuse *= INV_PI;
  aDiffuse *= theBaseColor.rgb;
  aDiffuse = mix (vec3(0.0), aDiffuse, theBaseColor.a);
  return (aDiffuse + aSpecular) * theLightColor * theLightIntensity * max(0.0, dot(theLight, theNormal));
}
