// Created on: 1993-06-03
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

#ifndef _Contap_SurfFunction_HeaderFile
#define _Contap_SurfFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Contap_TFunction.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <Standard_Boolean.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
class Adaptor3d_HSurface;
class StdFail_UndefinedDerivative;
class gp_Pnt;
class gp_Dir;
class math_Matrix;
class gp_Vec;
class gp_Dir2d;


//! This class describes the function on a parametric surface.
//! the form of the function is F(u,v) = 0 where u and v are
//! the parameteric coordinates of a point on the surface,
//! to compute the contours of the surface.
class Contap_SurfFunction  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Contap_SurfFunction();
  
  Standard_EXPORT void Set (const Handle(Adaptor3d_HSurface)& S);
  
    void Set (const gp_Pnt& Eye);
  
    void Set (const gp_Dir& Dir);
  
    void Set (const gp_Dir& Dir, const Standard_Real Angle);
  
    void Set (const gp_Pnt& Eye, const Standard_Real Angle);
  
    void Set (const Standard_Real Tolerance);
  
  //! This method has to return 2.
  Standard_EXPORT Standard_Integer NbVariables() const;
  
  //! This method has to return 1.
  Standard_EXPORT Standard_Integer NbEquations() const;
  
  //! The dimension of F is 1.
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F);
  
  //! The dimension of D is (1,2).
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D);
  
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D);
  
  //! Root is the value of the function at the solution.
  //! It is a vector of dimension 1, i-e a real.
    Standard_Real Root() const;
  
  //! Returns the value Tol so that if Abs(Func.Root())<Tol
  //! the function is considered null.
    Standard_Real Tolerance() const;
  
  //! Returns the value of the solution point on the surface.
    const gp_Pnt& Point() const;
  
  Standard_EXPORT Standard_Boolean IsTangent();
  
    const gp_Vec& Direction3d();
  
    const gp_Dir2d& Direction2d();
  
    Contap_TFunction FunctionType() const;
  
    const gp_Pnt& Eye() const;
  
    const gp_Dir& Direction() const;
  
    Standard_Real Angle() const;
  
    const Handle(Adaptor3d_HSurface)& Surface() const;




protected:





private:



  Handle(Adaptor3d_HSurface) mySurf;
  Standard_Real myMean;
  Contap_TFunction myType;
  gp_Dir myDir;
  gp_Pnt myEye;
  Standard_Real myAng;
  Standard_Real myCosAng;
  Standard_Real tol;
  gp_Pnt solpt;
  Standard_Real valf;
  Standard_Real Usol;
  Standard_Real Vsol;
  Standard_Real Fpu;
  Standard_Real Fpv;
  gp_Dir2d d2d;
  gp_Vec d3d;
  Standard_Boolean tangent;
  Standard_Boolean computed;
  Standard_Boolean derived;


};


#include <Contap_SurfFunction.lxx>





#endif // _Contap_SurfFunction_HeaderFile
