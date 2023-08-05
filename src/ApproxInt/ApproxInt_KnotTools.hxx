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

#ifndef _ApproxInt_KnotTools_HeaderFile
#define _ApproxInt_KnotTools_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_List.hxx>
#include <math_Vector.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Vector.hxx>
#include <Approx_ParametrizationType.hxx>

class IntPatch_WLine;

// Corresponds for debug information output.
// Debug information is also printed when OCCT_DEBUG defined.
//#define APPROXINT_KNOTTOOLS_DEBUG

//! This class intended to build knots sequence on discrete set of points for further approximation into bspline curve.
//!
//! Short description of algorithm:
//! 1) Build discrete curvature on points set.
//! 2) According to special rules build draft knots sequence.
//! 3) Filter draft sequence to build output sequence.
//!
//! For more details look at:
//! Anshuman Razdan - Knot Placement for B-Spline curve Approximation.
class ApproxInt_KnotTools
{
public:

  DEFINE_STANDARD_ALLOC

  //! Main function to build optimal knot sequence.
  //! At least one set from (thePntsXYZ, thePntsU1V1, thePntsU2V2) should exist.
  //! @param thePntsXYZ - Set of 3d points.
  //! @param thePntsU1V1 - Set of 2d points.
  //! @param thePntsU2V2 - Set of 2d points.
  //! @param thePars - Expected parameters associated with set.
  //! @param theApproxXYZ - Flag, existence of 3d set.
  //! @param theApproxU1V1 - Flag existence of first 2d set.
  //! @param theApproxU2V2 - Flag existence of second 2d set.
  //! @param theMinNbPnts - Minimal number of points per knot interval.
  //! @param theKnots - output knots sequence.
  Standard_EXPORT static void BuildKnots(const TColgp_Array1OfPnt& thePntsXYZ,
                                         const TColgp_Array1OfPnt2d& thePntsU1V1,
                                         const TColgp_Array1OfPnt2d& thePntsU2V2,
                                         const math_Vector& thePars,
                                         const Standard_Boolean theApproxXYZ,
                                         const Standard_Boolean theApproxU1V1,
                                         const Standard_Boolean theApproxU2V2,
                                         const Standard_Integer theMinNbPnts,
                                         NCollection_Vector<Standard_Integer>& theKnots);

  //! Builds discrete curvature
  Standard_EXPORT static void BuildCurvature(
    const NCollection_LocalArray<Standard_Real>& theCoords,
    const Standard_Integer theDim,
    const math_Vector& thePars,
    TColStd_Array1OfReal& theCurv,
    Standard_Real& theMaxCurv);

  //! Defines preferable parametrization type for theWL 
  Standard_EXPORT static Approx_ParametrizationType DefineParType(const Handle(IntPatch_WLine)& theWL,
    const Standard_Integer theFpar, const Standard_Integer theLpar,
    const Standard_Boolean theApproxXYZ,
    const Standard_Boolean theApproxU1V1,
    const Standard_Boolean theApproxU2V2);


private:

  //! Compute indices of knots:
  //!
  //! I: Build discrete curvature in points set,
  //! using outer product of two vectors.
  //!
  //! II: Put knots in points which has extremity on discrete curvature.
  //!
  //! III: Put knots in monotone intervals of curvature.
  //!
  //! IV: Put additional knots near extrema points.
  static void ComputeKnotInds(const NCollection_LocalArray<Standard_Real>& theCoords,
                              const Standard_Integer theDim,
                              const math_Vector& thePars,
                              NCollection_Sequence<Standard_Integer>& theInds);

  //! Insert knots before index I.
  //!
  //! I: Check curvature change:
  //! if ( maxCurvature / minCurvature ) of current interval greater than 
  //! threshold value, then stop and use upper index as knot.
  //!
  //! II: Check midpoint criteria:
  //! If exist point between two knot indices with angle greater than
  //! threshold value, then stop and put this index as knot.
  static Standard_Boolean InsKnotBefI(const Standard_Integer theI,
                                      const TColStd_Array1OfReal& theCurv,
                                      const NCollection_LocalArray<Standard_Real>& theCoords,
                                      const Standard_Integer theDim, 
                                      NCollection_Sequence<Standard_Integer>& theInds,
                                      const Standard_Boolean ChkCurv);

  //! Perform knots filtration.
  //!
  //! I: Filter too big number of points per knot interval.
  //!
  //! II: Filter points with too small amount of points per knot interval.
  //!
  //! III: Fill Last Knot.
  static void FilterKnots(NCollection_Sequence<Standard_Integer>& theInds, 
                          const Standard_Integer theMinNbPnts,
                          NCollection_Vector<Standard_Integer>& theLKnots);
};

#endif
