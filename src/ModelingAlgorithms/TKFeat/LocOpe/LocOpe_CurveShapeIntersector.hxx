// Created on: 1995-05-29
// Created by: Jacques GOUSSARD
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

#ifndef _LocOpe_CurveShapeIntersector_HeaderFile
#define _LocOpe_CurveShapeIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <LocOpe_PntFace.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
class gp_Ax1;
class TopoDS_Shape;
class gp_Circ;
class LocOpe_PntFace;

//! This class provides the intersection between an
//! axis or a circle and the faces of a shape. The
//! intersection points are sorted in increasing
//! parameter along the axis.
class LocOpe_CurveShapeIntersector
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  LocOpe_CurveShapeIntersector();

  //! Creates and performs the intersection between
  //! <Ax1> and <S>.
  LocOpe_CurveShapeIntersector(const gp_Ax1& Axis, const TopoDS_Shape& S);

  //! Creates and performs the intersection between
  //! <C> and <S>.
  LocOpe_CurveShapeIntersector(const gp_Circ& C, const TopoDS_Shape& S);

  //! Performs the intersection between <Ax1 and <S>.
  Standard_EXPORT void Init(const gp_Ax1& Axis, const TopoDS_Shape& S);

  //! Performs the intersection between <Ax1 and <S>.
  Standard_EXPORT void Init(const gp_Circ& C, const TopoDS_Shape& S);

  //! Returns <true> if the intersection has
  //! been done.
  bool IsDone() const;

  //! Returns the number of intersection point.
  int NbPoints() const;

  //! Returns the intersection point of range <Index>.
  //! The points are sorted in increasing order of
  //! parameter along the axis.
  const LocOpe_PntFace& Point(const int Index) const;

  //! Searches the first intersection point located
  //! after the parameter <From>, which orientation is
  //! not TopAbs_EXTERNAL. If found, returns
  //! <true>. <Or> contains the orientation of
  //! the point, <IndFrom> and <IndTo> represents the
  //! interval of index in the sequence of intersection
  //! point corresponding to the point. (IndFrom <= IndTo).
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeAfter(const double From,
                                                 TopAbs_Orientation& Or,
                                                 int&   IndFrom,
                                                 int&   IndTo) const;

  //! Searches the first intersection point located
  //! before the parameter <From>, which orientation is
  //! not TopAbs_EXTERNAL. If found, returns
  //! <true>. <Or> contains the orientation of
  //! the point, <IndFrom> and <IndTo> represents the
  //! interval of index in the sequence of intersection
  //! point corresponding to the point (IndFrom <= IndTo).
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeBefore(const double From,
                                                  TopAbs_Orientation& Or,
                                                  int&   IndFrom,
                                                  int&   IndTo) const;

  //! Searches the first intersection point located
  //! after the index <FromInd> (>= FromInd + 1), which
  //! orientation is not TopAbs_EXTERNAL. If found,
  //! returns <true>. <Or> contains the
  //! orientation of the point, <IndFrom> and <IndTo>
  //! represents the interval of index in the sequence
  //! of intersection point corresponding to the point.
  //! (IndFrom <= IndTo).
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeAfter(const int FromInd,
                                                 TopAbs_Orientation&    Or,
                                                 int&      IndFrom,
                                                 int&      IndTo) const;

  //! Searches the first intersection point located
  //! before the index <FromInd> ( <= FromInd -1), which
  //! orientation is not TopAbs_EXTERNAL. If found,
  //! returns <true>. <Or> contains the
  //! orientation of the point, <IndFrom> and <IndTo>
  //! represents the interval of index in the sequence
  //! of intersection point corresponding to the point
  //! (IndFrom <= IndTo).
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeBefore(const int FromInd,
                                                  TopAbs_Orientation&    Or,
                                                  int&      IndFrom,
                                                  int&      IndTo) const;

private:
  bool         myDone;
  NCollection_Sequence<LocOpe_PntFace> myPoints;
};

#include <LocOpe_CurveShapeIntersector.lxx>

#endif // _LocOpe_CurveShapeIntersector_HeaderFile
