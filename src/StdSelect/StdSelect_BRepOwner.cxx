// Copyright: 	Matra-Datavision 1995
// File:	StdSelect_BRepOwner.cxx
// Created:	Wed Mar  8 16:13:35 1995
// Author:	Mister rmi
//		<rmi>

#define BUC60876	//GG_050401 Enable to highlight something
//			with a specific hilight mode
//

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
  if(myFromDecomposition)
    if(myPrsSh.IsNull())
      myPrsSh = new StdSelect_Shape(myShape);
  
  if(myPrsSh.IsNull())
    PM->Highlight(Selectable(),M);
  else
    PM->Highlight(myPrsSh,M);
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
  if(myFromDecomposition){
    if(myPrsSh.IsNull()){
      if(HasLocation()){
	TopLoc_Location lbid = Location() * myShape.Location();
	TopoDS_Shape ShBis = myShape.Located(lbid);
	myPrsSh = new StdSelect_Shape(ShBis);
      }
      else
	myPrsSh = new StdSelect_Shape(myShape);
    }
  }
  if(myPrsSh.IsNull())
    PM->Color(Selectable(),aCol,M);
  else
    PM->Color(myPrsSh,aCol,M);
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
  if(!myPrsSh.IsNull())
    myPrsSh.Nullify();
  
}
void StdSelect_BRepOwner::ResetLocation()
{
  SelectMgr_EntityOwner::ResetLocation();
  if(!myPrsSh.IsNull())
    myPrsSh.Nullify(); 
}
