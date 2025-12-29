// Created on: 1997-02-21
// Created by: Laurent BOURESCHE
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepBlend_SurfCurvConstRadInv_HeaderFile
#define _BRepBlend_SurfCurvConstRadInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <math_Vector.hxx>

class math_Matrix;

//! Function of reframing between a restriction surface of the
//! surface and a curve.
//! Class used to compute a solution of the
//! surfRstConstRad problem on a done restriction of the
//! surface.
//! The vector <X> used in Value, Values and Derivatives
//! methods has to be the vector of the parametric
//! coordinates wguide, wcurv, wrst where wguide is the
//! parameter on the guide line, wcurv is the parameter on
//! the curve, wrst is the parameter on the restriction on
//! the surface.
class BRepBlend_SurfCurvConstRadInv : public Blend_SurfCurvFuncInv
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_SurfCurvConstRadInv(const occ::handle<Adaptor3d_Surface>& S,
                                                const occ::handle<Adaptor3d_Curve>&   C,
                                                const occ::handle<Adaptor3d_Curve>&   Cg);

  Standard_EXPORT void Set(const double R, const int Choix);

  //! returns 3.
  Standard_EXPORT int NbEquations() const override;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D) override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override;

  //! Set the restriction on which a solution has to be found.
  Standard_EXPORT void Set(const occ::handle<Adaptor2d_Curve2d>& Rst) override;

  //! Returns in the vector Tolerance the parametric tolerance
  //! for each of the 3 variables;
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT void GetTolerance(math_Vector& Tolerance, const double Tol) const override;

  //! Returns in the vector InfBound the lowest values allowed
  //! for each of the 3 variables.
  //! Returns in the vector SupBound the greatest values allowed
  //! for each of the 3 variables.
  Standard_EXPORT void GetBounds(math_Vector& InfBound, math_Vector& SupBound) const override;

  //! Returns true if Sol is a zero of the function.
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT bool IsSolution(const math_Vector& Sol, const double Tol) override;

private:
  occ::handle<Adaptor3d_Surface> surf;
  occ::handle<Adaptor3d_Curve>   curv;
  occ::handle<Adaptor3d_Curve>   guide;
  occ::handle<Adaptor2d_Curve2d> rst;
  double                         ray;
  int                            choix;
};

#endif // _BRepBlend_SurfCurvConstRadInv_HeaderFile
