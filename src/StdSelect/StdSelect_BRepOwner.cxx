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
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_Shape.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdSelect_BRepOwner, SelectMgr_EntityOwner)

//=================================================================================================

StdSelect_BRepOwner::StdSelect_BRepOwner(const Standard_Integer thePriority)
    : SelectMgr_EntityOwner(thePriority),
      myCurMode(0)
{
  //
}

//=================================================================================================

StdSelect_BRepOwner::StdSelect_BRepOwner(const TopoDS_Shape&    theShape,
                                         const Standard_Integer thePriority,
                                         const Standard_Boolean theComesFromDecomposition)
    : SelectMgr_EntityOwner(thePriority),
      myShape(theShape),
      myCurMode(0)
{
  myFromDecomposition = theComesFromDecomposition;
}

//=================================================================================================

StdSelect_BRepOwner::StdSelect_BRepOwner(const TopoDS_Shape&                       theShape,
                                         const Handle(SelectMgr_SelectableObject)& theOrigin,
                                         const Standard_Integer                    thePriority,
                                         const Standard_Boolean theComesFromDecomposition)
    : SelectMgr_EntityOwner(theOrigin, thePriority),
      myShape(theShape),
      myCurMode(0)
{
  myFromDecomposition = theComesFromDecomposition;
}

//=================================================================================================

Standard_Boolean StdSelect_BRepOwner::IsHilighted(const Handle(PrsMgr_PresentationManager)& PM,
                                                  const Standard_Integer aMode) const
{
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
  if (myPrsSh.IsNull())
    return PM->IsHighlighted(Selectable(), M);
  return PM->IsHighlighted(myPrsSh, M);
}

//=================================================================================================

void StdSelect_BRepOwner::HilightWithColor(const Handle(PrsMgr_PresentationManager)& thePM,
                                           const Handle(Prs3d_Drawer)&               theStyle,
                                           const Standard_Integer                    theMode)
{
  if (!HasSelectable())
  {
    return;
  }

  const Standard_Integer             aDispMode = (theMode < 0) ? myCurMode : theMode;
  Handle(SelectMgr_SelectableObject) aSel      = Selectable();
  const Graphic3d_ZLayerId           aHiLayer =
    theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN ? theStyle->ZLayer() : aSel->ZLayer();
  if (!myFromDecomposition)
  {
    thePM->Color(aSel, theStyle, aDispMode, NULL, aHiLayer);
    return;
  }

  // do the update flag check
  if (!myPrsSh.IsNull() && myPrsSh->ToBeUpdated(true))
  {
    myPrsSh.Nullify();
  }

  // generate new presentable shape
  if (myPrsSh.IsNull())
  {
    myPrsSh = new StdSelect_Shape(myShape, theStyle);
  }

  // initialize presentation attributes of child presentation
  myPrsSh->SetZLayer(aSel->ZLayer());
  myPrsSh->SetTransformPersistence(aSel->TransformPersistence());
  myPrsSh->SetLocalTransformation(Location());
  myPrsSh->Attributes()->SetLink(theStyle);
  myPrsSh->Attributes()->SetColor(theStyle->Color());
  myPrsSh->Attributes()->SetTransparency(theStyle->Transparency());
  myPrsSh->Attributes()->SetBasicFillAreaAspect(theStyle->BasicFillAreaAspect());

  // highlight with color and set layer
  thePM->Color(myPrsSh, theStyle, aDispMode, aSel, aHiLayer);
}

void StdSelect_BRepOwner::Unhilight(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                    const Standard_Integer)
{
  if (myPrsSh.IsNull() || !myFromDecomposition)
  {
    thePrsMgr->Unhighlight(Selectable());
  }
  else
  {
    thePrsMgr->Unhighlight(myPrsSh);
  }
}

void StdSelect_BRepOwner::Clear(const Handle(PrsMgr_PresentationManager)& PM,
                                const Standard_Integer                    aMode)
{
  Standard_Integer M = (aMode < 0) ? myCurMode : aMode;
  if (!myPrsSh.IsNull())
    PM->Clear(myPrsSh, M);
  myPrsSh.Nullify();
}

void StdSelect_BRepOwner::SetLocation(const TopLoc_Location& aLoc)
{
  SelectMgr_EntityOwner::SetLocation(aLoc);
  if (!myPrsSh.IsNull())
  {
    myPrsSh->SetLocalTransformation(Location());
  }
}

//=================================================================================================

void StdSelect_BRepOwner::UpdateHighlightTrsf(const Handle(V3d_Viewer)&                 theViewer,
                                              const Handle(PrsMgr_PresentationManager)& theManager,
                                              const Standard_Integer                    theDispMode)
{
  if (!myPrsSh.IsNull() || HasSelectable())
  {
    theManager->UpdateHighlightTrsf(theViewer, Selectable(), theDispMode, myPrsSh);
  }
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void StdSelect_BRepOwner::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myShape)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myPrsSh.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurMode)
}
