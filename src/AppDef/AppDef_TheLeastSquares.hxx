// Created on: 1991-12-02
// Created by: Laurent PAINNOT
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

#ifndef _AppDef_TheLeastSquares_HeaderFile
#define _AppDef_TheLeastSquares_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_Constraint.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_DimensionError;
class Standard_NoSuchObject;
class AppDef_MultiLine;
class AppDef_MyLineTool;
class AppParCurves_MultiCurve;
class AppParCurves_MultiBSpCurve;
class math_Matrix;



class AppDef_TheLeastSquares 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine& SSP, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const AppParCurves_Constraint FirstCons, const AppParCurves_Constraint LastCons, const math_Vector& Parameters, const Standard_Integer NbPol);
  
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine& SSP, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const AppParCurves_Constraint FirstCons, const AppParCurves_Constraint LastCons, const Standard_Integer NbPol);
  
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine& SSP, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const AppParCurves_Constraint FirstCons, const AppParCurves_Constraint LastCons, const math_Vector& Parameters, const Standard_Integer NbPol);
  
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine& SSP, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const AppParCurves_Constraint FirstCons, const AppParCurves_Constraint LastCons, const Standard_Integer NbPol);
  
  Standard_EXPORT void Perform (const math_Vector& Parameters);
  
  Standard_EXPORT void Perform (const math_Vector& Parameters, const Standard_Real l1, const Standard_Real l2);
  
  Standard_EXPORT void Perform (const math_Vector& Parameters, const math_Vector& V1t, const math_Vector& V2t, const Standard_Real l1, const Standard_Real l2);
  
  Standard_EXPORT void Perform (const math_Vector& Parameters, const math_Vector& V1t, const math_Vector& V2t, const math_Vector& V1c, const math_Vector& V2c, const Standard_Real l1, const Standard_Real l2);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT AppParCurves_MultiCurve BezierValue();
  
  Standard_EXPORT const AppParCurves_MultiBSpCurve& BSplineValue();
  
  Standard_EXPORT const math_Matrix& FunctionMatrix() const;
  
  Standard_EXPORT const math_Matrix& DerivativeFunctionMatrix() const;
  
  Standard_EXPORT void ErrorGradient (math_Vector& Grad, Standard_Real& F, Standard_Real& MaxE3d, Standard_Real& MaxE2d);
  
  Standard_EXPORT const math_Matrix& Distance();
  
  Standard_EXPORT void Error (Standard_Real& F, Standard_Real& MaxE3d, Standard_Real& MaxE2d);
  
  Standard_EXPORT Standard_Real FirstLambda() const;
  
  Standard_EXPORT Standard_Real LastLambda() const;
  
  Standard_EXPORT const math_Matrix& Points() const;
  
  Standard_EXPORT const math_Matrix& Poles() const;
  
  Standard_EXPORT const math_IntegerVector& KIndex() const;




protected:

  
  Standard_EXPORT void Init (const AppDef_MultiLine& SSP, const Standard_Integer FirstPoint, const Standard_Integer LastPoint);
  
  Standard_EXPORT Standard_Integer NbBColumns (const AppDef_MultiLine& SSP) const;
  
  Standard_EXPORT Standard_Integer TheFirstPoint (const AppParCurves_Constraint FirstCons, const Standard_Integer FirstPoint) const;
  
  Standard_EXPORT Standard_Integer TheLastPoint (const AppParCurves_Constraint LastCons, const Standard_Integer LastPoint) const;
  
  Standard_EXPORT void Affect (const AppDef_MultiLine& SSP, const Standard_Integer Index, AppParCurves_Constraint& Cons, math_Vector& Vt, math_Vector& Vc);
  
  Standard_EXPORT void ComputeFunction (const math_Vector& Parameters);
  
  Standard_EXPORT void SearchIndex (math_IntegerVector& Index);
  
  Standard_EXPORT void MakeTAA (math_Vector& TheA, math_Vector& TheB);
  
  Standard_EXPORT void MakeTAA (math_Vector& TheA);
  
  Standard_EXPORT void MakeTAA (math_Vector& TheA, math_Matrix& TheB);




private:



  AppParCurves_Constraint FirstConstraint;
  AppParCurves_Constraint LastConstraint;
  AppParCurves_MultiBSpCurve SCU;
  Handle(TColStd_HArray1OfReal) myknots;
  Handle(TColStd_HArray1OfInteger) mymults;
  math_Matrix mypoles;
  math_Matrix A;
  math_Matrix DA;
  math_Matrix B2;
  math_Matrix mypoints;
  math_Vector Vflatknots;
  math_Vector Vec1t;
  math_Vector Vec1c;
  math_Vector Vec2t;
  math_Vector Vec2c;
  math_Matrix theError;
  math_IntegerVector myindex;
  Standard_Real lambda1;
  Standard_Real lambda2;
  Standard_Integer FirstP;
  Standard_Integer LastP;
  Standard_Integer Nlignes;
  Standard_Integer Ninc;
  Standard_Integer NA;
  Standard_Integer myfirstp;
  Standard_Integer mylastp;
  Standard_Integer resinit;
  Standard_Integer resfin;
  Standard_Integer nbP2d;
  Standard_Integer nbP;
  Standard_Integer nbpoles;
  Standard_Integer deg;
  Standard_Boolean done;
  Standard_Boolean iscalculated;
  Standard_Boolean isready;


};







#endif // _AppDef_TheLeastSquares_HeaderFile
