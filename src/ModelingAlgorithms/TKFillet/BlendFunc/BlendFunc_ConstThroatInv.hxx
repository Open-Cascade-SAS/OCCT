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

  Standard_EXPORT BlendFunc_ConstThroatInv(const Handle(Adaptor3d_Surface)& S1,
                                           const Handle(Adaptor3d_Surface)& S2,
                                           const Handle(Adaptor3d_Curve)&   C);

  Standard_EXPORT Standard_Boolean IsSolution(const math_Vector&  Sol,
                                              const Standard_Real Tol) Standard_OVERRIDE;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, math_Vector& F) Standard_OVERRIDE;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives(const math_Vector& X,
                                               math_Matrix&       D) Standard_OVERRIDE;

  using Blend_FuncInv::Set;

  Standard_EXPORT virtual void Set(const Standard_Real theThroat,
                                   const Standard_Real,
                                   const Standard_Integer Choix) Standard_OVERRIDE;

protected:
  Standard_Real Throat;

  Standard_Real param;
  Standard_Real sign1;
  Standard_Real sign2;

  gp_Pnt        ptgui;
  gp_Vec        nplan;
  Standard_Real normtg;
  Standard_Real theD;
  gp_Vec        d1gui;
  gp_Vec        d2gui;

  gp_Pnt pts1;
  gp_Pnt pts2;
  gp_Vec d1u1;
  gp_Vec d1v1;
  gp_Vec d1u2;
  gp_Vec d1v2;

private:
};

#endif // _BlendFunc_ConstThroatInv_HeaderFile
