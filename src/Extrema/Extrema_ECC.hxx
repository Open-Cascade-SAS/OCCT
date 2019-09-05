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

#ifndef _Extrema_ECC_HeaderFile
#define _Extrema_ECC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Address.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Standard_Integer.hxx>

class Adaptor3d_Curve;
class Extrema_CurveTool;
class Extrema_POnCurv;
class gp_Pnt;
class gp_Vec;


class Extrema_ECC 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Calculates all the distances as above
  //! between Uinf and Usup for C1 and  between Vinf and Vsup
  //! for C2.
  Standard_EXPORT Extrema_ECC();
  
  //! It calculates all the distances.
  //! The function F(u,v)=distance(C1(u),C2(v)) has an
  //! extremum when gradient(f)=0. The algorithm uses
  //! Evtushenko's global optimization solver.
  Standard_EXPORT Extrema_ECC(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2);
  
  //! Calculates all the distances as above
  //! between Uinf and Usup for C1 and  between Vinf and Vsup
  //! for C2.
  Standard_EXPORT Extrema_ECC(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup);
  
  //! Set params in case of empty constructor is usage.
  Standard_EXPORT void SetParams (const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup);
  
  Standard_EXPORT void SetTolerance (const Standard_Real Tol);

  //! Set flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT void SetSingleSolutionFlag (const Standard_Boolean theSingleSolutionFlag);

  //! Get flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT Standard_Boolean GetSingleSolutionFlag () const;

  //! Performs calculations.
  Standard_EXPORT void Perform();
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns state of myParallel flag.
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth square extremum distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N = 1) const;
  
  //! Returns the points of the Nth extremum distance.
  //! P1 is on the first curve, P2 on the second one.
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnCurv& P1, Extrema_POnCurv& P2) const;




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







#endif // _Extrema_ECC_HeaderFile
