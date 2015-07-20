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


#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TypeOfStructure.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

//=======================================================================
//function : PrsMgr_PresentableObject
//purpose  :
//=======================================================================
PrsMgr_PresentableObject::PrsMgr_PresentableObject (const PrsMgr_TypeOfPresentation3d theType)
: myTypeOfPresentation3d (theType),
  myIsMutable (Standard_False),
  myZLayer (Graphic3d_ZLayerId_Default),
  myHasOwnPresentations (Standard_True),
  myParent (NULL)
{
  //
}

//=======================================================================
//function : ~PrsMgr_PresentableObject
//purpose  : destructor
//=======================================================================
PrsMgr_PresentableObject::~PrsMgr_PresentableObject()
{
  gp_Trsf anIdentity;
  for (PrsMgr_ListOfPresentableObjectsIter anIter (myChildren); anIter.More(); anIter.Next())
  {
    anIter.Value()->SetCombinedParentTransform (anIdentity);
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
  aStruct3d->SetTransformPersistence (GetTransformPersistenceMode(), GetTransformPersistencePoint());
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::Compute (const Handle(PrsMgr_PresentationManager)& /*aPresentationManager*/,
                                        const Handle(Prs3d_Presentation)& /*aPresentation*/,
                                        const Standard_Integer /*aMode*/)
{
  Standard_NotImplemented::Raise("cannot compute in a 3d visualizer");
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
                                       const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
  Standard_NotImplemented::Raise("cannot compute under a specific projector");
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::Compute(const Handle(Prs3d_Projector)& /* aProjector*/,
                                       const Handle(Geom_Transformation)& /*aTrsf*/,
				                               const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
  Standard_NotImplemented::Raise("cannot compute under a specific projector");
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
//function : Presentations
//purpose  : 
//=======================================================================
PrsMgr_Presentations& PrsMgr_PresentableObject::Presentations() {
  return myPresentations;
}

//=======================================================================
//function : HasTransformation
//purpose  : 
//=======================================================================
Standard_Boolean PrsMgr_PresentableObject::HasTransformation() const 
{
  return myTransformation.Form() != gp_Identity;
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
  static TColStd_MapOfInteger MI(myPresentations.Length()); 
  
  for(Standard_Integer IP =1; IP<=myPresentations.Length();IP++){
    const PrsMgr_ModedPresentation& MP = myPresentations(IP);
    if(MP.Presentation()->MustBeUpdated())
      if(!MI.Contains(MP.Mode())){
	OutList.Append(MP.Mode());
	MI.Add(MP.Mode());
      }
  }
  MI.Clear();
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
//function : SetLocalTransformation
//purpose  : WARNING : use with only 3D objects...
//=======================================================================
void PrsMgr_PresentableObject::SetLocalTransformation (const gp_Trsf& theTransformation) 
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
  SetLocalTransformation (gp_Trsf());  
}

//=======================================================================
//function : SetCombinedParentTransform
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::SetCombinedParentTransform (const gp_Trsf& theTransformation) 
{
  myCombinedParentTransform = theTransformation;
  UpdateTransformation();
}

//=======================================================================
//function : UpdateTransformation
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::UpdateTransformation()
{
  myTransformation = myCombinedParentTransform * myLocalTransformation;
  myInvTransformation = myTransformation.Inverted();
  Handle(Geom_Transformation) aTrsf = new Geom_Transformation (myTransformation);

  for (Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
  {
    myPresentations (aPrsIter).Presentation()->Transform (aTrsf);
  }
  
  PrsMgr_ListOfPresentableObjectsIter anIter (myChildren);

  for (; anIter.More(); anIter.Next())
  {
    anIter.Value()->SetCombinedParentTransform (myTransformation);
  }
}

//=======================================================================
//function : UpdateTransformation
//purpose  : 
//=======================================================================
void PrsMgr_PresentableObject::UpdateTransformation(const Handle(Prs3d_Presentation)& P)
{
  Handle(Geom_Transformation) aTrsf = new Geom_Transformation (myTransformation);
  P->Transform (aTrsf);  
}

//=======================================================================
//function : SetTransformPersistence
//purpose  :
//=======================================================================
void PrsMgr_PresentableObject::SetTransformPersistence (const Graphic3d_TransModeFlags& theFlag,
                                                        const gp_Pnt&                   thePoint)
{
  myTransformPersistence.Flags     = theFlag;
  myTransformPersistence.Point.x() = thePoint.X();
  myTransformPersistence.Point.y() = thePoint.Y();
  myTransformPersistence.Point.z() = thePoint.Z();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = myPresentations (aPrsIter).Presentation();
    if (!aPrs3d.IsNull()
     && !aPrs3d->Presentation().IsNull())
    {
      aPrs3d->Presentation()->SetTransformPersistence (theFlag, thePoint);
      aPrs3d->Presentation()->ReCompute();
    }
  }
}

//=======================================================================
//function : SetTransformPersistence
//purpose  : 
//=======================================================================
void  PrsMgr_PresentableObject::SetTransformPersistence( 
				       const Graphic3d_TransModeFlags& TheFlag )
{
  SetTransformPersistence( TheFlag, gp_Pnt(0,0,0) );
}

//=======================================================================
//function : GetTransformPersistence
//purpose  : 
//=======================================================================
Graphic3d_TransModeFlags PrsMgr_PresentableObject::GetTransformPersistenceMode() const
{
  return myTransformPersistence.Flags;
}

//=======================================================================
//function : GetTransformPersistence
//purpose  : 
//=======================================================================
gp_Pnt PrsMgr_PresentableObject::GetTransformPersistencePoint() const
{
  return gp_Pnt (myTransformPersistence.Point.x(),
                 myTransformPersistence.Point.y(),
                 myTransformPersistence.Point.z());
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
      theObject->SetCombinedParentTransform (gp_Trsf());
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
  if (myZLayer == theLayerId)
  {
    return;
  }

  myZLayer = theLayerId;
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
  return myZLayer;
}

// =======================================================================
// function : AddClipPlane
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::AddClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  // add to collection and process changes
  myClipPlanes.Append (thePlane);
  UpdateClipping();
}

// =======================================================================
// function : RemoveClipPlane
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::RemoveClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  // remove from collection and process changes
  Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (myClipPlanes);
  for (; aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (aPlane != thePlane)
      continue;

    myClipPlanes.Remove (aPlaneIt);
    UpdateClipping();
    return;
  }
}

// =======================================================================
// function : SetClipPlanes
// purpose  :
// =======================================================================
void PrsMgr_PresentableObject::SetClipPlanes (const Graphic3d_SequenceOfHClipPlane& thePlanes)
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
// function : IsMutable
// purpose  :
// =======================================================================
const Standard_Boolean PrsMgr_PresentableObject::IsMutable() const
{
  return myIsMutable;
}
