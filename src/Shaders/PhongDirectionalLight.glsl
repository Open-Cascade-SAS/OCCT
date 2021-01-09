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
  vec3 aLight = vec3 (occWorldViewMatrix * vec4 (occLight_Position (theId), 0.0));
  vec3 aHalf = normalize (aLight + theView);

  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;
  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));
  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));

  float aSpecl = 0.0;
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());
  }

  Diffuse  += occLight_Diffuse  (theId) * aNdotL * theShadow;
  Specular += occLight_Specular (theId) * aSpecl * theShadow;
}
