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

#include <ChFiDS_HData.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Transient.hxx>
class ChFiDS_Spine;
class Geom2d_Curve;

class ChFiDS_Stripe;
DEFINE_STANDARD_HANDLE(ChFiDS_Stripe, Standard_Transient)

//! Data characterising a band of fillet.
class ChFiDS_Stripe : public Standard_Transient
{

public:
  Standard_EXPORT ChFiDS_Stripe();

  //! Reset everything except Spine.
  Standard_EXPORT void Reset();

  const Handle(ChFiDS_HData)& SetOfSurfData() const;

  const Handle(ChFiDS_Spine)& Spine() const;

  TopAbs_Orientation OrientationOnFace1() const;

  TopAbs_Orientation OrientationOnFace2() const;

  Standard_Integer Choix() const;

  Handle(ChFiDS_HData)& ChangeSetOfSurfData();

  Handle(ChFiDS_Spine)& ChangeSpine();

  void OrientationOnFace1(const TopAbs_Orientation Or1);

  void OrientationOnFace2(const TopAbs_Orientation Or2);

  void Choix(const Standard_Integer C);

  void FirstParameters(Standard_Real& Pdeb, Standard_Real& Pfin) const;

  void LastParameters(Standard_Real& Pdeb, Standard_Real& Pfin) const;

  void ChangeFirstParameters(const Standard_Real Pdeb, const Standard_Real Pfin);

  void ChangeLastParameters(const Standard_Real Pdeb, const Standard_Real Pfin);

  Standard_Integer FirstCurve() const;

  Standard_Integer LastCurve() const;

  void ChangeFirstCurve(const Standard_Integer Index);

  void ChangeLastCurve(const Standard_Integer Index);

  const Handle(Geom2d_Curve)& FirstPCurve() const;

  const Handle(Geom2d_Curve)& LastPCurve() const;

  Handle(Geom2d_Curve)& ChangeFirstPCurve();

  Handle(Geom2d_Curve)& ChangeLastPCurve();

  TopAbs_Orientation FirstPCurveOrientation() const;

  TopAbs_Orientation LastPCurveOrientation() const;

  void FirstPCurveOrientation(const TopAbs_Orientation O);

  void LastPCurveOrientation(const TopAbs_Orientation O);

  Standard_Integer IndexFirstPointOnS1() const;

  Standard_Integer IndexFirstPointOnS2() const;

  Standard_Integer IndexLastPointOnS1() const;

  Standard_Integer IndexLastPointOnS2() const;

  void ChangeIndexFirstPointOnS1(const Standard_Integer Index);

  void ChangeIndexFirstPointOnS2(const Standard_Integer Index);

  void ChangeIndexLastPointOnS1(const Standard_Integer Index);

  void ChangeIndexLastPointOnS2(const Standard_Integer Index);

  Standard_EXPORT void Parameters(const Standard_Boolean First,
                                  Standard_Real&         Pdeb,
                                  Standard_Real&         Pfin) const;

  Standard_EXPORT void SetParameters(const Standard_Boolean First,
                                     const Standard_Real    Pdeb,
                                     const Standard_Real    Pfin);

  Standard_EXPORT Standard_Integer Curve(const Standard_Boolean First) const;

  Standard_EXPORT void SetCurve(const Standard_Integer Index, const Standard_Boolean First);

  Standard_EXPORT const Handle(Geom2d_Curve)& PCurve(const Standard_Boolean First) const;

  Standard_EXPORT Handle(Geom2d_Curve)& ChangePCurve(const Standard_Boolean First);

  Standard_EXPORT TopAbs_Orientation Orientation(const Standard_Integer OnS) const;

  Standard_EXPORT void SetOrientation(const TopAbs_Orientation Or, const Standard_Integer OnS);

  Standard_EXPORT TopAbs_Orientation Orientation(const Standard_Boolean First) const;

  Standard_EXPORT void SetOrientation(const TopAbs_Orientation Or, const Standard_Boolean First);

  Standard_EXPORT Standard_Integer IndexPoint(const Standard_Boolean First,
                                              const Standard_Integer OnS) const;

  Standard_EXPORT void SetIndexPoint(const Standard_Integer Index,
                                     const Standard_Boolean First,
                                     const Standard_Integer OnS);

  Standard_EXPORT Standard_Integer SolidIndex() const;

  Standard_EXPORT void SetSolidIndex(const Standard_Integer Index);

  //! Set nb of SurfData's at end put in DS
  Standard_EXPORT void InDS(const Standard_Boolean First, const Standard_Integer Nb = 1);

  //! Returns nb of SurfData's at end being in DS
  Standard_EXPORT Standard_Integer IsInDS(const Standard_Boolean First) const;

  DEFINE_STANDARD_RTTIEXT(ChFiDS_Stripe, Standard_Transient)

protected:
private:
  Standard_Real        pardeb1;
  Standard_Real        parfin1;
  Standard_Real        pardeb2;
  Standard_Real        parfin2;
  Handle(ChFiDS_Spine) mySpine;
  Handle(ChFiDS_HData) myHdata;
  Handle(Geom2d_Curve) pcrv1;
  Handle(Geom2d_Curve) pcrv2;
  Standard_Integer     myChoix;
  Standard_Integer     indexOfSolid;
  Standard_Integer     indexOfcurve1;
  Standard_Integer     indexOfcurve2;
  Standard_Integer     indexfirstPOnS1;
  Standard_Integer     indexlastPOnS1;
  Standard_Integer     indexfirstPOnS2;
  Standard_Integer     indexlastPOnS2;
  Standard_Integer     begfilled;
  Standard_Integer     endfilled;
  TopAbs_Orientation   myOr1;
  TopAbs_Orientation   myOr2;
  TopAbs_Orientation   orcurv1;
  TopAbs_Orientation   orcurv2;
};

#include <ChFiDS_Stripe.lxx>

#endif // _ChFiDS_Stripe_HeaderFile
