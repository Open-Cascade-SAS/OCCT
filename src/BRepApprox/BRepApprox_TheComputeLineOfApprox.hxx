// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepApprox_TheComputeLineOfApprox_HeaderFile
#define _BRepApprox_TheComputeLineOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiBSpCurve.hxx>
#include <Standard_Boolean.hxx>
#include <Approx_ParametrizationType.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <AppParCurves_Constraint.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
class BRepApprox_TheMultiLineOfApprox;
class BRepApprox_TheMultiLineToolOfApprox;
class BRepApprox_MyBSplGradientOfTheComputeLineOfApprox;
class BRepApprox_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfApprox;
class BRepApprox_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfApprox;
class BRepApprox_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfApprox;
class BRepApprox_MyGradientbisOfTheComputeLineOfApprox;
class BRepApprox_ParLeastSquareOfMyGradientbisOfTheComputeLineOfApprox;
class BRepApprox_ResConstraintOfMyGradientbisOfTheComputeLineOfApprox;
class BRepApprox_ParFunctionOfMyGradientbisOfTheComputeLineOfApprox;
class BRepApprox_Gradient_BFGSOfMyGradientbisOfTheComputeLineOfApprox;
class AppParCurves_MultiBSpCurve;
class AppParCurves_MultiCurve;



class BRepApprox_TheComputeLineOfApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepApprox_TheComputeLineOfApprox(const BRepApprox_TheMultiLineOfApprox& Line, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-3, const Standard_Real Tolerance2d = 1.0e-6, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  Standard_EXPORT BRepApprox_TheComputeLineOfApprox(const BRepApprox_TheMultiLineOfApprox& Line, const math_Vector& Parameters, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Standard_Boolean Squares = Standard_False);
  
  Standard_EXPORT BRepApprox_TheComputeLineOfApprox(const math_Vector& Parameters, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Standard_Boolean Squares = Standard_False);
  
  Standard_EXPORT BRepApprox_TheComputeLineOfApprox(const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  Standard_EXPORT void Interpol (const BRepApprox_TheMultiLineOfApprox& Line);
  
  Standard_EXPORT void Init (const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  Standard_EXPORT void Perform (const BRepApprox_TheMultiLineOfApprox& Line);
  
  Standard_EXPORT void SetParameters (const math_Vector& ThePar);
  
  Standard_EXPORT void SetKnots (const TColStd_Array1OfReal& Knots);
  
  Standard_EXPORT void SetKnotsAndMultiplicities (const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults);
  
  Standard_EXPORT void SetDegrees (const Standard_Integer degreemin, const Standard_Integer degreemax);
  
  Standard_EXPORT void SetTolerances (const Standard_Real Tolerance3d, const Standard_Real Tolerance2d);
  
  Standard_EXPORT void SetContinuity (const Standard_Integer C);
  
  Standard_EXPORT void SetConstraints (const AppParCurves_Constraint firstC, const AppParCurves_Constraint lastC);
  
  Standard_EXPORT Standard_Boolean IsAllApproximated() const;
  
  Standard_EXPORT Standard_Boolean IsToleranceReached() const;
  
  Standard_EXPORT void Error (Standard_Real& tol3d, Standard_Real& tol2d) const;
  
  Standard_EXPORT const AppParCurves_MultiBSpCurve& Value() const;
  
  Standard_EXPORT AppParCurves_MultiBSpCurve& ChangeValue();
  
  Standard_EXPORT const TColStd_Array1OfReal& Parameters() const;




protected:





private:

  
  Standard_EXPORT Standard_Boolean Compute (const BRepApprox_TheMultiLineOfApprox& Line, const Standard_Integer fpt, const Standard_Integer lpt, math_Vector& Para, const TColStd_Array1OfReal& Knots, TColStd_Array1OfInteger& Mults);
  
  Standard_EXPORT Standard_Boolean ComputeCurve (const BRepApprox_TheMultiLineOfApprox& Line, const Standard_Integer firspt, const Standard_Integer lastpt);
  
  Standard_EXPORT void Parameters (const BRepApprox_TheMultiLineOfApprox& Line, const Standard_Integer firstP, const Standard_Integer LastP, math_Vector& TheParameters) const;
  
  Standard_EXPORT Standard_Real SearchFirstLambda (const BRepApprox_TheMultiLineOfApprox& Line, const math_Vector& Para, const TColStd_Array1OfReal& Knots, const math_Vector& V, const Standard_Integer index) const;
  
  Standard_EXPORT Standard_Real SearchLastLambda (const BRepApprox_TheMultiLineOfApprox& Line, const math_Vector& Para, const TColStd_Array1OfReal& Knots, const math_Vector& V, const Standard_Integer index) const;
  
  Standard_EXPORT void TangencyVector (const BRepApprox_TheMultiLineOfApprox& Line, const AppParCurves_MultiCurve& C, const Standard_Real U, math_Vector& V) const;
  
  Standard_EXPORT void FirstTangencyVector (const BRepApprox_TheMultiLineOfApprox& Line, const Standard_Integer index, math_Vector& V) const;
  
  Standard_EXPORT void LastTangencyVector (const BRepApprox_TheMultiLineOfApprox& Line, const Standard_Integer index, math_Vector& V) const;
  
  Standard_EXPORT void FindRealConstraints (const BRepApprox_TheMultiLineOfApprox& Line);


  AppParCurves_MultiBSpCurve TheMultiBSpCurve;
  Standard_Boolean alldone;
  Standard_Boolean tolreached;
  Approx_ParametrizationType Par;
  Handle(TColStd_HArray1OfReal) myParameters;
  Handle(TColStd_HArray1OfReal) myfirstParam;
  Handle(TColStd_HArray1OfReal) myknots;
  Handle(TColStd_HArray1OfInteger) mymults;
  Standard_Boolean myhasknots;
  Standard_Boolean myhasmults;
  Handle(AppParCurves_HArray1OfConstraintCouple) myConstraints;
  Standard_Integer mydegremin;
  Standard_Integer mydegremax;
  Standard_Real mytol3d;
  Standard_Real mytol2d;
  Standard_Real currenttol3d;
  Standard_Real currenttol2d;
  Standard_Boolean mycut;
  Standard_Boolean mysquares;
  Standard_Integer myitermax;
  AppParCurves_Constraint myfirstC;
  AppParCurves_Constraint mylastC;
  AppParCurves_Constraint realfirstC;
  AppParCurves_Constraint reallastC;
  Standard_Integer mycont;
  Standard_Real mylambda1;
  Standard_Real mylambda2;


};







#endif // _BRepApprox_TheComputeLineOfApprox_HeaderFile
