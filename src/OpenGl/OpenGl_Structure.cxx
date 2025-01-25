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
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_ClippingIterator.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_StructureShadow.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Structure, Graphic3d_CStructure)

// =======================================================================
// function : renderBoundingBox
// purpose  :
// =======================================================================
void OpenGl_Structure::renderBoundingBox(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  if (!myBndBox.IsValid())
  {
    return;
  }

  const Handle(OpenGl_Context)&   aCtx = theWorkspace->GetGlContext();
  const Handle(OpenGl_TextureSet) aPrevTexture =
    aCtx->BindTextures(Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());
  const Graphic3d_ZLayerSettings& aLayer = myGraphicDriver->ZLayerSettings(myZLayer);
  const Graphic3d_Vec3d           aMoveVec =
    myTrsfPers.IsNull() && !aLayer.OriginTransformation().IsNull()
                ? -Graphic3d_Vec3d(aLayer.Origin().X(), aLayer.Origin().Y(), aLayer.Origin().Z())
                : Graphic3d_Vec3d(0.0, 0.0, 0.0);
  if (aCtx->core20fwd != NULL && aCtx->ShaderManager()->BindBoundBoxProgram())
  {
    const Graphic3d_Vec3d aCenter = myBndBox.Center() + aMoveVec;
    const Graphic3d_Vec3d aSize   = myBndBox.Size();
    aCtx->ActiveProgram()->SetUniform(
      aCtx,
      "occBBoxCenter",
      Graphic3d_Vec3((float)aCenter.x(), (float)aCenter.y(), (float)aCenter.z()));
    aCtx->ActiveProgram()->SetUniform(
      aCtx,
      "occBBoxSize",
      Graphic3d_Vec3((float)aSize.x(), (float)aSize.y(), (float)aSize.z()));
    aCtx->SetColor4fv(theWorkspace->InteriorColor());

    const Handle(OpenGl_VertexBuffer)& aBoundBoxVertBuffer =
      aCtx->ShaderManager()->BoundBoxVertBuffer();
    aBoundBoxVertBuffer->BindAttribute(aCtx, Graphic3d_TOA_POS);
    aCtx->core20fwd->glDrawArrays(GL_LINES, 0, aBoundBoxVertBuffer->GetElemsNb());
    aBoundBoxVertBuffer->UnbindAttribute(aCtx, Graphic3d_TOA_POS);
  }
  else if (aCtx->core11ffp != NULL)
  {
    const Graphic3d_Vec3d aMind = myBndBox.CornerMin() + aMoveVec;
    const Graphic3d_Vec3d aMaxd = myBndBox.CornerMax() + aMoveVec;
    const Graphic3d_Vec3  aMin((float)aMind.x(), (float)aMind.y(), (float)aMind.z());
    const Graphic3d_Vec3  aMax((float)aMaxd.x(), (float)aMaxd.y(), (float)aMaxd.z());
    const OpenGl_Vec3     aVerts[16] = {OpenGl_Vec3(aMin.x(), aMin.y(), aMin.z()),
                                        OpenGl_Vec3(aMin.x(), aMin.y(), aMax.z()),
                                        OpenGl_Vec3(aMin.x(), aMax.y(), aMax.z()),
                                        OpenGl_Vec3(aMin.x(), aMax.y(), aMin.z()),
                                        OpenGl_Vec3(aMin.x(), aMin.y(), aMin.z()),
                                        OpenGl_Vec3(aMax.x(), aMin.y(), aMin.z()),
                                        OpenGl_Vec3(aMax.x(), aMin.y(), aMax.z()),
                                        OpenGl_Vec3(aMax.x(), aMax.y(), aMax.z()),
                                        OpenGl_Vec3(aMax.x(), aMax.y(), aMin.z()),
                                        OpenGl_Vec3(aMax.x(), aMin.y(), aMin.z()),
                                        OpenGl_Vec3(aMax.x(), aMax.y(), aMin.z()),
                                        OpenGl_Vec3(aMin.x(), aMax.y(), aMin.z()),
                                        OpenGl_Vec3(aMin.x(), aMax.y(), aMax.z()),
                                        OpenGl_Vec3(aMax.x(), aMax.y(), aMax.z()),
                                        OpenGl_Vec3(aMax.x(), aMin.y(), aMax.z()),
                                        OpenGl_Vec3(aMin.x(), aMin.y(), aMax.z())};

    aCtx->ShaderManager()->BindLineProgram(Handle(OpenGl_TextureSet)(),
                                           Aspect_TOL_SOLID,
                                           Graphic3d_TypeOfShadingModel_Unlit,
                                           Graphic3d_AlphaMode_Opaque,
                                           false,
                                           Handle(OpenGl_ShaderProgram)());
    aCtx->SetColor4fv(theWorkspace->InteriorColor());
    aCtx->core11fwd->glDisable(GL_LIGHTING);
    aCtx->core11ffp->glEnableClientState(GL_VERTEX_ARRAY);
    aCtx->core11ffp->glVertexPointer(3, GL_FLOAT, 0, aVerts[0].GetData());
    aCtx->core11fwd->glDrawArrays(GL_LINE_STRIP, 0, 16);
    aCtx->core11ffp->glDisableClientState(GL_VERTEX_ARRAY);
  }
  aCtx->BindTextures(aPrevTexture, Handle(OpenGl_ShaderProgram)());
}

// =======================================================================
// function : OpenGl_Structure
// purpose  :
// =======================================================================
OpenGl_Structure::OpenGl_Structure(const Handle(Graphic3d_StructureManager)& theManager)
    : Graphic3d_CStructure(theManager),
      myInstancedStructure(NULL),
      myIsRaytracable(Standard_False),
      myModificationState(0),
      myIsMirrored(Standard_False)
{
  updateLayerTransformation();
}

// =======================================================================
// function : ~OpenGl_Structure
// purpose  :
// =======================================================================
OpenGl_Structure::~OpenGl_Structure()
{
  Release(Handle(OpenGl_Context)());
}

// =======================================================================
// function : SetZLayer
// purpose  :
// =======================================================================
void OpenGl_Structure::SetZLayer(const Graphic3d_ZLayerId theLayerIndex)
{
  Graphic3d_CStructure::SetZLayer(theLayerIndex);
  updateLayerTransformation();
}

// =======================================================================
// function : SetTransformation
// purpose  :
// =======================================================================
void OpenGl_Structure::SetTransformation(const Handle(TopLoc_Datum3D)& theTrsf)
{
  myTrsf       = theTrsf;
  myIsMirrored = Standard_False;
  if (!myTrsf.IsNull())
  {
    // Determinant of transform matrix less then 0 means that mirror transform applied.
    const gp_Trsf&      aTrsf = myTrsf->Transformation();
    const Standard_Real aDet =
      aTrsf.Value(1, 1)
        * (aTrsf.Value(2, 2) * aTrsf.Value(3, 3) - aTrsf.Value(3, 2) * aTrsf.Value(2, 3))
      - aTrsf.Value(1, 2)
          * (aTrsf.Value(2, 1) * aTrsf.Value(3, 3) - aTrsf.Value(3, 1) * aTrsf.Value(2, 3))
      + aTrsf.Value(1, 3)
          * (aTrsf.Value(2, 1) * aTrsf.Value(3, 2) - aTrsf.Value(3, 1) * aTrsf.Value(2, 2));
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
void OpenGl_Structure::SetTransformPersistence(const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  if ((myTrsfPers.IsNull() || theTrsfPers.IsNull()) && myTrsfPers != theTrsfPers)
  {
    ++myModificationState;
  }
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

  const Graphic3d_ZLayerSettings& aLayer = myGraphicDriver->ZLayerSettings(myZLayer);
  if (!aLayer.OriginTransformation().IsNull() && myTrsfPers.IsNull())
  {
    aRenderTrsf.SetTranslationPart(aRenderTrsf.TranslationPart() - aLayer.Origin());
  }
  aRenderTrsf.GetMat4(myRenderTrsf);
}

// =======================================================================
// function : GraphicHighlight
// purpose  :
// =======================================================================
void OpenGl_Structure::GraphicHighlight(const Handle(Graphic3d_PresentationAttributes)& theStyle)
{
  myHighlightStyle = theStyle;
  highlight        = 1;
}

// =======================================================================
// function : GraphicUnhighlight
// purpose  :
// =======================================================================
void OpenGl_Structure::GraphicUnhighlight()
{
  highlight = 0;
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
  if (!myGroups.IsEmpty() && myIsRaytracable && myTrsfPers.IsNull())
  {
    return Standard_True;
  }

  return myInstancedStructure != NULL && myInstancedStructure->IsRaytracable();
}

// =======================================================================
// function : UpdateRaytracableState
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateStateIfRaytracable(const Standard_Boolean toCheck) const
{
  myIsRaytracable = !toCheck;
  if (!myIsRaytracable)
  {
    for (OpenGl_Structure::GroupIterator anIter(myGroups); anIter.More(); anIter.Next())
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
void OpenGl_Structure::Connect(Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = static_cast<OpenGl_Structure*>(&theStructure);

  Standard_ASSERT_RAISE(myInstancedStructure == NULL || myInstancedStructure == aStruct,
                        "Error! Instanced structure is already defined");

  myInstancedStructure = aStruct;

  if (aStruct->IsRaytracable())
  {
    UpdateStateIfRaytracable(Standard_False);
  }
}

// =======================================================================
// function : Disconnect
// purpose  :
// =======================================================================
void OpenGl_Structure::Disconnect(Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = static_cast<OpenGl_Structure*>(&theStructure);

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
Handle(Graphic3d_Group) OpenGl_Structure::NewGroup(const Handle(Graphic3d_Structure)& theStruct)
{
  Handle(OpenGl_Group) aGroup = new OpenGl_Group(theStruct);
  myGroups.Append(aGroup);
  return aGroup;
}

// =======================================================================
// function : RemoveGroup
// purpose  :
// =======================================================================
void OpenGl_Structure::RemoveGroup(const Handle(Graphic3d_Group)& theGroup)
{
  if (theGroup.IsNull())
  {
    return;
  }

  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter(myGroups); aGroupIter.More();
       aGroupIter.Next())
  {
    // Check for the given group
    if (aGroupIter.Value() == theGroup)
    {
      const Standard_Boolean wasRaytracable =
        static_cast<const OpenGl_Group&>(*theGroup).IsRaytracable();

      theGroup->Clear(Standard_False);

      if (wasRaytracable)
      {
        UpdateStateIfRaytracable();
      }

      myGroups.Remove(aGroupIter);
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
  Clear(GlDriver()->GetSharedContext());
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_Structure::Clear(const Handle(OpenGl_Context)& theGlCtx)
{
  Standard_Boolean aRaytracableGroupDeleted(Standard_False);

  // Release groups
  for (OpenGl_Structure::GroupIterator aGroupIter(myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aRaytracableGroupDeleted |= aGroupIter.Value()->IsRaytracable();

    // Delete objects
    aGroupIter.ChangeValue()->Release(theGlCtx);
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
void OpenGl_Structure::renderGeometry(const Handle(OpenGl_Workspace)& theWorkspace,
                                      bool&                           theHasClosed) const
{
  if (myInstancedStructure != NULL)
  {
    myInstancedStructure->renderGeometry(theWorkspace, theHasClosed);
  }

  bool                          anOldCastShadows = false;
  const Handle(OpenGl_Context)& aCtx             = theWorkspace->GetGlContext();
  for (OpenGl_Structure::GroupIterator aGroupIter(myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    const OpenGl_Group* aGroup = aGroupIter.Value();

    const Handle(Graphic3d_TransformPers)& aTrsfPers = aGroup->TransformPersistence();
    if (!aTrsfPers.IsNull())
    {
      applyPersistence(aCtx, aTrsfPers, true, anOldCastShadows);
      aCtx->ApplyModelViewMatrix();
    }

    theHasClosed = theHasClosed || aGroup->IsClosed();
    aGroup->Render(theWorkspace);

    if (!aTrsfPers.IsNull())
    {
      revertPersistence(aCtx, aTrsfPers, true, anOldCastShadows);
      aCtx->ApplyModelViewMatrix();
    }
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Structure::Render(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // Process the structure only if visible
  if (!visible)
  {
    return;
  }

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  // Render named status
  if (highlight && !myHighlightStyle.IsNull() && myHighlightStyle->Method() != Aspect_TOHM_BOUNDBOX)
  {
    theWorkspace->SetHighlightStyle(myHighlightStyle);
  }

  // Apply local transformation
  aCtx->ModelWorldState.Push();
  OpenGl_Mat4& aModelWorld = aCtx->ModelWorldState.ChangeCurrent();
  aModelWorld              = myRenderTrsf;

  const Standard_Boolean anOldGlNormalize = aCtx->IsGlNormalizeEnabled();

  // detect scale transform
  if (aCtx->core11ffp != NULL && !myTrsf.IsNull())
  {
    const Standard_Real aScale = myTrsf->Trsf().ScaleFactor();
    if (Abs(aScale - 1.0) > Precision::Confusion())
    {
      aCtx->SetGlNormalizeEnabled(Standard_True);
    }
  }

  bool anOldCastShadows = false;
#ifdef GL_DEPTH_CLAMP
  bool toRestoreDepthClamp = false;
#endif
  if (!myTrsfPers.IsNull())
  {
    applyPersistence(aCtx, myTrsfPers, false, anOldCastShadows);

#ifdef GL_DEPTH_CLAMP
    if (myTrsfPers->Mode() == Graphic3d_TMF_CameraPers && aCtx->arbDepthClamp)
    {
      toRestoreDepthClamp = true;
      aCtx->core11fwd->glEnable(GL_DEPTH_CLAMP);
    }
#endif
  }

  // Take into account transform persistence
  aCtx->ApplyModelViewMatrix();

  // remember aspects
  const OpenGl_Aspects* aPrevAspectFace = theWorkspace->Aspects();

  // Apply correction for mirror transform
  if (myIsMirrored)
  {
    aCtx->core11fwd->glFrontFace(GL_CW);
  }

  // Collect clipping planes of structure scope
  aCtx->ChangeClipping().SetLocalPlanes(myClipPlanes);

  // True if structure is fully clipped
  bool isClipped   = false;
  bool hasDisabled = false;
  if (aCtx->Clipping().IsClippingOrCappingOn())
  {
    const Graphic3d_BndBox3d& aBBox = BoundingBox();
    if (!myClipPlanes.IsNull() && myClipPlanes->ToOverrideGlobal())
    {
      aCtx->ChangeClipping().DisableGlobal();
      hasDisabled = aCtx->Clipping().HasDisabled();
    }
    else if (!myTrsfPers.IsNull())
    {
      if (myTrsfPers->IsZoomOrRotate())
      {
        // Zoom/rotate persistence object lives in two worlds at the same time.
        // Global clipping planes can not be trivially applied without being converted
        // into local space of transformation persistence object.
        // As more simple alternative - just clip entire object by its anchor point defined in the
        // world space.
        const gp_Pnt anAnchor = myTrsfPers->AnchorPoint();
        for (OpenGl_ClippingIterator aPlaneIt(aCtx->Clipping());
             aPlaneIt.More() && aPlaneIt.IsGlobal();
             aPlaneIt.Next())
        {
          const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
          if (!aPlane->IsOn())
          {
            continue;
          }

          // check for clipping
          const Graphic3d_Vec4d aCheckPnt(anAnchor.X(), anAnchor.Y(), anAnchor.Z(), 1.0);
          if (aPlane->ProbePoint(aCheckPnt) == Graphic3d_ClipState_Out)
          {
            isClipped = true;
            break;
          }
        }
      }

      aCtx->ChangeClipping().DisableGlobal();
      hasDisabled = aCtx->Clipping().HasDisabled();
    }

    // Set of clipping planes that do not intersect the structure,
    // and thus can be disabled to improve rendering performance
    if (aBBox.IsValid() && myTrsfPers.IsNull())
    {
      for (OpenGl_ClippingIterator aPlaneIt(aCtx->Clipping()); aPlaneIt.More(); aPlaneIt.Next())
      {
        const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
        if (aPlaneIt.IsDisabled())
        {
          continue;
        }

        const Graphic3d_ClipState aBoxState = aPlane->ProbeBox(aBBox);
        if (aBoxState == Graphic3d_ClipState_In)
        {
          aCtx->ChangeClipping().SetEnabled(aPlaneIt, false);
          hasDisabled = true;
        }
        else if (aBoxState == Graphic3d_ClipState_Out && myBndBoxClipCheck)
        {
          isClipped = true;
          break;
        }
      }
    }

    if ((!myClipPlanes.IsNull() && !myClipPlanes->IsEmpty()) || hasDisabled)
    {
      // Set OCCT state uniform variables
      aCtx->ShaderManager()->UpdateClippingState();
    }
  }

  // Render groups
  bool hasClosedPrims = false;
  if (!isClipped)
  {
    renderGeometry(theWorkspace, hasClosedPrims);
  }

  // Reset correction for mirror transform
  if (myIsMirrored)
  {
    aCtx->core11fwd->glFrontFace(GL_CCW);
  }

  // Render capping for structure groups
  if (hasClosedPrims && aCtx->Clipping().IsCappingOn())
  {
    OpenGl_CappingAlgo::RenderCapping(theWorkspace, *this);
  }

  // Revert structure clippings
  if (hasDisabled)
  {
    // enable planes that were previously disabled
    aCtx->ChangeClipping().RestoreDisabled();
  }
  aCtx->ChangeClipping().SetLocalPlanes(Handle(Graphic3d_SequenceOfHClipPlane)());
  if ((!myClipPlanes.IsNull() && !myClipPlanes->IsEmpty()) || hasDisabled)
  {
    // Set OCCT state uniform variables
    aCtx->ShaderManager()->RevertClippingState();
  }

  // Restore local transformation
  aCtx->ModelWorldState.Pop();
  aCtx->SetGlNormalizeEnabled(anOldGlNormalize);

  // Restore aspects
  theWorkspace->SetAspects(aPrevAspectFace);

  // Apply highlight box
  if (!isClipped && !myHighlightStyle.IsNull()
      && myHighlightStyle->Method() == Aspect_TOHM_BOUNDBOX)
  {
    aCtx->ApplyModelViewMatrix();
    theWorkspace->SetHighlightStyle(myHighlightStyle);
    renderBoundingBox(theWorkspace);
  }

  if (!myTrsfPers.IsNull())
  {
    revertPersistence(aCtx, myTrsfPers, false, anOldCastShadows);
#ifdef GL_DEPTH_CLAMP
    if (toRestoreDepthClamp)
    {
      aCtx->core11fwd->glDisable(GL_DEPTH_CLAMP);
    }
#endif
  }

  // Restore named status
  theWorkspace->SetHighlightStyle(Handle(Graphic3d_PresentationAttributes)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Structure::Release(const Handle(OpenGl_Context)& theGlCtx)
{
  // Release groups
  Clear(theGlCtx);
  myHighlightStyle.Nullify();
}

// =======================================================================
// function : ReleaseGlResources
// purpose  :
// =======================================================================
void OpenGl_Structure::ReleaseGlResources(const Handle(OpenGl_Context)& theGlCtx)
{
  for (OpenGl_Structure::GroupIterator aGroupIter(myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.ChangeValue()->Release(theGlCtx);
  }
}

//=================================================================================================

Handle(Graphic3d_CStructure) OpenGl_Structure::ShadowLink(
  const Handle(Graphic3d_StructureManager)& theManager) const
{
  return new OpenGl_StructureShadow(theManager, this);
}

// =======================================================================
// function : applyPersistence
// purpose  :
// =======================================================================
void OpenGl_Structure::applyPersistence(const Handle(OpenGl_Context)&          theCtx,
                                        const Handle(Graphic3d_TransformPers)& theTrsfPers,
                                        const Standard_Boolean                 theIsLocal,
                                        Standard_Boolean& theOldCastShadows) const
{
  // temporarily disable shadows on non-3d objects
  theOldCastShadows = theCtx->ShaderManager()->SetCastShadows(false);

  theCtx->WorldViewState.Push();
  OpenGl_Mat4& aWorldView = theCtx->WorldViewState.ChangeCurrent();

  if (theIsLocal && theTrsfPers->IsZoomOrRotate())
  {
    // move anchor point to presentation location
    theCtx->ModelWorldState.Push();
    OpenGl_Mat4&   aModelWorld   = theCtx->ModelWorldState.ChangeCurrent();
    gp_Pnt         aStartPnt     = theTrsfPers->AnchorPoint();
    Graphic3d_Vec4 anAnchorPoint = aModelWorld
                                   * Graphic3d_Vec4((Standard_ShortReal)aStartPnt.X(),
                                                    (Standard_ShortReal)aStartPnt.Y(),
                                                    (Standard_ShortReal)aStartPnt.Z(),
                                                    1.0f);
    // clang-format off
    aModelWorld.SetColumn (3, Graphic3d_Vec4 (Graphic3d_Vec3 (0.0), 1.0)); // reset translation part
    // clang-format on
    aStartPnt.SetCoord(anAnchorPoint.x(), anAnchorPoint.y(), anAnchorPoint.z());

    theTrsfPers->Apply(theCtx->Camera(),
                       theCtx->ProjectionState.Current(),
                       aWorldView,
                       theCtx->VirtualViewport()[2],
                       theCtx->VirtualViewport()[3],
                       &aStartPnt);
  }
  else
  {
    theTrsfPers->Apply(theCtx->Camera(),
                       theCtx->ProjectionState.Current(),
                       aWorldView,
                       theCtx->VirtualViewport()[2],
                       theCtx->VirtualViewport()[3]);
  }

  if (!theCtx->IsGlNormalizeEnabled() && theCtx->core11ffp != NULL)
  {
    const Standard_Real aScale = Graphic3d_TransformUtils::ScaleFactor(aWorldView);
    if (Abs(aScale - 1.0) > Precision::Confusion())
    {
      theCtx->SetGlNormalizeEnabled(true);
    }
  }
}

// =======================================================================
// function : revertPersistence
// purpose  :
// =======================================================================
void OpenGl_Structure::revertPersistence(const Handle(OpenGl_Context)&          theCtx,
                                         const Handle(Graphic3d_TransformPers)& theTrsfPers,
                                         const Standard_Boolean                 theIsLocal,
                                         const Standard_Boolean theOldCastShadows) const
{
  if (theIsLocal && theTrsfPers->IsZoomOrRotate())
  {
    theCtx->ModelWorldState.Pop();
  }
  theCtx->WorldViewState.Pop();
  theCtx->ShaderManager()->SetCastShadows(theOldCastShadows);
}

//=================================================================================================

void OpenGl_Structure::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Graphic3d_CStructure)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myInstancedStructure)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsRaytracable)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myModificationState)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsMirrored)
}
