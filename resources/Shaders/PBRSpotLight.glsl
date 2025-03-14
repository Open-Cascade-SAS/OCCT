//! Function computes contribution of spotlight source
//! into global variable DirectLighting (PBR shading).
//! @param theId      light source index
//! @param theNormal  surface normal
//! @param theView    view direction
//! @param thePoint   3D position (world space)
//! @param theIsFront front/back face flag
void occSpotLight (in int  theId,
                   in vec3 theNormal,
                   in vec3 theView,
                   in vec3 thePoint,
                   in bool theIsFront,
                   in float theShadow)
{
  vec3 aLight = occLight_Position (theId) - thePoint;

  float aDist = length (aLight);
  float aRange = occLight_Range (theId);
  float anAtten = occPointLightAttenuation (aDist, aRange);
  if (anAtten <= 0.0) return;
  aLight /= aDist;

  vec3 aSpotDir = occLight_SpotDirection (theId);
  // light cone
  float aCosA = dot (aSpotDir, -aLight);
  float aRelativeAngle = 2.0 * acos(aCosA) / occLight_SpotCutOff(theId);
  if (aCosA >= 1.0 || aRelativeAngle > 1.0)
  {
    return;
  }
  float anExponent = occLight_SpotExponent (theId);
  if ((1.0 - aRelativeAngle) <= anExponent)
  {
    float anAngularAttenuationOffset = cos(0.5 * occLight_SpotCutOff(theId));
    float anAngularAttenuationScale = 1.0 / max(0.001, cos(0.5 * occLight_SpotCutOff(theId) * (1.0 - anExponent)) - anAngularAttenuationOffset);
    anAngularAttenuationOffset *= -anAngularAttenuationScale;
    float anAngularAttenuantion = clamp(aCosA * anAngularAttenuationScale + anAngularAttenuationOffset, 0.0, 1.0);
    anAtten *= anAngularAttenuantion * anAngularAttenuantion;
  }
  theNormal = theIsFront ? theNormal : -theNormal;
  DirectLighting += occPBRIllumination (theView, aLight, theNormal,
                                        BaseColor, Metallic, Roughness, IOR,
                                        occLight_Specular(theId),
                                        occLight_Intensity(theId) * anAtten) * theShadow;
}
