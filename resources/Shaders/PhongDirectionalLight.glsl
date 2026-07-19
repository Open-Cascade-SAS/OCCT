//! Function computes contribution of directional light source
//! into global variables Diffuse and Specular (Phong shading).
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
  vec3 aHalf = normalize (aLight + theView);

  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;
  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));
  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));

  float aSpecl = 0.0;
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, occMaterial_Shininess (theIsFront));
  }

  Diffuse  += occLight_Diffuse  (theId) * aNdotL * theShadow;
  Specular += occLight_Specular (theId) * aSpecl * theShadow;
}
