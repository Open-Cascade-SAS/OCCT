// Created on: 2013-09-26
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

#include <typeinfo>

#include <Graphic3d_TextureParams.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_ClippingIterator.hxx>
#include <OpenGl_Context.hxx>
#include <Graphic3d_CubeMapPacked.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_VertexBufferCompat.hxx>
#include <OpenGl_PointSprite.hxx>
#include <OpenGl_Workspace.hxx>
#include <TCollection_ExtendedString.hxx>

#include "../Shaders/Shaders_PBRDistribution_glsl.pxx"
#include "../Shaders/Shaders_PBRGeometry_glsl.pxx"
#include "../Shaders/Shaders_PBRFresnel_glsl.pxx"
#include "../Shaders/Shaders_PBRCookTorrance_glsl.pxx"
#include "../Shaders/Shaders_PBRIllumination_glsl.pxx"
#include "../Shaders/Shaders_PBREnvBaking_fs.pxx"
#include "../Shaders/Shaders_PBREnvBaking_vs.pxx"
#include "../Shaders/Shaders_PointLightAttenuation_glsl.pxx"
#include "../Shaders/Shaders_TangentSpaceNormal_glsl.pxx"

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShaderManager,Standard_Transient)

namespace
{
  //! Number specifying maximum number of light sources to prepare a GLSL program with unrolled loop.
  const Standard_Integer THE_NB_UNROLLED_LIGHTS_MAX = 32;

  //! Compute the size of array storing holding light sources definition.
  static Standard_Integer roundUpMaxLightSources (Standard_Integer theNbLights)
  {
    Standard_Integer aMaxLimit = THE_NB_UNROLLED_LIGHTS_MAX;
    for (; aMaxLimit < theNbLights; aMaxLimit *= 2) {}
    return aMaxLimit;
  }

#define EOL "\n"

//! Compute TexCoord value in Vertex Shader
const char THE_VARY_TexCoord_Trsf[] =
  EOL"  float aRotSin = occTextureTrsf_RotationSin();"
  EOL"  float aRotCos = occTextureTrsf_RotationCos();"
  EOL"  vec2  aTex2   = vec2 (occTexCoord.x * aRotCos - occTexCoord.y * aRotSin,"
  EOL"                        occTexCoord.x * aRotSin + occTexCoord.y * aRotCos);"
  EOL"  aTex2 = (aTex2 + occTextureTrsf_Translation()) * occTextureTrsf_Scale();"
  EOL"  TexCoord = vec4(aTex2, occTexCoord.zw);";

//! Auxiliary function to flip gl_PointCoord vertically
#define THE_VEC2_glPointCoord "vec2 (gl_PointCoord.x, 1.0 - gl_PointCoord.y)"

//! Auxiliary function to transform normal from model to view coordinate system.
const char THE_FUNC_transformNormal_view[] =
  EOL"vec3 transformNormal (in vec3 theNormal)"
  EOL"{"
  EOL"  vec4 aResult = occWorldViewMatrixInverseTranspose"
  EOL"               * occModelWorldMatrixInverseTranspose"
  EOL"               * vec4 (theNormal, 0.0);"
  EOL"  return normalize (aResult.xyz);"
  EOL"}";

//! The same function as THE_FUNC_transformNormal but is used in PBR pipeline.
//! The normals are expected to be in world coordinate system in PBR pipeline.
const char THE_FUNC_transformNormal_world[] =
  EOL"vec3 transformNormal (in vec3 theNormal)"
  EOL"{"
  EOL"  vec4 aResult = occModelWorldMatrixInverseTranspose"
  EOL"               * vec4 (theNormal, 0.0);"
  EOL"  return normalize (aResult.xyz);"
  EOL"}";

//! Global shader variable for color definition with lighting enabled.
const char THE_FUNC_lightDef[] =
  EOL"vec3 Ambient;"   //!< Ambient  contribution of light sources
  EOL"vec3 Diffuse;"   //!< Diffuse  contribution of light sources
  EOL"vec3 Specular;"; //!< Specular contribution of light sources

//! Global shader variable for color definition with lighting enabled.
const char THE_FUNC_PBR_lightDef[] =
  EOL"vec3  DirectLighting;" //!< Accumulator of direct lighting from light sources
  EOL"vec4  BaseColor;"      //!< Base color (albedo) of material for PBR
  EOL"float Metallic;"       //!< Metallic coefficient of material
  EOL"float NormalizedRoughness;" //!< Normalized roughness coefficient of material
  EOL"float Roughness;"      //!< Roughness coefficient of material
  EOL"vec3  Emission;"       //!< Light intensity emitted by material
  EOL"float IOR;";           //!< Material's index of refraction

//! Function computes contribution of isotropic point light source
const char THE_FUNC_pointLight[] =
  EOL"void pointLight (in int  theId,"
  EOL"                 in vec3 theNormal,"
  EOL"                 in vec3 theView,"
  EOL"                 in vec3 thePoint,"
  EOL"                 in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = occLight_Position (theId);"
  EOL"  if (!occLight_IsHeadlight (theId))"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrix * vec4 (aLight, 1.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  float aRange = occLight_Range (theId);"
  EOL"  float anAtten = occPointLightAttenuation (aDist, aRange, occLight_LinearAttenuation (theId), occLight_ConstAttenuation (theId));"
  EOL"  if (anAtten <= 0.0) return;"
  EOL"  aLight /= aDist;"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLight_Diffuse (theId) * aNdotL * anAtten;"
  EOL"  Specular += occLight_Specular(theId) * aSpecl * anAtten;"
  EOL"}";

//! Function computes contribution of isotropic point light source
const char THE_FUNC_PBR_pointLight[] =
  EOL"void pointLight (in int  theId,"
  EOL"                 in vec3 theNormal,"
  EOL"                 in vec3 theView,"
  EOL"                 in vec3 thePoint,"
  EOL"                 in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = occLight_Position (theId);"
  EOL"  if (occLight_IsHeadlight (theId))"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrixInverse * vec4 (aLight, 1.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  float aRange = occLight_Range (theId);"
  EOL"  float anAtten = occPointLightAttenuation (aDist, aRange);"
  EOL"  if (anAtten <= 0.0) return;"
  EOL"  aLight /= aDist;"
  EOL
  EOL"  theNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  DirectLighting += occPBRIllumination (theView, aLight, theNormal,"
  EOL"                                        BaseColor, Metallic, Roughness, IOR,"
  EOL"                                        occLight_Specular (theId),"
  EOL"                                        occLight_Intensity(theId) * anAtten);"
  EOL"}";

//! Function computes contribution of spotlight source
const char THE_FUNC_spotLight[] =
  EOL"void spotLight (in int  theId,"
  EOL"                in vec3 theNormal,"
  EOL"                in vec3 theView,"
  EOL"                in vec3 thePoint,"
  EOL"                in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight   = occLight_Position      (theId);"
  EOL"  vec3 aSpotDir = occLight_SpotDirection (theId);"
  EOL"  if (!occLight_IsHeadlight (theId))"
  EOL"  {"
  EOL"    aLight   = vec3 (occWorldViewMatrix * vec4 (aLight,   1.0));"
  EOL"    aSpotDir = vec3 (occWorldViewMatrix * vec4 (aSpotDir, 0.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  float aRange = occLight_Range (theId);"
  EOL"  float anAtten = occPointLightAttenuation (aDist, aRange, occLight_LinearAttenuation (theId), occLight_ConstAttenuation (theId));"
  EOL"  if (anAtten <= 0.0) return;"
  EOL"  aLight /= aDist;"
  EOL
  EOL"  aSpotDir = normalize (aSpotDir);"
  // light cone
  EOL"  float aCosA = dot (aSpotDir, -aLight);"
  EOL"  if (aCosA >= 1.0 || aCosA < cos (occLight_SpotCutOff (theId)))"
  EOL"  {"
  EOL"    return;"
  EOL"  }"
  EOL
  EOL"  float anExponent = occLight_SpotExponent (theId);"
  EOL"  if (anExponent > 0.0)"
  EOL"  {"
  EOL"    anAtten *= pow (aCosA, anExponent * 128.0);"
  EOL"  }"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLight_Diffuse (theId) * aNdotL * anAtten;"
  EOL"  Specular += occLight_Specular(theId) * aSpecl * anAtten;"
  EOL"}";

//! Function computes contribution of spotlight source
  const char THE_FUNC_PBR_spotLight[] =
  EOL"void spotLight (in int  theId,"
  EOL"                in vec3 theNormal,"
  EOL"                in vec3 theView,"
  EOL"                in vec3 thePoint,"
  EOL"                in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight   = occLight_Position      (theId);"
  EOL"  vec3 aSpotDir = occLight_SpotDirection (theId);"
  EOL"  if (occLight_IsHeadlight (theId))"
  EOL"  {"
  EOL"    aLight   = vec3 (occWorldViewMatrixInverse * vec4 (aLight,   1.0));"
  EOL"    aSpotDir = vec3 (occWorldViewMatrixInverse * vec4 (aSpotDir, 0.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  float aRange = occLight_Range (theId);"
  EOL"  float anAtten = occPointLightAttenuation (aDist, aRange);"
  EOL"  if (anAtten <= 0.0) return;"
  EOL"  aLight /= aDist;"
  EOL
  EOL"  aSpotDir = normalize (aSpotDir);"
  // light cone
  EOL"  float aCosA = dot (aSpotDir, -aLight);"
  EOL"  float aRelativeAngle = 2.0 * acos(aCosA) / occLight_SpotCutOff(theId);"
  EOL"  if (aCosA >= 1.0 || aRelativeAngle > 1.0)"
  EOL"  {"
  EOL"    return;"
  EOL"  }"
  EOL"  float anExponent = occLight_SpotExponent (theId);"
  EOL"  if ((1.0 - aRelativeAngle) <= anExponent)"
  EOL"  {"
  EOL"    float anAngularAttenuationOffset = cos(0.5 * occLight_SpotCutOff(theId));"
  EOL"    float anAngularAttenuationScale = 1.0 / max(0.001, cos(0.5 * occLight_SpotCutOff(theId) * (1.0 - anExponent)) - anAngularAttenuationOffset);"
  EOL"    anAngularAttenuationOffset *= -anAngularAttenuationScale;"
  EOL"    float anAngularAttenuantion = clamp(aCosA * anAngularAttenuationScale + anAngularAttenuationOffset, 0.0, 1.0);"
  EOL"    anAtten *= anAngularAttenuantion * anAngularAttenuantion;"
  EOL"  }"
  EOL"  theNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  DirectLighting += occPBRIllumination (theView, aLight, theNormal,"
  EOL"                                        BaseColor, Metallic, Roughness, IOR,"
  EOL"                                        occLight_Specular(theId),"
  EOL"                                        occLight_Intensity(theId) * anAtten);"
  EOL"}";

//! Function computes contribution of directional light source
const char THE_FUNC_directionalLight[] =
  EOL"void directionalLight (in int  theId,"
  EOL"                       in vec3 theNormal,"
  EOL"                       in vec3 theView,"
  EOL"                       in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = normalize (occLight_Position (theId));"
  EOL"  if (!occLight_IsHeadlight (theId))"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrix * vec4 (aLight, 0.0));"
  EOL"  }"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLight_Diffuse  (theId) * aNdotL;"
  EOL"  Specular += occLight_Specular (theId) * aSpecl;"
  EOL"}";

//! Function computes contribution of directional light source
const char THE_FUNC_PBR_directionalLight[] =
  EOL"void directionalLight (in int  theId,"
  EOL"                       in vec3 theNormal,"
  EOL"                       in vec3 theView,"
  EOL"                       in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = normalize (occLight_Position (theId));"
  EOL"  if (occLight_IsHeadlight (theId))"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrixInverse * vec4 (aLight, 0.0));"
  EOL"  }"
  EOL
  EOL"  theNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  DirectLighting += occPBRIllumination (theView, aLight, theNormal,"
  EOL"                                        BaseColor, Metallic, Roughness, IOR,"
  EOL"                                        occLight_Specular (theId),"
  EOL"                                        occLight_Intensity(theId));"
  EOL"}";

//! The same as THE_FUNC_directionalLight but for the light with zero index
//! (avoids limitations on some mobile devices).
const char THE_FUNC_directionalLightFirst[] =
  EOL"void directionalLightFirst (in vec3 theNormal,"
  EOL"                            in vec3 theView,"
  EOL"                            in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = normalize (occLight_Position(0));"
  EOL"  if (!occLight_IsHeadlight (0))"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrix * vec4 (aLight, 0.0));"
  EOL"  }"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLight_Diffuse(0)  * aNdotL;"
  EOL"  Specular += occLight_Specular(0) * aSpecl;"
  EOL"}";

//! Returns the real cubemap fetching direction considering sides orientation, memory layout and vertical flip.
const char THE_FUNC_cubemap_vector_transform[] =
  EOL"vec3 cubemapVectorTransform (in vec3 theVector,"
  EOL"                             in int  theYCoeff,"
  EOL"                             in int  theZCoeff)"
  EOL"{"
  EOL"  theVector = theVector.yzx;"
  EOL"  theVector.y *= float(theYCoeff);"
  EOL"  theVector.z *= float(theZCoeff);"
  EOL"  return theVector;"
  EOL"}";

//! Process clipping planes in Fragment Shader.
//! Should be added at the beginning of the main() function.
const char THE_FRAG_CLIP_PLANES_N[] =
  EOL"  for (int aPlaneIter = 0; aPlaneIter < occClipPlaneCount; ++aPlaneIter)"
  EOL"  {"
  EOL"    vec4 aClipEquation = occClipPlaneEquations[aPlaneIter];"
  EOL"    if (dot (aClipEquation.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation.w < 0.0)"
  EOL"    {"
  EOL"      discard;"
  EOL"    }"
  EOL"  }";

//! Process chains of clipping planes in Fragment Shader.
const char THE_FRAG_CLIP_CHAINS_N[] =
EOL"  for (int aPlaneIter = 0; aPlaneIter < occClipPlaneCount;)"
EOL"  {"
EOL"    vec4 aClipEquation = occClipPlaneEquations[aPlaneIter];"
EOL"    if (dot (aClipEquation.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation.w < 0.0)"
EOL"    {"
EOL"      if (occClipPlaneChains[aPlaneIter] == 1)"
EOL"      {"
EOL"        discard;"
EOL"      }"
EOL"      aPlaneIter += 1;"
EOL"    }"
EOL"    else"
EOL"    {"
EOL"      aPlaneIter += occClipPlaneChains[aPlaneIter];"
EOL"    }"
EOL"  }";

//! Process 1 clipping plane in Fragment Shader.
const char THE_FRAG_CLIP_PLANES_1[] =
  EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
  EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0)"
  EOL"  {"
  EOL"    discard;"
  EOL"  }";

//! Process 2 clipping planes in Fragment Shader.
const char THE_FRAG_CLIP_PLANES_2[] =
  EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
  EOL"  vec4 aClipEquation1 = occClipPlaneEquations[1];"
  EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0"
  EOL"   || dot (aClipEquation1.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation1.w < 0.0)"
  EOL"  {"
  EOL"    discard;"
  EOL"  }";

//! Process a chain of 2 clipping planes in Fragment Shader (3/4 section).
const char THE_FRAG_CLIP_CHAINS_2[] =
EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
EOL"  vec4 aClipEquation1 = occClipPlaneEquations[1];"
EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0"
EOL"   && dot (aClipEquation1.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation1.w < 0.0)"
EOL"  {"
EOL"    discard;"
EOL"  }";

//! Modify color for Wireframe presentation.
const char THE_FRAG_WIREFRAME_COLOR[] =
EOL"vec4 getFinalColor(void)"
EOL"{"
EOL"  float aDistance = min (min (EdgeDistance[0], EdgeDistance[1]), EdgeDistance[2]);"
EOL"  bool isHollow = occWireframeColor.a < 0.0;"
EOL"  float aMixVal = smoothstep (occLineWidth - occLineFeather * 0.5, occLineWidth + occLineFeather * 0.5, aDistance);"
EOL"  vec4 aMixColor = isHollow"
EOL"                 ? vec4 (getColor().rgb, 1.0 - aMixVal)"          // edges only (of interior color)
EOL"                 : mix (occWireframeColor, getColor(), aMixVal);" // interior + edges
EOL"  return aMixColor;"
EOL"}";

//! Compute gl_Position vertex shader output.
const char THE_VERT_gl_Position[] =
EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;";

//! Displace gl_Position alongside vertex normal for outline rendering.
//! This code adds silhouette only for smooth surfaces of closed primitive, and produces visual artifacts on sharp edges.
const char THE_VERT_gl_Position_OUTLINE[] =
EOL"  float anOutlineDisp = occOrthoScale > 0.0 ? occOrthoScale : gl_Position.w;"
EOL"  vec4  anOutlinePos  = occVertex + vec4 (occNormal * (occSilhouetteThickness * anOutlineDisp), 0.0);"
EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * anOutlinePos;";

#if !defined(GL_ES_VERSION_2_0)

  static const GLfloat THE_DEFAULT_AMBIENT[4]    = { 0.0f, 0.0f, 0.0f, 1.0f };
  static const GLfloat THE_DEFAULT_SPOT_DIR[3]   = { 0.0f, 0.0f, -1.0f };
  static const GLfloat THE_DEFAULT_SPOT_EXPONENT = 0.0f;
  static const GLfloat THE_DEFAULT_SPOT_CUTOFF   = 180.0f;

  //! Bind FFP light source.
  static void bindLight (const Graphic3d_CLight& theLight,
                         const GLenum        theLightGlId,
                         const OpenGl_Mat4&  theModelView,
                         OpenGl_Context*     theCtx)
  {
    // the light is a headlight?
    if (theLight.IsHeadlight())
    {
      theCtx->core11->glMatrixMode (GL_MODELVIEW);
      theCtx->core11->glLoadIdentity();
    }

    // setup light type
    const Graphic3d_Vec4& aLightColor = theLight.PackedColor();
    switch (theLight.Type())
    {
      case Graphic3d_TOLS_AMBIENT    : break; // handled by separate if-clause at beginning of method
      case Graphic3d_TOLS_DIRECTIONAL:
      {
        // if the last parameter of GL_POSITION, is zero, the corresponding light source is a Directional one
        const OpenGl_Vec4 anInfDir = -theLight.PackedDirectionRange();

        // to create a realistic effect,  set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE.
        theCtx->core11->glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
        theCtx->core11->glLightfv (theLightGlId, GL_DIFFUSE,               aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPECULAR,              aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_POSITION,              anInfDir.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
        theCtx->core11->glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
        theCtx->core11->glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
        break;
      }
      case Graphic3d_TOLS_POSITIONAL:
      {
        // to create a realistic effect, set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE
        const OpenGl_Vec4 aPosition (static_cast<float>(theLight.Position().X()), static_cast<float>(theLight.Position().Y()), static_cast<float>(theLight.Position().Z()), 1.0f);
        theCtx->core11->glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
        theCtx->core11->glLightfv (theLightGlId, GL_DIFFUSE,               aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPECULAR,              aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_POSITION,              aPosition.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
        theCtx->core11->glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
        theCtx->core11->glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
        theCtx->core11->glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
        theCtx->core11->glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
        theCtx->core11->glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0f);
        break;
      }
      case Graphic3d_TOLS_SPOT:
      {
        const OpenGl_Vec4 aPosition (static_cast<float>(theLight.Position().X()), static_cast<float>(theLight.Position().Y()), static_cast<float>(theLight.Position().Z()), 1.0f);
        theCtx->core11->glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
        theCtx->core11->glLightfv (theLightGlId, GL_DIFFUSE,               aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPECULAR,              aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_POSITION,              aPosition.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPOT_DIRECTION,        theLight.PackedDirectionRange().GetData());
        theCtx->core11->glLightf  (theLightGlId, GL_SPOT_EXPONENT,         theLight.Concentration() * 128.0f);
        theCtx->core11->glLightf  (theLightGlId, GL_SPOT_CUTOFF,          (theLight.Angle() * 180.0f) / GLfloat(M_PI));
        theCtx->core11->glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
        theCtx->core11->glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
        theCtx->core11->glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0f);
        break;
      }
    }

    // restore matrix in case of headlight
    if (theLight.IsHeadlight())
    {
      theCtx->core11->glLoadMatrixf (theModelView.GetData());
    }

    glEnable (theLightGlId);
  }
#endif

  //! Generate map key for light sources configuration.
  static TCollection_AsciiString genLightKey (const Handle(Graphic3d_LightSet)& theLights)
  {
    if (theLights->NbEnabled() <= THE_NB_UNROLLED_LIGHTS_MAX)
    {
      return TCollection_AsciiString ("l_") + theLights->KeyEnabledLong();
    }

    const Standard_Integer aMaxLimit = roundUpMaxLightSources (theLights->NbEnabled());
    return TCollection_AsciiString ("l_") + theLights->KeyEnabledShort() + aMaxLimit;
  }
}

// =======================================================================
// function : OpenGl_ShaderManager
// purpose  : Creates new empty shader manager
// =======================================================================
OpenGl_ShaderManager::OpenGl_ShaderManager (OpenGl_Context* theContext)
: myFfpProgram (new OpenGl_ShaderProgramFFP()),
  myShadingModel (Graphic3d_TOSM_VERTEX),
  myUnlitPrograms (new OpenGl_SetOfPrograms()),
  myContext  (theContext),
  mySRgbState (theContext->ToRenderSRGB()),
  myHasLocalOrigin (Standard_False),
  myLastView (NULL)
{
  //
}

// =======================================================================
// function : ~OpenGl_ShaderManager
// purpose  : Releases resources of shader manager
// =======================================================================
OpenGl_ShaderManager::~OpenGl_ShaderManager()
{
  myProgramList.Clear();
  if (!myPBREnvironment.IsNull())
  {
    myPBREnvironment->Release (myContext);
  }
}

// =======================================================================
// function : clear
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::clear()
{
  myProgramList.Clear();
  myLightPrograms.Nullify();
  myUnlitPrograms = new OpenGl_SetOfPrograms();
  myOutlinePrograms.Nullify();
  myMapOfLightPrograms.Clear();
  myFontProgram.Nullify();
  myBlitPrograms[0].Init (Handle(OpenGl_ShaderProgram)());
  myBlitPrograms[1].Init (Handle(OpenGl_ShaderProgram)());
  myBoundBoxProgram.Nullify();
  myBoundBoxVertBuffer.Nullify();
  for (Standard_Integer aModeIter = 0; aModeIter < Graphic3d_StereoMode_NB; ++aModeIter)
  {
    myStereoPrograms[aModeIter].Nullify();
  }
  switchLightPrograms();
}

// =======================================================================
// function : Create
// purpose  : Creates new shader program
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::Create (const Handle(Graphic3d_ShaderProgram)& theProxy,
                                               TCollection_AsciiString&               theShareKey,
                                               Handle(OpenGl_ShaderProgram)&          theProgram)
{
  theProgram.Nullify();
  if (theProxy.IsNull())
  {
    return Standard_False;
  }

  theShareKey = theProxy->GetId();
  if (myContext->GetResource<Handle(OpenGl_ShaderProgram)> (theShareKey, theProgram))
  {
    if (theProgram->Share())
    {
      myProgramList.Append (theProgram);
    }
    return Standard_True;
  }

  theProgram = new OpenGl_ShaderProgram (theProxy);
  if (!theProgram->Initialize (myContext, theProxy->ShaderObjects()))
  {
    theProgram->Release (myContext);
    theShareKey.Clear();
    theProgram.Nullify();
    return Standard_False;
  }

  myProgramList.Append (theProgram);
  myContext->ShareResource (theShareKey, theProgram);
  return Standard_True;
}

// =======================================================================
// function : Unregister
// purpose  : Removes specified shader program from the manager
// =======================================================================
void OpenGl_ShaderManager::Unregister (TCollection_AsciiString&      theShareKey,
                                       Handle(OpenGl_ShaderProgram)& theProgram)
{
  for (OpenGl_ShaderProgramList::Iterator anIt (myProgramList); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theProgram)
    {
      if (!theProgram->UnShare())
      {
        theShareKey.Clear();
        theProgram.Nullify();
        return;
      }

      myProgramList.Remove (anIt);
      break;
    }
  }

  const TCollection_AsciiString anID = theProgram->myProxy->GetId();
  if (anID.IsEmpty())
  {
    myContext->DelayedRelease (theProgram);
    theProgram.Nullify();
  }
  else
  {
    theProgram.Nullify();
    myContext->ReleaseResource (anID, Standard_True);
  }
}

// =======================================================================
// function : ShaderPrograms
// purpose  : Returns list of registered shader programs
// =======================================================================
const OpenGl_ShaderProgramList& OpenGl_ShaderManager::ShaderPrograms() const
{
  return myProgramList;
}

// =======================================================================
// function : Empty
// purpose  : Returns true if no program objects are attached
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::IsEmpty() const
{
  return myProgramList.IsEmpty();
}

// =======================================================================
// function : switchLightPrograms
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::switchLightPrograms()
{
  const Handle(Graphic3d_LightSet)& aLights = myLightSourceState.LightSources();
  if (aLights.IsNull())
  {
    if (!myMapOfLightPrograms.Find ("unlit", myLightPrograms))
    {
      myLightPrograms = new OpenGl_SetOfShaderPrograms (myUnlitPrograms);
      myMapOfLightPrograms.Bind ("unlit", myLightPrograms);
    }
    return;
  }

  const TCollection_AsciiString aKey = genLightKey (aLights);
  if (!myMapOfLightPrograms.Find (aKey, myLightPrograms))
  {
    myLightPrograms = new OpenGl_SetOfShaderPrograms();
    myMapOfLightPrograms.Bind (aKey, myLightPrograms);
  }
}

// =======================================================================
// function : UpdateSRgbState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::UpdateSRgbState()
{
  if (mySRgbState == myContext->ToRenderSRGB())
  {
    return;
  }

  mySRgbState = myContext->ToRenderSRGB();

  // special cases - GLSL programs dealing with sRGB/linearRGB internally
  myStereoPrograms[Graphic3d_StereoMode_Anaglyph].Nullify();
}

// =======================================================================
// function : UpdateLightSourceStateTo
// purpose  : Updates state of OCCT light sources
// =======================================================================
void OpenGl_ShaderManager::UpdateLightSourceStateTo (const Handle(Graphic3d_LightSet)& theLights,
                                                     Standard_Integer                  theSpecIBLMapLevels)
{
  myLightSourceState.Set (theLights);
  myLightSourceState.SetSpecIBLMapLevels (theSpecIBLMapLevels);
  myLightSourceState.Update();
  switchLightPrograms();
}

// =======================================================================
// function : UpdateLightSourceState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::UpdateLightSourceState()
{
  myLightSourceState.Update();
}

// =======================================================================
// function : SetShadingModel
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::SetShadingModel (const Graphic3d_TypeOfShadingModel theModel)
{
  if (theModel == Graphic3d_TOSM_DEFAULT)
  {
    throw Standard_ProgramError ("OpenGl_ShaderManager::SetShadingModel() - attempt to set invalid Shading Model!");
  }

  myShadingModel = theModel;
  switchLightPrograms();
}

// =======================================================================
// function : SetProjectionState
// purpose  : Sets new state of OCCT projection transform
// =======================================================================
void OpenGl_ShaderManager::UpdateProjectionStateTo (const OpenGl_Mat4& theProjectionMatrix)
{
  myProjectionState.Set (theProjectionMatrix);
  myProjectionState.Update();
}

// =======================================================================
// function : SetModelWorldState
// purpose  : Sets new state of OCCT model-world transform
// =======================================================================
void OpenGl_ShaderManager::UpdateModelWorldStateTo (const OpenGl_Mat4& theModelWorldMatrix)
{
  myModelWorldState.Set (theModelWorldMatrix);
  myModelWorldState.Update();
}

// =======================================================================
// function : SetWorldViewState
// purpose  : Sets new state of OCCT world-view transform
// =======================================================================
void OpenGl_ShaderManager::UpdateWorldViewStateTo (const OpenGl_Mat4& theWorldViewMatrix)
{
  myWorldViewState.Set (theWorldViewMatrix);
  myWorldViewState.Update();
}

// =======================================================================
// function : pushLightSourceState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushLightSourceState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  theProgram->UpdateState (OpenGl_LIGHT_SOURCES_STATE, myLightSourceState.Index());
  if (theProgram == myFfpProgram)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->core11 == NULL)
    {
      return;
    }

    GLenum aLightGlId = GL_LIGHT0;
    const OpenGl_Mat4 aModelView = myWorldViewState.WorldViewMatrix() * myModelWorldState.ModelWorldMatrix();
    for (Graphic3d_LightSet::Iterator aLightIt (myLightSourceState.LightSources(), Graphic3d_LightSet::IterationFilter_ExcludeDisabledAndAmbient);
         aLightIt.More(); aLightIt.Next())
    {
      if (aLightGlId > GL_LIGHT7) // only 8 lights in FFP...
      {
        myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                                "Warning: light sources limit (8) has been exceeded within Fixed-function pipeline.");
        continue;
      }

      bindLight (*aLightIt.Value(), aLightGlId, aModelView, myContext);
      ++aLightGlId;
    }

    // apply accumulated ambient color
    const Graphic3d_Vec4 anAmbient = !myLightSourceState.LightSources().IsNull()
                                    ? myLightSourceState.LightSources()->AmbientColor()
                                    : Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 1.0f);
    myContext->core11->glLightModelfv (GL_LIGHT_MODEL_AMBIENT, anAmbient.GetData());

    // GL_LIGHTING is managed by drawers to switch between shaded / no lighting output,
    // therefore managing the state here does not have any effect - do it just for consistency.
    if (aLightGlId != GL_LIGHT0)
    {
      ::glEnable (GL_LIGHTING);
    }
    else
    {
      ::glDisable (GL_LIGHTING);
    }
    // switch off unused lights
    for (; aLightGlId <= GL_LIGHT7; ++aLightGlId)
    {
      ::glDisable (aLightGlId);
    }
  #endif
    return;
  }

  const Standard_Integer aNbLightsMax = theProgram->NbLightsMax();
  const GLint anAmbientLoc = theProgram->GetStateLocation (OpenGl_OCC_LIGHT_AMBIENT);
  if (aNbLightsMax == 0
   && anAmbientLoc == OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    return;
  }

  if (myLightTypeArray.Size() < aNbLightsMax)
  {
    myLightTypeArray  .Resize (0, aNbLightsMax - 1, false);
    myLightParamsArray.Resize (0, aNbLightsMax - 1, false);
  }
  for (Standard_Integer aLightIt = 0; aLightIt < aNbLightsMax; ++aLightIt)
  {
    myLightTypeArray.SetValue (aLightIt, -1);
  }

  if (myLightSourceState.LightSources().IsNull()
   || myLightSourceState.LightSources()->IsEmpty())
  {
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_COUNT),
                            0);
    theProgram->SetUniform (myContext,
                            anAmbientLoc,
                            OpenGl_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_TYPES),
                            aNbLightsMax,
                            &myLightTypeArray.First());
    return;
  }

  Standard_Integer aLightsNb = 0;
  for (Graphic3d_LightSet::Iterator anIter (myLightSourceState.LightSources(), Graphic3d_LightSet::IterationFilter_ExcludeDisabledAndAmbient);
       anIter.More(); anIter.Next())
  {
    const Graphic3d_CLight& aLight = *anIter.Value();
    if (aLightsNb >= aNbLightsMax)
    {
      if (aNbLightsMax != 0)
      {
        myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                                TCollection_AsciiString("Warning: light sources limit (") + aNbLightsMax + ") has been exceeded.");
      }
      continue;
    }

    Standard_Integer&             aLightType   = myLightTypeArray  .ChangeValue (aLightsNb);
    OpenGl_ShaderLightParameters& aLightParams = myLightParamsArray.ChangeValue (aLightsNb);
    if (!aLight.IsEnabled()) // has no affect with Graphic3d_LightSet::IterationFilter_ExcludeDisabled - here just for consistency
    {
      // if it is desired to keep disabled light in the same order - we can replace it with a black light so that it will have no influence on result
      aLightType = -1; // Graphic3d_TOLS_AMBIENT can be used instead
      aLightParams.Color = OpenGl_Vec4 (0.0f, 0.0f, 0.0f, 0.0f);
      ++aLightsNb;
      continue;
    }

    // ignoring OpenGl_Context::ToRenderSRGB() for light colors,
    // as non-absolute colors for lights are rare and require tuning anyway
    aLightType = aLight.Type();
    aLightParams.Color     = aLight.PackedColor();
    aLightParams.Color.a() = aLight.Intensity(); // used by PBR and ignored by old shading model
    if (aLight.Type() == Graphic3d_TOLS_DIRECTIONAL)
    {
      aLightParams.Position = -aLight.PackedDirectionRange();
    }
    else if (!aLight.IsHeadlight())
    {
      aLightParams.Position.x() = static_cast<float>(aLight.Position().X() - myLocalOrigin.X());
      aLightParams.Position.y() = static_cast<float>(aLight.Position().Y() - myLocalOrigin.Y());
      aLightParams.Position.z() = static_cast<float>(aLight.Position().Z() - myLocalOrigin.Z());
    }
    else
    {
      aLightParams.Position.x() = static_cast<float>(aLight.Position().X());
      aLightParams.Position.y() = static_cast<float>(aLight.Position().Y());
      aLightParams.Position.z() = static_cast<float>(aLight.Position().Z());
    }
    aLightParams.Position.w() = aLight.IsHeadlight() ? 1.0f : 0.0f;

    if (aLight.Type() == Graphic3d_TOLS_SPOT)
    {
      aLightParams.Direction = aLight.PackedDirectionRange();
    }
    if (aLight.Type() == Graphic3d_TOLS_POSITIONAL)
    {
      aLightParams.Direction.w() = aLight.Range();
    }
    aLightParams.Parameters = aLight.PackedParams();
    ++aLightsNb;
  }

  const Graphic3d_Vec4& anAmbient = myLightSourceState.LightSources()->AmbientColor();
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_COUNT),
                          aLightsNb);
  theProgram->SetUniform (myContext,
                          anAmbientLoc,
                          anAmbient);
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_TYPES),
                          aNbLightsMax,
                          &myLightTypeArray.First());
  if (aLightsNb > 0)
  {
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_PARAMS),
                            aLightsNb * OpenGl_ShaderLightParameters::NbOfVec4(),
                            myLightParamsArray.First().Packed());
  }

  if (const OpenGl_ShaderUniformLocation aLocation = theProgram->GetStateLocation (OpenGl_OCCT_NB_SPEC_IBL_LEVELS))
  {
    theProgram->SetUniform (myContext, aLocation, myLightSourceState.SpecIBLMapLevels());
  }
}

// =======================================================================
// function : pushProjectionState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushProjectionState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  theProgram->UpdateState (OpenGl_PROJECTION_STATE, myProjectionState.Index());
  if (theProgram == myFfpProgram)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->core11 != NULL)
    {
      myContext->core11->glMatrixMode (GL_PROJECTION);
      myContext->core11->glLoadMatrixf (myProjectionState.ProjectionMatrix());
    }
  #endif
    return;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX),
                          myProjectionState.ProjectionMatrix());

  GLint aLocation = theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX_INVERSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myProjectionState.ProjectionMatrixInverse());
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX_TRANSPOSE),
                          myProjectionState.ProjectionMatrix(), true);

  aLocation = theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX_INVERSE_TRANSPOSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myProjectionState.ProjectionMatrixInverse(), true);
  }
}

// =======================================================================
// function : pushModelWorldState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushModelWorldState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  theProgram->UpdateState (OpenGl_MODEL_WORLD_STATE, myModelWorldState.Index());
  if (theProgram == myFfpProgram)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->core11 != NULL)
    {
      const OpenGl_Mat4 aModelView = myWorldViewState.WorldViewMatrix() * myModelWorldState.ModelWorldMatrix();
      myContext->core11->glMatrixMode (GL_MODELVIEW);
      myContext->core11->glLoadMatrixf (aModelView.GetData());
      theProgram->UpdateState (OpenGl_WORLD_VIEW_STATE, myWorldViewState.Index());
    }
  #endif
    return;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX),
                          myModelWorldState.ModelWorldMatrix());

  GLint aLocation = theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myModelWorldState.ModelWorldMatrixInverse());
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX_TRANSPOSE),
                          myModelWorldState.ModelWorldMatrix(), true);

  aLocation = theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE_TRANSPOSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myModelWorldState.ModelWorldMatrixInverse(), true);
  }
}

// =======================================================================
// function : pushWorldViewState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushWorldViewState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myWorldViewState.Index() == theProgram->ActiveState (OpenGl_WORLD_VIEW_STATE))
  {
    return;
  }

  theProgram->UpdateState (OpenGl_WORLD_VIEW_STATE, myWorldViewState.Index());
  if (theProgram == myFfpProgram)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->core11 != NULL)
    {
      const OpenGl_Mat4 aModelView = myWorldViewState.WorldViewMatrix() * myModelWorldState.ModelWorldMatrix();
      myContext->core11->glMatrixMode (GL_MODELVIEW);
      myContext->core11->glLoadMatrixf (aModelView.GetData());
      theProgram->UpdateState (OpenGl_MODEL_WORLD_STATE, myModelWorldState.Index());
    }
  #endif
    return;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX),
                          myWorldViewState.WorldViewMatrix());

  GLint aLocation = theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myWorldViewState.WorldViewMatrixInverse());
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX_TRANSPOSE),
                          myWorldViewState.WorldViewMatrix(), true);

  aLocation = theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE_TRANSPOSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myWorldViewState.WorldViewMatrixInverse(), true);
  }
}

// =======================================================================
// function : UpdateClippingState
// purpose  : Updates state of OCCT clipping planes
// =======================================================================
void OpenGl_ShaderManager::UpdateClippingState()
{
  myClippingState.Update();
}

// =======================================================================
// function : RevertClippingState
// purpose  : Reverts state of OCCT clipping planes
// =======================================================================
void OpenGl_ShaderManager::RevertClippingState()
{
  myClippingState.Revert();
}

// =======================================================================
// function : pushClippingState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushClippingState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  theProgram->UpdateState (OpenGl_CLIP_PLANES_STATE, myClippingState.Index());
  if (theProgram == myFfpProgram)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->core11 == NULL)
    {
      return;
    }

    const Standard_Integer aNbMaxPlanes = myContext->MaxClipPlanes();
    if (myClipPlaneArrayFfp.Size() < aNbMaxPlanes)
    {
      myClipPlaneArrayFfp.Resize (0, aNbMaxPlanes - 1, false);
    }

    Standard_Integer aPlaneId = 0;
    Standard_Boolean toRestoreModelView = Standard_False;
    const Handle(Graphic3d_ClipPlane)& aCappedChain = myContext->Clipping().CappedChain();
    for (OpenGl_ClippingIterator aPlaneIter (myContext->Clipping()); aPlaneIter.More(); aPlaneIter.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIter.Value();
      if (aPlaneIter.IsDisabled()
       || aPlane->IsChain()
       || (aPlane == aCappedChain
        && myContext->Clipping().IsCappingEnableAllExcept()))
      {
        continue;
      }
      else if (aPlaneId >= aNbMaxPlanes)
      {
        myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                                TCollection_ExtendedString("Warning: clipping planes limit (") + aNbMaxPlanes + ") has been exceeded.");
        break;
      }

      const Graphic3d_ClipPlane::Equation& anEquation = aPlane->GetEquation();
      OpenGl_Vec4d& aPlaneEq = myClipPlaneArrayFfp.ChangeValue (aPlaneId);
      aPlaneEq.x() = anEquation.x();
      aPlaneEq.y() = anEquation.y();
      aPlaneEq.z() = anEquation.z();
      aPlaneEq.w() = anEquation.w();
      if (myHasLocalOrigin)
      {
        const gp_XYZ        aPos = aPlane->ToPlane().Position().Location().XYZ() - myLocalOrigin;
        const Standard_Real aD   = -(anEquation.x() * aPos.X() + anEquation.y() * aPos.Y() + anEquation.z() * aPos.Z());
        aPlaneEq.w() = aD;
      }

      const GLenum anFfpPlaneID = GL_CLIP_PLANE0 + aPlaneId;
      if (anFfpPlaneID == GL_CLIP_PLANE0)
      {
        // set either identity or pure view matrix
        toRestoreModelView = Standard_True;
        myContext->core11->glMatrixMode (GL_MODELVIEW);
        myContext->core11->glLoadMatrixf (myWorldViewState.WorldViewMatrix().GetData());
      }

      ::glEnable (anFfpPlaneID);
      myContext->core11->glClipPlane (anFfpPlaneID, aPlaneEq);

      ++aPlaneId;
    }

    // switch off unused lights
    for (; aPlaneId < aNbMaxPlanes; ++aPlaneId)
    {
      ::glDisable (GL_CLIP_PLANE0 + aPlaneId);
    }

    // restore combined model-view matrix
    if (toRestoreModelView)
    {
      const OpenGl_Mat4 aModelView = myWorldViewState.WorldViewMatrix() * myModelWorldState.ModelWorldMatrix();
      myContext->core11->glLoadMatrixf (aModelView.GetData());
    }
  #endif
    return;
  }

  const GLint aLocEquations = theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_EQUATIONS);
  if (aLocEquations == OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    return;
  }

  const Standard_Integer aNbClipPlanesMax = theProgram->NbClipPlanesMax();
  const Standard_Integer aNbPlanes = Min (myContext->Clipping().NbClippingOrCappingOn(), aNbClipPlanesMax);
  if (aNbPlanes < 1)
  {
    theProgram->SetUniform (myContext, theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_COUNT), 0);
    return;
  }

  if (myClipPlaneArray.Size() < aNbClipPlanesMax)
  {
    myClipPlaneArray.Resize (0, aNbClipPlanesMax - 1, false);
    myClipChainArray.Resize (0, aNbClipPlanesMax - 1, false);
  }

  Standard_Integer aPlaneId = 0;
  const Handle(Graphic3d_ClipPlane)& aCappedChain = myContext->Clipping().CappedChain();
  for (OpenGl_ClippingIterator aPlaneIter (myContext->Clipping()); aPlaneIter.More(); aPlaneIter.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIter.Value();
    if (aPlaneIter.IsDisabled())
    {
      continue;
    }

    if (myContext->Clipping().IsCappingDisableAllExcept())
    {
      // enable only specific (sub) plane
      if (aPlane != aCappedChain)
      {
        continue;
      }

      Standard_Integer aSubPlaneIndex = 1;
      for (const Graphic3d_ClipPlane* aSubPlaneIter = aCappedChain.get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->ChainNextPlane().get(), ++aSubPlaneIndex)
      {
        if (aSubPlaneIndex == myContext->Clipping().CappedSubPlane())
        {
          addClippingPlane (aPlaneId, *aSubPlaneIter, aSubPlaneIter->GetEquation(), 1);
          break;
        }
      }
      break;
    }
    else if (aPlane == aCappedChain) // && myContext->Clipping().IsCappingEnableAllExcept()
    {
      // enable sub-planes within processed Chain as reversed and ORed, excluding filtered plane
      if (aPlaneId + aPlane->NbChainNextPlanes() - 1 > aNbClipPlanesMax)
      {
        myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                                TCollection_AsciiString("Error: clipping planes limit (") + aNbClipPlanesMax + ") has been exceeded.");
        break;
      }

      Standard_Integer aSubPlaneIndex = 1;
      for (const Graphic3d_ClipPlane* aSubPlaneIter = aPlane.get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->ChainNextPlane().get(), ++aSubPlaneIndex)
      {
        if (aSubPlaneIndex != -myContext->Clipping().CappedSubPlane())
        {
          addClippingPlane (aPlaneId, *aSubPlaneIter, aSubPlaneIter->ReversedEquation(), 1);
        }
      }
    }
    else
    {
      // normal case
      if (aPlaneId + aPlane->NbChainNextPlanes() > aNbClipPlanesMax)
      {
        myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                                TCollection_AsciiString("Error: clipping planes limit (") + aNbClipPlanesMax + ") has been exceeded.");
        break;
      }
      for (const Graphic3d_ClipPlane* aSubPlaneIter = aPlane.get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->ChainNextPlane().get())
      {
        addClippingPlane (aPlaneId, *aSubPlaneIter, aSubPlaneIter->GetEquation(), aSubPlaneIter->NbChainNextPlanes());
      }
    }
  }

  theProgram->SetUniform (myContext, theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_COUNT), aPlaneId);
  theProgram->SetUniform (myContext, aLocEquations, aNbClipPlanesMax, &myClipPlaneArray.First());
  theProgram->SetUniform (myContext, theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_CHAINS), aNbClipPlanesMax, &myClipChainArray.First());
}

// =======================================================================
// function : pushMaterialState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushMaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  const OpenGl_Material& aFrontMat = myMaterialState.FrontMaterial();
  const OpenGl_Material& aBackMat  = myMaterialState.BackMaterial();
  theProgram->UpdateState (OpenGl_MATERIAL_STATE, myMaterialState.Index());
  if (theProgram == myFfpProgram)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->core11 == NULL)
    {
      return;
    }

    if (myMaterialState.AlphaCutoff() < ShortRealLast())
    {
      glAlphaFunc (GL_GEQUAL, myMaterialState.AlphaCutoff());
      glEnable (GL_ALPHA_TEST);
    }
    else
    {
      glDisable (GL_ALPHA_TEST);
    }

    const GLenum aFrontFace = myMaterialState.ToDistinguish() ? GL_FRONT : GL_FRONT_AND_BACK;
    myContext->core11->glMaterialfv(aFrontFace, GL_AMBIENT,   aFrontMat.Common.Ambient.GetData());
    myContext->core11->glMaterialfv(aFrontFace, GL_DIFFUSE,   aFrontMat.Common.Diffuse.GetData());
    myContext->core11->glMaterialfv(aFrontFace, GL_SPECULAR,  aFrontMat.Common.Specular.GetData());
    myContext->core11->glMaterialfv(aFrontFace, GL_EMISSION,  aFrontMat.Common.Emission.GetData());
    myContext->core11->glMaterialf (aFrontFace, GL_SHININESS, aFrontMat.Common.Shine());
    if (myMaterialState.ToDistinguish())
    {
      myContext->core11->glMaterialfv(GL_BACK, GL_AMBIENT,   aBackMat.Common.Ambient.GetData());
      myContext->core11->glMaterialfv(GL_BACK, GL_DIFFUSE,   aBackMat.Common.Diffuse.GetData());
      myContext->core11->glMaterialfv(GL_BACK, GL_SPECULAR,  aBackMat.Common.Specular.GetData());
      myContext->core11->glMaterialfv(GL_BACK, GL_EMISSION,  aBackMat.Common.Emission.GetData());
      myContext->core11->glMaterialf (GL_BACK, GL_SHININESS, aBackMat.Common.Shine());
    }
  #endif
    return;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCCT_ALPHA_CUTOFF),
                          myMaterialState.AlphaCutoff());
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCCT_TEXTURE_ENABLE),
                          myMaterialState.ToMapTexture()  ? 1 : 0);
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCCT_DISTINGUISH_MODE),
                          myMaterialState.ToDistinguish() ? 1 : 0);

  if (const OpenGl_ShaderUniformLocation& aLocPbrFront = theProgram->GetStateLocation (OpenGl_OCCT_PBR_FRONT_MATERIAL))
  {
    theProgram->SetUniform (myContext, aLocPbrFront, OpenGl_MaterialPBR::NbOfVec4(),
                            aFrontMat.Pbr.Packed());
  }
  if (const OpenGl_ShaderUniformLocation aLocPbrBack = theProgram->GetStateLocation (OpenGl_OCCT_PBR_BACK_MATERIAL))
  {
    theProgram->SetUniform (myContext, aLocPbrBack,  OpenGl_MaterialPBR::NbOfVec4(),
                            aBackMat.Pbr.Packed());
  }
  if (const OpenGl_ShaderUniformLocation aLocFront = theProgram->GetStateLocation (OpenGl_OCCT_COMMON_FRONT_MATERIAL))
  {
    theProgram->SetUniform (myContext, aLocFront, OpenGl_MaterialCommon::NbOfVec4(),
                            aFrontMat.Common.Packed());
  }
  if (const OpenGl_ShaderUniformLocation aLocBack = theProgram->GetStateLocation (OpenGl_OCCT_COMMON_BACK_MATERIAL))
  {
    theProgram->SetUniform (myContext, aLocBack,  OpenGl_MaterialCommon::NbOfVec4(),
                            aBackMat.Common.Packed());
  }
}

// =======================================================================
// function : pushOitState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushOitState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  const GLint aLocOutput = theProgram->GetStateLocation (OpenGl_OCCT_OIT_OUTPUT);
  if (aLocOutput != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocOutput, myOitState.ToEnableWrite());
  }

  const GLint aLocDepthFactor = theProgram->GetStateLocation (OpenGl_OCCT_OIT_DEPTH_FACTOR);
  if (aLocDepthFactor != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocDepthFactor, myOitState.DepthFactor());
  }
}

// =======================================================================
// function : PushInteriorState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::PushInteriorState (const Handle(OpenGl_ShaderProgram)& theProgram,
                                              const Handle(Graphic3d_Aspects)& theAspect) const
{
  if (theProgram.IsNull()
  || !theProgram->IsValid())
  {
    return;
  }

  if (const OpenGl_ShaderUniformLocation aLocLineWidth = theProgram->GetStateLocation (OpenGl_OCCT_LINE_WIDTH))
  {
    theProgram->SetUniform (myContext, aLocLineWidth, theAspect->EdgeWidth() * myContext->LineWidthScale());
    theProgram->SetUniform (myContext, theProgram->GetStateLocation (OpenGl_OCCT_LINE_FEATHER), myContext->LineFeather() * myContext->LineWidthScale());
  }
  if (const OpenGl_ShaderUniformLocation aLocWireframeColor = theProgram->GetStateLocation (OpenGl_OCCT_WIREFRAME_COLOR))
  {
    if (theAspect->InteriorStyle() == Aspect_IS_HOLLOW)
    {
      theProgram->SetUniform (myContext, aLocWireframeColor, OpenGl_Vec4 (-1.0f, -1.0f, -1.0f, -1.0f));
    }
    else
    {
      theProgram->SetUniform (myContext, aLocWireframeColor, myContext->Vec4FromQuantityColor (theAspect->EdgeColorRGBA()));
    }
  }
  if (const OpenGl_ShaderUniformLocation aLocQuadModeState = theProgram->GetStateLocation (OpenGl_OCCT_QUAD_MODE_STATE))
  {
    theProgram->SetUniform (myContext, aLocQuadModeState, theAspect->ToSkipFirstEdge() ? 1 : 0);
  }
}

// =======================================================================
// function : PushState
// purpose  : Pushes state of OCCT graphics parameters to the program
// =======================================================================
void OpenGl_ShaderManager::PushState (const Handle(OpenGl_ShaderProgram)& theProgram,
                                      Graphic3d_TypeOfShadingModel theShadingModel) const
{
  const Handle(OpenGl_ShaderProgram)& aProgram = !theProgram.IsNull() ? theProgram : myFfpProgram;
  PushClippingState    (aProgram);
  PushWorldViewState   (aProgram);
  PushModelWorldState  (aProgram);
  PushProjectionState  (aProgram);
  PushLightSourceState (aProgram);
  PushMaterialState    (aProgram);
  PushOitState         (aProgram);

  if (!theProgram.IsNull())
  {
    if (const OpenGl_ShaderUniformLocation& aLocViewPort = theProgram->GetStateLocation (OpenGl_OCCT_VIEWPORT))
    {
      theProgram->SetUniform (myContext, aLocViewPort, OpenGl_Vec4 ((float )myContext->Viewport()[0], (float )myContext->Viewport()[1],
                                                                    (float )myContext->Viewport()[2], (float )myContext->Viewport()[3]));
    }
  }
#if !defined(GL_ES_VERSION_2_0)
  else if (myContext->core11 != NULL)
  {
    // manage FFP lighting
    myContext->SetShadeModel (theShadingModel);
    if (theShadingModel == Graphic3d_TOSM_UNLIT)
    {
      glDisable (GL_LIGHTING);
    }
    else
    {
      glEnable (GL_LIGHTING);
    }
  }
#else
  (void )theShadingModel;
#endif
}

// =======================================================================
// function : prepareStdProgramFont
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFont()
{
  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aUniforms   .Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
    + EOL"void main()"
      EOL"{"
      EOL"  TexCoord = occTexCoord.st;"
    + THE_VERT_gl_Position
    + EOL"}";

  TCollection_AsciiString
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st).a; }";
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core11 == NULL)
  {
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st).r; }";
  }
#endif

  TCollection_AsciiString aSrcFrag =
       aSrcGetAlpha
     + EOL"void main()"
       EOL"{"
       EOL"  vec4 aColor = occColor;"
       EOL"  aColor.a *= getAlpha();"
       EOL"  if (aColor.a <= 0.285) discard;"
       EOL"  occSetFragColor (aColor);"
       EOL"}";

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  defaultGlslVersion (aProgramSrc, "font", 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myFontProgram))
  {
    myFontProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : BindFboBlitProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::BindFboBlitProgram (Standard_Integer theNbSamples,
                                                           Standard_Boolean theIsFallback_sRGB)
{
  NCollection_Array1<Handle(OpenGl_ShaderProgram)>& aList = myBlitPrograms[theIsFallback_sRGB ? 1 : 0];
  Standard_Integer aNbSamples = Max (theNbSamples, 1);
  if (aNbSamples > aList.Upper())
  {
    aList.Resize (1, aNbSamples, true);
  }

  Handle(OpenGl_ShaderProgram)& aProg = aList[aNbSamples];
  if (aProg.IsNull())
  {
    prepareStdProgramFboBlit (aProg, aNbSamples, theIsFallback_sRGB);
  }
  return !aProg.IsNull()
       && myContext->BindProgram (aProg);
}

// =======================================================================
// function : prepareStdProgramFboBlit
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFboBlit (Handle(OpenGl_ShaderProgram)& theProgram,
                                                                 Standard_Integer theNbSamples,
                                                                 Standard_Boolean theIsFallback_sRGB)
{
  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  TCollection_AsciiString aSrcVert =
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag;
  if (theNbSamples > 1)
  {
  #if defined(GL_ES_VERSION_2_0)
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("highp sampler2DMS uColorSampler", Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("highp sampler2DMS uDepthSampler", Graphic3d_TOS_FRAGMENT));
  #else
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2DMS uColorSampler", Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2DMS uDepthSampler", Graphic3d_TOS_FRAGMENT));
  #endif
    aSrcFrag = TCollection_AsciiString()
    + EOL"#define THE_NUM_SAMPLES " + theNbSamples
    + (theIsFallback_sRGB ? EOL"#define THE_SHIFT_sRGB" : "")
    + EOL"void main()"
      EOL"{"
      EOL"  ivec2 aSize  = textureSize (uColorSampler);"
      EOL"  ivec2 anUV   = ivec2 (vec2 (aSize) * TexCoord);"
      EOL"  gl_FragDepth = texelFetch (uDepthSampler, anUV, THE_NUM_SAMPLES / 2 - 1).r;"
      EOL
      EOL"  vec4 aColor = vec4 (0.0);"
      EOL"  for (int aSample = 0; aSample < THE_NUM_SAMPLES; ++aSample)"
      EOL"  {"
      EOL"    vec4 aVal = texelFetch (uColorSampler, anUV, aSample);"
      EOL"    aColor += aVal;"
      EOL"  }"
      EOL"  aColor /= float(THE_NUM_SAMPLES);"
      EOL"#ifdef THE_SHIFT_sRGB"
      EOL"  aColor.rgb = pow (aColor.rgb, vec3 (1.0 / 2.2));"
      EOL"#endif"
      EOL"  occSetFragColor (aColor);"
      EOL"}";
  }
  else
  {
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D uColorSampler", Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D uDepthSampler", Graphic3d_TOS_FRAGMENT));
    aSrcFrag = TCollection_AsciiString()
    + (theIsFallback_sRGB ? EOL"#define THE_SHIFT_sRGB" : "")
    + EOL"void main()"
      EOL"{"
      EOL"  gl_FragDepth = occTexture2D (uDepthSampler, TexCoord).r;"
      EOL"  vec4  aColor = occTexture2D (uColorSampler, TexCoord);"
      EOL"#ifdef THE_SHIFT_sRGB"
      EOL"  aColor.rgb = pow (aColor.rgb, vec3 (1.0 / 2.2));"
      EOL"#endif"
      EOL"  occSetFragColor (aColor);"
      EOL"}";
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
#if defined(GL_ES_VERSION_2_0)
  if (myContext->IsGlGreaterEqual (3, 1))
  {
    // required for MSAA sampler
    aProgramSrc->SetHeader ("#version 310 es");
  }
  else if (myContext->IsGlGreaterEqual (3, 0))
  {
    aProgramSrc->SetHeader ("#version 300 es");
  }
  else if (myContext->extFragDepth)
  {
    aProgramSrc->SetHeader ("#extension GL_EXT_frag_depth : enable"
                         EOL"#define gl_FragDepth gl_FragDepthEXT");
  }
  else
  {
    // there is no way to draw into depth buffer
    aSrcFrag =
      EOL"void main()"
      EOL"{"
      EOL"  occSetFragColor (occTexture2D (uColorSampler, TexCoord));"
      EOL"}";
  }
#else
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
  TCollection_AsciiString anId = "occt_blit";
  if (theNbSamples > 1)
  {
    anId += TCollection_AsciiString ("_msaa") + theNbSamples;
  }
  if (theIsFallback_sRGB)
  {
    anId += "_gamma";
  }
  aProgramSrc->SetId (anId);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (theProgram);
  theProgram->SetSampler (myContext, "uColorSampler", Graphic3d_TextureUnit_0);
  theProgram->SetSampler (myContext, "uDepthSampler", Graphic3d_TextureUnit_1);
  myContext->BindProgram (NULL);
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramOitCompositing
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramOitCompositing (const Standard_Boolean theMsaa)
{
  Handle(OpenGl_ShaderProgram)& aProgram = myOitCompositingProgram[theMsaa ? 1 : 0];
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcFrag;

  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  aSrcVert =
    EOL"void main()"
    EOL"{"
    EOL"  TexCoord    = occVertex.zw;"
    EOL"  gl_Position = vec4 (occVertex.x, occVertex.y, 0.0, 1.0);"
    EOL"}";

  if (!theMsaa)
  {
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D uAccumTexture",  Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D uWeightTexture", Graphic3d_TOS_FRAGMENT));
    aSrcFrag =
      EOL"void main()"
      EOL"{"
      EOL"  vec4 aAccum   = occTexture2D (uAccumTexture,  TexCoord);"
      EOL"  float aWeight = occTexture2D (uWeightTexture, TexCoord).r;"
      EOL"  occSetFragColor (vec4 (aAccum.rgb / max (aWeight, 0.00001), aAccum.a));"
      EOL"}";
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->IsGlGreaterEqual (3, 2))
    {
      aProgramSrc->SetHeader ("#version 150");
    }
  #else
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es");
    }
  #endif
  }
  else
  {
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2DMS uAccumTexture",  Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2DMS uWeightTexture", Graphic3d_TOS_FRAGMENT));
    aSrcFrag =
      EOL"void main()"
      EOL"{"
      EOL"  ivec2 aTexel  = ivec2 (vec2 (textureSize (uAccumTexture)) * TexCoord);"
      EOL"  vec4 aAccum   = texelFetch (uAccumTexture,  aTexel, gl_SampleID);"
      EOL"  float aWeight = texelFetch (uWeightTexture, aTexel, gl_SampleID).r;"
      EOL"  occSetFragColor (vec4 (aAccum.rgb / max (aWeight, 0.00001), aAccum.a));"
      EOL"}";
  #if !defined(GL_ES_VERSION_2_0)
    if (myContext->IsGlGreaterEqual (4, 0))
    {
      aProgramSrc->SetHeader ("#version 400");
    }
  #else
    if (myContext->IsGlGreaterEqual (3, 2))
    {
      aProgramSrc->SetHeader ("#version 320 es");
    }
    else if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es"); // with GL_OES_sample_variables extension
    }
  #endif
  }

  aProgramSrc->SetId (theMsaa ? "occt_weight-oit-msaa" : "occt_weight-oit");
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, aProgram))
  {
    aProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (aProgram);
  aProgram->SetSampler (myContext, "uAccumTexture",  Graphic3d_TextureUnit_0);
  aProgram->SetSampler (myContext, "uWeightTexture", Graphic3d_TextureUnit_1);
  myContext->BindProgram (Handle(OpenGl_ShaderProgram)());
  return Standard_True;
}

// =======================================================================
// function : pointSpriteAlphaSrc
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::pointSpriteAlphaSrc (Standard_Integer theBits)
{
  TCollection_AsciiString aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord ").a; }";
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core11 == NULL
   && (theBits & OpenGl_PO_PointSpriteA) == OpenGl_PO_PointSpriteA)
  {
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord ").r; }";
  }
#else
  (void )theBits;
#endif
  return aSrcGetAlpha;
}

// =======================================================================
// function : defaultGlslVersion
// purpose  :
// =======================================================================
int OpenGl_ShaderManager::defaultGlslVersion (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                              const TCollection_AsciiString& theName,
                                              int theBits,
                                              bool theUsesDerivates) const
{
  int aBits = theBits;
  const bool toUseDerivates = theUsesDerivates
                          || (theBits & OpenGl_PO_StippleLine) != 0
                          || (theBits & OpenGl_PO_HasTextures) == OpenGl_PO_TextureNormal;
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    theProgram->SetHeader ("#version 150");
  }
  else
  {
    if ((theBits & OpenGl_PO_StippleLine) != 0
     || theProgram->IsPBR())
    {
      if (myContext->IsGlGreaterEqual (3, 0))
      {
        theProgram->SetHeader ("#version 130");
      }
      else if (myContext->CheckExtension ("GL_EXT_gpu_shader4")) // myContext->hasGlslBitwiseOps == OpenGl_FeatureInExtensions
      {
        // GL_EXT_gpu_shader4 defines GLSL type "unsigned int", while core GLSL specs define type "uint"
        theProgram->SetHeader ("#extension GL_EXT_gpu_shader4 : enable\n"
                               "#define uint unsigned int");
      }
    }
  }
  (void )toUseDerivates;
#else

#if defined(__EMSCRIPTEN__)
  if (myContext->IsGlGreaterEqual (3, 0))
  {
    // consider this is browser responsibility to provide working WebGL 2.0 implementation
    // and black-list broken drivers (there is no OpenGL ES greater than 3.0)
    theProgram->SetHeader ("#version 300 es");
  }
#endif
  // prefer "100 es" on OpenGL ES 3.0- devices (save the features unavailable before "300 es")
  // and    "300 es" on OpenGL ES 3.1+ devices
  if (myContext->IsGlGreaterEqual (3, 1))
  {
    if ((theBits & OpenGl_PO_NeedsGeomShader) != 0)
    {
      theProgram->SetHeader (myContext->hasGeometryStage != OpenGl_FeatureInExtensions ? "#version 320 es" : "#version 310 es");
    }
    else
    {
      theProgram->SetHeader ("#version 300 es");
    }
  }
  else
  {
    if (theProgram->IsPBR()
     && myContext->IsGlGreaterEqual (3, 0))
    {
      theProgram->SetHeader ("#version 300 es");
    }
    if ((theBits & OpenGl_PO_WriteOit) != 0
     || (theBits & OpenGl_PO_StippleLine) != 0)
    {
      if (myContext->IsGlGreaterEqual (3, 0))
      {
        theProgram->SetHeader ("#version 300 es");
      }
      else
      {
        aBits = aBits & ~OpenGl_PO_WriteOit;
        if (!myContext->oesStdDerivatives)
        {
          aBits = aBits & ~OpenGl_PO_StippleLine;
        }
      }
    }
    if (toUseDerivates)
    {
      if (myContext->IsGlGreaterEqual (3, 0))
      {
        theProgram->SetHeader ("#version 300 es");
      }
      else if (myContext->oesStdDerivatives)
      {
        theProgram->SetHeader ("#extension GL_OES_standard_derivatives : enable");
      }
    }
  }
#endif

  // should fit OpenGl_PO_NB
  char aBitsStr[64];
  Sprintf (aBitsStr, "%04x", aBits);
  theProgram->SetId (TCollection_AsciiString ("occt_") + theName + aBitsStr);
  return aBits;
}

// =======================================================================
// function : prepareGeomMainSrc
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::prepareGeomMainSrc (OpenGl_ShaderObject::ShaderVariableList& theUnifoms,
                                                                  OpenGl_ShaderObject::ShaderVariableList& theStageInOuts,
                                                                  Standard_Integer theBits)
{
  if ((theBits & OpenGl_PO_NeedsGeomShader) == 0)
  {
    return TCollection_AsciiString();
  }

  TCollection_AsciiString aSrcMainGeom =
    EOL"void main()"
    EOL"{";

  if ((theBits & OpenGl_PO_MeshEdges) != 0)
  {
    theUnifoms.Append    (OpenGl_ShaderObject::ShaderVariable ("vec4 occViewport",       Graphic3d_TOS_GEOMETRY));
    theUnifoms.Append    (OpenGl_ShaderObject::ShaderVariable ("bool occIsQuadMode",     Graphic3d_TOS_GEOMETRY));
    theUnifoms.Append    (OpenGl_ShaderObject::ShaderVariable ("float occLineWidth",     Graphic3d_TOS_GEOMETRY));
    theUnifoms.Append    (OpenGl_ShaderObject::ShaderVariable ("float occLineWidth",     Graphic3d_TOS_FRAGMENT));
    theUnifoms.Append    (OpenGl_ShaderObject::ShaderVariable ("float occLineFeather",   Graphic3d_TOS_FRAGMENT));
    theUnifoms.Append    (OpenGl_ShaderObject::ShaderVariable ("vec4 occWireframeColor", Graphic3d_TOS_FRAGMENT));
    theStageInOuts.Append(OpenGl_ShaderObject::ShaderVariable ("vec3 EdgeDistance",      Graphic3d_TOS_GEOMETRY | Graphic3d_TOS_FRAGMENT));

    aSrcMainGeom = TCollection_AsciiString()
    + EOL"vec3 ViewPortTransform (vec4 theVec)"
      EOL"{"
      EOL"  vec3 aWinCoord = theVec.xyz / theVec.w;"
      EOL"  aWinCoord    = aWinCoord * 0.5 + 0.5;"
      EOL"  aWinCoord.xy = aWinCoord.xy * occViewport.zw + occViewport.xy;"
      EOL"  return aWinCoord;"
      EOL"}"
    + aSrcMainGeom
    + EOL"  vec3 aSideA = ViewPortTransform (gl_in[2].gl_Position) - ViewPortTransform (gl_in[1].gl_Position);"
      EOL"  vec3 aSideB = ViewPortTransform (gl_in[2].gl_Position) - ViewPortTransform (gl_in[0].gl_Position);"
      EOL"  vec3 aSideC = ViewPortTransform (gl_in[1].gl_Position) - ViewPortTransform (gl_in[0].gl_Position);"
      EOL"  float aQuadArea = abs (aSideB.x * aSideC.y - aSideB.y * aSideC.x);"
      EOL"  vec3 aLenABC    = vec3 (length (aSideA), length (aSideB), length (aSideC));"
      EOL"  vec3 aHeightABC = vec3 (aQuadArea) / aLenABC;"
      EOL"  aHeightABC = max (aHeightABC, vec3 (10.0 * occLineWidth));" // avoid shrunk presentation disappearing at distance
      EOL"  float aQuadModeHeightC = occIsQuadMode ? occLineWidth + 1.0 : 0.0;";
  }

  for (Standard_Integer aVertIter = 0; aVertIter < 3; ++aVertIter)
  {
    const TCollection_AsciiString aVertIndex (aVertIter);
    // pass variables from Vertex shader to Fragment shader through Geometry shader
    for (OpenGl_ShaderObject::ShaderVariableList::Iterator aVarListIter (theStageInOuts); aVarListIter.More(); aVarListIter.Next())
    {
      if (aVarListIter.Value().Stages == (Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT))
      {
        const TCollection_AsciiString aVarName = aVarListIter.Value().Name.Token (" ", 2);
        aSrcMainGeom += TCollection_AsciiString()
         + EOL"  geomOut." + aVarName + " = geomIn[" + aVertIndex + "]." + aVarName + ";";
      }
    }

    if ((theBits & OpenGl_PO_MeshEdges) != 0)
    {
      switch (aVertIter)
      {
        case 0: aSrcMainGeom += EOL"  EdgeDistance = vec3 (aHeightABC[0], 0.0, aQuadModeHeightC);"; break;
        case 1: aSrcMainGeom += EOL"  EdgeDistance = vec3 (0.0, aHeightABC[1], aQuadModeHeightC);"; break;
        case 2: aSrcMainGeom += EOL"  EdgeDistance = vec3 (0.0, 0.0, aHeightABC[2]);"; break;
      }
    }
    aSrcMainGeom += TCollection_AsciiString()
     + EOL"  gl_Position = gl_in[" + aVertIndex + "].gl_Position;"
       EOL"  EmitVertex();";
  }
  aSrcMainGeom +=
    EOL"  EndPrimitive();"
    EOL"}";

  return aSrcMainGeom;
}

// =======================================================================
// function : prepareStdProgramUnlit
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramUnlit (Handle(OpenGl_ShaderProgram)& theProgram,
                                                               Standard_Integer theBits,
                                                               Standard_Boolean theIsOutline)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraMain, aSrcVertExtraFunc, aSrcGetAlpha, aSrcVertEndMain;
  TCollection_AsciiString aSrcFrag, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor     = EOL"vec4 getColor(void) { return occColor; }";
  TCollection_AsciiString aSrcFragMainGetColor = EOL"  occSetFragColor (getFinalColor());";
  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;

  if ((theBits & OpenGl_PO_IsPoint) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if ((theBits & OpenGl_PO_PointSprite) != 0)
    {
      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerPointSprite", Graphic3d_TOS_FRAGMENT));
      if ((theBits & OpenGl_PO_PointSpriteA) != OpenGl_PO_PointSpriteA)
      {
        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord "); }";
      }
      else if ((theBits & OpenGl_PO_TextureRGB) != 0
            && (theBits & OpenGl_PO_VertColor) == 0)
      {
        aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
        aUniforms   .Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
        aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
        aSrcVertExtraMain +=
          EOL"  VertColor = occTexture2D (occSamplerBaseColor, occTexCoord.xy);";
        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return VertColor; }";
      }

      aSrcGetAlpha = pointSpriteAlphaSrc (theBits);

    #if !defined(GL_ES_VERSION_2_0)
      if (myContext->core11 != NULL
        && myContext->IsGlGreaterEqual (2, 1))
      {
        aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
      }
    #endif

      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  aColor.a = getAlpha();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
    else
    {
      if ((theBits & OpenGl_PO_TextureRGB) != 0
       && (theBits & OpenGl_PO_VertColor) == 0)
      {
        aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
        aUniforms   .Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
        aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
        aSrcVertExtraMain +=
          EOL"  VertColor = occTexture2D (occSamplerBaseColor, occTexCoord.xy);";
        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return VertColor; }";
      }

      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_HasTextures) != 0)
    {
      aUniforms   .Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
      aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

      if ((theBits & OpenGl_PO_HasTextures) == OpenGl_PO_TextureEnv)
      {
        aSrcVertExtraFunc = THE_FUNC_transformNormal_view;

        aSrcVertExtraMain +=
          EOL"  vec4 aPosition = occWorldViewMatrix * occModelWorldMatrix * occVertex;"
          EOL"  vec3 aNormal   = transformNormal (occNormal);"
          EOL"  vec3 aReflect  = reflect (normalize (aPosition.xyz), aNormal);"
          EOL"  aReflect.z += 1.0;"
          EOL"  TexCoord = vec4(aReflect.xy * inversesqrt (dot (aReflect, aReflect)) * 0.5 + vec2 (0.5), 0.0, 1.0);";

        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return occTexture2D (occSamplerBaseColor, TexCoord.st); }";
      }
      else
      {
        aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
        aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st / TexCoord.w); }";
      }
    }
  }
  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain += EOL"  VertColor = occVertColor;";
    aSrcFragGetColor  =  EOL"vec4 getColor(void) { return VertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 PositionWorld", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 Position",      Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain +=
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  Position      = occWorldViewMatrix * PositionWorld;";

    if ((theBits & OpenGl_PO_ClipPlanesN) == OpenGl_PO_ClipPlanesN)
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
    {
      aNbClipPlanes = 1;
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes2) != 0)
    {
      aNbClipPlanes = 2;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_2
                         : THE_FRAG_CLIP_PLANES_2;
    }
  }
  if ((theBits & OpenGl_PO_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetWeightOitOutput (true);
  }

  if (theIsOutline)
  {
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("float occOrthoScale",          Graphic3d_TOS_VERTEX));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("float occSilhouetteThickness", Graphic3d_TOS_VERTEX));
    aSrcVertEndMain = THE_VERT_gl_Position_OUTLINE;
  }
  else if ((theBits & OpenGl_PO_StippleLine) != 0)
  {
    const Standard_Integer aBits = defaultGlslVersion (aProgramSrc, "unlit", theBits);
    if ((aBits & OpenGl_PO_StippleLine) != 0)
    {
      if (myContext->hasGlslBitwiseOps != OpenGl_FeatureNotAvailable)
      {
        aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("int   occStipplePattern", Graphic3d_TOS_FRAGMENT));
      }
      else
      {
        aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("bool  occStipplePattern[16]", Graphic3d_TOS_FRAGMENT));
      }
      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("float occStippleFactor",  Graphic3d_TOS_FRAGMENT));
      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 occViewport", Graphic3d_TOS_VERTEX));
      aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec2 ScreenSpaceCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
      aSrcVertEndMain =
        EOL"  vec2 aPosition   = gl_Position.xy / gl_Position.w;"
        EOL"  aPosition        = aPosition * 0.5 + 0.5;"
        EOL"  ScreenSpaceCoord = aPosition.xy * occViewport.zw + occViewport.xy;";
      aSrcFragMainGetColor = TCollection_AsciiString()
      + EOL"  vec2 anAxis = vec2 (0.0, 1.0);"
        EOL"  if (abs (dFdx (ScreenSpaceCoord.x)) - abs (dFdy (ScreenSpaceCoord.y)) > 0.001)" 
        EOL"  {"
        EOL"    anAxis = vec2 (1.0, 0.0);"
        EOL"  }"
        EOL"  float aRotatePoint = dot (gl_FragCoord.xy, anAxis);"
      + (myContext->hasGlslBitwiseOps != OpenGl_FeatureNotAvailable
       ? EOL"  uint aBit = uint (floor (aRotatePoint / occStippleFactor + 0.5)) & 15U;"
         EOL"  if ((uint (occStipplePattern) & (1U << aBit)) == 0U) discard;"
       : EOL"  int aBit = int (mod (floor (aRotatePoint / occStippleFactor + 0.5), 16.0));"
         EOL"  if (!occStipplePattern[aBit]) discard;")
      + EOL"  vec4 aColor = getFinalColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
    else
    {
      myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, "Warning: stipple lines in GLSL will be ignored.");
    }
  }

  aSrcVert =
      aSrcVertExtraFunc
    + EOL"void main()"
      EOL"{"
    + aSrcVertExtraMain
    + THE_VERT_gl_Position
    + aSrcVertEndMain
    + EOL"}";

  TCollection_AsciiString aSrcGeom = prepareGeomMainSrc (aUniforms, aStageInOuts, theBits);
  aSrcFragGetColor += (theBits & OpenGl_PO_MeshEdges) != 0
    ? THE_FRAG_WIREFRAME_COLOR
    : EOL"#define getFinalColor getColor";

  aSrcFrag =
      aSrcFragGetColor
    + aSrcGetAlpha
    + EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + aSrcFragMainGetColor
    + EOL"}";

  defaultGlslVersion (aProgramSrc, theIsOutline ? "outline" : "unlit", theBits);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & OpenGl_PO_AlphaTest) != 0);
  const Standard_Integer aNbGeomInputVerts = !aSrcGeom.IsEmpty() ? 3 : 0;
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcGeom, Graphic3d_TOS_GEOMETRY, aUniforms, aStageInOuts, "geomIn", "geomOut", aNbGeomInputVerts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : pointSpriteShadingSrc
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::pointSpriteShadingSrc (const TCollection_AsciiString& theBaseColorSrc,
                                                                     Standard_Integer theBits)
{
  TCollection_AsciiString aSrcFragGetColor;
  if ((theBits & OpenGl_PO_PointSpriteA) == OpenGl_PO_PointSpriteA)
  {
    aSrcFragGetColor = pointSpriteAlphaSrc (theBits) +
      EOL"vec4 getColor(void)"
      EOL"{"
      EOL"  vec4 aColor = " + theBaseColorSrc + ";"
      EOL"  aColor.a = getAlpha();"
      EOL"  if (aColor.a <= 0.1) discard;"
      EOL"  return aColor;"
      EOL"}";
  }
  else if ((theBits & OpenGl_PO_PointSprite) == OpenGl_PO_PointSprite)
  {
    aSrcFragGetColor = TCollection_AsciiString() +
      EOL"vec4 getColor(void)"
      EOL"{"
      EOL"  vec4 aColor = " + theBaseColorSrc + ";"
      EOL"  aColor = occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord ") * aColor;"
      EOL"  if (aColor.a <= 0.1) discard;"
      EOL"  return aColor;"
      EOL"}";
  }

  return aSrcFragGetColor;
}

// =======================================================================
// function : stdComputeLighting
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::stdComputeLighting (Standard_Integer& theNbLights,
                                                                  Standard_Boolean  theHasVertColor,
                                                                  Standard_Boolean  theIsPBR,
                                                                  Standard_Boolean  theHasEmissive)
{
  TCollection_AsciiString aLightsFunc, aLightsLoop;
  theNbLights = 0;
  const Handle(Graphic3d_LightSet)& aLights = myLightSourceState.LightSources();
  if (!aLights.IsNull())
  {
    theNbLights = aLights->NbEnabled();
    if (theNbLights <= THE_NB_UNROLLED_LIGHTS_MAX)
    {
      Standard_Integer anIndex = 0;
      for (Graphic3d_LightSet::Iterator aLightIter (aLights, Graphic3d_LightSet::IterationFilter_ExcludeDisabledAndAmbient);
           aLightIter.More(); aLightIter.Next(), ++anIndex)
      {
        switch (aLightIter.Value()->Type())
        {
          case Graphic3d_TOLS_AMBIENT:
            --anIndex;
            break; // skip ambient
          case Graphic3d_TOLS_DIRECTIONAL:
            aLightsLoop = aLightsLoop + EOL"    directionalLight (" + anIndex + ", theNormal, theView, theIsFront);";
            break;
          case Graphic3d_TOLS_POSITIONAL:
            aLightsLoop = aLightsLoop + EOL"    pointLight (" + anIndex + ", theNormal, theView, aPoint, theIsFront);";
            break;
          case Graphic3d_TOLS_SPOT:
            aLightsLoop = aLightsLoop + EOL"    spotLight (" + anIndex + ", theNormal, theView, aPoint, theIsFront);";
            break;
        }
      }
    }
    else
    {
      theNbLights = roundUpMaxLightSources (theNbLights);
      bool isFirstInLoop = true;
      aLightsLoop = aLightsLoop +
        EOL"    for (int anIndex = 0; anIndex < occLightSourcesCount; ++anIndex)"
        EOL"    {"
        EOL"      int aType = occLight_Type (anIndex);";
      if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_DIRECTIONAL) > 0)
      {
        isFirstInLoop = false;
        aLightsLoop +=
          EOL"      if (aType == OccLightType_Direct)"
          EOL"      {"
          EOL"        directionalLight (anIndex, theNormal, theView, theIsFront);"
          EOL"      }";
      }
      if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_POSITIONAL) > 0)
      {
        if (!isFirstInLoop)
        {
          aLightsLoop += EOL"      else ";
        }
        isFirstInLoop = false;
        aLightsLoop +=
          EOL"      if (aType == OccLightType_Point)"
          EOL"      {"
          EOL"        pointLight (anIndex, theNormal, theView, aPoint, theIsFront);"
          EOL"      }";
      }
      if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_SPOT) > 0)
      {
        if (!isFirstInLoop)
        {
          aLightsLoop += EOL"      else ";
        }
        isFirstInLoop = false;
        aLightsLoop +=
          EOL"      if (aType == OccLightType_Spot)"
          EOL"      {"
          EOL"        spotLight (anIndex, theNormal, theView, aPoint, theIsFront);"
          EOL"      }";
      }
      aLightsLoop += EOL"    }";
    }

    if (theIsPBR)
    {
      aLightsFunc += Shaders_PBRDistribution_glsl;
      aLightsFunc += Shaders_PBRGeometry_glsl;
      aLightsFunc += Shaders_PBRFresnel_glsl;
      aLightsFunc += Shaders_PBRCookTorrance_glsl;
      aLightsFunc += Shaders_PBRIllumination_glsl;
    }

    if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_DIRECTIONAL) == 1
     && theNbLights == 1
     && !theIsPBR)
    {
      // use the version with hard-coded first index
      aLightsLoop = EOL"    directionalLightFirst(theNormal, theView, theIsFront);";
      aLightsFunc += THE_FUNC_directionalLightFirst;
    }
    else if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_DIRECTIONAL) > 0)
    {
      aLightsFunc += theIsPBR ? THE_FUNC_PBR_directionalLight : THE_FUNC_directionalLight;
    }
    if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_POSITIONAL) > 0)
    {
      aLightsFunc += theIsPBR ? THE_FUNC_PBR_pointLight : THE_FUNC_pointLight;
    }
    if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_SPOT) > 0)
    {
      aLightsFunc += theIsPBR ? THE_FUNC_PBR_spotLight : THE_FUNC_spotLight;
    }
  }

  TCollection_AsciiString aGetMatAmbient = "theIsFront ? occFrontMaterial_Ambient()  : occBackMaterial_Ambient();";
  TCollection_AsciiString aGetMatDiffuse = "theIsFront ? occFrontMaterial_Diffuse()  : occBackMaterial_Diffuse();";
  if (theHasVertColor)
  {
    aGetMatAmbient = "getVertColor();";
    aGetMatDiffuse = "getVertColor();";
  }

  if (!theIsPBR)
  {
    return TCollection_AsciiString()
    + THE_FUNC_lightDef
    + Shaders_PointLightAttenuation_glsl
    + aLightsFunc
    + EOL
      EOL"vec4 computeLighting (in vec3 theNormal,"
      EOL"                      in vec3 theView,"
      EOL"                      in vec4 thePoint,"
      EOL"                      in bool theIsFront)"
      EOL"{"
      EOL"  Ambient  = occLightAmbient.rgb;"
      EOL"  Diffuse  = vec3 (0.0);"
      EOL"  Specular = vec3 (0.0);"
      EOL"  vec3 aPoint = thePoint.xyz / thePoint.w;"
    + aLightsLoop
    + EOL"  vec4 aMatAmbient  = " + aGetMatAmbient
    + EOL"  vec4 aMatDiffuse  = " + aGetMatDiffuse
    + EOL"  vec4 aMatSpecular = theIsFront ? occFrontMaterial_Specular() : occBackMaterial_Specular();"
      EOL"  vec3 aColor = Ambient * aMatAmbient.rgb + Diffuse * aMatDiffuse.rgb + Specular * aMatSpecular.rgb;"
      EOL"  occTextureOcclusion(aColor, TexCoord.st);"
    + (theHasEmissive
    ? EOL"  vec4 aMatEmission = theIsFront ? occFrontMaterial_Emission() : occBackMaterial_Emission();"
      EOL"  aColor += aMatEmission.rgb;" : "")
    + EOL"  return vec4 (aColor, aMatDiffuse.a);"
      EOL"}";
  }
  else
  {
    return TCollection_AsciiString()
    + THE_FUNC_PBR_lightDef
    + Shaders_PointLightAttenuation_glsl
    + aLightsFunc
    + EOL
      EOL"vec4 computeLighting (in vec3 theNormal,"
      EOL"                      in vec3 theView,"
      EOL"                      in vec4 thePoint,"
      EOL"                      in bool theIsFront)"
      EOL"{"
      EOL"  DirectLighting = vec3(0.0);"
      EOL"  BaseColor = " + (theHasVertColor ? "getVertColor();" : "occTextureColor(occPBRMaterial_Color (theIsFront), TexCoord.st / TexCoord.w);")
    + EOL"  Emission            = occTextureEmissive(occPBRMaterial_Emission (theIsFront), TexCoord.st / TexCoord.w);"
      EOL"  Metallic            = occTextureMetallic(occPBRMaterial_Metallic (theIsFront), TexCoord.st / TexCoord.w);"
      EOL"  NormalizedRoughness = occTextureRoughness(occPBRMaterial_NormalizedRoughness (theIsFront), TexCoord.st / TexCoord.w);"
      EOL"  Roughness = occRoughness (NormalizedRoughness);"
      EOL"  IOR       = occPBRMaterial_IOR (theIsFront);"
      EOL"  vec3 aPoint = thePoint.xyz / thePoint.w;"
    + aLightsLoop
    + EOL"  vec3 aColor = DirectLighting;"
      EOL"  vec3 anIndirectLightingSpec = occPBRFresnel (BaseColor.rgb, Metallic, IOR);"
      EOL"  vec2 aCoeff = occTexture2D (occEnvLUT, vec2(abs(dot(theView, theNormal)), NormalizedRoughness)).xy;"
      EOL"  anIndirectLightingSpec *= aCoeff.x;"
      EOL"  anIndirectLightingSpec += aCoeff.y;"
      EOL"  anIndirectLightingSpec *= occTextureCubeLod (occSpecIBLMap, -reflect (theView, theNormal), NormalizedRoughness * float (occNbSpecIBLLevels - 1)).rgb;"
      EOL"  vec3 aRefractionCoeff = 1.0 - occPBRFresnel (BaseColor.rgb, Metallic, NormalizedRoughness, IOR, abs(dot(theView, theNormal)));"
      EOL"  aRefractionCoeff *= (1.0 - Metallic);"
      EOL"  vec3 anIndirectLightingDiff = aRefractionCoeff * BaseColor.rgb * BaseColor.a;"
      EOL"  anIndirectLightingDiff *= occDiffIBLMap (theNormal).rgb;"
      EOL"  aColor += occLightAmbient.rgb * (anIndirectLightingDiff + anIndirectLightingSpec);"
      EOL"  aColor += Emission;"
      EOL"  occTextureOcclusion(aColor, TexCoord.st / TexCoord.w);"
      EOL"  return vec4 (aColor, mix(1.0, BaseColor.a, aRefractionCoeff.x));"
      EOL"}";
  }
}

// =======================================================================
// function : prepareStdProgramGouraud
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramGouraud (Handle(OpenGl_ShaderProgram)& theProgram,
                                                                 const Standard_Integer        theBits)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertColor, aSrcVertExtraMain;
  TCollection_AsciiString aSrcFrag, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return gl_FrontFacing ? FrontColor : BackColor; }";
  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;

  if ((theBits & OpenGl_PO_IsPoint) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if ((theBits & OpenGl_PO_PointSprite) != 0)
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (myContext->core11 != NULL
        && myContext->IsGlGreaterEqual (2, 1))
      {
        aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
      }
    #endif

      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerPointSprite", Graphic3d_TOS_FRAGMENT));
      aSrcFragGetColor = pointSpriteShadingSrc ("gl_FrontFacing ? FrontColor : BackColor", theBits);
    }

    if ((theBits & OpenGl_PO_TextureRGB) != 0
     && (theBits & OpenGl_PO_VertColor) == 0)
    {
      aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
      aSrcVertColor = EOL"vec4 getVertColor(void) { return occTexture2D (occSamplerBaseColor, occTexCoord.xy); }";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
      aUniforms   .Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
      aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = gl_FrontFacing ? FrontColor : BackColor;"
        EOL"  return occTexture2D(occSamplerBaseColor, TexCoord.st / TexCoord.w) * aColor;"
        EOL"}";
    }
  }

  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertColor = EOL"vec4 getVertColor(void) { return occVertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 PositionWorld", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 Position",      Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain +=
      EOL"  PositionWorld = aPositionWorld;"
      EOL"  Position      = aPosition;";

    if ((theBits & OpenGl_PO_ClipPlanesN) == OpenGl_PO_ClipPlanesN)
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
    {
      aNbClipPlanes = 1;
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes2) != 0)
    {
      aNbClipPlanes = 2;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                          ? THE_FRAG_CLIP_CHAINS_2
                          : THE_FRAG_CLIP_PLANES_2;
    }
  }
  if ((theBits & OpenGl_PO_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetWeightOitOutput (true);
  }

  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 FrontColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 BackColor",  Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  Standard_Integer aNbLights = 0;
  const TCollection_AsciiString aLights = stdComputeLighting (aNbLights, !aSrcVertColor.IsEmpty(), false, true);
  aSrcVert = TCollection_AsciiString()
    + THE_FUNC_transformNormal_view
    + EOL
    + aSrcVertColor
    + aLights
    + EOL"void main()"
      EOL"{"
      EOL"  vec4 aPositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  vec4 aPosition      = occWorldViewMatrix * aPositionWorld;"
      EOL"  vec3 aNormal        = transformNormal (occNormal);"
      EOL"  vec3 aView          = vec3 (0.0, 0.0, 1.0);"
      EOL"  FrontColor  = computeLighting (normalize (aNormal), normalize (aView), aPosition, true);"
      EOL"  BackColor   = computeLighting (normalize (aNormal), normalize (aView), aPosition, false);"
    + aSrcVertExtraMain
    + THE_VERT_gl_Position
    + EOL"}";

  TCollection_AsciiString aSrcGeom = prepareGeomMainSrc (aUniforms, aStageInOuts, theBits);
  aSrcFragGetColor += (theBits & OpenGl_PO_MeshEdges) != 0
    ? THE_FRAG_WIREFRAME_COLOR
    : EOL"#define getFinalColor getColor";

  aSrcFrag = TCollection_AsciiString()
    + aSrcFragGetColor
    + EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + EOL"  occSetFragColor (getFinalColor());"
    + EOL"}";

  const TCollection_AsciiString aProgId = TCollection_AsciiString ("gouraud-") + genLightKey (myLightSourceState.LightSources()) + "-";
  defaultGlslVersion (aProgramSrc, aProgId, theBits);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (aNbLights);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & OpenGl_PO_AlphaTest) != 0);
  const Standard_Integer aNbGeomInputVerts = !aSrcGeom.IsEmpty() ? 3 : 0;
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcGeom, Graphic3d_TOS_GEOMETRY, aUniforms, aStageInOuts, "geomIn", "geomOut", aNbGeomInputVerts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramPhong
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramPhong (Handle(OpenGl_ShaderProgram)& theProgram,
                                                               const Standard_Integer        theBits,
                                                               const Standard_Boolean        theIsFlatNormal,
                                                               const Standard_Boolean        theIsPBR)
{
  TCollection_AsciiString aPosition = theIsPBR ? "PositionWorld" : "Position";
  TCollection_AsciiString aPhongCompLight = TCollection_AsciiString() +
    "computeLighting (normalize (Normal), normalize (View), " + aPosition + ", gl_FrontFacing)";
  const bool isFlatNormal = theIsFlatNormal
                         && myContext->hasFlatShading != OpenGl_FeatureNotAvailable;
  const char* aDFdxSignReversion = "";
#if defined(GL_ES_VERSION_2_0)
  if (isFlatNormal != theIsFlatNormal)
  {
    myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                            "Warning: flat shading requires OpenGL ES 3.0+ or GL_OES_standard_derivatives extension.");
  }
  else if (isFlatNormal
        && myContext->Vendor().Search("qualcomm") != -1)
  {
    // workaround Adreno driver bug computing reversed normal using dFdx/dFdy
    aDFdxSignReversion = "-";
    myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                            "Warning: applied workaround for flat shading normal computation using dFdx/dFdy on Adreno");
  }
#endif
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  aProgramSrc->SetPBR (theIsPBR);

  TCollection_AsciiString aSrcVert, aSrcVertExtraFunc, aSrcVertExtraMain;
  TCollection_AsciiString aSrcFrag, aSrcFragGetVertColor, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = TCollection_AsciiString() + EOL"vec4 getColor(void) { return " + aPhongCompLight +  "; }";
  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  if ((theBits & OpenGl_PO_IsPoint) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if ((theBits & OpenGl_PO_PointSprite) != 0)
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (myContext->core11 != NULL
        && myContext->IsGlGreaterEqual (2, 1))
      {
        aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
      }
    #endif

      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerPointSprite", Graphic3d_TOS_FRAGMENT));
      aSrcFragGetColor = pointSpriteShadingSrc (aPhongCompLight, theBits);
    }

    if ((theBits & OpenGl_PO_TextureRGB) != 0
     && (theBits & OpenGl_PO_VertColor) == 0)
    {
      aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
      aUniforms   .Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
      aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

      aSrcVertExtraMain   += EOL"  VertColor = occTexture2D (occSamplerBaseColor, occTexCoord.xy);";
      aSrcFragGetVertColor = EOL"vec4 getVertColor(void) { return VertColor; }";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aUniforms   .Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
      aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      Standard_Integer aTextureBits = Graphic3d_TextureSetBits_BaseColor | Graphic3d_TextureSetBits_Occlusion | Graphic3d_TextureSetBits_Emissive;
      if (!theIsPBR)
      {
        aSrcFragGetColor = TCollection_AsciiString() +
          EOL"vec4 getColor(void)"
          EOL"{"
          EOL"  vec2 aTexUV = TexCoord.st / TexCoord.w;"
          EOL"  vec4 aColor = " + aPhongCompLight + ";"
          EOL"  aColor *= occTexture2D(occSamplerBaseColor, aTexUV);"
          EOL"  vec3 anEmission = occTextureEmissive((gl_FrontFacing ? occFrontMaterial_Emission() : occBackMaterial_Emission()).rgb, aTexUV);"
          EOL"  aColor.rgb += anEmission;"
          EOL"  return aColor;"
          EOL"}";
      }
      else
      {
        aTextureBits |= Graphic3d_TextureSetBits_MetallicRoughness;
      }
      if ((theBits & OpenGl_PO_HasTextures) == OpenGl_PO_TextureNormal
       && !isFlatNormal)
      {
        if (myContext->hasFlatShading != OpenGl_FeatureNotAvailable)
        {
          aTextureBits |= Graphic3d_TextureSetBits_Normal;
        }
        else
        {
          myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                                  "Warning: ignoring Normal Map texture due to hardware capabilities");
        }
      }
      aProgramSrc->SetTextureSetBits (aTextureBits);
    }
  }

  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain   += EOL"  VertColor = occVertColor;";
    aSrcFragGetVertColor = EOL"vec4 getVertColor(void) { return VertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    if ((theBits & OpenGl_PO_ClipPlanesN) == OpenGl_PO_ClipPlanesN)
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
    {
      aNbClipPlanes = 1;
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes2) != 0)
    {
      aNbClipPlanes = 2;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_2
                         : THE_FRAG_CLIP_PLANES_2;
    }
  }
  if ((theBits & OpenGl_PO_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetWeightOitOutput (true);
  }

  if (isFlatNormal)
  {
    aSrcFragExtraMain += TCollection_AsciiString()
      + EOL"  Normal = " + aDFdxSignReversion + "normalize (cross (dFdx (" + aPosition + ".xyz / " + aPosition + ".w), dFdy (" + aPosition + ".xyz / " + aPosition + ".w)));"
        EOL"  if (!gl_FrontFacing) { Normal = -Normal; }";
  }
  else
  {
    aStageInOuts.Append(OpenGl_ShaderObject::ShaderVariable("vec3 vNormal", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraFunc += THE_FUNC_transformNormal_world;
    aSrcVertExtraMain += EOL"  vNormal = transformNormal (occNormal);";
    aSrcFragExtraMain += EOL"  Normal = vNormal;";

    if ((theBits & OpenGl_PO_IsPoint) == 0
     && (theBits & OpenGl_PO_HasTextures) == OpenGl_PO_TextureNormal
     && myContext->hasFlatShading != OpenGl_FeatureNotAvailable)
    {
      aSrcFrag += Shaders_TangentSpaceNormal_glsl;
      // apply normal map texture
      aSrcFragExtraMain +=
        EOL"#if defined(THE_HAS_TEXTURE_NORMAL)"
        EOL"  vec2 aTexCoord = TexCoord.st / TexCoord.w;"
        EOL"  vec4 aMapNormalValue = occTextureNormal(aTexCoord);"
        EOL"  if (aMapNormalValue.w > 0.5)"
        EOL"  {"
        EOL"    mat2 aDeltaUVMatrix = mat2 (dFdx(aTexCoord), dFdy(aTexCoord));"
        EOL"    mat2x3 aDeltaVectorMatrix = mat2x3 (dFdx (PositionWorld.xyz), dFdy (PositionWorld.xyz));"
        EOL"    Normal = TangentSpaceNormal (aDeltaUVMatrix, aDeltaVectorMatrix, aMapNormalValue.xyz, Normal, !gl_FrontFacing);"
        EOL"  }"
        EOL"#endif";
    }
    if (!theIsPBR)
    {
      aSrcFragExtraMain +=
        EOL"  Normal = normalize ((occWorldViewMatrixInverseTranspose * vec4 (Normal, 0.0)).xyz);";
    }
  }

  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 PositionWorld", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec4 Position",      Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec3 View",          Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  aSrcVert = TCollection_AsciiString()
    + aSrcVertExtraFunc
    + EOL"void main()"
      EOL"{"
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  Position      = occWorldViewMatrix * PositionWorld;"
      EOL"  if (occProjectionMatrix[3][3] == 1.0)"
      EOL"  {"
      EOL"    View = vec3(0.0, 0.0, 1.0);"
      EOL"  }"
      EOL"  else"
      EOL"  {"
      EOL"    View = -Position.xyz;"
      EOL"  }"
    + (theIsPBR ? EOL"  View = (occWorldViewMatrixInverse * vec4(View, 0.0)).xyz;" : "")
    + aSrcVertExtraMain
    + THE_VERT_gl_Position
    + EOL"}";

  TCollection_AsciiString aSrcGeom = prepareGeomMainSrc (aUniforms, aStageInOuts, theBits);
  aSrcFragGetColor += (theBits & OpenGl_PO_MeshEdges) != 0
    ? THE_FRAG_WIREFRAME_COLOR
    : EOL"#define getFinalColor getColor";

  Standard_Integer aNbLights = 0;
  const TCollection_AsciiString aLights = stdComputeLighting (aNbLights, !aSrcFragGetVertColor.IsEmpty(), theIsPBR,
                                                              (theBits & OpenGl_PO_TextureRGB) == 0
                                                           || (theBits & OpenGl_PO_IsPoint) != 0);
  aSrcFrag += TCollection_AsciiString()
    + EOL
    + aSrcFragGetVertColor
    + EOL"vec3  Normal;"
    + aLights
    + aSrcFragGetColor
    + EOL
      EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + EOL"  occSetFragColor (getFinalColor());"
    + EOL"}";

  const TCollection_AsciiString aProgId = TCollection_AsciiString (theIsFlatNormal ? "flat-" : "phong-") + (theIsPBR ? "pbr-" : "") + genLightKey (myLightSourceState.LightSources()) + "-";
  defaultGlslVersion (aProgramSrc, aProgId, theBits, isFlatNormal);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (aNbLights);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & OpenGl_PO_AlphaTest) != 0);

  const Standard_Integer aNbGeomInputVerts = !aSrcGeom.IsEmpty() ? 3 : 0;
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcGeom, Graphic3d_TOS_GEOMETRY, aUniforms, aStageInOuts, "geomIn", "geomOut", aNbGeomInputVerts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramStereo
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramStereo (Handle(OpenGl_ShaderProgram)& theProgram,
                                                                const Graphic3d_StereoMode    theStereoMode)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;

  aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  TCollection_AsciiString aSrcVert =
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag;
  aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D uLeftSampler",  Graphic3d_TOS_FRAGMENT));
  aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("sampler2D uRightSampler", Graphic3d_TOS_FRAGMENT));
  const char* aName = "stereo";
  switch (theStereoMode)
  {
    case Graphic3d_StereoMode_Anaglyph:
    {
      aName = "anaglyph";
      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("mat4 uMultL", Graphic3d_TOS_FRAGMENT));
      aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("mat4 uMultR", Graphic3d_TOS_FRAGMENT));
      const TCollection_AsciiString aNormalize = mySRgbState
                                               ? EOL"#define sRgb2linear(theColor) theColor"
                                                 EOL"#define linear2sRgb(theColor) theColor"
                                               : EOL"#define sRgb2linear(theColor) pow(theColor, vec4(2.2, 2.2, 2.2, 1.0))"
                                                 EOL"#define linear2sRgb(theColor) pow(theColor, 1.0 / vec4(2.2, 2.2, 2.2, 1.0))";
      aSrcFrag = aNormalize
      + EOL"void main()"
        EOL"{"
        EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
        EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
        EOL"  aColorL = sRgb2linear (aColorL);"
        EOL"  aColorR = sRgb2linear (aColorR);"
        EOL"  vec4 aColor = uMultR * aColorR + uMultL * aColorL;"
        EOL"  occSetFragColor (linear2sRgb (aColor));"
        EOL"}";
      break;
    }
    case Graphic3d_StereoMode_RowInterlaced:
    {
      aName = "row-interlaced";
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  if (int (mod (gl_FragCoord.y - 1023.5, 2.0)) != 1)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_ColumnInterlaced:
    {
      aName = "column-interlaced";
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  if (int (mod (gl_FragCoord.x - 1023.5, 2.0)) == 1)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_ChessBoard:
    {
      aName = "chessboard";
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  bool isEvenX = int(mod(floor(gl_FragCoord.x - 1023.5), 2.0)) != 1;"
          EOL"  bool isEvenY = int(mod(floor(gl_FragCoord.y - 1023.5), 2.0)) == 1;"
          EOL"  if ((isEvenX && isEvenY) || (!isEvenX && !isEvenY))"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_SideBySide:
    {
      aName = "sidebyside";
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoord = vec2 (TexCoord.x * 2.0, TexCoord.y);"
          EOL"  if (TexCoord.x > 0.5)"
          EOL"  {"
          EOL"    aTexCoord.x -= 1.0;"
          EOL"  }"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoord);"
          EOL"  if (TexCoord.x <= 0.5)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_OverUnder:
    {
      aName = "overunder";
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoord = vec2 (TexCoord.x, TexCoord.y * 2.0);"
          EOL"  if (TexCoord.y > 0.5)"
          EOL"  {"
          EOL"    aTexCoord.y -= 1.0;"
          EOL"  }"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoord);"
          EOL"  if (TexCoord.y <= 0.5)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_QuadBuffer:
    case Graphic3d_StereoMode_SoftPageFlip:
    case Graphic3d_StereoMode_OpenVR:
    default:
    {
      /*const Handle(OpenGl_ShaderProgram)& aProgram = myStereoPrograms[Graphic3d_StereoMode_QuadBuffer];
      if (!aProgram.IsNull())
      {
        return aProgram->IsValid();
      }*/
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  aColorL.b = 0.0;"
          EOL"  aColorL.g = 0.0;"
          EOL"  aColorR.r = 0.0;"
          EOL"  occSetFragColor (aColorL + aColorR);"
          EOL"}";
      break;
    }
  }

  defaultGlslVersion (aProgramSrc, aName, 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (theProgram);
  theProgram->SetSampler (myContext, "uLeftSampler",  Graphic3d_TextureUnit_0);
  theProgram->SetSampler (myContext, "uRightSampler", Graphic3d_TextureUnit_1);
  myContext->BindProgram (NULL);
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramBoundBox
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramBoundBox()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();

  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("vec3 occBBoxCenter", Graphic3d_TOS_VERTEX));
  aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("vec3 occBBoxSize",   Graphic3d_TOS_VERTEX));

  TCollection_AsciiString aSrcVert =
    EOL"void main()"
    EOL"{"
    EOL"  vec4 aCenter = vec4(occVertex.xyz * occBBoxSize + occBBoxCenter, 1.0);"
    EOL"  vec4 aPos    = vec4(occVertex.xyz * occBBoxSize + occBBoxCenter, 1.0);"
    EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * aPos;"
    EOL"}";

  TCollection_AsciiString aSrcFrag =
    EOL"void main()"
    EOL"{"
    EOL"  occSetFragColor (occColor);"
    EOL"}";

  defaultGlslVersion (aProgramSrc, "bndbox", 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myBoundBoxProgram))
  {
    myBoundBoxProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  const OpenGl_Vec4 aMin (-0.5f, -0.5f, -0.5f, 1.0f);
  const OpenGl_Vec4 anAxisShifts[3] =
  {
    OpenGl_Vec4 (1.0f, 0.0f, 0.0f, 0.0f),
    OpenGl_Vec4 (0.0f, 1.0f, 0.0f, 0.0f),
    OpenGl_Vec4 (0.0f, 0.0f, 1.0f, 0.0f)
  };

  const OpenGl_Vec4 aLookup1 (0.0f, 1.0f, 0.0f, 1.0f);
  const OpenGl_Vec4 aLookup2 (0.0f, 0.0f, 1.0f, 1.0f);
  OpenGl_Vec4 aLinesVertices[24];
  for (int anAxis = 0, aVertex = 0; anAxis < 3; ++anAxis)
  {
    for (int aCompIter = 0; aCompIter < 4; ++aCompIter)
    {
      aLinesVertices[aVertex++] = aMin
        + anAxisShifts[(anAxis + 1) % 3] * aLookup1[aCompIter]
        + anAxisShifts[(anAxis + 2) % 3] * aLookup2[aCompIter];

      aLinesVertices[aVertex++] = aMin
        + anAxisShifts[anAxis]
        + anAxisShifts[(anAxis + 1) % 3] * aLookup1[aCompIter]
        + anAxisShifts[(anAxis + 2) % 3] * aLookup2[aCompIter];
    }
  }
  if (myContext->ToUseVbo())
  {
    myBoundBoxVertBuffer = new OpenGl_VertexBuffer();
    if (myBoundBoxVertBuffer->Init (myContext, 4, 24, aLinesVertices[0].GetData()))
    {
      myContext->ShareResource ("OpenGl_ShaderManager_BndBoxVbo", myBoundBoxVertBuffer);
      return Standard_True;
    }
  }
  myBoundBoxVertBuffer = new OpenGl_VertexBufferCompat();
  myBoundBoxVertBuffer->Init (myContext, 4, 24, aLinesVertices[0].GetData());
  myContext->ShareResource ("OpenGl_ShaderManager_BndBoxVbo", myBoundBoxVertBuffer);
  return Standard_True;
}

// =======================================================================
// function : preparePBREnvBakingProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::preparePBREnvBakingProgram()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;

  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
  + THE_FUNC_cubemap_vector_transform
  + Shaders_PBREnvBaking_vs;

  TCollection_AsciiString aSrcFrag = TCollection_AsciiString()
  + THE_FUNC_cubemap_vector_transform
  + Shaders_PBRDistribution_glsl
  + Shaders_PBREnvBaking_fs;

  // constant array definition requires OpenGL 2.1+ or OpenGL ES 3.0+
#if defined(GL_ES_VERSION_2_0)
  aProgramSrc->SetHeader ("#version 300 es");
#else
  aProgramSrc->SetHeader ("#version 120");
#endif

  defaultGlslVersion (aProgramSrc, "pbr_env_baking", 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->SetPBR (true);
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myPBREnvBakingProgram))
  {
    myPBREnvBakingProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : GetBgCubeMapProgram
// purpose  :
// =======================================================================
const Handle(Graphic3d_ShaderProgram)& OpenGl_ShaderManager::GetBgCubeMapProgram ()
{
  if (myBgCubeMapProgram.IsNull())
  {
    myBgCubeMapProgram = new Graphic3d_ShaderProgram();

    OpenGl_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
    aStageInOuts.Append (OpenGl_ShaderObject::ShaderVariable("vec3 ViewDirection", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("samplerCube occSampler0", Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("int uYCoeff", Graphic3d_TOS_VERTEX));
    aUniforms.Append (OpenGl_ShaderObject::ShaderVariable ("int uZCoeff", Graphic3d_TOS_VERTEX));

    TCollection_AsciiString aSrcVert = TCollection_AsciiString()
    + THE_FUNC_cubemap_vector_transform
    + EOL"void main()"
      EOL"{"
      EOL"  ViewDirection = cubemapVectorTransform (occVertex.xyz, uYCoeff, uZCoeff);"
      EOL"  vec4 aPos = occProjectionMatrix * occWorldViewMatrix * vec4(occVertex.xyz, 1.0);"
      EOL"  gl_Position = aPos.xyww;"
      EOL"}";

    TCollection_AsciiString aSrcFrag =
      EOL"#define occEnvCubemap occSampler0"
      EOL"void main()"
      EOL"{"
      EOL"  occSetFragColor (vec4(occTextureCube (occEnvCubemap, ViewDirection).rgb, 1.0));"
      EOL"}";

    defaultGlslVersion (myBgCubeMapProgram, "background_cubemap", 0);
    myBgCubeMapProgram->SetDefaultSampler (false);
    myBgCubeMapProgram->SetNbLightsMax (0);
    myBgCubeMapProgram->SetNbClipPlanesMax (0);
    myBgCubeMapProgram->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX, aUniforms, aStageInOuts));
    myBgCubeMapProgram->AttachShader (OpenGl_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  }

  return myBgCubeMapProgram;
}

// =======================================================================
// function : bindProgramWithState
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::bindProgramWithState (const Handle(OpenGl_ShaderProgram)& theProgram,
                                                             Graphic3d_TypeOfShadingModel theShadingModel)
{
  const Standard_Boolean isBound = myContext->BindProgram (theProgram);
  if (isBound
  && !theProgram.IsNull())
  {
    theProgram->ApplyVariables (myContext);
  }
  PushState (theProgram, theShadingModel);
  return isBound;
}

// =======================================================================
// function : BindMarkerProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::BindMarkerProgram (const Handle(OpenGl_TextureSet)& theTextures,
                                                          Graphic3d_TypeOfShadingModel theShadingModel,
                                                          Graphic3d_AlphaMode theAlphaMode,
                                                          Standard_Boolean theHasVertColor,
                                                          const Handle(OpenGl_ShaderProgram)& theCustomProgram)
{
  if (!theCustomProgram.IsNull()
    || myContext->caps->ffpEnable)
  {
    return bindProgramWithState (theCustomProgram, theShadingModel);
  }

  Standard_Integer aBits = getProgramBits (theTextures, theAlphaMode, Aspect_IS_SOLID, theHasVertColor, false, false);
  if (!theTextures.IsNull()
    && theTextures->HasPointSprite())
  {
    aBits |= theTextures->Last()->IsAlpha() ? OpenGl_PO_PointSpriteA : OpenGl_PO_PointSprite;
  }
  else
  {
    aBits |= OpenGl_PO_PointSimple;
  }
  Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theShadingModel, aBits);
  return bindProgramWithState (aProgram, theShadingModel);
}
