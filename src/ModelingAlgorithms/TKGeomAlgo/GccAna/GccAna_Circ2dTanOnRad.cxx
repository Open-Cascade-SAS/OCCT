// Copyright (c) 1995-1999 Matra Datavision
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

// PRO12736 : bug quand OnLine // Ox, JCT 20/03/98
//========================================================================
//       circular tangent to element of type :      - Circle.            +
//                                                  - Line.             +
//                                                  - Point.             +
//              center on second element of type :  - Circle.  +
//                                                  - Line.   +
//              of given radius : Radius.                             +
//========================================================================

#include <ElCLib.hxx>
#include <GccAna_Circ2dTanOnRad.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>

typedef math_DirectPolynomialRoots Roots;

//=========================================================================
//  Circle tangent  :  to circle Qualified1 (C1).                         +
//         center   :  on straight line OnLine.                           +
//         of radius :  Radius.                                           +
//                                                                        +
//  Initialise the table of solutions cirsol and all fields.              +
//  Eliminate depending on the qualifier the cases not being solutions.   +
//  Solve the equation of the second degree indicating that the found center +
//  point (xc,yc) is at a distance Radius from circle C1 and              +
//                     on straight line OnLine.                           +
//  The solutions aret represented by circles :                           +
//                   - with center Pntcen(xc,yc)                          +
//                   - with radius Radius.                                +
//=========================================================================

GccAna_Circ2dTanOnRad::GccAna_Circ2dTanOnRad(const GccEnt_QualifiedCirc& Qualified1,
                                             const gp_Lin2d&             OnLine,
                                             const double         Radius,
                                             const double         Tolerance)
    : cirsol(1, 4),
      qualifier1(1, 4),
      TheSame1(1, 4),
      pnttg1sol(1, 4),
      pntcen3(1, 4),
      par1sol(1, 4),
      pararg1(1, 4),
      parcen3(1, 4)
{

  TheSame1.Init(0);
  gp_Dir2d      dirx(gp_Dir2d::D::X);
  double Tol = std::abs(Tolerance);
  WellDone          = false;
  NbrSol            = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || Qualified1.IsOutside()
        || Qualified1.IsUnqualified()))
  {
    throw GccEnt_BadQualifier();
    return;
  }
  NCollection_Array1<double> Coef(1, 2);
  gp_Circ2d            C1 = Qualified1.Qualified();

  if (Radius < 0.0)
  {
    throw Standard_NegativeValue();
  }
  else
  {
    int nbsol = 0;
    int signe = 0;
    gp_Pnt2d         Center;
    double    xc;
    double    yc;
    double    R1    = C1.Radius();
    double    dist  = OnLine.Distance(C1.Location());
    double    xdir  = (OnLine.Direction()).X();
    double    ydir  = (OnLine.Direction()).Y();
    double    lxloc = (OnLine.Location()).X();
    double    lyloc = (OnLine.Location()).Y();
    gp_Pnt2d         center1(C1.Location());
    double    x1 = center1.X();
    double    y1 = center1.Y();
    if (Qualified1.IsEnclosed())
    {
      //   ============================
      if (Tol < Radius - R1 + dist)
      {
        WellDone = true;
      }
      else
      {
        if (std::abs(Radius - R1 + dist) < Tol)
        {
          WellDone = true;
          NbrSol   = 1;
          if (-ydir * (x1 - lxloc) + xdir * (y1 - lyloc) < 0.0)
          {
            Center = gp_Pnt2d(x1 - ydir * dist, y1 + xdir * dist);
          }
          else
          {
            Center = gp_Pnt2d(x1 + ydir * dist, y1 - xdir * dist);
          }
          signe = 1;
        }
        else
        {
          Coef(1) = (R1 - Radius) * (R1 - Radius);
          nbsol   = 1;
        }
      }
    }
    else if (Qualified1.IsEnclosing())
    {
      //   ==================================
      if (R1 + dist - Radius > Tol)
      {
        WellDone = true;
      }
      else
      {
        if (R1 + dist - Radius > 0.0)
        {
          WellDone = true;
          NbrSol   = 1;
          if (-ydir * (x1 - lxloc) + xdir * (y1 - lyloc) < 0.0)
          {
            Center = gp_Pnt2d(x1 - ydir * dist, y1 + xdir * dist);
          }
          else
          {
            Center = gp_Pnt2d(x1 + ydir * dist, y1 - xdir * dist);
          }
          signe = -1;
        }
        else
        {
          Coef(1) = (Radius - R1) * (Radius - R1);
          nbsol   = 1;
        }
      }
    }
    else
    {
      //   ====
      if (dist - R1 - Radius > Tol)
      {
        WellDone = false;
      }
      else
      {
        if (std::abs(dist - R1 - Radius) < Tol)
        {
          WellDone = true;
          NbrSol   = 1;
          if (-ydir * (x1 - lxloc) + xdir * (y1 - lyloc) < 0.0)
          {
            Center = gp_Pnt2d(x1 - ydir * dist, y1 + xdir * dist);
          }
          else
          {
            Center = gp_Pnt2d(x1 + ydir * dist, y1 - xdir * dist);
          }
          signe = -1;
        }
        else
        {
          if (Qualified1.IsOutside())
          {
            //         ===========================
            Coef(1) = (Radius + R1) * (Radius + R1);
            nbsol   = 1;
          }
          else
          {
            //         ====
            Coef(1) = (Radius - R1) * (Radius - R1);
            Coef(2) = (Radius + R1) * (Radius + R1);
            nbsol   = 2;
          }
        }
      }
    }
    if (signe != 0)
    {
      cirsol(1) = gp_Circ2d(gp_Ax2d(Center, dirx), Radius);
      //     ==================================================
      double distcc1 = Center.Distance(center1);
      if (!Qualified1.IsUnqualified())
      {
        qualifier1(1) = Qualified1.Qualifier();
      }
      else if (std::abs(distcc1 + Radius - R1) < Tol)
      {
        qualifier1(1) = GccEnt_enclosed;
      }
      else if (std::abs(distcc1 - R1 - Radius) < Tol)
      {
        qualifier1(1) = GccEnt_outside;
      }
      else
      {
        qualifier1(1) = GccEnt_enclosing;
      }
      if (std::abs(Radius - R1) <= Tol)
      {
        TheSame1(1) = 1;
      }
      else
      {
        gp_Dir2d dir1cen(Center.X() - x1, Center.Y() - y1);
        pnttg1sol(1) = gp_Pnt2d(Center.XY() + signe * Radius * dir1cen.XY());
        par1sol(1)   = ElCLib::Parameter(cirsol(1), pnttg1sol(1));
        pararg1(1)   = ElCLib::Parameter(C1, pnttg1sol(1));
      }
      pntcen3(1) = cirsol(NbrSol).Location();
      parcen3(1) = ElCLib::Parameter(OnLine, pntcen3(1));
    }
    else if (nbsol > 0)
    {
      for (int j = 1; j <= nbsol; j++)
      {
        double A, B, C;
        OnLine.Coefficients(A, B, C);
        double D = A;
        double x0, y0;
        if (std::abs(D) <= Tol)
        {
          A  = B;
          B  = D;
          x0 = y1;
          y0 = x1;
        }
        else
        {
          x0 = x1;
          y0 = y1;
        }
        Roots Sol((B * B + A * A) / (A * A),
                  2.0 * (B * C / (A * A) + (B / A) * x0 - y0),
                  x0 * x0 + y0 * y0 + C * C / (A * A) - Coef(j) + 2.0 * C * x0 / A);
        if (Sol.IsDone())
        {
          for (int i = 1; i <= Sol.NbSolutions(); i++)
          {

            if (std::abs(D) > Tol)
            {
              yc = Sol.Value(i);
              xc = -(B / A) * yc - C / A;
            }
            else
            {
              xc = Sol.Value(i);
              yc = -(B / A) * xc - C / A;
            }
            Center = gp_Pnt2d(xc, yc);
            if (OnLine.Distance(Center) > Tol)
              continue;
            NbrSol++;
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center, dirx), Radius);
            //           =======================================================
            double distcc1 = Center.Distance(center1);
            if (!Qualified1.IsUnqualified())
            {
              qualifier1(NbrSol) = Qualified1.Qualifier();
            }
            else if (std::abs(distcc1 + Radius - R1) < Tol)
            {
              qualifier1(NbrSol) = GccEnt_enclosed;
            }
            else if (std::abs(distcc1 - R1 - Radius) < Tol)
            {
              qualifier1(NbrSol) = GccEnt_outside;
            }
            else
            {
              qualifier1(NbrSol) = GccEnt_enclosing;
            }
            gp_Dir2d dir1cen(Center.X() - x1, Center.Y() - y1);
            if ((Radius > R1) || (Center.Distance(center1) > R1))
            {
              pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY() + Radius * dir1cen.XY());
            }
            else
            {
              pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY() - Radius * dir1cen.XY());
            }
            pntcen3(NbrSol) = cirsol(NbrSol).Location();
            par1sol(NbrSol) = ElCLib::Parameter(cirsol(NbrSol), pnttg1sol(NbrSol));
            pararg1(NbrSol) = ElCLib::Parameter(C1, pnttg1sol(NbrSol));
            parcen3(NbrSol) = ElCLib::Parameter(OnLine, pntcen3(NbrSol));
          }
          WellDone = true;
        }
      }
    }
  }
}

bool GccAna_Circ2dTanOnRad::IsDone() const
{
  return WellDone;
}

int GccAna_Circ2dTanOnRad::NbSolutions() const
{
  return NbrSol;
}

gp_Circ2d GccAna_Circ2dTanOnRad::ThisSolution(const int Index) const
{
  if (Index > NbrSol || Index <= 0)
  {
    throw Standard_OutOfRange();
  }
  return cirsol(Index);
}

void GccAna_Circ2dTanOnRad::WhichQualifier(const int Index,
                                           GccEnt_Position&       Qualif1) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  else if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }
  else
  {
    Qualif1 = qualifier1(Index);
  }
}

void GccAna_Circ2dTanOnRad::Tangency1(const int Index,
                                      double&         ParSol,
                                      double&         ParArg,
                                      gp_Pnt2d&              PntSol) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  else if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }
  else
  {
    ParSol = par1sol(Index);
    ParArg = pararg1(Index);
    PntSol = gp_Pnt2d(pnttg1sol(Index));
  }
}

void GccAna_Circ2dTanOnRad::CenterOn3(const int Index,
                                      double&         ParArg,
                                      gp_Pnt2d&              PntSol) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  else if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }
  else
  {
    ParArg = parcen3(Index);
    PntSol = pnttg1sol(Index);
  }
}

bool GccAna_Circ2dTanOnRad::IsTheSame1(const int Index) const
{
  if (!WellDone)
    throw StdFail_NotDone();

  if (Index <= 0 || Index > NbrSol)
    throw Standard_OutOfRange();

  if (TheSame1(Index) == 0)
    return false;

  return true;
}
