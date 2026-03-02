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

#include <gce_MakeCone.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Cone.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

gce_MakeCone::gce_MakeCone(const gp_Ax2& A2, const double Ang, const double Radius)
{
  if (Radius < 0.0)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    if (Ang <= gp::Resolution() || M_PI / 2 - Ang <= gp::Resolution())
    {
      TheError = gce_BadAngle;
    }
    else
    {
      TheError = gce_Done;
      TheCone  = gp_Cone(A2, Ang, Radius);
    }
  }
}

//=================================================================================================

gce_MakeCone::gce_MakeCone(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3, const gp_Pnt& P4)
{
  // P1 and P2 define the cone axis. The distance from P3 to the axis gives
  // the base radius, and the distance from P4 to the axis gives the radius
  // of the section passing through P4.
  if (P1.Distance(P2) < gp::Resolution() || P3.Distance(P4) < gp::Resolution())
  {
    TheError = gce_ConfusedPoints;
    return;
  }

  gp_Dir D1(P2.XYZ() - P1.XYZ());
  double cos  = D1.Dot(gp_Dir(P4.XYZ() - P1.XYZ()));
  double dist = P1.Distance(P4);
  gp_Pnt PP4(P1.XYZ() + cos * dist * D1.XYZ());
  cos  = D1.Dot(gp_Dir(P3.XYZ() - P1.XYZ()));
  dist = P1.Distance(P3);
  gp_Pnt PP3(P1.XYZ() + cos * dist * D1.XYZ());

  double Dist13 = PP3.Distance(P1);
  double Dist14 = PP4.Distance(P1);
  if (std::abs(Dist13 - Dist14) < gp::Resolution())
  {
    TheError = gce_NullAngle;
    return;
  }
  gp_Lin L1(P1, D1);
  double Dist3  = L1.Distance(P3);
  double Dist4  = L1.Distance(P4);
  double DifRad = Dist3 - Dist4;
  double angle  = std::abs(std::atan(DifRad / (Dist13 - Dist14)));
  if (std::abs(M_PI / 2. - angle) < gp::Resolution() || std::abs(angle) < gp::Resolution())
  {
    TheError = gce_NullRadius;
    return;
  }
  double R1 = PP3.Distance(P3);
  double R2 = PP4.Distance(P4);
  if (R1 < 0.0 || R2 < 0.0)
  {
    TheError = gce_NegativeRadius;
    return;
  }
  gp_Dir DD1(PP4.XYZ() - PP3.XYZ());
  gp_Dir D2;
  double x = DD1.X();
  double y = DD1.Y();
  double z = DD1.Z();
  if (std::abs(x) > gp::Resolution())
  {
    D2 = gp_Dir(-y, x, 0.0);
  }
  else if (std::abs(y) > gp::Resolution())
  {
    D2 = gp_Dir(0.0, -z, y);
  }
  else if (std::abs(z) > gp::Resolution())
  {
    D2 = gp_Dir(z, 0.0, -x);
  }
  if (R1 > R2)
  {
    angle *= -1;
  }
  TheCone  = gp_Cone(gp_Ax2(PP3, DD1, D2), angle, R1);
  TheError = gce_Done;
}

//=================================================================================================

gce_MakeCone::gce_MakeCone(const gp_Ax1& Axis, const gp_Pnt& P1, const gp_Pnt& P2)
{
  // The distance from P1 to the axis gives the base radius, and
  // the distance from P2 to the axis gives the section radius at P2.
  gp_Pnt       P3(Axis.Location());
  gp_Pnt       P4(P3.XYZ() + Axis.Direction().XYZ());
  gce_MakeCone Cone(P3, P4, P1, P2);
  if (Cone.IsDone())
  {
    TheCone  = Cone.Value();
    TheError = gce_Done;
  }
  else
  {
    TheError = Cone.Status();
  }
}

//=================================================================================================

// gce_MakeCone::gce_MakeCone(const gp_Cone&  cone ,
//			   const gp_Pnt&   P    )
gce_MakeCone::gce_MakeCone(const gp_Cone& Cone, const gp_Pnt& P)
{
  const gp_XYZ aVecToPoint = P.XYZ() - Cone.Location().XYZ();
  const double aVParam     = aVecToPoint.Dot(Cone.Axis().Direction().XYZ());
  const double aRadius     = gp_Lin(Cone.Axis()).Distance(P);
  const double aTan        = std::tan(Cone.SemiAngle());
  const double aCos        = std::cos(Cone.SemiAngle());

  const double aCandidate1 = aRadius - aVParam * aTan;
  const double aCandidate2 = -aRadius - aVParam * aTan;

  // Accept tiny negative values as zero (rounding noise from analytic formulas),
  // then clamp to 0.0 before constructing gp_Cone.
  const auto isNonNegativeWithTol = [](const double theValue) {
    return theValue >= -gp::Resolution();
  };
  const auto clampToZero = [](const double theValue) { return theValue < 0.0 ? 0.0 : theValue; };

  double aResultRadius = -1.0;
  if (isNonNegativeWithTol(aCandidate1) && isNonNegativeWithTol(aCandidate2))
  {
    // Keep the nearest parallel cone to preserve predictable behavior.
    const double aCand1 = clampToZero(aCandidate1);
    const double aCand2 = clampToZero(aCandidate2);
    const double aDist1 = std::abs((aCand1 - Cone.RefRadius()) * aCos);
    const double aDist2 = std::abs((aCand2 - Cone.RefRadius()) * aCos);
    aResultRadius       = (aDist1 <= aDist2) ? aCand1 : aCand2;
  }
  else if (isNonNegativeWithTol(aCandidate1))
  {
    aResultRadius = clampToZero(aCandidate1);
  }
  else if (isNonNegativeWithTol(aCandidate2))
  {
    aResultRadius = clampToZero(aCandidate2);
  }

  if (aResultRadius < 0.0)
  {
    TheError = gce_NegativeRadius;
    return;
  }

  TheCone  = gp_Cone(Cone.Position(), Cone.SemiAngle(), aResultRadius);
  TheError = gce_Done;
}

//=================================================================================================

// gce_MakeCone::gce_MakeCone(const gp_Cone&      cone ,
//			   const double Dist )
gce_MakeCone::gce_MakeCone(const gp_Cone& Cone, const double Dist)
{
  // gp_Cone keeps |SemiAngle| in ]gp::Resolution(), PI/2 - gp::Resolution()[,
  // thus aCos is expected to be positive and not too small.
  const double aCos = std::cos(Cone.SemiAngle());
  if (std::abs(aCos) <= gp::Resolution())
  {
    TheError = gce_NullAngle;
    return;
  }
  const double aRadius = Cone.RefRadius() + Dist / aCos;
  if (aRadius < 0.0)
  {
    TheError = gce_NegativeRadius;
    return;
  }

  TheCone  = gp_Cone(Cone.Position(), Cone.SemiAngle(), aRadius);
  TheError = gce_Done;
}

//=================================================================================================

gce_MakeCone::gce_MakeCone(const gp_Lin& Axis, const gp_Pnt& P1, const gp_Pnt& P2)
{
  gp_Pnt       P3(Axis.Location());
  gp_Pnt       P4(P3.XYZ() + Axis.Direction().XYZ());
  gce_MakeCone Cone(P3, P4, P1, P2);
  if (Cone.IsDone())
  {
    TheCone  = Cone.Value();
    TheError = gce_Done;
  }
  else
  {
    TheError = Cone.Status();
  }
}

//=================================================================================================

gce_MakeCone::gce_MakeCone(const gp_Pnt& P1, const gp_Pnt& P2, const double R1, const double R2)
{
  // Cone defined by two points (axis) and two radii (section radii at each point).
  double dist = P1.Distance(P2);
  if (dist < gp::Resolution())
  {
    TheError = gce_NullAxis;
  }
  else
  {
    if (R1 < 0.0 || R2 < 0.0)
    {
      TheError = gce_NegativeRadius;
    }
    else
    {
      double Angle = std::abs(atan((R1 - R2) / dist));
      if (std::abs(M_PI / 2. - Angle) < gp::Resolution() || std::abs(Angle) < gp::Resolution())
      {
        TheError = gce_NullAngle;
      }
      else
      {
        gp_Dir D1(P2.XYZ() - P1.XYZ());
        gp_Dir D2;
        double x = D1.X();
        double y = D1.Y();
        double z = D1.Z();
        if (std::abs(x) > gp::Resolution())
        {
          D2 = gp_Dir(-y, x, 0.0);
        }
        else if (std::abs(y) > gp::Resolution())
        {
          D2 = gp_Dir(0.0, -z, y);
        }
        else if (std::abs(z) > gp::Resolution())
        {
          D2 = gp_Dir(z, 0.0, -x);
        }
        if (R1 > R2)
        {
          Angle *= -1;
        }
        TheCone  = gp_Cone(gp_Ax2(P1, D1, D2), Angle, R1);
        TheError = gce_Done;
      }
    }
  }
}

//=================================================================================================

const gp_Cone& gce_MakeCone::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "gce_MakeCone::Value() - no result");
  return TheCone;
}

