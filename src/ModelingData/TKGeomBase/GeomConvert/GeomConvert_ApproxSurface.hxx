// Created on: 1997-08-26
// Created by: Stepan MISHIN
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

#ifndef _GeomConvert_ApproxSurface_HeaderFile
#define _GeomConvert_ApproxSurface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_OStream.hxx>

class Geom_BSplineSurface;
class Geom_Surface;

//! A framework to convert a surface to a BSpline
//! surface. This is done by approximation to a BSpline
//! surface within a given tolerance.
class GeomConvert_ApproxSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a surface approximation framework defined by
  //! -   the conic Surf
  //! -   the tolerance value Tol3d
  //! -   the degree of continuity UContinuity, VContinuity
  //! in the directions of the U and V parameters
  //! -   the highest degree MaxDegU, MaxDegV which
  //! the polynomial defining the BSpline curve may
  //! have in the directions of the U and V parameters
  //! -   the maximum number of segments MaxSegments
  //! allowed in the resulting BSpline curve
  //! -   the index of precision PrecisCode.
  Standard_EXPORT GeomConvert_ApproxSurface(const occ::handle<Geom_Surface>& Surf,
                                            const double                     Tol3d,
                                            const GeomAbs_Shape              UContinuity,
                                            const GeomAbs_Shape              VContinuity,
                                            const int                        MaxDegU,
                                            const int                        MaxDegV,
                                            const int                        MaxSegments,
                                            const int                        PrecisCode);

  //! Constructs a surface approximation framework defined by
  //! -   the Surf
  //! -   the tolerance value Tol3d
  //! -   the degree of continuity UContinuity, VContinuity
  //! in the directions of the U and V parameters
  //! -   the highest degree MaxDegU, MaxDegV which
  //! the polynomial defining the BSpline curve may
  //! have in the directions of the U and V parameters
  //! -   the maximum number of segments MaxSegments
  //! allowed in the resulting BSpline curve
  //! -   the index of precision PrecisCode.
  Standard_EXPORT GeomConvert_ApproxSurface(const occ::handle<Adaptor3d_Surface>& Surf,
                                            const double                          Tol3d,
                                            const GeomAbs_Shape                   UContinuity,
                                            const GeomAbs_Shape                   VContinuity,
                                            const int                             MaxDegU,
                                            const int                             MaxDegV,
                                            const int                             MaxSegments,
                                            const int                             PrecisCode);

  //! Returns the BSpline surface resulting from the approximation algorithm.
  Standard_EXPORT occ::handle<Geom_BSplineSurface> Surface() const;

  //! Returns true if the approximation has be done
  Standard_EXPORT bool IsDone() const;

  //! Returns true if the approximation did come out with a result that
  //! is not NECESSARILY within the required tolerance or a result
  //! that is not recognized with the wished continuities.
  Standard_EXPORT bool HasResult() const;

  //! Returns the greatest distance between a point on the
  //! source conic surface and the BSpline surface
  //! resulting from the approximation (>0 when an approximation
  //! has been done, 0 if no  approximation )
  Standard_EXPORT double MaxError() const;

  //! Prints on the stream o information on the current state of the object.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  //! Converts a surface to B-spline
  Standard_EXPORT void Approximate(const occ::handle<Adaptor3d_Surface>& theSurf,
                                   const double                          theTol3d,
                                   const GeomAbs_Shape                   theUContinuity,
                                   const GeomAbs_Shape                   theVContinuity,
                                   const int                             theMaxDegU,
                                   const int                             theMaxDegV,
                                   const int                             theMaxSegments,
                                   const int                             thePrecisCode);

  bool                             myIsDone;
  bool                             myHasResult;
  occ::handle<Geom_BSplineSurface> myBSplSurf;
  double                           myMaxError;
};

#endif // _GeomConvert_ApproxSurface_HeaderFile
