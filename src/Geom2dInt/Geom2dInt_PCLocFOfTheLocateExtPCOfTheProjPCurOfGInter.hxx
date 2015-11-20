// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
#define _Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Extrema_SequenceOfPOnCurv2d.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <math_FunctionWithDerivative.hxx>
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;

class Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter();
  
  Standard_EXPORT Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C);
  
  //! sets the field mycurve of the function.
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C);
  
  //! sets the field P of the function.
  Standard_EXPORT void SetPoint (const gp_Pnt2d& P);
  
  //! Calculation of F(U).
  Standard_EXPORT Standard_Boolean Value (const Standard_Real U, Standard_Real& F) Standard_OVERRIDE;
  
  //! Calculation of F'(U).
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real U, Standard_Real& DF) Standard_OVERRIDE;
  
  //! Calculation of F(U) and F'(U).
  Standard_EXPORT Standard_Boolean Values (const Standard_Real U, Standard_Real& F, Standard_Real& DF) Standard_OVERRIDE;
  
  //! Save the found extremum.
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
  //! Return the nunber of found extrema.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the Nth distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Shows if the Nth distance is a minimum.
  Standard_EXPORT Standard_Boolean IsMin (const Standard_Integer N) const;
  
  //! Returns the Nth extremum.
  Standard_EXPORT const Extrema_POnCurv2d& Point (const Standard_Integer N) const;
  
  //! Determines boundaries of subinterval for find of root.
  Standard_EXPORT void SubIntervalInitialize (const Standard_Real theUfirst, const Standard_Real theUlast);
  
  //! Computes a Tol value. If 1st derivative of curve
  //! |D1|<Tol, it is considered D1=0.
  Standard_EXPORT Standard_Real SearchOfTolerance();




protected:





private:



  gp_Pnt2d myP;
  Standard_Address myC;
  Standard_Real myU;
  gp_Pnt2d myPc;
  Standard_Real myD1f;
  TColStd_SequenceOfReal mySqDist;
  TColStd_SequenceOfInteger myIsMin;
  Extrema_SequenceOfPOnCurv2d myPoint;
  Standard_Boolean myPinit;
  Standard_Boolean myCinit;
  Standard_Boolean myD1Init;
  Standard_Real myTol;
  Standard_Integer myMaxDerivOrder;
  Standard_Real myUinfium;
  Standard_Real myUsupremum;


};







#endif // _Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
