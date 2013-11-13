// Created on: 2013-10-10
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

varying vec3 View;     //!< Direction to the viewer
varying vec3 Normal;   //!< Vertex normal in view space
varying vec4 Position; //!< Vertex position in view space.

vec3 Ambient;  //!< Ambient  contribution of light sources
vec3 Diffuse;  //!< Diffuse  contribution of light sources
vec3 Specular; //!< Specular contribution of light sources

//! Computes contribution of isotropic point light source
void pointLight (in int  theId,
                 in vec3 theNormal,
                 in vec3 theView,
                 in vec3 thePoint)
{
  vec3 aLight = occLight_Position (theId).xyz;
  if (occLight_IsHeadlight (theId) == 0)
  {
    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 1.0));
  }
  aLight -= thePoint;

  float aDist = length (aLight);
  aLight = aLight * (1.0 / aDist);

  float anAtten = 1.0 / (occLight_ConstAttenuation  (theId)
                       + occLight_LinearAttenuation (theId) * aDist);

  vec3 aHalf = normalize (aLight + theView);

  vec3  aFaceSideNormal = gl_FrontFacing ? theNormal : -theNormal;
  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));
  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));

  float aSpecl = 0.0;
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, gl_FrontFacing ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());
  }

  Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL * anAtten;
  Specular += occLight_Specular (theId).rgb * aSpecl * anAtten;
}

//! Computes contribution of directional light source
void directionalLight (in int  theId,
                       in vec3 theNormal,
                       in vec3 theView)
{
  vec3 aLight = normalize (occLight_Position (theId).xyz);
  if (occLight_IsHeadlight (theId) == 0)
  {
    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 0.0));
  }

  vec3 aHalf = normalize (aLight + theView);

  vec3  aFaceSideNormal = gl_FrontFacing ? theNormal : -theNormal;
  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));
  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));

  float aSpecl = 0.0;
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, gl_FrontFacing ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());
  }

  Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL;
  Specular += occLight_Specular (theId).rgb * aSpecl;
}

//! Computes illumination from light sources
vec4 computeLighting (in vec3 theNormal,
                      in vec3 theView,
                      in vec4 thePoint)
{
  // Clear the light intensity accumulators
  Ambient  = occLightAmbient.rgb;
  Diffuse  = vec3 (0.0);
  Specular = vec3 (0.0);
  vec3 aPoint = thePoint.xyz / thePoint.w;
  for (int anIndex = 0; anIndex < THE_MAX_LIGHTS; ++anIndex)
  {
    int aType = occLight_Type (anIndex);
    if (aType == OccLightType_Direct)
    {
      directionalLight (anIndex, theNormal, theView);
    }
    else if (aType == OccLightType_Point)
    {
      pointLight (anIndex, theNormal, theView, aPoint);
    }
    else if (aType == OccLightType_Spot)
    {
      // Not implemented
    }
  }

  vec4 aMaterialAmbient  = gl_FrontFacing ? occFrontMaterial_Ambient()  : occBackMaterial_Ambient();
  vec4 aMaterialDiffuse  = gl_FrontFacing ? occFrontMaterial_Diffuse()  : occBackMaterial_Diffuse();
  vec4 aMaterialSpecular = gl_FrontFacing ? occFrontMaterial_Specular() : occBackMaterial_Specular();
  return vec4 (Ambient,  1.0) * aMaterialAmbient
       + vec4 (Diffuse,  1.0) * aMaterialDiffuse
       + vec4 (Specular, 1.0) * aMaterialSpecular;
}

//! Entry point to the Fragment Shader
void main()
{
  gl_FragColor = computeLighting (normalize (Normal),
                                  normalize (View),
                                  Position);
}
