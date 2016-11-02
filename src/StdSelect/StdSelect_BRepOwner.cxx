// Created on: 1995-03-08
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#include <StdSelect_BRepOwner.hxx>

#include <Graphic3d_StructureManager.hxx>
#include <Prs3d_Drawer.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_Shape.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdSelect_BRepOwner,SelectMgr_EntityOwner)

//==================================================
// Function: StdSelect_BRepOwner
// Purpose :
//==================================================
StdSelect_BRepOwner::StdSelect_BRepOwner (const Standard_Integer thePriority)
: SelectMgr_EntityOwner (thePriority),
  myCurMode (0)
{
  //
}

//==================================================
// Function: StdSelect_BRepOwner
// Purpose :
//==================================================
StdSelect_BRepOwner::StdSelect_BRepOwner (const TopoDS_Shape& theShape,
                                          const Standard_Integer thePriority,
                                          const Standard_Boolean theComesFromDecomposition)
: SelectMgr_EntityOwner (thePriority),
  myShape (theShape),
  myCurMode (0)
{
  myFromDecomposition = theComesFromDecomposition;
}

//==================================================
// Function: StdSelect_BRepOwner
// Purpose :
//==================================================
StdSelect_BRepOwner::StdSelect_BRepOwner (const TopoDS_Shape& theShape,
                                          const Handle (SelectMgr_SelectableObject)& theOrigin,
                                          const Standard_Integer thePriority,
                                          const Standard_Boolean theComesFromDecomposition)
: SelectMgr_EntityOwner (theOrigin, thePriority),
  myShape (theShape),
  myCurMode (0)
{
  myFromDecomposition = theComesFromDecomposition;
}

//=======================================================================
//function : IsHilighted
//purpose  : 
//=======================================================================
Standard_Boolean StdSelect_BRepOwner::
IsHilighted(const Handle(PrsMgr_PresentationManager)& PM,
	    const Standard_Integer aMode) const 
{
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
  if(myPrsSh.IsNull())
    return PM->IsHighlighted(Selectable(),M);
  return PM->IsHighlighted(myPrsSh,M);
}

//=======================================================================
//function : HilightWithColor
//purpose  :
//=======================================================================
void StdSelect_BRepOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                            const Handle(Prs3d_Drawer)&                 theStyle,
                                            const Standard_Integer                      theMode)
{
  if (!HasSelectable())
  {
    return;
  }

  const Standard_Integer aDispMode = (theMode < 0) ? myCurMode : theMode;
  Handle(SelectMgr_SelectableObject) aSel = Selectable();
  const Graphic3d_ZLayerId aHiLayer = theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN ? theStyle->ZLayer() : aSel->ZLayer();
  if (!myFromDecomposition)
  {
    thePM->Color (aSel, theStyle, aDispMode, NULL, aHiLayer);
    return;
  }

  // do the update flag check
  if (!myPrsSh.IsNull())
  {
    TColStd_ListOfInteger aModesList;
    myPrsSh->ToBeUpdated (aModesList);
    if (!aModesList.IsEmpty())
      myPrsSh.Nullify();
  }

  // generate new presentable shape
  if (myPrsSh.IsNull())
  {
    if (HasLocation())
    {
      TopLoc_Location lbid = Location() * myShape.Location();
      TopoDS_Shape ShBis = myShape.Located(lbid);
      myPrsSh = new StdSelect_Shape (ShBis, theStyle);
    }
    else
    {
      myPrsSh = new StdSelect_Shape (myShape, theStyle);
    }
  }

  // initialize presentation attributes of child presentation
  myPrsSh->SetZLayer               (aSel->ZLayer());
  myPrsSh->SetTransformPersistence (aSel->TransformPersistence());
  myPrsSh->Attributes()->SetLink                (theStyle);
  myPrsSh->Attributes()->SetColor               (theStyle->Color());
  myPrsSh->Attributes()->SetTransparency        (theStyle->Transparency());
  myPrsSh->Attributes()->SetBasicFillAreaAspect (theStyle->BasicFillAreaAspect());

  // highlight with color and set layer
  thePM->Color (myPrsSh, theStyle, aDispMode, aSel, aHiLayer);
}

void StdSelect_BRepOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePrsMgr, const Standard_Integer )
{
  if (myPrsSh.IsNull() || !myFromDecomposition)
  {
    thePrsMgr->Unhighlight (Selectable());
  }
  else
  {
    thePrsMgr->Unhighlight (myPrsSh);
  }
}

void StdSelect_BRepOwner::Clear(const Handle(PrsMgr_PresentationManager)& PM,
				const Standard_Integer aMode)
{
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
  if (!myPrsSh.IsNull())
    PM->Clear(myPrsSh,M);
  myPrsSh.Nullify();
}

void StdSelect_BRepOwner::SetLocation(const TopLoc_Location& aLoc)
{
  SelectMgr_EntityOwner::SetLocation(aLoc);
  // we must not nullify the myPrsSh here, because unhilight method
  // will be working with wrong entity in this case, the best is to
  // set the update flag and then recompute myPrsSh on hilighting
  if (!myPrsSh.IsNull())
    myPrsSh->SetToUpdate();
}

void StdSelect_BRepOwner::ResetLocation()
{
  SelectMgr_EntityOwner::ResetLocation();
  // we must not nullify the myPrsSh here, because unhilight method
  // will be working with wrong entity in this case, the best is to
  // set the update flag and then recompute myPrsSh on hilighting
  if (!myPrsSh.IsNull())
    myPrsSh->SetToUpdate();
}

//=======================================================================
//function : UpdateHighlightTrsf
//purpose  :
//=======================================================================
void StdSelect_BRepOwner::UpdateHighlightTrsf (const Handle(V3d_Viewer)& theViewer,
                                               const Handle(PrsMgr_PresentationManager3d)& theManager,
                                               const Standard_Integer theDispMode)
{
  if (myPrsSh.IsNull() && Selectable().IsNull())
    return;

  theManager->UpdateHighlightTrsf (theViewer, Selectable(), theDispMode, myPrsSh);
}
