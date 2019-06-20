// Created on: 2013-10-10
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

varying vec3 View;          //!< Direction to the viewer
varying vec3 Normal;        //!< Vertex normal in view space
varying vec4 Position;      //!< Vertex position in view space.
varying vec4 PositionWorld; //!< Vertex position in world space

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
  if (!occLight_IsHeadlight (theId))
  {
    aLight = vec3 (occWorldViewMatrix * vec4 (aLight, 1.0));
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

//! Computes contribution of spotlight source
void spotLight (in int  theId,
                in vec3 theNormal,
                in vec3 theView,
                in vec3 thePoint)
{
  vec3 aLight   = occLight_Position      (theId).xyz;
  vec3 aSpotDir = occLight_SpotDirection (theId).xyz;
  if (occLight_IsHeadlight (theId) == 0)
  {
    aLight   = vec3 (occWorldViewMatrix * vec4 (aLight,   1.0));
    aSpotDir = vec3 (occWorldViewMatrix * vec4 (aSpotDir, 0.0));
  }
  aLight -= thePoint;

  float aDist = length (aLight);
  aLight = aLight * (1.0 / aDist);

  aSpotDir = normalize (aSpotDir);

  // light cone
  float aCosA = dot (aSpotDir, -aLight);
  if (aCosA >= 1.0 || aCosA < cos (occLight_SpotCutOff (theId)))
  {
    return;
  }

  float anExponent = occLight_SpotExponent (theId);
  float anAtten    = 1.0 / (occLight_ConstAttenuation  (theId)
                          + occLight_LinearAttenuation (theId) * aDist);
  if (anExponent > 0.0)
  {
    anAtten *= pow (aCosA, anExponent * 128.0);
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
    aLight = vec3 (occWorldViewMatrix * vec4 (aLight, 0.0));
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
  for (int anIndex = 0; anIndex < occLightSourcesCount; ++anIndex)
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
      spotLight (anIndex, theNormal, theView, aPoint);
    }
  }

  vec4 aMaterialAmbient  = gl_FrontFacing ? occFrontMaterial_Ambient()  : occBackMaterial_Ambient();
  vec4 aMaterialDiffuse  = gl_FrontFacing ? occFrontMaterial_Diffuse()  : occBackMaterial_Diffuse();
  vec4 aMaterialSpecular = gl_FrontFacing ? occFrontMaterial_Specular() : occBackMaterial_Specular();
  vec4 aMaterialEmission = gl_FrontFacing ? occFrontMaterial_Emission() : occBackMaterial_Emission();
  vec3 aColor = Ambient  * aMaterialAmbient.rgb
              + Diffuse  * aMaterialDiffuse.rgb
              + Specular * aMaterialSpecular.rgb
                         + aMaterialEmission.rgb;
  return vec4 (aColor, aMaterialDiffuse.a);
}

//! Entry point to the Fragment Shader
void main()
{
  // process clipping planes
  for (int anIndex = 0; anIndex < occClipPlaneCount; ++anIndex)
  {
    vec4 aClipEquation = occClipPlaneEquations[anIndex];
    if (dot (aClipEquation.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation.w < 0.0)
    {
      discard;
    }
  }

  vec4 aColor = computeLighting (normalize (Normal), normalize (View), Position);
  occSetFragColor (aColor);
}
