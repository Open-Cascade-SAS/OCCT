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

// This files includes definition of common uniform variables in OCCT GLSL programs

#define THE_MAX_LIGHTS      8
#define THE_MAX_CLIP_PLANES 8

// compatibility macros
#if (__VERSION__ >= 130)
  #define THE_ATTRIBUTE  in
  #define THE_SHADER_IN  in
  #define THE_SHADER_OUT out
  #define THE_OUT        out
  #define occTexture2D   texture
#else
  #define THE_ATTRIBUTE  attribute
  #define THE_SHADER_IN  varying
  #define THE_SHADER_OUT varying
  #define THE_OUT
  #define occTexture2D   texture2D
#endif

#ifdef GL_ES
  #define THE_PREC_ENUM lowp // enumerations should fit into lowp range
#else
  #define THE_PREC_ENUM
#endif

// Vertex attributes
#ifdef VERTEX_SHADER
  THE_ATTRIBUTE vec4 occVertex;
  THE_ATTRIBUTE vec3 occNormal;
  THE_ATTRIBUTE vec4 occTexCoord;
  THE_ATTRIBUTE vec4 occVertColor;
#elif (__VERSION__ >= 130)
  out vec4 occFragColor;
#else
  #define occFragColor gl_FragColor
#endif

// Matrix state
uniform mat4 occWorldViewMatrix;  //!< World-view  matrix
uniform mat4 occProjectionMatrix; //!< Projection  matrix
uniform mat4 occModelWorldMatrix; //!< Model-world matrix

uniform mat4 occWorldViewMatrixInverse;    //!< Inverse of the world-view  matrix
uniform mat4 occProjectionMatrixInverse;   //!< Inverse of the projection  matrix
uniform mat4 occModelWorldMatrixInverse;   //!< Inverse of the model-world matrix

uniform mat4 occWorldViewMatrixTranspose;  //!< Transpose of the world-view  matrix
uniform mat4 occProjectionMatrixTranspose; //!< Transpose of the projection  matrix
uniform mat4 occModelWorldMatrixTranspose; //!< Transpose of the model-world matrix

uniform mat4 occWorldViewMatrixInverseTranspose;  //!< Transpose of the inverse of the world-view  matrix
uniform mat4 occProjectionMatrixInverseTranspose; //!< Transpose of the inverse of the projection  matrix
uniform mat4 occModelWorldMatrixInverseTranspose; //!< Transpose of the inverse of the model-world matrix

// light type enumeration
const int OccLightType_Direct = 1; //!< directional     light source
const int OccLightType_Point  = 2; //!< isotropic point light source
const int OccLightType_Spot   = 3; //!< spot            light source

// Light sources
uniform               vec4 occLightAmbient;      //!< Cumulative ambient color
uniform THE_PREC_ENUM int  occLightSourcesCount; //!< Total number of light sources
int   occLight_Type              (in int theId); //!< Type of light source
int   occLight_IsHeadlight       (in int theId); //!< Is light a headlight?
vec4  occLight_Diffuse           (in int theId); //!< Diffuse intensity for specified light source
vec4  occLight_Specular          (in int theId); //!< Specular intensity (currently - equals to diffuse intencity)
vec4  occLight_Position          (in int theId); //!< Position of specified light source
vec4  occLight_SpotDirection     (in int theId); //!< Direction of specified spot light source
float occLight_ConstAttenuation  (in int theId); //!< Const attenuation factor of positional light source
float occLight_LinearAttenuation (in int theId); //!< Linear attenuation factor of positional light source
float occLight_SpotCutOff        (in int theId); //!< Maximum spread angle of the spot light (in radians)
float occLight_SpotExponent      (in int theId); //!< Attenuation of the spot light intensity (from 0 to 1)

// Front material properties accessors
vec4  occFrontMaterial_Emission(void);     //!< Emission color
vec4  occFrontMaterial_Ambient(void);      //!< Ambient  reflection
vec4  occFrontMaterial_Diffuse(void);      //!< Diffuse  reflection
vec4  occFrontMaterial_Specular(void);     //!< Specular reflection
float occFrontMaterial_Shininess(void);    //!< Specular exponent
float occFrontMaterial_Transparency(void); //!< Transparency coefficient

// Back material properties accessors
vec4  occBackMaterial_Emission(void);      //!< Emission color
vec4  occBackMaterial_Ambient(void);       //!< Ambient  reflection
vec4  occBackMaterial_Diffuse(void);       //!< Diffuse  reflection
vec4  occBackMaterial_Specular(void);      //!< Specular reflection
float occBackMaterial_Shininess(void);     //!< Specular exponent
float occBackMaterial_Transparency(void);  //!< Transparency coefficient

uniform               vec4      occColor;              //!< color value (in case of disabled lighting)
uniform THE_PREC_ENUM int       occDistinguishingMode; //!< Are front and back faces distinguished?
uniform THE_PREC_ENUM int       occTextureEnable;      //!< Is texture enabled?
uniform               sampler2D occActiveSampler;      //!< Current active sampler
uniform               float     occPointSize;          //!< point size

// clipping planes state
const int OccEquationCoords_View  = 0; //!< view-space  clipping plane
const int OccEquationCoords_World = 1; //!< world-space clipping plane

//! Parameters of clipping planes
uniform               vec4 occClipPlaneEquations[THE_MAX_CLIP_PLANES];
uniform THE_PREC_ENUM int  occClipPlaneSpaces   [THE_MAX_CLIP_PLANES];
uniform THE_PREC_ENUM int  occClipPlaneCount;   //!< Total number of clip planes
