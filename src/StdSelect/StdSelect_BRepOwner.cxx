// Created on: 1995-03-08
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#define BUC60876	//GG_050401 Enable to highlight something
//			with a specific hilight mode

#include <StdSelect_BRepOwner.ixx>
#include <SelectBasics_EntityOwner.hxx>

#include <StdPrs_WFShape.hxx>

#include <Graphic3d_StructureManager.hxx>

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
#ifdef BUC60876
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
#else
  Standard_Integer M = (myCurMode==-1) ? aMode:myCurMode;
#endif
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
  
#ifdef BUC60876
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
#else
  Standard_Integer M = (myCurMode==-1) ? aMode:myCurMode;
#endif
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

    // generate new presentable shape
    if(myPrsSh.IsNull())
      myPrsSh = new StdSelect_Shape (myShape);

    // highlight and set layer
    PM->Highlight (myPrsSh, M);
    Handle(SelectMgr_SelectableObject) aSel = Selectable();
    if (!aSel.IsNull())
    {
      Standard_Integer aLayer = aSel->GetZLayer (PM);
      if (aLayer >= 0)
        PM->SetZLayer (myPrsSh, aLayer);
    }
  }  
  else
  {
    if(myPrsSh.IsNull())
      PM->Highlight(Selectable(),M);
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
#ifdef BUC60876
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
#else
  Standard_Integer M = (myCurMode==-1) ? aMode:myCurMode;
#endif
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

    // generate new presentable shape
    if(myPrsSh.IsNull())
    {
      if(HasLocation())
      {
        TopLoc_Location lbid = Location() * myShape.Location();
        TopoDS_Shape ShBis = myShape.Located(lbid);
        myPrsSh = new StdSelect_Shape(ShBis);
      }
      else
        myPrsSh = new StdSelect_Shape(myShape);
    }

    // highlight with color and set layer
    PM->Color (myPrsSh, aCol, M);
    Handle(SelectMgr_SelectableObject) aSel = Selectable();
    if (!aSel.IsNull())
    {
      Standard_Integer aLayer = aSel->GetZLayer (PM);
      if (aLayer >= 0)
        PM->SetZLayer (myPrsSh, aLayer);
    }
  }
  else
  {
    if(myPrsSh.IsNull())
      PM->Color(Selectable(),aCol,M);
    else
      PM->Color(myPrsSh,aCol,M);
  }
}

void StdSelect_BRepOwner::Unhilight(const Handle(PrsMgr_PresentationManager)& PM,
				    const Standard_Integer aMode)
{
#ifdef BUC60876
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
#else
  Standard_Integer M = (myCurMode==-1) ? aMode:myCurMode;
#endif
  if(myPrsSh.IsNull() || !myFromDecomposition)
    PM->Unhighlight(Selectable(),M);
  else
    PM->Unhighlight(myPrsSh,M);
}

void StdSelect_BRepOwner::Clear(const Handle(PrsMgr_PresentationManager)& PM,
				const Standard_Integer aMode)
{
#ifdef BUC60876
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
#else
  Standard_Integer M = (myCurMode==-1) ? aMode:myCurMode;
#endif
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
void StdSelect_BRepOwner::SetZLayer 
  (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
   const Standard_Integer theLayerId)
{
  if (!myPrsSh.IsNull())
    thePrsMgr->SetZLayer (myPrsSh, theLayerId);
}
