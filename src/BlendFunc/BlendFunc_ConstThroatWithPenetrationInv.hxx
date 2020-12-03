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

#ifndef _BlendFunc_ConstThroatWithPenetrationInv_HeaderFile
#define _BlendFunc_ConstThroatWithPenetrationInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <BlendFunc_ConstThroatInv.hxx>
#include <math_Vector.hxx>

class math_Matrix;

//! Class for a function used to compute a ConstThroatWithPenetration chamfer
//! on a surface's boundary
class BlendFunc_ConstThroatWithPenetrationInv  : public BlendFunc_ConstThroatInv
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BlendFunc_ConstThroatWithPenetrationInv(const Handle(Adaptor3d_Surface)& S1,
                                                          const Handle(Adaptor3d_Surface)& S2,
                                                          const Handle(Adaptor3d_Curve)& C);
  
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
  


protected:


private:
  

};







#endif // _BlendFunc_ConstThroatWithPenetrationInv_HeaderFile
