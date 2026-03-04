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

#include <Extrema_ExtElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <gce_MakeCirc.hxx>
#include <gce_MakeDir.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3)
{
  // Three cases:
  // 1) All three points are coincident: result is a circle centered at P1 with zero radius.
  // 2) Two of the three points are coincident: no solution (error: ConfusedPoints).
  // 3) All three points are distinct: create the perpendicular bisector of P1P2 and
  //    the perpendicular bisector of P2P3. The center is their intersection and
  //    the radius is the distance from that center to any of the three points.
  double dist1, dist2, dist3, aResolution;
  //
  aResolution = gp::Resolution();
  //
  dist1 = P1.Distance(P2);
  dist2 = P1.Distance(P3);
  dist3 = P2.Distance(P3);
  //
  if ((dist1 < aResolution) && (dist2 < aResolution) && (dist3 < aResolution))
  {
    gp_Dir Dirx(gp_Dir::D::X);
    gp_Dir Dirz(gp_Dir::D::Z);
    TheCirc = gp_Circ(gp_Ax2(P1, Dirx, Dirz), 0.);
    return;
  }
  if (dist1 < aResolution || dist2 < aResolution)
  {
    TheError = gce_ConfusedPoints;
    return;
  }
  //
  double x1, y1, z1, x2, y2, z2, x3, y3, z3;
  //
  P1.Coord(x1, y1, z1);
  P2.Coord(x2, y2, z2);
  P3.Coord(x3, y3, z3);
  gp_Dir Dir1(x2 - x1, y2 - y1, z2 - z1);
  gp_Vec VDir2(x3 - x2, y3 - y2, z3 - z2);
  //
  gp_Ax1 anAx1(P1, Dir1);
  gp_Lin aL12(anAx1);
  if (aL12.Distance(P3) < aResolution)
  {
    TheError = gce_ColinearPoints;
    return;
  }
  //
  gp_Vec VDir1(Dir1);
  gp_Vec VDir3 = VDir1.Crossed(VDir2);
  if (VDir3.SquareMagnitude() < aResolution)
  {
    TheError = gce_ColinearPoints;
    return;
  }
  //
  gp_Dir Dir3(VDir3);
  gp_Dir dir = Dir1.Crossed(Dir3);
  gp_Lin L1(gp_Pnt((P1.XYZ() + P2.XYZ()) / 2.), dir);
  dir = VDir2.Crossed(Dir3);
  gp_Lin L2(gp_Pnt((P3.XYZ() + P2.XYZ()) / 2.), dir);

  constexpr double Tol = Precision::PConfusion();
  Extrema_ExtElC   distmin(L1, L2, Tol);

  if (!distmin.IsDone())
  {
    TheError = gce_IntersectionError;
  }
  else
  {
    int nbext;
    //
    //
    if (distmin.IsParallel())
    {
      TheError = gce_IntersectionError;
      return;
    }
    nbext = distmin.NbExt();
    //
    //
    if (nbext == 0)
    {
      TheError = gce_IntersectionError;
    }
    else
    {
      double          TheDist = RealLast();
      gp_Pnt          pInt, pon1, pon2;
      int             i = 1;
      Extrema_POnCurv Pon1, Pon2;
      while (i <= nbext)
      {
        if (distmin.SquareDistance(i) < TheDist)
        {
          TheDist = distmin.SquareDistance(i);
          distmin.Points(i, Pon1, Pon2);
          pon1 = Pon1.Value();
          pon2 = Pon2.Value();
          pInt = gp_Pnt((pon1.XYZ() + pon2.XYZ()) / 2.);
        }
        i++;
      }
      // modified by NIZNHY-PKV Thu Mar  3 11:30:34 2005f
      // Dir2.Cross(Dir1);
      // modified by NIZNHY-PKV Thu Mar  3 11:30:37 2005t
      dist1 = P1.Distance(pInt);
      dist2 = P2.Distance(pInt);
      dist3 = P3.Distance(pInt);
      pInt.Coord(x3, y3, z3);
      if (dist1 < aResolution)
      {
        gp_Dir Dirx(gp_Dir::D::X);
        gp_Dir Dirz(gp_Dir::D::Z);
        TheCirc = gp_Circ(gp_Ax2(pInt, Dirx, Dirz), 0.);
        return;
      }
      Dir1 = gp_Dir(x1 - x3, y1 - y3, z1 - z3);
      // modified by NIZNHY-PKV Thu Mar  3 11:31:11 2005f
      // Dir2 = gp_Dir(x2-x3,y2-y3,z2-z3);
      // modified by NIZNHY-PKV Thu Mar  3 11:31:13 2005t
      //
      TheCirc  = gp_Circ(gp_Ax2(pInt, gp_Dir(VDir3), Dir1), (dist1 + dist2 + dist3) / 3.);
      TheError = gce_Done;
    }
  }
}

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Ax2& A2, const double Radius)
{
  if (Radius < 0.)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    TheError = gce_Done;
    TheCirc  = gp_Circ(A2, Radius);
  }
}

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Pnt& Center, const gp_Pln& Plane, const double Radius)
{
  gce_MakeCirc C = gce_MakeCirc(Center, Plane.Position().Direction(), Radius);
  TheCirc        = C.Value();
  TheError       = C.Status();
}

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Pnt& Center, const gp_Dir& Norm, const double Radius)
{
  if (Radius < 0.)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    double A    = Norm.X();
    double B    = Norm.Y();
    double C    = Norm.Z();
    double Aabs = std::abs(A);
    double Babs = std::abs(B);
    double Cabs = std::abs(C);
    gp_Ax2 Pos;

    // To determine the X axis:
    // the dot product Vx.Norm = 0.
    // find the max(A,B,C) for the division.
    // one of the vector coordinates is zero.

    if (Babs <= Aabs && Babs <= Cabs)
    {
      if (Aabs > Cabs)
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(-C, 0., A));
      }
      else
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(C, 0., -A));
      }
    }
    else if (Aabs <= Babs && Aabs <= Cabs)
    {
      if (Babs > Cabs)
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(0., -C, B));
      }
      else
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(0., C, -B));
      }
    }
    else
    {
      if (Aabs > Babs)
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(-B, A, 0.));
      }
      else
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(B, -A, 0.));
      }
    }
    TheCirc  = gp_Circ(Pos, Radius);
    TheError = gce_Done;
  }
}

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Pnt& Center, const gp_Pnt& Ptaxis, const double Radius)
{
  if (Radius < 0.)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    if (Center.Distance(Ptaxis) <= gp::Resolution())
    {
      TheError = gce_NullAxis;
    }
    else
    {
      double A    = Ptaxis.X() - Center.X();
      double B    = Ptaxis.Y() - Center.Y();
      double C    = Ptaxis.Z() - Center.Z();
      double Aabs = std::abs(A);
      double Babs = std::abs(B);
      double Cabs = std::abs(C);
      gp_Ax2 Pos;

      // To determine the X axis:
      // the dot product Vx.Norm = 0.
      // find the max(A,B,C) for the division.
      // one of the vector coordinates is zero.

      gp_Dir Norm = gce_MakeDir(Center, Ptaxis);
      if (Babs <= Aabs && Babs <= Cabs)
      {
        if (Aabs > Cabs)
        {
          Pos = gp_Ax2(Center, Norm, gp_Dir(-C, 0., A));
        }
        else
        {
          Pos = gp_Ax2(Center, Norm, gp_Dir(C, 0., -A));
        }
      }
      else if (Aabs <= Babs && Aabs <= Cabs)
      {
        if (Babs > Cabs)
        {
          Pos = gp_Ax2(Center, Norm, gp_Dir(0., -C, B));
        }
        else
        {
          Pos = gp_Ax2(Center, Norm, gp_Dir(0., C, -B));
        }
      }
      else
      {
        if (Aabs > Babs)
        {
          Pos = gp_Ax2(Center, Norm, gp_Dir(-B, A, 0.));
        }
        else
        {
          Pos = gp_Ax2(Center, Norm, gp_Dir(B, -A, 0.));
        }
      }
      TheCirc  = gp_Circ(Pos, Radius);
      TheError = gce_Done;
    }
  }
}

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Ax1& Axis, const double Radius)
{
  if (Radius < 0.)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    gp_Dir Norm(Axis.Direction());
    gp_Pnt Center(Axis.Location());
    double A    = Norm.X();
    double B    = Norm.Y();
    double C    = Norm.Z();
    double Aabs = std::abs(A);
    double Babs = std::abs(B);
    double Cabs = std::abs(C);
    gp_Ax2 Pos;

    // To determine the X axis:
    // the dot product Vx.Norm = 0.
    // find the max(A,B,C) for the division.
    // one of the vector coordinates is zero.

    if (Babs <= Aabs && Babs <= Cabs)
    {
      if (Aabs > Cabs)
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(-C, 0., A));
      }
      else
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(C, 0., -A));
      }
    }
    else if (Aabs <= Babs && Aabs <= Cabs)
    {
      if (Babs > Cabs)
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(0., -C, B));
      }
      else
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(0., C, -B));
      }
    }
    else
    {
      if (Aabs > Babs)
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(-B, A, 0.));
      }
      else
      {
        Pos = gp_Ax2(Center, Norm, gp_Dir(B, -A, 0.));
      }
    }
    TheCirc  = gp_Circ(Pos, Radius);
    TheError = gce_Done;
  }
}

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Circ& Circ, const double Dist)
{
  double Rad = Circ.Radius() + Dist;
  if (Rad < 0.)
  {
    TheError = gce_NegativeRadius;
  }
  else
  {
    TheCirc  = gp_Circ(Circ.Position(), Rad);
    TheError = gce_Done;
  }
}

//=================================================================================================

gce_MakeCirc::gce_MakeCirc(const gp_Circ& Circ, const gp_Pnt& P)
{
  double Rad = gp_Lin(Circ.Axis()).Distance(P);
  TheCirc    = gp_Circ(Circ.Position(), Rad);
  TheError   = gce_Done;
}

//=================================================================================================

const gp_Circ& gce_MakeCirc::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "gce_MakeCirc::Value() - no result");
  return TheCirc;
}
