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
#include <NCollection_Array1.hxx>
#include <gp_GTrsf2d.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Sequence.hxx>
class Approx_SweepFunction;
class AdvApprox_Cutting;

//! Approximation of an Surface S(u,v)
//! (and eventually associate 2d Curves) defined
//! by section's law.
//!
//! This surface is defined by a function F(u, v)
//! where Ft(u) = F(u, t) is a bspline curve.
//! To use this algorithm, you have to implement Ft(u)
//! as a derivative class of Approx_SweepFunction.
//! This algorithm can be used by blending, sweeping...
class Approx_SweepApproximation
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Approx_SweepApproximation(const occ::handle<Approx_SweepFunction>& Func);

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
  Standard_EXPORT void Perform(const double        First,
                               const double        Last,
                               const double        Tol3d,
                               const double        BoundTol,
                               const double        Tol2d,
                               const double        TolAngular,
                               const GeomAbs_Shape Continuity = GeomAbs_C0,
                               const int           Degmax     = 11,
                               const int           Segmax     = 50);

  //! The EvaluatorFunction from AdvApprox;
  Standard_EXPORT int Eval(const double Parameter,
                           const int    DerivativeRequest,
                           const double First,
                           const double Last,
                           double&      Result);

  //! returns if we have an result
  bool IsDone() const;

  Standard_EXPORT void SurfShape(int& UDegree,
                                 int& VDegree,
                                 int& NbUPoles,
                                 int& NbVPoles,
                                 int& NbUKnots,
                                 int& NbVKnots) const;

  Standard_EXPORT void Surface(NCollection_Array2<gp_Pnt>& TPoles,
                               NCollection_Array2<double>& TWeights,
                               NCollection_Array1<double>& TUKnots,
                               NCollection_Array1<double>& TVKnots,
                               NCollection_Array1<int>&    TUMults,
                               NCollection_Array1<int>&    TVMults) const;

  int UDegree() const;

  int VDegree() const;

  const NCollection_Array2<gp_Pnt>& SurfPoles() const;

  const NCollection_Array2<double>& SurfWeights() const;

  const NCollection_Array1<double>& SurfUKnots() const;

  const NCollection_Array1<double>& SurfVKnots() const;

  const NCollection_Array1<int>& SurfUMults() const;

  const NCollection_Array1<int>& SurfVMults() const;

  //! returns the maximum error in the surface approximation.
  Standard_EXPORT double MaxErrorOnSurf() const;

  //! returns the average error in the surface approximation.
  Standard_EXPORT double AverageErrorOnSurf() const;

  int NbCurves2d() const;

  Standard_EXPORT void Curves2dShape(int& Degree, int& NbPoles, int& NbKnots) const;

  Standard_EXPORT void Curve2d(const int                     Index,
                               NCollection_Array1<gp_Pnt2d>& TPoles,
                               NCollection_Array1<double>&   TKnots,
                               NCollection_Array1<int>&      TMults) const;

  int Curves2dDegree() const;

  const NCollection_Array1<gp_Pnt2d>& Curve2dPoles(const int Index) const;

  const NCollection_Array1<double>& Curves2dKnots() const;

  const NCollection_Array1<int>& Curves2dMults() const;

  //! returns the maximum error of the <Index>
  //! 2d curve approximation.
  Standard_EXPORT double Max2dError(const int Index) const;

  //! returns the average error of the <Index>
  //! 2d curve approximation.
  Standard_EXPORT double Average2dError(const int Index) const;

  //! returns the maximum 3d error of the <Index>
  //! 2d curve approximation on the Surface.
  Standard_EXPORT double TolCurveOnSurf(const int Index) const;

  //! display information on approximation.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  Standard_EXPORT void Approximation(const occ::handle<NCollection_HArray1<double>>& OneDTol,
                                     const occ::handle<NCollection_HArray1<double>>& TwoDTol,
                                     const occ::handle<NCollection_HArray1<double>>& ThreeDTol,
                                     const double                                    BounTol,
                                     const double                                    First,
                                     const double                                    Last,
                                     const GeomAbs_Shape                             Continuity,
                                     const int                                       Degmax,
                                     const int                                       Segmax,
                                     const AdvApprox_EvaluatorFunction& TheApproxFunction,
                                     const AdvApprox_Cutting&           TheCuttingTool);

  Standard_EXPORT bool D0(const double Param,
                          const double First,
                          const double Last,
                          double&      Result);

  Standard_EXPORT bool D1(const double Param,
                          const double First,
                          const double Last,
                          double&      Result);

  Standard_EXPORT bool D2(const double Param,
                          const double First,
                          const double Last,
                          double&      Result);

  occ::handle<Approx_SweepFunction>                                myFunc;
  bool                                                             done;
  int                                                              Num1DSS;
  int                                                              Num2DSS;
  int                                                              Num3DSS;
  int                                                              udeg;
  int                                                              vdeg;
  int                                                              deg2d;
  occ::handle<NCollection_HArray2<gp_Pnt>>                         tabPoles;
  occ::handle<NCollection_HArray2<double>>                         tabWeights;
  occ::handle<NCollection_HArray1<double>>                         tabUKnots;
  occ::handle<NCollection_HArray1<double>>                         tabVKnots;
  occ::handle<NCollection_HArray1<double>>                         tab2dKnots;
  occ::handle<NCollection_HArray1<int>>                            tabUMults;
  occ::handle<NCollection_HArray1<int>>                            tabVMults;
  occ::handle<NCollection_HArray1<int>>                            tab2dMults;
  NCollection_Sequence<occ::handle<NCollection_HArray1<gp_Pnt2d>>> seqPoles2d;
  occ::handle<NCollection_HArray1<double>>                         MError1d;
  occ::handle<NCollection_HArray1<double>>                         tab2dError;
  occ::handle<NCollection_HArray1<double>>                         MError3d;
  occ::handle<NCollection_HArray1<double>>                         AError1d;
  occ::handle<NCollection_HArray1<double>>                         Ave2dError;
  occ::handle<NCollection_HArray1<double>>                         AError3d;
  occ::handle<NCollection_HArray1<gp_GTrsf2d>>                     AAffin;
  occ::handle<NCollection_HArray1<double>>                         COnSurfErr;
  gp_Vec                                                           Translation;
  occ::handle<NCollection_HArray1<gp_Pnt>>                         myPoles;
  occ::handle<NCollection_HArray1<gp_Pnt2d>>                       myPoles2d;
  occ::handle<NCollection_HArray1<double>>                         myWeigths;
  occ::handle<NCollection_HArray1<gp_Vec>>                         myDPoles;
  occ::handle<NCollection_HArray1<gp_Vec>>                         myD2Poles;
  occ::handle<NCollection_HArray1<gp_Vec2d>>                       myDPoles2d;
  occ::handle<NCollection_HArray1<gp_Vec2d>>                       myD2Poles2d;
  occ::handle<NCollection_HArray1<double>>                         myDWeigths;
  occ::handle<NCollection_HArray1<double>>                         myD2Weigths;
  int                                                              myOrder;
  double                                                           myParam;
  double                                                           first;
  double                                                           last;
};

#include <Approx_SweepApproximation.lxx>

#endif // _Approx_SweepApproximation_HeaderFile
