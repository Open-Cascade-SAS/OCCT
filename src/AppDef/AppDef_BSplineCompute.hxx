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

#ifndef _AppDef_BSplineCompute_HeaderFile
#define _AppDef_BSplineCompute_HeaderFile

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
class AppDef_MultiLine;
class AppDef_MyLineTool;
class AppDef_MyBSplGradientOfBSplineCompute;
class AppDef_BSpParLeastSquareOfMyBSplGradientOfBSplineCompute;
class AppDef_BSpParFunctionOfMyBSplGradientOfBSplineCompute;
class AppDef_BSpGradient_BFGSOfMyBSplGradientOfBSplineCompute;
class AppDef_MyGradientbisOfBSplineCompute;
class AppDef_ParLeastSquareOfMyGradientbisOfBSplineCompute;
class AppDef_ResConstraintOfMyGradientbisOfBSplineCompute;
class AppDef_ParFunctionOfMyGradientbisOfBSplineCompute;
class AppDef_Gradient_BFGSOfMyGradientbisOfBSplineCompute;
class AppParCurves_MultiBSpCurve;
class AppParCurves_MultiCurve;



class AppDef_BSplineCompute 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  //!
  //! The multiplicities of the internal knots is set by
  //! default.
  Standard_EXPORT AppDef_BSplineCompute(const AppDef_MultiLine& Line, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-3, const Standard_Real Tolerance2d = 1.0e-6, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  Standard_EXPORT AppDef_BSplineCompute(const AppDef_MultiLine& Line, const math_Vector& Parameters, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Standard_Boolean Squares = Standard_False);
  
  //! Initializes the fields of the algorithm.
  Standard_EXPORT AppDef_BSplineCompute(const math_Vector& Parameters, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Standard_Boolean Squares = Standard_False);
  
  //! Initializes the fields of the algorithm.
  Standard_EXPORT AppDef_BSplineCompute(const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  //! Constructs an interpolation of the MultiLine <Line>
  //! The result will be a C2 curve of degree 3.
  Standard_EXPORT void Interpol (const AppDef_MultiLine& Line);
  
  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Init (const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  //! runs the algorithm after having initialized the fields.
  Standard_EXPORT void Perform (const AppDef_MultiLine& Line);
  
  //! The approximation will begin with the
  //! set of  parameters <ThePar>.
  Standard_EXPORT void SetParameters (const math_Vector& ThePar);
  
  //! The approximation will be done with the
  //! set of knots <Knots>. The multiplicities will be set
  //! with the degree and the desired continuity.
  Standard_EXPORT void SetKnots (const TColStd_Array1OfReal& Knots);
  
  //! The approximation will be done with the
  //! set of knots <Knots> and the multiplicities <Mults>.
  Standard_EXPORT void SetKnotsAndMultiplicities (const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults);
  
  //! changes the degrees of the approximation.
  Standard_EXPORT void SetDegrees (const Standard_Integer degreemin, const Standard_Integer degreemax);
  
  //! Changes the tolerances of the approximation.
  Standard_EXPORT void SetTolerances (const Standard_Real Tolerance3d, const Standard_Real Tolerance2d);
  
  //! sets the continuity of the spline.
  //! if C = 2, the spline will be C2.
  Standard_EXPORT void SetContinuity (const Standard_Integer C);
  
  //! changes the first and the last constraint points.
  Standard_EXPORT void SetConstraints (const AppParCurves_Constraint firstC, const AppParCurves_Constraint lastC);
  
  //! returns False if at a moment of the approximation,
  //! the status NoApproximation has been sent by the user
  //! when more points were needed.
  Standard_EXPORT Standard_Boolean IsAllApproximated() const;
  
  //! returns False if the status NoPointsAdded has been sent.
  Standard_EXPORT Standard_Boolean IsToleranceReached() const;
  
  //! returns the tolerances 2d and 3d of the MultiBSpCurve.
  Standard_EXPORT void Error (Standard_Real& tol3d, Standard_Real& tol2d) const;
  
  //! returns the result of the approximation.
  Standard_EXPORT const AppParCurves_MultiBSpCurve& Value() const;
  
  //! returns the result of the approximation.
  Standard_EXPORT AppParCurves_MultiBSpCurve& ChangeValue();
  
  //! returns the new parameters of the approximation
  //! corresponding to the points of the MultiBSpCurve.
  Standard_EXPORT const TColStd_Array1OfReal& Parameters() const;




protected:





private:

  
  //! is internally used in the algorithm.
  Standard_EXPORT Standard_Boolean Compute (const AppDef_MultiLine& Line, const Standard_Integer fpt, const Standard_Integer lpt, math_Vector& Para, const TColStd_Array1OfReal& Knots, TColStd_Array1OfInteger& Mults);
  
  //! is internally used in the algorithm.
  Standard_EXPORT Standard_Boolean ComputeCurve (const AppDef_MultiLine& Line, const Standard_Integer firspt, const Standard_Integer lastpt);
  
  //! computes new parameters between firstP and lastP.
  Standard_EXPORT void Parameters (const AppDef_MultiLine& Line, const Standard_Integer firstP, const Standard_Integer LastP, math_Vector& TheParameters) const;
  
  Standard_EXPORT Standard_Real SearchFirstLambda (const AppDef_MultiLine& Line, const math_Vector& Para, const TColStd_Array1OfReal& Knots, const math_Vector& V, const Standard_Integer index) const;
  
  Standard_EXPORT Standard_Real SearchLastLambda (const AppDef_MultiLine& Line, const math_Vector& Para, const TColStd_Array1OfReal& Knots, const math_Vector& V, const Standard_Integer index) const;
  
  Standard_EXPORT void TangencyVector (const AppDef_MultiLine& Line, const AppParCurves_MultiCurve& C, const Standard_Real U, math_Vector& V) const;
  
  Standard_EXPORT void FirstTangencyVector (const AppDef_MultiLine& Line, const Standard_Integer index, math_Vector& V) const;
  
  Standard_EXPORT void LastTangencyVector (const AppDef_MultiLine& Line, const Standard_Integer index, math_Vector& V) const;
  
  Standard_EXPORT void FindRealConstraints (const AppDef_MultiLine& Line);


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







#endif // _AppDef_BSplineCompute_HeaderFile
