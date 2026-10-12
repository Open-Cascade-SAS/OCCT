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
#include <GeomLProp_SLProps.hxx>

//! This class gives tools to check local continuity C0
//! C1 C2 G1 G2 between two points situated on two surfaces
class LocalAnalysis_SurfaceContinuity
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes local continuity between two surface points.
  //! @param[in] Surf1 first surface
  //! @param[in] u1 U parameter on the first surface
  //! @param[in] v1 V parameter on the first surface
  //! @param[in] Surf2 second surface
  //! @param[in] u2 U parameter on the second surface
  //! @param[in] v2 V parameter on the second surface
  //! @param[in] Order requested continuity: C0, C1, C2, G1 or G2
  //! @param[in] EpsNul tolerance for detecting null derivatives
  //! @param[in] EpsC0 positional tolerance in model units
  //! @param[in] EpsC1 first-derivative angular tolerance in radians
  //! @param[in] EpsC2 second-derivative angular tolerance in radians
  //! @param[in] EpsG1 normal angular tolerance in radians
  //! @param[in] Percent relative G2 tolerance against the largest absolute principal curvature
  //! @param[in] Maxlen reference length in model units for detecting null curvature
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

  //! Computes local continuity at corresponding pcurve parameters.
  //! @param[in] curv1 pcurve on the first surface
  //! @param[in] curv2 pcurve on the second surface
  //! @param[in] U common pcurve parameter
  //! @param[in] Surf1 first surface
  //! @param[in] Surf2 second surface
  //! @param[in] Order requested continuity: C0, C1, C2, G1 or G2
  //! @param[in] EpsNul tolerance for detecting null derivatives
  //! @param[in] EpsC0 positional tolerance in model units
  //! @param[in] EpsC1 first-derivative angular tolerance in radians
  //! @param[in] EpsC2 second-derivative angular tolerance in radians
  //! @param[in] EpsG1 normal angular tolerance in radians
  //! @param[in] Percent relative G2 tolerance against the largest absolute principal curvature
  //! @param[in] Maxlen reference length in model units for detecting null curvature
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

  //! Initializes tolerances for subsequent calls to ComputeAnalysis().
  //! @param[in] EpsNul tolerance for detecting null derivatives
  //! @param[in] EpsC0 positional tolerance in model units
  //! @param[in] EpsC1 first-derivative angular tolerance in radians
  //! @param[in] EpsC2 second-derivative angular tolerance in radians
  //! @param[in] EpsG1 normal angular tolerance in radians
  //! @param[in] Percent relative G2 tolerance against the largest absolute principal curvature
  //! @param[in] Maxlen reference length in model units for detecting null curvature
  Standard_EXPORT LocalAnalysis_SurfaceContinuity(const double EpsNul  = 0.001,
                                                  const double EpsC0   = 0.001,
                                                  const double EpsC1   = 0.001,
                                                  const double EpsC2   = 0.001,
                                                  const double EpsG1   = 0.001,
                                                  const double Percent = 0.01,
                                                  const double Maxlen  = 10000);

  //! Computes local continuity from surface properties.
  //! @param[in,out] Surf1 first surface properties; derivatives are evaluated as needed
  //! @param[in,out] Surf2 second surface properties; derivatives are evaluated as needed
  //! @param[in] Order requested continuity: C0, C1, C2, G1 or G2
  Standard_EXPORT void ComputeAnalysis(GeomLProp_SLProps&  Surf1,
                                       GeomLProp_SLProps&  Surf2,
                                       const GeomAbs_Shape Order);

  //! Reports whether the analysis completed.
  //! @return true if the requested quantities were computed
  Standard_EXPORT bool IsDone() const;

  //! Returns the analyzed continuity order.
  //! @return continuity order supplied to the analysis
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT GeomAbs_Shape ContinuityStatus() const;

  //! Returns the analysis error status.
  //! @return failure reason, or LocalAnalysis_NoError after success
  Standard_EXPORT LocalAnalysis_StatusErrorType StatusError() const;

  //! Returns the positional gap.
  //! @return distance between the analyzed points in model units
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C0Value() const;

  //! Returns the C1 derivative angle in U.
  //! @return derivative angle in radians
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C1UAngle() const;

  //! Returns the C1 derivative magnitude ratio in U.
  //! @return derivative magnitude ratio used by the continuity test
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C1URatio() const;

  //! Returns the C1 derivative angle in V.
  //! @return derivative angle in radians
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C1VAngle() const;

  //! Returns the C1 derivative magnitude ratio in V.
  //! @return derivative magnitude ratio used by the continuity test
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C1VRatio() const;

  //! Returns the C2 derivative angle in U.
  //! @return derivative angle in radians
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C2UAngle() const;

  //! Returns the C2 derivative magnitude ratio in U.
  //! @return derivative magnitude ratio used by the continuity test
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C2URatio() const;

  //! Returns the C2 derivative angle in V.
  //! @return derivative angle in radians
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C2VAngle() const;

  //! Returns the C2 derivative magnitude ratio in V.
  //! @return derivative magnitude ratio used by the continuity test
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double C2VRatio() const;

  //! Returns the angle between surface normals.
  //! @return normal angle in radians
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double G1Angle() const;

  //! Returns the normal-curvature gap in aligned tangent planes.
  //! @return maximum absolute normal-curvature difference
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT double G2CurvatureGap() const;

  //! Tests C0 continuity.
  //! @return true if the computed quantities satisfy the C0 tolerances
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT bool IsC0() const;

  //! Tests C1 continuity.
  //! @return true if the computed quantities satisfy the C1 tolerances
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT bool IsC1() const;

  //! Tests C2 continuity.
  //! @return true if the computed quantities satisfy the C2 tolerances
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT bool IsC2() const;

  //! Tests G1 continuity.
  //! @return true if the computed quantities satisfy the G1 tolerances
  //! @throw StdFail_NotDone if the analysis has not completed
  Standard_EXPORT bool IsG1() const;

  //! Tests G2 continuity.
  //! @return true if the computed quantities satisfy the G2 tolerances
  //! @throw StdFail_NotDone if the analysis has not completed
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
  GeomAbs_Shape                 myTypeCont;
  double                        myepsC0;
  double                        myepsnul;
  double                        myepsC1;
  double                        myepsC2;
  double                        myepsG1;
  double                        myperce;
  double                        mymaxlen;
  double                        myCurvatureScale; //!< Largest absolute principal curvature.
  double                        myGap;
  bool                          myIsDone;
  LocalAnalysis_StatusErrorType myErrorStatus;
};

#endif // _LocalAnalysis_SurfaceContinuity_HeaderFile
