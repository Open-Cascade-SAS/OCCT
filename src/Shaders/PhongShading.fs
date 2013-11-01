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

//! Direction to the viewer.
varying vec3 View;

//! Vertex normal in view space.
varying vec3 Normal;

//! Vertex position in view space.
varying vec4 Position;


//! Ambient contribution of light sources.
vec3 Ambient;

//! Diffuse contribution of light sources.
vec3 Diffuse;

//! Specular contribution of light sources.
vec3 Specular;


// =======================================================================
// function : AmbientLight
// purpose  : Computes contribution of OCCT pure ambient light source
// =======================================================================
void AmbientLight (in int theIndex)
{
  Ambient += occLightSources[theIndex].Ambient;
}

// =======================================================================
// function : PointLight
// purpose  : Computes contribution of OCCT isotropic point light source
// =======================================================================
void PointLight (in int  theIndex,
                 in vec3 theNormal,
                 in vec3 theView,
                 in vec3 thePoint)
{
  vec3 aLight = occLightSources[theIndex].Position;
  if (occLightSources[theIndex].Head == 0)
  {
    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 1.0));
  }
  aLight -= thePoint;

  float aDist = length (aLight);
  aLight = aLight * (1.0 / aDist);
  
  float anAttenuation = 1.0 / (occLightSources[theIndex].ConstAttenuation +
                               occLightSources[theIndex].LinearAttenuation * aDist);

  vec3 aHalf = normalize (aLight + theView);

  float aNdotL = max (0.0, dot (theNormal, aLight));
  float aNdotH = max (0.0, dot (theNormal,  aHalf));

  float aSpecl = 0.0;
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, occFrontMaterial.Shininess);
  }
  
  Ambient  += occLightSources[theIndex].Ambient * anAttenuation;
  Diffuse  += occLightSources[theIndex].Diffuse * aNdotL * anAttenuation;
  Specular += occLightSources[theIndex].Specular * aSpecl * anAttenuation;
}

// =======================================================================
// function : DirectionalLight
// purpose  : Computes contribution of OCCT directional light source
// =======================================================================
void DirectionalLight (in int theIndex,
                       in vec3 theNormal,
                       in vec3 theView)
{
  vec3 aLight = normalize (occLightSources[theIndex].Position);

  if (occLightSources[theIndex].Head == 0)
  {
    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 0.0));
  }
  
  vec3 aHalf = normalize (aLight + theView);
  
  float aNdotL = max (0.0, dot (theNormal, aLight));
  float aNdotH = max (0.0, dot (theNormal,  aHalf));

  float aSpecl = 0.0;
  
  if (aNdotL > 0.0)
  {
    aSpecl = pow (aNdotH, occFrontMaterial.Shininess);
  }

  Ambient  += occLightSources[theIndex].Ambient;
  Diffuse  += occLightSources[theIndex].Diffuse * aNdotL;
  Specular += occLightSources[theIndex].Specular * aSpecl;
}

// =======================================================================
// function : ComputeLighting
// purpose  : Computes illumination from OCCT light sources
// =======================================================================
vec4 ComputeLighting (in vec3 theNormal,
                      in vec3 theView,
                      in vec4 thePoint)
{
  // Clear the light intensity accumulators
  Ambient  = vec3 (0.0);
  Diffuse  = vec3 (0.0);
  Specular = vec3 (0.0);

  vec3 aPoint = thePoint.xyz / thePoint.w;
	
  for (int anIndex = 0; anIndex < occLightSourcesCount; ++anIndex)
  {
    occLightSource light = occLightSources[anIndex];
    
    if (light.Type == occAmbientLight)
    {
      AmbientLight (anIndex);
    }
    else if (light.Type == occDirectLight)
	  {
	    DirectionalLight (anIndex, theNormal, theView);
	  }
	  else if (light.Type == occPointLight)
		{
      PointLight (anIndex, theNormal, theView, aPoint);
		}
		else if (light.Type == occSpotLight)
		{
		  /* Not implemented */
		}
  }
  
  return vec4 (Ambient,  1.0) * occFrontMaterial.Ambient +
         vec4 (Diffuse,  1.0) * occFrontMaterial.Diffuse +
         vec4 (Specular, 1.0) * occFrontMaterial.Specular;
}

// =======================================================================
// function : main
// purpose  : Entry point to the fragment shader
// =======================================================================
void main()
{
  gl_FragColor = ComputeLighting (normalize (Normal),
                                  normalize (View),
                                  Position);
}
