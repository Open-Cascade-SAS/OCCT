// Created on: 1993-12-21
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BlendFunc_EvolRadInv_HeaderFile
#define _BlendFunc_EvolRadInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Blend_FuncInv.hxx>
#include <math_Vector.hxx>

class Law_Function;
class math_Matrix;

class BlendFunc_EvolRadInv : public Blend_FuncInv
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_EvolRadInv(const Handle(Adaptor3d_Surface)& S1,
                                       const Handle(Adaptor3d_Surface)& S2,
                                       const Handle(Adaptor3d_Curve)&   C,
                                       const Handle(Law_Function)&      Law);

  Standard_EXPORT void Set(const Standard_Boolean           OnFirst,
                           const Handle(Adaptor2d_Curve2d)& COnSurf);

  Standard_EXPORT void GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const;

  Standard_EXPORT void GetBounds(math_Vector& InfBound, math_Vector& SupBound) const;

  Standard_EXPORT Standard_Boolean IsSolution(const math_Vector& Sol, const Standard_Real Tol);

  //! returns the number of equations of the function.
  Standard_EXPORT Standard_Integer NbEquations() const;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, math_Vector& F);

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D);

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D);

  Standard_EXPORT void Set(const Standard_Integer Choix);

protected:
private:
  Handle(Adaptor3d_Surface) surf1;
  Handle(Adaptor3d_Surface) surf2;
  Handle(Adaptor3d_Curve)   curv;
  Handle(Adaptor2d_Curve2d) csurf;
  Handle(Law_Function)      fevol;
  Standard_Real             sg1;
  Standard_Real             sg2;
  Standard_Integer          choix;
  Standard_Boolean          first;
};

#endif // _BlendFunc_EvolRadInv_HeaderFile
