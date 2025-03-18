//! Function computes contribution of directional light source
//! into global variable DirectLighting (PBR shading).
//! @param theId      light source index
//! @param theNormal  surface normal
//! @param theView    view direction
//! @param theIsFront front/back face flag
//! @param theShadow  shadow attenuation
void occDirectionalLight (in int  theId,
                          in vec3 theNormal,
                          in vec3 theView,
                          in bool theIsFront,
                          in float theShadow)
{
  vec3 aLight = occLight_Position (theId);
  theNormal = theIsFront ? theNormal : -theNormal;
  DirectLighting += occPBRIllumination (theView, aLight, theNormal,
                                        BaseColor, Metallic, Roughness, IOR,
                                        occLight_Specular (theId),
                                        occLight_Intensity(theId)) * theShadow;
}
