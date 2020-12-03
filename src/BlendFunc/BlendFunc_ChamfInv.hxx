// Created on: 1996-06-06
// Created by: Stagiaire Xuan Trang PHAMPHU
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BlendFunc_ChamfInv_HeaderFile
#define _BlendFunc_ChamfInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <BlendFunc_Corde.hxx>
#include <BlendFunc_GenChamfInv.hxx>
#include <math_Vector.hxx>

class math_Matrix;

//! Class for a function used to compute a chamfer with two constant distances
//! on a surface's boundary
class BlendFunc_ChamfInv  : public BlendFunc_GenChamfInv
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BlendFunc_ChamfInv(const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_Curve)& C);
  
  Standard_EXPORT Standard_Boolean IsSolution (const math_Vector& Sol, const Standard_Real Tol) Standard_OVERRIDE;
  
  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F) Standard_OVERRIDE;
  
  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D) Standard_OVERRIDE;

  using Blend_FuncInv::Set;
  
  Standard_EXPORT virtual void Set (const Standard_Real Dist1,
                                    const Standard_Real Dist2,
                                    const Standard_Integer Choix) Standard_OVERRIDE;




protected:





private:



  BlendFunc_Corde corde1;
  BlendFunc_Corde corde2;


};







#endif // _BlendFunc_ChamfInv_HeaderFile
