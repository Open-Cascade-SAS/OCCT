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
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
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
  //! Curve2d(n)  will correspond to the nth entry
  //! described by Num2DSS, Curve(n) will correspond to
  //! the nth entry described by Num3DSS
  //! The same type of schema applies to the Poles1d, Poles2d and
  //! Poles.
  Standard_EXPORT AdvApprox_ApproxAFunction(const Standard_Integer               Num1DSS,
                                            const Standard_Integer               Num2DSS,
                                            const Standard_Integer               Num3DSS,
                                            const Handle(TColStd_HArray1OfReal)& OneDTol,
                                            const Handle(TColStd_HArray1OfReal)& TwoDTol,
                                            const Handle(TColStd_HArray1OfReal)& ThreeDTol,
                                            const Standard_Real                  First,
                                            const Standard_Real                  Last,
                                            const GeomAbs_Shape                  Continuity,
                                            const Standard_Integer               MaxDeg,
                                            const Standard_Integer               MaxSeg,
                                            const AdvApprox_EvaluatorFunction&   Func);

  //! Approximation with user methode of cutting
  Standard_EXPORT AdvApprox_ApproxAFunction(const Standard_Integer               Num1DSS,
                                            const Standard_Integer               Num2DSS,
                                            const Standard_Integer               Num3DSS,
                                            const Handle(TColStd_HArray1OfReal)& OneDTol,
                                            const Handle(TColStd_HArray1OfReal)& TwoDTol,
                                            const Handle(TColStd_HArray1OfReal)& ThreeDTol,
                                            const Standard_Real                  First,
                                            const Standard_Real                  Last,
                                            const GeomAbs_Shape                  Continuity,
                                            const Standard_Integer               MaxDeg,
                                            const Standard_Integer               MaxSeg,
                                            const AdvApprox_EvaluatorFunction&   Func,
                                            const AdvApprox_Cutting&             CutTool);

  Standard_EXPORT static void Approximation(const Standard_Integer         TotalDimension,
                                            const Standard_Integer         TotalNumSS,
                                            const TColStd_Array1OfInteger& LocalDimension,
                                            const Standard_Real            First,
                                            const Standard_Real            Last,
                                            AdvApprox_EvaluatorFunction&   Evaluator,
                                            const AdvApprox_Cutting&       CutTool,
                                            const Standard_Integer         ContinuityOrder,
                                            const Standard_Integer         NumMaxCoeffs,
                                            const Standard_Integer         MaxSegments,
                                            const TColStd_Array1OfReal&    TolerancesArray,
                                            const Standard_Integer         code_precis,
                                            Standard_Integer&              NumCurves,
                                            TColStd_Array1OfInteger&       NumCoeffPerCurveArray,
                                            TColStd_Array1OfReal&          LocalCoefficientArray,
                                            TColStd_Array1OfReal&          IntervalsArray,
                                            TColStd_Array1OfReal&          ErrorMaxArray,
                                            TColStd_Array1OfReal&          AverageErrorArray,
                                            Standard_Integer&              ErrorCode);

  Standard_Boolean IsDone() const;

  Standard_Boolean HasResult() const;

  //! returns the poles from the algorithms as is
  Handle(TColStd_HArray2OfReal) Poles1d() const;

  //! returns the poles from the algorithms as is
  Handle(TColgp_HArray2OfPnt2d) Poles2d() const;

  //! -- returns the poles from the algorithms as is
  Handle(TColgp_HArray2OfPnt) Poles() const;

  //! as the name says
  Standard_EXPORT Standard_Integer NbPoles() const;

  //! returns the poles at Index from the 1d subspace
  Standard_EXPORT void Poles1d(const Standard_Integer Index, TColStd_Array1OfReal& P) const;

  //! returns the poles at Index from the 2d subspace
  Standard_EXPORT void Poles2d(const Standard_Integer Index, TColgp_Array1OfPnt2d& P) const;

  //! returns the poles at Index from the 3d subspace
  Standard_EXPORT void Poles(const Standard_Integer Index, TColgp_Array1OfPnt& P) const;

  Standard_Integer Degree() const;

  Standard_Integer NbKnots() const;

  Standard_Integer NumSubSpaces(const Standard_Integer Dimension) const;

  Handle(TColStd_HArray1OfReal) Knots() const;

  Handle(TColStd_HArray1OfInteger) Multiplicities() const;

  //! returns the error as is in the algorithms
  Standard_EXPORT Handle(TColStd_HArray1OfReal) MaxError(const Standard_Integer Dimension) const;

  //! returns the error as is in the algorithms
  Standard_EXPORT Handle(TColStd_HArray1OfReal) AverageError(
    const Standard_Integer Dimension) const;

  Standard_EXPORT Standard_Real MaxError(const Standard_Integer Dimension,
                                         const Standard_Integer Index) const;

  Standard_EXPORT Standard_Real AverageError(const Standard_Integer Dimension,
                                             const Standard_Integer Index) const;

  //! display information on approximation.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
private:
  Standard_EXPORT void Perform(const Standard_Integer   Num1DSS,
                               const Standard_Integer   Num2DSS,
                               const Standard_Integer   Num3DSS,
                               const AdvApprox_Cutting& CutTool);

  Standard_Integer                 myNumSubSpaces[3];
  Handle(TColStd_HArray1OfReal)    my1DTolerances;
  Handle(TColStd_HArray1OfReal)    my2DTolerances;
  Handle(TColStd_HArray1OfReal)    my3DTolerances;
  Standard_Real                    myFirst;
  Standard_Real                    myLast;
  GeomAbs_Shape                    myContinuity;
  Standard_Integer                 myMaxDegree;
  Standard_Integer                 myMaxSegments;
  Standard_Boolean                 myDone;
  Standard_Boolean                 myHasResult;
  Handle(TColStd_HArray2OfReal)    my1DPoles;
  Handle(TColgp_HArray2OfPnt2d)    my2DPoles;
  Handle(TColgp_HArray2OfPnt)      my3DPoles;
  Handle(TColStd_HArray1OfReal)    myKnots;
  Handle(TColStd_HArray1OfInteger) myMults;
  Standard_Integer                 myDegree;
  Standard_Address                 myEvaluator;
  Handle(TColStd_HArray1OfReal)    my1DMaxError;
  Handle(TColStd_HArray1OfReal)    my1DAverageError;
  Handle(TColStd_HArray1OfReal)    my2DMaxError;
  Handle(TColStd_HArray1OfReal)    my2DAverageError;
  Handle(TColStd_HArray1OfReal)    my3DMaxError;
  Handle(TColStd_HArray1OfReal)    my3DAverageError;
};

#include <AdvApprox_ApproxAFunction.lxx>

#endif // _AdvApprox_ApproxAFunction_HeaderFile
