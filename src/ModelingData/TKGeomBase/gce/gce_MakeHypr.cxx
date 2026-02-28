// Created on: 1992-09-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#include <gce_MakeHypr.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

gce_MakeHypr::gce_MakeHypr(const gp_Pnt& S1, const gp_Pnt& S2, const gp_Pnt& Center)
{
  const gp_XYZ aVecS1  = S1.XYZ() - Center.XYZ();
  const gp_XYZ aVecS2  = S2.XYZ() - Center.XYZ();
  const double aDistS1 = aVecS1.Modulus();
  const double aDistS2 = aVecS2.Modulus();
  if (aDistS1 <= gp::Resolution() || aDistS2 <= gp::Resolution()
      || S1.Distance(S2) <= gp::Resolution())
  {
    TheError = gce_ConfusedPoints;
    return;
  }

  const gp_Dir aXAxis(aVecS1);
  const gp_Lin aLine(Center, aXAxis);
  const double aMinorDist = aLine.Distance(S2);
  if (aMinorDist <= gp::Resolution())
  {
    TheError = gce_ColinearPoints;
    return;
  }
  const gp_XYZ aCross = aXAxis.XYZ().Crossed(aVecS2);
  if (aCross.Modulus() <= gp::Resolution())
  {
    TheError = gce_ColinearPoints;
    return;
  }

  const gp_Dir aNorm(aCross);
  TheHypr  = gp_Hypr(gp_Ax2(Center, aNorm, aXAxis), aDistS1, aMinorDist);
  TheError = gce_Done;
}

//=================================================================================================

gce_MakeHypr::gce_MakeHypr(const gp_Ax2& A2, const double MajorRadius, const double MinorRadius)
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    TheHypr  = gp_Hypr(A2, MajorRadius, MinorRadius);
    TheError = gce_Done;
  }
}

//=================================================================================================

const gp_Hypr& gce_MakeHypr::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "gce_MakeHypr::Value() - no result");
  return TheHypr;
}

//=================================================================================================

const gp_Hypr& gce_MakeHypr::Operator() const
{
  return Value();
}

//=================================================================================================

gce_MakeHypr::operator gp_Hypr() const
{
  return Value();
}
