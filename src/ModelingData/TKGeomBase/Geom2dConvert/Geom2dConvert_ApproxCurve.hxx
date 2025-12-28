// Created on: 1997-09-11
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Geom2dConvert_ApproxCurve_HeaderFile
#define _Geom2dConvert_ApproxCurve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_OStream.hxx>

class Geom2d_BSplineCurve;
class Geom2d_Curve;

//! A framework to convert a 2D curve to a BSpline.
//! This is done by approximation within a given tolerance.
class Geom2dConvert_ApproxCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an approximation framework defined by
  //! -   the 2D conic Curve
  //! -   the tolerance value Tol2d
  //! -   the degree of continuity Order
  //! -   the maximum number of segments allowed MaxSegments
  //! -   the highest degree MaxDegree which the
  //! polynomial defining the BSpline is allowed to have.
  Standard_EXPORT Geom2dConvert_ApproxCurve(const occ::handle<Geom2d_Curve>& Curve,
                                            const double                     Tol2d,
                                            const GeomAbs_Shape              Order,
                                            const int                        MaxSegments,
                                            const int                        MaxDegree);

  //! Constructs an approximation framework defined by
  //! -   the 2D conic Curve
  //! -   the tolerance value Tol2d
  //! -   the degree of continuity Order
  //! -   the maximum number of segments allowed MaxSegments
  //! -   the highest degree MaxDegree which the
  //! polynomial defining the BSpline is allowed to have.
  Standard_EXPORT Geom2dConvert_ApproxCurve(const occ::handle<Adaptor2d_Curve2d>& Curve,
                                            const double                          Tol2d,
                                            const GeomAbs_Shape                   Order,
                                            const int                             MaxSegments,
                                            const int                             MaxDegree);

  //! Returns the 2D BSpline curve resulting from the
  //! approximation algorithm.
  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> Curve() const;

  //! returns true if the approximation has
  //! been done with within required tolerance
  Standard_EXPORT bool IsDone() const;

  //! returns true if the approximation did come out
  //! with a result that is not NECESSARELY within the required tolerance
  Standard_EXPORT bool HasResult() const;

  //! Returns the greatest distance between a point on the
  //! source conic and the BSpline curve resulting from the
  //! approximation. (>0 when an approximation
  //! has been done, 0 if no approximation)
  Standard_EXPORT double MaxError() const;

  //! Print on the stream o information about the object
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  //! Converts a curve to B-spline
  Standard_EXPORT void Approximate(const occ::handle<Adaptor2d_Curve2d>& theCurve,
                                   const double                          theTol3d,
                                   const GeomAbs_Shape                   theOrder,
                                   const int                             theMaxSegments,
                                   const int                             theMaxDegree);

  bool                             myIsDone;
  bool                             myHasResult;
  occ::handle<Geom2d_BSplineCurve> myBSplCurve;
  double                           myMaxError;
};

#endif // _Geom2dConvert_ApproxCurve_HeaderFile
