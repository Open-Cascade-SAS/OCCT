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
#include <OpenGl_Clipping.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Workspace.hxx>

#include <TCollection_ExtendedString.hxx>




namespace
{

#define EOL "\n"

//! Definition of VertColor varying.
const char THE_VARY_TexCoord_OUT[] =
  EOL"THE_SHADER_OUT vec2 TexCoord;";
const char THE_VARY_TexCoord_IN[] =
  EOL"THE_SHADER_IN  vec2 TexCoord;";

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
  EOL"    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 1.0));"
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
  EOL"    aLight   = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight,   1.0));"
  EOL"    aSpotDir = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aSpotDir, 0.0));"
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
  EOL"    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 0.0));"
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

//! Process clipping planes in Fragment Shader.
//! Should be added at the beginning of the main() function.
const char THE_FRAG_CLIP_PLANES[] =
  EOL"  for (int aPlaneIter = 0; aPlaneIter < occClipPlaneCount; ++aPlaneIter)"
  EOL"  {"
  EOL"    vec4 aClipEquation = occClipPlaneEquations[aPlaneIter];"
  EOL"    int  aClipSpace    = occClipPlaneSpaces[aPlaneIter];"
  EOL"    if (aClipSpace == OccEquationCoords_World)"
  EOL"    {"
  EOL"      if (dot (aClipEquation.xyz, PositionWorld.xyz) + aClipEquation.w < 0.0)"
  EOL"      {"
  EOL"        discard;"
  EOL"      }"
  EOL"    }"
  EOL"    else if (aClipSpace == OccEquationCoords_View)"
  EOL"    {"
  EOL"      if (dot (aClipEquation.xyz, Position.xyz) + aClipEquation.w < 0.0)"
  EOL"      {"
  EOL"        discard;"
  EOL"      }"
  EOL"    }"
  EOL"  }";

}

// =======================================================================
// function : OpenGl_ShaderManager
// purpose  : Creates new empty shader manager
// =======================================================================
OpenGl_ShaderManager::OpenGl_ShaderManager (OpenGl_Context* theContext)
: myShadingModel (Graphic3d_TOSM_VERTEX),
  myContext  (theContext),
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
  myFlatPrograms = OpenGl_SetOfShaderPrograms();
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
      myMaterialStates.UnBind (theProgram);
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
  TCollection_AsciiString aKey (myShadingModel == Graphic3d_TOSM_FRAGMENT ? "p_" : "g_");
  const OpenGl_ListOfLight* aLights = myLightSourceState.LightSources();
  if (aLights != NULL)
  {
    for (OpenGl_ListOfLight::Iterator aLightIter (*aLights); aLightIter.More(); aLightIter.Next())
    {
      switch (aLightIter.Value().Type)
      {
        case Graphic3d_TOLS_AMBIENT:
          break; // skip ambient
        case Graphic3d_TOLS_DIRECTIONAL:
          aKey += "d";
          break;
        case Graphic3d_TOLS_POSITIONAL:
          aKey += "p";
          break;
        case Graphic3d_TOLS_SPOT:
          aKey += "s";
          break;
      }
    }
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
void OpenGl_ShaderManager::UpdateLightSourceStateTo (const OpenGl_ListOfLight* theLights)
{
  myLightSourceState.Set (theLights);
  myLightSourceState.Update();
  switchLightPrograms();
}

// =======================================================================
// function : SetShadingModel
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::SetShadingModel (const Graphic3d_TypeOfShadingModel theModel)
{
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
// function : LightSourceState
// purpose  : Returns current state of OCCT light sources
// =======================================================================
const OpenGl_LightSourceState& OpenGl_ShaderManager::LightSourceState() const
{
  return myLightSourceState;
}

// =======================================================================
// function : ProjectionState
// purpose  : Returns current state of OCCT projection transform
// =======================================================================
const OpenGl_ProjectionState& OpenGl_ShaderManager::ProjectionState() const
{
  return myProjectionState;
}

// =======================================================================
// function : ModelWorldState
// purpose  : Returns current state of OCCT model-world transform
// =======================================================================
const OpenGl_ModelWorldState& OpenGl_ShaderManager::ModelWorldState() const
{
  return myModelWorldState;
}

// =======================================================================
// function : WorldViewState
// purpose  : Returns current state of OCCT world-view transform
// =======================================================================
const OpenGl_WorldViewState& OpenGl_ShaderManager::WorldViewState() const
{
  return myWorldViewState;
}

//! Packed properties of light source
class OpenGl_ShaderLightParameters
{
public:

  OpenGl_Vec4 Color;
  OpenGl_Vec4 Position;
  OpenGl_Vec4 Direction;
  OpenGl_Vec4 Parameters;

  //! Returns packed (serialized) representation of light source properties
  const OpenGl_Vec4* Packed() const { return reinterpret_cast<const OpenGl_Vec4*> (this); }
  static Standard_Integer NbOfVec4() { return 4; }

};

//! Packed light source type information
class OpenGl_ShaderLightType
{
public:

  Standard_Integer Type;
  Standard_Integer IsHeadlight;

  //! Returns packed (serialized) representation of light source type
  const OpenGl_Vec2i* Packed() const { return reinterpret_cast<const OpenGl_Vec2i*> (this); }
  static Standard_Integer NbOfVec2i() { return 1; }

};

// =======================================================================
// function : PushLightSourceState
// purpose  : Pushes state of OCCT light sources to the program
// =======================================================================
void OpenGl_ShaderManager::PushLightSourceState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myLightSourceState.Index() == theProgram->ActiveState (OpenGl_LIGHT_SOURCES_STATE)
   || !theProgram->IsValid())
  {
    return;
  }

  OpenGl_ShaderLightType* aLightTypeArray = new OpenGl_ShaderLightType[OpenGLMaxLights];
  for (Standard_Integer aLightIt = 0; aLightIt < OpenGLMaxLights; ++aLightIt)
  {
    aLightTypeArray[aLightIt].Type = -1;
  }

  const Standard_Integer aLightsDefNb = Min (myLightSourceState.LightSources()->Size(), OpenGLMaxLights);
  if (aLightsDefNb < 1)
  {
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_COUNT),
                            0);
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_AMBIENT),
                            OpenGl_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_TYPES),
                            OpenGLMaxLights * OpenGl_ShaderLightType::NbOfVec2i(),
                            aLightTypeArray[0].Packed());
    theProgram->UpdateState (OpenGl_LIGHT_SOURCES_STATE, myLightSourceState.Index());
    delete[] aLightTypeArray;
    return;
  }

  OpenGl_ShaderLightParameters* aLightParamsArray = new OpenGl_ShaderLightParameters[aLightsDefNb];

  OpenGl_Vec4 anAmbient (0.0f, 0.0f, 0.0f, 0.0f);
  Standard_Integer aLightsNb = 0;
  for (OpenGl_ListOfLight::Iterator anIter (*myLightSourceState.LightSources()); anIter.More(); anIter.Next())
  {
    const OpenGl_Light& aLight = anIter.Value();
    if (aLight.Type == Graphic3d_TOLS_AMBIENT)
    {
      anAmbient += aLight.Color;
      continue;
    }
    else if (aLightsNb >= OpenGLMaxLights)
    {
      continue;
    }

    OpenGl_ShaderLightType& aLightType = aLightTypeArray[aLightsNb];
    aLightType.Type        = aLight.Type;
    aLightType.IsHeadlight = aLight.IsHeadlight;

    OpenGl_ShaderLightParameters& aLightParams = aLightParamsArray[aLightsNb];
    aLightParams.Color    = aLight.Color;
    aLightParams.Position = aLight.Type == Graphic3d_TOLS_DIRECTIONAL
                         ? -aLight.Direction
                         :  aLight.Position;
    if (aLight.Type == Graphic3d_TOLS_SPOT)
    {
      aLightParams.Direction = aLight.Direction;
    }
    aLightParams.Parameters = aLight.Params;
    ++aLightsNb;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_COUNT),
                          aLightsNb);
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_AMBIENT),
                          anAmbient);
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_TYPES),
                          OpenGLMaxLights * OpenGl_ShaderLightType::NbOfVec2i(),
                          aLightTypeArray[0].Packed());
  if (aLightsNb > 0)
  {
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_PARAMS),
                            aLightsNb * OpenGl_ShaderLightParameters::NbOfVec4(),
                            aLightParamsArray[0].Packed());
  }
  delete[] aLightParamsArray;
  delete[] aLightTypeArray;

  theProgram->UpdateState (OpenGl_LIGHT_SOURCES_STATE, myLightSourceState.Index());
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

  theProgram->UpdateState (OpenGl_PROJECTION_STATE, myProjectionState.Index());
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

  theProgram->UpdateState (OpenGl_MODEL_WORLD_STATE, myModelWorldState.Index());
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

  theProgram->UpdateState (OpenGl_WORLD_VIEW_STATE, myWorldViewState.Index());
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
  const GLint aLocEquations = theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_EQUATIONS);
  const GLint aLocSpaces    = theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_SPACES);
  if (aLocEquations == OpenGl_ShaderProgram::INVALID_LOCATION
   && aLocSpaces    == OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    return;
  }

  GLint aPlanesNb = 0;
  for (Graphic3d_SequenceOfHClipPlane::Iterator anIter (myContext->Clipping().Planes());
       anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = anIter.Value();
    if (!myContext->Clipping().IsEnabled (aPlane))
    {
      continue;
    }

    ++aPlanesNb;
  }
  if (aPlanesNb < 1)
  {
    return;
  }

  const Standard_Size MAX_CLIP_PLANES = 8;
  OpenGl_Vec4* anEquations = new OpenGl_Vec4[MAX_CLIP_PLANES];
  GLint*       aSpaces     = new GLint      [MAX_CLIP_PLANES];
  GLuint aPlaneId = 0;
  for (Graphic3d_SequenceOfHClipPlane::Iterator anIter (myContext->Clipping().Planes());
       anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = anIter.Value();
    if (!myContext->Clipping().IsEnabled (aPlane))
    {
      continue;
    }

    const Graphic3d_ClipPlane::Equation& anEquation = aPlane->GetEquation();
    anEquations[aPlaneId] = OpenGl_Vec4 ((float) anEquation.x(),
                                         (float) anEquation.y(),
                                         (float) anEquation.z(),
                                         (float) anEquation.w());
    aSpaces[aPlaneId] = myContext->Clipping().GetEquationSpace (aPlane);
    ++aPlaneId;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_COUNT),
                          aPlanesNb);
  theProgram->SetUniform (myContext, aLocEquations, MAX_CLIP_PLANES, anEquations);
  theProgram->SetUniform (myContext, aLocSpaces,    MAX_CLIP_PLANES, aSpaces);

  delete[] anEquations;
  delete[] aSpaces;
}

// =======================================================================
// function : UpdateMaterialStateTo
// purpose  : Updates state of OCCT material for specified program
// =======================================================================
void OpenGl_ShaderManager::UpdateMaterialStateTo (const Handle(OpenGl_ShaderProgram)& theProgram,
                                                  const OpenGl_Element*               theAspect)
{
  if (myMaterialStates.IsBound (theProgram))
  {
    OpenGl_MaterialState& aState = myMaterialStates.ChangeFind (theProgram);
    aState.Set (theAspect);
    aState.Update();
  }
  else
  {
    myMaterialStates.Bind       (theProgram, OpenGl_MaterialState (theAspect));
    myMaterialStates.ChangeFind (theProgram).Update();
  }
}

// =======================================================================
// function : ResetMaterialStates
// purpose  : Resets state of OCCT material for all programs
// =======================================================================
void OpenGl_ShaderManager::ResetMaterialStates()
{
  for (OpenGl_ShaderProgramList::Iterator anIt (myProgramList); anIt.More(); anIt.Next())
  {
    anIt.Value()->UpdateState (OpenGl_MATERIALS_STATE, 0);
  }
}

// =======================================================================
// function : MaterialState
// purpose  : Returns state of OCCT material for specified program
// =======================================================================
const OpenGl_MaterialState* OpenGl_ShaderManager::MaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (!myMaterialStates.IsBound (theProgram))
    return NULL;

  return &myMaterialStates.Find (theProgram);
}

// =======================================================================
// function : SurfaceDetailState
// purpose  : Returns current state of OCCT surface detail
// =======================================================================
const OpenGl_SurfaceDetailState& OpenGl_ShaderManager::SurfaceDetailState() const
{
  return mySurfaceDetailState;
}

// =======================================================================
// function : UpdateSurfaceDetailStateTo
// purpose  : Updates state of OCCT surface detail
// =======================================================================
void OpenGl_ShaderManager::UpdateSurfaceDetailStateTo (const Graphic3d_TypeOfSurfaceDetail theDetail)
{
  mySurfaceDetailState.Set (theDetail);
  mySurfaceDetailState.Update();
}

namespace
{

static const OpenGl_Vec4 THE_COLOR_BLACK_VEC4 (0.0f, 0.0f, 0.0f, 0.0f);

// =======================================================================
// function : PushAspectFace
// purpose  :
// =======================================================================
static void PushAspectFace (const Handle(OpenGl_Context)&       theCtx,
                            const Handle(OpenGl_ShaderProgram)& theProgram,
                            const OpenGl_AspectFace*            theAspect)
{
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_TEXTURE_ENABLE),
                          theAspect->DoTextureMap());
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_ACTIVE_SAMPLER),
                          0 /* GL_TEXTURE0 */);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_DISTINGUISH_MODE),
                          theAspect->DistinguishingMode());

  OpenGl_Material aParams;
  for (Standard_Integer anIndex = 0; anIndex < 2; ++anIndex)
  {
    const GLint aLoc = theProgram->GetStateLocation (anIndex == 0
                                                   ? OpenGl_OCCT_FRONT_MATERIAL
                                                   : OpenGl_OCCT_BACK_MATERIAL);
    if (aLoc == OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      continue;
    }

    const OPENGL_SURF_PROP& aProp = anIndex == 0 || theAspect->DistinguishingMode() != TOn
                                  ? theAspect->IntFront()
                                  : theAspect->IntBack();
    aParams.Init (aProp);
    aParams.Diffuse.a() = aProp.trans;
    theProgram->SetUniform (theCtx, aLoc, OpenGl_Material::NbOfVec4(),
                            aParams.Packed());
  }
}

// =======================================================================
// function : PushAspectLine
// purpose  :
// =======================================================================
static void PushAspectLine (const Handle(OpenGl_Context)&       theCtx,
                            const Handle(OpenGl_ShaderProgram)& theProgram,
                            const OpenGl_AspectLine*            theAspect)
{
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_TEXTURE_ENABLE),   TOff);
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_DISTINGUISH_MODE), TOff);

  const OpenGl_Vec4 aDiffuse (theAspect->Color().rgb[0],
                              theAspect->Color().rgb[1],
                              theAspect->Color().rgb[2],
                              theAspect->Color().rgb[3]);
  OpenGl_Vec4 aParams[5];
  aParams[0] = THE_COLOR_BLACK_VEC4;
  aParams[1] = THE_COLOR_BLACK_VEC4;
  aParams[2] = aDiffuse;
  aParams[3] = THE_COLOR_BLACK_VEC4;
  aParams[4].x() = 0.0f; // shininess
  aParams[4].y() = 0.0f; // transparency
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL),
                          5, aParams);
}

// =======================================================================
// function : PushAspectText
// purpose  :
// =======================================================================
static void PushAspectText (const Handle(OpenGl_Context)&       theCtx,
                            const Handle(OpenGl_ShaderProgram)& theProgram,
                            const OpenGl_AspectText*            theAspect)
{
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_TEXTURE_ENABLE),   TOn);
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_DISTINGUISH_MODE), TOff);
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_ACTIVE_SAMPLER),   0 /* GL_TEXTURE0 */);

  OpenGl_Vec4 aDiffuse (theAspect->Color().rgb[0],
                        theAspect->Color().rgb[1],
                        theAspect->Color().rgb[2],
                        theAspect->Color().rgb[3]);
  if (theAspect->DisplayType() == Aspect_TODT_DEKALE
   || theAspect->DisplayType() == Aspect_TODT_SUBTITLE)
  {
    aDiffuse = OpenGl_Vec4 (theAspect->SubtitleColor().rgb[0],
                            theAspect->SubtitleColor().rgb[1],
                            theAspect->SubtitleColor().rgb[2],
                            theAspect->SubtitleColor().rgb[3]);
  }

  OpenGl_Vec4 aParams[5];
  aParams[0] = THE_COLOR_BLACK_VEC4;
  aParams[1] = THE_COLOR_BLACK_VEC4;
  aParams[2] = aDiffuse;
  aParams[3] = THE_COLOR_BLACK_VEC4;
  aParams[4].x() = 0.0f; // shininess
  aParams[4].y() = 0.0f; // transparency
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL),
                          5, aParams);
}

// =======================================================================
// function : PushAspectMarker
// purpose  :
// =======================================================================
static void PushAspectMarker (const Handle(OpenGl_Context)&       theCtx,
                              const Handle(OpenGl_ShaderProgram)& theProgram,
                              const OpenGl_AspectMarker*          theAspect)
{
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_TEXTURE_ENABLE),   TOn);
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_DISTINGUISH_MODE), TOff);
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_ACTIVE_SAMPLER),   0 /* GL_TEXTURE0 */);

  const OpenGl_Vec4 aDiffuse (theAspect->Color().rgb[0],
                              theAspect->Color().rgb[1],
                              theAspect->Color().rgb[2],
                              theAspect->Color().rgb[3]);
  OpenGl_Vec4 aParams[5];
  aParams[0] = THE_COLOR_BLACK_VEC4;
  aParams[1] = THE_COLOR_BLACK_VEC4;
  aParams[2] = aDiffuse;
  aParams[3] = THE_COLOR_BLACK_VEC4;
  aParams[4].x() = 0.0f; // shininess
  aParams[4].y() = 0.0f; // transparency
  theProgram->SetUniform (theCtx, theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL),
                          5, aParams);
}

}; // nameless namespace

// =======================================================================
// function : PushMaterialState
// purpose  : Pushes current state of OCCT material to the program
// =======================================================================
void OpenGl_ShaderManager::PushMaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (!myMaterialStates.IsBound (theProgram))
  {
    return;
  }

  const OpenGl_MaterialState& aState = myMaterialStates.Find (theProgram);
  if (aState.Index() == theProgram->ActiveState (OpenGl_MATERIALS_STATE))
  {
    return;
  }

  if (typeid (*aState.Aspect()) == typeid (OpenGl_AspectFace))
  {
    PushAspectFace   (myContext, theProgram, dynamic_cast<const OpenGl_AspectFace*> (aState.Aspect()));
  }
  else if (typeid (*aState.Aspect()) == typeid (OpenGl_AspectLine))
  {
    PushAspectLine   (myContext, theProgram, dynamic_cast<const OpenGl_AspectLine*> (aState.Aspect()));
  }
  else if (typeid (*aState.Aspect()) == typeid (OpenGl_AspectText))
  {
    PushAspectText   (myContext, theProgram, dynamic_cast<const OpenGl_AspectText*> (aState.Aspect()));
  }
  else if (typeid (*aState.Aspect()) == typeid (OpenGl_AspectMarker))
  {
    PushAspectMarker (myContext, theProgram, dynamic_cast<const OpenGl_AspectMarker*> (aState.Aspect()));
  }

  theProgram->UpdateState (OpenGl_MATERIALS_STATE, aState.Index());
}

// =======================================================================
// function : PushState
// purpose  : Pushes state of OCCT graphics parameters to the program
// =======================================================================
void OpenGl_ShaderManager::PushState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  PushClippingState    (theProgram);
  PushMaterialState    (theProgram);
  PushWorldViewState   (theProgram);
  PushModelWorldState  (theProgram);
  PushProjectionState  (theProgram);
  PushLightSourceState (theProgram);
}

// =======================================================================
// function : prepareStdProgramFont
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFont()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
     + THE_VARY_TexCoord_OUT
     + EOL"void main()"
       EOL"{"
       EOL"  TexCoord = occTexCoord.st;"
       EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
       EOL"}";

  TCollection_AsciiString
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, TexCoord.st).a; }";
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core11 == NULL)
  {
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, TexCoord.st).r; }";
  }
#endif

  TCollection_AsciiString aSrcFrag = TCollection_AsciiString() +
     + THE_VARY_TexCoord_IN
     + aSrcGetAlpha
     + EOL"void main()"
       EOL"{"
       EOL"  vec4 aColor = occColor;"
       EOL"  aColor.a *= getAlpha();"
       EOL"  if (aColor.a <= 0.285) discard;"
       EOL"  occFragColor = aColor;"
       EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
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
      EOL"  occFragColor = occTexture2D (uColorSampler, TexCoord);"
      EOL"}";

#if defined(GL_ES_VERSION_2_0)
  if (myContext->IsGlGreaterEqual (3, 0))
  {
    aProgramSrc->SetHeader ("#version 300 es");
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
      EOL"  occFragColor = occTexture2D (uColorSampler, TexCoord);"
      EOL"}";
  }
#else
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myBlitProgram))
  {
    myBlitProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (myBlitProgram);
  myBlitProgram->SetSampler (myContext, "uColorSampler", 0);
  myBlitProgram->SetSampler (myContext, "uDepthSampler", 1);
  myContext->BindProgram (NULL);
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramFlat
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFlat (Handle(OpenGl_ShaderProgram)& theProgram,
                                                              const Standard_Integer        theBits)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraOut, aSrcVertExtraMain, aSrcVertExtraFunc, aSrcFrag, aSrcFragExtraOut, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor     = EOL"vec4 getColor(void) { return occColor; }";
  TCollection_AsciiString aSrcFragMainGetColor = EOL"  occFragColor = getColor();";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif
    if ((theBits & OpenGl_PO_TextureA) != 0)
    {
      TCollection_AsciiString
        aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, gl_PointCoord).a; }";
    #if !defined(GL_ES_VERSION_2_0)
      if (myContext->core11 == NULL)
      {
        aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, gl_PointCoord).r; }";
      }
    #endif

      aSrcFragGetColor = aSrcGetAlpha
      + EOL"vec4  getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = occColor;"
        EOL"  aColor.a *= getAlpha();"
        EOL"  return aColor;"
        EOL"}";

      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occFragColor = aColor;";
    }
    else if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D(occActiveSampler, gl_PointCoord); }";
      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occFragColor = aColor;";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain +=
        EOL"  TexCoord = occTexCoord.st;";

      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D(occActiveSampler, TexCoord.st); }";
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
        EOL"  TexCoord = aReflect.xy * inversesqrt (dot (aReflect, aReflect)) * 0.5 + vec2 (0.5);";

      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D (occActiveSampler, TexCoord.st); }";
    }
  }
  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertExtraOut  += EOL"THE_SHADER_OUT vec4 VertColor;";
    aSrcVertExtraMain += EOL"  VertColor = occVertColor;";
    aSrcFragExtraOut  += EOL"THE_SHADER_IN  vec4 VertColor;";
    aSrcFragGetColor  =  EOL"vec4 getColor(void) { return VertColor; }";
  }
  if ((theBits & OpenGl_PO_ClipPlanes) != 0)
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
    aSrcFragExtraMain += THE_FRAG_CLIP_PLANES;
  }

  TCollection_AsciiString aSrcVertEndMain;
  if ((theBits & OpenGl_PO_StippleLine) != 0)
  {
    bool hasCaps = false;
  #if defined(GL_ES_VERSION_2_0)
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es");
      hasCaps = true;
    }
  #else
    if (myContext->core32 != NULL)
    {
      aProgramSrc->SetHeader ("#version 150");
      hasCaps = true;
    }
  #endif

    if (hasCaps)
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
        EOL"  occFragColor = aColor;";
    }
    else
    {
      const TCollection_ExtendedString aWarnMessage =
        "Warning: stipple lines in GLSL will be ignored.";
      myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
        GL_DEBUG_TYPE_PORTABILITY_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aWarnMessage);
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
    + EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + aSrcFragMainGetColor
    + EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
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
// function : stdComputeLighting
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::stdComputeLighting (const Standard_Boolean theHasVertColor)
{
  bool aLightsMap[Graphic3d_TOLS_SPOT + 1] = { false, false, false, false };
  TCollection_AsciiString aLightsFunc, aLightsLoop;
  const OpenGl_ListOfLight* aLights = myLightSourceState.LightSources();
  if (aLights != NULL)
  {
    Standard_Integer anIndex = 0;
    for (OpenGl_ListOfLight::Iterator aLightIter (*aLights); aLightIter.More(); aLightIter.Next(), ++anIndex)
    {
      switch (aLightIter.Value().Type)
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

      bool& aTypeBit = aLightsMap[aLightIter.Value().Type];
      if (aTypeBit)
      {
        continue;
      }

      aTypeBit = true;
      switch (aLightIter.Value().Type)
      {
        case Graphic3d_TOLS_AMBIENT:     break;
        case Graphic3d_TOLS_DIRECTIONAL: aLightsFunc += THE_FUNC_directionalLight; break;
        case Graphic3d_TOLS_POSITIONAL:  aLightsFunc += THE_FUNC_pointLight;       break;
        case Graphic3d_TOLS_SPOT:        aLightsFunc += THE_FUNC_spotLight;        break;
      }
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
  TCollection_AsciiString aSrcVert, aSrcVertColor, aSrcVertExtraOut, aSrcVertExtraMain, aSrcFrag, aSrcFragExtraOut, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return gl_FrontFacing ? FrontColor : BackColor; }";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif
  }
  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertColor = EOL"vec4 getVertColor(void) { return occVertColor; }";
  }
  if ((theBits & OpenGl_PO_Point) != 0)
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = gl_FrontFacing ? FrontColor : BackColor;"
        EOL"  return occTexture2D(occActiveSampler, gl_PointCoord) * aColor;"
        EOL"}";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain +=
        EOL"  TexCoord = occTexCoord.st;";

      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = gl_FrontFacing ? FrontColor : BackColor;"
        EOL"  return occTexture2D(occActiveSampler, TexCoord.st) * aColor;"
        EOL"}";
    }
  }
  if ((theBits & OpenGl_PO_ClipPlanes) != 0)
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
    aSrcFragExtraMain += THE_FRAG_CLIP_PLANES;
  }

  const TCollection_AsciiString aLights = stdComputeLighting ((theBits & OpenGl_PO_VertColor) != 0);
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
    + EOL"  occFragColor = getColor();"
      EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
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
                                                               const Standard_Integer        theBits)
{
  #define thePhongCompLight "computeLighting (normalize (Normal), normalize (View), Position, gl_FrontFacing)"

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraOut, aSrcVertExtraMain, aSrcFrag, aSrcFragExtraOut, aSrcFragGetVertColor, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return " thePhongCompLight "; }";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif
  }
  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertExtraOut    += EOL"THE_SHADER_OUT vec4 VertColor;";
    aSrcVertExtraMain   += EOL"  VertColor = occVertColor;";
    aSrcFragGetVertColor = EOL"THE_SHADER_IN  vec4 VertColor;"
                           EOL"vec4 getVertColor(void) { return VertColor; }";
  }

  if ((theBits & OpenGl_PO_Point) != 0)
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = " thePhongCompLight ";"
        EOL"  return occTexture2D(occActiveSampler, gl_PointCoord) * aColor;"
        EOL"}";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain +=
        EOL"  TexCoord = occTexCoord.st;";

      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = " thePhongCompLight ";"
        EOL"  return occTexture2D(occActiveSampler, TexCoord.st) * aColor;"
        EOL"}";
    }
  }

  if ((theBits & OpenGl_PO_ClipPlanes) != 0)
  {
    aSrcFragExtraMain += THE_FRAG_CLIP_PLANES;
  }

  aSrcVert = TCollection_AsciiString()
    + THE_FUNC_transformNormal
    + EOL
      EOL"THE_SHADER_OUT vec4 PositionWorld;"
      EOL"THE_SHADER_OUT vec4 Position;"
      EOL"THE_SHADER_OUT vec3 Normal;"
      EOL"THE_SHADER_OUT vec3 View;"
      EOL
    + aSrcVertExtraOut
    + EOL"void main()"
      EOL"{"
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  Position      = occWorldViewMatrix * PositionWorld;"
      EOL"  Normal        = transformNormal (occNormal);"
      EOL"  View          = vec3 (0.0, 0.0, 1.0);"
    + aSrcVertExtraMain
    + EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
      EOL"}";

  const TCollection_AsciiString aLights = stdComputeLighting ((theBits & OpenGl_PO_VertColor) != 0);
  aSrcFrag = TCollection_AsciiString()
    + EOL"THE_SHADER_IN vec4 PositionWorld;"
      EOL"THE_SHADER_IN vec4 Position;"
      EOL"THE_SHADER_IN vec3 Normal;"
      EOL"THE_SHADER_IN vec3 View;"
    + EOL
    + aSrcFragExtraOut
    + aSrcFragGetVertColor
    + aLights
    + aSrcFragGetColor
    + EOL
      EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + EOL"  occFragColor = getColor();"
      EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
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
          EOL"vec4 THE_POW_UP   = vec4 (2.2, 2.2, 2.2, 1.0);"
          EOL"vec4 THE_POW_DOWN = 1.0 / THE_POW_UP;"
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
          EOL"  occFragColor = pow (aColor, THE_POW_DOWN);"
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
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
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
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
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
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
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
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
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
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
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
          EOL"  occFragColor = aColorL + aColorR;"
          EOL"}";
      break;
    }
  }

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif

  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (theProgram);
  theProgram->SetSampler (myContext, "uLeftSampler",  0);
  theProgram->SetSampler (myContext, "uRightSampler", 1);
  myContext->BindProgram (NULL);
  return Standard_True;
}

// =======================================================================
// function : bindProgramWithState
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::bindProgramWithState (const Handle(OpenGl_ShaderProgram)& theProgram,
                                                             const OpenGl_Element*               theAspect)
{
  if (!myContext->BindProgram (theProgram))
  {
    return Standard_False;
  }
  theProgram->ApplyVariables (myContext);

  const OpenGl_MaterialState* aMaterialState = MaterialState (theProgram);
  if (aMaterialState == NULL || aMaterialState->Aspect() != theAspect)
  {
    UpdateMaterialStateTo (theProgram, theAspect);
  }

  PushState (theProgram);
  return Standard_True;
}
