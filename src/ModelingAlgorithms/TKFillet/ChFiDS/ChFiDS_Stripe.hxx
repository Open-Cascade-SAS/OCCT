// Created on: 1994-03-09
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _ChFiDS_Stripe_HeaderFile
#define _ChFiDS_Stripe_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ChFiDS_SurfData.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Transient.hxx>
class ChFiDS_Spine;
class Geom2d_Curve;

//! Data characterising a band of fillet.
class ChFiDS_Stripe : public Standard_Transient
{

public:
  Standard_EXPORT ChFiDS_Stripe();

  //! Reset everything except Spine.
  Standard_EXPORT void Reset();

  const occ::handle<NCollection_HSequence<occ::handle<ChFiDS_SurfData>>>& SetOfSurfData() const;

  const occ::handle<ChFiDS_Spine>& Spine() const;

  TopAbs_Orientation OrientationOnFace1() const;

  TopAbs_Orientation OrientationOnFace2() const;

  int Choix() const;

  occ::handle<NCollection_HSequence<occ::handle<ChFiDS_SurfData>>>& ChangeSetOfSurfData();

  occ::handle<ChFiDS_Spine>& ChangeSpine();

  void OrientationOnFace1(const TopAbs_Orientation Or1);

  void OrientationOnFace2(const TopAbs_Orientation Or2);

  void Choix(const int C);

  void FirstParameters(double& Pdeb, double& Pfin) const;

  void LastParameters(double& Pdeb, double& Pfin) const;

  void ChangeFirstParameters(const double Pdeb, const double Pfin);

  void ChangeLastParameters(const double Pdeb, const double Pfin);

  int FirstCurve() const;

  int LastCurve() const;

  void ChangeFirstCurve(const int Index);

  void ChangeLastCurve(const int Index);

  const occ::handle<Geom2d_Curve>& FirstPCurve() const;

  const occ::handle<Geom2d_Curve>& LastPCurve() const;

  occ::handle<Geom2d_Curve>& ChangeFirstPCurve();

  occ::handle<Geom2d_Curve>& ChangeLastPCurve();

  TopAbs_Orientation FirstPCurveOrientation() const;

  TopAbs_Orientation LastPCurveOrientation() const;

  void FirstPCurveOrientation(const TopAbs_Orientation O);

  void LastPCurveOrientation(const TopAbs_Orientation O);

  int IndexFirstPointOnS1() const;

  int IndexFirstPointOnS2() const;

  int IndexLastPointOnS1() const;

  int IndexLastPointOnS2() const;

  void ChangeIndexFirstPointOnS1(const int Index);

  void ChangeIndexFirstPointOnS2(const int Index);

  void ChangeIndexLastPointOnS1(const int Index);

  void ChangeIndexLastPointOnS2(const int Index);

  Standard_EXPORT void Parameters(const bool First,
                                  double&         Pdeb,
                                  double&         Pfin) const;

  Standard_EXPORT void SetParameters(const bool First,
                                     const double    Pdeb,
                                     const double    Pfin);

  Standard_EXPORT int Curve(const bool First) const;

  Standard_EXPORT void SetCurve(const int Index, const bool First);

  Standard_EXPORT const occ::handle<Geom2d_Curve>& PCurve(const bool First) const;

  Standard_EXPORT occ::handle<Geom2d_Curve>& ChangePCurve(const bool First);

  Standard_EXPORT TopAbs_Orientation Orientation(const int OnS) const;

  Standard_EXPORT void SetOrientation(const TopAbs_Orientation Or, const int OnS);

  Standard_EXPORT TopAbs_Orientation Orientation(const bool First) const;

  Standard_EXPORT void SetOrientation(const TopAbs_Orientation Or, const bool First);

  Standard_EXPORT int IndexPoint(const bool First,
                                              const int OnS) const;

  Standard_EXPORT void SetIndexPoint(const int Index,
                                     const bool First,
                                     const int OnS);

  Standard_EXPORT int SolidIndex() const;

  Standard_EXPORT void SetSolidIndex(const int Index);

  //! Set nb of SurfData's at end put in DS
  Standard_EXPORT void InDS(const bool First, const int Nb = 1);

  //! Returns nb of SurfData's at end being in DS
  Standard_EXPORT int IsInDS(const bool First) const;

  DEFINE_STANDARD_RTTIEXT(ChFiDS_Stripe, Standard_Transient)

private:
  double        pardeb1;
  double        parfin1;
  double        pardeb2;
  double        parfin2;
  occ::handle<ChFiDS_Spine> mySpine;
  occ::handle<NCollection_HSequence<occ::handle<ChFiDS_SurfData>>> myHdata;
  occ::handle<Geom2d_Curve> pcrv1;
  occ::handle<Geom2d_Curve> pcrv2;
  int     myChoix;
  int     indexOfSolid;
  int     indexOfcurve1;
  int     indexOfcurve2;
  int     indexfirstPOnS1;
  int     indexlastPOnS1;
  int     indexfirstPOnS2;
  int     indexlastPOnS2;
  int     begfilled;
  int     endfilled;
  TopAbs_Orientation   myOr1;
  TopAbs_Orientation   myOr2;
  TopAbs_Orientation   orcurv1;
  TopAbs_Orientation   orcurv2;
};

#include <ChFiDS_Stripe.lxx>

#endif // _ChFiDS_Stripe_HeaderFile
