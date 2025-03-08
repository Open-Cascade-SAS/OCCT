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
      begfilled(/*Standard_False*/ 0), // eap, Apr 29 2002, occ293
      endfilled(/*Standard_False*/ 0),
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

void ChFiDS_Stripe::Parameters(const Standard_Boolean First,
                               Standard_Real&         Pdeb,
                               Standard_Real&         Pfin) const
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

void ChFiDS_Stripe::SetParameters(const Standard_Boolean First,
                                  const Standard_Real    Pdeb,
                                  const Standard_Real    Pfin)
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

Standard_Integer ChFiDS_Stripe::Curve(const Standard_Boolean First) const
{
  if (First)
    return indexOfcurve1;
  else
    return indexOfcurve2;
}

//=================================================================================================

void ChFiDS_Stripe::SetCurve(const Standard_Integer Index, const Standard_Boolean First)
{
  if (First)
    indexOfcurve1 = Index;
  else
    indexOfcurve2 = Index;
}

//=======================================================================
// function : Handle(Geom2d_Curve)&
// purpose  :
//=======================================================================

const Handle(Geom2d_Curve)& ChFiDS_Stripe::PCurve(const Standard_Boolean First) const
{
  if (First)
    return pcrv1;
  else
    return pcrv2;
}

//=================================================================================================

Handle(Geom2d_Curve)& ChFiDS_Stripe::ChangePCurve(const Standard_Boolean First)
{
  if (First)
    return pcrv1;
  else
    return pcrv2;
}

//=================================================================================================

TopAbs_Orientation ChFiDS_Stripe::Orientation(const Standard_Integer OnS) const
{
  if (OnS == 1)
    return myOr1;
  else
    return myOr2;
}

//=================================================================================================

void ChFiDS_Stripe::SetOrientation(const TopAbs_Orientation Or, const Standard_Integer OnS)
{
  if (OnS == 1)
    myOr1 = Or;
  else
    myOr2 = Or;
}

//=================================================================================================

TopAbs_Orientation ChFiDS_Stripe::Orientation(const Standard_Boolean First) const
{
  if (First)
    return orcurv1;
  else
    return orcurv2;
}

//=================================================================================================

void ChFiDS_Stripe::SetOrientation(const TopAbs_Orientation Or, const Standard_Boolean First)
{
  if (First)
    orcurv1 = Or;
  else
    orcurv2 = Or;
}

//=================================================================================================

Standard_Integer ChFiDS_Stripe::IndexPoint(const Standard_Boolean First,
                                           const Standard_Integer OnS) const
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

void ChFiDS_Stripe::SetIndexPoint(const Standard_Integer Index,
                                  const Standard_Boolean First,
                                  const Standard_Integer OnS)
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

Standard_Integer ChFiDS_Stripe::SolidIndex() const
{
  return indexOfSolid;
}

void ChFiDS_Stripe::SetSolidIndex(const Standard_Integer Index)
{
  indexOfSolid = Index;
}

//=================================================================================================

void ChFiDS_Stripe::InDS(const Standard_Boolean First,
                         const Standard_Integer Nb) // eap, Apr 29 2002, occ293
{
  if (First)
  {
    begfilled = /*Standard_True*/ Nb;
  }
  else
  {
    endfilled = /*Standard_True*/ Nb;
  }
}

//=================================================================================================

Standard_Integer ChFiDS_Stripe::IsInDS(const Standard_Boolean First) const
{
  if (First)
    return begfilled;
  else
    return endfilled;
}
