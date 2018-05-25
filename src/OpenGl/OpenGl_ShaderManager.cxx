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
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_ClippingIterator.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Workspace.hxx>

#include <TCollection_ExtendedString.hxx>

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

//! Definition of TexCoord varying.
const char THE_VARY_TexCoord_OUT[] =
  EOL"THE_SHADER_OUT vec4 TexCoord;";
const char THE_VARY_TexCoord_IN[] =
  EOL"THE_SHADER_IN  vec4 TexCoord;";
//! Compute TexCoord value in Vertex Shader
const char THE_VARY_TexCoord_Trsf[] =
  EOL"  float aRotSin = occTextureTrsf_RotationSin();"
  EOL"  float aRotCos = occTextureTrsf_RotationCos();"
  EOL"  vec2  aTex2   = (occTexCoord.xy + occTextureTrsf_Translation()) * occTextureTrsf_Scale();"
  EOL"  vec2  aCopy   = aTex2;"
  EOL"  aTex2.x = aCopy.x * aRotCos - aCopy.y * aRotSin;"
  EOL"  aTex2.y = aCopy.x * aRotSin + aCopy.y * aRotCos;"
  EOL"  TexCoord = vec4(aTex2, occTexCoord.zw);";

//! Auxiliary function to flip gl_PointCoord vertically
#define THE_VEC2_glPointCoord "vec2 (gl_PointCoord.x, 1.0 - gl_PointCoord.y)"

//! Auxiliary function to transform normal
const char THE_FUNC_transformNormal[] =
  EOL"vec3 transformNormal (in vec3 theNormal)"
  EOL"{"
  EOL"  vec4 aResult = occWorldViewMatrixInverseTranspose"
  EOL"               * occModelWorldMatrixInverseTranspose"
  EOL"               * vec4 (theNormal, 0.0);"
  EOL"  return normalize (aResult.xyz);"
  EOL"}";

//! Global shader variable for color definition with lighting enabled.
const char THE_FUNC_lightDef[] =
  EOL"vec3 Ambient;"   //!< Ambient  contribution of light sources
  EOL"vec3 Diffuse;"   //!< Diffuse  contribution of light sources
  EOL"vec3 Specular;"; //!< Specular contribution of light sources

//! Function computes contribution of isotropic point light source
const char THE_FUNC_pointLight[] =
  EOL"void pointLight (in int  theId,"
  EOL"                 in vec3 theNormal,"
  EOL"                 in vec3 theView,"
  EOL"                 in vec3 thePoint,"
  EOL"                 in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = occLight_Position (theId).xyz;"
  EOL"  if (occLight_IsHeadlight (theId) == 0)"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrix * vec4 (aLight, 1.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  aLight = aLight * (1.0 / aDist);"
  EOL
  EOL"  float anAtten = 1.0 / (occLight_ConstAttenuation  (theId)"
  EOL"                       + occLight_LinearAttenuation (theId) * aDist);"
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
  EOL"Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL * anAtten;"
  EOL"Specular += occLight_Specular (theId).rgb * aSpecl * anAtten;"
  EOL"}";

//! Function computes contribution of spotlight source
const char THE_FUNC_spotLight[] =
  EOL"void spotLight (in int  theId,"
  EOL"                in vec3 theNormal,"
  EOL"                in vec3 theView,"
  EOL"                in vec3 thePoint,"
  EOL"                in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight   = occLight_Position      (theId).xyz;"
  EOL"  vec3 aSpotDir = occLight_SpotDirection (theId).xyz;"
  EOL"  if (occLight_IsHeadlight (theId) == 0)"
  EOL"  {"
  EOL"    aLight   = vec3 (occWorldViewMatrix * vec4 (aLight,   1.0));"
  EOL"    aSpotDir = vec3 (occWorldViewMatrix * vec4 (aSpotDir, 0.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  aLight = aLight * (1.0 / aDist);"
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
  EOL"  float anAtten    = 1.0 / (occLight_ConstAttenuation  (theId)"
  EOL"                          + occLight_LinearAttenuation (theId) * aDist);"
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
  EOL"  Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL * anAtten;"
  EOL"  Specular += occLight_Specular (theId).rgb * aSpecl * anAtten;"
  EOL"}";

//! Function computes contribution of directional light source
const char THE_FUNC_directionalLight[] =
  EOL"void directionalLight (in int  theId,"
  EOL"                       in vec3 theNormal,"
  EOL"                       in vec3 theView,"
  EOL"                       in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = normalize (occLight_Position (theId).xyz);"
  EOL"  if (occLight_IsHeadlight (theId) == 0)"
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
  EOL"  Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL;"
  EOL"  Specular += occLight_Specular (theId).rgb * aSpecl;"
  EOL"}";

//! The same as THE_FUNC_directionalLight but for the light with zero index
//! (avoids limitations on some mobile devices).
const char THE_FUNC_directionalLightFirst[] =
  EOL"void directionalLightFirst (in vec3 theNormal,"
  EOL"                            in vec3 theView,"
  EOL"                            in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = normalize (occLightSources[1].xyz);"
  EOL"  if (occLight_IsHeadlight (0) == 0)"
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
  EOL"  Diffuse  += occLightSources[0].rgb * aNdotL;"
  EOL"  Specular += occLightSources[0].rgb * aSpecl;"
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
        const OpenGl_Vec4 anInfDir = -theLight.PackedDirection();

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
        theCtx->core11->glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0);
        break;
      }
      case Graphic3d_TOLS_SPOT:
      {
        const OpenGl_Vec4 aPosition (static_cast<float>(theLight.Position().X()), static_cast<float>(theLight.Position().Y()), static_cast<float>(theLight.Position().Z()), 1.0f);
        theCtx->core11->glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
        theCtx->core11->glLightfv (theLightGlId, GL_DIFFUSE,               aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPECULAR,              aLightColor.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_POSITION,              aPosition.GetData());
        theCtx->core11->glLightfv (theLightGlId, GL_SPOT_DIRECTION,        theLight.PackedDirection().GetData());
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

}

// =======================================================================
// function : OpenGl_ShaderManager
// purpose  : Creates new empty shader manager
// =======================================================================
OpenGl_ShaderManager::OpenGl_ShaderManager (OpenGl_Context* theContext)
: myFfpProgram (new OpenGl_ShaderProgramFFP()),
  myShadingModel (Graphic3d_TOSM_VERTEX),
  myUnlitPrograms (new OpenGl_SetOfShaderPrograms()),
  myContext  (theContext),
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
}

// =======================================================================
// function : clear
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::clear()
{
  myProgramList.Clear();
  myLightPrograms.Nullify();
  myUnlitPrograms = new OpenGl_SetOfShaderPrograms();
  myMapOfLightPrograms.Clear();
  myFontProgram.Nullify();
  myBlitProgram.Nullify();
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
    myLightPrograms = myUnlitPrograms;
    return;
  }

  TCollection_AsciiString aKey ("l_");
  if (aLights->NbEnabled() <= THE_NB_UNROLLED_LIGHTS_MAX)
  {
    aKey += aLights->KeyEnabledLong();
  }
  else
  {
    const Standard_Integer aMaxLimit = roundUpMaxLightSources (aLights->NbEnabled());
    aKey += aLights->KeyEnabledShort();
    aKey += aMaxLimit;
  }

  if (!myMapOfLightPrograms.Find (aKey, myLightPrograms))
  {
    myLightPrograms = new OpenGl_SetOfShaderPrograms();
    myMapOfLightPrograms.Bind (aKey, myLightPrograms);
  }
}

// =======================================================================
// function : UpdateLightSourceStateTo
// purpose  : Updates state of OCCT light sources
// =======================================================================
void OpenGl_ShaderManager::UpdateLightSourceStateTo (const Handle(Graphic3d_LightSet)& theLights)
{
  myLightSourceState.Set (theLights);
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
// function : PushLightSourceState
// purpose  : Pushes state of OCCT light sources to the program
// =======================================================================
void OpenGl_ShaderManager::PushLightSourceState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myLightSourceState.Index() == theProgram->ActiveState (OpenGl_LIGHT_SOURCES_STATE))
  {
    return;
  }

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
    myLightTypeArray.ChangeValue (aLightIt).Type = -1;
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
                            aNbLightsMax * OpenGl_ShaderLightType::NbOfVec2i(),
                            myLightTypeArray.First().Packed());
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

    OpenGl_ShaderLightType&       aLightType   = myLightTypeArray.ChangeValue (aLightsNb);
    OpenGl_ShaderLightParameters& aLightParams = myLightParamsArray.ChangeValue (aLightsNb);
    if (!aLight.IsEnabled()) // has no affect with Graphic3d_LightSet::IterationFilter_ExcludeDisabled - here just for consistency
    {
      // if it is desired to keep disabled light in the same order - we can replace it with a black light so that it will have no influence on result
      aLightType.Type        = -1; // Graphic3d_TOLS_AMBIENT can be used instead
      aLightType.IsHeadlight = false;
      aLightParams.Color     = OpenGl_Vec4 (0.0f, 0.0f, 0.0f, 0.0f);
      ++aLightsNb;
      continue;
    }

    aLightType.Type        = aLight.Type();
    aLightType.IsHeadlight = aLight.IsHeadlight();
    aLightParams.Color     = aLight.PackedColor();
    if (aLight.Type() == Graphic3d_TOLS_DIRECTIONAL)
    {
      aLightParams.Position = -aLight.PackedDirection();
    }
    else if (!aLight.IsHeadlight())
    {
      aLightParams.Position.x() = static_cast<float>(aLight.Position().X() - myLocalOrigin.X());
      aLightParams.Position.y() = static_cast<float>(aLight.Position().Y() - myLocalOrigin.Y());
      aLightParams.Position.z() = static_cast<float>(aLight.Position().Z() - myLocalOrigin.Z());
      aLightParams.Position.w() = 1.0f;
    }
    else
    {
      aLightParams.Position.x() = static_cast<float>(aLight.Position().X());
      aLightParams.Position.y() = static_cast<float>(aLight.Position().Y());
      aLightParams.Position.z() = static_cast<float>(aLight.Position().Z());
      aLightParams.Position.w() = 1.0f;
    }

    if (aLight.Type() == Graphic3d_TOLS_SPOT)
    {
      aLightParams.Direction = aLight.PackedDirection();
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
                          aNbLightsMax * OpenGl_ShaderLightType::NbOfVec2i(),
                          myLightTypeArray.First().Packed());
  if (aLightsNb > 0)
  {
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_PARAMS),
                            aLightsNb * OpenGl_ShaderLightParameters::NbOfVec4(),
                            myLightParamsArray.First().Packed());
  }
}

// =======================================================================
// function : PushProjectionState
// purpose  : Pushes state of OCCT projection transform to the program
// =======================================================================
void OpenGl_ShaderManager::PushProjectionState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myProjectionState.Index() == theProgram->ActiveState (OpenGl_PROJECTION_STATE))
  {
    return;
  }

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
// function : PushModelWorldState
// purpose  : Pushes state of OCCT model-world transform to the program
// =======================================================================
void OpenGl_ShaderManager::PushModelWorldState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myModelWorldState.Index() == theProgram->ActiveState (OpenGl_MODEL_WORLD_STATE))
  {
    return;
  }

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
// function : PushWorldViewState
// purpose  : Pushes state of OCCT world-view transform to the program
// =======================================================================
void OpenGl_ShaderManager::PushWorldViewState (const Handle(OpenGl_ShaderProgram)& theProgram) const
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
// function : PushClippingState
// purpose  : Pushes state of OCCT clipping planes to the program
// =======================================================================
void OpenGl_ShaderManager::PushClippingState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myClippingState.Index() == theProgram->ActiveState (OpenGl_CLIP_PLANES_STATE))
  {
    return;
  }

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
// function : PushMaterialState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::PushMaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myMaterialState.Index() == theProgram->ActiveState (OpenGl_MATERIAL_STATE))
  {
    return;
  }

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
    myContext->core11->glMaterialfv(aFrontFace, GL_AMBIENT,   aFrontMat.Ambient.GetData());
    myContext->core11->glMaterialfv(aFrontFace, GL_DIFFUSE,   aFrontMat.Diffuse.GetData());
    myContext->core11->glMaterialfv(aFrontFace, GL_SPECULAR,  aFrontMat.Specular.GetData());
    myContext->core11->glMaterialfv(aFrontFace, GL_EMISSION,  aFrontMat.Emission.GetData());
    myContext->core11->glMaterialf (aFrontFace, GL_SHININESS, aFrontMat.Shine());
    if (myMaterialState.ToDistinguish())
    {
      myContext->core11->glMaterialfv(GL_BACK, GL_AMBIENT,   aBackMat.Ambient.GetData());
      myContext->core11->glMaterialfv(GL_BACK, GL_DIFFUSE,   aBackMat.Diffuse.GetData());
      myContext->core11->glMaterialfv(GL_BACK, GL_SPECULAR,  aBackMat.Specular.GetData());
      myContext->core11->glMaterialfv(GL_BACK, GL_EMISSION,  aBackMat.Emission.GetData());
      myContext->core11->glMaterialf (GL_BACK, GL_SHININESS, aBackMat.Shine());
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

  const GLint aLocFront = theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL);
  if (aLocFront != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocFront, OpenGl_Material::NbOfVec4(),
                            aFrontMat.Packed());
  }

  const GLint aLocBack = theProgram->GetStateLocation (OpenGl_OCCT_BACK_MATERIAL);
  if (aLocBack != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocBack,  OpenGl_Material::NbOfVec4(),
                            aBackMat.Packed());
  }
}

// =======================================================================
// function : PushOitState
// purpose  : Pushes state of OIT uniforms to the specified program
// =======================================================================
void OpenGl_ShaderManager::PushOitState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (!theProgram->IsValid())
  {
    return;
  }

  if (myOitState.Index() == theProgram->ActiveState (OpenGL_OIT_STATE))
  {
    return;
  }

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
// function : PushState
// purpose  : Pushes state of OCCT graphics parameters to the program
// =======================================================================
void OpenGl_ShaderManager::PushState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  const Handle(OpenGl_ShaderProgram)& aProgram = !theProgram.IsNull() ? theProgram : myFfpProgram;
  PushClippingState    (aProgram);
  PushWorldViewState   (aProgram);
  PushModelWorldState  (aProgram);
  PushProjectionState  (aProgram);
  PushLightSourceState (aProgram);
  PushMaterialState    (aProgram);
  PushOitState         (aProgram);
}

// =======================================================================
// function : prepareStdProgramFont
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFont()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
     + EOL"THE_SHADER_OUT vec2 TexCoord;"
       EOL"void main()"
       EOL"{"
       EOL"  TexCoord = occTexCoord.st;"
       EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
       EOL"}";

  TCollection_AsciiString
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st).a; }";
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core11 == NULL)
  {
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st).r; }";
  }
#endif

  TCollection_AsciiString aSrcFrag = TCollection_AsciiString() +
     + EOL"THE_SHADER_IN vec2 TexCoord;"
     + aSrcGetAlpha
     + EOL"void main()"
       EOL"{"
       EOL"  vec4 aColor = occColor;"
       EOL"  aColor.a *= getAlpha();"
       EOL"  if (aColor.a <= 0.285) discard;"
       EOL"  occSetFragColor (aColor);"
       EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#else
  if (myContext->IsGlGreaterEqual (3, 1))
  {
    // prefer "100 es" on OpenGL ES 3.0 devices
    // and    "300 es" on newer devices (3.1+)
    aProgramSrc->SetHeader ("#version 300 es");
  }
#endif
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myFontProgram))
  {
    myFontProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramFboBlit
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFboBlit()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert =
      EOL"THE_SHADER_OUT vec2 TexCoord;"
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag =
      EOL"uniform sampler2D uColorSampler;"
      EOL"uniform sampler2D uDepthSampler;"
      EOL
      EOL"THE_SHADER_IN vec2 TexCoord;"
      EOL
      EOL"void main()"
      EOL"{"
      EOL"  gl_FragDepth = occTexture2D (uDepthSampler, TexCoord).r;"
      EOL"  occSetFragColor (occTexture2D (uColorSampler, TexCoord));"
      EOL"}";

#if defined(GL_ES_VERSION_2_0)
  if (myContext->IsGlGreaterEqual (3, 0))
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
      EOL"uniform sampler2D uColorSampler;"
      EOL
      EOL"THE_SHADER_IN vec2 TexCoord;"
      EOL
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
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myBlitProgram))
  {
    myBlitProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (myBlitProgram);
  myBlitProgram->SetSampler (myContext, "uColorSampler", Graphic3d_TextureUnit_0);
  myBlitProgram->SetSampler (myContext, "uDepthSampler", Graphic3d_TextureUnit_1);
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

  aSrcVert =
    EOL"THE_SHADER_OUT vec2 TexCoord;"
    EOL"void main()"
    EOL"{"
    EOL"  TexCoord    = occVertex.zw;"
    EOL"  gl_Position = vec4 (occVertex.x, occVertex.y, 0.0, 1.0);"
    EOL"}";

  if (!theMsaa)
  {
    aSrcFrag =
      EOL"uniform sampler2D uAccumTexture;"
      EOL"uniform sampler2D uWeightTexture;"
      EOL
      EOL"THE_SHADER_IN vec2 TexCoord;"
      EOL
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
    aSrcFrag =
      EOL"uniform sampler2DMS uAccumTexture;"
      EOL"uniform sampler2DMS uWeightTexture;"
      EOL
      EOL"THE_SHADER_IN vec2 TexCoord;"
      EOL
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

  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
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
TCollection_AsciiString OpenGl_ShaderManager::pointSpriteAlphaSrc (const Standard_Integer theBits)
{
  TCollection_AsciiString aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, " THE_VEC2_glPointCoord ").a; }";
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core11 == NULL
   && (theBits & OpenGl_PO_TextureA) != 0)
  {
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, " THE_VEC2_glPointCoord ").r; }";
  }
#else
  (void )theBits;
#endif
  return aSrcGetAlpha;
}

namespace
{

  // =======================================================================
  // function : textureUsed
  // purpose  :
  // =======================================================================
  static bool textureUsed (const Standard_Integer theBits)
  {
    return (theBits & OpenGl_PO_TextureA) != 0 || (theBits & OpenGl_PO_TextureRGB) != 0;
  }

}

// =======================================================================
// function : prepareStdProgramUnlit
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramUnlit (Handle(OpenGl_ShaderProgram)& theProgram,
                                                               const Standard_Integer        theBits)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraOut, aSrcVertExtraMain, aSrcVertExtraFunc, aSrcGetAlpha;
  TCollection_AsciiString aSrcFrag, aSrcFragExtraOut, aSrcFragExtraMain, aSrcFragWriteOit;
  TCollection_AsciiString aSrcFragGetColor     = EOL"vec4 getColor(void) { return occColor; }";
  TCollection_AsciiString aSrcFragMainGetColor = EOL"  occSetFragColor (getColor());";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D(occSamplerBaseColor, " THE_VEC2_glPointCoord "); }";
    }

    if (textureUsed (theBits))
    {
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
      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st / TexCoord.w); }";
    }
    else if ((theBits & OpenGl_PO_TextureEnv) != 0)
    {
      aSrcVertExtraOut += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut += THE_VARY_TexCoord_IN;

      aSrcVertExtraFunc = THE_FUNC_transformNormal;

      aSrcVertExtraMain +=
        EOL"  vec4 aPosition = occWorldViewMatrix * occModelWorldMatrix * occVertex;"
        EOL"  vec3 aNormal   = transformNormal (occNormal);"
        EOL"  vec3 aReflect  = reflect (normalize (aPosition.xyz), aNormal);"
        EOL"  aReflect.z += 1.0;"
        EOL"  TexCoord = vec4(aReflect.xy * inversesqrt (dot (aReflect, aReflect)) * 0.5 + vec2 (0.5), 0.0, 1.0);";

      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D (occSamplerBaseColor, TexCoord.st); }";
    }
  }
  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertExtraOut  += EOL"THE_SHADER_OUT vec4 VertColor;";
    aSrcVertExtraMain += EOL"  VertColor = occVertColor;";
    aSrcFragExtraOut  += EOL"THE_SHADER_IN  vec4 VertColor;";
    aSrcFragGetColor  =  EOL"vec4 getColor(void) { return VertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    aSrcVertExtraOut +=
      EOL"THE_SHADER_OUT vec4 PositionWorld;"
      EOL"THE_SHADER_OUT vec4 Position;";
    aSrcFragExtraOut +=
      EOL"THE_SHADER_IN  vec4 PositionWorld;"
      EOL"THE_SHADER_IN  vec4 Position;";
    aSrcVertExtraMain +=
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  Position      = occWorldViewMatrix * PositionWorld;";

    if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
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
    else
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                          ? THE_FRAG_CLIP_CHAINS_N
                          : THE_FRAG_CLIP_PLANES_N;
    }
  }
  if ((theBits & OpenGl_PO_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetWeightOitOutput (true);
  #if defined(GL_ES_VERSION_2_0)
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es");
    }
  #endif
  }

  TCollection_AsciiString aSrcVertEndMain;
  if ((theBits & OpenGl_PO_StippleLine) != 0)
  {
    bool hasGlslBitOps = false;
  #if defined(GL_ES_VERSION_2_0)
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es");
      hasGlslBitOps = true;
    }
  #else
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 130");
      hasGlslBitOps = true;
    }
    else if(myContext->CheckExtension("GL_EXT_gpu_shader4"))
    {
      aProgramSrc->SetHeader ("#extension GL_EXT_gpu_shader4 : enable");
      hasGlslBitOps = true;
    }
  #endif

    if (hasGlslBitOps)
    {
      aSrcVertExtraOut +=
        EOL"THE_SHADER_OUT vec2 ScreenSpaceCoord;";
      aSrcFragExtraOut +=
        EOL"THE_SHADER_IN  vec2 ScreenSpaceCoord;"
        EOL"uniform int   uPattern;"
        EOL"uniform float uFactor;";
      aSrcVertEndMain =
        EOL"  ScreenSpaceCoord = gl_Position.xy / gl_Position.w;";
      aSrcFragMainGetColor =
        EOL"  float anAngle      = atan (dFdx (ScreenSpaceCoord.x), dFdy (ScreenSpaceCoord.y));"
        EOL"  float aRotatePoint = gl_FragCoord.x * sin (anAngle) + gl_FragCoord.y * cos (anAngle);"
        EOL"  uint  aBit         = uint (floor (aRotatePoint / uFactor + 0.5)) & 15U;"
        EOL"  if ((uint (uPattern) & (1U << aBit)) == 0U) discard;"
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
    else
    {
      const TCollection_ExtendedString aWarnMessage =
        "Warning: stipple lines in GLSL will be ignored.";
      myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);
    }
  }

  aSrcVert =
      aSrcVertExtraFunc
    + aSrcVertExtraOut
    + EOL"void main()"
      EOL"{"
    + aSrcVertExtraMain
    + EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
    + aSrcVertEndMain
    + EOL"}";

  aSrcFrag =
      aSrcFragExtraOut
    + aSrcFragGetColor
    + aSrcGetAlpha
    + EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + aSrcFragMainGetColor
    + aSrcFragWriteOit
    + EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#else
  if (myContext->IsGlGreaterEqual (3, 1))
  {
    // prefer "100 es" on OpenGL ES 3.0 devices
    // and    "300 es" on newer devices (3.1+)
    aProgramSrc->SetHeader ("#version 300 es");
  }
#endif
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & OpenGl_PO_AlphaTest) != 0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));

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
TCollection_AsciiString OpenGl_ShaderManager::pointSpriteShadingSrc (const TCollection_AsciiString theBaseColorSrc,
                                                                     const Standard_Integer theBits)
{
  TCollection_AsciiString aSrcFragGetColor;
  if ((theBits & OpenGl_PO_TextureA) != 0)
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
  else if ((theBits & OpenGl_PO_TextureRGB) != 0)
  {
    aSrcFragGetColor = TCollection_AsciiString() +
      EOL"vec4 getColor(void)"
      EOL"{"
      EOL"  vec4 aColor = " + theBaseColorSrc + ";"
      EOL"  aColor = occTexture2D(occSamplerBaseColor, " THE_VEC2_glPointCoord ") * aColor;"
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
                                                                  Standard_Boolean  theHasVertColor)
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
    if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_DIRECTIONAL) == 1
     && theNbLights == 1)
    {
      // use the version with hard-coded first index
      aLightsLoop = EOL"    directionalLightFirst(theNormal, theView, theIsFront);";
      aLightsFunc += THE_FUNC_directionalLightFirst;
    }
    else if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_DIRECTIONAL) > 0)
    {
      aLightsFunc += THE_FUNC_directionalLight;
    }
    if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_POSITIONAL) > 0)
    {
      aLightsFunc += THE_FUNC_pointLight;
    }
    if (aLights->NbEnabledLightsOfType (Graphic3d_TOLS_SPOT) > 0)
    {
      aLightsFunc += THE_FUNC_spotLight;
    }
  }

  TCollection_AsciiString aGetMatAmbient = "theIsFront ? occFrontMaterial_Ambient()  : occBackMaterial_Ambient();";
  TCollection_AsciiString aGetMatDiffuse = "theIsFront ? occFrontMaterial_Diffuse()  : occBackMaterial_Diffuse();";
  if (theHasVertColor)
  {
    aGetMatAmbient = "getVertColor();";
    aGetMatDiffuse = "getVertColor();";
  }

  return TCollection_AsciiString()
    + THE_FUNC_lightDef
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
      EOL"  vec4 aMatEmission = theIsFront ? occFrontMaterial_Emission() : occBackMaterial_Emission();"
      EOL"  vec3 aColor = Ambient  * aMatAmbient.rgb"
      EOL"              + Diffuse  * aMatDiffuse.rgb"
      EOL"              + Specular * aMatSpecular.rgb"
      EOL"                         + aMatEmission.rgb;"
      EOL"  return vec4 (aColor, aMatDiffuse.a);"
      EOL"}";
}

// =======================================================================
// function : prepareStdProgramGouraud
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramGouraud (Handle(OpenGl_ShaderProgram)& theProgram,
                                                                 const Standard_Integer        theBits)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertColor, aSrcVertExtraOut, aSrcVertExtraMain;
  TCollection_AsciiString aSrcFrag, aSrcFragExtraOut, aSrcFragExtraMain, aSrcFragWriteOit;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return gl_FrontFacing ? FrontColor : BackColor; }";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if (textureUsed (theBits))
    {
      #if !defined(GL_ES_VERSION_2_0)
        if (myContext->core11 != NULL
         && myContext->IsGlGreaterEqual (2, 1))
        {
          aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
        }
      #endif

      aSrcFragGetColor = pointSpriteShadingSrc ("gl_FrontFacing ? FrontColor : BackColor", theBits);
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
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
    aSrcVertExtraOut +=
      EOL"THE_SHADER_OUT vec4 PositionWorld;"
      EOL"THE_SHADER_OUT vec4 Position;";
    aSrcFragExtraOut +=
      EOL"THE_SHADER_IN  vec4 PositionWorld;"
      EOL"THE_SHADER_IN  vec4 Position;";
    aSrcVertExtraMain +=
      EOL"  PositionWorld = aPositionWorld;"
      EOL"  Position      = aPosition;";

    if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
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
    else
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
  }
  if ((theBits & OpenGl_PO_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetWeightOitOutput (true);
  #if defined(GL_ES_VERSION_2_0)
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es");
    }
  #endif
  }

  Standard_Integer aNbLights = 0;
  const TCollection_AsciiString aLights = stdComputeLighting (aNbLights, (theBits & OpenGl_PO_VertColor) != 0);
  aSrcVert = TCollection_AsciiString()
    + THE_FUNC_transformNormal
    + EOL
    + aSrcVertColor
    + aLights
    + EOL
      EOL"THE_SHADER_OUT vec4 FrontColor;"
      EOL"THE_SHADER_OUT vec4 BackColor;"
      EOL
    + aSrcVertExtraOut
    + EOL"void main()"
      EOL"{"
      EOL"  vec4 aPositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  vec4 aPosition      = occWorldViewMatrix * aPositionWorld;"
      EOL"  vec3 aNormal        = transformNormal (occNormal);"
      EOL"  vec3 aView          = vec3 (0.0, 0.0, 1.0);"
      EOL"  FrontColor  = computeLighting (normalize (aNormal), normalize (aView), aPosition, true);"
      EOL"  BackColor   = computeLighting (normalize (aNormal), normalize (aView), aPosition, false);"
    + aSrcVertExtraMain
    + EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
      EOL"}";

  aSrcFrag = TCollection_AsciiString()
    + EOL"THE_SHADER_IN vec4 FrontColor;"
      EOL"THE_SHADER_IN vec4 BackColor;"
    + aSrcFragExtraOut
    + aSrcFragGetColor
    + EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + EOL"  occSetFragColor (getColor());"
    + aSrcFragWriteOit
    + EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#else
  if (myContext->IsGlGreaterEqual (3, 1))
  {
    // prefer "100 es" on OpenGL ES 3.0 devices
    // and    "300 es" on newer devices (3.1+)
    aProgramSrc->SetHeader ("#version 300 es");
  }
#endif
  aProgramSrc->SetNbLightsMax (aNbLights);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & OpenGl_PO_AlphaTest) != 0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
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
                                                               const Standard_Boolean        theIsFlatNormal)
{
  #define thePhongCompLight "computeLighting (normalize (Normal), normalize (View), Position, gl_FrontFacing)"
#if defined(GL_ES_VERSION_2_0)
  const bool isFlatNormal = theIsFlatNormal
                         && (myContext->IsGlGreaterEqual (3, 0)
                          || myContext->oesStdDerivatives);
#else
  const bool isFlatNormal = theIsFlatNormal;
#endif

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraOut, aSrcVertExtraMain;
  TCollection_AsciiString aSrcFrag, aSrcFragExtraOut, aSrcFragGetVertColor, aSrcFragExtraMain, aSrcFragWriteOit;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return " thePhongCompLight "; }";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if (textureUsed (theBits))
    {
      #if !defined(GL_ES_VERSION_2_0)
        if (myContext->core11 != NULL
         && myContext->IsGlGreaterEqual (2, 1))
        {
          aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
        }
      #endif

      aSrcFragGetColor = pointSpriteShadingSrc (thePhongCompLight, theBits);
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = " thePhongCompLight ";"
        EOL"  return occTexture2D(occSamplerBaseColor, TexCoord.st / TexCoord.w) * aColor;"
        EOL"}";
    }
  }

  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertExtraOut    += EOL"THE_SHADER_OUT vec4 VertColor;";
    aSrcVertExtraMain   += EOL"  VertColor = occVertColor;";
    aSrcFragGetVertColor = EOL"THE_SHADER_IN  vec4 VertColor;"
                           EOL"vec4 getVertColor(void) { return VertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
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
    else
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & OpenGl_PO_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
  }
  if ((theBits & OpenGl_PO_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetWeightOitOutput (true);
  }

  aSrcVert = TCollection_AsciiString()
    + (isFlatNormal ? "" : THE_FUNC_transformNormal)
    + EOL
      EOL"THE_SHADER_OUT vec4 PositionWorld;"
      EOL"THE_SHADER_OUT vec4 Position;"
      EOL"THE_SHADER_OUT vec3 View;"
    + (isFlatNormal ? ""
    : EOL"THE_SHADER_OUT vec3 Normal;")
    + EOL
    + aSrcVertExtraOut
    + EOL"void main()"
      EOL"{"
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  Position      = occWorldViewMatrix * PositionWorld;"
    + (isFlatNormal ? ""
    : EOL"  Normal        = transformNormal (occNormal);")
    + EOL"  View          = vec3 (0.0, 0.0, 1.0);"
    + aSrcVertExtraMain
    + EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
      EOL"}";

  Standard_Integer aNbLights = 0;
  const TCollection_AsciiString aLights = stdComputeLighting (aNbLights, (theBits & OpenGl_PO_VertColor) != 0);
  aSrcFrag = TCollection_AsciiString()
    + EOL"THE_SHADER_IN vec4 PositionWorld;"
      EOL"THE_SHADER_IN vec4 Position;"
      EOL"THE_SHADER_IN vec3 View;"
    + (isFlatNormal
    ? EOL"vec3 Normal;"
    : EOL"THE_SHADER_IN vec3 Normal;")
    + EOL
    + aSrcFragExtraOut
    + aSrcFragGetVertColor
    + aLights
    + aSrcFragGetColor
    + EOL
      EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + (isFlatNormal
    ? EOL"  Normal = normalize (cross (dFdx (Position.xyz / Position.w), dFdy (Position.xyz / Position.w)));"
    : "")
    + EOL"  occSetFragColor (getColor());"
    + aSrcFragWriteOit
    + EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#else
  if (myContext->IsGlGreaterEqual (3, 1))
  {
    // prefer "100 es" on OpenGL ES 3.0 devices
    // and    "300 es" on newer devices (3.1+)
    aProgramSrc->SetHeader ("#version 300 es");
  }
  else if (isFlatNormal)
  {
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es");
    }
    else if (myContext->oesStdDerivatives)
    {
      aProgramSrc->SetHeader ("#extension GL_OES_standard_derivatives : enable");
    }
    else
    {
      myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                              GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                              "Warning: flat shading requires OpenGL ES 3.0+ or GL_OES_standard_derivatives extension.");
    }
  }
#endif
  aProgramSrc->SetNbLightsMax (aNbLights);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & OpenGl_PO_AlphaTest) != 0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
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
  TCollection_AsciiString aSrcVert =
      EOL"THE_SHADER_OUT vec2 TexCoord;"
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag;
  switch (theStereoMode)
  {
    case Graphic3d_StereoMode_Anaglyph:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"uniform mat4 uMultL;"
          EOL"uniform mat4 uMultR;"
          EOL
          EOL"const vec4 THE_POW_UP   =       vec4 (2.2, 2.2, 2.2, 1.0);"
          EOL"const vec4 THE_POW_DOWN = 1.0 / vec4 (2.2, 2.2, 2.2, 1.0);"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  aColorL = pow (aColorL, THE_POW_UP);" // normalize
          EOL"  aColorR = pow (aColorR, THE_POW_UP);"
          EOL"  vec4 aColor = uMultR * aColorR + uMultL * aColorL;"
          EOL"  occSetFragColor (pow (aColor, THE_POW_DOWN));"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_RowInterlaced:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
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
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
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
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
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
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
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
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
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
    default:
    {
      /*const Handle(OpenGl_ShaderProgram)& aProgram = myStereoPrograms[Graphic3d_StereoMode_QuadBuffer];
      if (!aProgram.IsNull())
      {
        return aProgram->IsValid();
      }*/
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
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

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#else
  if (myContext->IsGlGreaterEqual (3, 1))
  {
    // prefer "100 es" on OpenGL ES 3.0 devices
    // and    "300 es" on newer devices (3.1+)
    aProgramSrc->SetHeader ("#version 300 es");
  }
#endif

  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
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
// function : bindProgramWithState
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::bindProgramWithState (const Handle(OpenGl_ShaderProgram)& theProgram)
{
  const Standard_Boolean isBound = myContext->BindProgram (theProgram);
  if (isBound
  && !theProgram.IsNull())
  {
    theProgram->ApplyVariables (myContext);
  }
  PushState (theProgram);
  return isBound;
}
