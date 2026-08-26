// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _ExtremaPC_Planar_HeaderFile
#define _ExtremaPC_Planar_HeaderFile

#include <ExtremaPC.hxx>
#include <ExtremaPC2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>

#include <optional>

namespace ExtremaPC
{

//! Converts the 3D search mode to its 2D counterpart without relying on enum ordinals.
//! @param[in] theMode 3D search mode
//! @return corresponding 2D search mode; MinMax for an invalid enumeration value
constexpr ExtremaPC2d::SearchMode ToSearchMode2d(ExtremaPC::SearchMode theMode)
{
  switch (theMode)
  {
    case ExtremaPC::SearchMode::Min:
      return ExtremaPC2d::SearchMode::Min;
    case ExtremaPC::SearchMode::Max:
      return ExtremaPC2d::SearchMode::Max;
    case ExtremaPC::SearchMode::MinMax:
      return ExtremaPC2d::SearchMode::MinMax;
  }
  return ExtremaPC2d::SearchMode::MinMax;
}

//! Builds a deterministic plane containing the line.
//! The plane X direction is the line direction, preserving the line parameter.
//! @param[in] theLine source line
//! @return containing plane
Standard_EXPORT gp_Pln LinePlane(const gp_Lin& theLine);

//! Projects a planar Bezier curve to 2D without changing its parameterization.
//! Curves with evaluation representations and non-planar control polygons are rejected.
//! @param[in] theCurve source curve
//! @param[out] thePlane plane used as the 2D coordinate system
//! @param[out] theCurve2d projected curve
//! @return true if an exact supported projection was built
Standard_EXPORT bool ProjectBezier(const occ::handle<Geom_BezierCurve>& theCurve,
                                   gp_Pln&                              thePlane,
                                   occ::handle<Geom2d_BezierCurve>&     theCurve2d);

//! Projects a planar BSpline curve to 2D without changing its parameterization.
//! Curves with evaluation representations, non-planar control polygons, or a rational
//! representation that cannot be retained by Geom2d_BSplineCurve are rejected.
//! @param[in] theCurve source curve
//! @param[out] thePlane plane used as the 2D coordinate system
//! @param[out] theCurve2d projected curve
//! @return true if an exact supported projection was built
Standard_EXPORT bool ProjectBSpline(const occ::handle<Geom_BSplineCurve>& theCurve,
                                    gp_Pln&                               thePlane,
                                    occ::handle<Geom2d_BSplineCurve>&     theCurve2d);

//! Projects a planar 3D offset curve to its parameter-preserving 2D representation.
//! The basis curve must have an exact supported projection and the offset direction
//! must be normal to that plane.
//! @param[in] theCurve source offset curve
//! @param[out] thePlane plane used as the 2D coordinate system
//! @param[out] theCurve2d projected offset curve
//! @return true if an exact supported projection was built
Standard_EXPORT bool ProjectOffset(const occ::handle<Geom_OffsetCurve>& theCurve,
                                   gp_Pln&                              thePlane,
                                   occ::handle<Geom2d_OffsetCurve>&     theCurve2d);

//! Converts a 2D result to the corresponding 3D result.
//! Points and distances are evaluated on the original 3D evaluator at unchanged parameters.
//! @tparam CurveEvaluator type providing Value(double)
//! @param[in] theResult2d source 2D result
//! @param[in] theQuery original 3D query point
//! @param[in] thePlane projection plane
//! @param[in] theEvaluator original 3D evaluator
//! @param[out] theResult converted result
//! @return false when the 2D result should be ignored in favor of the 3D implementation
template <typename CurveEvaluator>
bool ConvertPlanarResult(const ExtremaPC2d::Result& theResult2d,
                         const gp_Pnt&              theQuery,
                         const gp_Pln&              thePlane,
                         const CurveEvaluator&      theEvaluator,
                         ExtremaPC::Result&         theResult)
{
  if (theResult2d.Status == ExtremaPC2d::Status::NotDone
      || theResult2d.Status == ExtremaPC2d::Status::NumericalError)
  {
    return false;
  }

  theResult.Clear();
  switch (theResult2d.Status)
  {
    case ExtremaPC2d::Status::OK:
      theResult.Status = ExtremaPC::Status::OK;
      break;
    case ExtremaPC2d::Status::InfiniteSolutions:
      theResult.Status = ExtremaPC::Status::InfiniteSolutions;
      break;
    case ExtremaPC2d::Status::NoSolution:
      theResult.Status = ExtremaPC::Status::NoSolution;
      break;
    case ExtremaPC2d::Status::InvalidInput:
      theResult.Status = ExtremaPC::Status::InvalidInput;
      break;
    case ExtremaPC2d::Status::NotDone:
    case ExtremaPC2d::Status::NumericalError:
      return false;
  }

  if (theResult2d.IsInfinite())
  {
    const double aHeight             = thePlane.Distance(theQuery);
    theResult.InfiniteSquareDistance = theResult2d.InfiniteSquareDistance + aHeight * aHeight;
    return true;
  }

  for (size_t anIndex = 0; anIndex < theResult2d.NbExt(); ++anIndex)
  {
    const double aParameter  = theResult2d[anIndex].Parameter;
    const gp_Pnt aCurvePoint = theEvaluator.Value(aParameter);
    theResult.Extrema.Append(ExtremaPC::ExtremumResult{aParameter,
                                                       aCurvePoint,
                                                       theQuery.SquareDistance(aCurvePoint),
                                                       theResult2d[anIndex].IsMinimum,
                                                       theResult2d[anIndex].IsMaximum});
  }
  return true;
}

//! Runs a parameter-preserving 2D evaluator and converts its result to 3D.
//! @tparam Evaluator2d 2D extrema evaluator type
//! @tparam Geometry2d 2D geometry type accepted by Evaluator2d
//! @tparam CurveEvaluator original 3D evaluator type
//! @param[in] theGeometry projected 2D geometry
//! @param[in] theDomain optional parameter domain
//! @param[in] theQuery2d projected query point
//! @param[in] theQuery3d original query point
//! @param[in] thePlane projection plane
//! @param[in] theEvaluator original 3D evaluator
//! @param[in] theTolerance geometric tolerance
//! @param[in] theMode search mode
//! @param[in] theIncludeEndpoints whether endpoints should be included
//! @param[out] theResult converted result
//! @return false when the 3D implementation should be used instead
template <typename Evaluator2d, typename Geometry2d, typename CurveEvaluator>
bool PerformPlanar(const Geometry2d&                         theGeometry,
                   const std::optional<ExtremaPC::Domain1D>& theDomain,
                   const gp_Pnt2d&                           theQuery2d,
                   const gp_Pnt&                             theQuery3d,
                   const gp_Pln&                             thePlane,
                   const CurveEvaluator&                     theEvaluator,
                   double                                    theTolerance,
                   ExtremaPC::SearchMode                     theMode,
                   bool                                      theIncludeEndpoints,
                   ExtremaPC::Result&                        theResult)
{
  const ExtremaPC2d::SearchMode aMode = ExtremaPC::ToSearchMode2d(theMode);
  if (theDomain.has_value())
  {
    Evaluator2d                anEvaluator2d(theGeometry, theDomain.value());
    const ExtremaPC2d::Result& aResult2d =
      theIncludeEndpoints ? anEvaluator2d.PerformWithEndpoints(theQuery2d, theTolerance, aMode)
                          : anEvaluator2d.Perform(theQuery2d, theTolerance, aMode);
    return ConvertPlanarResult(aResult2d, theQuery3d, thePlane, theEvaluator, theResult);
  }

  Evaluator2d                anEvaluator2d(theGeometry);
  const ExtremaPC2d::Result& aResult2d =
    theIncludeEndpoints ? anEvaluator2d.PerformWithEndpoints(theQuery2d, theTolerance, aMode)
                        : anEvaluator2d.Perform(theQuery2d, theTolerance, aMode);
  return ConvertPlanarResult(aResult2d, theQuery3d, thePlane, theEvaluator, theResult);
}

} // namespace ExtremaPC

#endif // _ExtremaPC_Planar_HeaderFile
