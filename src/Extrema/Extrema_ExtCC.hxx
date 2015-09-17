// Created on: 1994-07-06
// Created by: Laurent PAINNOT
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Extrema_ExtCC_HeaderFile
#define _Extrema_ExtCC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Extrema_ECC.hxx>
#include <Standard_Boolean.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt.hxx>
class StdFail_InfiniteSolutions;
class StdFail_NotDone;
class Standard_OutOfRange;
class Adaptor3d_Curve;
class Extrema_POnCurv;
class gp_Pnt;
class Extrema_ExtElC;
class Extrema_ECC;


//! It calculates all the distance between two curves.
//! These distances can be maximum or minimum.
class Extrema_ExtCC 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtCC(const Standard_Real TolC1 = 1.0e-10, const Standard_Real TolC2 = 1.0e-10);
  
  //! It calculates all the distances.
  Standard_EXPORT Extrema_ExtCC(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2, const Standard_Real TolC1 = 1.0e-10, const Standard_Real TolC2 = 1.0e-10);
  
  //! It calculates all the distances.
  Standard_EXPORT Extrema_ExtCC(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2, const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2, const Standard_Real TolC1 = 1.0e-10, const Standard_Real TolC2 = 1.0e-10);
  
  Standard_EXPORT void SetCurve (const Standard_Integer theRank, const Adaptor3d_Curve& C);
  
  Standard_EXPORT void SetCurve (const Standard_Integer theRank, const Adaptor3d_Curve& C, const Standard_Real Uinf, const Standard_Real Usup);
  
  Standard_EXPORT void SetRange (const Standard_Integer theRank, const Standard_Real Uinf, const Standard_Real Usup);
  
  Standard_EXPORT void SetTolerance (const Standard_Integer theRank, const Standard_Real Tol);
  
  Standard_EXPORT void Perform();
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns True if the two curves are parallel.
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Returns the value of the Nth extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N = 1) const;
  
  //! Returns the points of the Nth extremum distance.
  //! P1 is on the first curve, P2 on the second one.
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnCurv& P1, Extrema_POnCurv& P2) const;
  
  //! if the curve is a trimmed curve,
  //! dist11 is a square distance between the point on C1
  //! of parameter FirstParameter and the point of
  //! parameter FirstParameter on C2.
  Standard_EXPORT void TrimmedSquareDistances (Standard_Real& dist11, Standard_Real& distP12, Standard_Real& distP21, Standard_Real& distP22, gp_Pnt& P11, gp_Pnt& P12, gp_Pnt& P21, gp_Pnt& P22) const;

  //! Set flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT void SetSingleSolutionFlag (const Standard_Boolean theSingleSolutionFlag);

  //! Get flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT Standard_Boolean GetSingleSolutionFlag () const;

protected:

  
  Standard_EXPORT void Results (const Extrema_ExtElC& AlgExt, const Standard_Real Ut11, const Standard_Real Ut12, const Standard_Real Ut21, const Standard_Real Ut22);
  
  Standard_EXPORT void Results (const Extrema_ECC& AlgExt, const Standard_Real Ut11, const Standard_Real Ut12, const Standard_Real Ut21, const Standard_Real Ut22);




private:


  Standard_Boolean myIsFindSingleSolution; // Default value is false.
  Extrema_ECC myECC;
  Standard_Boolean myDone;
  Standard_Boolean myIsPar;
  Extrema_SequenceOfPOnCurv mypoints;
  TColStd_SequenceOfReal mySqDist;
  Standard_Integer mynbext;
  Standard_Boolean inverse;
  Standard_Address myC[2];
  Standard_Real myInf[2];
  Standard_Real mySup[2];
  Standard_Real myTol[2];
  gp_Pnt P1f;
  gp_Pnt P1l;
  gp_Pnt P2f;
  gp_Pnt P2l;
  Standard_Real mydist11;
  Standard_Real mydist12;
  Standard_Real mydist21;
  Standard_Real mydist22;


};







#endif // _Extrema_ExtCC_HeaderFile
