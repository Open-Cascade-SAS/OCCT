//! Function computes contribution of spotlight source
//! into global variables Diffuse and Specular (Phong shading).
//! @param theId      light source index
//! @param theNormal  surface normal
//! @param theView    view direction
//! @param thePoint   3D position (world space)
//! @param theIsFront front/back face flag
//! @param theShadow  the value from shadow map
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
  float anAtten = occPointLightAttenuation (aDist, aRange, occLight_LinearAttenuation (theId), occLight_ConstAttenuation (theId));
  if (anAtten <= 0.0) return;
  aLight /= aDist;

  vec3 aSpotDir = occLight_SpotDirection (theId);
  // light cone
  float aCosA = dot (aSpotDir, -aLight);
  if (aCosA >= 1.0 || aCosA < cos (occLight_SpotCutOff (theId)))
  {
    return;
  }

  float anExponent = occLight_SpotExponent (theId);
  if (anExponent > 0.0)
  {
    anAtten *= pow (aCosA, anExponent * 128.0);
  }

  vec3 aHalf = normalize (aLight + theView);

  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;
  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));
  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));

  float aSpecl = 0.0;
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, occMaterial_Shininess (theIsFront));
  }

  Diffuse  += occLight_Diffuse (theId) * aNdotL * anAtten * theShadow;
  Specular += occLight_Specular(theId) * aSpecl * anAtten * theShadow;
}
