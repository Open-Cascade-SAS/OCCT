// Created on: 1997-02-12
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

#ifndef _BRepBlend_SurfPointConstRadInv_HeaderFile
#define _BRepBlend_SurfPointConstRadInv_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <math_Vector.hxx>

class math_Matrix;

//! Function of reframing between a point and a surface.
//! This function  is used  to find a  solution on  a done
//! point   of   the curve when   using  SurfRstConsRad or
//! CSConstRad...
//! The vector <X>  used in Value, Values and  Derivatives
//! methods  has  to   be the  vector   of the  parametric
//! coordinates w, U,  V where w is  the parameter  on the
//! guide line, U,V   are the parametric coordinates of  a
//! point on the partner surface.
class BRepBlend_SurfPointConstRadInv  : public Blend_SurfPointFuncInv
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepBlend_SurfPointConstRadInv(const Handle(Adaptor3d_Surface)& S, const Handle(Adaptor3d_Curve)& C);
  
  Standard_EXPORT void Set (const Standard_Real R, const Standard_Integer Choix);
  
  //! returns 3.
  Standard_EXPORT Standard_Integer NbEquations() const;
  
  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F);
  
  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D);
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D);
  
  //! Set the Point on which a solution has to be found.
  Standard_EXPORT void Set (const gp_Pnt& P);
  
  //! Returns in the vector Tolerance the parametric tolerance
  //! for each of the 3 variables;
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT void GetTolerance (math_Vector& Tolerance, const Standard_Real Tol) const;
  
  //! Returns in the vector InfBound the lowest values allowed
  //! for each of the 3 variables.
  //! Returns in the vector SupBound the greatest values allowed
  //! for each of the 3 variables.
  Standard_EXPORT void GetBounds (math_Vector& InfBound, math_Vector& SupBound) const;
  
  //! Returns Standard_True if Sol is a zero of the function.
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT Standard_Boolean IsSolution (const math_Vector& Sol, const Standard_Real Tol);




protected:





private:



  Handle(Adaptor3d_Surface) surf;
  Handle(Adaptor3d_Curve) curv;
  gp_Pnt point;
  Standard_Real ray;
  Standard_Integer choix;


};







#endif // _BRepBlend_SurfPointConstRadInv_HeaderFile
