// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _Approx_BSplineApproxInterp_HeaderFile
#define _Approx_BSplineApproxInterp_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DynamicArray.hxx>
#include <gp_Pnt.hxx>
#include <math_Matrix.hxx>

class GeomAdaptor_Curve;

//! Constrained least-squares B-spline curve approximation with exact interpolation constraints.
//!
//! This class fits a B-spline curve through a set of 3D points, where each point
//! can be either approximated (in the least-squares sense) or exactly interpolated.
//! Selected interpolation points can additionally be marked as "kinks" - the solver
//! then inserts high-multiplicity knots at the corresponding parameters to preserve
//! C0 discontinuities.
//!
//! The algorithm solves a KKT (Karush-Kuhn-Tucker) saddle-point system:
//! @code
//!   | A^T*A  C^T  L^T |   | x |   | A^T*b |
//!   | C       0    0  | * | l | = |   d   |
//!   | L       0    0  |   | m |   |   0   |
//! @endcode
//! where:
//! - A is the basis matrix for approximated points
//! - C is the basis matrix for interpolated points
//! - L encodes continuity constraints (C1/C2 for closed curves)
//! - x are the control point coordinates
//! - l, m are Lagrange multipliers
//!
//! Usage:
//! @code
//!   NCollection_Array1<gp_Pnt> aPts(1, 100);
//!   // ... fill points ...
//!   Approx_BSplineApproxInterp anApprox(aPts, 20);
//!   anApprox.InterpolatePoint(0);        // first point exact
//!   anApprox.InterpolatePoint(99);       // last point exact
//!   anApprox.InterpolatePoint(50, true); // kink at midpoint
//!   anApprox.Perform(aParams);
//!   if (anApprox.IsDone())
//!   {
//!     const occ::handle<Geom_BSplineCurve>& aCurve = anApprox.Curve();
//!     double aMaxErr = anApprox.MaxError();
//!   }
//! @endcode
class Approx_BSplineApproxInterp
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a constrained approximation solver.
  //! @param[in] thePoints        array of 3D points to fit (1-based indexing)
  //! @param[in] theNbControlPts  desired number of control points for the B-spline
  //! @param[in] theDegree        degree of the B-spline (default 3)
  //! @param[in] theContinuousIfClosed  if true, enforces C2 continuity for closed curves
  Standard_EXPORT Approx_BSplineApproxInterp(const NCollection_Array1<gp_Pnt>& thePoints,
                                             int                               theNbControlPts,
                                             int                               theDegree = 3,
                                             bool theContinuousIfClosed                  = false);

  //! Marks a point to be exactly interpolated rather than approximated.
  //! @param[in] thePointIndex  0-based index of the point
  //! @param[in] theWithKink    if true, a kink (C0 break) is inserted at this parameter
  Standard_EXPORT void InterpolatePoint(int thePointIndex, bool theWithKink = false);

  //! Performs the fit using automatically computed parameters.
  //! Parameters are computed from input points using current parametrization alpha.
  Standard_EXPORT void Perform();

  //! Performs the fit with given parameters.
  //! @param[in] theParams  parameter values for each point (size must match point count)
  Standard_EXPORT void Perform(const NCollection_Array1<double>& theParams);

  //! Performs the fit with iterative parameter optimization using automatically
  //! computed initial parameters.
  //! @param[in] theMaxIter  maximum number of optimization iterations
  Standard_EXPORT void PerformOptimal(int theMaxIter);

  //! Performs the fit with iterative parameter optimization.
  //! Parameters of approximated points are re-projected onto the curve
  //! after each iteration to improve the fit.
  //! @param[in] theParams   initial parameter values
  //! @param[in] theMaxIter  maximum number of optimization iterations
  Standard_EXPORT void PerformOptimal(const NCollection_Array1<double>& theParams, int theMaxIter);

  //! Returns true if the fit was successfully computed.
  [[nodiscard]] bool IsDone() const { return myIsDone; }

  //! Returns the resulting B-spline curve.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineCurve>& Curve() const;

  //! Returns the maximum approximation error (distance at approximated points).
  [[nodiscard]] double MaxError() const { return myMaxError; }

  //! Sets the parametrization power for automatic parameter computation.
  //! 0.0 = uniform, 0.5 = centripetal (default), 1.0 = chord-length.
  //! @param[in] theAlpha  parametrization exponent in [0, 1]
  void SetParametrizationAlpha(double theAlpha) { myAlpha = theAlpha; }

  //! Sets the minimum pivot value for the Gauss solver.
  //! Matrices with pivots below this threshold are treated as singular.
  //! @param[in] theMinPivot  minimum pivot threshold (default 1e-20)
  void SetMinPivot(double theMinPivot) { myMinPivot = theMinPivot; }

  //! Sets the relative tolerance for detecting closed curves.
  //! Closedness is detected when first/last points are within
  //! theRelTol * (bounding box diagonal).
  //! @param[in] theRelTol  relative tolerance (default 1e-12)
  void SetClosedTolerance(double theRelTol) { myClosedRelTol = theRelTol; }

  //! Sets the tolerance for detecting duplicate knot positions during insertion.
  //! @param[in] theTol  knot matching tolerance (default 1e-4)
  void SetKnotInsertionTolerance(double theTol) { myKnotInsertTol = theTol; }

  //! Sets the convergence tolerance for parameter optimization.
  //! Optimization stops when relative error reduction falls below this value.
  //! @param[in] theTol  convergence tolerance (default 1e-3)
  void SetConvergenceTolerance(double theTol) { myConvergenceTol = theTol; }

  //! Sets the tolerance for point projection onto curve during optimization.
  //! @param[in] theTol  projection accuracy (default 1e-6)
  void SetProjectionTolerance(double theTol) { myProjectionTol = theTol; }

private:
  //! Computes centripetal/chord-length parameters from point distances.
  //! @param[in] theAlpha  parametrization power
  //! @return array of parameters in [0, 1]
  NCollection_Array1<double> computeParameters(double theAlpha) const;

  //! Computes knot vector from parameters and number of control points.
  //! Inserts high-multiplicity knots at kink parameters.
  //! @param[in]  theNbCP    number of control points
  //! @param[in]  theParams  parameter values
  //! @param[out] theKnots   computed knot values
  //! @param[out] theMults   computed knot multiplicities
  void computeKnots(int                               theNbCP,
                    const NCollection_Array1<double>& theParams,
                    NCollection_Array1<double>&       theKnots,
                    NCollection_Array1<int>&          theMults) const;

  //! Solves the constrained least-squares system.
  //! @param[in] theParams    parameter values
  //! @param[in] theKnots     knot values
  //! @param[in] theMults     knot multiplicities
  //! @return true if the system was solved successfully
  bool solve(const NCollection_Array1<double>& theParams,
             const NCollection_Array1<double>& theKnots,
             const NCollection_Array1<int>&    theMults);

  //! Builds the B-spline basis matrix for given parameters and flat knots.
  //! @param[in] theFlatKnots    flat knot vector
  //! @param[in] theParams       parameter values (1-based)
  //! @param[in] theDerivOrder   derivative order (0 = values, 1 = first deriv, etc.)
  //! @return matrix of size (theParams.Length() x nControlPoints)
  math_Matrix buildBasisMatrix(const NCollection_Array1<double>& theFlatKnots,
                               const NCollection_Array1<double>& theParams,
                               int                               theDerivOrder = 0) const;

  //! Builds the continuity constraint matrix for closed curve C1/C2 conditions.
  //! @param[in] theNbCtrPnts    number of control points
  //! @param[in] theNbContinuity number of continuity conditions
  //! @param[in] theParams       parameter values
  //! @param[in] theFlatKnots    flat knot vector
  //! @return continuity matrix (theNbContinuity x theNbCtrPnts)
  math_Matrix buildContinuityMatrix(int                               theNbCtrPnts,
                                    int                               theNbContinuity,
                                    const NCollection_Array1<double>& theParams,
                                    const NCollection_Array1<double>& theFlatKnots) const;

  //! Re-projects approximated points onto the curve to optimize parameters.
  //! @param[in]     theCurve   current fitted curve
  //! @param[in,out] theParams  parameters to optimize
  void optimizeParameters(const occ::handle<Geom_BSplineCurve>& theCurve,
                          NCollection_Array1<double>&           theParams) const;

  //! Projects a point onto a curve using Newton iteration.
  //! @param[in] thePnt       point to project
  //! @param[in] theCurveAdaptor curve adaptor to project onto
  //! @param[in] theInitParam initial parameter guess
  //! @param[out] theParam    optimized parameter
  //! @return projection distance
  double projectOnCurve(const gp_Pnt&            thePnt,
                        const GeomAdaptor_Curve& theCurveAdaptor,
                        double                   theInitParam,
                        double&                  theParam) const;

  //! Returns cached adaptor for the given curve, reloading cache when curve changes.
  const GeomAdaptor_Curve& curveAdaptor(const occ::handle<Geom_BSplineCurve>& theCurve) const;

  //! Returns true if the point set represents a closed curve.
  bool isClosed() const;

  //! Returns true if the first and last points are in the interpolated set.
  bool isFirstAndLastInterpolated() const;

  //! Computes the diagonal of the bounding box of the points.
  double boundingBoxDiagonal() const;

  NCollection_Array1<gp_Pnt>             myPoints;
  NCollection_DynamicArray<int>          myInterpolated;
  NCollection_DynamicArray<int>          myApproximated;
  NCollection_DynamicArray<int>          myKinks;
  occ::handle<Geom_BSplineCurve>         myCurve;
  mutable occ::handle<GeomAdaptor_Curve> myCurveAdaptorCache;
  int                                    myDegree             = 3;
  int                                    myNbControlPts       = 0;
  double                                 myMaxError           = 0.0;
  double                                 myAlpha              = 0.5;
  double                                 myMinPivot           = 1.0e-20;
  double                                 myClosedRelTol       = 1.0e-12;
  double                                 myKnotInsertTol      = 1.0e-4;
  double                                 myConvergenceTol     = 1.0e-3;
  double                                 myProjectionTol      = 1.0e-6;
  bool                                   myContinuousIfClosed = false;
  bool                                   myIsDone             = false;
};

#endif // _Approx_BSplineApproxInterp_HeaderFile
