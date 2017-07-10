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
  OpenGl_BndBoxPrs (const Graphic3d_BndBox3d& theBndBox)
  {
    const float Xm = (float )theBndBox.CornerMin().x();
    const float Ym = (float)theBndBox.CornerMin().y();
    const float Zm = (float)theBndBox.CornerMin().z();
    const float XM = (float)theBndBox.CornerMax().x();
    const float YM = (float)theBndBox.CornerMax().y();
    const float ZM = (float)theBndBox.CornerMax().z();

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
    const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
    if (aCtx->core11 == NULL)
    {
      return;
    }

    const Handle(OpenGl_TextureSet) aPrevTexture = aCtx->BindTextures (Handle(OpenGl_TextureSet)());
    aCtx->BindProgram (Handle(OpenGl_ShaderProgram)());
    aCtx->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());

    glDisable (GL_LIGHTING);

    // Use highlight colors
    aCtx->core11->glColor3fv (theWorkspace->LineColor().GetData());

    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, (GLfloat* )&myVerts);
    glDrawArrays (GL_LINE_STRIP, 0, 16);
    glDisableClientState (GL_VERTEX_ARRAY);

    // restore aspects
    if (!aPrevTexture.IsNull())
    {
      aCtx->BindTextures (aPrevTexture);
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
  myInstancedStructure (NULL),
  myIsRaytracable      (Standard_False),
  myModificationState  (0),
  myIsCulled           (Standard_True),
  myIsMirrored         (Standard_False)
{
  updateLayerTransformation();
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
// function : SetZLayer
// purpose  :
// =======================================================================
void OpenGl_Structure::SetZLayer (const Graphic3d_ZLayerId theLayerIndex)
{
  Graphic3d_CStructure::SetZLayer (theLayerIndex);
  updateLayerTransformation();
}

// =======================================================================
// function : SetTransformation
// purpose  :
// =======================================================================
void OpenGl_Structure::SetTransformation (const Handle(Geom_Transformation)& theTrsf)
{
  myTrsf = theTrsf;
  myIsMirrored = Standard_False;
  if (!myTrsf.IsNull())
  {
    // Determinant of transform matrix less then 0 means that mirror transform applied.
    const Standard_Real aDet = myTrsf->Value(1, 1) * (myTrsf->Value (2, 2) * myTrsf->Value (3, 3) - myTrsf->Value (3, 2) * myTrsf->Value (2, 3))
                             - myTrsf->Value(1, 2) * (myTrsf->Value (2, 1) * myTrsf->Value (3, 3) - myTrsf->Value (3, 1) * myTrsf->Value (2, 3))
                             + myTrsf->Value(1, 3) * (myTrsf->Value (2, 1) * myTrsf->Value (3, 2) - myTrsf->Value (3, 1) * myTrsf->Value (2, 2));
    myIsMirrored = aDet < 0.0;
  }

  updateLayerTransformation();
  if (IsRaytracable())
  {
    ++myModificationState;
  }
}

// =======================================================================
// function : SetTransformPersistence
// purpose  :
// =======================================================================
void OpenGl_Structure::SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  myTrsfPers = theTrsfPers;
  updateLayerTransformation();
}

// =======================================================================
// function : updateLayerTransformation
// purpose  :
// =======================================================================
void OpenGl_Structure::updateLayerTransformation()
{
  gp_Trsf aRenderTrsf;
  if (!myTrsf.IsNull())
  {
    aRenderTrsf = myTrsf->Trsf();
  }

  const Graphic3d_ZLayerSettings& aLayer = myGraphicDriver->ZLayerSettings (myZLayer);
  if (!aLayer.OriginTransformation().IsNull()
    && myTrsfPers.IsNull())
  {
    aRenderTrsf.SetTranslationPart (aRenderTrsf.TranslationPart() - aLayer.Origin());
  }
  aRenderTrsf.GetMat4 (myRenderTrsf);
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
// function : HighlightWithBndBox
// purpose  :
// =======================================================================
void OpenGl_Structure::highlightWithBndBox (const Handle(Graphic3d_Structure)& theStruct)
{
  const Handle(OpenGl_Context)& aContext = GlDriver()->GetSharedContext();

  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Release (aContext);
  }
  else
  {
    myHighlightBox = new OpenGl_Group (theStruct);
  }

  myHighlightBox->SetGroupPrimitivesAspect (new Graphic3d_AspectLine3d (myHighlightStyle->Color(), Aspect_TOL_SOLID, 1.0));

  OpenGl_BndBoxPrs* aBndBoxPrs = new OpenGl_BndBoxPrs (myBndBox);
  myHighlightBox->AddElement (aBndBoxPrs);
}

// =======================================================================
// function : GraphicHighlight
// purpose  :
// =======================================================================
void OpenGl_Structure::GraphicHighlight (const Handle(Graphic3d_PresentationAttributes)& theStyle,
                                         const Handle(Graphic3d_Structure)& theStruct)
{
  if (!myHighlightStyle.IsNull()
    && myHighlightStyle->Method() == Aspect_TOHM_BOUNDBOX
    && theStyle->Method() != Aspect_TOHM_BOUNDBOX)
  {
    const Handle(OpenGl_Context)& aContext = GlDriver()->GetSharedContext();
    clearHighlightBox (aContext);
  }

  myHighlightStyle = theStyle;

  highlight = 1;
  if (myHighlightStyle->Method() == Aspect_TOHM_BOUNDBOX)
  {
    highlightWithBndBox (theStruct);
  }
}

// =======================================================================
// function : GraphicUnhighlight
// purpose  :
// =======================================================================
void OpenGl_Structure::GraphicUnhighlight()
{
  highlight = 0;

  if (myHighlightStyle->Method() == Aspect_TOHM_BOUNDBOX)
  {
    const Handle(OpenGl_Context)& aContext = GlDriver()->GetSharedContext();
    clearHighlightBox (aContext);
  }

  myHighlightStyle.Nullify();
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
  if (highlight && myHighlightBox.IsNull())
  {
    theWorkspace->SetHighlightStyle (myHighlightStyle);
  }

  // Apply local transformation
  aCtx->ModelWorldState.Push();
  OpenGl_Mat4& aModelWorld = aCtx->ModelWorldState.ChangeCurrent();
  aModelWorld = myRenderTrsf;

  const Standard_Boolean anOldGlNormalize = aCtx->IsGlNormalizeEnabled();
#if !defined(GL_ES_VERSION_2_0)
  // detect scale transform
  if (aCtx->core11 != NULL
  && !myTrsf.IsNull())
  {
    const Standard_Real aScale = myTrsf->ScaleFactor();
    if (Abs (aScale - 1.0) > Precision::Confusion())
    {
      aCtx->SetGlNormalizeEnabled (Standard_True);
    }
  }
#endif

  if (!myTrsfPers.IsNull())
  {
    aCtx->WorldViewState.Push();
    OpenGl_Mat4& aWorldView = aCtx->WorldViewState.ChangeCurrent();
    myTrsfPers->Apply (theWorkspace->View()->Camera(),
                       aCtx->ProjectionState.Current(), aWorldView,
                       aCtx->VirtualViewport()[2], aCtx->VirtualViewport()[3]);

  #if !defined(GL_ES_VERSION_2_0)
    if (!aCtx->IsGlNormalizeEnabled()
      && aCtx->core11 != NULL)
    {
      const Standard_Real aScale = Graphic3d_TransformUtils::ScaleFactor<Standard_ShortReal> (aWorldView);
      if (Abs (aScale - 1.0) > Precision::Confusion())
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

  // Collect clipping planes of structure scope
  aCtx->ChangeClipping().SetLocalPlanes (aCtx, myClipPlanes);

  // True if structure is fully clipped
  bool isClipped = false;
  bool hasDisabled = false;
  if (aCtx->Clipping().IsClippingOrCappingOn())
  {
    const Graphic3d_BndBox3d& aBBox = BoundingBox();
    if (!myClipPlanes.IsNull()
      && myClipPlanes->ToOverrideGlobal())
    {
      aCtx->ChangeClipping().DisableGlobal (aCtx);
      hasDisabled = aCtx->Clipping().HasDisabled();
    }
    else if (!myTrsfPers.IsNull())
    {
      if (myTrsfPers->IsZoomOrRotate())
      {
        // Zoom/rotate persistence object lives in two worlds at the same time.
        // Global clipping planes can not be trivially applied without being converted
        // into local space of transformation persistence object.
        // As more simple alternative - just clip entire object by its anchor point defined in the world space.
        const gp_Pnt anAnchor = myTrsfPers->AnchorPoint();
        for (OpenGl_ClippingIterator aPlaneIt (aCtx->Clipping()); aPlaneIt.More() && aPlaneIt.IsGlobal(); aPlaneIt.Next())
        {
          const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
          if (!aPlane->IsOn())
          {
            continue;
          }

          // check for clipping
          const Graphic3d_Vec4d& aPlaneEquation = aPlane->GetEquation();
          const Graphic3d_Vec4d aCheckPnt (anAnchor.X(), anAnchor.Y(), anAnchor.Z(), 1.0);
          if (aPlaneEquation.Dot (aCheckPnt) < 0.0) // vertex is outside the half-space
          {
            isClipped = true;
            break;
          }
        }
      }

      aCtx->ChangeClipping().DisableGlobal (aCtx);
      hasDisabled = aCtx->Clipping().HasDisabled();
    }

    // Set of clipping planes that do not intersect the structure,
    // and thus can be disabled to improve rendering performance
    if (aBBox.IsValid()
     && myTrsfPers.IsNull())
    {
      for (OpenGl_ClippingIterator aPlaneIt (aCtx->Clipping()); aPlaneIt.More(); aPlaneIt.Next())
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
          aCtx->ChangeClipping().SetEnabled (aCtx, aPlaneIt, Standard_False);
          hasDisabled = true;
        }
      }
    }

    if ((!myClipPlanes.IsNull() && !myClipPlanes->IsEmpty())
     || hasDisabled)
    {
      // Set OCCT state uniform variables
      aCtx->ShaderManager()->UpdateClippingState();
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
  if (hasDisabled)
  {
    // enable planes that were previously disabled
    aCtx->ChangeClipping().RestoreDisabled (aCtx);
  }
  aCtx->ChangeClipping().SetLocalPlanes (aCtx, Handle(Graphic3d_SequenceOfHClipPlane)());
  if ((!myClipPlanes.IsNull() && !myClipPlanes->IsEmpty())
    || hasDisabled)
  {
    // Set OCCT state uniform variables
    aCtx->ShaderManager()->RevertClippingState();
  }

  // Restore local transformation
  aCtx->ModelWorldState.Pop();
  aCtx->SetGlNormalizeEnabled (anOldGlNormalize);
  if (!myTrsfPers.IsNull())
  {
    aCtx->WorldViewState.Pop();
  }

  // Restore aspects
  theWorkspace->SetAspectLine   (aPrevAspectLine);
  theWorkspace->SetAspectFace   (aPrevAspectFace);
  theWorkspace->SetAspectMarker (aPrevAspectMarker);
  theWorkspace->SetAspectText   (aPrevAspectText);

  // Apply highlight box
  if (!myHighlightBox.IsNull())
  {
    theWorkspace->SetHighlightStyle (myHighlightStyle);
    myHighlightBox->Render (theWorkspace);
  }

  // Restore named status
  theWorkspace->SetHighlightStyle (Handle(Graphic3d_PresentationAttributes)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Structure::Release (const Handle(OpenGl_Context)& theGlCtx)
{
  // Release groups
  Clear (theGlCtx);
  clearHighlightBox (theGlCtx);
  myHighlightStyle.Nullify();
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
