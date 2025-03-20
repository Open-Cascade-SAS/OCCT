// Created on: 1997-06-24
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Approx_SweepApproximation_HeaderFile
#define _Approx_SweepApproximation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AdvApprox_EvaluatorFunction.hxx>
#include <Approx_HArray1OfGTrsf2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColgp_HArray1OfVec2d.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColgp_SequenceOfArray1OfPnt2d.hxx>
#include <gp_Vec.hxx>
class Approx_SweepFunction;
class AdvApprox_Cutting;

//! Approximation  of  an  Surface   S(u,v)
//! (and eventually associate  2d Curves) defined
//! by section's law.
//!
//! This surface is defined by a function F(u, v)
//! where Ft(u) = F(u, t) is a bspline curve.
//! To use this algorithme, you  have to implement Ft(u)
//! as a derivative class  of Approx_SweepFunction.
//! This algorithm can be used by blending, sweeping...
class Approx_SweepApproximation
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Approx_SweepApproximation(const Handle(Approx_SweepFunction)& Func);

  //! Perform the Approximation
  //! [First, Last] : Approx_SweepApproximation.cdl
  //! Tol3d : Tolerance to surface approximation
  //! Tol2d : Tolerance used to perform curve approximation
  //! Normally the 2d curve are approximated with a
  //! tolerance given by the resolution on support surfaces,
  //! but if this tolerance is too large Tol2d is used.
  //! TolAngular : Tolerance (in radian) to control the angle
  //! between tangents on the section law and
  //! tangent of iso-v on approximated surface
  //! Continuity : The continuity in v waiting on the surface
  //! Degmax     : The maximum degree in v required on the surface
  //! Segmax     : The maximum number of span in v required on
  //! the surface
  //! Warning : The continuity ci can be obtained only if Ft is Ci
  Standard_EXPORT void Perform(const Standard_Real    First,
                               const Standard_Real    Last,
                               const Standard_Real    Tol3d,
                               const Standard_Real    BoundTol,
                               const Standard_Real    Tol2d,
                               const Standard_Real    TolAngular,
                               const GeomAbs_Shape    Continuity = GeomAbs_C0,
                               const Standard_Integer Degmax     = 11,
                               const Standard_Integer Segmax     = 50);

  //! The EvaluatorFunction from AdvApprox;
  Standard_EXPORT Standard_Integer Eval(const Standard_Real    Parameter,
                                        const Standard_Integer DerivativeRequest,
                                        const Standard_Real    First,
                                        const Standard_Real    Last,
                                        Standard_Real&         Result);

  //! returns if we have an result
  Standard_Boolean IsDone() const;

  Standard_EXPORT void SurfShape(Standard_Integer& UDegree,
                                 Standard_Integer& VDegree,
                                 Standard_Integer& NbUPoles,
                                 Standard_Integer& NbVPoles,
                                 Standard_Integer& NbUKnots,
                                 Standard_Integer& NbVKnots) const;

  Standard_EXPORT void Surface(TColgp_Array2OfPnt&      TPoles,
                               TColStd_Array2OfReal&    TWeights,
                               TColStd_Array1OfReal&    TUKnots,
                               TColStd_Array1OfReal&    TVKnots,
                               TColStd_Array1OfInteger& TUMults,
                               TColStd_Array1OfInteger& TVMults) const;

  Standard_Integer UDegree() const;

  Standard_Integer VDegree() const;

  const TColgp_Array2OfPnt& SurfPoles() const;

  const TColStd_Array2OfReal& SurfWeights() const;

  const TColStd_Array1OfReal& SurfUKnots() const;

  const TColStd_Array1OfReal& SurfVKnots() const;

  const TColStd_Array1OfInteger& SurfUMults() const;

  const TColStd_Array1OfInteger& SurfVMults() const;

  //! returns the maximum error in the surface approximation.
  Standard_EXPORT Standard_Real MaxErrorOnSurf() const;

  //! returns the average error in the surface approximation.
  Standard_EXPORT Standard_Real AverageErrorOnSurf() const;

  Standard_Integer NbCurves2d() const;

  Standard_EXPORT void Curves2dShape(Standard_Integer& Degree,
                                     Standard_Integer& NbPoles,
                                     Standard_Integer& NbKnots) const;

  Standard_EXPORT void Curve2d(const Standard_Integer   Index,
                               TColgp_Array1OfPnt2d&    TPoles,
                               TColStd_Array1OfReal&    TKnots,
                               TColStd_Array1OfInteger& TMults) const;

  Standard_Integer Curves2dDegree() const;

  const TColgp_Array1OfPnt2d& Curve2dPoles(const Standard_Integer Index) const;

  const TColStd_Array1OfReal& Curves2dKnots() const;

  const TColStd_Array1OfInteger& Curves2dMults() const;

  //! returns the maximum error of the <Index>
  //! 2d curve approximation.
  Standard_EXPORT Standard_Real Max2dError(const Standard_Integer Index) const;

  //! returns the average error of the <Index>
  //! 2d curve approximation.
  Standard_EXPORT Standard_Real Average2dError(const Standard_Integer Index) const;

  //! returns the  maximum 3d  error  of the  <Index>
  //! 2d curve approximation on the Surface.
  Standard_EXPORT Standard_Real TolCurveOnSurf(const Standard_Integer Index) const;

  //! display information on approximation.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
private:
  Standard_EXPORT void Approximation(const Handle(TColStd_HArray1OfReal)& OneDTol,
                                     const Handle(TColStd_HArray1OfReal)& TwoDTol,
                                     const Handle(TColStd_HArray1OfReal)& ThreeDTol,
                                     const Standard_Real                  BounTol,
                                     const Standard_Real                  First,
                                     const Standard_Real                  Last,
                                     const GeomAbs_Shape                  Continuity,
                                     const Standard_Integer               Degmax,
                                     const Standard_Integer               Segmax,
                                     const AdvApprox_EvaluatorFunction&   TheApproxFunction,
                                     const AdvApprox_Cutting&             TheCuttingTool);

  Standard_EXPORT Standard_Boolean D0(const Standard_Real Param,
                                      const Standard_Real First,
                                      const Standard_Real Last,
                                      Standard_Real&      Result);

  Standard_EXPORT Standard_Boolean D1(const Standard_Real Param,
                                      const Standard_Real First,
                                      const Standard_Real Last,
                                      Standard_Real&      Result);

  Standard_EXPORT Standard_Boolean D2(const Standard_Real Param,
                                      const Standard_Real First,
                                      const Standard_Real Last,
                                      Standard_Real&      Result);

  Handle(Approx_SweepFunction)     myFunc;
  Standard_Boolean                 done;
  Standard_Integer                 Num1DSS;
  Standard_Integer                 Num2DSS;
  Standard_Integer                 Num3DSS;
  Standard_Integer                 udeg;
  Standard_Integer                 vdeg;
  Standard_Integer                 deg2d;
  Handle(TColgp_HArray2OfPnt)      tabPoles;
  Handle(TColStd_HArray2OfReal)    tabWeights;
  Handle(TColStd_HArray1OfReal)    tabUKnots;
  Handle(TColStd_HArray1OfReal)    tabVKnots;
  Handle(TColStd_HArray1OfReal)    tab2dKnots;
  Handle(TColStd_HArray1OfInteger) tabUMults;
  Handle(TColStd_HArray1OfInteger) tabVMults;
  Handle(TColStd_HArray1OfInteger) tab2dMults;
  TColgp_SequenceOfArray1OfPnt2d   seqPoles2d;
  Handle(TColStd_HArray1OfReal)    MError1d;
  Handle(TColStd_HArray1OfReal)    tab2dError;
  Handle(TColStd_HArray1OfReal)    MError3d;
  Handle(TColStd_HArray1OfReal)    AError1d;
  Handle(TColStd_HArray1OfReal)    Ave2dError;
  Handle(TColStd_HArray1OfReal)    AError3d;
  Handle(Approx_HArray1OfGTrsf2d)  AAffin;
  Handle(TColStd_HArray1OfReal)    COnSurfErr;
  gp_Vec                           Translation;
  Handle(TColgp_HArray1OfPnt)      myPoles;
  Handle(TColgp_HArray1OfPnt2d)    myPoles2d;
  Handle(TColStd_HArray1OfReal)    myWeigths;
  Handle(TColgp_HArray1OfVec)      myDPoles;
  Handle(TColgp_HArray1OfVec)      myD2Poles;
  Handle(TColgp_HArray1OfVec2d)    myDPoles2d;
  Handle(TColgp_HArray1OfVec2d)    myD2Poles2d;
  Handle(TColStd_HArray1OfReal)    myDWeigths;
  Handle(TColStd_HArray1OfReal)    myD2Weigths;
  Standard_Integer                 myOrder;
  Standard_Real                    myParam;
  Standard_Real                    first;
  Standard_Real                    last;
};

#include <Approx_SweepApproximation.lxx>

#endif // _Approx_SweepApproximation_HeaderFile
