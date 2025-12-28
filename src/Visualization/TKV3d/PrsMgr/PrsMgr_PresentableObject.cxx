// Created on: 1997-12-16
// Created by: Jean Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <PrsMgr_PresentableObject.hxx>

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsMgr_PresentableObject, Standard_Transient)

//=================================================================================================

const gp_Trsf& PrsMgr_PresentableObject::getIdentityTrsf()
{
  static const gp_Trsf THE_IDENTITY_TRSF;
  return THE_IDENTITY_TRSF;
}

//=================================================================================================

PrsMgr_PresentableObject::PrsMgr_PresentableObject(const PrsMgr_TypeOfPresentation3d theType)
    : myParent(nullptr),
      myViewAffinity(new Graphic3d_ViewAffinity()),
      myDrawer(new Prs3d_Drawer()),
      myTypeOfPresentation3d(theType),
      myDisplayStatus(PrsMgr_DisplayStatus_None),
      //
      myCurrentFacingModel(Aspect_TOFM_BOTH_SIDE),
      myOwnWidth(0.0f),
      hasOwnColor(false),
      hasOwnMaterial(false),
      //
      myInfiniteState(false),
      myIsMutable(false),
      myHasOwnPresentations(true),
      myToPropagateVisualState(true)
{
  myDrawer->SetDisplayMode(-1);
}

//=================================================================================================

PrsMgr_PresentableObject::~PrsMgr_PresentableObject()
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    // should never happen - assertion can be used
    const occ::handle<PrsMgr_Presentation>& aPrs3d = aPrsIter.Value();
    aPrs3d->Erase();
    aPrs3d->myPresentableObject = nullptr;
  }

  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(myChildren);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->SetCombinedParentTransform(occ::handle<TopLoc_Datum3D>());
    anIter.Value()->myParent = nullptr;
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::Fill(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                    const occ::handle<PrsMgr_Presentation>&        thePrs,
                                    const int                                      theMode)
{
  const occ::handle<Prs3d_Presentation>& aStruct3d = thePrs;
  Compute(thePrsMgr, aStruct3d, theMode);
  aStruct3d->SetTransformation(myTransformation);
  aStruct3d->SetClipPlanes(myClipPlanes);
  aStruct3d->SetTransformPersistence(TransformPersistence());
}

//=================================================================================================

void PrsMgr_PresentableObject::computeHLR(const occ::handle<Graphic3d_Camera>&,
                                          const occ::handle<TopLoc_Datum3D>&,
                                          const occ::handle<Prs3d_Presentation>&)
{
  throw Standard_NotImplemented("cannot compute under a specific projector");
}

//=================================================================================================

bool PrsMgr_PresentableObject::ToBeUpdated(bool theToIncludeHidden) const
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aModedPrs = aPrsIter.Value();
    if (aModedPrs->MustBeUpdated())
    {
      if (theToIncludeHidden)
      {
        return true;
      }

      occ::handle<PrsMgr_PresentationManager> aPrsMgr = aModedPrs->PresentationManager();
      if (aPrsMgr->IsDisplayed(this, aModedPrs->Mode())
          || aPrsMgr->IsHighlighted(this, aModedPrs->Mode()))
      {
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool PrsMgr_PresentableObject::UpdatePresentations(bool theToIncludeHidden)
{
  bool hasUpdates = false;
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aModedPrs = aPrsIter.Value();
    if (aModedPrs->MustBeUpdated())
    {
      occ::handle<PrsMgr_PresentationManager> aPrsMgr = aModedPrs->PresentationManager();
      if (theToIncludeHidden || aPrsMgr->IsDisplayed(this, aModedPrs->Mode())
          || aPrsMgr->IsHighlighted(this, aModedPrs->Mode()))
      {
        hasUpdates = true;
        aPrsMgr->Update(this, aModedPrs->Mode());
      }
    }
  }
  return hasUpdates;
}

//=================================================================================================

void PrsMgr_PresentableObject::Update(int theMode, bool theToClearOther)
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();)
  {
    if (aPrsIter.Value()->Mode() == theMode)
    {
      occ::handle<PrsMgr_PresentationManager> aPrsMgr = aPrsIter.Value()->PresentationManager();
      if (aPrsMgr->IsDisplayed(this, theMode) || aPrsMgr->IsHighlighted(this, theMode))
      {
        aPrsMgr->Update(this, theMode);
        aPrsIter.Value()->SetUpdateStatus(false);
      }
      else
      {
        SetToUpdate(aPrsIter.Value()->Mode());
      }
    }
    else if (theToClearOther)
    {
      myPresentations.Remove(aPrsIter);
      continue;
    }
    aPrsIter.Next();
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::SetToUpdate(int theMode)
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    if (theMode == -1 || aPrsIter.Value()->Mode() == theMode)
    {
      aPrsIter.ChangeValue()->SetUpdateStatus(true);
    }
  }
}

//=======================================================================
// function : ToBeUpdated
// purpose  : gets the list of modes to be updated
//=======================================================================
void PrsMgr_PresentableObject::ToBeUpdated(NCollection_List<int>& theOutList) const
{
  theOutList.Clear();
  NCollection_Map<int> MI(myPresentations.Length());
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aModedPrs = aPrsIter.Value();
    if (aModedPrs->MustBeUpdated() && MI.Add(aModedPrs->Mode()))
    {
      theOutList.Append(aModedPrs->Mode());
    }
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::SetTypeOfPresentation(const PrsMgr_TypeOfPresentation3d theType)
{
  myTypeOfPresentation3d = theType;
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPrs = aPrsIter.Value();
    aPrs->SetVisual(myTypeOfPresentation3d == PrsMgr_TOP_ProjectorDependent ? Graphic3d_TOS_COMPUTED
                                                                            : Graphic3d_TOS_ALL);
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::setLocalTransformation(
  const occ::handle<TopLoc_Datum3D>& theTransformation)
{
  myLocalTransformation = theTransformation;
  UpdateTransformation();
}

//=================================================================================================

void PrsMgr_PresentableObject::ResetTransformation()
{
  setLocalTransformation(occ::handle<TopLoc_Datum3D>());
}

//=================================================================================================

void PrsMgr_PresentableObject::SetCombinedParentTransform(
  const occ::handle<TopLoc_Datum3D>& theTrsf)
{
  myCombinedParentTransform = theTrsf;
  UpdateTransformation();
}

//=================================================================================================

void PrsMgr_PresentableObject::UpdateTransformation()
{
  myTransformation.Nullify();
  myInvTransformation = gp_Trsf();
  if (!myCombinedParentTransform.IsNull() && myCombinedParentTransform->Form() != gp_Identity)
  {
    if (!myLocalTransformation.IsNull() && myLocalTransformation->Form() != gp_Identity)
    {
      const gp_Trsf aTrsf = myCombinedParentTransform->Trsf() * myLocalTransformation->Trsf();
      myTransformation    = new TopLoc_Datum3D(aTrsf);
      myInvTransformation = aTrsf.Inverted();
    }
    else
    {
      myTransformation    = myCombinedParentTransform;
      myInvTransformation = myCombinedParentTransform->Trsf().Inverted();
    }
  }
  else if (!myLocalTransformation.IsNull() && myLocalTransformation->Form() != gp_Identity)
  {
    myTransformation    = myLocalTransformation;
    myInvTransformation = myLocalTransformation->Trsf().Inverted();
  }

  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    aPrsIter.ChangeValue()->SetTransformation(myTransformation);
  }

  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator aChildIter(myChildren);
       aChildIter.More();
       aChildIter.Next())
  {
    aChildIter.Value()->SetCombinedParentTransform(myTransformation);
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::recomputeComputed() const
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPrs3d = aPrsIter.Value();
    aPrs3d->ReCompute();
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::SetTransformPersistence(
  const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
{
  myTransformPersistence = theTrsfPers;
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPrs3d = aPrsIter.Value();
    aPrs3d->SetTransformPersistence(myTransformPersistence);
    aPrs3d->ReCompute();
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::AddChild(const occ::handle<PrsMgr_PresentableObject>& theObject)
{
  const occ::handle<PrsMgr_PresentableObject>& aHandleGuard = theObject;
  if (theObject->myParent != nullptr)
  {
    theObject->myParent->RemoveChild(aHandleGuard);
  }

  myChildren.Append(theObject);
  theObject->myParent = this;
  theObject->SetCombinedParentTransform(myTransformation);
}

//=================================================================================================

void PrsMgr_PresentableObject::AddChildWithCurrentTransformation(
  const occ::handle<PrsMgr_PresentableObject>& theObject)
{
  gp_Trsf aTrsf = Transformation().Inverted() * theObject->Transformation();
  theObject->SetLocalTransformation(aTrsf);
  AddChild(theObject);
}

//=================================================================================================

void PrsMgr_PresentableObject::RemoveChild(const occ::handle<PrsMgr_PresentableObject>& theObject)
{
  NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(myChildren);
  for (; anIter.More(); anIter.Next())
  {
    if (anIter.Value() == theObject)
    {
      theObject->myParent = nullptr;
      theObject->SetCombinedParentTransform(occ::handle<TopLoc_Datum3D>());
      myChildren.Remove(anIter);
      break;
    }
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::RemoveChildWithRestoreTransformation(
  const occ::handle<PrsMgr_PresentableObject>& theObject)
{
  gp_Trsf aTrsf = theObject->Transformation();
  RemoveChild(theObject);
  theObject->SetLocalTransformation(aTrsf);
}

//=================================================================================================

void PrsMgr_PresentableObject::SetZLayer(const Graphic3d_ZLayerId theLayerId)
{
  if (myDrawer->ZLayer() == theLayerId)
  {
    return;
  }

  myDrawer->SetZLayer(theLayerId);
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aModedPrs = aPrsIter.Value();
    aModedPrs->SetZLayer(theLayerId);
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::AddClipPlane(const occ::handle<Graphic3d_ClipPlane>& thePlane)
{
  // add to collection and process changes
  if (myClipPlanes.IsNull())
  {
    myClipPlanes = new Graphic3d_SequenceOfHClipPlane();
  }

  myClipPlanes->Append(thePlane);
  UpdateClipping();
}

//=================================================================================================

void PrsMgr_PresentableObject::RemoveClipPlane(const occ::handle<Graphic3d_ClipPlane>& thePlane)
{
  if (myClipPlanes.IsNull())
  {
    return;
  }

  // remove from collection and process changes
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(*myClipPlanes); aPlaneIt.More();
       aPlaneIt.Next())
  {
    const occ::handle<Graphic3d_ClipPlane>& aPlane = aPlaneIt.Value();
    if (aPlane != thePlane)
      continue;

    myClipPlanes->Remove(aPlaneIt);
    UpdateClipping();
    return;
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::SetClipPlanes(
  const occ::handle<Graphic3d_SequenceOfHClipPlane>& thePlanes)
{
  // change collection and process changes
  myClipPlanes = thePlanes;
  UpdateClipping();
}

//=================================================================================================

void PrsMgr_PresentableObject::UpdateClipping()
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aModedPrs = aPrsIter.Value();
    aModedPrs->SetClipPlanes(myClipPlanes);
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::SetInfiniteState(const bool theFlag)
{
  if (myInfiniteState == theFlag)
  {
    return;
  }

  myInfiniteState = theFlag;
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aModedPrs = aPrsIter.Value();
    aModedPrs->SetInfiniteState(theFlag);
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::SetMutable(const bool theIsMutable)
{
  if (myIsMutable == theIsMutable)
  {
    return;
  }

  myIsMutable = theIsMutable;
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aModedPrs = aPrsIter.Value();
    aModedPrs->SetMutable(theIsMutable);
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::UnsetAttributes()
{
  occ::handle<Prs3d_Drawer> aDrawer = new Prs3d_Drawer();
  if (myDrawer->HasLink())
  {
    aDrawer->Link(myDrawer->Link());
  }
  myDrawer = aDrawer;

  hasOwnColor    = false;
  hasOwnMaterial = false;
  myOwnWidth     = 0.0f;
  myDrawer->SetTransparency(0.0f);
}

//=================================================================================================

void PrsMgr_PresentableObject::SetHilightMode(const int theMode)
{
  if (myHilightDrawer.IsNull())
  {
    myHilightDrawer = new Prs3d_Drawer();
    myHilightDrawer->Link(myDrawer);
    myHilightDrawer->SetAutoTriangulation(false);
    myHilightDrawer->SetColor(Quantity_NOC_GRAY80);
    myHilightDrawer->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
  }
  if (myDynHilightDrawer.IsNull())
  {
    myDynHilightDrawer = new Prs3d_Drawer();
    myDynHilightDrawer->Link(myDrawer);
    myDynHilightDrawer->SetColor(Quantity_NOC_CYAN1);
    myDynHilightDrawer->SetAutoTriangulation(false);
    myDynHilightDrawer->SetZLayer(Graphic3d_ZLayerId_Top);
  }
  myHilightDrawer->SetDisplayMode(theMode);
  myDynHilightDrawer->SetDisplayMode(theMode);
}

//=================================================================================================

void PrsMgr_PresentableObject::SynchronizeAspects()
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPrs3d = aPrsIter.ChangeValue();
    for (NCollection_Sequence<occ::handle<Graphic3d_Group>>::Iterator aGroupIter(aPrs3d->Groups());
         aGroupIter.More();
         aGroupIter.Next())
    {
      if (!aGroupIter.Value().IsNull())
      {
        aGroupIter.ChangeValue()->SynchronizeAspects();
      }
    }
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::replaceAspects(
  const NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>>& theMap)
{
  if (theMap.IsEmpty())
  {
    return;
  }

  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPrs3d = aPrsIter.ChangeValue();
    for (NCollection_Sequence<occ::handle<Graphic3d_Group>>::Iterator aGroupIter(aPrs3d->Groups());
         aGroupIter.More();
         aGroupIter.Next())
    {
      if (!aGroupIter.Value().IsNull())
      {
        aGroupIter.ChangeValue()->ReplaceAspects(theMap);
      }
    }
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::BoundingBox(Bnd_Box& theBndBox)
{
  if (myDrawer->DisplayMode() == -1)
  {
    if (!myPresentations.IsEmpty())
    {
      const occ::handle<PrsMgr_Presentation>& aPrs3d  = myPresentations.First();
      const Graphic3d_BndBox3d&               aBndBox = aPrs3d->CStructure()->BoundingBox();
      if (aBndBox.IsValid())
      {
        theBndBox.Update(aBndBox.CornerMin().x(),
                         aBndBox.CornerMin().y(),
                         aBndBox.CornerMin().z(),
                         aBndBox.CornerMax().x(),
                         aBndBox.CornerMax().y(),
                         aBndBox.CornerMax().z());
      }
      else
      {
        theBndBox.SetVoid();
      }
      return;
    }

    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator aPrsIter(myChildren);
         aPrsIter.More();
         aPrsIter.Next())
    {
      if (const occ::handle<PrsMgr_PresentableObject>& aChild = aPrsIter.Value())
      {
        Bnd_Box aBox;
        aChild->BoundingBox(aBox);
        theBndBox.Add(aBox);
      }
    }
    return;
  }

  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPrs3d = aPrsIter.ChangeValue();
    if (aPrs3d->Mode() == myDrawer->DisplayMode())
    {
      const Graphic3d_BndBox3d& aBndBox = aPrs3d->CStructure()->BoundingBox();
      if (aBndBox.IsValid())
      {
        theBndBox.Update(aBndBox.CornerMin().x(),
                         aBndBox.CornerMin().y(),
                         aBndBox.CornerMin().z(),
                         aBndBox.CornerMax().x(),
                         aBndBox.CornerMax().y(),
                         aBndBox.CornerMax().z());
      }
      else
      {
        theBndBox.SetVoid();
      }
      return;
    }
  }
}

//=================================================================================================

Graphic3d_NameOfMaterial PrsMgr_PresentableObject::Material() const
{
  return myDrawer->ShadingAspect()->Material().Name();
}

//=================================================================================================

void PrsMgr_PresentableObject::SetMaterial(const Graphic3d_MaterialAspect& theMaterial)
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->SetMaterial(theMaterial);
  hasOwnMaterial = true;
}

//=================================================================================================

void PrsMgr_PresentableObject::UnsetMaterial()
{
  if (!HasMaterial())
  {
    return;
  }

  if (HasColor() || IsTransparent())
  {
    if (myDrawer->HasLink())
    {
      myDrawer->ShadingAspect()->SetMaterial(
        myDrawer->Link()->ShadingAspect()->Aspect()->BackMaterial());
    }

    if (HasColor())
    {
      SetColor(myDrawer->Color());
    }

    if (IsTransparent())
    {
      SetTransparency(myDrawer->Transparency());
    }
  }
  else
  {
    myDrawer->SetShadingAspect(occ::handle<Prs3d_ShadingAspect>());
  }

  hasOwnMaterial = false;
}

//=================================================================================================

void PrsMgr_PresentableObject::SetTransparency(const double theValue)
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency(float(theValue));
  myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial().SetTransparency(float(theValue));
  myDrawer->SetTransparency(float(theValue));
}

//=================================================================================================

void PrsMgr_PresentableObject::UnsetTransparency()
{
  if (HasColor() || HasMaterial())
  {
    myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency(0.0f);
    myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial().SetTransparency(0.0f);
  }
  else
  {
    myDrawer->SetShadingAspect(occ::handle<Prs3d_ShadingAspect>());
  }
  myDrawer->SetTransparency(0.0f);
}

//=================================================================================================

void PrsMgr_PresentableObject::SetPolygonOffsets(const int   theMode,
                                                 const float theFactor,
                                                 const float theUnits)
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->Aspect()->SetPolygonOffsets(theMode, theFactor, theUnits);
  SynchronizeAspects();
}

//=================================================================================================

bool PrsMgr_PresentableObject::HasPolygonOffsets() const
{
  return !(
    myDrawer->HasOwnShadingAspect()
    || (myDrawer->HasLink() && myDrawer->ShadingAspect() == myDrawer->Link()->ShadingAspect()));
}

//=================================================================================================

void PrsMgr_PresentableObject::PolygonOffsets(int& theMode, float& theFactor, float& theUnits) const
{
  if (HasPolygonOffsets())
  {
    myDrawer->ShadingAspect()->Aspect()->PolygonOffsets(theMode, theFactor, theUnits);
  }
}

//=================================================================================================

void PrsMgr_PresentableObject::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myParent)

  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator anIterator(myPresentations);
       anIterator.More();
       anIterator.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPresentation = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aPresentation.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myClipPlanes.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myDrawer.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myHilightDrawer.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myDynHilightDrawer.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTransformPersistence.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myLocalTransformation.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTransformation.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCombinedParentTransform.get())

  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIterator(myChildren);
       anIterator.More();
       anIterator.Next())
  {
    const occ::handle<PrsMgr_PresentableObject>& aChildObject = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aChildObject.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myInvTransformation)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTypeOfPresentation3d)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurrentFacingModel)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOwnWidth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, hasOwnColor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, hasOwnMaterial)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myInfiniteState)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsMutable)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnPresentations)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToPropagateVisualState)
}
