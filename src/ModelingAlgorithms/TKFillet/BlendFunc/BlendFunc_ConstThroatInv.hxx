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

#ifndef _BlendFunc_ConstThroatInv_HeaderFile
#define _BlendFunc_ConstThroatInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <BlendFunc_GenChamfInv.hxx>
#include <math_Vector.hxx>

class math_Matrix;

//! Class for a function used to compute a ConstThroat chamfer on a surface's boundary
class BlendFunc_ConstThroatInv : public BlendFunc_GenChamfInv
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_ConstThroatInv(const occ::handle<Adaptor3d_Surface>& S1,
                                           const occ::handle<Adaptor3d_Surface>& S2,
                                           const occ::handle<Adaptor3d_Curve>&   C);

  Standard_EXPORT bool IsSolution(const math_Vector& Sol, const double Tol) override;

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

  using Blend_FuncInv::Set;

  Standard_EXPORT virtual void Set(const double theThroat, const double, const int Choix) override;

protected:
  double Throat;

  double param;
  double sign1;
  double sign2;

  gp_Pnt ptgui;
  gp_Vec nplan;
  double normtg;
  double theD;
  gp_Vec d1gui;
  gp_Vec d2gui;

  gp_Pnt pts1;
  gp_Pnt pts2;
  gp_Vec d1u1;
  gp_Vec d1v1;
  gp_Vec d1u2;
  gp_Vec d1v2;
};

#endif // _BlendFunc_ConstThroatInv_HeaderFile
