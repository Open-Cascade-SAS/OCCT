// Created on: 1996-06-11
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _LocOpe_CSIntersector_HeaderFile
#define _LocOpe_CSIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <gp_Lin.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Circ.hxx>
#include <Geom_Curve.hxx>
#include <TopAbs_Orientation.hxx>
class LocOpe_PntFace;

//! This class provides the intersection between a set
//! of axis or a circle and the faces of a shape. The
//! intersection points are sorted in increasing
//! parameter along each axis or circle.
class LocOpe_CSIntersector
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  LocOpe_CSIntersector();

  //! Creates and performs the intersection between
  //! <Ax1> and <S>.
  LocOpe_CSIntersector(const TopoDS_Shape& S);

  //! Performs the intersection between <Ax1 and <S>.
  Standard_EXPORT void Init(const TopoDS_Shape& S);

  Standard_EXPORT void Perform(const NCollection_Sequence<gp_Lin>& Slin);

  Standard_EXPORT void Perform(const NCollection_Sequence<gp_Circ>& Scir);

  Standard_EXPORT void Perform(const NCollection_Sequence<occ::handle<Geom_Curve>>& Scur);

  //! Returns <true> if the intersection has
  //! been done.
  bool IsDone() const;

  //! Returns the number of intersection point on the
  //! element of range <I>.
  Standard_EXPORT int NbPoints(const int I) const;

  //! Returns the intersection point of range <Index> on
  //! element of range <I>. The points are sorted in
  //! increasing order of parameter along the axis.
  Standard_EXPORT const LocOpe_PntFace& Point(const int I, const int Index) const;

  //! On the element of range <I>, searches the first
  //! intersection point located after the parameter
  //! <From>, which orientation is not TopAbs_EXTERNAL.
  //! If found, returns <true>. <Or> contains
  //! the orientation of the point, <IndFrom> and
  //! <IndTo> represents the interval of index in the
  //! sequence of intersection point corresponding to
  //! the point. (IndFrom <= IndTo). <Tol> is used to
  //! determine if 2 parameters are equal.
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeAfter(const int           I,
                                     const double        From,
                                     const double        Tol,
                                     TopAbs_Orientation& Or,
                                     int&                IndFrom,
                                     int&                IndTo) const;

  //! On the element of range <I>, searches the first
  //! intersection point located before the parameter
  //! <From>, which orientation is not TopAbs_EXTERNAL.
  //! If found, returns <true>. <Or> contains
  //! the orientation of the point, <IndFrom> and
  //! <IndTo> represents the interval of index in the
  //! sequence of intersection point corresponding to
  //! the point (IndFrom <= IndTo). <Tol> is used to
  //! determine if 2 parameters are equal.
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeBefore(const int           I,
                                      const double        From,
                                      const double        Tol,
                                      TopAbs_Orientation& Or,
                                      int&                IndFrom,
                                      int&                IndTo) const;

  //! On the element of range <I>, searches the first
  //! intersection point located after the index
  //! <FromInd> ( >= FromInd + 1), which orientation is
  //! not TopAbs_EXTERNAL. If found, returns
  //! <true>. <Or> contains the orientation of
  //! the point, <IndFrom> and <IndTo> represents the
  //! interval of index in the sequence of intersection
  //! point corresponding to the point. (IndFrom <= IndTo).
  //! <Tol> is used to determine if 2 parameters are equal.
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeAfter(const int           I,
                                     const int           FromInd,
                                     const double        Tol,
                                     TopAbs_Orientation& Or,
                                     int&                IndFrom,
                                     int&                IndTo) const;

  //! On the element of range <I>, searches the first
  //! intersection point located before the index
  //! <FromInd> (<= FromInd -1), which orientation is
  //! not TopAbs_EXTERNAL. If found, returns
  //! <true>. <Or> contains the orientation of
  //! the point, <IndFrom> and <IndTo> represents the
  //! interval of index in the sequence of intersection
  //! point corresponding to the point (IndFrom <= IndTo).
  //! <Tol> is used to determine if 2 parameters are equal.
  //!
  //! Otherwise, returns <false>.
  Standard_EXPORT bool LocalizeBefore(const int           I,
                                      const int           FromInd,
                                      const double        Tol,
                                      TopAbs_Orientation& Or,
                                      int&                IndFrom,
                                      int&                IndTo) const;

  Standard_EXPORT void Destroy();

  ~LocOpe_CSIntersector() { Destroy(); }

private:
  bool         myDone;
  TopoDS_Shape myShape;
  void*        myPoints;
  int          myNbelem;
};

#include <LocOpe_CSIntersector.lxx>

#endif // _LocOpe_CSIntersector_HeaderFile
