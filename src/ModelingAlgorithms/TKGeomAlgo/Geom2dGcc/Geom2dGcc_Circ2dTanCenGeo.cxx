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

#include <Extrema_ExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <Geom2dGcc_Circ2dTanCenGeo.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

//=========================================================================
//   Creation d un cercle tangent a une courbe centre en un point.        +
//=========================================================================
Geom2dGcc_Circ2dTanCenGeo::Geom2dGcc_Circ2dTanCenGeo(const Geom2dGcc_QCurve& Qualified1,
                                                     const gp_Pnt2d&         Pcenter,
                                                     const double            Tolerance)
    :

      //========================================================================
      //   Initialisation des champs.                                          +
      //========================================================================
      WellDone(false),
      NbrSol(0),
      cirsol(1, 2),
      qualifier1(1, 2),
      pnttg1sol(1, 2),
      par1sol(1, 2),
      pararg1(1, 2)
{
  double                       Tol = std::abs(Tolerance);
  NCollection_Array1<gp_Pnt2d> pTan(1, 2);
  NCollection_Array1<int>      Index(1, 2);
  NCollection_Array1<double>   theDist2(1, 2);
  NCollection_Array1<double>   theParam(1, 2);
  theDist2(1)               = RealLast();
  theDist2(2)               = 0.;
  int                 i     = 1;
  int                 nbsol = 0;
  gp_Dir2d            dirx(gp_Dir2d::D::X);
  double              thePar;
  Geom2dAdaptor_Curve curve = Qualified1.Qualified();
  Extrema_ExtPC2d     distmin(Pcenter,
                          curve,
                          Geom2dGcc_CurveTool::FirstParameter(curve),
                          Geom2dGcc_CurveTool::LastParameter(curve),
                          Tol);
  if (!distmin.IsDone())
  {
    throw Standard_Failure();
  }
  int nbext = distmin.NbExt();
  if (nbext == 0)
  {
    throw Standard_Failure();
  }
  while (i <= nbext)
  {
    thePar = distmin.Point(i).Parameter();
    if (distmin.SquareDistance(i) < theDist2(1))
    {
      theDist2(1) = distmin.SquareDistance(i);
      theParam(1) = thePar;
      pTan(1)     = distmin.Point(i).Value();
    }
    if (distmin.SquareDistance(i) > theDist2(2))
    {
      theDist2(2) = distmin.SquareDistance(i);
      theParam(2) = thePar;
      pTan(2)     = distmin.Point(i).Value();
    }
    i++;
  }
  if (Index(1) == Index(2))
  {
    nbsol = 1;
  }
  else
  {
    nbsol = 2;
  }
  for (i = 1; i <= nbsol; i++)
  {
    gp_Pnt2d point1;
    gp_Vec2d Tan1;
    Geom2dGcc_CurveTool::D1(curve, theParam(i), point1, Tan1);
    double   normetan1 = Tan1.Magnitude();
    gp_Vec2d Vec1(point1, Pcenter);
    double   normevec1 = Vec1.Magnitude();
    double   dot1;
    if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution())
    {
      dot1 = Vec1.Dot(Tan1) / (normevec1 * normetan1);
    }
    else
    {
      dot1 = 0.;
    }
    Tol = 1.e-12;
    if (dot1 <= Tol)
    {
      double Angle1 = Vec1.Angle(Tan1);
      if (Qualified1.IsUnqualified() || (Qualified1.IsEnclosing() && Angle1 <= 0.)
          || (Qualified1.IsOutside() && Angle1 >= 0.) || (Qualified1.IsEnclosed() && Angle1 <= 0.))
      {
        NbrSol++;
        cirsol(NbrSol)     = gp_Circ2d(gp_Ax2d(Pcenter, dirx), sqrt(theDist2(i)));
        qualifier1(NbrSol) = Qualified1.Qualifier();
        pararg1(NbrSol)    = theParam(i);
        par1sol(NbrSol)    = 0.;
        pnttg1sol(NbrSol)  = pTan(i);
        WellDone           = true;
      }
    }
  }
}

//=========================================================================

bool Geom2dGcc_Circ2dTanCenGeo::IsDone() const
{
  return WellDone;
}

int Geom2dGcc_Circ2dTanCenGeo::NbSolutions() const
{
  return NbrSol;
}

gp_Circ2d Geom2dGcc_Circ2dTanCenGeo::ThisSolution(const int Index) const
{
  if (Index > NbrSol || Index <= 0)
    throw Standard_OutOfRange();

  return cirsol(Index);
}

void Geom2dGcc_Circ2dTanCenGeo::WhichQualifier(const int Index, GccEnt_Position& Qualif1) const
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

void Geom2dGcc_Circ2dTanCenGeo::Tangency1(const int Index,
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
    PntSol = gp_Pnt2d(pnttg1sol(Index));
    ParSol = par1sol(Index);
    ParArg = pararg1(Index);
  }
}
