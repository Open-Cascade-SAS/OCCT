// Created on: 2013-09-26
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <typeinfo>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Clipping.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Workspace.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_ShaderManager, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShaderManager, Standard_Transient)

// =======================================================================
// function : OpenGl_ShaderManager
// purpose  : Creates new empty shader manager
// =======================================================================
OpenGl_ShaderManager::OpenGl_ShaderManager (OpenGl_Context* theContext)
: myContext (theContext),
  myIsPP    (Standard_False)
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
// function : Create
// purpose  : Creates new shader program
// =======================================================================
void OpenGl_ShaderManager::Create (const Handle(Graphic3d_ShaderProgram)& theProxy,
                                   TCollection_AsciiString&               theShareKey,
                                   Handle(OpenGl_ShaderProgram)&          theProgram)
{
  theProgram.Nullify();
  if (theProxy.IsNull())
  {
    return;
  }

  theShareKey = theProxy->GetId();
  if (myContext->GetResource<Handle(OpenGl_ShaderProgram)> (theShareKey, theProgram))
  {
    if (theProgram->Share())
    {
      myProgramList.Append (theProgram);
    }
    return;
  }

  theProgram = new OpenGl_ShaderProgram (theProxy);
  if (!theProgram->Initialize (myContext, theProxy->ShaderObjects()))
  {
    theProgram->Release (myContext);
    theShareKey.Clear();
    theProgram.Nullify();
    return;
  }

  myProgramList.Append (theProgram);
  myContext->ShareResource (theShareKey, theProgram);
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
// function : UpdateLightSourceStateTo
// purpose  : Updates state of OCCT light sources
// =======================================================================
void OpenGl_ShaderManager::UpdateLightSourceStateTo (const OpenGl_ListOfLight* theLights)
{
  myLightSourceState.Set (theLights);
  myLightSourceState.Update();
}

// =======================================================================
// function : SetProjectionState
// purpose  : Sets new state of OCCT projection transform
// =======================================================================
void OpenGl_ShaderManager::UpdateProjectionStateTo (const Tmatrix3& theProjectionMatrix)
{
  myProjectionState.Set (theProjectionMatrix);
  myProjectionState.Update();
}

// =======================================================================
// function : SetModelWorldState
// purpose  : Sets new state of OCCT model-world transform
// =======================================================================
void OpenGl_ShaderManager::UpdateModelWorldStateTo (const Tmatrix3& theModelWorldMatrix)
{
  myModelWorldState.Set (theModelWorldMatrix);
  myModelWorldState.Update();
}

// =======================================================================
// function : SetWorldViewState
// purpose  : Sets new state of OCCT world-view transform
// =======================================================================
void OpenGl_ShaderManager::UpdateWorldViewStateTo (const Tmatrix3& theWorldViewMatrix)
{
  myWorldViewState.Set (theWorldViewMatrix);
  myWorldViewState.Update();
}

// =======================================================================
// function : RevertProjectionStateTo
// purpose  : Reverts state of OCCT projection transform
// =======================================================================
void OpenGl_ShaderManager::RevertProjectionStateTo (const Tmatrix3& theProjectionMatrix)
{
  myProjectionState.Set (theProjectionMatrix);
  myProjectionState.Revert();
}

// =======================================================================
// function : RevertModelWorldStateTo
// purpose  : Reverts state of OCCT model-world transform
// =======================================================================
void OpenGl_ShaderManager::RevertModelWorldStateTo (const Tmatrix3& theModelWorldMatrix)
{
  myModelWorldState.Set (theModelWorldMatrix);
  myModelWorldState.Revert();
}

// =======================================================================
// function : RevertWorldViewStateTo
// purpose  : Reverts state of OCCT world-view transform
// =======================================================================
void OpenGl_ShaderManager::RevertWorldViewStateTo (const Tmatrix3& theWorldViewMatrix)
{
  myWorldViewState.Set (theWorldViewMatrix);
  myWorldViewState.Revert();
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
    if (aLight.Type == Visual3d_TOLS_AMBIENT)
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
    aLightParams.Position = aLight.Type == Visual3d_TOLS_DIRECTIONAL
                         ? -aLight.Direction
                         :  aLight.Position;
    if (aLight.Type == Visual3d_TOLS_SPOT)
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

  GLuint aPlanesNb = 0;
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

  OpenGl_Vec4* anEquations = new OpenGl_Vec4[aPlanesNb];
  GLint*       aSpaces     = new GLint      [aPlanesNb];
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
  theProgram->SetUniform (myContext, aLocEquations, aPlanesNb, anEquations[0].GetData());
  theProgram->SetUniform (myContext, aLocSpaces,    aPlanesNb, aSpaces);

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
    myMaterialStates.ChangeFind (theProgram).Set (theAspect);
  }
  else
  {
    myMaterialStates.Bind (theProgram, OpenGl_MaterialState (theAspect));
  }

  myMaterialStates.ChangeFind (theProgram).Update();
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

    aParams.Init (anIndex == 0 ? theAspect->IntFront() : theAspect->IntBack());
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
// function : PushWorldViewState
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
