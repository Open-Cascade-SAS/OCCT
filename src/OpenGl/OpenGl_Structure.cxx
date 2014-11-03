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

#include <Graphic3d_SequenceOfHClipPlane_Handle.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_Structure, Graphic3d_CStructure)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Structure, Graphic3d_CStructure)

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
    if ((theWorkspace->NamedStatus & OPENGL_NS_IMMEDIATE) != 0)
    {
      glDepthMask (GL_FALSE);
    }

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
  myTransformation(NULL),
  myTransPers(NULL),
  myAspectLine(NULL),
  myAspectFace(NULL),
  myAspectMarker(NULL),
  myAspectText(NULL),
  myHighlightColor(NULL),
  myNamedStatus(0),
  myZLayer(0),
  myIsRaytracable (Standard_False),
  myModificationState (0),
  myIsCulled (Standard_True),
  myIsMirrored (Standard_False)
{
  UpdateNamedStatus();
}

// =======================================================================
// function : ~OpenGl_Structure
// purpose  :
// =======================================================================
OpenGl_Structure::~OpenGl_Structure()
{
  Release (Handle(OpenGl_Context)());
  delete myTransformation;  myTransformation  = NULL;
  delete myTransPers;       myTransPers       = NULL;
}

// =======================================================================
// function : UpdateAspects
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateAspects()
{
  SetTransformPersistence (TransformPersistence);

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

  if (myIsRaytracable)
  {
    UpdateStateWithAncestorStructures();
  }
}

// =======================================================================
// function : SetTransformPersistence
// purpose  :
// =======================================================================
void OpenGl_Structure::SetTransformPersistence(const CALL_DEF_TRANSFORM_PERSISTENCE &ATransPers)
{
  if (!myTransPers)
    myTransPers = new TEL_TRANSFORM_PERSISTENCE;

  myTransPers->mode = ATransPers.Flag;
  myTransPers->pointX = ATransPers.Point.x;
  myTransPers->pointY = ATransPers.Point.y;
  myTransPers->pointZ = ATransPers.Point.z;
  MarkAsNotCulled();
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

  if (myIsRaytracable)
  {
    UpdateStateWithAncestorStructures();
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
// function : UpdateNamedStatus
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateNamedStatus()
{
  myNamedStatus = 0;
  if (highlight) myNamedStatus |= OPENGL_NS_HIGHLIGHT;
  if (!visible)  myNamedStatus |= OPENGL_NS_HIDE;

  if (myIsRaytracable)
  {
    UpdateStateWithAncestorStructures();
  }
}

// =======================================================================
// function : RegisterAncestorStructure
// purpose  :
// =======================================================================
void OpenGl_Structure::RegisterAncestorStructure (const OpenGl_Structure* theStructure) const
{
  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theStructure)
    {
      return;
    }
  }

  myAncestorStructures.Append (theStructure);
}

// =======================================================================
// function : UnregisterAncestorStructure
// purpose  :
// =======================================================================
void OpenGl_Structure::UnregisterAncestorStructure (const OpenGl_Structure* theStructure) const
{
  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theStructure)
    {
      myAncestorStructures.Remove (anIt);
      return;
    }
  }
}

// =======================================================================
// function : UnregisterFromAncestorStructure
// purpose  :
// =======================================================================
void OpenGl_Structure::UnregisterFromAncestorStructure() const
{
  for (OpenGl_ListOfStructure::Iterator anIta (myAncestorStructures); anIta.More(); anIta.Next())
  {
    OpenGl_Structure* anAncestor = const_cast<OpenGl_Structure*> (anIta.ChangeValue());

    for (OpenGl_ListOfStructure::Iterator anIts (anAncestor->myConnected); anIts.More(); anIts.Next())
    {
      if (anIts.Value() == this)
      {
        anAncestor->myConnected.Remove (anIts);
        return;
      }
    }
  }
}

// =======================================================================
// function : UpdateStateWithAncestorStructures
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateStateWithAncestorStructures() const
{
  myModificationState++;

  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    anIt.Value()->UpdateStateWithAncestorStructures();
  }
}

// =======================================================================
// function : UpdateRaytracableWithAncestorStructures
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateRaytracableWithAncestorStructures() const
{
  myIsRaytracable = OpenGl_Raytrace::IsRaytracedStructure (this);

  if (!myIsRaytracable)
  {
    for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
    {
      anIt.Value()->UpdateRaytracableWithAncestorStructures();
    }
  }
}

// =======================================================================
// function : SetRaytracableWithAncestorStructures
// purpose  :
// =======================================================================
void OpenGl_Structure::SetRaytracableWithAncestorStructures() const
{
  myIsRaytracable = Standard_True;

  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    if (!anIt.Value()->IsRaytracable())
    {
      anIt.Value()->SetRaytracableWithAncestorStructures();
    }
  }
}

// =======================================================================
// function : Connect
// purpose  :
// =======================================================================
void OpenGl_Structure::Connect (Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = (OpenGl_Structure* )&theStructure;
  Disconnect (theStructure);
  myConnected.Append (aStruct);

  if (aStruct->IsRaytracable())
  {
    UpdateStateWithAncestorStructures();
    SetRaytracableWithAncestorStructures();
  }

  aStruct->RegisterAncestorStructure (this);
}

// =======================================================================
// function : Disconnect
// purpose  :
// =======================================================================
void OpenGl_Structure::Disconnect (Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = (OpenGl_Structure* )&theStructure;
  for (OpenGl_ListOfStructure::Iterator anIter (myConnected); anIter.More(); anIter.Next())
  {
    // Check for the given structure
    if (anIter.Value() == aStruct)
    {
      myConnected.Remove (anIter);

      if (aStruct->IsRaytracable())
      {
        UpdateStateWithAncestorStructures();
        UpdateRaytracableWithAncestorStructures();
      }

      aStruct->UnregisterAncestorStructure (this);
      return;
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
      theGroup->Clear (Standard_False);

      if (((OpenGl_Group* )theGroup.operator->())->IsRaytracable())
      {
        UpdateStateWithAncestorStructures();
        UpdateRaytracableWithAncestorStructures();
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
    UpdateStateWithAncestorStructures();
    UpdateRaytracableWithAncestorStructures();
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
  if (myNamedStatus & OPENGL_NS_HIDE)
  {
    return;
  }

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  // Render named status
  const Standard_Integer aNamedStatus = theWorkspace->NamedStatus;
  theWorkspace->NamedStatus |= myNamedStatus;

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

  // Apply transform persistence
  const TEL_TRANSFORM_PERSISTENCE *aTransPersistence = NULL;
  if ( myTransPers && myTransPers->mode != 0 )
  {
    aTransPersistence = theWorkspace->ActiveView()->BeginTransformPersistence (aCtx, myTransPers);
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

  // Render connected structures
  OpenGl_ListOfStructure::Iterator anIter (myConnected);
  while (anIter.More())
  {
    anIter.Value()->RenderGeometry (theWorkspace);
    anIter.Next();
  }

  // Set up plane equations for non-structure transformed global model-view matrix
  // List of planes to be applied to context state
  Handle(Graphic3d_SequenceOfHClipPlane) aUserPlanes;

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

  // Apply local transformation
  if (myTransformation)
  {
    aCtx->ModelWorldState.Pop();
    aCtx->SetGlNormalizeEnabled (anOldGlNormalize);
  }

  // Restore highlight color
  theWorkspace->HighlightColor = aHighlightColor;

  // Restore aspects
  theWorkspace->SetAspectLine (anAspectLine);
  theWorkspace->SetAspectFace (anAspectFace);
  theWorkspace->SetAspectMarker (anAspectMarker);
  theWorkspace->SetAspectText (anAspectText);

  // Restore transform persistence
  if ( myTransPers && myTransPers->mode != 0 )
  {
    theWorkspace->ActiveView()->BeginTransformPersistence (aCtx, aTransPersistence);
  }

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

  // Remove from connected list of ancestor
  UnregisterFromAncestorStructure();
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
//function : SetZLayer
//purpose  :
//=======================================================================
void OpenGl_Structure::SetZLayer (const Standard_Integer theLayerIndex)
{
  myZLayer = theLayerIndex;
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================
Standard_Integer OpenGl_Structure::GetZLayer () const
{
  return myZLayer;
}

//=======================================================================
//function : ShadowLink
//purpose  :
//=======================================================================
Handle(Graphic3d_CStructure) OpenGl_Structure::ShadowLink (const Handle(Graphic3d_StructureManager)& theManager) const
{
  return new OpenGl_StructureShadow (theManager, this);
}
