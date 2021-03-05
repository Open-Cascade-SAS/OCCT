//! Function computes contribution of isotropic point light source
//! into global variables Diffuse and Specular (Phong shading).
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
  float anAtten = occPointLightAttenuation (aDist, aRange, occLight_LinearAttenuation (theId), occLight_ConstAttenuation (theId));
  if (anAtten <= 0.0) return;
  aLight /= aDist;

  vec3 aHalf = normalize (aLight + theView);

  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;
  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));
  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));

  float aSpecl = 0.0;
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, occMaterial_Shininess (theIsFront));
  }

  Diffuse  += occLight_Diffuse (theId) * aNdotL * anAtten;
  Specular += occLight_Specular(theId) * aSpecl * anAtten;
}
