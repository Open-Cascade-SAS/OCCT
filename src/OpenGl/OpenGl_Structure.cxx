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
#include <OpenGl_Vec.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_SequenceOfHClipPlane.hxx>


IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Structure,Graphic3d_CStructure)

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
    const Handle(OpenGl_Texture) aPrevTexture = theWorkspace->DisableTexture();

    glDisable (GL_LIGHTING);

    // Use highlight colors
    theWorkspace->GetGlContext()->core11->glColor3fv (theWorkspace->LineColor().GetData());

    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, (GLfloat* )&myVerts);
    glDrawArrays (GL_LINE_STRIP, 0, 16);
    glDisableClientState (GL_VERTEX_ARRAY);

    // restore aspects
    if (!aPrevTexture.IsNull())
    {
      theWorkspace->EnableTexture (aPrevTexture);
    }
  #else
    (void )theWorkspace;
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
}

// =======================================================================
// function : UpdateTransformation
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateTransformation()
{
  const OpenGl_Mat4& aMat = Graphic3d_CStructure::Transformation;
  Standard_ShortReal aDet =
    aMat.GetValue(0, 0) * (aMat.GetValue(1, 1) * aMat.GetValue(2, 2) - aMat.GetValue(2, 1) * aMat.GetValue(1, 2)) -
    aMat.GetValue(0, 1) * (aMat.GetValue(1, 0) * aMat.GetValue(2, 2) - aMat.GetValue(2, 0) * aMat.GetValue(1, 2)) +
    aMat.GetValue(0, 2) * (aMat.GetValue(1, 0) * aMat.GetValue(2, 1) - aMat.GetValue(2, 0) * aMat.GetValue(1, 1));

  // Determinant of transform matrix less then 0 means that mirror transform applied.
  myIsMirrored = aDet < 0.0f;

  if (IsRaytracable())
  {
    ++myModificationState;
  }
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

  myHighlightBox->SetGroupPrimitivesAspect (new Graphic3d_AspectLine3d (HighlightColor, Aspect_TOL_SOLID, 1.0));

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
    myHighlightColor = new OpenGl_Vec4 (theColor, 1.0f);
  }
  else
  {
    myHighlightColor->xyz() = theColor;
  }
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
  if (!myGroups.IsEmpty()
    && myIsRaytracable)
  {
    return Standard_True;
  }

  return myInstancedStructure != NULL
     &&  myInstancedStructure->IsRaytracable();
}

// =======================================================================
// function : UpdateRaytracableState
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateStateIfRaytracable (const Standard_Boolean toCheck) const
{
  myIsRaytracable = !toCheck;
  if (!myIsRaytracable)
  {
    for (OpenGl_Structure::GroupIterator anIter (myGroups); anIter.More(); anIter.Next())
    {
      if (anIter.Value()->IsRaytracable())
      {
        myIsRaytracable = Standard_True;
        break;
      }
    }
  }

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
// function : renderGeometry
// purpose  :
// =======================================================================
void OpenGl_Structure::renderGeometry (const Handle(OpenGl_Workspace)& theWorkspace,
                                       bool&                           theHasClosed) const
{
  if (myInstancedStructure != NULL)
  {
    myInstancedStructure->renderGeometry (theWorkspace, theHasClosed);
  }

  for (OpenGl_Structure::GroupIterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    theHasClosed = theHasClosed || aGroupIter.Value()->IsClosed();
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
  if (highlight)
  {
    theWorkspace->SetHighlight (true);
  }

  // Apply local transformation
  aCtx->ModelWorldState.Push();
  aCtx->ModelWorldState.SetCurrent (Transformation);

  const Standard_Boolean anOldGlNormalize = aCtx->IsGlNormalizeEnabled();

#if !defined(GL_ES_VERSION_2_0)
  // detect scale transform
  if (aCtx->core11 != NULL)
  {
    const Standard_ShortReal aScaleX = Transformation.GetRow (0).xyz().SquareModulus();
    if (Abs (aScaleX - 1.f) > Precision::Confusion())
    {
      aCtx->SetGlNormalizeEnabled (Standard_True);
    }
  }
#endif

  if (TransformPersistence.Flags)
  {
    OpenGl_Mat4 aProjection = aCtx->ProjectionState.Current();
    OpenGl_Mat4 aWorldView  = aCtx->WorldViewState.Current();
    TransformPersistence.Apply (theWorkspace->View()->Camera(), aProjection, aWorldView,
                                aCtx->Viewport()[2], aCtx->Viewport()[3]);

    aCtx->ProjectionState.Push();
    aCtx->WorldViewState.Push();
    aCtx->ProjectionState.SetCurrent (aProjection);
    aCtx->WorldViewState.SetCurrent (aWorldView);
    aCtx->ApplyProjectionMatrix();

  #if !defined(GL_ES_VERSION_2_0)
    if (!aCtx->IsGlNormalizeEnabled()
      && aCtx->core11 != NULL)
    {
      const Standard_Real aScale = Graphic3d_TransformUtils::ScaleFactor<Standard_ShortReal> (aWorldView);
      if (Abs (aScale - 1.0f) > Precision::Confusion())
      {
        aCtx->SetGlNormalizeEnabled (Standard_True);
      }
    }
  #endif
  }

  // Take into account transform persistence
  aCtx->ApplyModelViewMatrix();

  // remember aspects
  const OpenGl_AspectLine*   aPrevAspectLine   = theWorkspace->AspectLine();
  const OpenGl_AspectFace*   aPrevAspectFace   = theWorkspace->AspectFace();
  const OpenGl_AspectMarker* aPrevAspectMarker = theWorkspace->AspectMarker();
  const OpenGl_AspectText*   aPrevAspectText   = theWorkspace->AspectText();

  // Apply correction for mirror transform
  if (myIsMirrored)
  {
    aCtx->core11fwd->glFrontFace (GL_CW);
  }

  // Apply highlight color
  const OpenGl_Vec4* aHighlightColor = theWorkspace->HighlightColor;
  if (myHighlightColor)
    theWorkspace->HighlightColor = myHighlightColor;

  // Set up plane equations for non-structure transformed global model-view matrix
  // List of planes to be applied to context state
  Handle(NCollection_Shared<Graphic3d_SequenceOfHClipPlane>) aUserPlanes, aDisabledPlanes;

  // Collect clipping planes of structure scope
  if (!myClipPlanes.IsEmpty())
  {
    for (Graphic3d_SequenceOfHClipPlane::Iterator aClippingIter (myClipPlanes); aClippingIter.More(); aClippingIter.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aClipPlane = aClippingIter.Value();
      if (!aClipPlane->IsOn())
      {
        continue;
      }

      if (aUserPlanes.IsNull())
      {
        aUserPlanes = new NCollection_Shared<Graphic3d_SequenceOfHClipPlane>();
      }
      aUserPlanes->Append (aClipPlane);
    }
  }
  if (!aUserPlanes.IsNull())
  {
    // add planes at loaded view matrix state
    aCtx->ChangeClipping().AddWorld (aCtx, *aUserPlanes);

    // Set OCCT state uniform variables
    aCtx->ShaderManager()->UpdateClippingState();
  }

  // True if structure is fully clipped
  bool isClipped = false;

  // Set of clipping planes that do not intersect the structure,
  // and thus can be disabled to improve rendering performance
  const Graphic3d_BndBox4f& aBBox = BoundingBox();
  if (!aCtx->Clipping().Planes().IsEmpty() && aBBox.IsValid() && TransformPersistence.Flags == Graphic3d_TMF_None)
  {
    for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (aCtx->Clipping().Planes()); aPlaneIt.More(); aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      if (!aPlane->IsOn())
      {
        continue;
      }

      // check for clipping
      const Graphic3d_Vec4d& aPlaneEquation = aPlane->GetEquation();
      const Graphic3d_Vec4d aMaxPnt (aPlaneEquation.x() > 0.0 ? aBBox.CornerMax().x() : aBBox.CornerMin().x(),
                                     aPlaneEquation.y() > 0.0 ? aBBox.CornerMax().y() : aBBox.CornerMin().y(),
                                     aPlaneEquation.z() > 0.0 ? aBBox.CornerMax().z() : aBBox.CornerMin().z(),
                                     1.0);
      if (aPlaneEquation.Dot (aMaxPnt) < 0.0) // max vertex is outside the half-space
      {
        isClipped = true;
        break;
      }

      // check for no intersection (e.g. object is "entirely not clipped")
      const Graphic3d_Vec4d aMinPnt (aPlaneEquation.x() > 0.0 ? aBBox.CornerMin().x() : aBBox.CornerMax().x(),
                                     aPlaneEquation.y() > 0.0 ? aBBox.CornerMin().y() : aBBox.CornerMax().y(),
                                     aPlaneEquation.z() > 0.0 ? aBBox.CornerMin().z() : aBBox.CornerMax().z(),
                                     1.0);
      if (aPlaneEquation.Dot (aMinPnt) > 0.0) // min vertex is inside the half-space
      {
        aCtx->ChangeClipping().SetEnabled (aCtx, aPlane, Standard_False);
        if (aDisabledPlanes.IsNull())
        {
          aDisabledPlanes = new NCollection_Shared<Graphic3d_SequenceOfHClipPlane>();
          if (aUserPlanes.IsNull())
          {
            aCtx->ShaderManager()->UpdateClippingState();
          }
        }
        aDisabledPlanes->Append (aPlane);
      }
    }
  }

  // Render groups
  bool hasClosedPrims = false;
  if (!isClipped)
  {
    renderGeometry (theWorkspace, hasClosedPrims);
  }

  // Reset correction for mirror transform
  if (myIsMirrored)
  {
    aCtx->core11fwd->glFrontFace (GL_CCW);
  }

  // Render capping for structure groups
  if (hasClosedPrims
   && aCtx->Clipping().IsCappingOn())
  {
    OpenGl_CappingAlgo::RenderCapping (theWorkspace, *this);
  }

  // Revert structure clippings
  if (!aDisabledPlanes.IsNull())
  {
    // enable planes that were previously disabled
    for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aDisabledPlanes); aPlaneIt.More(); aPlaneIt.Next())
    {
      aCtx->ChangeClipping().SetEnabled (aCtx, aPlaneIt.Value(), Standard_True);
    }
    if (aUserPlanes.IsNull())
    {
      aCtx->ShaderManager()->RevertClippingState();
    }
  }
  if (!aUserPlanes.IsNull())
  {
    aCtx->ChangeClipping().Remove (aCtx, *aUserPlanes);

    // Set OCCT state uniform variables
    aCtx->ShaderManager()->RevertClippingState();
  }

  // Restore local transformation
  aCtx->ModelWorldState.Pop();
  aCtx->SetGlNormalizeEnabled (anOldGlNormalize);
  if (TransformPersistence.Flags)
  {
    aCtx->ProjectionState.Pop();
    aCtx->WorldViewState.Pop();
    aCtx->ApplyProjectionMatrix();
  }

  // Restore highlight color
  theWorkspace->HighlightColor = aHighlightColor;

  // Restore aspects
  theWorkspace->SetAspectLine   (aPrevAspectLine);
  theWorkspace->SetAspectFace   (aPrevAspectFace);
  theWorkspace->SetAspectMarker (aPrevAspectMarker);
  theWorkspace->SetAspectText   (aPrevAspectText);

  // Apply highlight box
  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Render (theWorkspace);
  }

  // Restore named status
  theWorkspace->SetHighlight (false);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Structure::Release (const Handle(OpenGl_Context)& theGlCtx)
{
  // Release groups
  Clear (theGlCtx);
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
