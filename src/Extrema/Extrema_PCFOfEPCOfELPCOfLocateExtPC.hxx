// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Extrema_PCFOfEPCOfELPCOfLocateExtPC_HeaderFile
#define _Extrema_PCFOfEPCOfELPCOfLocateExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <Standard_Integer.hxx>
#include <math_FunctionWithDerivative.hxx>
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor3d_Curve;
class Extrema_CurveTool;
class Extrema_POnCurv;
class gp_Pnt;
class gp_Vec;

class Extrema_PCFOfEPCOfELPCOfLocateExtPC  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_PCFOfEPCOfELPCOfLocateExtPC();
  
  Standard_EXPORT Extrema_PCFOfEPCOfELPCOfLocateExtPC(const gp_Pnt& P, const Adaptor3d_Curve& C);
  
  //! sets the field mycurve of the function.
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C);
  
  //! sets the field P of the function.
  Standard_EXPORT void SetPoint (const gp_Pnt& P);
  
  //! Calculation of F(U).
  Standard_EXPORT Standard_Boolean Value (const Standard_Real U, Standard_Real& F) Standard_OVERRIDE;
  
  //! Calculation of F'(U).
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real U, Standard_Real& DF) Standard_OVERRIDE;
  
  //! Calculation of F(U) and F'(U).
  Standard_EXPORT Standard_Boolean Values (const Standard_Real U, Standard_Real& F, Standard_Real& DF) Standard_OVERRIDE;
  
  //! Save the found extremum.
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
  //! Return the number of found extrema.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the Nth distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Shows if the Nth distance is a minimum.
  Standard_EXPORT Standard_Boolean IsMin (const Standard_Integer N) const;
  
  //! Returns the Nth extremum.
  Standard_EXPORT const Extrema_POnCurv& Point (const Standard_Integer N) const;
  
  //! Determines boundaries of subinterval for find of root.
  Standard_EXPORT void SubIntervalInitialize (const Standard_Real theUfirst, const Standard_Real theUlast);
  
  //! Computes a Tol value. If 1st derivative of curve
  //! |D1|<Tol, it is considered D1=0.
  Standard_EXPORT Standard_Real SearchOfTolerance();




protected:





private:



  gp_Pnt myP;
  Standard_Address myC;
  Standard_Real myU;
  gp_Pnt myPc;
  Standard_Real myD1f;
  TColStd_SequenceOfReal mySqDist;
  TColStd_SequenceOfInteger myIsMin;
  Extrema_SequenceOfPOnCurv myPoint;
  Standard_Boolean myPinit;
  Standard_Boolean myCinit;
  Standard_Boolean myD1Init;
  Standard_Real myTol;
  Standard_Integer myMaxDerivOrder;
  Standard_Real myUinfium;
  Standard_Real myUsupremum;


};







#endif // _Extrema_PCFOfEPCOfELPCOfLocateExtPC_HeaderFile
