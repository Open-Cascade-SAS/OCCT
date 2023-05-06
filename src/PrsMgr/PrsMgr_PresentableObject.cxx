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
#include <TColStd_MapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsMgr_PresentableObject, Standard_Transient)

//=======================================================================
//function : getIdentityTrsf
//purpose  :
//=======================================================================
const gp_Trsf& PrsMgr_PresentableObject::getIdentityTrsf()
{
  static const gp_Trsf THE_IDENTITY_TRSF;
  return THE_IDENTITY_TRSF;
}

//=======================================================================
//function : PrsMgr_PresentableObject
//purpose  :
//=======================================================================
PrsMgr_PresentableObject::PrsMgr_PresentableObject (const PrsMgr_TypeOfPresentation3d theType)
: myParent (NULL),
  myViewAffinity (new Graphic3d_ViewAffinity()),
  myDrawer (new Prs3d_Drawer()),
  myTypeOfPresentation3d (theType),
  myDisplayStatus (PrsMgr_DisplayStatus_None),
  //
  myCurrentFacingModel (Aspect_TOFM_BOTH_SIDE),
  myOwnWidth (0.0f),
  hasOwnColor (Standard_False),
  hasOwnMaterial (Standard_False),
  //
  myInfiniteState (Standard_False),
  myIsMutable (Standard_False),
  myHasOwnPresentations (Standard_True),
  myToPropagateVisualState (Standard_True)
{
  myDrawer->SetDisplayMode (-1);
}

//=======================================================================
//function : ~PrsMgr_PresentableObject
//purpose  : destructor
//=======================================================================
PrsMgr_PresentableObject::~PrsMgr_PresentableObject()
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    // should never happen - assertion can be used
    const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.Value();
    aPrs3d->Erase();
    aPrs3d->myPresentableObject = NULL;
  }

  for (PrsMgr_ListOfPresentableObjectsIter anIter (myChildren); anIter.More(); anIter.Next())
  {
    anIter.Value()->SetCombinedParentTransform (Handle(TopLoc_Datum3D)());
    anIter.Value()->myParent = NULL;
  }
}

//=======================================================================
//function : Fill
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::Fill (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                     const Handle(PrsMgr_Presentation)&        thePrs,
                                     const Standard_Integer                    theMode)
{
  const Handle(Prs3d_Presentation)& aStruct3d = thePrs;
  Compute (thePrsMgr, aStruct3d, theMode);
  aStruct3d->SetTransformation (myTransformation);
  aStruct3d->SetClipPlanes (myClipPlanes);
  aStruct3d->SetTransformPersistence (TransformPersistence());
}

//=======================================================================
//function : computeHLR
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::computeHLR (const Handle(Graphic3d_Camera)& ,
                                           const Handle(TopLoc_Datum3D)& ,
                                           const Handle(Prs3d_Presentation)& )
{
  throw Standard_NotImplemented("cannot compute under a specific projector");
}

//=======================================================================
//function : ToBeUpdated
//purpose  :
//=======================================================================
Standard_Boolean PrsMgr_PresentableObject::ToBeUpdated (Standard_Boolean theToIncludeHidden) const
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPrs = aPrsIter.Value();
    if (aModedPrs->MustBeUpdated())
    {
      if (theToIncludeHidden)
      {
        return Standard_True;
      }

      Handle(PrsMgr_PresentationManager) aPrsMgr = aModedPrs->PresentationManager();
      if (aPrsMgr->IsDisplayed  (this, aModedPrs->Mode())
       || aPrsMgr->IsHighlighted(this, aModedPrs->Mode()))
      {
        return Standard_True;
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : UpdatePresentations
//purpose  :
//=======================================================================
Standard_Boolean PrsMgr_PresentableObject::UpdatePresentations (Standard_Boolean theToIncludeHidden)
{
  Standard_Boolean hasUpdates = Standard_False;
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPrs = aPrsIter.Value();
    if (aModedPrs->MustBeUpdated())
    {
      Handle(PrsMgr_PresentationManager) aPrsMgr = aModedPrs->PresentationManager();
      if (theToIncludeHidden
       || aPrsMgr->IsDisplayed  (this, aModedPrs->Mode())
       || aPrsMgr->IsHighlighted(this, aModedPrs->Mode()))
      {
        hasUpdates = Standard_True;
        aPrsMgr->Update (this, aModedPrs->Mode());
      }
    }
  }
  return hasUpdates;
}

//=======================================================================
//function : Update
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::Update (Standard_Integer theMode, Standard_Boolean theToClearOther)
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More();)
  {
    if (aPrsIter.Value()->Mode() == theMode)
    {
      Handle(PrsMgr_PresentationManager) aPrsMgr = aPrsIter.Value()->PresentationManager();
      if (aPrsMgr->IsDisplayed  (this, theMode)
       || aPrsMgr->IsHighlighted(this, theMode))
      {
        aPrsMgr->Update (this, theMode);
        aPrsIter.Value()->SetUpdateStatus (Standard_False);
      }
      else
      {
        SetToUpdate (aPrsIter.Value()->Mode());
      }
    }
    else if (theToClearOther)
    {
      myPresentations.Remove (aPrsIter);
      continue;
    }
    aPrsIter.Next();
  }
}

//=======================================================================
//function : SetToUpdate
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::SetToUpdate (Standard_Integer theMode)
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    if (theMode == -1
     || aPrsIter.Value()->Mode() == theMode)
    {
      aPrsIter.ChangeValue()->SetUpdateStatus (Standard_True);
    }
  }
}

//=======================================================================
//function : ToBeUpdated
//purpose  : gets the list of modes to be updated
//=======================================================================
void PrsMgr_PresentableObject::ToBeUpdated (TColStd_ListOfInteger& theOutList) const
{
  theOutList.Clear();
  TColStd_MapOfInteger MI(myPresentations.Length()); 
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPrs = aPrsIter.Value();
    if (aModedPrs->MustBeUpdated()
     && MI.Add (aModedPrs->Mode()))
    {
      theOutList.Append (aModedPrs->Mode());
    }
  }
}

//=======================================================================
//function : SetTypeOfPresentation
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetTypeOfPresentation (const PrsMgr_TypeOfPresentation3d theType)
{
  myTypeOfPresentation3d = theType;
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs  = aPrsIter.Value();
    aPrs->SetVisual (myTypeOfPresentation3d == PrsMgr_TOP_ProjectorDependent
                   ? Graphic3d_TOS_COMPUTED
                   : Graphic3d_TOS_ALL);
  }
}

//=======================================================================
//function : setLocalTransformation
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::setLocalTransformation (const Handle(TopLoc_Datum3D)& theTransformation)
{
  myLocalTransformation = theTransformation;
  UpdateTransformation();
}

//=======================================================================
//function : ResetTransformation
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::ResetTransformation() 
{
  setLocalTransformation (Handle(TopLoc_Datum3D)());
}

//=======================================================================
//function : SetCombinedParentTransform
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::SetCombinedParentTransform (const Handle(TopLoc_Datum3D)& theTrsf)
{
  myCombinedParentTransform = theTrsf;
  UpdateTransformation();
}

//=======================================================================
//function : UpdateTransformation
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::UpdateTransformation()
{
  myTransformation.Nullify();
  myInvTransformation = gp_Trsf();
  if (!myCombinedParentTransform.IsNull() && myCombinedParentTransform->Form() != gp_Identity)
  {
    if (!myLocalTransformation.IsNull() && myLocalTransformation->Form() != gp_Identity)
    {
      const gp_Trsf aTrsf = myCombinedParentTransform->Trsf() * myLocalTransformation->Trsf();
      myTransformation    = new TopLoc_Datum3D (aTrsf);
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

  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    aPrsIter.ChangeValue()->SetTransformation (myTransformation);
  }

  for (PrsMgr_ListOfPresentableObjectsIter aChildIter (myChildren); aChildIter.More(); aChildIter.Next())
  {
    aChildIter.Value()->SetCombinedParentTransform (myTransformation);
  }
}

//=======================================================================
//function : recomputeComputed
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::recomputeComputed() const
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.Value();
    aPrs3d->ReCompute();
  }
}

//=======================================================================
//function : SetTransformPersistence
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  myTransformPersistence = theTrsfPers;
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.Value();
    aPrs3d->SetTransformPersistence (myTransformPersistence);
    aPrs3d->ReCompute();
  }
}

//=======================================================================
//function : AddChild
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::AddChild (const Handle(PrsMgr_PresentableObject)& theObject)
{
  const Handle(PrsMgr_PresentableObject)& aHandleGuard = theObject;
  if (theObject->myParent != NULL)
  {
    theObject->myParent->RemoveChild (aHandleGuard);
  }

  myChildren.Append (theObject);  
  theObject->myParent = this;
  theObject->SetCombinedParentTransform (myTransformation);
}

//=======================================================================
//function : AddChildWithCurrentTransformation
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::AddChildWithCurrentTransformation(const Handle(PrsMgr_PresentableObject)& theObject)
{
  gp_Trsf aTrsf = Transformation().Inverted() * theObject->Transformation();
  theObject->SetLocalTransformation(aTrsf);
  AddChild(theObject);
}

//=======================================================================
//function : RemoveChild
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::RemoveChild (const Handle(PrsMgr_PresentableObject)& theObject)
{
  PrsMgr_ListOfPresentableObjectsIter anIter (myChildren);
  for (; anIter.More(); anIter.Next())
  {
    if (anIter.Value() == theObject)
    {
      theObject->myParent = NULL;
      theObject->SetCombinedParentTransform (Handle(TopLoc_Datum3D)());
      myChildren.Remove (anIter);
      break;
    }
  }
}

//=======================================================================
//function : RemoveChildWithRestoreTransformation
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::RemoveChildWithRestoreTransformation(const Handle(PrsMgr_PresentableObject)& theObject)
{
  gp_Trsf aTrsf = theObject->Transformation();
  RemoveChild(theObject);
  theObject->SetLocalTransformation(aTrsf);
}

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetZLayer (const Graphic3d_ZLayerId theLayerId)
{
  if (myDrawer->ZLayer() == theLayerId)
  {
    return;
  }

  myDrawer->SetZLayer (theLayerId);
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPrs = aPrsIter.Value();
    aModedPrs->SetZLayer (theLayerId);
  }
}

// =======================================================================
// function : AddClipPlane
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::AddClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  // add to collection and process changes
  if (myClipPlanes.IsNull())
  {
    myClipPlanes = new Graphic3d_SequenceOfHClipPlane();
  }

  myClipPlanes->Append (thePlane);
  UpdateClipping();
}

// =======================================================================
// function : RemoveClipPlane
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::RemoveClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  if (myClipPlanes.IsNull())
  {
    return;
  }

  // remove from collection and process changes
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*myClipPlanes); aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (aPlane != thePlane)
      continue;

    myClipPlanes->Remove (aPlaneIt);
    UpdateClipping();
    return;
  }
}

// =======================================================================
// function : SetClipPlanes
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  // change collection and process changes
  myClipPlanes = thePlanes;
  UpdateClipping();
}

// =======================================================================
// function : UpdateClipping
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::UpdateClipping()
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPrs = aPrsIter.Value();
    aModedPrs->SetClipPlanes (myClipPlanes);
  }
}

//=======================================================================
//function : SetInfiniteState
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetInfiniteState (const Standard_Boolean theFlag)
{
  if (myInfiniteState == theFlag)
  {
    return;
  }

  myInfiniteState = theFlag;
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPrs = aPrsIter.Value();
    aModedPrs->SetInfiniteState (theFlag);
  }
}

// =======================================================================
// function : SetMutable
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::SetMutable (const Standard_Boolean theIsMutable)
{
  if (myIsMutable == theIsMutable)
  {
    return;
  }

  myIsMutable = theIsMutable;
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPrs = aPrsIter.Value();
    aModedPrs->SetMutable (theIsMutable);
  }
}

// =======================================================================
// function : UnsetAttributes
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::UnsetAttributes()
{
  Handle(Prs3d_Drawer) aDrawer = new Prs3d_Drawer();
  if (myDrawer->HasLink())
  {
    aDrawer->Link(myDrawer->Link());
  }
  myDrawer = aDrawer;

  hasOwnColor    = Standard_False;
  hasOwnMaterial = Standard_False;
  myOwnWidth     = 0.0f;
  myDrawer->SetTransparency (0.0f);
}

//=======================================================================
//function : SetHilightMode
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetHilightMode (const Standard_Integer theMode)
{
  if (myHilightDrawer.IsNull())
  {
    myHilightDrawer = new Prs3d_Drawer();
    myHilightDrawer->Link (myDrawer);
    myHilightDrawer->SetAutoTriangulation (Standard_False);
    myHilightDrawer->SetColor (Quantity_NOC_GRAY80);
    myHilightDrawer->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
  }
  if (myDynHilightDrawer.IsNull())
  {
    myDynHilightDrawer = new Prs3d_Drawer();
    myDynHilightDrawer->Link (myDrawer);
    myDynHilightDrawer->SetColor (Quantity_NOC_CYAN1);
    myDynHilightDrawer->SetAutoTriangulation (Standard_False);
    myDynHilightDrawer->SetZLayer(Graphic3d_ZLayerId_Top);
  }
  myHilightDrawer   ->SetDisplayMode (theMode);
  myDynHilightDrawer->SetDisplayMode (theMode);
}

//=======================================================================
//function : SynchronizeAspects
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SynchronizeAspects()
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.ChangeValue();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (aPrs3d->Groups()); aGroupIter.More(); aGroupIter.Next())
    {
      if (!aGroupIter.Value().IsNull())
      {
        aGroupIter.ChangeValue()->SynchronizeAspects();
      }
    }
  }
}

//=======================================================================
//function : replaceAspects
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::replaceAspects (const Graphic3d_MapOfAspectsToAspects& theMap)
{
  if (theMap.IsEmpty())
  {
    return;
  }

  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.ChangeValue();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (aPrs3d->Groups()); aGroupIter.More(); aGroupIter.Next())
    {
      if (!aGroupIter.Value().IsNull())
      {
        aGroupIter.ChangeValue()->ReplaceAspects (theMap);
      }
    }
  }
}

//=======================================================================
//function : BoundingBox
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::BoundingBox (Bnd_Box& theBndBox)
{
  if (myDrawer->DisplayMode() == -1)
  {
    if (!myPresentations.IsEmpty())
    {
      const Handle(PrsMgr_Presentation)& aPrs3d = myPresentations.First();
      const Graphic3d_BndBox3d& aBndBox = aPrs3d->CStructure()->BoundingBox();
      if (aBndBox.IsValid())
      {
        theBndBox.Update (aBndBox.CornerMin().x(), aBndBox.CornerMin().y(), aBndBox.CornerMin().z(),
                          aBndBox.CornerMax().x(), aBndBox.CornerMax().y(), aBndBox.CornerMax().z());
      }
      else
      {
        theBndBox.SetVoid();
      }
      return;
    }

    for (PrsMgr_ListOfPresentableObjectsIter aPrsIter (myChildren); aPrsIter.More(); aPrsIter.Next())
    {
      if (const Handle(PrsMgr_PresentableObject)& aChild = aPrsIter.Value())
      {
        Bnd_Box aBox;
        aChild->BoundingBox (aBox);
        theBndBox.Add (aBox);
      }
    }
    return;
  }

  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.ChangeValue();
    if (aPrs3d->Mode() == myDrawer->DisplayMode())
    {
      const Graphic3d_BndBox3d& aBndBox = aPrs3d->CStructure()->BoundingBox();
      if (aBndBox.IsValid())
      {
        theBndBox.Update (aBndBox.CornerMin().x(), aBndBox.CornerMin().y(), aBndBox.CornerMin().z(),
                          aBndBox.CornerMax().x(), aBndBox.CornerMax().y(), aBndBox.CornerMax().z());
      }
      else
      {
        theBndBox.SetVoid();
      }
      return;
    }
  }
}

//=======================================================================
//function : Material
//purpose  :
//=======================================================================
Graphic3d_NameOfMaterial PrsMgr_PresentableObject::Material() const
{
  return myDrawer->ShadingAspect()->Material().Name();
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetMaterial (const Graphic3d_MaterialAspect& theMaterial)
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->SetMaterial (theMaterial);
  hasOwnMaterial = Standard_True;
}

//=======================================================================
//function : UnsetMaterial
//purpose  :
//=======================================================================
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
      myDrawer->ShadingAspect()->SetMaterial (myDrawer->Link()->ShadingAspect()->Aspect()->BackMaterial());
    }

    if (HasColor())
    {
      SetColor (myDrawer->Color());
    }

    if (IsTransparent())
    {
      SetTransparency (myDrawer->Transparency());
    }
  }
  else
  {
    myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
  }

  hasOwnMaterial = Standard_False;
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetTransparency (const Standard_Real theValue)
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency (Standard_ShortReal(theValue));
  myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial() .SetTransparency (Standard_ShortReal(theValue));
  myDrawer->SetTransparency (Standard_ShortReal(theValue));
}

//=======================================================================
//function : UnsetTransparency
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::UnsetTransparency()
{
  if (HasColor() || HasMaterial())
  {
    myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency (0.0f);
    myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial() .SetTransparency (0.0f);
  }
  else
  {
    myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
  }
  myDrawer->SetTransparency (0.0f);
}

//=======================================================================
//function : SetPolygonOffsets
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetPolygonOffsets (const Standard_Integer   theMode,
                                                  const Standard_ShortReal theFactor,
                                                  const Standard_ShortReal theUnits)
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->Aspect()->SetPolygonOffsets (theMode, theFactor, theUnits);
  SynchronizeAspects();
}

//=======================================================================
//function : HasPolygonOffsets
//purpose  :
//=======================================================================
Standard_Boolean PrsMgr_PresentableObject::HasPolygonOffsets() const
{
  return !(myDrawer->HasOwnShadingAspect()
        || (myDrawer->HasLink()
         && myDrawer->ShadingAspect() == myDrawer->Link()->ShadingAspect()));
}

//=======================================================================
//function : PolygonOffsets
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::PolygonOffsets (Standard_Integer&   theMode,
                                               Standard_ShortReal& theFactor,
                                               Standard_ShortReal& theUnits) const
{
  if (HasPolygonOffsets())
  {
    myDrawer->ShadingAspect()->Aspect()->PolygonOffsets (theMode, theFactor, theUnits);
  }
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myParent)

  for (PrsMgr_Presentations::Iterator anIterator (myPresentations); anIterator.More(); anIterator.Next())
  {
    const Handle(PrsMgr_Presentation)& aPresentation = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aPresentation.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myClipPlanes.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myDrawer.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myHilightDrawer.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myDynHilightDrawer.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myTransformPersistence.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myLocalTransformation.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myTransformation.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myCombinedParentTransform.get())

  for (PrsMgr_ListOfPresentableObjects::Iterator anIterator (myChildren); anIterator.More(); anIterator.Next())
  {
    const Handle(PrsMgr_PresentableObject)& aChildObject = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aChildObject.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myInvTransformation)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTypeOfPresentation3d)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myCurrentFacingModel)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myOwnWidth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, hasOwnColor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, hasOwnMaterial)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myInfiniteState)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsMutable)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnPresentations)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToPropagateVisualState)
}
