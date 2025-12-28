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

#ifndef _IntTools_CommonPrt_HeaderFile
#define _IntTools_CommonPrt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <IntTools_Range.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>

//! The class is to describe a common part
//! between two edges in 3D space.
class IntTools_CommonPrt
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT IntTools_CommonPrt();

  //! Copy constructor
  Standard_EXPORT IntTools_CommonPrt(const IntTools_CommonPrt& aCPrt);

  Standard_EXPORT IntTools_CommonPrt& Assign(const IntTools_CommonPrt& Other);

  IntTools_CommonPrt& operator=(const IntTools_CommonPrt& Other) { return Assign(Other); }

  //! Sets the first edge.
  Standard_EXPORT void SetEdge1(const TopoDS_Edge& anE);

  //! Sets the second edge.
  Standard_EXPORT void SetEdge2(const TopoDS_Edge& anE);

  //! Sets the type of the common part
  //! Vertex or Edge
  Standard_EXPORT void SetType(const TopAbs_ShapeEnum aType);

  //! Sets the range of first edge.
  Standard_EXPORT void SetRange1(const IntTools_Range& aR);

  //! Sets the range of first edge.
  Standard_EXPORT void SetRange1(const double tf, const double tl);

  //! Appends the range of second edge.
  Standard_EXPORT void AppendRange2(const IntTools_Range& aR);

  //! Appends the range of second edge.
  Standard_EXPORT void AppendRange2(const double tf, const double tl);

  //! Sets a parameter of first vertex
  Standard_EXPORT void SetVertexParameter1(const double tV);

  //! Sets a parameter of second vertex
  Standard_EXPORT void SetVertexParameter2(const double tV);

  //! Returns the first edge.
  Standard_EXPORT const TopoDS_Edge& Edge1() const;

  //! Returns the second edge
  Standard_EXPORT const TopoDS_Edge& Edge2() const;

  //! Returns the type of the common part
  Standard_EXPORT TopAbs_ShapeEnum Type() const;

  //! Returns the range of first edge
  Standard_EXPORT const IntTools_Range& Range1() const;

  //! Returns the range of first edge.
  Standard_EXPORT void Range1(double& tf, double& tl) const;

  //! Returns the ranges of second edge.
  Standard_EXPORT const NCollection_Sequence<IntTools_Range>& Ranges2() const;

  //! Returns the ranges of second edge.
  Standard_EXPORT NCollection_Sequence<IntTools_Range>& ChangeRanges2();

  //! Returns parameter of first vertex
  Standard_EXPORT double VertexParameter1() const;

  //! Returns parameter of second vertex
  Standard_EXPORT double VertexParameter2() const;

  //! Copies me to anOther
  Standard_EXPORT void Copy(IntTools_CommonPrt& anOther) const;

  //! Modifier
  Standard_EXPORT bool AllNullFlag() const;

  //! Selector
  Standard_EXPORT void SetAllNullFlag(const bool aFlag);

  //! Modifier
  Standard_EXPORT void SetBoundingPoints(const gp_Pnt& aP1, const gp_Pnt& aP2);

  //! Selector
  Standard_EXPORT void BoundingPoints(gp_Pnt& aP1, gp_Pnt& aP2) const;

private:
  TopoDS_Edge               myEdge1;
  TopoDS_Edge               myEdge2;
  TopAbs_ShapeEnum          myType;
  IntTools_Range            myRange1;
  double             myVertPar1;
  double             myVertPar2;
  NCollection_Sequence<IntTools_Range> myRanges2;
  bool          myAllNullFlag;
  gp_Pnt                    myPnt1;
  gp_Pnt                    myPnt2;
};

#endif // _IntTools_CommonPrt_HeaderFile
