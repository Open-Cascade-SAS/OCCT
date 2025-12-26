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

#ifndef _ExtremaPC_Curve_HeaderFile
#define _ExtremaPC_Curve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_BezierCurve.hxx>
#include <ExtremaPC_BSplineCurve.hxx>
#include <ExtremaPC_Circle.hxx>
#include <ExtremaPC_Ellipse.hxx>
#include <ExtremaPC_Hyperbola.hxx>
#include <ExtremaPC_Line.hxx>
#include <ExtremaPC_OffsetCurve.hxx>
#include <ExtremaPC_OtherCurve.hxx>
#include <ExtremaPC_Parabola.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Main aggregator for Point-Curve extrema computation.
//!
//! Provides a unified interface for computing extrema between a 3D point
//! and any type of curve. Uses std::variant with std::visit for efficient
//! curve type dispatch at runtime.
//!
//! Supports all elementary curve types (analytical solutions):
//! - Line, Circle, Ellipse, Hyperbola, Parabola
//!
//! And numerical curves (grid-based with Newton refinement):
//! - BSpline, Bezier, Offset, and general curves
//!
//! Usage example:
//! @code
//! ExtremaPC_Curve anExtPC;
//! anExtPC.Initialize(myAdaptorCurve);
//! ExtremaPC::Result aResult = anExtPC.Perform(myPoint);
//! if (aResult.IsDone())
//! {
//!   for (int i = 0; i < aResult.NbExt(); ++i)
//!   {
//!     double aDist = std::sqrt(aResult[i].SquareDistance);
//!     gp_Pnt aPtOnCurve = aResult[i].Point;
//!   }
//! }
//! @endcode
class ExtremaPC_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all specialized evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        ExtremaPC_Line,
                                        ExtremaPC_Circle,
                                        ExtremaPC_Ellipse,
                                        ExtremaPC_Hyperbola,
                                        ExtremaPC_Parabola,
                                        ExtremaPC_BezierCurve,
                                        ExtremaPC_BSplineCurve,
                                        ExtremaPC_OffsetCurve,
                                        ExtremaPC_OtherCurve>;

  //! Default constructor - creates uninitialized evaluator.
  Standard_EXPORT ExtremaPC_Curve();

  //! Constructor with curve adaptor.
  //! @param theCurve curve adaptor
  Standard_EXPORT explicit ExtremaPC_Curve(const Adaptor3d_Curve& theCurve);

  //! Constructor with curve adaptor and parameter range.
  //! @param theCurve curve adaptor
  //! @param theUMin lower parameter bound
  //! @param theUMax upper parameter bound
  Standard_EXPORT ExtremaPC_Curve(const Adaptor3d_Curve& theCurve, double theUMin, double theUMax);

  //! Initializes the evaluator with a curve adaptor.
  //! Uses the curve's natural parameter bounds.
  //! @param theCurve curve adaptor
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theCurve);

  //! Initializes the evaluator with a curve adaptor and parameter range.
  //! @param theCurve curve adaptor
  //! @param theUMin lower parameter bound
  //! @param theUMax upper parameter bound
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theCurve, double theUMin, double theUMax);

  //! Sets the tolerance for extrema computation.
  //! @param theTol tolerance value
  void SetTolerance(double theTol) { myConfig.Tolerance = theTol; }

  //! Sets the number of samples for numerical methods.
  //! @param theNb number of samples
  void SetNbSamples(int theNb) { myConfig.NbSamples = theNb; }

  //! Sets the search mode.
  //! @param theMode search mode (MinMax, Min, or Max)
  void SetSearchMode(ExtremaPC::SearchMode theMode) { myConfig.Mode = theMode; }

  //! Returns the current search mode.
  ExtremaPC::SearchMode SearchMode() const { return myConfig.Mode; }

  //! Sets whether to include endpoints as extrema.
  //! @param theFlag true to include endpoints
  void SetIncludeEndpoints(bool theFlag) { myConfig.IncludeEndpoints = theFlag; }

  //! Returns whether endpoints are included as extrema.
  bool IncludeEndpoints() const { return myConfig.IncludeEndpoints; }

  //! Computes extrema between point P and the curve.
  //! @param theP query point
  //! @return result containing all found extrema
  Standard_EXPORT ExtremaPC::Result Perform(const gp_Pnt& theP) const;

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return !std::holds_alternative<std::monostate>(myEvaluator); }

  //! Returns the curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

  //! Returns the configuration.
  const ExtremaPC::Config& Config() const { return myConfig; }

private:
  EvaluatorVariant  myEvaluator; //!< Specialized evaluator
  ExtremaPC::Config myConfig;    //!< Computation configuration
  GeomAbs_CurveType myCurveType; //!< Curve type
};

#endif // _ExtremaPC_Curve_HeaderFile
