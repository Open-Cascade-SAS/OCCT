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
#include <BlendFunc_Corde.hxx>
#include <Blend_FuncInv.hxx>
#include <math_Vector.hxx>

class math_Matrix;

//! Deferred class for a function used to compute a general chamfer on a surface's boundary
class BlendFunc_GenChamfInv  : public Blend_FuncInv
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_GenChamfInv(const Handle(Adaptor3d_Surface)& S1,
                                        const Handle(Adaptor3d_Surface)& S2,
                                        const Handle(Adaptor3d_Curve)& C);
  
  Standard_EXPORT virtual void Set (const Standard_Boolean OnFirst,
                                    const Handle(Adaptor2d_Curve2d)& COnSurf) Standard_OVERRIDE;
  
  Standard_EXPORT void GetTolerance (math_Vector& Tolerance,
                                     const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT void GetBounds (math_Vector& InfBound,
                                  math_Vector& SupBound) const Standard_OVERRIDE;
  
  //! returns the number of equations of the function.
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X,
                                           math_Vector& F,
                                           math_Matrix& D) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Set (const Standard_Real Dist1,
                                    const Standard_Real Dist2,
                                    const Standard_Integer Choix) = 0;




protected:

  Handle(Adaptor3d_Surface) surf1;
  Handle(Adaptor3d_Surface) surf2;
  Handle(Adaptor3d_Curve) curv;
  Handle(Adaptor2d_Curve2d) csurf;
  Standard_Integer choix;
  Standard_Boolean first;


private:

  
};







#endif // _BlendFunc_GenChamfInv_HeaderFile
