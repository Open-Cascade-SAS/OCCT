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

#ifndef _Geom2dProp_Circle_HeaderFile
#define _Geom2dProp_Circle_HeaderFile

#include <Geom2d_Circle.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dProp.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 2D circle.
//!
//! A circle has constant curvature = 1/R, well-defined tangent and normal
//! at every point, and no curvature extrema or inflection points.
//!
//! Can be constructed from either a Geom2dAdaptor_Curve pointer or a occ::handle<Geom2d_Curve>.
//! When constructed from a handle, no adaptor is created.
class Geom2dProp_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 2D curve adaptor (must wrap a circle, must not be null)
  Geom2dProp_Circle(const Geom2dAdaptor_Curve*  theAdaptor,
                    Geom2dProp::CurveDerivOrder theOrder = Geom2dProp::CurveDerivOrder::Undefined)
      : myAdaptor(theAdaptor),
        myRadius(theAdaptor->Circle().Radius()),
        myCenter(theAdaptor->Circle().Location())
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 2D circle geometry (must be a Geom2d_Circle or downcastable to it)
  //! @param theDomain optional parameter domain (unused for circle)
  Geom2dProp_Circle(const occ::handle<Geom2d_Curve>&              theCurve,
                    const std::optional<Geom2dProp::CurveDomain>& theDomain = std::nullopt,
                    Geom2dProp::CurveDerivOrder                   theOrder  = Geom2dProp::CurveDerivOrder::Undefined)
      : myAdaptor(nullptr),
        myCurve(theCurve)
  {
    (void)theDomain;
    (void)theOrder;
    const occ::handle<Geom2d_Circle> aCircle = occ::down_cast<Geom2d_Circle>(theCurve);
    myRadius                                 = aCircle->Radius();
    myCenter                                 = aCircle->Circ2d().Location();
  }

  //! Non-copyable and non-movable.
  Geom2dProp_Circle(const Geom2dProp_Circle&)            = delete;
  Geom2dProp_Circle& operator=(const Geom2dProp_Circle&) = delete;
  Geom2dProp_Circle(Geom2dProp_Circle&&)                 = delete;
  Geom2dProp_Circle& operator=(Geom2dProp_Circle&&)      = delete;

  //! Sets the derivative caching order (no-op for analytical curves).
  void SetDerivOrder(Geom2dProp::CurveDerivOrder) {}

  //! Returns the derivative caching order (always Undefined for analytical curves).
  Geom2dProp::CurveDerivOrder DerivOrder() const { return Geom2dProp::CurveDerivOrder::Undefined; }

  //! Returns nullptr (no adaptor is stored).
  const Geom2dAdaptor_Curve* Adaptor() const { return nullptr; }

  //! Compute tangent at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol tolerance (unused for circle)
  //! @return tangent result (always defined)
  Geom2dProp::TangentResult Tangent(double theParam, double theTol) const
  {
    (void)theTol;
    gp_Pnt2d aPnt;
    gp_Vec2d aD1;
    if (!myCurve.IsNull())
    {
      myCurve->D1(theParam, aPnt, aD1);
    }
    else
    {
      myAdaptor->D1(theParam, aPnt, aD1);
    }
    return {gp_Dir2d(aD1), true};
  }

  //! Compute curvature at given parameter.
  //! For a circle, curvature = 1/R (constant).
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return curvature result (always defined, constant)
  Geom2dProp::CurvatureResult Curvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {1.0 / myRadius, true, false};
  }

  //! Compute normal at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol tolerance (unused)
  //! @return normal result (always defined)
  Geom2dProp::NormalResult Normal(double theParam, double theTol) const
  {
    (void)theTol;
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2;
    if (!myCurve.IsNull())
    {
      myCurve->D2(theParam, aPnt, aD1, aD2);
    }
    else
    {
      myAdaptor->D2(theParam, aPnt, aD1, aD2);
    }
    // Normal = D2 * (D1.D1) - D1 * (D1.D2)
    const gp_Vec2d aNorm = aD2 * aD1.Dot(aD1) - aD1 * aD1.Dot(aD2);
    return {gp_Dir2d(aNorm), true};
  }

  //! Compute centre of curvature at given parameter.
  //! For a circle, the centre of curvature is the geometric centre.
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return centre result (always the circle centre)
  Geom2dProp::CentreResult CentreOfCurvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {myCenter, true};
  }

  //! Find curvature extrema on the circle.
  //! A circle has constant curvature, so no extrema.
  //! @return empty analysis (always done)
  Geom2dProp::CurveAnalysis FindCurvatureExtrema() const { return {{}, true}; }

  //! Find inflection points on the circle.
  //! A circle has no inflection points.
  //! @return empty analysis (always done)
  Geom2dProp::CurveAnalysis FindInflections() const { return {{}, true}; }

private:
  const Geom2dAdaptor_Curve* myAdaptor = nullptr; //!< Non-owning adaptor pointer (adaptor path)
  occ::handle<Geom2d_Curve>  myCurve;             //!< Geometry handle (handle path)
  double                     myRadius;            //!< Cached circle radius
  gp_Pnt2d                   myCenter;            //!< Cached circle centre
};

#endif // _Geom2dProp_Circle_HeaderFile
