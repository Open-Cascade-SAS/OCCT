// Created on: 1998-01-27
// Created by: Laurent BUCHARD
// Copyright (c) 1998-1999 Matra Datavision
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

#include <IntCurvesFace_ShapeIntersector.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <gp_Lin.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

IntCurvesFace_ShapeIntersector::IntCurvesFace_ShapeIntersector()
    : myIsDone(Standard_False),
      myNbFaces(0)
{
}

void IntCurvesFace_ShapeIntersector::Load(const TopoDS_Shape& theShape, const Standard_Real theTol)
{
  TopExp_Explorer  Ex;
  Standard_Integer i;
  for (myNbFaces = 0, i = 0, Ex.Init(theShape, TopAbs_FACE); Ex.More(); ++i, Ex.Next())
  {
    ++myNbFaces;
    TopoDS_Face aCurrentFace = TopoDS::Face(Ex.Current());
    myIntersector.Append(new IntCurvesFace_Intersector(aCurrentFace, theTol));
  }
}

void IntCurvesFace_ShapeIntersector::Perform(const gp_Lin&       theL,
                                             const Standard_Real theParMin,
                                             const Standard_Real theParMax)
{
  myIsDone = Standard_False;
  for (Standard_Integer i = 1; i <= myNbFaces; ++i)
  {
    myIntersector.ChangeValue(i)->Perform(theL, theParMin, theParMax);
  }
  SortResult();
}

void IntCurvesFace_ShapeIntersector::PerformNearest(const gp_Lin&       theL,
                                                    const Standard_Real theParMin,
                                                    const Standard_Real theParMax)
{
  Standard_Integer i = 0;
  if (myNbFaces > 2)
  {
    if (myPtrNums.IsEmpty())
    {
      myPtrNums      = TColStd_HArray1OfInteger(0, myNbFaces - 1);
      myPtrIndexNums = TColStd_HArray1OfInteger(0, myNbFaces - 1);
      for (; i < myNbFaces; ++i)
      {
        myPtrNums.ChangeValue(i)      = 0;
        myPtrIndexNums.ChangeValue(i) = i + 1;
      }
    }
  }

  Standard_Integer anIndexFace = -1;
  Standard_Real    aParMax     = theParMax;
  myIsDone                     = Standard_False;
  for (Standard_Integer ii = 1; ii <= myNbFaces; ++ii)
  {
    if (!myPtrNums.IsEmpty())
    {
      i = myPtrIndexNums.Value(ii - 1);
    }
    else
    {
      i = ii;
    }
    Handle(IntCurvesFace_Intersector) anIntersector = myIntersector.ChangeValue(i);
    if (theParMin < aParMax)
    {
      anIntersector->Perform(theL, theParMin, aParMax);
      if (anIntersector->IsDone())
      {
        Standard_Integer n = anIntersector->NbPnt();
        for (Standard_Integer j = 1; j <= n; ++j)
        {
          Standard_Real w = anIntersector->WParameter(j);
          if (w < aParMax)
          {
            aParMax     = w;
            anIndexFace = ii - 1;
          }
        }
      }
      else
      {
        myIsDone = Standard_False;
        return;
      }
    }
  }
  if (!myPtrNums.IsEmpty() && anIndexFace >= 0)
  {
    myPtrNums.ChangeValue(anIndexFace) += 1;
    Standard_Integer im1;
    for (im1 = anIndexFace - 1, i = anIndexFace;
         i >= 1 && myPtrNums.Value(i) > myPtrNums.Value(im1);
         --i, --im1)
    {
      std::swap(myPtrIndexNums.ChangeValue(i), myPtrIndexNums.ChangeValue(im1));
      std::swap(myPtrNums.ChangeValue(i), myPtrNums.ChangeValue(im1));
    }
  }
  SortResult();
}

void IntCurvesFace_ShapeIntersector::Perform(const Handle(Adaptor3d_Curve)& theHCurve,
                                             const Standard_Real            theParMin,
                                             const Standard_Real            theParMax)
{
  myIsDone = Standard_False;
  for (Standard_Integer i = 1; i <= myNbFaces; ++i)
  {
    Handle(IntCurvesFace_Intersector) anIntersector = myIntersector.ChangeValue(i);
    anIntersector->Perform(theHCurve, theParMin, theParMax);
  }
  SortResult();
}

//-- ================================================================================
//-- myIntersector   : Sequence of the addresses
//-- myIndexPt          : 1 2 3 .... n  Points before the sorting
//-- myNumberFace       : Number of the face (of the intersector) of the point myIndexPt(i)
//-- myNumberIntPnt     : Number of the point  myIndexPt(i) of the intersection
// myNumberFace(myIndexPt(i))
//-- myIndexPar         : Parameter W of point myIndexPt(i)
//--
//-- To sum up, for each point index of K = myIndexPt(i) on a
//--      * the face to which it belongs                               : myNumberFace(K)
//--      * the number of the point in the intersection for FaceCurve  : myNumberIntPnt(K)
//--      * the parameter W of the point on the curve                  : myIndexPar(K)
//--
//-- SortResult Sorts the points in ascending order of W
//-- (updating the index table TabPt(.))
//--
//-- ================================================================================
void IntCurvesFace_ShapeIntersector::SortResult()
{
  myIsDone                = Standard_True;
  Standard_Integer aNbPnt = 0;
  myIndexPt.Clear();
  myIndexFace.Clear();
  myIndexIntPnt.Clear();
  myIndexPar.Clear();

  // Retrieval of the results
  for (Standard_Integer f = 1; f <= myNbFaces; ++f)
  {
    Handle(IntCurvesFace_Intersector) anIntersector = myIntersector.ChangeValue(f);
    if (anIntersector->IsDone())
    {
      Standard_Integer n = anIntersector->NbPnt();
      for (Standard_Integer j = 1; j <= n; ++j)
      {
        myIndexPt.Append(++aNbPnt);
        myIndexFace.Append(f);
        myIndexIntPnt.Append(j);
        myIndexPar.Append(anIntersector->WParameter(j));
      }
    }
    else
    {
      myIsDone = Standard_False;
      return;
    }
  }

  // Sort according to parameter  w
  Standard_Boolean isOK;
  do
  {
    isOK = Standard_True;
    for (Standard_Integer ind0 = 1; ind0 < aNbPnt; ind0++)
    {
      Standard_Integer ind   = myIndexPt(ind0);
      Standard_Integer indp1 = myIndexPt(ind0 + 1);
      if (myIndexPar(ind) > myIndexPar(indp1))
      {
        myIndexPt(ind0)     = indp1;
        myIndexPt(ind0 + 1) = ind;
        isOK                = Standard_False;
      }
    }
  } while (!isOK);
}

IntCurvesFace_ShapeIntersector::~IntCurvesFace_ShapeIntersector() {}
