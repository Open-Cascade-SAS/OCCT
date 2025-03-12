//! Functions to calculate fresnel coefficient and approximate zero fresnel value.
vec3 occPBRFresnel (in vec3  theBaseColor,
                    in float theMetallic,
                    in float theIOR)
{
  theIOR = (1.0 - theIOR) / (1.0 + theIOR);
  theIOR *= theIOR;
  vec3 f0 = vec3(theIOR);
  f0 = mix (f0, theBaseColor.rgb, theMetallic);
  return f0;
}

vec3 occPBRFresnel (in vec3  theBaseColor,
                    in float theMetallic,
                    in float theIOR,
                    in float theCosVH)
{
  vec3 f0 = occPBRFresnel (theBaseColor, theMetallic, theIOR);
  theCosVH = 1.0 - theCosVH;
  theCosVH *= theCosVH;
  theCosVH *= theCosVH * theCosVH * theCosVH * theCosVH;
  return f0 + (vec3 (1.0) - f0) * theCosVH;
}

vec3 occPBRFresnel (in vec3  theBaseColor,
                    in float theMetallic,
                    in float theRoughness,
                    in float theIOR,
                    in float theCosV)
{
  vec3 f0 = occPBRFresnel (theBaseColor, theMetallic, theIOR);
  theCosV = 1.0 - theCosV;
  theCosV *= theCosV;
  theCosV *= theCosV * theCosV * theCosV * theCosV;
  return f0 + (max(vec3(1.0 - theRoughness), f0) - f0) * theCosV;
}
