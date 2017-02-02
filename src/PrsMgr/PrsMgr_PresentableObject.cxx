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

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsMgr_PresentableObject, Standard_Transient)

namespace
{
  static const gp_Trsf THE_IDENTITY_TRSF;
}

//=======================================================================
//function : getIdentityTrsf
//purpose  :
//=======================================================================
const gp_Trsf& PrsMgr_PresentableObject::getIdentityTrsf()
{
  return THE_IDENTITY_TRSF;
}

//=======================================================================
//function : PrsMgr_PresentableObject
//purpose  :
//=======================================================================
PrsMgr_PresentableObject::PrsMgr_PresentableObject (const PrsMgr_TypeOfPresentation3d theType)
: myDrawer (new Prs3d_Drawer()),
  myTypeOfPresentation3d (theType),
  myIsMutable (Standard_False),
  myHasOwnPresentations (Standard_True),
  myParent (NULL)
{
  myDrawer->SetDisplayMode (-1);
}

//=======================================================================
//function : ~PrsMgr_PresentableObject
//purpose  : destructor
//=======================================================================
PrsMgr_PresentableObject::~PrsMgr_PresentableObject()
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (myChildren); anIter.More(); anIter.Next())
  {
    anIter.Value()->SetCombinedParentTransform (Handle(Geom_Transformation)());
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
  Handle(Prs3d_Presentation) aStruct3d = thePrs->Presentation();
  Compute (thePrsMgr, aStruct3d, theMode);
  UpdateTransformation (aStruct3d);
  aStruct3d->SetClipPlanes (myClipPlanes);
  aStruct3d->SetTransformPersistence (TransformPersistence());
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::Compute (const Handle(PrsMgr_PresentationManager)& /*aPresentationManager*/,
                                        const Handle(Prs3d_Presentation)& /*aPresentation*/,
                                        const Standard_Integer /*aMode*/)
{
  throw Standard_NotImplemented("cannot compute in a 3d visualizer");
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
                                       const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
  throw Standard_NotImplemented("cannot compute under a specific projector");
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::Compute(const Handle(Prs3d_Projector)& /* aProjector*/,
                                       const Handle(Geom_Transformation)& /*aTrsf*/,
				                               const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
  throw Standard_NotImplemented("cannot compute under a specific projector");
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::Update (const Standard_Boolean AllModes) {
  Standard_Integer l = myPresentations.Length();
  Handle(PrsMgr_PresentationManager) PM; 
  for (Standard_Integer i=1; i <= l; i++) {
    PM = myPresentations(i).Presentation()->PresentationManager();
    if(AllModes) 
      PM->Update(this,myPresentations(i).Mode());
    else{
      if(PM->IsDisplayed(this,myPresentations(i).Mode()) ||
	 PM->IsHighlighted(this,myPresentations(i).Mode())){
	PM->Update(this,myPresentations(i).Mode());
      }
      else
	SetToUpdate(myPresentations(i).Mode());
    }
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::Update (const Standard_Integer aMode, const Standard_Boolean ClearOther) {
  Standard_Integer l = myPresentations.Length();
  for (Standard_Integer i=1; i <= l; i++) {
    if( myPresentations(i).Mode() == aMode){
       Handle(PrsMgr_PresentationManager) PM=
	 myPresentations(i).Presentation()->PresentationManager();
       
       if(PM->IsDisplayed(this,aMode) ||
	  PM->IsHighlighted(this,aMode)){
	 PM->Update(this,aMode);
	 myPresentations(i).Presentation()->SetUpdateStatus(Standard_False);
	 
       }
       else
	 SetToUpdate(myPresentations(i).Mode());
     }
    
  }
  if(ClearOther) {
    PrsMgr_Presentations save;
    save =  myPresentations; 
    myPresentations.Clear();
    for (Standard_Integer i=1; i <= l; i++) {
      if( save(i).Mode() == aMode) myPresentations.Append(save(i));
    }
  }

}

//=======================================================================
//function : SetToUpdate
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::SetToUpdate(const Standard_Integer aMode)
{
  for(Standard_Integer IP =1; IP<=myPresentations.Length();IP++){
    if(myPresentations(IP).Mode()==aMode)
      myPresentations(IP).Presentation()->SetUpdateStatus(Standard_True);
  }
}

//=======================================================================
//function : SetToUpdate
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetToUpdate()
{
  for(Standard_Integer IP =1; IP<=myPresentations.Length();IP++){
    myPresentations(IP).Presentation()->SetUpdateStatus(Standard_True);
  }
}

//=======================================================================
//function : ToBeUpdated
//purpose  : gets the list of modes to be updated
//=======================================================================
void PrsMgr_PresentableObject::ToBeUpdated(TColStd_ListOfInteger& OutList) const
{
  OutList.Clear();
  // on dimensionne les buckets a la taille de la seq.
  TColStd_MapOfInteger MI(myPresentations.Length()); 
  
  for(Standard_Integer IP =1; IP<=myPresentations.Length();IP++){
    const PrsMgr_ModedPresentation& MP = myPresentations(IP);
    if(MP.Presentation()->MustBeUpdated())
      if(!MI.Contains(MP.Mode())){
        OutList.Append(MP.Mode());
        MI.Add(MP.Mode());
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
  for(Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
  {
    const Handle(PrsMgr_Presentation)& aPrs  = myPresentations (aPrsIter).Presentation();
    aPrs->Presentation()->SetVisual (myTypeOfPresentation3d == PrsMgr_TOP_ProjectorDependant
                                   ? Graphic3d_TOS_COMPUTED
                                   : Graphic3d_TOS_ALL);
  }
}

//=======================================================================
//function : setLocalTransformation
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::setLocalTransformation (const Handle(Geom_Transformation)& theTransformation)
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
  setLocalTransformation (Handle(Geom_Transformation)());
}

//=======================================================================
//function : SetCombinedParentTransform
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::SetCombinedParentTransform (const Handle(Geom_Transformation)& theTrsf)
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
      myTransformation    = new Geom_Transformation (aTrsf);
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

  for (Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
  {
    myPresentations (aPrsIter).Presentation()->SetTransformation (myTransformation);
  }

  for (PrsMgr_ListOfPresentableObjectsIter aChildIter (myChildren); aChildIter.More(); aChildIter.Next())
  {
    aChildIter.Value()->SetCombinedParentTransform (myTransformation);
  }
}

//=======================================================================
//function : UpdateTransformation
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::UpdateTransformation(const Handle(Prs3d_Presentation)& P)
{
  P->SetTransformation (myTransformation);
}

//=======================================================================
//function : SetTransformPersistence
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  myTransformPersistence = theTrsfPers;
  for (Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = myPresentations (aPrsIter).Presentation();
    if (!aPrs3d.IsNull()
     && !aPrs3d->Presentation().IsNull())
    {
      aPrs3d->Presentation()->SetTransformPersistence (myTransformPersistence);
      aPrs3d->Presentation()->ReCompute();
    }
  }
}

//=======================================================================
//function : GetTransformPersistence
//purpose  :
//=======================================================================
gp_Pnt PrsMgr_PresentableObject::GetTransformPersistencePoint() const
{
  if (myTransformPersistence.IsNull())
  {
    return gp_Pnt();
  }
  else if (myTransformPersistence->IsZoomOrRotate())
  {
    return myTransformPersistence->AnchorPoint();
  }
  else if (!myTransformPersistence->IsTrihedronOr2d())
  {
    return gp_Pnt();
  }

  Standard_Real anX = 0.0;
  if ((myTransformPersistence->Corner2d() & Aspect_TOTP_RIGHT) != 0)
  {
    anX = 1.0;
  }
  else if ((myTransformPersistence->Corner2d() & Aspect_TOTP_LEFT) != 0)
  {
    anX = -1.0;
  }

  Standard_Real anY = 0.0;
  if ((myTransformPersistence->Corner2d() & Aspect_TOTP_TOP) != 0)
  {
    anY = 1.0;
  }
  else if ((myTransformPersistence->Corner2d() & Aspect_TOTP_BOTTOM) != 0)
  {
    anY = -1.0;
  }

  return gp_Pnt (anX, anY, myTransformPersistence->Offset2d().x());
}

//=======================================================================
//function : AddChild
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::AddChild (const Handle(PrsMgr_PresentableObject)& theObject)
{
  Handle(PrsMgr_PresentableObject) aHandleGuard = theObject;
  if (theObject->myParent != NULL)
  {
    theObject->myParent->RemoveChild (aHandleGuard);
  }

  myChildren.Append (theObject);  
  theObject->myParent = this;
  theObject->SetCombinedParentTransform (myTransformation);
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
      theObject->SetCombinedParentTransform (Handle(Geom_Transformation)());
      myChildren.Remove (anIter);
      break;
    }
  }
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
  for (Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
  {
    const PrsMgr_ModedPresentation& aModedPrs = myPresentations (aPrsIter);
    if (aModedPrs.Presentation().IsNull()
     || aModedPrs.Presentation()->Presentation().IsNull())
    {
      continue;
    }

    aModedPrs.Presentation()->Presentation()->SetZLayer (theLayerId);
  }
}

//=======================================================================
//function : ZLayer
//purpose  :
//=======================================================================
Graphic3d_ZLayerId PrsMgr_PresentableObject::ZLayer() const
{
  return myDrawer->ZLayer();
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
  // affect generated structures
  for (Standard_Integer aPrsIt = 1; aPrsIt <= myPresentations.Length(); ++aPrsIt)
  {
    // pass over presentation manager 3d mechanism right to the structures -
    // we do not interested in display mode collections.
    const PrsMgr_ModedPresentation& aModedPrs = myPresentations (aPrsIt);
    if (aModedPrs.Presentation().IsNull()
     || aModedPrs.Presentation()->Presentation().IsNull())
    {
      continue;
    }

    aModedPrs.Presentation()->Presentation()->SetClipPlanes (myClipPlanes);
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
  for (Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
  {
    const PrsMgr_ModedPresentation& aModedPrs = myPresentations (aPrsIter);
    if (aModedPrs.Presentation().IsNull()
     || aModedPrs.Presentation()->Presentation().IsNull())
    {
      continue;
    }

    aModedPrs.Presentation()->Presentation()->SetMutable (theIsMutable);
  }
}

// =======================================================================
// function : SetAttributes
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::SetAttributes (const Handle(Prs3d_Drawer)& theDrawer)
{
  myDrawer = theDrawer;
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
}
