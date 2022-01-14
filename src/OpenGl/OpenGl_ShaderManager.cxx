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

#include <OpenGl_ShaderManager.hxx>

#include <Graphic3d_CubeMapPacked.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_ClippingIterator.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShadowMap.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_VertexBufferCompat.hxx>
#include <OpenGl_Workspace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShaderManager, Graphic3d_ShaderManager)

namespace
{
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
      theCtx->core11ffp->glMatrixMode (GL_MODELVIEW);
      theCtx->core11ffp->glLoadIdentity();
    }

    // setup light type
    const Graphic3d_Vec4& aLightColor = theLight.PackedColor();
    switch (theLight.Type())
    {
      case Graphic3d_TypeOfLightSource_Ambient:
      {
        break; // handled by separate if-clause at beginning of method
      }
      case Graphic3d_TypeOfLightSource_Directional:
      {
        // if the last parameter of GL_POSITION, is zero, the corresponding light source is a Directional one
        const OpenGl_Vec4 anInfDir = -theLight.PackedDirectionRange();

        // to create a realistic effect,  set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE.
        theCtx->core11ffp->glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
        theCtx->core11ffp->glLightfv (theLightGlId, GL_DIFFUSE,               aLightColor.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_SPECULAR,              aLightColor.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_POSITION,              anInfDir.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
        theCtx->core11ffp->glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
        theCtx->core11ffp->glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
        break;
      }
      case Graphic3d_TypeOfLightSource_Positional:
      {
        // to create a realistic effect, set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE
        const OpenGl_Vec4 aPosition (static_cast<float>(theLight.Position().X()), static_cast<float>(theLight.Position().Y()), static_cast<float>(theLight.Position().Z()), 1.0f);
        theCtx->core11ffp->glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
        theCtx->core11ffp->glLightfv (theLightGlId, GL_DIFFUSE,               aLightColor.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_SPECULAR,              aLightColor.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_POSITION,              aPosition.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
        theCtx->core11ffp->glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
        theCtx->core11ffp->glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
        theCtx->core11ffp->glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
        theCtx->core11ffp->glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
        theCtx->core11ffp->glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0f);
        break;
      }
      case Graphic3d_TypeOfLightSource_Spot:
      {
        const OpenGl_Vec4 aPosition (static_cast<float>(theLight.Position().X()), static_cast<float>(theLight.Position().Y()), static_cast<float>(theLight.Position().Z()), 1.0f);
        theCtx->core11ffp->glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
        theCtx->core11ffp->glLightfv (theLightGlId, GL_DIFFUSE,               aLightColor.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_SPECULAR,              aLightColor.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_POSITION,              aPosition.GetData());
        theCtx->core11ffp->glLightfv (theLightGlId, GL_SPOT_DIRECTION,        theLight.PackedDirectionRange().GetData());
        theCtx->core11ffp->glLightf  (theLightGlId, GL_SPOT_EXPONENT,         theLight.Concentration() * 128.0f);
        theCtx->core11ffp->glLightf  (theLightGlId, GL_SPOT_CUTOFF,          (theLight.Angle() * 180.0f) / GLfloat(M_PI));
        theCtx->core11ffp->glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
        theCtx->core11ffp->glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
        theCtx->core11ffp->glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0f);
        break;
      }
    }

    // restore matrix in case of headlight
    if (theLight.IsHeadlight())
    {
      theCtx->core11ffp->glLoadMatrixf (theModelView.GetData());
    }

    theCtx->core11fwd->glEnable (theLightGlId);
  }
}

// =======================================================================
// function : OpenGl_ShaderManager
// purpose  : Creates new empty shader manager
// =======================================================================
OpenGl_ShaderManager::OpenGl_ShaderManager (OpenGl_Context* theContext)
: Graphic3d_ShaderManager (theContext->GraphicsLibrary()),
  myFfpProgram (new OpenGl_ShaderProgramFFP()),
  myShadingModel (Graphic3d_TypeOfShadingModel_Gouraud),
  myUnlitPrograms (new OpenGl_SetOfPrograms()),
  myContext  (theContext),
  myHasLocalOrigin (Standard_False)
{
  mySRgbState = theContext->ToRenderSRGB();
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

  const TCollection_AsciiString aKey = genLightKey (aLights, myLightSourceState.HasShadowMaps());
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
                                                     Standard_Integer theSpecIBLMapLevels,
                                                     const Handle(OpenGl_ShadowMapArray)& theShadowMaps)
{
  myLightSourceState.Set (theLights);
  myLightSourceState.SetSpecIBLMapLevels (theSpecIBLMapLevels);
  myLightSourceState.SetShadowMaps (theShadowMaps);
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
  if (theModel == Graphic3d_TypeOfShadingModel_DEFAULT)
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
    if (myContext->core11ffp == NULL)
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
    myContext->core11ffp->glLightModelfv (GL_LIGHT_MODEL_AMBIENT, anAmbient.GetData());

    // GL_LIGHTING is managed by drawers to switch between shaded / no lighting output,
    // therefore managing the state here does not have any effect - do it just for consistency.
    if (aLightGlId != GL_LIGHT0)
    {
      myContext->core11fwd->glEnable (GL_LIGHTING);
    }
    else
    {
      myContext->core11fwd->glDisable (GL_LIGHTING);
    }
    // switch off unused lights
    for (; aLightGlId <= GL_LIGHT7; ++aLightGlId)
    {
      myContext->core11fwd->glDisable (aLightGlId);
    }
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
      aLightType = -1; // Graphic3d_TypeOfLightSource_Ambient can be used instead
      aLightParams.Color = OpenGl_Vec4 (0.0f, 0.0f, 0.0f, 0.0f);
      ++aLightsNb;
      continue;
    }

    // ignoring OpenGl_Context::ToRenderSRGB() for light colors,
    // as non-absolute colors for lights are rare and require tuning anyway
    aLightType = aLight.Type();
    aLightParams.Color     = aLight.PackedColor();
    aLightParams.Color.a() = aLight.Intensity(); // used by PBR and ignored by old shading model
    aLightParams.Parameters = aLight.PackedParams();
    switch (aLight.Type())
    {
      case Graphic3d_TypeOfLightSource_Ambient:
      {
        break;
      }
      case Graphic3d_TypeOfLightSource_Directional:
      {
        if (aLight.IsHeadlight())
        {
          const Graphic3d_Mat4& anOrientInv = myWorldViewState.WorldViewMatrixInverse();
          aLightParams.Position = anOrientInv * Graphic3d_Vec4 (-aLight.PackedDirection(), 0.0f);
          aLightParams.Position.SetValues (aLightParams.Position.xyz().Normalized(), 0.0f);
        }
        else
        {
          aLightParams.Position = Graphic3d_Vec4 (-aLight.PackedDirection(), 0.0f);
        }
        break;
      }
      case Graphic3d_TypeOfLightSource_Spot:
      {
        if (aLight.IsHeadlight())
        {
          const Graphic3d_Mat4& anOrientInv = myWorldViewState.WorldViewMatrixInverse();
          aLightParams.Direction = anOrientInv * Graphic3d_Vec4 (aLight.PackedDirection(), 0.0f);
          aLightParams.Direction.SetValues (aLightParams.Direction.xyz().Normalized(), 0.0f);
        }
        else
        {
          aLightParams.Direction = Graphic3d_Vec4 (aLight.PackedDirection(), 0.0f);
        }
      }
      Standard_FALLTHROUGH
      case Graphic3d_TypeOfLightSource_Positional:
      {
        if (aLight.IsHeadlight())
        {
          aLightParams.Position.x() = static_cast<float>(aLight.Position().X());
          aLightParams.Position.y() = static_cast<float>(aLight.Position().Y());
          aLightParams.Position.z() = static_cast<float>(aLight.Position().Z());
          const Graphic3d_Mat4& anOrientInv = myWorldViewState.WorldViewMatrixInverse();
          aLightParams.Position = anOrientInv * Graphic3d_Vec4 (aLightParams.Position.xyz(), 1.0f);
        }
        else
        {
          aLightParams.Position.x() = static_cast<float>(aLight.Position().X() - myLocalOrigin.X());
          aLightParams.Position.y() = static_cast<float>(aLight.Position().Y() - myLocalOrigin.Y());
          aLightParams.Position.z() = static_cast<float>(aLight.Position().Z() - myLocalOrigin.Z());
          aLightParams.Position.w() = 0.0f;
        }
        aLightParams.Direction.w() = aLight.Range();
        break;
      }
    }
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

  // update shadow map variables
  if (const OpenGl_ShaderUniformLocation aShadowMatLoc = theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SHADOWMAP_MATRICES))
  {
    if (myShadowMatArray.Size() < theProgram->NbShadowMaps())
    {
      myShadowMatArray.Resize (0, theProgram->NbShadowMaps() - 1, false);
    }

    Graphic3d_Vec2 aSizeBias;
    if (myLightSourceState.HasShadowMaps())
    {
      aSizeBias.SetValues (1.0f / (float )myLightSourceState.ShadowMaps()->First()->Texture()->SizeX(),
                           myLightSourceState.ShadowMaps()->First()->ShadowMapBias());
      const Standard_Integer aNbShadows = Min (theProgram->NbShadowMaps(), myLightSourceState.ShadowMaps()->Size());
      for (Standard_Integer aShadowIter = 0; aShadowIter < aNbShadows; ++aShadowIter)
      {
        const Handle(OpenGl_ShadowMap)& aShadow = myLightSourceState.ShadowMaps()->Value (aShadowIter);
        myShadowMatArray[aShadowIter] = aShadow->LightSourceMatrix();
      }
    }

    theProgram->SetUniform (myContext, aShadowMatLoc, theProgram->NbShadowMaps(), &myShadowMatArray.First());
    theProgram->SetUniform (myContext, theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SHADOWMAP_SIZE_BIAS), aSizeBias);
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
    if (myContext->core11ffp != NULL)
    {
      myContext->core11ffp->glMatrixMode (GL_PROJECTION);
      myContext->core11ffp->glLoadMatrixf (myProjectionState.ProjectionMatrix().GetData());
    }
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
    if (myContext->core11ffp != NULL)
    {
      const OpenGl_Mat4 aModelView = myWorldViewState.WorldViewMatrix() * myModelWorldState.ModelWorldMatrix();
      myContext->core11ffp->glMatrixMode (GL_MODELVIEW);
      myContext->core11ffp->glLoadMatrixf (aModelView.GetData());
      theProgram->UpdateState (OpenGl_WORLD_VIEW_STATE, myWorldViewState.Index());
    }
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
    if (myContext->core11ffp != NULL)
    {
      const OpenGl_Mat4 aModelView = myWorldViewState.WorldViewMatrix() * myModelWorldState.ModelWorldMatrix();
      myContext->core11ffp->glMatrixMode (GL_MODELVIEW);
      myContext->core11ffp->glLoadMatrixf (aModelView.GetData());
      theProgram->UpdateState (OpenGl_MODEL_WORLD_STATE, myModelWorldState.Index());
    }
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
    if (myContext->core11ffp == NULL)
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
        Message::SendWarning() << "OpenGl_ShaderManager, warning: clipping planes limit (" << aNbMaxPlanes << ") has been exceeded";
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
        myContext->core11ffp->glMatrixMode (GL_MODELVIEW);
        myContext->core11ffp->glLoadMatrixf (myWorldViewState.WorldViewMatrix().GetData());
      }

      myContext->core11fwd->glEnable (anFfpPlaneID);
      myContext->core11ffp->glClipPlane (anFfpPlaneID, aPlaneEq);

      ++aPlaneId;
    }

    // switch off unused lights
    for (; aPlaneId < aNbMaxPlanes; ++aPlaneId)
    {
      myContext->core11fwd->glDisable (GL_CLIP_PLANE0 + aPlaneId);
    }

    // restore combined model-view matrix
    if (toRestoreModelView)
    {
      const OpenGl_Mat4 aModelView = myWorldViewState.WorldViewMatrix() * myModelWorldState.ModelWorldMatrix();
      myContext->core11ffp->glLoadMatrixf (aModelView.GetData());
    }
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
  const OpenGl_Material& aMat = myMaterialState.Material();
  theProgram->UpdateState (OpenGl_MATERIAL_STATE, myMaterialState.Index());
  if (theProgram == myFfpProgram)
  {
    if (myContext->core11ffp == NULL)
    {
      return;
    }

    if (myMaterialState.AlphaCutoff() < ShortRealLast())
    {
      myContext->core11fwd->glAlphaFunc (GL_GEQUAL, myMaterialState.AlphaCutoff());
      myContext->core11fwd->glEnable (GL_ALPHA_TEST);
    }
    else
    {
      myContext->core11fwd->glDisable (GL_ALPHA_TEST);
    }

    const GLenum aFrontFace = myMaterialState.ToDistinguish() ? GL_FRONT : GL_FRONT_AND_BACK;
    const OpenGl_MaterialCommon& aFrontMat = aMat.Common[0];
    const OpenGl_MaterialCommon& aBackMat  = aMat.Common[1];
    const Graphic3d_Vec4 aSpec4 (aFrontMat.SpecularShininess.rgb(), 1.0f);
    myContext->core11ffp->glMaterialfv(aFrontFace, GL_AMBIENT,   aFrontMat.Ambient.GetData());
    myContext->core11ffp->glMaterialfv(aFrontFace, GL_DIFFUSE,   aFrontMat.Diffuse.GetData());
    myContext->core11ffp->glMaterialfv(aFrontFace, GL_SPECULAR,  aSpec4.GetData());
    myContext->core11ffp->glMaterialfv(aFrontFace, GL_EMISSION,  aFrontMat.Emission.GetData());
    myContext->core11ffp->glMaterialf (aFrontFace, GL_SHININESS, aFrontMat.Shine());
    if (myMaterialState.ToDistinguish())
    {
      const Graphic3d_Vec4 aSpec4Back (aBackMat.SpecularShininess.rgb(), 1.0f);
      myContext->core11ffp->glMaterialfv(GL_BACK, GL_AMBIENT,   aBackMat.Ambient.GetData());
      myContext->core11ffp->glMaterialfv(GL_BACK, GL_DIFFUSE,   aBackMat.Diffuse.GetData());
      myContext->core11ffp->glMaterialfv(GL_BACK, GL_SPECULAR,  aSpec4Back.GetData());
      myContext->core11ffp->glMaterialfv(GL_BACK, GL_EMISSION,  aBackMat.Emission.GetData());
      myContext->core11ffp->glMaterialf (GL_BACK, GL_SHININESS, aBackMat.Shine());
    }
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

  if (const OpenGl_ShaderUniformLocation& aLocPbrFront = theProgram->GetStateLocation (OpenGl_OCCT_PBR_MATERIAL))
  {
    theProgram->SetUniform (myContext, aLocPbrFront, OpenGl_Material::NbOfVec4Pbr(), aMat.PackedPbr());
  }
  if (const OpenGl_ShaderUniformLocation& aLocFront = theProgram->GetStateLocation (OpenGl_OCCT_COMMON_MATERIAL))
  {
    theProgram->SetUniform (myContext, aLocFront, OpenGl_Material::NbOfVec4Common(), aMat.PackedCommon());
  }
}

// =======================================================================
// function : pushOitState
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::pushOitState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (const OpenGl_ShaderUniformLocation& aLocOutput = theProgram->GetStateLocation (OpenGl_OCCT_OIT_OUTPUT))
  {
    theProgram->SetUniform (myContext, aLocOutput, (GLint )myOitState.ActiveMode());
  }
  if (const OpenGl_ShaderUniformLocation& aLocDepthFactor = theProgram->GetStateLocation (OpenGl_OCCT_OIT_DEPTH_FACTOR))
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
  PushLightSourceState (aProgram); // should be before PushWorldViewState()
  PushWorldViewState   (aProgram);
  PushModelWorldState  (aProgram);
  PushProjectionState  (aProgram);
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
  else if (myContext->core11ffp != NULL)
  {
    // manage FFP lighting
    myContext->SetShadeModel (theShadingModel);
    if (theShadingModel == Graphic3d_TypeOfShadingModel_Unlit)
    {
      myContext->core11fwd->glDisable (GL_LIGHTING);
    }
    else
    {
      myContext->core11fwd->glEnable (GL_LIGHTING);
    }
  }
}

// =======================================================================
// function : BindFontProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::BindFontProgram (const Handle(OpenGl_ShaderProgram)& theCustomProgram)
{
  if (!theCustomProgram.IsNull()
    || myContext->caps->ffpEnable)
  {
    return bindProgramWithState (theCustomProgram, Graphic3d_TypeOfShadingModel_Unlit);
  }

  if (myFontProgram.IsNull())
  {
    Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramFont();
    TCollection_AsciiString aKey;
    if (!Create (aProgramSrc, aKey, myFontProgram))
    {
      myFontProgram = new OpenGl_ShaderProgram(); // just mark as invalid
      return false;
    }
  }

  return bindProgramWithState (myFontProgram, Graphic3d_TypeOfShadingModel_Unlit);
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
  if (!aProg.IsNull())
  {
    return myContext->BindProgram (aProg);
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramFboBlit (aNbSamples, theIsFallback_sRGB);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, aProg))
  {
    aProg = new OpenGl_ShaderProgram(); // just mark as invalid
    return false;
  }

  myContext->BindProgram (aProg);
  aProg->SetSampler (myContext, "uColorSampler", Graphic3d_TextureUnit_0);
  aProg->SetSampler (myContext, "uDepthSampler", Graphic3d_TextureUnit_1);
  return true;
}

// =======================================================================
// function : BindOitCompositingProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::BindOitCompositingProgram (Standard_Boolean theIsMSAAEnabled)
{
  const Standard_Integer aProgramIdx = theIsMSAAEnabled ? 1 : 0;
  Handle(OpenGl_ShaderProgram)& aProgram = myOitCompositingProgram[aProgramIdx];
  if (!aProgram.IsNull())
  {
    return myContext->BindProgram (aProgram);
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramOitCompositing (theIsMSAAEnabled);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, aProgram))
  {
    aProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return false;
  }

  myContext->BindProgram (aProgram);
  aProgram->SetSampler (myContext, "uAccumTexture",  Graphic3d_TextureUnit_0);
  aProgram->SetSampler (myContext, "uWeightTexture", Graphic3d_TextureUnit_1);
  return true;
}

// =======================================================================
// function : BindOitDepthPeelingBlendProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::BindOitDepthPeelingBlendProgram (bool theIsMSAAEnabled)
{
  const Standard_Integer aProgramIdx = theIsMSAAEnabled ? 1 : 0;
  Handle(OpenGl_ShaderProgram)& aProgram = myOitDepthPeelingBlendProgram [aProgramIdx];
  if (!aProgram.IsNull())
  {
    return myContext->BindProgram (aProgram);
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramOitDepthPeelingBlend (theIsMSAAEnabled);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, aProgram))
  {
    aProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return false;
  }

  myContext->BindProgram (aProgram);
  aProgram->SetSampler (myContext, "uDepthPeelingBackColor", Graphic3d_TextureUnit_0);
  return true;
}

// =======================================================================
// function : BindOitDepthPeelingFlushProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::BindOitDepthPeelingFlushProgram (bool theIsMSAAEnabled)
{
  const Standard_Integer aProgramIdx = theIsMSAAEnabled ? 1 : 0;
  Handle(OpenGl_ShaderProgram)& aProgram = myOitDepthPeelingFlushProgram [aProgramIdx];
  if (!aProgram.IsNull())
  {
    return myContext->BindProgram (aProgram);
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramOitDepthPeelingFlush (theIsMSAAEnabled);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, aProgram))
  {
    aProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return false;
  }

  myContext->BindProgram (aProgram);
  aProgram->SetSampler (myContext, "uDepthPeelingFrontColor", Graphic3d_TextureUnit_0);
  aProgram->SetSampler (myContext, "uDepthPeelingBackColor",  Graphic3d_TextureUnit_1);
  return true;
}

// =======================================================================
// function : prepareStdProgramUnlit
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramUnlit (Handle(OpenGl_ShaderProgram)& theProgram,
                                                               Standard_Integer theBits,
                                                               Standard_Boolean theIsOutline)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramUnlit (theBits, theIsOutline);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramGouraud
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramGouraud (Handle(OpenGl_ShaderProgram)& theProgram,
                                                                 const Standard_Integer        theBits)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramGouraud (myLightSourceState.LightSources(), theBits);
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
  Standard_Integer aNbShadowMaps = myLightSourceState.HasShadowMaps()
                                 ? myLightSourceState.LightSources()->NbCastShadows()
                                 : 0;
  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramPhong (myLightSourceState.LightSources(), theBits, theIsFlatNormal, theIsPBR, aNbShadowMaps);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : BindStereoProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::BindStereoProgram (Graphic3d_StereoMode theStereoMode)
{
  if (theStereoMode < 0 || (int )theStereoMode >= Graphic3d_StereoMode_NB)
  {
    return false;
  }

  Handle(OpenGl_ShaderProgram)& aProgram = myStereoPrograms[theStereoMode];
  if (!aProgram.IsNull())
  {
    return myContext->BindProgram (aProgram);
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = getStdProgramStereo (theStereoMode);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, aProgram))
  {
    aProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return false;
  }

  myContext->BindProgram (aProgram);
  aProgram->SetSampler (myContext, "uLeftSampler",  Graphic3d_TextureUnit_0);
  aProgram->SetSampler (myContext, "uRightSampler", Graphic3d_TextureUnit_1);
  aProgram->SetUniform (myContext, "uTexOffset",    Graphic3d_Vec2(0.0f));
  return true;
}

// =======================================================================
// function : prepareStdProgramBoundBox
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramBoundBox()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = Graphic3d_ShaderManager::getStdProgramBoundBox();
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
Standard_Boolean OpenGl_ShaderManager::preparePBREnvBakingProgram (Standard_Integer theIndex)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = getPBREnvBakingProgram (theIndex);
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myPBREnvBakingProgram[theIndex]))
  {
    myPBREnvBakingProgram[theIndex] = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  if (theIndex == 0
   || theIndex == 2)
  {
    // workaround for old GLSL - load constants as uniform
    myContext->BindProgram (myPBREnvBakingProgram[theIndex]);
    const float aSHBasisFuncCoeffs[9] =
    {
      0.282095f * 0.282095f, 0.488603f * 0.488603f, 0.488603f * 0.488603f, 0.488603f * 0.488603f,
      1.092548f * 1.092548f, 1.092548f * 1.092548f, 1.092548f * 1.092548f, 0.315392f * 0.315392f, 0.546274f * 0.546274f
    };
    const float aSHCosCoeffs[9] = { 3.141593f, 2.094395f, 2.094395f, 2.094395f, 0.785398f, 0.785398f, 0.785398f, 0.785398f, 0.785398f };
    myPBREnvBakingProgram[theIndex]->SetUniform (myContext, myPBREnvBakingProgram[theIndex]->GetUniformLocation (myContext, "aSHBasisFuncCoeffs"), 9, aSHBasisFuncCoeffs);
    myPBREnvBakingProgram[theIndex]->SetUniform (myContext, myPBREnvBakingProgram[theIndex]->GetUniformLocation (myContext, "aSHCosCoeffs"), 9, aSHCosCoeffs);
    myContext->BindProgram (NULL);
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
    myBgCubeMapProgram = getBgCubeMapProgram();
  }
  return myBgCubeMapProgram;
}

// =======================================================================
// function : GetBgSkydomeProgram
// purpose  :
// =======================================================================
const Handle(Graphic3d_ShaderProgram)& OpenGl_ShaderManager::GetBgSkydomeProgram ()
{
  if (myBgSkydomeProgram.IsNull())
  {
    myBgSkydomeProgram = getBgSkydomeProgram();
  }
  return myBgSkydomeProgram;
}

// =======================================================================
// function : GetColoredQuadProgram
// purpose  :
// =======================================================================
const Handle(Graphic3d_ShaderProgram)& OpenGl_ShaderManager::GetColoredQuadProgram ()
{
  if (myColoredQuadProgram.IsNull())
  {
    myColoredQuadProgram = getColoredQuadProgram();
  }
  return myColoredQuadProgram;
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
    aBits |= theTextures->Last()->IsAlpha() ? Graphic3d_ShaderFlags_PointSpriteA : Graphic3d_ShaderFlags_PointSprite;
  }
  else
  {
    aBits |= Graphic3d_ShaderFlags_PointSimple;
  }
  Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theShadingModel, aBits);
  return bindProgramWithState (aProgram, theShadingModel);
}
