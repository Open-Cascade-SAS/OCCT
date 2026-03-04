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

#ifndef GeomBndLib_Curve2d_HeaderFile
#define GeomBndLib_Curve2d_HeaderFile

#include <GeomBndLib_BezierCurve2d.hxx>
#include <GeomBndLib_BSplineCurve2d.hxx>
#include <GeomBndLib_Circle2d.hxx>
#include <GeomBndLib_Ellipse2d.hxx>
#include <GeomBndLib_Hyperbola2d.hxx>
#include <GeomBndLib_Line2d.hxx>
#include <GeomBndLib_OffsetCurve2d.hxx>
#include <GeomBndLib_OtherCurve2d.hxx>
#include <GeomBndLib_Parabola2d.hxx>
#include <Bnd_Box2d.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

class Adaptor2d_Curve2d;
class Geom2d_Curve;

//! Variant-based dispatcher for 2D curve bounding box computation.
//! Auto-detects the curve type and delegates to the appropriate specialized class.
class GeomBndLib_Curve2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct from an adaptor curve.
  Standard_EXPORT GeomBndLib_Curve2d(const Adaptor2d_Curve2d& theCurve);

  //! Construct from a Geom2d_Curve handle.
  Standard_EXPORT GeomBndLib_Curve2d(const occ::handle<Geom2d_Curve>& theCurve);

  GeomBndLib_Curve2d(const GeomBndLib_Curve2d&)            = delete;
  GeomBndLib_Curve2d& operator=(const GeomBndLib_Curve2d&) = delete;
  GeomBndLib_Curve2d(GeomBndLib_Curve2d&&)                 = delete;
  GeomBndLib_Curve2d& operator=(GeomBndLib_Curve2d&&)      = delete;

  //! Return detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

  //! Add bounding box for full curve.
  Standard_EXPORT void Add(double theTol, Bnd_Box2d& theBox) const;

  //! Add bounding box for arc [theU1, theU2].
  Standard_EXPORT void Add(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const;

  //! Add precise bounding box for full curve.
  Standard_EXPORT void AddOptimal(double theTol, Bnd_Box2d& theBox) const;

  //! Add precise bounding box for arc [theU1, theU2].
  Standard_EXPORT void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const;

private:
  using EvaluatorVariant = std::variant<std::monostate,
                                        GeomBndLib_Line2d,
                                        GeomBndLib_Circle2d,
                                        GeomBndLib_Ellipse2d,
                                        GeomBndLib_Hyperbola2d,
                                        GeomBndLib_Parabola2d,
                                        GeomBndLib_BezierCurve2d,
                                        GeomBndLib_BSplineCurve2d,
                                        GeomBndLib_OffsetCurve2d,
                                        GeomBndLib_OtherCurve2d>;
  const Adaptor2d_Curve2d* adaptor() const;
  double effectiveU1() const;
  double effectiveU2() const;

private:
  const Adaptor2d_Curve2d* myAdaptorRef = nullptr;
  occ::handle<Adaptor2d_Curve2d> myAdaptorOwned;
  EvaluatorVariant  myEvaluator;
  GeomAbs_CurveType myCurveType;
};

#endif // GeomBndLib_Curve2d_HeaderFile
