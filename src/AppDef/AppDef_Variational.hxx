// Created on: 1996-05-14
// Created by: Philippe MANGIN / Jeannine PANCIATICI
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AppDef_Variational_HeaderFile
#define _AppDef_Variational_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppDef_MultiLine.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <GeomAbs_Shape.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <Standard_OStream.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <math_Vector.hxx>
#include <AppParCurves_Constraint.hxx>
class AppDef_SmoothCriterion;
class math_Matrix;
class FEmTool_Curve;
class FEmTool_Assembly;
class PLib_Base;


//! This class is used to smooth N points with constraints
//! by   minimization  of quadratic  criterium   but  also
//! variational criterium in order to obtain " fair Curve "
//! Computes the approximation of a Multiline by
//! Variational optimization.
class AppDef_Variational 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructor.
  //! Initialization of   the   fields.
  //! warning :  Nc0 : number of PassagePoint consraints
  //! Nc2 : number  of  TangencyPoint constraints
  //! Nc3 : number of    CurvaturePoint   constraints
  //! if
  //! ((MaxDegree-Continuity)*MaxSegment -Nc0  - 2*Nc1
  //! -3*Nc2)
  //! is  negative
  //! The problem is over-constrained.
  //!
  //! Limitation : The MultiLine from AppDef has to be composed by
  //! only one Line ( Dimension 2 or 3).
  Standard_EXPORT AppDef_Variational(const AppDef_MultiLine& SSP, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints, const Standard_Integer MaxDegree = 14, const Standard_Integer MaxSegment = 100, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Boolean WithMinMax = Standard_False, const Standard_Boolean WithCutting = Standard_True, const Standard_Real Tolerance = 1.0, const Standard_Integer NbIterations = 2);
  
  //! Makes the approximation with the current fields.
  Standard_EXPORT void Approximate();
  
  //! returns True if the creation is done
  //! and correspond  to the current fields.
  Standard_EXPORT Standard_Boolean IsCreated() const;
  
  //! returns True if the  approximation is ok
  //! and correspond  to the current fields.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns True if the problem is overconstrained
  //! in this case, approximation cannot be done.
  Standard_EXPORT Standard_Boolean IsOverConstrained() const;
  
  //! returns all the BSpline curves approximating the
  //! MultiLine from AppDef SSP after minimization of the parameter.
  Standard_EXPORT AppParCurves_MultiBSpCurve Value() const;
  
  //! returns the maximum of the distances between
  //! the points of the multiline and the approximation
  //! curves.
  Standard_EXPORT Standard_Real MaxError() const;
  
  //! returns the index of the MultiPoint of ErrorMax
  Standard_EXPORT Standard_Integer MaxErrorIndex() const;
  
  //! returns the quadratic average of the distances between
  //! the points of the multiline and the approximation
  //! curves.
  Standard_EXPORT Standard_Real QuadraticError() const;
  
  //! returns the distances between the points of the
  //! multiline and the approximation curves.
  Standard_EXPORT void Distance (math_Matrix& mat);
  
  //! returns the average error between
  //! the MultiLine from AppDef and the approximation.
  Standard_EXPORT Standard_Real AverageError() const;
  
  //! returns the parameters uses to the approximations
  Standard_EXPORT const Handle(TColStd_HArray1OfReal)& Parameters() const;
  
  //! returns the knots uses to the approximations
  Standard_EXPORT const Handle(TColStd_HArray1OfReal)& Knots() const;
  
  //! returns the values of the quality criterium.
  Standard_EXPORT void Criterium (Standard_Real& VFirstOrder, Standard_Real& VSecondOrder, Standard_Real& VThirdOrder) const;
  
  //! returns the Weights (as percent) associed  to the criterium used in
  //! the  optimization.
  Standard_EXPORT void CriteriumWeight (Standard_Real& Percent1, Standard_Real& Percent2, Standard_Real& Percent3) const;
  
  //! returns the Maximum Degree used in the approximation
  Standard_EXPORT Standard_Integer MaxDegree() const;
  
  //! returns the Maximum of segment used in the approximation
  Standard_EXPORT Standard_Integer MaxSegment() const;
  
  //! returns the Continuity used in the approximation
  Standard_EXPORT GeomAbs_Shape Continuity() const;
  
  //! returns if the  approximation  search to  minimize the
  //! maximum Error or not.
  Standard_EXPORT Standard_Boolean WithMinMax() const;
  
  //! returns if the  approximation can insert new Knots or not.
  Standard_EXPORT Standard_Boolean WithCutting() const;
  
  //! returns the tolerance used in the approximation.
  Standard_EXPORT Standard_Real Tolerance() const;
  
  //! returns the number of iterations used in the approximation.
  Standard_EXPORT Standard_Integer NbIterations() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! MaxError,MaxErrorIndex,AverageError,QuadraticError,Criterium
  //! Distances,Degre,Nombre de poles, parametres, noeuds
  Standard_EXPORT void Dump (Standard_OStream& o) const;
  
  //! Define the constraints to approximate
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT Standard_Boolean SetConstraints (const Handle(AppParCurves_HArray1OfConstraintCouple)& aConstrainst);
  
  //! Defines the parameters used by the approximations.
  Standard_EXPORT void SetParameters (const Handle(TColStd_HArray1OfReal)& param);
  
  //! Defines the knots used by the approximations
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT Standard_Boolean SetKnots (const Handle(TColStd_HArray1OfReal)& knots);
  
  //! Define the Maximum Degree used in the approximation
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT Standard_Boolean SetMaxDegree (const Standard_Integer Degree);
  
  //! Define the maximum number of segments used in the approximation
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT Standard_Boolean SetMaxSegment (const Standard_Integer NbSegment);
  
  //! Define the Continuity used in the approximation
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT Standard_Boolean SetContinuity (const GeomAbs_Shape C);
  
  //! Define if the  approximation  search to  minimize the
  //! maximum Error or not.
  Standard_EXPORT void SetWithMinMax (const Standard_Boolean MinMax);
  
  //! Define if the  approximation can insert new Knots or not.
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT Standard_Boolean SetWithCutting (const Standard_Boolean Cutting);
  
  //! define the Weights (as percent) associed to the criterium used in
  //! the  optimization.
  //!
  //! if Percent <= 0
  Standard_EXPORT void SetCriteriumWeight (const Standard_Real Percent1, const Standard_Real Percent2, const Standard_Real Percent3);
  
  //! define the  Weight   (as  percent)  associed  to   the
  //! criterium   Order used in   the optimization  : Others
  //! weights are updated.
  //! if Percent < 0
  //! if Order < 1 or Order > 3
  Standard_EXPORT void SetCriteriumWeight (const Standard_Integer Order, const Standard_Real Percent);
  
  //! define the tolerance used in the approximation.
  Standard_EXPORT void SetTolerance (const Standard_Real Tol);
  
  //! define the number of iterations used in the approximation.
  //! if Iter < 1
  Standard_EXPORT void SetNbIterations (const Standard_Integer Iter);




protected:





private:

  
  Standard_EXPORT void TheMotor (Handle(AppDef_SmoothCriterion)& J, const Standard_Real WQuadratic, const Standard_Real WQuality, Handle(FEmTool_Curve)& TheCurve, TColStd_Array1OfReal& Ecarts);
  
  Standard_EXPORT void Adjusting (Handle(AppDef_SmoothCriterion)& J, Standard_Real& WQuadratic, Standard_Real& WQuality, Handle(FEmTool_Curve)& TheCurve, TColStd_Array1OfReal& Ecarts);
  
  Standard_EXPORT void Optimization (Handle(AppDef_SmoothCriterion)& J, FEmTool_Assembly& A, const Standard_Boolean ToAssemble, const Standard_Real EpsDeg, Handle(FEmTool_Curve)& Curve, const TColStd_Array1OfReal& Parameters) const;
  
  Standard_EXPORT void Project (const Handle(FEmTool_Curve)& C, const TColStd_Array1OfReal& Ti, TColStd_Array1OfReal& ProjTi, TColStd_Array1OfReal& Distance, Standard_Integer& NumPoints, Standard_Real& MaxErr, Standard_Real& QuaErr, Standard_Real& AveErr, const Standard_Integer NbIterations = 2) const;
  
  Standard_EXPORT void ACR (Handle(FEmTool_Curve)& Curve, TColStd_Array1OfReal& Ti, const Standard_Integer Decima) const;
  
  Standard_EXPORT void SplitCurve (const Handle(FEmTool_Curve)& InCurve, const TColStd_Array1OfReal& Ti, const Standard_Real CurveTol, Handle(FEmTool_Curve)& OutCurve, Standard_Boolean& iscut) const;
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT void InitSmoothCriterion();
  
  Standard_EXPORT void InitParameters (Standard_Real& Length);
  
  Standard_EXPORT void InitCriterionEstimations (const Standard_Real Length, Standard_Real& J1, Standard_Real& J2, Standard_Real& J3) const;
  
  Standard_EXPORT void EstTangent (const Standard_Integer ipnt, math_Vector& VTang) const;
  
  Standard_EXPORT void EstSecnd (const Standard_Integer ipnt, const math_Vector& VTang1, const math_Vector& VTang2, const Standard_Real Length, math_Vector& VScnd) const;
  
  Standard_EXPORT void InitCutting (const Handle(PLib_Base)& aBase, const Standard_Real CurvTol, Handle(FEmTool_Curve)& aCurve) const;
  
  Standard_EXPORT void AssemblingConstraints (const Handle(FEmTool_Curve)& Curve, const TColStd_Array1OfReal& Parameters, const Standard_Real CBLONG, FEmTool_Assembly& A) const;
  
  Standard_EXPORT Standard_Boolean InitTthetaF (const Standard_Integer ndimen, const AppParCurves_Constraint typcon, const Standard_Integer begin, const Standard_Integer jndex);


  AppDef_MultiLine mySSP;
  Standard_Integer myNbP3d;
  Standard_Integer myNbP2d;
  Standard_Integer myDimension;
  Standard_Integer myFirstPoint;
  Standard_Integer myLastPoint;
  Standard_Integer myNbPoints;
  Handle(TColStd_HArray1OfReal) myTabPoints;
  Handle(AppParCurves_HArray1OfConstraintCouple) myConstraints;
  Standard_Integer myNbConstraints;
  Handle(TColStd_HArray1OfReal) myTabConstraints;
  Standard_Integer myNbPassPoints;
  Standard_Integer myNbTangPoints;
  Standard_Integer myNbCurvPoints;
  Handle(TColStd_HArray1OfInteger) myTypConstraints;
  Handle(TColStd_HArray1OfReal) myTtheta;
  Handle(TColStd_HArray1OfReal) myTfthet;
  Standard_Integer myMaxDegree;
  Standard_Integer myMaxSegment;
  Standard_Integer myNbIterations;
  Standard_Real myTolerance;
  GeomAbs_Shape myContinuity;
  Standard_Integer myNivCont;
  Standard_Boolean myWithMinMax;
  Standard_Boolean myWithCutting;
  Standard_Real myPercent[3];
  Standard_Real myCriterium[4];
  Handle(AppDef_SmoothCriterion) mySmoothCriterion;
  Handle(TColStd_HArray1OfReal) myParameters;
  Handle(TColStd_HArray1OfReal) myKnots;
  AppParCurves_MultiBSpCurve myMBSpCurve;
  Standard_Real myMaxError;
  Standard_Integer myMaxErrorIndex;
  Standard_Real myAverageError;
  Standard_Boolean myIsCreated;
  Standard_Boolean myIsDone;
  Standard_Boolean myIsOverConstr;


};







#endif // _AppDef_Variational_HeaderFile
