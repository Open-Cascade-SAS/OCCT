//! Function computes contribution of isotropic point light source
//! into global variable DirectLighting (PBR shading).
//! @param theId      light source index
//! @param theNormal  surface normal
//! @param theView    view direction
//! @param thePoint   3D position (world space)
//! @param theIsFront front/back face flag
void occPointLight (in int  theId,
                    in vec3 theNormal,
                    in vec3 theView,
                    in vec3 thePoint,
                    in bool theIsFront)
{
  vec3 aLight = occLight_Position (theId) - thePoint;

  float aDist = length (aLight);
  float aRange = occLight_Range (theId);
  float anAtten = occPointLightAttenuation (aDist, aRange);
  if (anAtten <= 0.0) return;
  aLight /= aDist;

  theNormal = theIsFront ? theNormal : -theNormal;
  DirectLighting += occPBRIllumination (theView, aLight, theNormal,
                                        BaseColor, Metallic, Roughness, IOR,
                                        occLight_Specular (theId),
                                        occLight_Intensity(theId) * anAtten);
}
