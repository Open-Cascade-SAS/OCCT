// Created on: 1996-08-09
// Created by: Herve LOUESSARD
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

#ifndef _LocalAnalysis_SurfaceContinuity_HeaderFile
#define _LocalAnalysis_SurfaceContinuity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_Shape.hxx>
#include <LocalAnalysis_StatusErrorType.hxx>
class Geom_Surface;
class Geom2d_Curve;
class GeomLProp_SLProps;

//! This class gives tools to check local continuity C0
//! C1 C2 G1 G2 between two points situated on two surfaces
class LocalAnalysis_SurfaceContinuity
{
public:
  DEFINE_STANDARD_ALLOC

  //! -u1,v1 are the parameters of the point on Surf1
  //! -u2,v2 are the parameters of the point on Surf2
  //! -Order is the required continuity:
  //! GeomAbs_C0 GeomAbs_C1 GeomAbs_C2
  //! GeomAbs_G1 GeomAbs_G2
  //!
  //! -EpsNul is used to detect a a vector with nul
  //! magnitude
  //!
  //! -EpsC0 is used for C0 continuity to confuse two
  //! points (in mm)
  //!
  //! -EpsC1 is an angular tolerance in radians used
  //! for C1 continuity to compare the angle between
  //! the first derivatives
  //!
  //! -EpsC2 is an angular tolerance in radians used
  //! for C2 continuity to compare the angle between
  //! the second derivatives
  //!
  //! -EpsG1 is an angular tolerance in radians used
  //! for G1 continuity to compare the angle between
  //! the normals
  //!
  //! -Percent: percentage of curvature variation (unitless)
  //! used for G2 continuity
  //!
  //! - Maxlen is the maximum length of Surf1 or Surf2 in
  //! meters used to detect null curvature (in mm)
  //!
  //! the constructor computes the quantities which are
  //! necessary to check the continuity in the following cases:
  //!
  //! case C0
  //! --------
  //! - the distance between P1 and P2 with P1=Surf (u1,v1) and
  //! P2=Surfv2(u2,v2)
  //!
  //! case C1
  //! -------
  //!
  //! - the angle between the first derivatives in u :
  //!
  //! dSurf1(u1,v1)               dSurf2(u2,v2)
  //! -----------      and        ---------
  //! du                           du
  //!
  //! the angle value is between 0 and PI/2
  //!
  //! - the angle between the first derivatives in v :
  //!
  //! dSurf1(u1,v1)               dSurf2(u2,v2)
  //! --------         and         ---------
  //! dv                           dv
  //!
  //! - the ratio between the magnitudes of the first derivatives in u
  //! - the ratio between the magnitudes of the first derivatives in v
  //!
  //! the angle value is between 0 and pi/2
  //!
  //! case C2
  //! -------
  //! - the angle between the second derivatives in u
  //! 2                  2
  //! d Surf1(u1,v1)    d  Surf2(u2,v2)
  //! ----------        ----------
  //! 2                  2
  //! d u               d  u
  //!
  //! - the ratio between the magnitudes of the second derivatives in u
  //! - the ratio between the magnitudes of the second derivatives in v
  //!
  //! the angle value is between 0 and PI/2
  //!
  //! case G1
  //! -------
  //! -the angle between the normals at each point
  //! the angle value is between 0 and PI/2
  //!
  //! case G2
  //! -------
  //! - the maximum normal curvature gap between the two
  //! points
  Standard_EXPORT LocalAnalysis_SurfaceContinuity(const occ::handle<Geom_Surface>& Surf1,
                                                  const double                     u1,
                                                  const double                     v1,
                                                  const occ::handle<Geom_Surface>& Surf2,
                                                  const double                     u2,
                                                  const double                     v2,
                                                  const GeomAbs_Shape              Order,
                                                  const double                     EpsNul  = 0.001,
                                                  const double                     EpsC0   = 0.001,
                                                  const double                     EpsC1   = 0.001,
                                                  const double                     EpsC2   = 0.001,
                                                  const double                     EpsG1   = 0.001,
                                                  const double                     Percent = 0.01,
                                                  const double                     Maxlen  = 10000);

  Standard_EXPORT LocalAnalysis_SurfaceContinuity(const occ::handle<Geom2d_Curve>& curv1,
                                                  const occ::handle<Geom2d_Curve>& curv2,
                                                  const double                     U,
                                                  const occ::handle<Geom_Surface>& Surf1,
                                                  const occ::handle<Geom_Surface>& Surf2,
                                                  const GeomAbs_Shape              Order,
                                                  const double                     EpsNul  = 0.001,
                                                  const double                     EpsC0   = 0.001,
                                                  const double                     EpsC1   = 0.001,
                                                  const double                     EpsC2   = 0.001,
                                                  const double                     EpsG1   = 0.001,
                                                  const double                     Percent = 0.01,
                                                  const double                     Maxlen  = 10000);

  //! This constructor is used when we want to compute many analysis.
  //! After we use the method ComputeAnalysis
  Standard_EXPORT LocalAnalysis_SurfaceContinuity(const double EpsNul  = 0.001,
                                                  const double EpsC0   = 0.001,
                                                  const double EpsC1   = 0.001,
                                                  const double EpsC2   = 0.001,
                                                  const double EpsG1   = 0.001,
                                                  const double Percent = 0.01,
                                                  const double Maxlen  = 10000);

  Standard_EXPORT void ComputeAnalysis(GeomLProp_SLProps&  Surf1,
                                       GeomLProp_SLProps&  Surf2,
                                       const GeomAbs_Shape Order);

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT GeomAbs_Shape ContinuityStatus() const;

  Standard_EXPORT LocalAnalysis_StatusErrorType StatusError() const;

  Standard_EXPORT double C0Value() const;

  Standard_EXPORT double C1UAngle() const;

  Standard_EXPORT double C1URatio() const;

  Standard_EXPORT double C1VAngle() const;

  Standard_EXPORT double C1VRatio() const;

  Standard_EXPORT double C2UAngle() const;

  Standard_EXPORT double C2URatio() const;

  Standard_EXPORT double C2VAngle() const;

  Standard_EXPORT double C2VRatio() const;

  Standard_EXPORT double G1Angle() const;

  Standard_EXPORT double G2CurvatureGap() const;

  Standard_EXPORT bool IsC0() const;

  Standard_EXPORT bool IsC1() const;

  Standard_EXPORT bool IsC2() const;

  Standard_EXPORT bool IsG1() const;

  Standard_EXPORT bool IsG2() const;

private:
  Standard_EXPORT void SurfC0(const GeomLProp_SLProps& Surf1, const GeomLProp_SLProps& Surf2);

  Standard_EXPORT void SurfC1(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2);

  Standard_EXPORT void SurfC2(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2);

  Standard_EXPORT void SurfG1(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2);

  Standard_EXPORT void SurfG2(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2);

  double                        myContC0;
  double                        myContC1U;
  double                        myContC1V;
  double                        myContC2U;
  double                        myContC2V;
  double                        myContG1;
  double                        myLambda1U;
  double                        myLambda2U;
  double                        myLambda1V;
  double                        myLambda2V;
  double                        myETA1;
  double                        myETA2;
  double                        myETA;
  double                        myZETA1;
  double                        myZETA2;
  double                        myZETA;
  double                        myAlpha;
  GeomAbs_Shape                 myTypeCont;
  double                        myepsC0;
  double                        myepsnul;
  double                        myepsC1;
  double                        myepsC2;
  double                        myepsG1;
  double                        myperce;
  double                        mymaxlen;
  double                        myGap;
  bool                          myIsDone;
  LocalAnalysis_StatusErrorType myErrorStatus;
};

#endif // _LocalAnalysis_SurfaceContinuity_HeaderFile
