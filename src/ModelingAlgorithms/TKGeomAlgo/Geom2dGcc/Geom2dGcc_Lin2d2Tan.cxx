// Created on: 1992-10-21
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

#include <GccAna_Lin2d2Tan.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_Lin2d2Tan.hxx>
#include <Geom2dGcc_Lin2d2TanIter.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//  Modified by Sergey KHROMOV - Wed Oct 16 11:44:41 2002 Begin
Geom2dGcc_Lin2d2Tan::Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1,
                                         const Geom2dGcc_QualifiedCurve& Qualified2,
                                         const double                    Tolang)
    : linsol(1, 4),
      qualifier1(1, 4),
      qualifier2(1, 4),
      pnttg1sol(1, 4),
      pnttg2sol(1, 4),
      par1sol(1, 4),
      par2sol(1, 4),
      pararg1(1, 4),
      pararg2(1, 4)
{
  Geom2dAdaptor_Curve              C1    = Qualified1.Qualified();
  Geom2dAdaptor_Curve              C2    = Qualified2.Qualified();
  const occ::handle<Geom2d_Curve>& CC1   = C1.Curve();
  const occ::handle<Geom2d_Curve>& CC2   = C2.Curve();
  GeomAbs_CurveType                Type1 = C1.GetType();
  GeomAbs_CurveType                Type2 = C2.GetType();

  //=============================================================================
  //                            Appel a GccAna.                                 +
  //=============================================================================

  NbrSol = 0;
  if (Type1 == GeomAbs_Circle && Type2 == GeomAbs_Circle)
  {
    occ::handle<Geom2d_Circle> CCC1 = occ::down_cast<Geom2d_Circle>(CC1);
    gp_Circ2d                  c1(CCC1->Circ2d());
    occ::handle<Geom2d_Circle> CCC2 = occ::down_cast<Geom2d_Circle>(CC2);
    gp_Circ2d                  c2(CCC2->Circ2d());
    GccEnt_QualifiedCirc       Qc1 = GccEnt_QualifiedCirc(c1, Qualified1.Qualifier());
    GccEnt_QualifiedCirc       Qc2 = GccEnt_QualifiedCirc(c2, Qualified2.Qualifier());
    GccAna_Lin2d2Tan           Lin(Qc1, Qc2, Tolang);
    WellDone = Lin.IsDone();
    //  Modified by Sergey KHROMOV - Thu Apr  5 17:50:48 2001 Begin
    if (WellDone)
    {
      //  Modified by Sergey KHROMOV - Thu Apr  5 17:50:49 2001 End
      NbrSol = Lin.NbSolutions();
      for (int i = 1; i <= NbrSol; i++)
      {
        linsol(i) = Lin.ThisSolution(i);
        Lin.Tangency1(i, par1sol(i), pararg1(i), pnttg1sol(i));
        Lin.Tangency2(i, par2sol(i), pararg2(i), pnttg2sol(i));
        Lin.WhichQualifier(i, qualifier1(i), qualifier2(i));
      }
    }
  }
  else
  {
    Geom2dGcc_QCurve Qc1(C1, Qualified1.Qualifier());
    double           a1FPar      = Geom2dGcc_CurveTool::FirstParameter(C1);
    double           a1LPar      = Geom2dGcc_CurveTool::LastParameter(C1);
    int              aNbSamples1 = Geom2dGcc_CurveTool::NbSamples(C1);
    double           aStep1      = (a1LPar - a1FPar) / aNbSamples1;
    double           Param1      = a1FPar;
    Geom2dGcc_QCurve Qc2(C2, Qualified2.Qualifier());
    double           a2FPar      = Geom2dGcc_CurveTool::FirstParameter(C2);
    double           a2LPar      = Geom2dGcc_CurveTool::LastParameter(C2);
    int              aNbSamples2 = Geom2dGcc_CurveTool::NbSamples(C2);
    double           aStep2      = (a2LPar - a2FPar) / aNbSamples2;
    double           Param2      = a2FPar;
    int              i;
    int              j;

    for (i = 0; i <= aNbSamples1 && NbrSol < 4; i++)
    {
      Param2 = a2FPar;

      for (j = 0; j <= aNbSamples2 && NbrSol < 4; j++)
      {
        Geom2dGcc_Lin2d2TanIter Lin(Qc1, Qc2, Param1, Param2, Tolang);

        if (Lin.IsDone())
        {
          if (Add(NbrSol + 1, Lin, Tolang, C1, C2))
            NbrSol++;
        }
        Param2 += aStep2;
      }
      Param1 += aStep1;
    }

    WellDone = (NbrSol > 0);
  }
}

Geom2dGcc_Lin2d2Tan::Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1,
                                         const gp_Pnt2d&                 ThePoint,
                                         const double                    Tolang)
    : linsol(1, 2),
      qualifier1(1, 2),
      qualifier2(1, 2),
      pnttg1sol(1, 2),
      pnttg2sol(1, 2),
      par1sol(1, 2),
      par2sol(1, 2),
      pararg1(1, 2),
      pararg2(1, 2)
{
  Geom2dAdaptor_Curve              C1    = Qualified1.Qualified();
  const occ::handle<Geom2d_Curve>& CC1   = C1.Curve();
  GeomAbs_CurveType                Type1 = C1.GetType();

  //=============================================================================
  //                            Appel a GccAna.                                 +
  //=============================================================================

  NbrSol = 0;
  if (Type1 == GeomAbs_Circle)
  {
    occ::handle<Geom2d_Circle> CCC1 = occ::down_cast<Geom2d_Circle>(CC1);
    gp_Circ2d                  c1(CCC1->Circ2d());
    GccEnt_QualifiedCirc       Qc1 = GccEnt_QualifiedCirc(c1, Qualified1.Qualifier());
    GccAna_Lin2d2Tan           Lin(Qc1, ThePoint, Tolang);
    WellDone = Lin.IsDone();

    if (WellDone)
    {
      NbrSol = Lin.NbSolutions();
      for (int i = 1; i <= NbrSol; i++)
      {
        linsol(i) = Lin.ThisSolution(i);
        Lin.Tangency1(i, par1sol(i), pararg1(i), pnttg1sol(i));
        Lin.Tangency2(i, par2sol(i), pararg2(i), pnttg2sol(i));
        Lin.WhichQualifier(i, qualifier1(i), qualifier2(i));
      }
    }
  }
  else
  {
    Geom2dGcc_QCurve Qc1(C1, Qualified1.Qualifier());
    double           aFirstPar  = Geom2dGcc_CurveTool::FirstParameter(C1);
    double           aLastPar   = Geom2dGcc_CurveTool::LastParameter(C1);
    int              aNbSamples = Geom2dGcc_CurveTool::NbSamples(C1);
    double           aStep      = (aLastPar - aFirstPar) / aNbSamples;
    double           Param1     = aFirstPar;
    int              i;

    for (i = 0; i <= aNbSamples && NbrSol < 2; i++)
    {
      Geom2dGcc_Lin2d2TanIter Lin(Qc1, ThePoint, Param1, Tolang);

      if (Lin.IsDone())
      {
        if (Add(NbrSol + 1, Lin, Tolang, C1, Geom2dAdaptor_Curve()))
          NbrSol++;
      }

      Param1 += aStep;
    }

    WellDone = (NbrSol > 0);
  }
}

//  Modified by Sergey KHROMOV - Wed Oct 16 11:44:41 2002 End

Geom2dGcc_Lin2d2Tan::Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1,
                                         const Geom2dGcc_QualifiedCurve& Qualified2,
                                         const double                    Tolang,
                                         const double                    Param1,
                                         const double                    Param2)
    : linsol(1, 4),
      qualifier1(1, 4),
      qualifier2(1, 4),
      pnttg1sol(1, 4),
      pnttg2sol(1, 4),
      par1sol(1, 4),
      par2sol(1, 4),
      pararg1(1, 4),
      pararg2(1, 4)
{
  Geom2dAdaptor_Curve              C1    = Qualified1.Qualified();
  Geom2dAdaptor_Curve              C2    = Qualified2.Qualified();
  const occ::handle<Geom2d_Curve>& CC1   = C1.Curve();
  const occ::handle<Geom2d_Curve>& CC2   = C2.Curve();
  GeomAbs_CurveType                Type1 = C1.GetType();
  GeomAbs_CurveType                Type2 = C2.GetType();

  //=============================================================================
  //                            Appel a GccAna.                                 +
  //=============================================================================

  NbrSol = 0;
  if (Type1 == GeomAbs_Circle && Type2 == GeomAbs_Circle)
  {
    occ::handle<Geom2d_Circle> CCC1 = occ::down_cast<Geom2d_Circle>(CC1);
    gp_Circ2d                  c1(CCC1->Circ2d());
    occ::handle<Geom2d_Circle> CCC2 = occ::down_cast<Geom2d_Circle>(CC2);
    gp_Circ2d                  c2(CCC2->Circ2d());
    GccEnt_QualifiedCirc       Qc1 = GccEnt_QualifiedCirc(c1, Qualified1.Qualifier());
    GccEnt_QualifiedCirc       Qc2 = GccEnt_QualifiedCirc(c2, Qualified2.Qualifier());
    GccAna_Lin2d2Tan           Lin(Qc1, Qc2, Tolang);
    WellDone = Lin.IsDone();
    //  Modified by Sergey KHROMOV - Thu Apr  5 17:50:48 2001 Begin
    if (WellDone)
    {
      //  Modified by Sergey KHROMOV - Thu Apr  5 17:50:49 2001 End
      NbrSol = Lin.NbSolutions();
      for (int i = 1; i <= NbrSol; i++)
      {
        linsol(i) = Lin.ThisSolution(i);
        Lin.Tangency1(i, par1sol(i), pararg1(i), pnttg1sol(i));
        Lin.Tangency2(i, par2sol(i), pararg2(i), pnttg2sol(i));
        Lin.WhichQualifier(i, qualifier1(i), qualifier2(i));
      }
    }
  }
  else
  {
    Geom2dGcc_QCurve        Qc1(C1, Qualified1.Qualifier());
    Geom2dGcc_QCurve        Qc2(C2, Qualified2.Qualifier());
    Geom2dGcc_Lin2d2TanIter Lin(Qc1, Qc2, Param1, Param2, Tolang);
    WellDone = Lin.IsDone();
    //  Modified by Sergey KHROMOV - Thu Apr  5 17:51:59 2001 Begin
    if (WellDone)
    {
      //  Modified by Sergey KHROMOV - Thu Apr  5 17:52:00 2001 End
      NbrSol    = 1;
      linsol(1) = Lin.ThisSolution();
      Lin.Tangency1(par1sol(1), pararg1(1), pnttg1sol(1));
      Lin.Tangency2(par2sol(1), pararg2(1), pnttg2sol(1));
      Lin.WhichQualifier(qualifier1(1), qualifier2(1));
    }
  }
}

Geom2dGcc_Lin2d2Tan::Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1,
                                         const gp_Pnt2d&                 ThePoint,
                                         const double                    Tolang,
                                         const double                    Param1)
    : linsol(1, 2),
      qualifier1(1, 2),
      qualifier2(1, 2),
      pnttg1sol(1, 2),
      pnttg2sol(1, 2),
      par1sol(1, 2),
      par2sol(1, 2),
      pararg1(1, 2),
      pararg2(1, 2)
{
  Geom2dAdaptor_Curve              C1    = Qualified1.Qualified();
  const occ::handle<Geom2d_Curve>& CC1   = C1.Curve();
  GeomAbs_CurveType                Type1 = C1.GetType();

  //=============================================================================
  //                            Appel a GccAna.                                 +
  //=============================================================================

  NbrSol = 0;
  if (Type1 == GeomAbs_Circle)
  {
    occ::handle<Geom2d_Circle> CCC1 = occ::down_cast<Geom2d_Circle>(CC1);
    gp_Circ2d                  c1(CCC1->Circ2d());
    GccEnt_QualifiedCirc       Qc1 = GccEnt_QualifiedCirc(c1, Qualified1.Qualifier());
    GccAna_Lin2d2Tan           Lin(Qc1, ThePoint, Tolang);
    WellDone = Lin.IsDone();
    //  Modified by Sergey KHROMOV - Thu Apr  5 17:52:32 2001 Begin
    if (WellDone)
    {
      //  Modified by Sergey KHROMOV - Thu Apr  5 17:52:34 2001 End
      NbrSol = Lin.NbSolutions();
      for (int i = 1; i <= NbrSol; i++)
      {
        linsol(i) = Lin.ThisSolution(i);
        Lin.Tangency1(i, par1sol(i), pararg1(i), pnttg1sol(i));
        Lin.Tangency2(i, par2sol(i), pararg2(i), pnttg2sol(i));
        Lin.WhichQualifier(i, qualifier1(i), qualifier2(i));
      }
    }
  }
  else
  {
    Geom2dGcc_QCurve        Qc1(C1, Qualified1.Qualifier());
    Geom2dGcc_Lin2d2TanIter Lin(Qc1, ThePoint, Param1, Tolang);
    WellDone = Lin.IsDone();
    //  Modified by Sergey KHROMOV - Thu Apr  5 17:53:01 2001 Begin
    if (WellDone)
    {
      //  Modified by Sergey KHROMOV - Thu Apr  5 17:53:02 2001 End
      NbrSol    = 1;
      linsol(1) = Lin.ThisSolution();
      Lin.Tangency1(par1sol(1), pararg1(1), pnttg1sol(1));
      Lin.Tangency2(par2sol(1), pararg2(1), pnttg2sol(1));
      Lin.WhichQualifier(qualifier1(1), qualifier2(1));
    }
  }
}

bool Geom2dGcc_Lin2d2Tan::IsDone() const
{
  return WellDone;
}

int Geom2dGcc_Lin2d2Tan::NbSolutions() const
{
  return NbrSol;
}

gp_Lin2d Geom2dGcc_Lin2d2Tan::ThisSolution(const int Index) const
{
  if (Index > NbrSol || Index <= 0)
  {
    throw Standard_OutOfRange();
  }
  return linsol(Index);
}

void Geom2dGcc_Lin2d2Tan::WhichQualifier(const int        Index,
                                         GccEnt_Position& Qualif1,
                                         GccEnt_Position& Qualif2) const
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
    Qualif2 = qualifier2(Index);
  }
}

void Geom2dGcc_Lin2d2Tan::Tangency1(const int Index,
                                    double&   ParSol,
                                    double&   ParArg,
                                    gp_Pnt2d& PntSol) const
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
    PntSol = pnttg1sol(Index);
  }
}

void Geom2dGcc_Lin2d2Tan::Tangency2(const int Index,
                                    double&   ParSol,
                                    double&   ParArg,
                                    gp_Pnt2d& PntSol) const
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
    ParSol = par2sol(Index);
    ParArg = pararg2(Index);
    PntSol = pnttg2sol(Index);
  }
}

bool Geom2dGcc_Lin2d2Tan::Add(const int                      theIndex,
                              const Geom2dGcc_Lin2d2TanIter& theLin,
                              const double                   theTol,
                              const Geom2dAdaptor_Curve&     theC1,
                              const Geom2dAdaptor_Curve&     theC2)
{
  int      i;
  double   aPar1sol;
  double   aPar2sol;
  double   aPar1arg;
  double   aPar2arg;
  gp_Pnt2d aPnt1Sol;
  gp_Pnt2d aPnt2Sol;
  gp_Lin2d aLin = theLin.ThisSolution();

  theLin.Tangency1(aPar1sol, aPar1arg, aPnt1Sol);
  theLin.Tangency2(aPar2sol, aPar2arg, aPnt2Sol);

  for (i = 1; i < theIndex; i++)
  {
    if (std::abs(aPar1arg - pararg1(i)) <= theTol && std::abs(aPar2arg - pararg2(i)) <= theTol)
      return false;
  }

  gp_Dir2d aLinDir = aLin.Direction();
  gp_Vec2d aVTan;
  gp_Pnt2d aPoint;

  Geom2dGcc_CurveTool::D1(theC1, aPar1arg, aPoint, aVTan);

  if (std::abs(aLinDir.Crossed(gp_Dir2d(aVTan))) > theTol)
    return false;

  if (!theC2.Curve().IsNull())
  {
    Geom2dGcc_CurveTool::D1(theC2, aPar2arg, aPoint, aVTan);

    if (std::abs(aLinDir.Crossed(gp_Dir2d(aVTan))) > theTol)
      return false;
  }

  linsol(theIndex)    = aLin;
  par1sol(theIndex)   = aPar1sol;
  pararg1(theIndex)   = aPar1arg;
  pnttg1sol(theIndex) = aPnt1Sol;
  par2sol(theIndex)   = aPar2sol;
  pararg2(theIndex)   = aPar2arg;
  pnttg2sol(theIndex) = aPnt2Sol;

  theLin.WhichQualifier(qualifier1(theIndex), qualifier2(theIndex));

  return true;
}
