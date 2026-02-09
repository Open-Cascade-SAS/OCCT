// Created on: 1995-05-22
// Created by: Laurent BOURESCHE
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

#include <ChFiKPart_ComputeData_Fcts.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Pnt2d.hxx>

#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Surface.hxx>

#include <Standard_NotImplemented.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>

//=================================================================================================

double ChFiKPart_InPeriod(const double U, const double UFirst, const double ULast, const double Eps)
{
  double u = U, period = ULast - UFirst;
  while (Eps < (UFirst - u))
    u += period;
  while (Eps > (ULast - u))
    u -= period;
  if (u < UFirst)
    u = UFirst;
  return u;
}

//=======================================================================
// function : PCurve
// purpose  : Calculate a straight line in form of BSpline to guarantee
//           the parameters.
//=======================================================================

occ::handle<Geom2d_BSplineCurve> ChFiKPart_PCurve(const gp_Pnt2d& UV1,
                                                  const gp_Pnt2d& UV2,
                                                  const double    Pardeb,
                                                  const double    Parfin)
{
  NCollection_Array1<gp_Pnt2d> p(1, 2);
  NCollection_Array1<double>   k(1, 2);
  NCollection_Array1<int>      m(1, 2);
  m.Init(2);
  k(1)                                   = Pardeb;
  k(2)                                   = Parfin;
  p(1)                                   = UV1;
  p(2)                                   = UV2;
  occ::handle<Geom2d_BSplineCurve> Pcurv = new Geom2d_BSplineCurve(p, k, m, 1);
  return Pcurv;
}

//=======================================================================
// function : ProjPC
// purpose  : For spherical corners the contours which of are not
//           isos the circle is projected.
//=======================================================================

void ChFiKPart_ProjPC(const GeomAdaptor_Curve&   Cg,
                      const GeomAdaptor_Surface& Sg,
                      occ::handle<Geom2d_Curve>& Pcurv)
{
  if (Sg.GetType() < GeomAbs_BezierSurface)
  {
    occ::handle<GeomAdaptor_Curve>   HCg = new GeomAdaptor_Curve(Cg);
    occ::handle<GeomAdaptor_Surface> HSg = new GeomAdaptor_Surface(Sg);
    ProjLib_ProjectedCurve           Projc(HSg, HCg);
    switch (Projc.GetType())
    {
      case GeomAbs_Line: {
        Pcurv = new Geom2d_Line(Projc.Line());
      }
      break;
      case GeomAbs_BezierCurve: {
        occ::handle<Geom2d_BezierCurve>      BezProjc = Projc.Bezier();
        const NCollection_Array1<gp_Pnt2d>&  TP       = BezProjc->Poles();
        const NCollection_Array1<double>*    aWPtr    = BezProjc->Weights();
        if (aWPtr != nullptr)
        {
          Pcurv = new Geom2d_BezierCurve(TP, *aWPtr);
        }
        else
        {
          Pcurv = new Geom2d_BezierCurve(TP);
        }
      }
      break;
      case GeomAbs_BSplineCurve: {
        occ::handle<Geom2d_BSplineCurve>    BspProjc = Projc.BSpline();
        const NCollection_Array1<gp_Pnt2d>& TP       = BspProjc->Poles();
        const NCollection_Array1<double>&   TK       = BspProjc->Knots();
        const NCollection_Array1<int>&      TM       = BspProjc->Multiplicities();

        const NCollection_Array1<double>* aWPtr = BspProjc->Weights();
        if (aWPtr != nullptr)
        {
          Pcurv = new Geom2d_BSplineCurve(TP, *aWPtr, TK, TM, BspProjc->Degree());
        }
        else
        {
          Pcurv = new Geom2d_BSplineCurve(TP, TK, TM, BspProjc->Degree());
        }
      }
      break;
      default:
        throw Standard_NotImplemented("failed approximation of the pcurve ");
    }
  }
  else
  {
    throw Standard_NotImplemented("approximate pcurve on the left surface");
  }
}

//=======================================================================
// function : IndexCurveInDS
// purpose  : Place a Curve in the DS and return its index.
//=======================================================================

int ChFiKPart_IndexCurveInDS(const occ::handle<Geom_Curve>& C, TopOpeBRepDS_DataStructure& DStr)
{
  return DStr.AddCurve(TopOpeBRepDS_Curve(C, 0.));
}

//=======================================================================
// function : IndexSurfaceInDS
// purpose  : Place a Surface in the DS and return its index.
//=======================================================================

int ChFiKPart_IndexSurfaceInDS(const occ::handle<Geom_Surface>& S, TopOpeBRepDS_DataStructure& DStr)
{
  return DStr.AddSurface(TopOpeBRepDS_Surface(S, 0.));
}
