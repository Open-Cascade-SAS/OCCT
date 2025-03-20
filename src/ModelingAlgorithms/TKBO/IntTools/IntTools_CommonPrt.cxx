// Created on: 2000-10-27
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <IntTools_CommonPrt.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfRanges.hxx>
#include <TopoDS_Edge.hxx>

//=================================================================================================

IntTools_CommonPrt::IntTools_CommonPrt()
    : myType(TopAbs_SHAPE),
      myAllNullFlag(Standard_False)

{
  //
  myPnt1.SetCoord(0., 0., 0.);
  myPnt2.SetCoord(0., 0., 0.);
  //
  // modified by NIZHNY-MKK  Wed Jun  8 16:47:04 2005.BEGIN
  myVertPar1 = 0.;
  myVertPar2 = 0.;
  // modified by NIZHNY-MKK  Wed Jun  8 16:47:07 2005.END
}

//=================================================================================================

IntTools_CommonPrt::IntTools_CommonPrt(const IntTools_CommonPrt& Other)
    : myEdge1(Other.myEdge1),
      myEdge2(Other.myEdge2),
      myType(Other.myType),
      myRange1(Other.myRange1),
      myVertPar1(Other.myVertPar1),
      myVertPar2(Other.myVertPar2),
      myAllNullFlag(Standard_False),
      //
      myPnt1(Other.myPnt1),
      myPnt2(Other.myPnt2)
//
{
  Standard_Integer i, aNb = Other.myRanges2.Length();
  for (i = 1; i <= aNb; i++)
  {
    myRanges2.Append(Other.myRanges2(i));
  }
}

//=================================================================================================

IntTools_CommonPrt& IntTools_CommonPrt::Assign(const IntTools_CommonPrt& Other)
{
  myEdge1    = Other.myEdge1;
  myEdge2    = Other.myEdge2;
  myType     = Other.myType;
  myRange1   = Other.myRange1;
  myVertPar1 = Other.myVertPar1;
  myVertPar2 = Other.myVertPar2;
  //
  myPnt1 = Other.myPnt1;
  myPnt2 = Other.myPnt2;
  //
  Standard_Integer i, aNb = Other.myRanges2.Length();
  for (i = 1; i <= aNb; i++)
  {
    myRanges2.Append(Other.myRanges2(i));
  }
  myAllNullFlag = Other.myAllNullFlag;
  return *this;
}

//=================================================================================================

void IntTools_CommonPrt::SetEdge1(const TopoDS_Edge& anEdge)
{
  myEdge1 = anEdge;
}

//=================================================================================================

void IntTools_CommonPrt::SetEdge2(const TopoDS_Edge& anEdge)
{
  myEdge2 = anEdge;
}

//=================================================================================================

void IntTools_CommonPrt::SetType(const TopAbs_ShapeEnum aType)
{
  myType = aType;
}

//=================================================================================================

void IntTools_CommonPrt::SetRange1(const IntTools_Range& aRange)
{
  myRange1 = aRange;
}

//=================================================================================================

void IntTools_CommonPrt::SetRange1(const Standard_Real tf, const Standard_Real tl)
{
  myRange1.SetFirst(tf);
  myRange1.SetLast(tl);
}

//=================================================================================================

void IntTools_CommonPrt::AppendRange2(const IntTools_Range& aRange)
{
  myRanges2.Append(aRange);
}

//=================================================================================================

void IntTools_CommonPrt::AppendRange2(const Standard_Real tf, const Standard_Real tl)
{
  IntTools_Range aRange(tf, tl);
  myRanges2.Append(aRange);
}

//=================================================================================================

void IntTools_CommonPrt::SetVertexParameter1(const Standard_Real tV)
{
  myVertPar1 = tV;
}

//=================================================================================================

void IntTools_CommonPrt::SetVertexParameter2(const Standard_Real tV)
{
  myVertPar2 = tV;
}

//=================================================================================================

const TopoDS_Edge& IntTools_CommonPrt::Edge1() const
{
  return myEdge1;
}

//=================================================================================================

const TopoDS_Edge& IntTools_CommonPrt::Edge2() const
{
  return myEdge2;
}

//=================================================================================================

TopAbs_ShapeEnum IntTools_CommonPrt::Type() const
{
  return myType;
}

//=================================================================================================

const IntTools_Range& IntTools_CommonPrt::Range1() const
{
  return myRange1;
}

//=================================================================================================

void IntTools_CommonPrt::Range1(Standard_Real& tf, Standard_Real& tl) const
{
  tf = myRange1.First();
  tl = myRange1.Last();
}

//=================================================================================================

const IntTools_SequenceOfRanges& IntTools_CommonPrt::Ranges2() const
{
  return myRanges2;
}

//=================================================================================================

IntTools_SequenceOfRanges& IntTools_CommonPrt::ChangeRanges2()
{
  return myRanges2;
}

//=================================================================================================

Standard_Real IntTools_CommonPrt::VertexParameter1() const
{
  return myVertPar1;
}

//=================================================================================================

Standard_Real IntTools_CommonPrt::VertexParameter2() const
{
  return myVertPar2;
}

//=================================================================================================

void IntTools_CommonPrt::Copy(IntTools_CommonPrt& aCP) const
{
  aCP.SetEdge1(Edge1());
  aCP.SetEdge2(Edge2());
  aCP.SetType(Type());
  aCP.SetRange1(Range1());
  aCP.SetVertexParameter1(myVertPar1);
  aCP.SetVertexParameter2(myVertPar2);

  IntTools_SequenceOfRanges aSeqRanges;
  Standard_Integer          i, aNb;
  aNb = myRanges2.Length();
  for (i = 1; i <= aNb; i++)
  {
    aCP.AppendRange2(myRanges2(i));
  }
}

//=================================================================================================

void IntTools_CommonPrt::SetAllNullFlag(const Standard_Boolean aFlag)
{
  myAllNullFlag = aFlag;
}

//=================================================================================================

Standard_Boolean IntTools_CommonPrt::AllNullFlag() const
{
  return myAllNullFlag;
}

//
//=================================================================================================

void IntTools_CommonPrt::SetBoundingPoints(const gp_Pnt& aP1, const gp_Pnt& aP2)
{
  myPnt1 = aP1;
  myPnt2 = aP2;
}

//=================================================================================================

void IntTools_CommonPrt::BoundingPoints(gp_Pnt& aP1, gp_Pnt& aP2) const
{
  aP1 = myPnt1;
  aP2 = myPnt2;
}

//
