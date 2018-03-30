// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#ifndef _GeomInt_TheComputeLineBezierOfWLApprox_HeaderFile
#define _GeomInt_TheComputeLineBezierOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_SequenceOfMultiCurve.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <Standard_Boolean.hxx>
#include <Approx_ParametrizationType.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Approx_SequenceOfHArray1OfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <AppParCurves_Constraint.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>
class GeomInt_TheMultiLineOfWLApprox;
class GeomInt_TheMultiLineToolOfWLApprox;
class GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox;
class GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox;
class GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox;
class GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox;
class GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox;
class AppParCurves_MultiCurve;
class AppParCurves_MultiBSpCurve;



class GeomInt_TheComputeLineBezierOfWLApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  Standard_EXPORT GeomInt_TheComputeLineBezierOfWLApprox(const GeomInt_TheMultiLineOfWLApprox& Line, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-3, const Standard_Real Tolerance2d = 1.0e-6, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  Standard_EXPORT GeomInt_TheComputeLineBezierOfWLApprox(const GeomInt_TheMultiLineOfWLApprox& Line, const math_Vector& Parameters, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Standard_Boolean Squares = Standard_False);
  
  //! Initializes the fields of the algorithm.
  Standard_EXPORT GeomInt_TheComputeLineBezierOfWLApprox(const math_Vector& Parameters, const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Standard_Boolean Squares = Standard_False);
  
  //! Initializes the fields of the algorithm.
  Standard_EXPORT GeomInt_TheComputeLineBezierOfWLApprox(const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Init (const Standard_Integer degreemin = 4, const Standard_Integer degreemax = 8, const Standard_Real Tolerance3d = 1.0e-03, const Standard_Real Tolerance2d = 1.0e-06, const Standard_Integer NbIterations = 5, const Standard_Boolean cutting = Standard_True, const Approx_ParametrizationType parametrization = Approx_ChordLength, const Standard_Boolean Squares = Standard_False);
  
  //! runs the algorithm after having initialized the fields.
  Standard_EXPORT void Perform (const GeomInt_TheMultiLineOfWLApprox& Line);
  
  //! changes the degrees of the approximation.
  Standard_EXPORT void SetDegrees (const Standard_Integer degreemin, const Standard_Integer degreemax);
  
  //! Changes the tolerances of the approximation.
  Standard_EXPORT void SetTolerances (const Standard_Real Tolerance3d, const Standard_Real Tolerance2d);
  
  //! changes the first and the last constraint points.
  Standard_EXPORT void SetConstraints (const AppParCurves_Constraint firstC, const AppParCurves_Constraint lastC);
  
  //! returns False if at a moment of the approximation,
  //! the status NoApproximation has been sent by the user
  //! when more points were needed.
  Standard_EXPORT Standard_Boolean IsAllApproximated() const;
  
  //! returns False if the status NoPointsAdded has been sent.
  Standard_EXPORT Standard_Boolean IsToleranceReached() const;
  
  //! returns the tolerances 2d and 3d of the <Index> MultiCurve.
  Standard_EXPORT void Error (const Standard_Integer Index, Standard_Real& tol3d, Standard_Real& tol2d) const;
  
  //! Returns the number of MultiCurve doing the approximation
  //! of the MultiLine.
  Standard_EXPORT Standard_Integer NbMultiCurves() const;
  
  //! returns the result of the approximation.
  Standard_EXPORT const AppParCurves_MultiCurve& Value (const Standard_Integer Index = 1) const;
  
  //! returns the result of the approximation.
  Standard_EXPORT AppParCurves_MultiCurve& ChangeValue (const Standard_Integer Index = 1);
  
  //! returns the result of the approximation.
  Standard_EXPORT const AppParCurves_MultiBSpCurve& SplineValue();
  
  //! returns the type  of  parametrization
  Standard_EXPORT Approx_ParametrizationType Parametrization () const;

  //! returns the new parameters of the approximation
  //! corresponding to the points of the multicurve <Index>.
  Standard_EXPORT const TColStd_Array1OfReal& Parameters (const Standard_Integer Index = 1) const;




protected:





private:

  
  //! is internally used in the algorithm.
  Standard_EXPORT Standard_Boolean Compute (const GeomInt_TheMultiLineOfWLApprox& Line,
                                            const Standard_Integer fpt,
                                            const Standard_Integer lpt,
                                            math_Vector& Para,
                                            Standard_Real& TheTol3d,
                                            Standard_Real& TheTol2d,
                                            Standard_Integer& indbad);
  
  //! is internally used in the algorithm.
  Standard_EXPORT Standard_Boolean ComputeCurve (const GeomInt_TheMultiLineOfWLApprox& Line, const Standard_Integer firspt, const Standard_Integer lastpt);
  
  //! computes new parameters between firstP and lastP.
  Standard_EXPORT void Parameters (const GeomInt_TheMultiLineOfWLApprox& Line, const Standard_Integer firstP, const Standard_Integer LastP, math_Vector& TheParameters) const;
  
  Standard_EXPORT Standard_Real SearchFirstLambda (const GeomInt_TheMultiLineOfWLApprox& Line, const math_Vector& Para, const math_Vector& V, const Standard_Integer index) const;
  
  Standard_EXPORT Standard_Real SearchLastLambda (const GeomInt_TheMultiLineOfWLApprox& Line, const math_Vector& Para, const math_Vector& V, const Standard_Integer index) const;
  
  Standard_EXPORT void FirstTangencyVector (const GeomInt_TheMultiLineOfWLApprox& Line, const Standard_Integer index, math_Vector& V) const;
  
  Standard_EXPORT void LastTangencyVector (const GeomInt_TheMultiLineOfWLApprox& Line, const Standard_Integer index, math_Vector& V) const;


  AppParCurves_SequenceOfMultiCurve myMultiCurves;
  AppParCurves_MultiCurve TheMultiCurve;
  AppParCurves_MultiBSpCurve myspline;
  Standard_Boolean alldone;
  Standard_Boolean tolreached;
  Approx_ParametrizationType Par;
  Handle(TColStd_HArray1OfReal) myParameters;
  Handle(TColStd_HArray1OfReal) myfirstParam;
  Approx_SequenceOfHArray1OfReal myPar;
  TColStd_SequenceOfReal Tolers3d;
  TColStd_SequenceOfReal Tolers2d;
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
  Standard_Integer myMultiLineNb;
  Standard_Boolean myIsClear;


};







#endif // _GeomInt_TheComputeLineBezierOfWLApprox_HeaderFile
