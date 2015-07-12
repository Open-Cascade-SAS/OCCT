// Created on: 1994-03-03
// Created by: Joelle CHAUVET
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

#ifndef _BRepFill_ComputeCLine_HeaderFile
#define _BRepFill_ComputeCLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_SequenceOfMultiCurve.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <AppParCurves_Constraint.hxx>
#include <BRepFill_MultiLine.hxx>
class AppParCurves_MultiCurve;



class BRepFill_ComputeCLine 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_ComputeCLine(const BRepFill_MultiLine& Line, const Standard_Integer degreemin = 3, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-5, const Standard_Real Tolerance2d = 1.0e-5, const Standard_Boolean cutting = Standard_False, const AppParCurves_Constraint FirstC = AppParCurves_TangencyPoint, const AppParCurves_Constraint LastC = AppParCurves_TangencyPoint);
  
  Standard_EXPORT BRepFill_ComputeCLine(const Standard_Integer degreemin = 3, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-05, const Standard_Real Tolerance2d = 1.0e-05, const Standard_Boolean cutting = Standard_False, const AppParCurves_Constraint FirstC = AppParCurves_TangencyPoint, const AppParCurves_Constraint LastC = AppParCurves_TangencyPoint);
  
  Standard_EXPORT void Perform (const BRepFill_MultiLine& Line);
  
  Standard_EXPORT void SetDegrees (const Standard_Integer degreemin, const Standard_Integer degreemax);
  
  Standard_EXPORT void SetTolerances (const Standard_Real Tolerance3d, const Standard_Real Tolerance2d);
  
  Standard_EXPORT void SetConstraints (const AppParCurves_Constraint FirstC, const AppParCurves_Constraint LastC);
  
  Standard_EXPORT Standard_Boolean IsAllApproximated() const;
  
  Standard_EXPORT Standard_Boolean IsToleranceReached() const;
  
  Standard_EXPORT void Error (const Standard_Integer Index, Standard_Real& tol3d, Standard_Real& tol2d) const;
  
  Standard_EXPORT Standard_Integer NbMultiCurves() const;
  
  Standard_EXPORT AppParCurves_MultiCurve Value (const Standard_Integer Index = 1) const;
  
  Standard_EXPORT void Parameters (const Standard_Integer Index, Standard_Real& firstp, Standard_Real& lastp) const;




protected:





private:

  
  Standard_EXPORT Standard_Boolean Compute (const BRepFill_MultiLine& Line, const Standard_Real Ufirst, const Standard_Real Ulast, Standard_Real& TheTol3d, Standard_Real& TheTol2d);


  AppParCurves_SequenceOfMultiCurve myMultiCurves;
  TColStd_SequenceOfReal myfirstparam;
  TColStd_SequenceOfReal mylastparam;
  AppParCurves_MultiCurve TheMultiCurve;
  Standard_Boolean alldone;
  Standard_Boolean tolreached;
  TColStd_SequenceOfReal Tolers3d;
  TColStd_SequenceOfReal Tolers2d;
  Standard_Integer mydegremin;
  Standard_Integer mydegremax;
  Standard_Real mytol3d;
  Standard_Real mytol2d;
  Standard_Real currenttol3d;
  Standard_Real currenttol2d;
  Standard_Boolean mycut;
  AppParCurves_Constraint myfirstC;
  AppParCurves_Constraint mylastC;


};







#endif // _BRepFill_ComputeCLine_HeaderFile
