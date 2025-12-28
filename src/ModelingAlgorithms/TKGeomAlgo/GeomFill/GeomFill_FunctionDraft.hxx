// Created on: 1998-04-27
// Created by: Stephanie HUMEAU
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _GeomFill_FunctionDraft_HeaderFile
#define _GeomFill_FunctionDraft_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>

class math_Matrix;
class gp_Vec;
class GeomFill_Tensor;

class GeomFill_FunctionDraft : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_FunctionDraft(const occ::handle<Adaptor3d_Surface>& S,
                                         const occ::handle<Adaptor3d_Curve>&   C);

  //! returns the number of variables of the function.
  Standard_EXPORT virtual int NbVariables() const override;

  //! returns the number of equations of the function.
  Standard_EXPORT virtual int NbEquations() const override;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Value(const math_Vector& X, math_Vector& F) override;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Derivatives(const math_Vector& X, math_Matrix& D) override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Values(const math_Vector& X,
                                      math_Vector&       F,
                                      math_Matrix&       D) override;

  //! returns the values <F> of the T derivatives for
  //! the parameter Param.
  Standard_EXPORT bool DerivT(const occ::handle<Adaptor3d_Curve>& C,
                              const double                        Param,
                              const double                        W,
                              const gp_Vec&                       dN,
                              const double                        teta,
                              math_Vector&                        F);

  //! returns the values <F> of the T2 derivatives for
  //! the parameter Param.
  Standard_EXPORT bool Deriv2T(const occ::handle<Adaptor3d_Curve>& C,
                               const double                        Param,
                               const double                        W,
                               const gp_Vec&                       d2N,
                               const double                        teta,
                               math_Vector&                        F);

  //! returns the values <D> of the TX derivatives for
  //! the parameter Param.
  Standard_EXPORT bool DerivTX(const gp_Vec& dN, const double teta, math_Matrix& D);

  //! returns the values <T> of the X2 derivatives for
  //! the parameter Param.
  Standard_EXPORT bool Deriv2X(const math_Vector& X, GeomFill_Tensor& T);

private:
  occ::handle<Adaptor3d_Curve>   TheCurve;
  occ::handle<Adaptor3d_Surface> TheSurface;
};

#endif // _GeomFill_FunctionDraft_HeaderFile
