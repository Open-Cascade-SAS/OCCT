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
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Real.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Network.hxx>
#include <AdvApp2Var_Framework.hxx>
#include <Standard_Boolean.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>
#include <Standard_OStream.hxx>
class AdvApprox_Cutting;
class AdvApp2Var_Criterion;
class Geom_BSplineSurface;

//! Perform the approximation of <Func> F(U,V)
//! Arguments are :
//! Num1DSS, Num2DSS, Num3DSS :The numbers of 1,2,3 dimensional subspaces
//! OneDTol, TwoDTol, ThreeDTol: The tolerance of approximation in each
//! subspaces
//! OneDTolFr, TwoDTolFr, ThreeDTolFr: The tolerance of approximation on
//! the boundaries in each subspaces
//! [FirstInU, LastInU]: The Bounds in U of the Approximation
//! [FirstInV, LastInV]: The Bounds in V of the Approximation
//! FavorIso : Give preference to extract u-iso or v-iso on F(U,V)
//! This can be useful to optimize the <Func> method
//! ContInU, ContInV : Continuity waiting in u and v
//! PrecisCode : Precision on approximation's error measurement
//! 1 : Fast computation and average precision
//! 2 : Average computation and good precision
//! 3 : Slow computation and very good precision
//! MaxDegInU : Maximum u-degree waiting in U
//! MaxDegInV : Maximum u-degree waiting in V
//! Warning:
//! MaxDegInU (resp. MaxDegInV) must be >= 2*iu (resp. iv) + 1,
//! where iu (resp. iv) = 0 if ContInU (resp. ContInV) = GeomAbs_C0,
//! = 1 if = GeomAbs_C1,
//! = 2 if = GeomAbs_C2.
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

  Standard_EXPORT AdvApp2Var_ApproxAFunc2Var(const int               Num1DSS,
                                             const int               Num2DSS,
                                             const int               Num3DSS,
                                             const occ::handle<NCollection_HArray1<double>>& OneDTol,
                                             const occ::handle<NCollection_HArray1<double>>& TwoDTol,
                                             const occ::handle<NCollection_HArray1<double>>& ThreeDTol,
                                             const occ::handle<NCollection_HArray2<double>>& OneDTolFr,
                                             const occ::handle<NCollection_HArray2<double>>& TwoDTolFr,
                                             const occ::handle<NCollection_HArray2<double>>& ThreeDTolFr,
                                             const double                  FirstInU,
                                             const double                  LastInU,
                                             const double                  FirstInV,
                                             const double                  LastInV,
                                             const GeomAbs_IsoType                FavorIso,
                                             const GeomAbs_Shape                  ContInU,
                                             const GeomAbs_Shape                  ContInV,
                                             const int               PrecisCode,
                                             const int               MaxDegInU,
                                             const int               MaxDegInV,
                                             const int               MaxPatch,
                                             const AdvApp2Var_EvaluatorFunc2Var&  Func,
                                             AdvApprox_Cutting&                   UChoice,
                                             AdvApprox_Cutting&                   VChoice);

  Standard_EXPORT AdvApp2Var_ApproxAFunc2Var(const int               Num1DSS,
                                             const int               Num2DSS,
                                             const int               Num3DSS,
                                             const occ::handle<NCollection_HArray1<double>>& OneDTol,
                                             const occ::handle<NCollection_HArray1<double>>& TwoDTol,
                                             const occ::handle<NCollection_HArray1<double>>& ThreeDTol,
                                             const occ::handle<NCollection_HArray2<double>>& OneDTolFr,
                                             const occ::handle<NCollection_HArray2<double>>& TwoDTolFr,
                                             const occ::handle<NCollection_HArray2<double>>& ThreeDTolFr,
                                             const double                  FirstInU,
                                             const double                  LastInU,
                                             const double                  FirstInV,
                                             const double                  LastInV,
                                             const GeomAbs_IsoType                FavorIso,
                                             const GeomAbs_Shape                  ContInU,
                                             const GeomAbs_Shape                  ContInV,
                                             const int               PrecisCode,
                                             const int               MaxDegInU,
                                             const int               MaxDegInV,
                                             const int               MaxPatch,
                                             const AdvApp2Var_EvaluatorFunc2Var&  Func,
                                             const AdvApp2Var_Criterion&          Crit,
                                             AdvApprox_Cutting&                   UChoice,
                                             AdvApprox_Cutting&                   VChoice);

  //! True if the approximation succeeded within the imposed
  //! tolerances and the wished continuities
  bool IsDone() const;

  //! True if the approximation did come out with a result that
  //! is not NECESSARELY within the required tolerance or a result
  //! that is not recognized with the wished continuities
  bool HasResult() const;

  //! returns the BSplineSurface of range Index
  occ::handle<Geom_BSplineSurface> Surface(const int Index) const;

  int UDegree() const;

  int VDegree() const;

  int NumSubSpaces(const int Dimension) const;

  //! returns the errors max
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> MaxError(const int Dimension) const;

  //! returns the average errors
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> AverageError(
    const int Dimension) const;

  //! returns the errors max on UFrontiers
  //! Warning:
  //! Dimension must be equal to 3.
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> UFrontError(const int Dimension) const;

  //! returns the errors max on VFrontiers
  //! Warning:
  //! Dimension must be equal to 3.
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> VFrontError(const int Dimension) const;

  //! returns the error max of the BSplineSurface of range Index
  Standard_EXPORT double MaxError(const int Dimension,
                                         const int Index) const;

  //! returns the average error of the BSplineSurface of range Index
  Standard_EXPORT double AverageError(const int Dimension,
                                             const int Index) const;

  //! returns the error max of the BSplineSurface of range Index on a UFrontier
  Standard_EXPORT double UFrontError(const int Dimension,
                                            const int Index) const;

  //! returns the error max of the BSplineSurface of range Index on a VFrontier
  Standard_EXPORT double VFrontError(const int Dimension,
                                            const int Index) const;

  Standard_EXPORT double CritError(const int Dimension,
                                          const int Index) const;

  //! Prints on the stream 'o' information on the current state
  //! of the object.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  //! Initialisation of the approximation ; used by Create
  Standard_EXPORT void Init();

  //! Initialisation of the approximation with a grid of regular cuttings ;
  //! used by Init and Perform
  Standard_EXPORT void InitGrid(const int NbInt);

  //! Computation of the approximation result ; used by Create
  Standard_EXPORT void Perform(const AdvApprox_Cutting&            UChoice,
                               const AdvApprox_Cutting&            VChoice,
                               const AdvApp2Var_EvaluatorFunc2Var& Func);

  //! Computation of the approximation result ; used by Create
  Standard_EXPORT void Perform(const AdvApprox_Cutting&            UChoice,
                               const AdvApprox_Cutting&            VChoice,
                               const AdvApp2Var_EvaluatorFunc2Var& Func,
                               const AdvApp2Var_Criterion&         Crit);

  //! Computation of the polynomial approximations ; used by Perform
  Standard_EXPORT void ComputePatches(const AdvApprox_Cutting&            UChoice,
                                      const AdvApprox_Cutting&            VChoice,
                                      const AdvApp2Var_EvaluatorFunc2Var& Func);

  //! Computation of the polynomial approximations ; used by Perform
  Standard_EXPORT void ComputePatches(const AdvApprox_Cutting&            UChoice,
                                      const AdvApprox_Cutting&            VChoice,
                                      const AdvApp2Var_EvaluatorFunc2Var& Func,
                                      const AdvApp2Var_Criterion&         Crit);

  //! Approximation of the constraints ; used by ComputePatches
  Standard_EXPORT void ComputeConstraints(const AdvApprox_Cutting&            UChoice,
                                          const AdvApprox_Cutting&            VChoice,
                                          const AdvApp2Var_EvaluatorFunc2Var& Func);

  //! Approximation of the constraints ; used by ComputePatches
  Standard_EXPORT void ComputeConstraints(const AdvApprox_Cutting&            UChoice,
                                          const AdvApprox_Cutting&            VChoice,
                                          const AdvApp2Var_EvaluatorFunc2Var& Func,
                                          const AdvApp2Var_Criterion&         Crit);

  //! Computation of the 3D errors on the approximation result ; used by Perform
  Standard_EXPORT void Compute3DErrors();

  //! Computation of the max value of the criterion on the approximation result ;
  //! used by Perform
  Standard_EXPORT void ComputeCritError();

  //! Conversion of the approximation result in BSpline; used by Create
  Standard_EXPORT void ConvertBS();

  int                  myNumSubSpaces[3];
  occ::handle<NCollection_HArray1<double>>     my1DTolerances;
  occ::handle<NCollection_HArray1<double>>     my2DTolerances;
  occ::handle<NCollection_HArray1<double>>     my3DTolerances;
  occ::handle<NCollection_HArray2<double>>     my1DTolOnFront;
  occ::handle<NCollection_HArray2<double>>     my2DTolOnFront;
  occ::handle<NCollection_HArray2<double>>     my3DTolOnFront;
  double                     myFirstParInU;
  double                     myLastParInU;
  double                     myFirstParInV;
  double                     myLastParInV;
  GeomAbs_IsoType                   myFavoriteIso;
  GeomAbs_Shape                     myContInU;
  GeomAbs_Shape                     myContInV;
  int                  myPrecisionCode;
  int                  myMaxDegInU;
  int                  myMaxDegInV;
  int                  myMaxPatches;
  AdvApp2Var_Context                myConditions;
  AdvApp2Var_Network                myResult;
  AdvApp2Var_Framework              myConstraints;
  bool                  myDone;
  bool                  myHasResult;
  occ::handle<NCollection_HArray1<occ::handle<Geom_Surface>>> mySurfaces;
  int                  myDegreeInU;
  int                  myDegreeInV;
  occ::handle<NCollection_HArray1<double>>     my1DMaxError;
  occ::handle<NCollection_HArray1<double>>     my1DAverageError;
  occ::handle<NCollection_HArray1<double>>     my1DUFrontError;
  occ::handle<NCollection_HArray1<double>>     my1DVFrontError;
  occ::handle<NCollection_HArray1<double>>     my2DMaxError;
  occ::handle<NCollection_HArray1<double>>     my2DAverageError;
  occ::handle<NCollection_HArray1<double>>     my2DUFrontError;
  occ::handle<NCollection_HArray1<double>>     my2DVFrontError;
  occ::handle<NCollection_HArray1<double>>     my3DMaxError;
  occ::handle<NCollection_HArray1<double>>     my3DAverageError;
  occ::handle<NCollection_HArray1<double>>     my3DUFrontError;
  occ::handle<NCollection_HArray1<double>>     my3DVFrontError;
  double                     myCriterionError;
};

#include <AdvApp2Var_ApproxAFunc2Var.lxx>

#endif // _AdvApp2Var_ApproxAFunc2Var_HeaderFile
