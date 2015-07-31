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


#include <Graphic3d_StructureManager.hxx>
#include <Prs3d_Drawer.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_Shape.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

//==================================================
// Function: 
// Purpose :
//==================================================
StdSelect_BRepOwner::StdSelect_BRepOwner(const Standard_Integer aPriority):
SelectMgr_EntityOwner(aPriority),
myFromDecomposition(Standard_False),
myCurMode(0)
{
}

StdSelect_BRepOwner::StdSelect_BRepOwner(const TopoDS_Shape& aShape,
					 const Standard_Integer aPriority,
					 const Standard_Boolean ComesFromDecomposition):
SelectMgr_EntityOwner(aPriority),
myFromDecomposition(ComesFromDecomposition),
myShape(aShape),
myCurMode(0)
{
}

StdSelect_BRepOwner::StdSelect_BRepOwner(const TopoDS_Shape& aShape,
					 const Handle (SelectMgr_SelectableObject)& theOrigin,
					 const Standard_Integer aPriority,
					 const Standard_Boolean ComesFromDecomposition):
SelectMgr_EntityOwner(theOrigin,aPriority),
myFromDecomposition(ComesFromDecomposition),
myShape(aShape),
myCurMode(0)
{
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
//function : Hilight/Unhilight Methods...
//purpose  : 
//=======================================================================
void StdSelect_BRepOwner::Hilight(const Handle(PrsMgr_PresentationManager)& PM,
				  const Standard_Integer aMode)
{
  
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
  Handle(SelectMgr_SelectableObject) aSel = Selectable();
  if (myFromDecomposition)
  {
    // do the update flag check
    if (!myPrsSh.IsNull())
    {
      TColStd_ListOfInteger aModesList;
      myPrsSh->ToBeUpdated (aModesList);
      if (!aModesList.IsEmpty())
        myPrsSh.Nullify();
    }

    Handle(Prs3d_Drawer) aDrawer;
    if (!aSel.IsNull())
    {
      aDrawer = aSel->HilightAttributes();
    }
    else
    {
      aDrawer = new Prs3d_Drawer();
      SelectMgr_SelectableObject::InitDefaultHilightAttributes (aDrawer);
    }

    // generate new presentable shape
    if (myPrsSh.IsNull())
    {
      myPrsSh = new StdSelect_Shape (myShape, aDrawer);
    }
    if (!aSel.IsNull())
    {
      myPrsSh->SetZLayer (aSel->ZLayer());
    }

    // highlight and set layer
    PM->Highlight (myPrsSh, M);
  }  
  else
  {
    if(myPrsSh.IsNull())
      PM->Highlight(aSel,M);
    else
      PM->Highlight(myPrsSh,M);
  }
}

void StdSelect_BRepOwner::Hilight()
{}

void StdSelect_BRepOwner::HilightWithColor(const Handle(PrsMgr_PresentationManager3d)& PM,
					   const Quantity_NameOfColor aCol,
					   const Standard_Integer aMode)
{
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
  Graphic3d_ZLayerId aHiLayer = this == Selectable()->GlobalSelOwner().get() ?
                                Graphic3d_ZLayerId_Top : Graphic3d_ZLayerId_Topmost;
  Handle(SelectMgr_SelectableObject) aSel = Selectable();
  if (myFromDecomposition)
  {
    // do the update flag check
    if (!myPrsSh.IsNull())
    {
      TColStd_ListOfInteger aModesList;
      myPrsSh->ToBeUpdated (aModesList);
      if (!aModesList.IsEmpty())
        myPrsSh.Nullify();
    }

    Handle(Prs3d_Drawer) aDrawer;
    if (!aSel.IsNull())
    {
      aDrawer = aSel->HilightAttributes();
    }
    else
    {
      aDrawer = new Prs3d_Drawer();
      SelectMgr_SelectableObject::InitDefaultHilightAttributes (aDrawer);
    }

    // generate new presentable shape
    if(myPrsSh.IsNull())
    {
      if(HasLocation())
      {
        TopLoc_Location lbid = Location() * myShape.Location();
        TopoDS_Shape ShBis = myShape.Located(lbid);
        myPrsSh = new StdSelect_Shape(ShBis, aDrawer);
      }
      else
        myPrsSh = new StdSelect_Shape(myShape, aDrawer);
    }
    if (!aSel.IsNull())
    {
      myPrsSh->SetZLayer (aSel->ZLayer());
    }

    // highlight with color and set layer
    PM->Color (myPrsSh, aCol, M, aSel, aHiLayer);
  }
  else
  {
    if (!myPrsSh.IsNull())
    {
      PM->Color (myPrsSh, aCol, M, aSel, aHiLayer);
    }
    else
    {
      PM->Color (aSel, aCol, M, NULL, aHiLayer);
    }
  }
}

void StdSelect_BRepOwner::Unhilight(const Handle(PrsMgr_PresentationManager)& PM,
				    const Standard_Integer aMode)
{
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
  if(myPrsSh.IsNull() || !myFromDecomposition)
    PM->Unhighlight(Selectable(),M);
  else
    PM->Unhighlight(myPrsSh,M);
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
//function : SetZLayer
//purpose  :
//=======================================================================
void StdSelect_BRepOwner::SetZLayer (const Graphic3d_ZLayerId theLayerId)
{
  if (!myPrsSh.IsNull())
  {
    myPrsSh->SetZLayer (theLayerId);
  }
}
