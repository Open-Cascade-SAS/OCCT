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

#include <gce_MakeHypr2d.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Pnt2d& S1, const gp_Pnt2d& S2, const gp_Pnt2d& Center)
{
  const gp_XY  aVecS1  = S1.XY() - Center.XY();
  const gp_XY  aVecS2  = S2.XY() - Center.XY();
  const double aDistS1 = aVecS1.Modulus();
  const double aDistS2 = aVecS2.Modulus();
  if (aDistS1 <= gp::Resolution() || aDistS2 <= gp::Resolution()
      || S1.Distance(S2) <= gp::Resolution())
  {
    TheError = gce_ConfusedPoints;
    return;
  }

  const gp_Dir2d aXAxis(aVecS1);
  const gp_Dir2d aYAxis(aVecS2);
  const gp_Ax22d anAxis(Center, aXAxis, aYAxis);
  const gp_Lin2d aLine(Center, aXAxis);
  const double   aMinorDist = aLine.Distance(S2);
  if (aMinorDist <= gp::Resolution())
  {
    TheError = gce_ColinearPoints;
    return;
  }
  TheHypr2d = gp_Hypr2d(anAxis, aDistS1, aMinorDist);
  TheError  = gce_Done;
}

//=================================================================================================

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Ax2d& MajorAxis,
                               const double   MajorRadius,
                               const double   MinorRadius,
                               const bool     Sense)
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    TheHypr2d = gp_Hypr2d(MajorAxis, MajorRadius, MinorRadius, Sense);
    TheError  = gce_Done;
  }
}

//=================================================================================================

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Ax22d& A,
                               const double    MajorRadius,
                               const double    MinorRadius)
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    TheHypr2d = gp_Hypr2d(A, MajorRadius, MinorRadius);
    TheError  = gce_Done;
  }
}

//=================================================================================================

const gp_Hypr2d& gce_MakeHypr2d::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "gce_MakeHypr2d::Value() - no result");
  return TheHypr2d;
}
