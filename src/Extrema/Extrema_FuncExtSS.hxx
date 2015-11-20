// Created on: 1996-01-09
// Created by: Laurent PAINNOT
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

#ifndef _Extrema_FuncExtSS_HeaderFile
#define _Extrema_FuncExtSS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Adaptor3d_SurfacePtr.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <Standard_Boolean.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
class Standard_OutOfRange;
class Adaptor3d_Surface;
class math_Matrix;
class Extrema_POnSurf;


//! Function to find extrema of the
//! distance between two surfaces.
class Extrema_FuncExtSS  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_FuncExtSS();
  
  Standard_EXPORT Extrema_FuncExtSS(const Adaptor3d_Surface& S1, const Adaptor3d_Surface& S2);
  
  //! sets the field mysurf of the function.
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S1, const Adaptor3d_Surface& S2);
  
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! Calculate Fi(U,V).
  Standard_EXPORT Standard_Boolean Value (const math_Vector& UV, math_Vector& F) Standard_OVERRIDE;
  
  //! Calculate Fi'(U,V).
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& UV, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Calculate Fi(U,V) and Fi'(U,V).
  Standard_EXPORT Standard_Boolean Values (const math_Vector& UV, math_Vector& F, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Save the found extremum.
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
  //! Return the number of found extrema.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Return the value of the Nth distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Return the Nth extremum on S1.
  Standard_EXPORT const Extrema_POnSurf& PointOnS1 (const Standard_Integer N) const;
  
  //! Renvoie le Nieme extremum sur S2.
  Standard_EXPORT const Extrema_POnSurf& PointOnS2 (const Standard_Integer N) const;




protected:





private:

  
  Standard_EXPORT Adaptor3d_SurfacePtr Bidon() const;


  Adaptor3d_SurfacePtr myS1;
  Adaptor3d_SurfacePtr myS2;
  gp_Pnt myP1;
  gp_Pnt myP2;
  Standard_Real myU1;
  Standard_Real myV1;
  Standard_Real myU2;
  Standard_Real myV2;
  TColStd_SequenceOfReal mySqDist;
  Extrema_SequenceOfPOnSurf myPoint1;
  Extrema_SequenceOfPOnSurf myPoint2;
  Standard_Boolean myS1init;
  Standard_Boolean myS2init;


};







#endif // _Extrema_FuncExtSS_HeaderFile
