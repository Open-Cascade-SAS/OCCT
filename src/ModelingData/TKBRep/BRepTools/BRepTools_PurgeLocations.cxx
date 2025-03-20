// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BRepTools_PurgeLocations.hxx>
#include <TopoDS_Iterator.hxx>
#include <NCollection_Vector.hxx>
#include <BRepTools.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>
#include <TopLoc_Datum3D.hxx>

//=================================================================================================

BRepTools_PurgeLocations::BRepTools_PurgeLocations()
    : myDone(Standard_False)
{
}

//=================================================================================================

Standard_Boolean BRepTools_PurgeLocations::Perform(const TopoDS_Shape& theShape)
{
  myShape = theShape;
  myMapShapes.Clear();
  myLocations.Clear();
  myDone = Standard_True;
  AddShape(myShape);

  // Check locations;
  Standard_Integer                     ind;
  NCollection_Vector<Standard_Integer> aBadTrsfInds;
  for (ind = 1;; ++ind)
  {
    const TopLoc_Location& aLoc = myLocations.Location(ind);

    if (aLoc.IsIdentity())
      break;

    const gp_Trsf&   aTrsf = aLoc.Transformation();
    Standard_Boolean isBadTrsf =
      aTrsf.IsNegative() || (Abs(Abs(aTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());
    if (isBadTrsf)
    {
      aBadTrsfInds.Append(ind);
    }
  }

  if (aBadTrsfInds.IsEmpty())
  {
    return myDone;
  }

  Standard_Integer aNbShapes = myMapShapes.Extent();
  myMapNewShapes.Clear();
  Standard_Integer inds;
  for (inds = 1; inds <= aNbShapes; ++inds)
  {
    const TopoDS_Shape& anS     = myMapShapes(inds);
    Standard_Integer    aLocInd = myLocations.Index(anS.Location());
    if (aLocInd == 0)
    {
      continue;
    }
    Standard_Integer il;
    for (il = 0; il < aBadTrsfInds.Size(); ++il)
    {
      if (aBadTrsfInds(il) == aLocInd)
      {
        TopoDS_Shape     aTrS;
        Standard_Boolean isDone = PurgeLocation(anS, aTrS);
        myDone                  = myDone && isDone;
        myMapNewShapes.Bind(anS, aTrS);
        break;
      }
    }
  }

  if (myReShape.IsNull())
  {
    myReShape = new BRepTools_ReShape;
  }
  else
  {
    myReShape->Clear();
  }
  TopTools_DataMapIteratorOfDataMapOfShapeShape anIter(myMapNewShapes);
  for (; anIter.More(); anIter.Next())
  {
    const TopoDS_Shape& anOldS = anIter.Key();
    const TopoDS_Shape& aNewS  = anIter.Value();
    myReShape->Replace(anOldS, aNewS);
  }

  myShape = myReShape->Apply(myShape);

  return myDone;
}

//=================================================================================================

Standard_Boolean BRepTools_PurgeLocations::PurgeLocation(const TopoDS_Shape& theS,
                                                         TopoDS_Shape&       theRes)
{
  Standard_Boolean isDone  = Standard_True;
  TopLoc_Location  aRefLoc = theS.Location();
  Standard_Boolean isEmpty = aRefLoc.IsIdentity();
  if (isEmpty)
  {
    theRes = theS;
    return isDone;
  }

  TopLoc_Location aNullLoc;
  theRes = theS.Located(aNullLoc);

  while (!isEmpty)
  {
    const Handle(TopLoc_Datum3D)& aFD   = aRefLoc.FirstDatum();
    gp_Trsf                       aTrsf = aFD->Trsf();
    Standard_Integer              aFP   = aRefLoc.FirstPower();
    Standard_Boolean              isBad =
      aTrsf.IsNegative() || (Abs(Abs(aTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());
    TopLoc_Location aLoc(aFD);
    aLoc  = aLoc.Powered(aFP);
    aTrsf = aLoc.Transformation();
    if (isBad)
    {
      Handle(BRepTools_TrsfModification) aModification = new BRepTools_TrsfModification(aTrsf);
      BRepTools_Modifier                 aModifier(theRes, aModification);
      if (aModifier.IsDone())
      {
        theRes = aModifier.ModifiedShape(theRes);
      }
      else
      {
        isDone = Standard_False;
        theRes = theRes.Moved(aLoc);
      }
    }
    else
    {
      theRes = theRes.Moved(aLoc);
    }

    aRefLoc = aRefLoc.NextLocation();
    isEmpty = aRefLoc.IsIdentity();
  }

  return isDone;
}

//=================================================================================================

void BRepTools_PurgeLocations::AddShape(const TopoDS_Shape& theS)
{
  myMapShapes.Add(theS);
  myLocations.Add(theS.Location());

  TopoDS_Iterator It(theS, Standard_False, Standard_False);
  while (It.More())
  {
    AddShape(It.Value());
    It.Next();
  }
}

//=================================================================================================

const TopoDS_Shape& BRepTools_PurgeLocations::GetResult() const
{
  return myShape;
}

//=================================================================================================

Standard_Boolean BRepTools_PurgeLocations::IsDone() const
{
  return myDone;
}

//=================================================================================================

TopoDS_Shape BRepTools_PurgeLocations::ModifiedShape(const TopoDS_Shape& theInitShape) const
{
  TopoDS_Shape aShape = theInitShape;
  if (myMapNewShapes.IsBound(theInitShape))
    aShape = myMapNewShapes.Find(theInitShape);
  return aShape;
}
