// Created on: 1995-05-29
// Created by: Xavier BENVENISTE
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

#ifndef _AdvApprox_ApproxAFunction_HeaderFile
#define _AdvApprox_ApproxAFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColgp_HArray2OfPnt2d.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <AdvApprox_EvaluatorFunction.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_OStream.hxx>
class AdvApprox_Cutting;

//! this approximate a given function
class AdvApprox_ApproxAFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs approximator tool.
  //!
  //! Warning:
  //! the Func should be valid reference to object of type
  //! inherited from class EvaluatorFunction from Approx
  //! with life time longer than that of the approximator tool;
  //!
  //! the result should be formatted in the following way :
  //! <--Num1DSS--> <--2 * Num2DSS--> <--3 * Num3DSS-->
  //! R[0] ....     R[Num1DSS].....                   R[Dimension-1]
  //!
  //! the order in which each Subspace appears should be consistent
  //! with the tolerances given in the create function and the
  //! results will be given in that order as well that is :
  //! Curve2d(n) will correspond to the nth entry
  //! described by Num2DSS, Curve(n) will correspond to
  //! the nth entry described by Num3DSS
  //! The same type of schema applies to the Poles1d, Poles2d and
  //! Poles.
  Standard_EXPORT AdvApprox_ApproxAFunction(const int               Num1DSS,
                                            const int               Num2DSS,
                                            const int               Num3DSS,
                                            const occ::handle<TColStd_HArray1OfReal>& OneDTol,
                                            const occ::handle<TColStd_HArray1OfReal>& TwoDTol,
                                            const occ::handle<TColStd_HArray1OfReal>& ThreeDTol,
                                            const double                  First,
                                            const double                  Last,
                                            const GeomAbs_Shape                  Continuity,
                                            const int               MaxDeg,
                                            const int               MaxSeg,
                                            const AdvApprox_EvaluatorFunction&   Func);

  //! Approximation with user methode of cutting
  Standard_EXPORT AdvApprox_ApproxAFunction(const int               Num1DSS,
                                            const int               Num2DSS,
                                            const int               Num3DSS,
                                            const occ::handle<TColStd_HArray1OfReal>& OneDTol,
                                            const occ::handle<TColStd_HArray1OfReal>& TwoDTol,
                                            const occ::handle<TColStd_HArray1OfReal>& ThreeDTol,
                                            const double                  First,
                                            const double                  Last,
                                            const GeomAbs_Shape                  Continuity,
                                            const int               MaxDeg,
                                            const int               MaxSeg,
                                            const AdvApprox_EvaluatorFunction&   Func,
                                            const AdvApprox_Cutting&             CutTool);

  Standard_EXPORT static void Approximation(const int         TotalDimension,
                                            const int         TotalNumSS,
                                            const NCollection_Array1<int>& LocalDimension,
                                            const double            First,
                                            const double            Last,
                                            AdvApprox_EvaluatorFunction&   Evaluator,
                                            const AdvApprox_Cutting&       CutTool,
                                            const int         ContinuityOrder,
                                            const int         NumMaxCoeffs,
                                            const int         MaxSegments,
                                            const NCollection_Array1<double>&    TolerancesArray,
                                            const int         code_precis,
                                            int&              NumCurves,
                                            NCollection_Array1<int>&       NumCoeffPerCurveArray,
                                            NCollection_Array1<double>&          LocalCoefficientArray,
                                            NCollection_Array1<double>&          IntervalsArray,
                                            NCollection_Array1<double>&          ErrorMaxArray,
                                            NCollection_Array1<double>&          AverageErrorArray,
                                            int&              ErrorCode);

  bool IsDone() const;

  bool HasResult() const;

  //! returns the poles from the algorithms as is
  occ::handle<TColStd_HArray2OfReal> Poles1d() const;

  //! returns the poles from the algorithms as is
  occ::handle<TColgp_HArray2OfPnt2d> Poles2d() const;

  //! -- returns the poles from the algorithms as is
  occ::handle<TColgp_HArray2OfPnt> Poles() const;

  //! as the name says
  Standard_EXPORT int NbPoles() const;

  //! returns the poles at Index from the 1d subspace
  Standard_EXPORT void Poles1d(const int Index, NCollection_Array1<double>& P) const;

  //! returns the poles at Index from the 2d subspace
  Standard_EXPORT void Poles2d(const int Index, NCollection_Array1<gp_Pnt2d>& P) const;

  //! returns the poles at Index from the 3d subspace
  Standard_EXPORT void Poles(const int Index, NCollection_Array1<gp_Pnt>& P) const;

  int Degree() const;

  int NbKnots() const;

  int NumSubSpaces(const int Dimension) const;

  occ::handle<TColStd_HArray1OfReal> Knots() const;

  occ::handle<TColStd_HArray1OfInteger> Multiplicities() const;

  //! returns the error as is in the algorithms
  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> MaxError(const int Dimension) const;

  //! returns the error as is in the algorithms
  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> AverageError(
    const int Dimension) const;

  Standard_EXPORT double MaxError(const int Dimension,
                                         const int Index) const;

  Standard_EXPORT double AverageError(const int Dimension,
                                             const int Index) const;

  //! display information on approximation.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  Standard_EXPORT void Perform(const int   Num1DSS,
                               const int   Num2DSS,
                               const int   Num3DSS,
                               const AdvApprox_Cutting& CutTool);

  int                 myNumSubSpaces[3];
  occ::handle<TColStd_HArray1OfReal>    my1DTolerances;
  occ::handle<TColStd_HArray1OfReal>    my2DTolerances;
  occ::handle<TColStd_HArray1OfReal>    my3DTolerances;
  double                    myFirst;
  double                    myLast;
  GeomAbs_Shape                    myContinuity;
  int                 myMaxDegree;
  int                 myMaxSegments;
  bool                 myDone;
  bool                 myHasResult;
  occ::handle<TColStd_HArray2OfReal>    my1DPoles;
  occ::handle<TColgp_HArray2OfPnt2d>    my2DPoles;
  occ::handle<TColgp_HArray2OfPnt>      my3DPoles;
  occ::handle<TColStd_HArray1OfReal>    myKnots;
  occ::handle<TColStd_HArray1OfInteger> myMults;
  int                 myDegree;
  void*                 myEvaluator;
  occ::handle<TColStd_HArray1OfReal>    my1DMaxError;
  occ::handle<TColStd_HArray1OfReal>    my1DAverageError;
  occ::handle<TColStd_HArray1OfReal>    my2DMaxError;
  occ::handle<TColStd_HArray1OfReal>    my2DAverageError;
  occ::handle<TColStd_HArray1OfReal>    my3DMaxError;
  occ::handle<TColStd_HArray1OfReal>    my3DAverageError;
};

#include <AdvApprox_ApproxAFunction.lxx>

#endif // _AdvApprox_ApproxAFunction_HeaderFile
