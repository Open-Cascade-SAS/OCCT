// Created on: 1996-02-14
// Created by: Joelle CHAUVET
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

#ifndef _AdvApp2Var_ApproxAFunc2Var_HeaderFile
#define _AdvApp2Var_ApproxAFunc2Var_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_Shape.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Network.hxx>
#include <AdvApp2Var_Framework.hxx>
#include <Standard_Boolean.hxx>
#include <TColGeom_HArray1OfSurface.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>
#include <Standard_OStream.hxx>
class AdvApprox_Cutting;
class AdvApp2Var_Criterion;
class Geom_BSplineSurface;


//! Perform   the  approximation of  <Func>     F(U,V)
//! Arguments are :
//! Num1DSS, Num2DSS, Num3DSS :The numbers of 1,2,3 dimensional subspaces
//! OneDTol, TwoDTol, ThreeDTol: The tolerance of approximation in each
//! subspaces
//! OneDTolFr, TwoDTolFr, ThreeDTolFr: The tolerance of approximation on
//! the boundarys in each subspaces
//! [FirstInU, LastInU]: The Bounds in U of the Approximation
//! [FirstInV, LastInV]: The Bounds in V of the Approximation
//! FavorIso : Give preference to extract u-iso or v-iso on F(U,V)
//! This can be useful to optimize the <Func> method
//! ContInU, ContInV : Continuity waiting in u and v
//! PrecisCode : Precision on approximation's error mesurement
//! 1 : Fast computation and average precision
//! 2 : Average computation and good precision
//! 3 : Slow computation and very good precision
//! MaxDegInU : Maximum u-degree waiting in U
//! MaxDegInV : Maximum u-degree waiting in V
//! Warning:
//! MaxDegInU (resp. MaxDegInV) must be >= 2*iu (resp. iv) + 1,
//! where iu (resp. iv) = 0 if ContInU (resp. ContInV)  = GeomAbs_C0,
//! = 1 if                          = GeomAbs_C1,
//! = 2 if                          = GeomAbs_C2.
//! MaxPatch  : Maximum number of Patch waiting
//! number of Patch is number of u span * number of v span
//! Func      : The external method to evaluate F(U,V)
//! Crit      : To (re)defined condition of convergence
//! UChoice, VChoice : To define the way in U (or V) Knot insertion
//! Warning:
//! for the moment, the result is a 3D Surface
//! so Num1DSS and Num2DSS must be equals to 0
//! and Num3DSS must be equal to 1.
//! Warning:
//! the Function of type EvaluatorFunc2Var from Approx
//! must be a subclass of AdvApp2Var_EvaluatorFunc2Var
//!
//! the result should be formatted in the following way :
//! <--Num1DSS--> <--2 * Num2DSS--> <--3 * Num3DSS-->
//! R[0,0] ....   R[Num1DSS,0].....  R[Dimension-1,0] for the 1st parameter
//! R[0,i] ....   R[Num1DSS,i].....  R[Dimension-1,i] for the ith parameter
//! R[0,N-1] .... R[Num1DSS,N-1].... R[Dimension-1,N-1] for the Nth parameter
//!
//! the order in which each Subspace appears should be consistent
//! with the tolerances given in the create function and the
//! results will be given in that order as well that is :
//! Surface(n) will correspond to the nth entry described by Num3DSS
class AdvApp2Var_ApproxAFunc2Var 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AdvApp2Var_ApproxAFunc2Var(const Standard_Integer Num1DSS, const Standard_Integer Num2DSS, const Standard_Integer Num3DSS, const Handle(TColStd_HArray1OfReal)& OneDTol, const Handle(TColStd_HArray1OfReal)& TwoDTol, const Handle(TColStd_HArray1OfReal)& ThreeDTol, const Handle(TColStd_HArray2OfReal)& OneDTolFr, const Handle(TColStd_HArray2OfReal)& TwoDTolFr, const Handle(TColStd_HArray2OfReal)& ThreeDTolFr, const Standard_Real FirstInU, const Standard_Real LastInU, const Standard_Real FirstInV, const Standard_Real LastInV, const GeomAbs_IsoType FavorIso, const GeomAbs_Shape ContInU, const GeomAbs_Shape ContInV, const Standard_Integer PrecisCode, const Standard_Integer MaxDegInU, const Standard_Integer MaxDegInV, const Standard_Integer MaxPatch, const AdvApp2Var_EvaluatorFunc2Var& Func, AdvApprox_Cutting& UChoice, AdvApprox_Cutting& VChoice);
  
  Standard_EXPORT AdvApp2Var_ApproxAFunc2Var(const Standard_Integer Num1DSS, const Standard_Integer Num2DSS, const Standard_Integer Num3DSS, const Handle(TColStd_HArray1OfReal)& OneDTol, const Handle(TColStd_HArray1OfReal)& TwoDTol, const Handle(TColStd_HArray1OfReal)& ThreeDTol, const Handle(TColStd_HArray2OfReal)& OneDTolFr, const Handle(TColStd_HArray2OfReal)& TwoDTolFr, const Handle(TColStd_HArray2OfReal)& ThreeDTolFr, const Standard_Real FirstInU, const Standard_Real LastInU, const Standard_Real FirstInV, const Standard_Real LastInV, const GeomAbs_IsoType FavorIso, const GeomAbs_Shape ContInU, const GeomAbs_Shape ContInV, const Standard_Integer PrecisCode, const Standard_Integer MaxDegInU, const Standard_Integer MaxDegInV, const Standard_Integer MaxPatch, const AdvApp2Var_EvaluatorFunc2Var& Func, const AdvApp2Var_Criterion& Crit, AdvApprox_Cutting& UChoice, AdvApprox_Cutting& VChoice);
  
  //! True if the approximation succeeded within the imposed
  //! tolerances and the wished continuities
    Standard_Boolean IsDone() const;
  
  //! True if the approximation did come out with a result that
  //! is not NECESSARELY within the required tolerance or a result
  //! that is not recognized with the wished continuities
    Standard_Boolean HasResult() const;
  
  //! returns the BSplineSurface of range Index
    Handle(Geom_BSplineSurface) Surface (const Standard_Integer Index) const;
  
    Standard_Integer UDegree() const;
  
    Standard_Integer VDegree() const;
  
    Standard_Integer NumSubSpaces (const Standard_Integer Dimension) const;
  
  //! returns the errors max
  Standard_EXPORT Handle(TColStd_HArray1OfReal) MaxError (const Standard_Integer Dimension) const;
  
  //! returns the average errors
  Standard_EXPORT Handle(TColStd_HArray1OfReal) AverageError (const Standard_Integer Dimension) const;
  
  //! returns the errors max on UFrontiers
  //! Warning:
  //! Dimension must be equal to 3.
  Standard_EXPORT Handle(TColStd_HArray1OfReal) UFrontError (const Standard_Integer Dimension) const;
  
  //! returns the errors max on VFrontiers
  //! Warning:
  //! Dimension must be equal to 3.
  Standard_EXPORT Handle(TColStd_HArray1OfReal) VFrontError (const Standard_Integer Dimension) const;
  
  //! returns the error max of the BSplineSurface of range Index
  Standard_EXPORT Standard_Real MaxError (const Standard_Integer Dimension, const Standard_Integer Index) const;
  
  //! returns the average error of the BSplineSurface of range Index
  Standard_EXPORT Standard_Real AverageError (const Standard_Integer Dimension, const Standard_Integer Index) const;
  
  //! returns the error max of the BSplineSurface of range Index on a UFrontier
  Standard_EXPORT Standard_Real UFrontError (const Standard_Integer Dimension, const Standard_Integer Index) const;
  
  //! returns the error max of the BSplineSurface of range Index on a VFrontier
  Standard_EXPORT Standard_Real VFrontError (const Standard_Integer Dimension, const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Real CritError (const Standard_Integer Dimension, const Standard_Integer Index) const;
  
  //! Prints on the stream 'o' information on the current state
  //! of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:

  
  //! Initialisation of the approximation ; used by Create
  Standard_EXPORT void Init();
  
  //! Initialisation of the approximation with a grid of regular cuttings ;
  //! used by Init and Perform
  Standard_EXPORT void InitGrid (const Standard_Integer NbInt);
  
  //! Computation of the approximation result ; used by Create
  Standard_EXPORT void Perform (const AdvApprox_Cutting& UChoice, const AdvApprox_Cutting& VChoice, const AdvApp2Var_EvaluatorFunc2Var& Func);
  
  //! Computation of the approximation result ; used by Create
  Standard_EXPORT void Perform (const AdvApprox_Cutting& UChoice, const AdvApprox_Cutting& VChoice, const AdvApp2Var_EvaluatorFunc2Var& Func, const AdvApp2Var_Criterion& Crit);
  
  //! Computation of the polynomial approximations ; used by Perform
  Standard_EXPORT void ComputePatches (const AdvApprox_Cutting& UChoice, const AdvApprox_Cutting& VChoice, const AdvApp2Var_EvaluatorFunc2Var& Func);
  
  //! Computation of the polynomial approximations ; used by Perform
  Standard_EXPORT void ComputePatches (const AdvApprox_Cutting& UChoice, const AdvApprox_Cutting& VChoice, const AdvApp2Var_EvaluatorFunc2Var& Func, const AdvApp2Var_Criterion& Crit);
  
  //! Approximation of the constraints ; used by ComputePatches
  Standard_EXPORT void ComputeConstraints (const AdvApprox_Cutting& UChoice, const AdvApprox_Cutting& VChoice, const AdvApp2Var_EvaluatorFunc2Var& Func);
  
  //! Approximation of the constraints ; used by ComputePatches
  Standard_EXPORT void ComputeConstraints (const AdvApprox_Cutting& UChoice, const AdvApprox_Cutting& VChoice, const AdvApp2Var_EvaluatorFunc2Var& Func, const AdvApp2Var_Criterion& Crit);
  
  //! Computation of the 3D errors on the approximation result ; used by Perform
  Standard_EXPORT void Compute3DErrors();
  
  //! Computation of the max value of the criterion on the approximation result ;
  //! used by Perform
  Standard_EXPORT void ComputeCritError();
  
  //! Conversion of the approximation result in BSpline; used by Create
  Standard_EXPORT void ConvertBS();


  Standard_Integer myNumSubSpaces[3];
  Handle(TColStd_HArray1OfReal) my1DTolerances;
  Handle(TColStd_HArray1OfReal) my2DTolerances;
  Handle(TColStd_HArray1OfReal) my3DTolerances;
  Handle(TColStd_HArray2OfReal) my1DTolOnFront;
  Handle(TColStd_HArray2OfReal) my2DTolOnFront;
  Handle(TColStd_HArray2OfReal) my3DTolOnFront;
  Standard_Real myFirstParInU;
  Standard_Real myLastParInU;
  Standard_Real myFirstParInV;
  Standard_Real myLastParInV;
  GeomAbs_IsoType myFavoriteIso;
  GeomAbs_Shape myContInU;
  GeomAbs_Shape myContInV;
  Standard_Integer myPrecisionCode;
  Standard_Integer myMaxDegInU;
  Standard_Integer myMaxDegInV;
  Standard_Integer myMaxPatches;
  AdvApp2Var_Context myConditions;
  AdvApp2Var_Network myResult;
  AdvApp2Var_Framework myConstraints;
  Standard_Boolean myDone;
  Standard_Boolean myHasResult;
  Handle(TColGeom_HArray1OfSurface) mySurfaces;
  Standard_Integer myDegreeInU;
  Standard_Integer myDegreeInV;
  Handle(TColStd_HArray1OfReal) my1DMaxError;
  Handle(TColStd_HArray1OfReal) my1DAverageError;
  Handle(TColStd_HArray1OfReal) my1DUFrontError;
  Handle(TColStd_HArray1OfReal) my1DVFrontError;
  Handle(TColStd_HArray1OfReal) my2DMaxError;
  Handle(TColStd_HArray1OfReal) my2DAverageError;
  Handle(TColStd_HArray1OfReal) my2DUFrontError;
  Handle(TColStd_HArray1OfReal) my2DVFrontError;
  Handle(TColStd_HArray1OfReal) my3DMaxError;
  Handle(TColStd_HArray1OfReal) my3DAverageError;
  Handle(TColStd_HArray1OfReal) my3DUFrontError;
  Handle(TColStd_HArray1OfReal) my3DVFrontError;
  Standard_Real myCriterionError;


};


#include <AdvApp2Var_ApproxAFunc2Var.lxx>





#endif // _AdvApp2Var_ApproxAFunc2Var_HeaderFile
