// Created on: 2011-08-01
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_CappingAlgo.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_StructureShadow.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_SequenceOfHClipPlane.hxx>


//! Auxiliary class for bounding box presentation
class OpenGl_BndBoxPrs : public OpenGl_Element
{

public:

  //! Main constructor
  OpenGl_BndBoxPrs (const Graphic3d_BndBox4f& theBndBox)
  {
    const float Xm = theBndBox.CornerMin().x();
    const float Ym = theBndBox.CornerMin().y();
    const float Zm = theBndBox.CornerMin().z();
    const float XM = theBndBox.CornerMax().x();
    const float YM = theBndBox.CornerMax().y();
    const float ZM = theBndBox.CornerMax().z();

    myVerts[0]  = OpenGl_Vec3 (Xm, Ym, Zm);
    myVerts[1]  = OpenGl_Vec3 (Xm, Ym, ZM);
    myVerts[2]  = OpenGl_Vec3 (Xm, YM, ZM);
    myVerts[3]  = OpenGl_Vec3 (Xm, YM, Zm);
    myVerts[4]  = OpenGl_Vec3 (Xm, Ym, Zm);
    myVerts[5]  = OpenGl_Vec3 (XM, Ym, Zm);
    myVerts[6]  = OpenGl_Vec3 (XM, Ym, ZM);
    myVerts[7]  = OpenGl_Vec3 (XM, YM, ZM);
    myVerts[8]  = OpenGl_Vec3 (XM, YM, Zm);
    myVerts[9]  = OpenGl_Vec3 (XM, Ym, Zm);
    myVerts[10] = OpenGl_Vec3 (XM, YM, Zm);
    myVerts[11] = OpenGl_Vec3 (Xm, YM, Zm);
    myVerts[12] = OpenGl_Vec3 (Xm, YM, ZM);
    myVerts[13] = OpenGl_Vec3 (XM, YM, ZM);
    myVerts[14] = OpenGl_Vec3 (XM, Ym, ZM);
    myVerts[15] = OpenGl_Vec3 (Xm, Ym, ZM);
  }

  //! Render presentation
  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const
  {
  #if !defined(GL_ES_VERSION_2_0)
    // Apply line aspect
    const OpenGl_AspectLine*     anAspectLine = theWorkspace->AspectLine (Standard_True);
    const Handle(OpenGl_Texture) aPrevTexture = theWorkspace->DisableTexture();

    glDisable (GL_LIGHTING);

    // Use highlight colors
    theWorkspace->GetGlContext()->core11->glColor3fv ((theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT) ? theWorkspace->HighlightColor->rgb : anAspectLine->Color().rgb);

    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, (GLfloat* )&myVerts);
    glDrawArrays (GL_LINE_STRIP, 0, 16);
    glDisableClientState (GL_VERTEX_ARRAY);

    // restore aspects
    if (!aPrevTexture.IsNull())
    {
      theWorkspace->EnableTexture (aPrevTexture);
    }
  #endif
  }

  //! Release graphical resources
  virtual void Release (OpenGl_Context*)
  {
    //
  }

protected:

  //! Protected destructor
  virtual ~OpenGl_BndBoxPrs() {}

private:

  OpenGl_Vec3 myVerts[16]; //!< vertices array

public:

  DEFINE_STANDARD_ALLOC

};

/*----------------------------------------------------------------------*/

// =======================================================================
// function : OpenGl_Structure
// purpose  :
// =======================================================================
OpenGl_Structure::OpenGl_Structure (const Handle(Graphic3d_StructureManager)& theManager)
: Graphic3d_CStructure (theManager),
  myTransformation     (NULL),
  myAspectLine         (NULL),
  myAspectFace         (NULL),
  myAspectMarker       (NULL),
  myAspectText         (NULL),
  myHighlightColor     (NULL),
  myInstancedStructure (NULL),
  myIsRaytracable      (Standard_False),
  myModificationState  (0),
  myIsCulled           (Standard_True),
  myIsMirrored         (Standard_False)
{
  //
}

// =======================================================================
// function : ~OpenGl_Structure
// purpose  :
// =======================================================================
OpenGl_Structure::~OpenGl_Structure()
{
  Release (Handle(OpenGl_Context)());
  delete myTransformation;  myTransformation  = NULL;
}

// =======================================================================
// function : UpdateAspects
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateAspects()
{
  if (ContextLine.IsDef)
    SetAspectLine (ContextLine);

  if (ContextFillArea.IsDef)
    SetAspectFace (ContextFillArea);

  if (ContextMarker.IsDef)
    SetAspectMarker (ContextMarker);

  if (ContextText.IsDef)
    SetAspectText (ContextText);
}

// =======================================================================
// function : UpdateTransformation
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateTransformation()
{
  if (myTransformation == NULL)
  {
    myTransformation = new OpenGl_Matrix();
  }

  Standard_ShortReal (*aMat)[4] = Graphic3d_CStructure::Transformation;

  Standard_ShortReal aDet =
    aMat[0][0] * (aMat[1][1] * aMat[2][2] - aMat[2][1] * aMat[1][2]) -
    aMat[0][1] * (aMat[1][0] * aMat[2][2] - aMat[2][0] * aMat[1][2]) +
    aMat[0][2] * (aMat[1][0] * aMat[2][1] - aMat[2][0] * aMat[1][1]);

  // Determinant of transform matrix less then 0 means that mirror transform applied.
  myIsMirrored = aDet < 0.0f;

  matcpy (myTransformation->mat, &Graphic3d_CStructure::Transformation[0][0]);

  if (IsRaytracable())
  {
    ++myModificationState;
  }
}

// =======================================================================
// function : SetAspectLine
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectLine (const CALL_DEF_CONTEXTLINE &theAspect)
{
  if (!myAspectLine)
  {
    myAspectLine = new OpenGl_AspectLine();
  }
  myAspectLine->SetAspect (theAspect);
}

// =======================================================================
// function : SetAspectFace
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectFace (const CALL_DEF_CONTEXTFILLAREA& theAspect)
{
  if (!myAspectFace)
  {
    myAspectFace = new OpenGl_AspectFace();
  }
  myAspectFace->SetAspect (theAspect);

  if (IsRaytracable())
  {
    ++myModificationState;
  }
}

// =======================================================================
// function : SetAspectMarker
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectMarker (const CALL_DEF_CONTEXTMARKER& theAspect)
{
  if (!myAspectMarker)
  {
    myAspectMarker = new OpenGl_AspectMarker();
  }
  myAspectMarker->SetAspect (theAspect);
}

// =======================================================================
// function : SetAspectText
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectText (const CALL_DEF_CONTEXTTEXT &theAspect)
{
  if (!myAspectText)
  {
    myAspectText = new OpenGl_AspectText();
  }
  myAspectText->SetAspect (theAspect);
}

// =======================================================================
// function : clearHighlightBox
// purpose  :
// =======================================================================
void OpenGl_Structure::clearHighlightBox (const Handle(OpenGl_Context)& theGlCtx)
{
  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Release (theGlCtx);
    myHighlightBox.Nullify();
  }
}

// =======================================================================
// function : HighlightWithColor
// purpose  :
// =======================================================================
void OpenGl_Structure::HighlightWithColor (const Graphic3d_Vec3&  theColor,
                                           const Standard_Boolean theToCreate)
{
  const Handle(OpenGl_Context)& aContext = GlDriver()->GetSharedContext();
  if (theToCreate)
    setHighlightColor   (aContext, theColor);
  else
    clearHighlightColor (aContext);
}

// =======================================================================
// function : HighlightWithBndBox
// purpose  :
// =======================================================================
void OpenGl_Structure::HighlightWithBndBox (const Handle(Graphic3d_Structure)& theStruct,
                                            const Standard_Boolean             theToCreate)
{
  const Handle(OpenGl_Context)& aContext = GlDriver()->GetSharedContext();
  if (!theToCreate)
  {
    clearHighlightBox (aContext);
    return;
  }

  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Release (aContext);
  }
  else
  {
    myHighlightBox = new OpenGl_Group (theStruct);
  }

  CALL_DEF_CONTEXTLINE& aContextLine = myHighlightBox->ChangeContextLine();
  aContextLine.IsDef    = 1;
  aContextLine.Color    = HighlightColor;
  aContextLine.LineType = Aspect_TOL_SOLID;
  aContextLine.Width    = 1.0f;
  myHighlightBox->UpdateAspectLine (Standard_True);

  OpenGl_BndBoxPrs* aBndBoxPrs = new OpenGl_BndBoxPrs (myBndBox);
  myHighlightBox->AddElement (aBndBoxPrs);
}

// =======================================================================
// function : setHighlightColor
// purpose  :
// =======================================================================
void OpenGl_Structure::setHighlightColor (const Handle(OpenGl_Context)& theGlCtx,
                                          const Graphic3d_Vec3&         theColor)
{
  clearHighlightBox (theGlCtx);
  if (myHighlightColor == NULL)
  {
    myHighlightColor = new TEL_COLOUR();
  }

  myHighlightColor->rgb[0] = theColor.r();
  myHighlightColor->rgb[1] = theColor.g();
  myHighlightColor->rgb[2] = theColor.b();
  myHighlightColor->rgb[3] = 1.F;
}

// =======================================================================
// function : clearHighlightColor
// purpose  :
// =======================================================================
void OpenGl_Structure::clearHighlightColor (const Handle(OpenGl_Context)& theGlCtx)
{
  clearHighlightBox(theGlCtx);
  delete myHighlightColor;
  myHighlightColor = NULL;
}

// =======================================================================
// function : OnVisibilityChanged
// purpose  :
// =======================================================================
void OpenGl_Structure::OnVisibilityChanged()
{
  if (IsRaytracable())
  {
    ++myModificationState;
  }
}

// =======================================================================
// function : IsRaytracable
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Structure::IsRaytracable() const
{
  if (!myGroups.IsEmpty())
  {
    return myIsRaytracable; // geometry structure
  }
  else if (myInstancedStructure != NULL)
  {
    return myInstancedStructure->IsRaytracable(); // instance structure
  }

  return Standard_False; // has no any groups or structures
}

// =======================================================================
// function : UpdateRaytracableState
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateStateIfRaytracable (const Standard_Boolean toCheck) const
{
  myIsRaytracable = !toCheck || OpenGl_Raytrace::IsRaytracedStructure (this);

  if (IsRaytracable())
  {
    ++myModificationState;
  }
}

// =======================================================================
// function : Connect
// purpose  :
// =======================================================================
void OpenGl_Structure::Connect (Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = static_cast<OpenGl_Structure*> (&theStructure);

  Standard_ASSERT_RAISE (myInstancedStructure == NULL || myInstancedStructure == aStruct,
    "Error! Instanced structure is already defined");

  myInstancedStructure = aStruct;

  if (aStruct->IsRaytracable())
  {
    UpdateStateIfRaytracable (Standard_False);
  }
}

// =======================================================================
// function : Disconnect
// purpose  :
// =======================================================================
void OpenGl_Structure::Disconnect (Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = static_cast<OpenGl_Structure*> (&theStructure);

  if (myInstancedStructure == aStruct)
  {
    myInstancedStructure = NULL;

    if (aStruct->IsRaytracable())
    {
      UpdateStateIfRaytracable();
    }
  }
}

// =======================================================================
// function : NewGroup
// purpose  :
// =======================================================================
Handle(Graphic3d_Group) OpenGl_Structure::NewGroup (const Handle(Graphic3d_Structure)& theStruct)
{
  Handle(OpenGl_Group) aGroup = new OpenGl_Group (theStruct);
  myGroups.Append (aGroup);
  return aGroup;
}

// =======================================================================
// function : RemoveGroup
// purpose  :
// =======================================================================
void OpenGl_Structure::RemoveGroup (const Handle(Graphic3d_Group)& theGroup)
{
  if (theGroup.IsNull())
  {
    return;
  }

  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    // Check for the given group
    if (aGroupIter.Value() == theGroup)
    {
      const Standard_Boolean wasRaytracable =
        static_cast<const OpenGl_Group&> (*theGroup).IsRaytracable();

      theGroup->Clear (Standard_False);

      if (wasRaytracable)
      {
        UpdateStateIfRaytracable();
      }

      myGroups.Remove (aGroupIter);
      return;
    }
  }
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_Structure::Clear()
{
  Clear (GlDriver()->GetSharedContext());
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_Structure::Clear (const Handle(OpenGl_Context)& theGlCtx)
{
  Standard_Boolean aRaytracableGroupDeleted (Standard_False);

  // Release groups
  for (OpenGl_Structure::GroupIterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aRaytracableGroupDeleted |= aGroupIter.Value()->IsRaytracable();

    // Delete objects
    aGroupIter.ChangeValue()->Release (theGlCtx);
  }
  myGroups.Clear();

  if (aRaytracableGroupDeleted)
  {
    myIsRaytracable = Standard_False;
  }

  Is2dText       = Standard_False;
  IsForHighlight = Standard_False;
}

// =======================================================================
// function : RenderGeometry
// purpose  :
// =======================================================================
void OpenGl_Structure::RenderGeometry (const Handle(OpenGl_Workspace) &theWorkspace) const
{
  // Render groups
  const Graphic3d_SequenceOfGroup& aGroups = DrawGroups();
  for (OpenGl_Structure::GroupIterator aGroupIter (aGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.Value()->Render (theWorkspace);
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Structure::Render (const Handle(OpenGl_Workspace) &theWorkspace) const
{
  // Process the structure only if visible
  if (!visible)
  {
    return;
  }

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  // Render named status
  const Standard_Integer aNamedStatus = theWorkspace->NamedStatus;
  if (highlight)
  {
    theWorkspace->NamedStatus |= OPENGL_NS_HIGHLIGHT;
  }

  // Do we need to restore GL_NORMALIZE?
  const Standard_Boolean anOldGlNormalize = aCtx->IsGlNormalizeEnabled();

  // Apply local transformation
  if (myTransformation)
  {
    OpenGl_Matrix aModelWorld;
    OpenGl_Transposemat3 (&aModelWorld, myTransformation);
    aCtx->ModelWorldState.Push();
    aCtx->ModelWorldState.SetCurrent (OpenGl_Mat4::Map ((Standard_ShortReal* )aModelWorld.mat));

    Standard_ShortReal aScaleX = OpenGl_Vec3 (myTransformation->mat[0][0],
                                              myTransformation->mat[0][1],
                                              myTransformation->mat[0][2]).SquareModulus();
    // Scale transform detected.
    if (Abs (aScaleX - 1.f) > Precision::Confusion())
    {
      aCtx->SetGlNormalizeEnabled (Standard_True);
    }
  }
  if (TransformPersistence.Flags)
  {
    OpenGl_Mat4 aProjection = aCtx->ProjectionState.Current();
    OpenGl_Mat4 aWorldView  = aCtx->WorldViewState.Current();
    TransformPersistence.Apply (aProjection, aWorldView, theWorkspace->Width(), theWorkspace->Height());

    aCtx->ProjectionState.Push();
    aCtx->WorldViewState.Push();
    aCtx->ProjectionState.SetCurrent (aProjection);
    aCtx->WorldViewState.SetCurrent (aWorldView);
    aCtx->ApplyProjectionMatrix();
  }

  // Take into account transform persistence
  aCtx->ApplyModelViewMatrix();

  // Apply aspects
  const OpenGl_AspectLine *anAspectLine = theWorkspace->AspectLine (Standard_False);
  const OpenGl_AspectFace *anAspectFace = theWorkspace->AspectFace (Standard_False);
  const OpenGl_AspectMarker *anAspectMarker = theWorkspace->AspectMarker (Standard_False);
  const OpenGl_AspectText *anAspectText = theWorkspace->AspectText (Standard_False);
  if (myAspectLine)
  {
    theWorkspace->SetAspectLine (myAspectLine);
  }
  if (myAspectFace)
  {
    theWorkspace->SetAspectFace (myAspectFace);
  }
  if (myAspectMarker)
  {
    theWorkspace->SetAspectMarker (myAspectMarker);
  }
  if (myAspectText)
  {
    theWorkspace->SetAspectText (myAspectText);
  }

  // Apply correction for mirror transform
  if (myIsMirrored)
  {
    aCtx->core11fwd->glFrontFace (GL_CW);
  }

  // Apply highlight color
  const TEL_COLOUR *aHighlightColor = theWorkspace->HighlightColor;
  if (myHighlightColor)
    theWorkspace->HighlightColor = myHighlightColor;

  // Render instanced structure (if exists)
  if (myInstancedStructure != NULL)
  {
    myInstancedStructure->RenderGeometry (theWorkspace);
  }

  // Set up plane equations for non-structure transformed global model-view matrix
  // List of planes to be applied to context state
  NCollection_Handle<Graphic3d_SequenceOfHClipPlane> aUserPlanes;

  // Collect clipping planes of structure scope
  if (!myClipPlanes.IsEmpty())
  {
    Graphic3d_SequenceOfHClipPlane::Iterator aClippingIter (myClipPlanes);
    for (; aClippingIter.More(); aClippingIter.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aClipPlane = aClippingIter.Value();
      if (!aClipPlane->IsOn())
      {
        continue;
      }

      if (aUserPlanes.IsNull())
      {
        aUserPlanes = new Graphic3d_SequenceOfHClipPlane();
      }

      aUserPlanes->Append (aClipPlane);
    }
  }

  if (!aUserPlanes.IsNull() && !aUserPlanes->IsEmpty())
  {
    // add planes at loaded view matrix state
    aCtx->ChangeClipping().AddWorld (*aUserPlanes, theWorkspace);

    // Set OCCT state uniform variables
    if (!aCtx->ShaderManager()->IsEmpty())
    {
      aCtx->ShaderManager()->UpdateClippingState();
    }
  }

  // Render groups
  const Graphic3d_SequenceOfGroup& aGroups = DrawGroups();
  for (OpenGl_Structure::GroupIterator aGroupIter (aGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.Value()->Render (theWorkspace);
  }

  // Reset correction for mirror transform
  if (myIsMirrored)
  {
    aCtx->core11fwd->glFrontFace (GL_CCW);
  }

  // Render capping for structure groups
  if (!aCtx->Clipping().Planes().IsEmpty())
  {
    OpenGl_CappingAlgo::RenderCapping (theWorkspace, aGroups);
  }

  // Revert structure clippings
  if (!aUserPlanes.IsNull() && !aUserPlanes->IsEmpty())
  {
    aCtx->ChangeClipping().Remove (*aUserPlanes);

    // Set OCCT state uniform variables
    if (!aCtx->ShaderManager()->IsEmpty())
    {
      aCtx->ShaderManager()->RevertClippingState();
    }
  }

  // Restore local transformation
  if (myTransformation)
  {
    aCtx->ModelWorldState.Pop();
    aCtx->SetGlNormalizeEnabled (anOldGlNormalize);
  }
  if (TransformPersistence.Flags)
  {
    aCtx->ProjectionState.Pop();
    aCtx->WorldViewState.Pop();
    aCtx->ApplyProjectionMatrix();
  }

  // Restore highlight color
  theWorkspace->HighlightColor = aHighlightColor;

  // Restore aspects
  theWorkspace->SetAspectLine (anAspectLine);
  theWorkspace->SetAspectFace (anAspectFace);
  theWorkspace->SetAspectMarker (anAspectMarker);
  theWorkspace->SetAspectText (anAspectText);

  // Apply highlight box
  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Render (theWorkspace);
  }

  // Restore named status
  theWorkspace->NamedStatus = aNamedStatus;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Structure::Release (const Handle(OpenGl_Context)& theGlCtx)
{
  // Release groups
  Clear (theGlCtx);
  OpenGl_Element::Destroy (theGlCtx.operator->(), myAspectLine);
  OpenGl_Element::Destroy (theGlCtx.operator->(), myAspectFace);
  OpenGl_Element::Destroy (theGlCtx.operator->(), myAspectMarker);
  OpenGl_Element::Destroy (theGlCtx.operator->(), myAspectText);
  clearHighlightColor (theGlCtx);
}

// =======================================================================
// function : ReleaseGlResources
// purpose  :
// =======================================================================
void OpenGl_Structure::ReleaseGlResources (const Handle(OpenGl_Context)& theGlCtx)
{
  for (OpenGl_Structure::GroupIterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.ChangeValue()->Release (theGlCtx);
  }
  if (myAspectLine != NULL)
  {
    myAspectLine->Release (theGlCtx.operator->());
  }
  if (myAspectFace != NULL)
  {
    myAspectFace->Release (theGlCtx.operator->());
  }
  if (myAspectMarker != NULL)
  {
    myAspectMarker->Release (theGlCtx.operator->());
  }
  if (myAspectText != NULL)
  {
    myAspectText->Release (theGlCtx.operator->());
  }
  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Release (theGlCtx.operator->());
  }
}

//=======================================================================
//function : ShadowLink
//purpose  :
//=======================================================================
Handle(Graphic3d_CStructure) OpenGl_Structure::ShadowLink (const Handle(Graphic3d_StructureManager)& theManager) const
{
  return new OpenGl_StructureShadow (theManager, this);
}
