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

#ifndef GeomBndLib_Curve_HeaderFile
#define GeomBndLib_Curve_HeaderFile

#include <GeomBndLib_BezierCurve.hxx>
#include <GeomBndLib_BSplineCurve.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_Ellipse.hxx>
#include <GeomBndLib_Hyperbola.hxx>
#include <GeomBndLib_Line.hxx>
#include <GeomBndLib_OffsetCurve.hxx>
#include <GeomBndLib_OtherCurve.hxx>
#include <GeomBndLib_Parabola.hxx>
#include <Bnd_Box.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

class Adaptor3d_Curve;
class Geom_Curve;

//! Variant-based dispatcher for 3D curve bounding box computation.
//! Auto-detects the curve type and delegates to the appropriate specialized class.
class GeomBndLib_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct from an adaptor curve.
  Standard_EXPORT GeomBndLib_Curve(const Adaptor3d_Curve& theCurve);

  //! Construct from a Geom_Curve handle.
  Standard_EXPORT GeomBndLib_Curve(const occ::handle<Geom_Curve>& theCurve);

  GeomBndLib_Curve(const GeomBndLib_Curve&)            = delete;
  GeomBndLib_Curve& operator=(const GeomBndLib_Curve&) = delete;
  GeomBndLib_Curve(GeomBndLib_Curve&&)                 = delete;
  GeomBndLib_Curve& operator=(GeomBndLib_Curve&&)      = delete;

  //! Return detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

  //! Compute bounding box for full curve.
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theTol) const;

  //! Compute bounding box for arc [theU1, theU2].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theU1, double theU2, double theTol) const;

  //! Compute precise bounding box for full curve.
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double theTol) const;

  //! Compute precise bounding box for arc [theU1, theU2].
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double theU1, double theU2, double theTol) const;

  //! Add bounding box for full curve.
  Standard_EXPORT void Add(double theTol, Bnd_Box& theBox) const;

  //! Add bounding box for arc [theU1, theU2].
  Standard_EXPORT void Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const;

  //! Add precise bounding box for full curve.
  Standard_EXPORT void AddOptimal(double theTol, Bnd_Box& theBox) const;

  //! Add precise bounding box for arc [theU1, theU2].
  Standard_EXPORT void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const;

private:
  using EvaluatorVariant = std::variant<std::monostate,
                                        GeomBndLib_Line,
                                        GeomBndLib_Circle,
                                        GeomBndLib_Ellipse,
                                        GeomBndLib_Hyperbola,
                                        GeomBndLib_Parabola,
                                        GeomBndLib_BezierCurve,
                                        GeomBndLib_BSplineCurve,
                                        GeomBndLib_OffsetCurve,
                                        GeomBndLib_OtherCurve>;
  const Adaptor3d_Curve* adaptor() const;
  double                 effectiveU1() const;
  double                 effectiveU2() const;

private:
  const Adaptor3d_Curve*       myAdaptorRef = nullptr;
  occ::handle<Adaptor3d_Curve> myAdaptorOwned;
  EvaluatorVariant             myEvaluator;
  GeomAbs_CurveType            myCurveType;
};

#endif // GeomBndLib_Curve_HeaderFile
