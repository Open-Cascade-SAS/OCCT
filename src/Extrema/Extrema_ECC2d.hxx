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

#ifndef _Extrema_ECC2d_HeaderFile
#define _Extrema_ECC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Address.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Standard_Integer.hxx>
class StdFail_InfiniteSolutions;
class StdFail_NotDone;
class Standard_OutOfRange;
class Adaptor2d_Curve2d;
class Extrema_Curve2dTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;



class Extrema_ECC2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ECC2d();
  
  Standard_EXPORT Extrema_ECC2d(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2);
  
  Standard_EXPORT Extrema_ECC2d(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup);
  
  Standard_EXPORT void SetParams (const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup);
  
  Standard_EXPORT void SetTolerance (const Standard_Real Tol);

  //! Set flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT void SetSingleSolutionFlag (const Standard_Boolean theSingleSolutionFlag);

  //! Get flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT Standard_Boolean GetSingleSolutionFlag () const;

  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  Standard_EXPORT Standard_Integer NbExt() const;
  
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N = 1) const;
  
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnCurv2d& P1, Extrema_POnCurv2d& P2) const;




protected:





private:


  Standard_Boolean myIsFindSingleSolution; // Default value is false.
  Standard_Boolean myParallel;
  Standard_Real myCurveMinTol;
  math_Vector myLowBorder;
  math_Vector myUppBorder;
  TColStd_SequenceOfReal myPoints1;
  TColStd_SequenceOfReal myPoints2;
  Standard_Address myC[2];
  Standard_Boolean myDone;


};







#endif // _Extrema_ECC2d_HeaderFile
