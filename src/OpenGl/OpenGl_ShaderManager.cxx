// Created on: 2013-09-26
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

#include <typeinfo>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Clipping.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>

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
Handle(OpenGl_ShaderProgram) OpenGl_ShaderManager::Create (const Handle(Graphic3d_ShaderProgram)& theProxyProgram)
{
  if (theProxyProgram.IsNull())
  {
    return NULL;
  }
  
  Handle(OpenGl_ShaderProgram) aProgram = new OpenGl_ShaderProgram (theProxyProgram);
  if (!aProgram->Initialize (myContext, theProxyProgram->ShaderObjects()))
  {
    return NULL;
  }
  
  myProgramList.Append (aProgram);

  return aProgram;
}

// =======================================================================
// function : Unregister
// purpose  : Removes specified shader program from the manager
// =======================================================================
void OpenGl_ShaderManager::Unregister (Handle(OpenGl_ShaderProgram)& theProgram)
{
  for (OpenGl_ShaderProgramList::Iterator anIt (myProgramList); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theProgram)
    {
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
    myContext->ReleaseResource (anID);
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
  
  theProgram->SetUniform (myContext, theProgram->GetStateLocation (
    OpenGl_OCC_LIGHT_SOURCE_COUNT), myLightSourceState.LightSources()->Size());

  OpenGl_ListOfLight::Iterator anIter (*myLightSourceState.LightSources());
  for (unsigned int anIndex = 0; anIter.More(); anIter.Next())
  {
    if (anIndex >= OpenGLMaxLights)
    {
      break;
    }

    const OpenGl_Light& aLight = anIter.Value();
    if (aLight.type == TLightAmbient)
    {
      OpenGl_Vec3 anAmbient (aLight.col.rgb[0],
                             aLight.col.rgb[1],
                             aLight.col.rgb[2]);

      theProgram->SetUniform (myContext,
        theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_AMBIENT + anIndex), anAmbient);

      anIter.Next();
      if (!anIter.More())
      {
        theProgram->SetUniform (myContext,
          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_TYPE + anIndex), int (aLight.type));
        break;
      }
    }

    OpenGl_Vec3 aDiffuse (aLight.col.rgb[0],
                          aLight.col.rgb[1],
                          aLight.col.rgb[2]);

    OpenGl_Vec3 aPosition (aLight.type == TLightDirectional ? -aLight.dir[0] : aLight.pos[0],
                           aLight.type == TLightDirectional ? -aLight.dir[1] : aLight.pos[1],
                           aLight.type == TLightDirectional ? -aLight.dir[2] : aLight.pos[2]);

    theProgram->SetUniform (myContext,
      theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_TYPE + anIndex), int (aLight.type));

    theProgram->SetUniform (myContext,
      theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_HEAD + anIndex), aLight.HeadLight);

    theProgram->SetUniform (myContext,
      theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_DIFFUSE + anIndex), aDiffuse);

    theProgram->SetUniform (myContext,
      theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_SPECULAR + anIndex), aDiffuse);

    theProgram->SetUniform (myContext,
      theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_POSITION + anIndex), aPosition);

    theProgram->SetUniform (myContext,
      theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_CONST_ATTENUATION + anIndex), aLight.atten[0]);

    theProgram->SetUniform (myContext,
      theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_LINEAR_ATTENUATION + anIndex), aLight.atten[1]);

    if (aLight.type == TLightSpot)
    {
      OpenGl_Vec3 aDirection (aLight.dir[0],
                              aLight.dir[1],
                              aLight.dir[2]);

      theProgram->SetUniform (myContext,
        theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_SPOT_CUTOFF + anIndex), aLight.angle);

      theProgram->SetUniform (myContext,
        theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_SPOT_EXPONENT + anIndex), aLight.shine);

      theProgram->SetUniform (myContext,
        theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_0_SPOT_DIRECTION + anIndex), aDirection);
    }

    ++anIndex;
  }

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

  Graphic3d_SetOfHClipPlane::Iterator anIter (myContext->Clipping().Planes());
  for (GLuint anIndex = 0; anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = anIter.Value();
    if (!myContext->Clipping().IsEnabled (aPlane))
    {
      continue;
    }

    GLint aLocation = theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_0_EQUATION + anIndex);
    if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      const Graphic3d_ClipPlane::Equation& anEquation = aPlane->GetEquation();
      theProgram->SetUniform (myContext, aLocation, OpenGl_Vec4 ((float) anEquation.x(),
                                                                 (float) anEquation.y(),
                                                                 (float) anEquation.z(),
                                                                 (float) anEquation.w()));
    }

    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_0_SPACE + anIndex),
                            myContext->Clipping().GetEquationSpace (aPlane));
    ++anIndex;
  }

  theProgram->UpdateState (OpenGl_CLIP_PLANES_STATE, myClippingState.Index());
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

  for (int anIndex = 0; anIndex < 2; ++anIndex)
  {
    const OPENGL_SURF_PROP& aProperties = (anIndex == 0) ? theAspect->IntFront() : theAspect->IntBack();
    GLint aLocation = theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_AMBIENT + anIndex);
    if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      OpenGl_Vec4 anAmbient (aProperties.ambcol.rgb[0] * aProperties.amb,
                             aProperties.ambcol.rgb[1] * aProperties.amb,
                             aProperties.ambcol.rgb[2] * aProperties.amb,
                             aProperties.ambcol.rgb[3] * aProperties.amb);
      theProgram->SetUniform (theCtx, aLocation, anAmbient);
    }

    aLocation = theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_DIFFUSE + anIndex);
    if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      OpenGl_Vec4 aDiffuse (aProperties.difcol.rgb[0] * aProperties.diff,
                            aProperties.difcol.rgb[1] * aProperties.diff,
                            aProperties.difcol.rgb[2] * aProperties.diff,
                            aProperties.difcol.rgb[3] * aProperties.diff);
      theProgram->SetUniform (theCtx, aLocation, aDiffuse);
    }

    aLocation = theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_SPECULAR + anIndex);
    if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      OpenGl_Vec4 aSpecular (aProperties.speccol.rgb[0] * aProperties.spec,
                             aProperties.speccol.rgb[1] * aProperties.spec,
                             aProperties.speccol.rgb[2] * aProperties.spec,
                             aProperties.speccol.rgb[3] * aProperties.spec);
      theProgram->SetUniform (theCtx, aLocation, aSpecular);
    }

    aLocation = theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_EMISSION + anIndex);
    if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      OpenGl_Vec4 anEmission (aProperties.emscol.rgb[0] * aProperties.emsv,
                              aProperties.emscol.rgb[1] * aProperties.emsv,
                              aProperties.emscol.rgb[2] * aProperties.emsv,
                              aProperties.emscol.rgb[3] * aProperties.emsv);
      theProgram->SetUniform (theCtx, aLocation, anEmission);
    }
    
    theProgram->SetUniform (theCtx,
                            theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_SHININESS + anIndex),
                            aProperties.shine);

    theProgram->SetUniform (theCtx,
                            theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_TRANSPARENCY + anIndex),
                            aProperties.trans);
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
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_AMBIENT),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_DIFFUSE),
                          aDiffuse);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_SPECULAR),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_EMISSION),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_TRANSPARENCY),
                          0.0f);
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

  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_AMBIENT),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_DIFFUSE),
                          aDiffuse);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_SPECULAR),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_EMISSION),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_TRANSPARENCY),
                          0.0f);
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

  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_AMBIENT),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_DIFFUSE),
                          aDiffuse);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_SPECULAR),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_EMISSION),
                          THE_COLOR_BLACK_VEC4);
  theProgram->SetUniform (theCtx,
                          theProgram->GetStateLocation (OpenGl_OCCT_FRONT_MATERIAL_TRANSPARENCY),
                          0.0f);
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