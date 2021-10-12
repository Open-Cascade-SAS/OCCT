// Created on: 1996-01-22
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

#ifndef _Extrema_FuncExtCS_HeaderFile
#define _Extrema_FuncExtCS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <Standard_Boolean.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
class Adaptor3d_Curve;
class Adaptor3d_Surface;
class math_Matrix;
class Extrema_POnCurv;
class Extrema_POnSurf;


//! Function to find extrema of the
//! distance between a curve and a surface.
class Extrema_FuncExtCS  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_FuncExtCS();
  
  Standard_EXPORT Extrema_FuncExtCS(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S);
  
  //! sets the field mysurf of the function.
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Adaptor3d_Surface& S);
  
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! Calculation of Fi(U,V).
  Standard_EXPORT Standard_Boolean Value (const math_Vector& UV, math_Vector& F) Standard_OVERRIDE;
  
  //! Calculation of Fi'(U,V).
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& UV, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Calculation of Fi(U,V) and Fi'(U,V).
  Standard_EXPORT Standard_Boolean Values (const math_Vector& UV, math_Vector& F, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Save the found extremum.
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
  //! Return the number of found extrema.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Return the value of the Nth distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the Nth extremum on C.
  Standard_EXPORT const Extrema_POnCurv& PointOnCurve (const Standard_Integer N) const;
  
  //! Return the Nth extremum on S.
  Standard_EXPORT const Extrema_POnSurf& PointOnSurface (const Standard_Integer N) const;

  //! Change Sequence of SquareDistance
  TColStd_SequenceOfReal& SquareDistances()
  {
    return mySqDist;
  }
  //! Change Sequence of PointOnCurv
  Extrema_SequenceOfPOnCurv& PointsOnCurve()
  {
    return myPoint1;
  }
  //! Change Sequence of PointOnSurf
  Extrema_SequenceOfPOnSurf& PointsOnSurf()
  {
    return myPoint2;
  }

private:

  const Adaptor3d_Curve* myC;
  const Adaptor3d_Surface* myS;
  gp_Pnt myP1;
  gp_Pnt myP2;
  Standard_Real myt;
  Standard_Real myU;
  Standard_Real myV;
  TColStd_SequenceOfReal mySqDist;
  Extrema_SequenceOfPOnCurv myPoint1;
  Extrema_SequenceOfPOnSurf myPoint2;
  Standard_Boolean myCinit;
  Standard_Boolean mySinit;

};

#endif // _Extrema_FuncExtCS_HeaderFile
