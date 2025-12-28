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

#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <Geom2d_Curve.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_Stripe, Standard_Transient)

ChFiDS_Stripe::ChFiDS_Stripe()
    : pardeb1(0.0),
      parfin1(0.0),
      pardeb2(0.0),
      parfin2(0.0),
      myChoix(0),
      indexOfSolid(0),
      indexOfcurve1(0),
      indexOfcurve2(0),
      indexfirstPOnS1(0),
      indexlastPOnS1(0),
      indexfirstPOnS2(0),
      indexlastPOnS2(0),
      begfilled(/*false*/ 0), // eap, Apr 29 2002, occ293
      endfilled(/*false*/ 0),
      myOr1(TopAbs_FORWARD),
      myOr2(TopAbs_FORWARD),
      orcurv1(TopAbs_FORWARD),
      orcurv2(TopAbs_FORWARD)
{
}

void ChFiDS_Stripe::Reset()
{
  myHdata.Nullify();
  orcurv1 = orcurv2 = TopAbs_FORWARD;
  pcrv1.Nullify();
  pcrv1.Nullify();
  mySpine->Reset();
}

//=================================================================================================

void ChFiDS_Stripe::Parameters(const bool First,
                               double&         Pdeb,
                               double&         Pfin) const
{
  if (First)
  {
    Pdeb = pardeb1;
    Pfin = parfin1;
  }
  else
  {
    Pdeb = pardeb2;
    Pfin = parfin2;
  }
}

//=================================================================================================

void ChFiDS_Stripe::SetParameters(const bool First,
                                  const double    Pdeb,
                                  const double    Pfin)
{
  if (First)
  {
    pardeb1 = Pdeb;
    parfin1 = Pfin;
  }
  else
  {
    pardeb2 = Pdeb;
    parfin2 = Pfin;
  }
}

//=================================================================================================

int ChFiDS_Stripe::Curve(const bool First) const
{
  if (First)
    return indexOfcurve1;
  else
    return indexOfcurve2;
}

//=================================================================================================

void ChFiDS_Stripe::SetCurve(const int Index, const bool First)
{
  if (First)
    indexOfcurve1 = Index;
  else
    indexOfcurve2 = Index;
}

//=======================================================================
// function : occ::handle<Geom2d_Curve>&
// purpose  :
//=======================================================================

const occ::handle<Geom2d_Curve>& ChFiDS_Stripe::PCurve(const bool First) const
{
  if (First)
    return pcrv1;
  else
    return pcrv2;
}

//=================================================================================================

occ::handle<Geom2d_Curve>& ChFiDS_Stripe::ChangePCurve(const bool First)
{
  if (First)
    return pcrv1;
  else
    return pcrv2;
}

//=================================================================================================

TopAbs_Orientation ChFiDS_Stripe::Orientation(const int OnS) const
{
  if (OnS == 1)
    return myOr1;
  else
    return myOr2;
}

//=================================================================================================

void ChFiDS_Stripe::SetOrientation(const TopAbs_Orientation Or, const int OnS)
{
  if (OnS == 1)
    myOr1 = Or;
  else
    myOr2 = Or;
}

//=================================================================================================

TopAbs_Orientation ChFiDS_Stripe::Orientation(const bool First) const
{
  if (First)
    return orcurv1;
  else
    return orcurv2;
}

//=================================================================================================

void ChFiDS_Stripe::SetOrientation(const TopAbs_Orientation Or, const bool First)
{
  if (First)
    orcurv1 = Or;
  else
    orcurv2 = Or;
}

//=================================================================================================

int ChFiDS_Stripe::IndexPoint(const bool First,
                                           const int OnS) const
{
  if (First)
  {
    if (OnS == 1)
      return indexfirstPOnS1;
    else
      return indexfirstPOnS2;
  }
  else
  {
    if (OnS == 1)
      return indexlastPOnS1;
    else
      return indexlastPOnS2;
  }
}

//=================================================================================================

void ChFiDS_Stripe::SetIndexPoint(const int Index,
                                  const bool First,
                                  const int OnS)
{
  if (First)
  {
    if (OnS == 1)
      indexfirstPOnS1 = Index;
    else
      indexfirstPOnS2 = Index;
  }
  else
  {
    if (OnS == 1)
      indexlastPOnS1 = Index;
    else
      indexlastPOnS2 = Index;
  }
}

int ChFiDS_Stripe::SolidIndex() const
{
  return indexOfSolid;
}

void ChFiDS_Stripe::SetSolidIndex(const int Index)
{
  indexOfSolid = Index;
}

//=================================================================================================

void ChFiDS_Stripe::InDS(const bool First,
                         const int Nb) // eap, Apr 29 2002, occ293
{
  if (First)
  {
    begfilled = /*true*/ Nb;
  }
  else
  {
    endfilled = /*true*/ Nb;
  }
}

//=================================================================================================

int ChFiDS_Stripe::IsInDS(const bool First) const
{
  if (First)
    return begfilled;
  else
    return endfilled;
}
