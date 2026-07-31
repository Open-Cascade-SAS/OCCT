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

//=================================================================================================

inline const occ::handle<NCollection_HSequence<occ::handle<ChFiDS_SurfData>>>& ChFiDS_Stripe::
  SetOfSurfData() const
{
  return myHdata;
}

//=================================================================================================

inline const occ::handle<ChFiDS_Spine>& ChFiDS_Stripe::Spine() const
{
  return mySpine;
}

//=================================================================================================

inline TopAbs_Orientation ChFiDS_Stripe::OrientationOnFace1() const
{
  return myOr1;
}

//=================================================================================================

inline TopAbs_Orientation ChFiDS_Stripe::OrientationOnFace2() const
{
  return myOr2;
}

//=================================================================================================

inline int ChFiDS_Stripe::Choix() const
{
  return myChoix;
}

//=================================================================================================

inline occ::handle<NCollection_HSequence<occ::handle<ChFiDS_SurfData>>>& ChFiDS_Stripe::
  ChangeSetOfSurfData()
{
  return myHdata;
}

//=================================================================================================

inline occ::handle<ChFiDS_Spine>& ChFiDS_Stripe::ChangeSpine()
{
  return mySpine;
}

//=================================================================================================

inline void ChFiDS_Stripe::OrientationOnFace1(const TopAbs_Orientation Or1)
{
  myOr1 = Or1;
}

//=================================================================================================

inline void ChFiDS_Stripe::OrientationOnFace2(const TopAbs_Orientation Or2)
{
  myOr2 = Or2;
}

//=================================================================================================

inline void ChFiDS_Stripe::Choix(const int C)
{
  myChoix = C;
}

//=================================================================================================

inline void ChFiDS_Stripe::FirstParameters(double& Pdeb, double& Pfin) const
{
  Pdeb = pardeb1;
  Pfin = parfin1;
}

//=================================================================================================

inline void ChFiDS_Stripe::LastParameters(double& Pdeb, double& Pfin) const
{
  Pdeb = pardeb2;
  Pfin = parfin2;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeFirstParameters(const double Pdeb, const double Pfin)
{
  pardeb1 = Pdeb;
  parfin1 = Pfin;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeLastParameters(const double Pdeb, const double Pfin)
{
  pardeb2 = Pdeb;
  parfin2 = Pfin;
}

//=================================================================================================

inline int ChFiDS_Stripe::FirstCurve() const
{
  return indexOfcurve1;
}

//=================================================================================================

inline int ChFiDS_Stripe::LastCurve() const
{
  return indexOfcurve2;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeFirstCurve(const int Index)
{
  indexOfcurve1 = Index;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeLastCurve(const int Index)
{
  indexOfcurve2 = Index;
}

//=================================================================================================

inline const occ::handle<Geom2d_Curve>& ChFiDS_Stripe::FirstPCurve() const
{
  return pcrv1;
}

//=================================================================================================

inline const occ::handle<Geom2d_Curve>& ChFiDS_Stripe::LastPCurve() const
{
  return pcrv2;
}

//=================================================================================================

inline occ::handle<Geom2d_Curve>& ChFiDS_Stripe::ChangeFirstPCurve()
{
  return pcrv1;
}

//=================================================================================================

inline occ::handle<Geom2d_Curve>& ChFiDS_Stripe::ChangeLastPCurve()
{
  return pcrv2;
}

//=================================================================================================

inline int ChFiDS_Stripe::IndexFirstPointOnS1() const
{
  return indexfirstPOnS1;
}

//=================================================================================================

inline int ChFiDS_Stripe::IndexLastPointOnS1() const
{
  return indexlastPOnS1;
}

//=================================================================================================

inline int ChFiDS_Stripe::IndexFirstPointOnS2() const
{
  return indexfirstPOnS2;
}

//=================================================================================================

inline int ChFiDS_Stripe::IndexLastPointOnS2() const
{
  return indexlastPOnS2;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeIndexFirstPointOnS1(const int Index)
{
  indexfirstPOnS1 = Index;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeIndexLastPointOnS1(const int Index)
{
  indexlastPOnS1 = Index;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeIndexFirstPointOnS2(const int Index)
{
  indexfirstPOnS2 = Index;
}

//=================================================================================================

inline void ChFiDS_Stripe::ChangeIndexLastPointOnS2(const int Index)
{
  indexlastPOnS2 = Index;
}

//=================================================================================================

inline TopAbs_Orientation ChFiDS_Stripe::FirstPCurveOrientation() const
{
  return orcurv1;
}

//=================================================================================================

inline TopAbs_Orientation ChFiDS_Stripe::LastPCurveOrientation() const
{
  return orcurv2;
}

//=================================================================================================

inline void ChFiDS_Stripe::FirstPCurveOrientation(const TopAbs_Orientation O)
{
  orcurv1 = O;
}

//=================================================================================================

inline void ChFiDS_Stripe::LastPCurveOrientation(const TopAbs_Orientation O)
{
  orcurv2 = O;
}
