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

#ifndef _GeomProp_Circle_HeaderFile
#define _GeomProp_Circle_HeaderFile

#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Local differential properties for a 3D circle.
//!
//! A circle has constant curvature = 1/R, well-defined tangent and normal
//! at every point, and no curvature extrema or inflection points.
//!
//! Can be constructed from either a GeomAdaptor_Curve pointer or a occ::handle<Geom_Curve>.
//! When constructed from a handle, no adaptor is created.
class GeomProp_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must wrap a circle, must not be null)
  GeomProp_Circle(const GeomAdaptor_Curve*  theAdaptor,
                  GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Undefined)
      : myAdaptor(theAdaptor),
        myRadius(theAdaptor->Circle().Radius()),
        myCenter(theAdaptor->Circle().Location())
  {
    (void)theOrder;
  }

  //! Constructor from geometry handle.
  //! @param theCurve the 3D circle geometry (must be a Geom_Circle or downcastable to it)
  //! @param theDomain optional parameter domain (unused for circle)
  GeomProp_Circle(const occ::handle<Geom_Curve>&              theCurve,
                  const std::optional<GeomProp::CurveDomain>& theDomain = std::nullopt,
                  GeomProp::CurveDerivOrder theOrder = GeomProp::CurveDerivOrder::Undefined)
      : myAdaptor(nullptr),
        myCurve(theCurve)
  {
    (void)theDomain;
    (void)theOrder;
    const occ::handle<Geom_Circle> aCircle = occ::down_cast<Geom_Circle>(theCurve);
    myRadius                               = aCircle->Radius();
    myCenter                               = aCircle->Circ().Location();
  }

  //! Non-copyable and non-movable.
  GeomProp_Circle(const GeomProp_Circle&)            = delete;
  GeomProp_Circle& operator=(const GeomProp_Circle&) = delete;
  GeomProp_Circle(GeomProp_Circle&&)                 = delete;
  GeomProp_Circle& operator=(GeomProp_Circle&&)      = delete;

  //! Sets the derivative caching order (no-op for analytical curves).
  void SetDerivOrder(GeomProp::CurveDerivOrder) {}

  //! Returns the derivative caching order (always Undefined for analytical curves).
  GeomProp::CurveDerivOrder DerivOrder() const { return GeomProp::CurveDerivOrder::Undefined; }

  //! Returns the adaptor pointer (nullptr when constructed from handle).
  const GeomAdaptor_Curve* Adaptor() const { return myAdaptor; }

  //! Returns pointer to underlying geometry, or nullptr if constructed from adaptor.
  const Geom_Curve* Geometry() const { return myCurve.get(); }

  //! Compute tangent at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol tolerance (unused for circle)
  //! @return tangent result (always defined)
  GeomProp::TangentResult Tangent(double theParam, double theTol) const
  {
    (void)theTol;
    gp_Pnt aPnt;
    gp_Vec aD1;
    if (!myCurve.IsNull())
    {
      myCurve->D1(theParam, aPnt, aD1);
    }
    else
    {
      myAdaptor->D1(theParam, aPnt, aD1);
    }
    return {gp_Dir(aD1), true};
  }

  //! Compute curvature at given parameter.
  //! For a circle, curvature = 1/R (constant).
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return curvature result (always defined, constant)
  GeomProp::CurvatureResult Curvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {1.0 / myRadius, true, false};
  }

  //! Compute normal at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol tolerance (unused)
  //! @return normal result (always defined)
  GeomProp::NormalResult Normal(double theParam, double theTol) const
  {
    (void)theTol;
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    if (!myCurve.IsNull())
    {
      myCurve->D2(theParam, aPnt, aD1, aD2);
    }
    else
    {
      myAdaptor->D2(theParam, aPnt, aD1, aD2);
    }
    // Normal = D2 * (D1.D1) - D1 * (D1.D2)
    const gp_Vec aNorm = aD2 * aD1.Dot(aD1) - aD1 * aD1.Dot(aD2);
    return {gp_Dir(aNorm), true};
  }

  //! Compute centre of curvature at given parameter.
  //! For a circle, the centre of curvature is the geometric centre.
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return centre result (always the circle centre)
  GeomProp::CentreResult CentreOfCurvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {myCenter, true};
  }

  //! Find curvature extrema on the circle.
  //! A circle has constant curvature, so no extrema.
  //! @return empty analysis (always done)
  GeomProp::CurveAnalysis FindCurvatureExtrema() const { return {{}, true}; }

  //! Find inflection points on the circle.
  //! A circle has no inflection points.
  //! @return empty analysis (always done)
  GeomProp::CurveAnalysis FindInflections() const { return {{}, true}; }

private:
  const GeomAdaptor_Curve* myAdaptor = nullptr; //!< Non-owning adaptor pointer (adaptor path)
  occ::handle<Geom_Curve>  myCurve;             //!< Geometry handle (handle path)
  double                   myRadius;            //!< Cached circle radius
  gp_Pnt                   myCenter;            //!< Cached circle centre
};

#endif // _GeomProp_Circle_HeaderFile
