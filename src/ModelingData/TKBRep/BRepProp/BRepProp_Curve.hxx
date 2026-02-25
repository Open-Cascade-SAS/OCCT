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

#ifndef _BRepProp_Curve_HeaderFile
#define _BRepProp_Curve_HeaderFile

#include <BRepAdaptor_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomProp.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Edge;

//! @brief Local differential property evaluator for BRep edges.
//!
//! Thin wrapper over GeomProp:: free functions.
//! Delegates derivative computation to BRepAdaptor_Curve and passes
//! results to GeomProp::ComputeTangent, ComputeCurvature, etc.
//!
//! Can be initialized from a TopoDS_Edge or a BRepAdaptor_Curve.
//! When initialized from an existing BRepAdaptor_Curve, the adaptor is
//! referenced without copying (non-owning); when initialized from a
//! TopoDS_Edge, an internal adaptor is created and owned.
//!
//! Usage:
//! @code
//!   BRepProp_Curve aProp(myEdge);
//!   GeomProp::CurvatureResult aCurv = aProp.Curvature(0.5, Precision::Confusion());
//!   if (aCurv.IsDefined)
//!   {
//!     double aValue = aCurv.Value;
//!   }
//! @endcode
class BRepProp_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct from a TopoDS_Edge.
  //! Creates an internal BRepAdaptor_Curve (owning).
  //! @param[in] theEdge the edge to evaluate
  Standard_EXPORT BRepProp_Curve(const TopoDS_Edge& theEdge);

  //! Construct from an existing BRepAdaptor_Curve.
  //! The adaptor is referenced without copying (non-owning);
  //! the caller must ensure the adaptor outlives this object.
  //! @param[in] theCurve the adaptor to reference
  Standard_EXPORT BRepProp_Curve(const BRepAdaptor_Curve& theCurve);

  //! Construct from a handle to BRepAdaptor_Curve.
  //! Shares ownership of the adaptor (no copy).
  //! @param[in] theCurve handle to the adaptor
  Standard_EXPORT BRepProp_Curve(const occ::handle<BRepAdaptor_Curve>& theCurve);

  //! Non-copyable and non-movable.
  BRepProp_Curve(const BRepProp_Curve&)            = delete;
  BRepProp_Curve& operator=(const BRepProp_Curve&) = delete;
  BRepProp_Curve(BRepProp_Curve&&)                 = delete;
  BRepProp_Curve& operator=(BRepProp_Curve&&)      = delete;

  //! Returns the underlying adaptor.
  const BRepAdaptor_Curve& Adaptor() const { return *myPtr; }

  //! Compute tangent at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return tangent result with validity flag
  Standard_EXPORT GeomProp::TangentResult Tangent(double theParam, double theTol) const;

  //! Compute curvature at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return curvature result with validity and infinity flags
  Standard_EXPORT GeomProp::CurvatureResult Curvature(double theParam, double theTol) const;

  //! Compute normal at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return normal result with validity flag
  Standard_EXPORT GeomProp::NormalResult Normal(double theParam, double theTol) const;

  //! Compute centre of curvature at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return centre result with validity flag
  Standard_EXPORT GeomProp::CentreResult CentreOfCurvature(double theParam, double theTol) const;

  //! Computes the regularity at the junction between two curves.
  //! The point theU1 on theCurve1 and the point theU2 on theCurve2 must be coincident.
  //! @param[in] theCurve1 first curve adaptor
  //! @param[in] theCurve2 second curve adaptor
  //! @param[in] theU1 parameter on theCurve1
  //! @param[in] theU2 parameter on theCurve2
  //! @param[in] theTolLinear linear tolerance
  //! @param[in] theTolAngular angular tolerance
  //! @return the continuity order (C0, G1, C1, C2, CN)
  Standard_EXPORT static GeomAbs_Shape Continuity(const BRepAdaptor_Curve& theCurve1,
                                                  const BRepAdaptor_Curve& theCurve2,
                                                  double                   theU1,
                                                  double                   theU2,
                                                  double                   theTolLinear,
                                                  double                   theTolAngular);

  //! Same as above but using the standard tolerances from package Precision.
  Standard_EXPORT static GeomAbs_Shape Continuity(const BRepAdaptor_Curve& theCurve1,
                                                  const BRepAdaptor_Curve& theCurve2,
                                                  double                   theU1,
                                                  double                   theU2);

protected:
  //! Initialize from a TopoDS_Edge.
  //! @param[in] theEdge the edge to evaluate
  Standard_EXPORT void initialization(const TopoDS_Edge& theEdge);

  //! Initialize from an existing BRepAdaptor_Curve (non-owning).
  //! @param[in] theCurve the adaptor to reference
  Standard_EXPORT void initialization(const BRepAdaptor_Curve& theCurve);

  //! Initialize from a handle to BRepAdaptor_Curve.
  //! @param[in] theCurve handle to the adaptor
  Standard_EXPORT void initialization(const occ::handle<BRepAdaptor_Curve>& theCurve);

private:
  occ::handle<BRepAdaptor_Curve> myOwned; //!< Owns the adaptor when created from TopoDS_Edge.
  const BRepAdaptor_Curve*       myPtr = nullptr; //!< Non-owning pointer to the active adaptor.
};

#endif // _BRepProp_Curve_HeaderFile
