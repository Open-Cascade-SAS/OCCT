// Created on: 1998-06-04
// Created by: Philippe NOUAILLE
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

#ifndef _BlendFunc_ChAsymInv_HeaderFile
#define _BlendFunc_ChAsymInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <math_Matrix.hxx>
#include <Blend_FuncInv.hxx>

class BlendFunc_ChAsymInv : public Blend_FuncInv
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_ChAsymInv(const occ::handle<Adaptor3d_Surface>& S1,
                                      const occ::handle<Adaptor3d_Surface>& S2,
                                      const occ::handle<Adaptor3d_Curve>&   C);

  Standard_EXPORT void Set(const bool           OnFirst,
                           const occ::handle<Adaptor2d_Curve2d>& COnSurf) override;

  Standard_EXPORT void GetTolerance(math_Vector&        Tolerance,
                                    const double Tol) const override;

  Standard_EXPORT void GetBounds(math_Vector& InfBound,
                                 math_Vector& SupBound) const override;

  Standard_EXPORT bool IsSolution(const math_Vector&  Sol,
                                              const double Tol) override;

  //! returns the number of equations of the function.
  Standard_EXPORT int NbEquations() const override;

  //! computes the values <F> of the derivatives for the
  //! variable <X> between DegF and DegL.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool ComputeValues(const math_Vector&     X,
                                                 const int DegF,
                                                 const int DegL);

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Derivatives(const math_Vector& X,
                                               math_Matrix&       D) override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X,
                                          math_Vector&       F,
                                          math_Matrix&       D) override;

  Standard_EXPORT void Set(const double    Dist1,
                           const double    Angle,
                           const int Choix);

private:
  occ::handle<Adaptor3d_Surface> surf1;
  occ::handle<Adaptor3d_Surface> surf2;
  double             dist1;
  double             angle;
  double             tgang;
  occ::handle<Adaptor3d_Curve>   curv;
  occ::handle<Adaptor2d_Curve2d> csurf;
  int          choix;
  bool          first;
  math_Vector               FX;
  math_Matrix               DX;
};

#endif // _BlendFunc_ChAsymInv_HeaderFile
