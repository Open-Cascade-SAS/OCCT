// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _BlendFunc_GenChamfInv_HeaderFile
#define _BlendFunc_GenChamfInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Blend_FuncInv.hxx>
#include <math_Vector.hxx>

class math_Matrix;

//! Deferred class for a function used to compute a general chamfer on a surface's boundary
class BlendFunc_GenChamfInv : public Blend_FuncInv
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_GenChamfInv(const occ::handle<Adaptor3d_Surface>& S1,
                                        const occ::handle<Adaptor3d_Surface>& S2,
                                        const occ::handle<Adaptor3d_Curve>&   C);

  Standard_EXPORT virtual void Set(const bool           OnFirst,
                                   const occ::handle<Adaptor2d_Curve2d>& COnSurf) override;

  Standard_EXPORT void GetTolerance(math_Vector&        Tolerance,
                                    const double Tol) const override;

  Standard_EXPORT void GetBounds(math_Vector& InfBound,
                                 math_Vector& SupBound) const override;

  //! returns the number of equations of the function.
  Standard_EXPORT int NbEquations() const override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X,
                                          math_Vector&       F,
                                          math_Matrix&       D) override;

  Standard_EXPORT virtual void Set(const double    Dist1,
                                   const double    Dist2,
                                   const int Choix) = 0;

protected:
  occ::handle<Adaptor3d_Surface> surf1;
  occ::handle<Adaptor3d_Surface> surf2;
  occ::handle<Adaptor3d_Curve>   curv;
  occ::handle<Adaptor2d_Curve2d> csurf;
  int          choix;
  bool          first;

};

#endif // _BlendFunc_GenChamfInv_HeaderFile
