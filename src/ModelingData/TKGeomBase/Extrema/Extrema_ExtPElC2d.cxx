// Created on: 1993-12-13
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#include <ElCLib.hxx>
#include <Extrema_ExtPElC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=============================================================================
Extrema_ExtPElC2d::Extrema_ExtPElC2d()
{
  myDone  = false;
  myNbExt = 0;

  for (int i = 0; i < 4; i++)
  {
    mySqDist[i] = RealLast();
    myIsMin[i]  = false;
  }
}

//=============================================================================

Extrema_ExtPElC2d::Extrema_ExtPElC2d(const gp_Pnt2d&     P,
                                     const gp_Lin2d&     L,
                                     const double Tol,
                                     const double Uinf,
                                     const double Usup)
{
  Perform(P, L, Tol, Uinf, Usup);
}

void Extrema_ExtPElC2d::Perform(const gp_Pnt2d&     P,
                                const gp_Lin2d&     L,
                                const double Tol,
                                const double Uinf,
                                const double Usup)
{
  myDone = true;
  gp_Pnt2d OR, MyP;
  myNbExt = 0;

  gp_Vec2d V1 = gp_Vec2d(L.Direction());
  OR          = L.Location();
  gp_Vec2d      V(OR, P);
  double Mydist = V1.Dot(V);
  if ((Mydist >= Uinf - Tol) && (Mydist <= Usup + Tol))
  {
    myNbExt = 1;
    MyP     = OR.Translated(Mydist * V1);
    Extrema_POnCurv2d MyPOnCurve(Mydist, MyP);
    mySqDist[0] = P.SquareDistance(MyP);
    myPoint[0]  = MyPOnCurve;
    myIsMin[0]  = true;
  }
}

//=============================================================================

Extrema_ExtPElC2d::Extrema_ExtPElC2d(const gp_Pnt2d&     P,
                                     const gp_Circ2d&    C,
                                     const double Tol,
                                     const double Uinf,
                                     const double Usup)
{
  Perform(P, C, Tol, Uinf, Usup);
}

void Extrema_ExtPElC2d::Perform(const gp_Pnt2d&     P,
                                const gp_Circ2d&    C,
                                const double Tol,
                                const double Uinf,
                                const double Usup)
{
  //  gp_Pnt2d OC, P1, P2, OL;
  gp_Pnt2d OC(C.Location());
  myNbExt = 0;

  if (OC.IsEqual(P, Precision::Confusion()))
  {
    myDone = false;
  }
  else
  {
    double radius, U1, U2;
    gp_Pnt2d      P1, P2;

    myDone = true;
    gp_Dir2d V(gp_Vec2d(P, OC));
    radius               = C.Radius();
    P1                   = OC.Translated(radius * V);
    U1                   = ElCLib::Parameter(C, P1);
    U2                   = U1 + M_PI;
    P2                   = OC.Translated(-radius * V);
    double myuinf = Uinf;
    ElCLib::AdjustPeriodic(Uinf, Uinf + 2 * M_PI, Precision::PConfusion(), myuinf, U1);
    ElCLib::AdjustPeriodic(Uinf, Uinf + 2 * M_PI, Precision::PConfusion(), myuinf, U2);
    if (((U1 - 2 * M_PI - Uinf) < Tol) && ((U1 - 2 * M_PI - Uinf) > -Tol))
    {
      U1 = Uinf;
      P1 = OC.XY()
           + radius * (cos(U1) * C.XAxis().Direction().XY() + sin(U1) * C.YAxis().Direction().XY());
    }

    if (((U2 - 2 * M_PI - Uinf) < Tol) && ((U2 - 2 * M_PI - Uinf) > -Tol))
    {
      U2 = Uinf;
      P2 = OC.XY()
           + radius * (cos(U2) * C.XAxis().Direction().XY() + sin(U2) * C.YAxis().Direction().XY());
    }

    if (((Uinf - U1) < Tol) && ((U1 - Usup) < Tol))
    {
      Extrema_POnCurv2d MyPOnCurve(U1, P1);
      mySqDist[0] = P.SquareDistance(P1);
      myPoint[0]  = MyPOnCurve;
      myIsMin[0]  = true;
      myNbExt++;
    }

    if (((Uinf - U2) < Tol) && ((U2 - Usup) < Tol))
    {
      Extrema_POnCurv2d MyPOnCurve(U2, P2);
      mySqDist[myNbExt] = P.SquareDistance(P2);
      myPoint[myNbExt]  = MyPOnCurve;
      myIsMin[myNbExt]  = true;
      myNbExt++;
    }
  }
}

//=============================================================================

Extrema_ExtPElC2d::Extrema_ExtPElC2d(const gp_Pnt2d&     P,
                                     const gp_Elips2d&   E,
                                     const double Tol,
                                     const double Uinf,
                                     const double Usup)
{
  Perform(P, E, Tol, Uinf, Usup);
}

void Extrema_ExtPElC2d::Perform(const gp_Pnt2d&     P,
                                const gp_Elips2d&   E,
                                const double Tol,
                                const double Uinf,
                                const double Usup)
{
  myDone  = false;
  myNbExt = 0;
  //  gp_Pnt2d OR, P1, P2;
  gp_Pnt2d OR;
  OR = E.Location();

  int NoSol, NbSol;
  double    A = E.MajorRadius();
  double    B = E.MinorRadius();
  gp_Vec2d         V(OR, P);

  if (OR.IsEqual(P, Precision::Confusion()) && (std::abs(A - B) <= Tol))
  {
    return;
  }
  else
  {
    double X = V.Dot(gp_Vec2d(E.XAxis().Direction()));
    double Y = V.Dot(gp_Vec2d(E.YAxis().Direction()));

    math_TrigonometricFunctionRoots Sol(0., (B * B - A * A) / 2., -B * Y, A * X, 0., Uinf, Usup);

    if (!Sol.IsDone())
    {
      return;
    }
    gp_Pnt2d      Cu;
    double Us;
    NbSol   = Sol.NbSolutions();
    myNbExt = 0;
    for (NoSol = 1; NoSol <= NbSol; NoSol++)
    {
      Us                = Sol.Value(NoSol);
      Cu                = ElCLib::Value(Us, E);
      mySqDist[myNbExt] = Cu.SquareDistance(P);
      myIsMin[myNbExt]  = (NoSol == 0);
      myPoint[myNbExt]  = Extrema_POnCurv2d(Us, Cu);
      myNbExt++;
    }
    myDone = true;
  }
}

//=============================================================================

Extrema_ExtPElC2d::Extrema_ExtPElC2d(const gp_Pnt2d&     P,
                                     const gp_Hypr2d&    C,
                                     const double Tol,
                                     const double Uinf,
                                     const double Usup)
{
  Perform(P, C, Tol, Uinf, Usup);
}

void Extrema_ExtPElC2d::Perform(const gp_Pnt2d&     P,
                                const gp_Hypr2d&    H,
                                const double Tol,
                                const double Uinf,
                                const double Usup)
{
  gp_Pnt2d O = H.Location();
  myDone     = false;
  myNbExt    = 0;

  double              R = H.MajorRadius();
  double              r = H.MinorRadius();
  gp_Vec2d                   OPp(O, P);
  double              Tol2 = Tol * Tol;
  double              X    = OPp.Dot(gp_Vec2d(H.XAxis().Direction()));
  double              Y    = OPp.Dot(gp_Vec2d(H.YAxis().Direction()));
  double              C1   = (R * R + r * r) / 4.;
  math_DirectPolynomialRoots Sol(C1, -(X * R + Y * r) / 2., 0., (X * R - Y * r) / 2., -C1);
  if (!Sol.IsDone())
  {
    return;
  }
  gp_Pnt2d         Cu;
  double    Us, Vs;
  int NbSol = Sol.NbSolutions();
  bool DejaEnr;
  int NoExt;
  gp_Pnt2d         TbExt[4];
  for (int NoSol = 1; NoSol <= NbSol; NoSol++)
  {
    Vs = Sol.Value(NoSol);
    if (Vs > 0.)
    {
      Us = std::log(Vs);
      if ((Us >= Uinf) && (Us <= Usup))
      {
        Cu      = ElCLib::Value(Us, H);
        DejaEnr = false;
        for (NoExt = 0; NoExt < myNbExt; NoExt++)
        {
          if (TbExt[NoExt].SquareDistance(Cu) < Tol2)
          {
            DejaEnr = true;
            break;
          }
        }
        if (!DejaEnr)
        {
          TbExt[myNbExt]    = Cu;
          mySqDist[myNbExt] = Cu.SquareDistance(P);
          myIsMin[myNbExt]  = (NoSol == 0);
          myPoint[myNbExt]  = Extrema_POnCurv2d(Us, Cu);
          myNbExt++;
        }
      } // if ((Us >= Uinf) && (Us <= Usup))
    } // if (Vs > 0.)
  } // for (int NoSol = 1; ...
  myDone = true;
}

//=============================================================================

Extrema_ExtPElC2d::Extrema_ExtPElC2d(const gp_Pnt2d&     P,
                                     const gp_Parab2d&   C,
                                     const double Tol,
                                     const double Uinf,
                                     const double Usup)
{
  Perform(P, C, Tol, Uinf, Usup);
}

void Extrema_ExtPElC2d::Perform(const gp_Pnt2d&     P,
                                const gp_Parab2d&   C,
                                const double Tol,
                                const double Uinf,
                                const double Usup)
{
  myDone     = false;
  myNbExt    = 0;
  gp_Pnt2d O = C.Location();

  double Tol2 = Tol * Tol;
  double F    = C.Focal();
  gp_Vec2d      OPp(O, P);
  double X = OPp.Dot(gp_Vec2d(C.MirrorAxis().Direction()));
  double Y = OPp.Dot(gp_Vec2d(C.Axis().YAxis().Direction()));

  math_DirectPolynomialRoots Sol(1. / (4. * F), 0., 2. * F - X, -2. * F * Y);
  if (!Sol.IsDone())
  {
    return;
  }
  gp_Pnt2d         Cu;
  double    Us;
  int NbSol = Sol.NbSolutions();
  bool DejaEnr;
  int NoExt;
  gp_Pnt2d         TbExt[3];
  for (int NoSol = 1; NoSol <= NbSol; NoSol++)
  {
    Us = Sol.Value(NoSol);
    if ((Us >= Uinf) && (Us <= Usup))
    {
      Cu      = ElCLib::Value(Us, C);
      DejaEnr = false;
      for (NoExt = 0; NoExt < myNbExt; NoExt++)
      {
        if (TbExt[NoExt].SquareDistance(Cu) < Tol2)
        {
          DejaEnr = true;
          break;
        }
      }
      if (!DejaEnr)
      {
        TbExt[myNbExt]    = Cu;
        mySqDist[myNbExt] = Cu.SquareDistance(P);
        myIsMin[myNbExt]  = (NoSol == 0);
        myPoint[myNbExt]  = Extrema_POnCurv2d(Us, Cu);
        myNbExt++;
      }
    } // if ((Us >= Uinf) && (Us <= Usup))
  } // for (int NoSol = 1; ...
  myDone = true;
}

//=============================================================================

bool Extrema_ExtPElC2d::IsDone() const
{
  return myDone;
}

//=============================================================================

int Extrema_ExtPElC2d::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myNbExt;
}

//=============================================================================

double Extrema_ExtPElC2d::SquareDistance(const int N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return mySqDist[N - 1];
}

//=============================================================================

bool Extrema_ExtPElC2d::IsMin(const int N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myIsMin[N - 1];
}

//=============================================================================

const Extrema_POnCurv2d& Extrema_ExtPElC2d::Point(const int N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myPoint[N - 1];
}

//=============================================================================
