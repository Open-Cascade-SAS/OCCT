// Created on: 1999-06-22
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeExtend_ComplexCurve_HeaderFile
#define _ShapeExtend_ComplexCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>
class gp_Trsf;

//! Defines a curve which consists of several segments.
//! Implements basic interface to it.
class ShapeExtend_ComplexCurve : public Geom_Curve
{

public:


  //! Returns number of curves
  Standard_EXPORT virtual int NbCurves() const = 0;

  //! Returns curve given by its index
  Standard_EXPORT virtual const occ::handle<Geom_Curve>& Curve(const int index) const = 0;

  //! Returns number of the curve for the given parameter U
  //! and local parameter UOut for the found curve
  Standard_EXPORT virtual int LocateParameter(const double U, double& UOut) const = 0;

  //! Returns global parameter for the whole curve according
  //! to the segment and local parameter on it
  Standard_EXPORT virtual double LocalToGlobal(const int index, const double Ulocal) const = 0;

  //! Applies transformation to each curve
  Standard_EXPORT void Transform(const gp_Trsf& T) override;

  //! Returns 1 - U
  double ReversedParameter(const double U) const override;

  //! Returns 0
  double FirstParameter() const override;

  //! Returns 1
  double LastParameter() const override;

  //! Returns True if the curve is closed
  bool IsClosed() const override;

  //! Returns False
  bool IsPeriodic() const override;

  //! Returns GeomAbs_C0
  GeomAbs_Shape Continuity() const override;

  //! Returns False if N > 0
  bool IsCN(const int N) const override;

  //! Returns point at parameter U.
  //! Finds appropriate curve and local parameter on it.
  Standard_EXPORT std::optional<gp_Pnt> EvalD0(const double U) const override;

  Standard_EXPORT std::optional<Geom_CurveD1> EvalD1(const double U) const override;

  Standard_EXPORT std::optional<Geom_CurveD2> EvalD2(const double U) const override;

  Standard_EXPORT std::optional<Geom_CurveD3> EvalD3(const double U) const override;

  Standard_EXPORT std::optional<gp_Vec> EvalDN(const double U, const int N) const override;

  //! Returns scale factor for recomputing of deviatives.
  Standard_EXPORT virtual double GetScaleFactor(const int ind) const = 0;

  //! Checks geometrical connectivity of the curves, including
  //! closure (sets fields myClosed)
  Standard_EXPORT bool CheckConnectivity(const double Preci);

  DEFINE_STANDARD_RTTIEXT(ShapeExtend_ComplexCurve, Geom_Curve)

protected:
  Standard_EXPORT ShapeExtend_ComplexCurve();

  //! Transform the derivative according to its order
  Standard_EXPORT void TransformDN(gp_Vec& V, const int ind, const int N) const;

  bool myClosed;
};

#include <ShapeExtend_ComplexCurve.lxx>

#endif // _ShapeExtend_ComplexCurve_HeaderFile
