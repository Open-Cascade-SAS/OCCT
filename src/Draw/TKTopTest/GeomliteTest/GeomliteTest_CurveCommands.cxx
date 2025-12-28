// Created on: 1993-08-12
// Created by: Bruno DUMORTIER
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

// Modified by xab, Tue Mar 11 18:31:18 1997
// Modified by PMN 14/04/97 : Passage a Geomlite
// Modified by JPI 01/08/97 : ajout de la commande approxcurve

#include <GeomliteTest.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_BezierCurve.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_BezierCurve2d.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>

#include <gp.hxx>

#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BezierCurve.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <GeomLib.hxx>
#include <Geom2dConvert.hxx>

#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#include <GeomLProp.hxx>
#include <GeomLProp_CLProps.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Integer.hxx>

#include <Precision.hxx>

#include <stdio.h>

#include <NCollection_HArray1.hxx>
#include <GCPnts_AbscissaPoint.hxx>

#include <GeomAbs_Shape.hxx>
#include <Geom_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2d_Curve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Approx_CurvilinearParameter.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <Geom_BSplineSurface.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Approx_FitAndDivide.hxx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

// Class is used in fitcurve
class CurveEvaluator : public AppCont_Function

{

public:
  occ::handle<Adaptor3d_Curve> myCurve;

  CurveEvaluator(const occ::handle<Adaptor3d_Curve>& C)
      : myCurve(C)
  {
    myNbPnt   = 1;
    myNbPnt2d = 0;
  }

  double FirstParameter() const { return myCurve->FirstParameter(); }

  double LastParameter() const { return myCurve->LastParameter(); }

  bool Value(const double theT,
             NCollection_Array1<gp_Pnt2d>& /*thePnt2d*/,
             NCollection_Array1<gp_Pnt>& thePnt) const
  {
    thePnt(1) = myCurve->Value(theT);
    return true;
  }

  bool D1(const double theT,
          NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
          NCollection_Array1<gp_Vec>& theVec) const
  {
    gp_Pnt aDummyPnt;
    myCurve->D1(theT, aDummyPnt, theVec(1));
    return true;
  }
};

//=================================================================================================

static int anacurve(Draw_Interpretor&, int n, const char** a)
{
  if (n < 5)
    return 1;

  occ::handle<Geom_Geometry> result;
  occ::handle<Geom2d_Curve>  result2d;

  if (!strcmp(a[0], "line"))
  {
    if (n == 6)
      result2d = new Geom2d_Line(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])),
                                 gp_Dir2d(Draw::Atof(a[4]), Draw::Atof(a[5])));
    else if (n == 8)
      result = new Geom_Line(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                             gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7])));
    else
      return 1;
  }

  else if (!strcmp(a[0], "circle"))
  {
    if (n == 5)
      result2d = new Geom2d_Circle(
        gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])), gp_Dir2d(gp_Dir2d::D::X)),
        Draw::Atof(a[4]));
    else if (n == 6)
      result = new Geom_Circle(
        gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])), gp_Dir(gp_Dir::D::Z)),
        Draw::Atof(a[5]));
    else if (n == 7)
      result2d = new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])),
                                            gp_Dir2d(Draw::Atof(a[4]), Draw::Atof(a[5]))),
                                   Draw::Atof(a[6]));
    else if (n == 9)
      result = new Geom_Circle(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                                      gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7]))),
                               Draw::Atof(a[8]));
    else if (n == 12)
      result =
        new Geom_Circle(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                               gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7])),
                               gp_Dir(Draw::Atof(a[8]), Draw::Atof(a[9]), Draw::Atof(a[10]))),
                        Draw::Atof(a[11]));
    else
      return 1;
  }

  else if (!strcmp(a[0], "parabola"))
  {
    if (n == 5)
      result2d = new Geom2d_Parabola(
        gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])), gp_Dir2d(gp_Dir2d::D::X)),
        Draw::Atof(a[4]));
    else if (n == 6)
      result = new Geom_Parabola(
        gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])), gp_Dir(gp_Dir::D::Z)),
        Draw::Atof(a[5]));
    else if (n == 7)
      result2d = new Geom2d_Parabola(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])),
                                              gp_Dir2d(Draw::Atof(a[4]), Draw::Atof(a[5]))),
                                     Draw::Atof(a[6]));
    else if (n == 9)
      result =
        new Geom_Parabola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                                 gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7]))),
                          Draw::Atof(a[8]));
    else if (n == 12)
      result =
        new Geom_Parabola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                                 gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7])),
                                 gp_Dir(Draw::Atof(a[8]), Draw::Atof(a[9]), Draw::Atof(a[10]))),
                          Draw::Atof(a[11]));
    else
      return 1;
  }

  else if (!strcmp(a[0], "ellipse"))
  {
    if (n == 6)
      result2d = new Geom2d_Ellipse(
        gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])), gp_Dir2d(gp_Dir2d::D::X)),
        Draw::Atof(a[4]),
        Draw::Atof(a[5]));
    else if (n == 7)
      result = new Geom_Ellipse(
        gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])), gp_Dir(gp_Dir::D::Z)),
        Draw::Atof(a[5]),
        Draw::Atof(a[6]));
    else if (n == 8)
      result2d = new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])),
                                             gp_Dir2d(Draw::Atof(a[4]), Draw::Atof(a[5]))),
                                    Draw::Atof(a[6]),
                                    Draw::Atof(a[7]));
    else if (n == 10)
      result =
        new Geom_Ellipse(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                                gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7]))),
                         Draw::Atof(a[8]),
                         Draw::Atof(a[9]));
    else if (n == 13)
      result =
        new Geom_Ellipse(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                                gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7])),
                                gp_Dir(Draw::Atof(a[8]), Draw::Atof(a[9]), Draw::Atof(a[10]))),
                         Draw::Atof(a[11]),
                         Draw::Atof(a[12]));
    else
      return 1;
  }

  else if (!strcmp(a[0], "hyperbola"))
  {
    if (n == 6)
      result2d = new Geom2d_Hyperbola(
        gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])), gp_Dir2d(gp_Dir2d::D::X)),
        Draw::Atof(a[4]),
        Draw::Atof(a[5]));
    else if (n == 7)
      result = new Geom_Hyperbola(
        gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])), gp_Dir(gp_Dir::D::Z)),
        Draw::Atof(a[5]),
        Draw::Atof(a[6]));
    else if (n == 8)
      result2d = new Geom2d_Hyperbola(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]), Draw::Atof(a[3])),
                                               gp_Dir2d(Draw::Atof(a[4]), Draw::Atof(a[5]))),
                                      Draw::Atof(a[6]),
                                      Draw::Atof(a[7]));
    else if (n == 10)
      result =
        new Geom_Hyperbola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                                  gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7]))),
                           Draw::Atof(a[8]),
                           Draw::Atof(a[9]));
    else if (n == 13)
      result =
        new Geom_Hyperbola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4])),
                                  gp_Dir(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7])),
                                  gp_Dir(Draw::Atof(a[8]), Draw::Atof(a[9]), Draw::Atof(a[10]))),
                           Draw::Atof(a[11]),
                           Draw::Atof(a[12]));
    else
      return 1;
  }

  if (!result.IsNull())
    DrawTrSurf::Set(a[1], result);
  else if (!result2d.IsNull())
    DrawTrSurf::Set(a[1], result2d);
  else
    return 1;

  return 0;
}

//=================================================================================================

static int polecurve(Draw_Interpretor&, int n, const char** a)
{
  int k, i;

  if (n < 3)
    return 1;

  if (!strcmp(a[0], "beziercurve"))
  {

    int np = Draw::Atoi(a[2]);
    if (np == 0)
      return 1;

    i = (n - 3) / (np);
    if (i < 3 || i > 4)
      return 1;
    bool hasw = i == 4;

    NCollection_Array1<gp_Pnt> poles(1, np);
    NCollection_Array1<double> weights(1, np);

    k = 3;
    for (i = 1; i <= np; i++)
    {
      poles(i).SetCoord(Draw::Atof(a[k]), Draw::Atof(a[k + 1]), Draw::Atof(a[k + 2]));
      k += 3;
      if (hasw)
      {
        weights(i) = Draw::Atof(a[k]);
        k++;
      }
    }

    occ::handle<Geom_BezierCurve> result;
    if (hasw)
      result = new Geom_BezierCurve(poles, weights);
    else
      result = new Geom_BezierCurve(poles);

    DrawTrSurf::Set(a[1], result);
  }

  else if (!strcmp((*a[0] == 'p') ? a[0] + 1 : a[0], "bsplinecurve"))
  {
    int deg = Draw::Atoi(a[2]);
    int nbk = Draw::Atoi(a[3]);

    NCollection_Array1<double> knots(1, nbk);
    NCollection_Array1<int>    mults(1, nbk);
    k         = 4;
    int Sigma = 0;
    for (i = 1; i <= nbk; i++)
    {
      knots(i) = Draw::Atof(a[k]);
      k++;
      mults(i) = Draw::Atoi(a[k]);
      Sigma += mults(i);
      k++;
    }

    bool periodic = *a[0] == 'p';
    int  np;
    if (periodic)
      np = Sigma - mults(nbk);
    else
      np = Sigma - deg - 1;

    NCollection_Array1<gp_Pnt> poles(1, np);
    NCollection_Array1<double> weights(1, np);

    for (i = 1; i <= np; i++)
    {
      poles(i).SetCoord(Draw::Atof(a[k]), Draw::Atof(a[k + 1]), Draw::Atof(a[k + 2]));
      k += 3;
      weights(i) = Draw::Atof(a[k]);
      k++;
    }

    occ::handle<Geom_BSplineCurve> result =
      new Geom_BSplineCurve(poles, weights, knots, mults, deg, periodic);
    DrawTrSurf::Set(a[1], result);
  }

  return 0;
}

//=================================================================================================

static int polecurve2d(Draw_Interpretor&, int n, const char** a)
{
  int k, i;

  if (n < 3)
    return 1;

  if (!strcmp(a[0], "2dbeziercurve"))
  {

    int np = Draw::Atoi(a[2]);
    if (np == 0)
      return 1;

    i = (n - 2) / (np);
    if (i < 2 || i > 3)
      return 1;
    bool hasw = i == 3;

    NCollection_Array1<gp_Pnt2d> poles(1, np);
    NCollection_Array1<double>   weights(1, np);

    k = 3;
    for (i = 1; i <= np; i++)
    {
      poles(i).SetCoord(Draw::Atof(a[k]), Draw::Atof(a[k + 1]));
      k += 2;
      if (hasw)
      {
        weights(i) = Draw::Atof(a[k]);
        k++;
      }
    }

    occ::handle<Geom2d_BezierCurve> result;
    if (hasw)
      result = new Geom2d_BezierCurve(poles, weights);
    else
      result = new Geom2d_BezierCurve(poles);

    DrawTrSurf::Set(a[1], result);
  }

  else if (!strcmp((*(a[0] + 2) == 'p') ? a[0] + 3 : a[0] + 2, "bsplinecurve"))
  {
    int deg = Draw::Atoi(a[2]);
    int nbk = Draw::Atoi(a[3]);

    NCollection_Array1<double> knots(1, nbk);
    NCollection_Array1<int>    mults(1, nbk);
    k         = 4;
    int Sigma = 0;
    for (i = 1; i <= nbk; i++)
    {
      knots(i) = Draw::Atof(a[k]);
      k++;
      mults(i) = Draw::Atoi(a[k]);
      Sigma += mults(i);
      k++;
    }

    bool periodic = *(a[0] + 2) == 'p';
    int  np;
    if (periodic)
      np = Sigma - mults(nbk);
    else
      np = Sigma - deg - 1;

    NCollection_Array1<gp_Pnt2d> poles(1, np);
    NCollection_Array1<double>   weights(1, np);

    for (i = 1; i <= np; i++)
    {
      poles(i).SetCoord(Draw::Atof(a[k]), Draw::Atof(a[k + 1]));
      k += 2;
      weights(i) = Draw::Atof(a[k]);
      k++;
    }

    occ::handle<Geom2d_BSplineCurve> result =
      new Geom2d_BSplineCurve(poles, weights, knots, mults, deg, periodic);
    DrawTrSurf::Set(a[1], result);
  }

  return 0;
}

//=================================================================================================

static int reverse(Draw_Interpretor&, int n, const char** a)
{
  if (n < 2)
    return 1;

  int i;
  for (i = 1; i < n; i++)
  {

    occ::handle<Geom_Curve> GC = DrawTrSurf::GetCurve(a[i]);
    if (!GC.IsNull())
    {
      GC->Reverse();
      Draw::Repaint();
    }
    occ::handle<Geom2d_Curve> GC2d = DrawTrSurf::GetCurve2d(a[i]);
    if (!GC2d.IsNull())
    {
      GC2d->Reverse();
      Draw::Repaint();
    }
  }
  return 0;
}

//=================================================================================================

static int cmovepole(Draw_Interpretor&, int n, const char** a)
{
  if (n < 5)
    return 1;

  double dx = Draw::Atof(a[3]);
  double dy = Draw::Atof(a[4]);
  double dz = 0;
  if (n >= 6)
    dz = Draw::Atof(a[5]);
  int Index = Draw::Atoi(a[2]);

  occ::handle<Geom_BezierCurve> G1 = DrawTrSurf::GetBezierCurve(a[1]);
  if (!G1.IsNull())
  {
    gp_Pnt P = G1->Pole(Index);
    P.SetCoord(P.X() + dx, P.Y() + dy, P.Z() + dz);
    G1->SetPole(Index, P);
    Draw::Repaint();
    return 0;
  }

  occ::handle<Geom_BSplineCurve> G2 = DrawTrSurf::GetBSplineCurve(a[1]);
  if (!G2.IsNull())
  {
    gp_Pnt P = G2->Pole(Index);
    P.SetCoord(P.X() + dx, P.Y() + dy, P.Z() + dz);
    G2->SetPole(Index, P);
    Draw::Repaint();
    return 0;
  }

  occ::handle<Geom2d_BezierCurve> G3 = DrawTrSurf::GetBezierCurve2d(a[1]);
  if (!G3.IsNull())
  {
    gp_Pnt2d P = G3->Pole(Index);
    P.SetCoord(P.X() + dx, P.Y() + dy);
    G3->SetPole(Index, P);
    Draw::Repaint();
    return 0;
  }

  occ::handle<Geom2d_BSplineCurve> G4 = DrawTrSurf::GetBSplineCurve2d(a[1]);
  if (!G4.IsNull())
  {
    gp_Pnt2d P = G4->Pole(Index);
    P.SetCoord(P.X() + dx, P.Y() + dy);
    G4->SetPole(Index, P);
    Draw::Repaint();
    return 0;
  }

  return 0;
}

//=================================================================================================

static int cmovetangent(Draw_Interpretor& di, int n, const char** a)
{
  int    dimension, condition, error_status;
  double u, x, y, z, tolerance, tx, ty, tz;
  u = Draw::Atof(a[2]);
  x = Draw::Atof(a[3]);
  y = Draw::Atof(a[4]);
  z = 0.0e0, tolerance = 1.0e-5;
  dimension = 3;
  if (n <= 8)
  {
    dimension = 2;
    if (n < 7)
    {
      return 1;
    }
  }
  else
  {
    dimension = 3;
    if (n < 9)
    {
      return 1;
    }
  }
  condition    = 0;
  error_status = 0;
  if (dimension == 3)
  {
    occ::handle<Geom_BSplineCurve> G2 = DrawTrSurf::GetBSplineCurve(a[1]);
    if (!G2.IsNull())
    {
      z  = Draw::Atof(a[5]);
      tx = Draw::Atof(a[6]);
      ty = Draw::Atof(a[7]);
      tz = Draw::Atof(a[8]);
      if (n == 10)
      {
        condition = std::max(Draw::Atoi(a[9]), -1);
        condition = std::min(condition, G2->Degree() - 1);
      }
      gp_Pnt p;
      gp_Vec tangent;
      p.SetCoord(x, y, z);
      tangent.SetCoord(tx, ty, tz);

      G2->MovePointAndTangent(u, p, tangent, tolerance, condition, condition, error_status);
      if (!error_status)
      {
        Draw::Repaint();
      }
      else
      {
        di << "Not enough degree of freedom increase degree please\n";
      }

      return 0;
    }
  }
  else
  {
    occ::handle<Geom2d_BSplineCurve> G2 = DrawTrSurf::GetBSplineCurve2d(a[1]);
    if (!G2.IsNull())
    {
      tx = Draw::Atof(a[5]);
      ty = Draw::Atof(a[6]);
      if (n == 8)
      {
        condition = std::max(Draw::Atoi(a[7]), -1);
        condition = std::min(condition, G2->Degree() - 1);
      }
      gp_Pnt2d p;
      gp_Vec2d tangent;
      p.SetCoord(x, y);
      tangent.SetCoord(tx, ty);

      G2->MovePointAndTangent(u, p, tangent, tolerance, condition, condition, error_status);
      if (!error_status)
      {
        Draw::Repaint();
      }
      else
      {
        di << "Not enough degree of freedom increase degree please\n";
      }

      return 0;
    }
  }
  return 0;
}

//=================================================================================================

static int cmovepoint(Draw_Interpretor&, int n, const char** a)
{
  if (n < 5)
    return 1;

  double dx = Draw::Atof(a[3]);
  double dy = Draw::Atof(a[4]);
  double dz = 0;
  if (n >= 6 && n != 7)
    dz = Draw::Atof(a[5]);
  double u      = Draw::Atof(a[2]);
  int    index1 = 0;
  int    index2 = 0;
  int    fmodif, lmodif;
  if (n == 7)
  {
    index1 = Draw::Atoi(a[5]);
    index2 = Draw::Atoi(a[6]);
  }
  else if (n == 8)
  {
    index1 = Draw::Atoi(a[6]);
    index2 = Draw::Atoi(a[7]);
  }

  occ::handle<Geom_BSplineCurve> G2 = DrawTrSurf::GetBSplineCurve(a[1]);
  if (!G2.IsNull())
  {
    if (index1 == 0)
    {
      index1 = 2;
      index2 = G2->NbPoles() - 1;
    }
    gp_Pnt p;
    G2->D0(u, p);
    p.SetCoord(p.X() + dx, p.Y() + dy, p.Z() + dz);
    G2->MovePoint(u, p, index1, index2, fmodif, lmodif);
    Draw::Repaint();
    return 0;
  }

  occ::handle<Geom2d_BSplineCurve> G4 = DrawTrSurf::GetBSplineCurve2d(a[1]);
  if (!G4.IsNull())
  {
    if (index1 == 0)
    {
      index1 = 2;
      index2 = G4->NbPoles() - 1;
    }
    gp_Pnt2d p;
    G4->D0(u, p);
    p.SetCoord(p.X() + dx, p.Y() + dy);
    G4->MovePoint(u, p, index1, index2, fmodif, lmodif);
    Draw::Repaint();
    return 0;
  }
  return 0;
}

//=================================================================================================

static int cinsertknot(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom_BSplineCurve>   GBs   = DrawTrSurf::GetBSplineCurve(a[1]);
  occ::handle<Geom2d_BSplineCurve> GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (GBs.IsNull() && GBs2d.IsNull())
    return 1;

  if (n <= 4)
  {
    double knot = Draw::Atof(a[2]);
    int    mult = 1;
    if (n == 4)
      mult = Draw::Atoi(a[3]);
    if (!GBs.IsNull())
      GBs->InsertKnot(knot, mult, Precision::PConfusion());
    else
      GBs2d->InsertKnot(knot, mult, Precision::PConfusion());
  }

  else
  {
    // multiple insertion
    if (n % 2 != 0)
      return 1;
    int                        i, nbk = (n - 2) / 2;
    NCollection_Array1<double> knots(1, nbk);
    NCollection_Array1<int>    mults(1, nbk);
    for (i = 2; i < n; i += 2)
    {
      knots(i / 2) = Draw::Atof(a[i]);
      mults(i / 2) = Draw::Atoi(a[i + 1]);
    }

    if (!GBs.IsNull())
      GBs->InsertKnots(knots, mults, Precision::PConfusion());
    else
      GBs2d->InsertKnots(knots, mults, Precision::PConfusion());
  }

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int csetknot(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom_BSplineCurve>   GBs   = DrawTrSurf::GetBSplineCurve(a[1]);
  occ::handle<Geom2d_BSplineCurve> GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (GBs.IsNull() && GBs2d.IsNull())
    return 1;

  int    index = Draw::Atoi(a[2]);
  double knot  = Draw::Atof(a[3]);

  if (n == 4)
  {
    if (!GBs.IsNull())
      GBs->SetKnot(index, knot);
    else
      GBs2d->SetKnot(index, knot);
  }
  else
  {
    int mult = Draw::Atoi(a[4]);
    if (!GBs.IsNull())
      GBs->SetKnot(index, knot, mult);
    else
      GBs2d->SetKnot(index, knot, mult);
  }

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int cremknot(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  occ::handle<Geom_BSplineCurve>   GBs   = DrawTrSurf::GetBSplineCurve(a[1]);
  occ::handle<Geom2d_BSplineCurve> GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (GBs.IsNull() && GBs2d.IsNull())
    return 1;

  int index = Draw::Atoi(a[2]);
  int mult  = 0;
  if (n >= 4)
    mult = Draw::Atoi(a[3]);

  double tol = RealLast();
  if (n >= 5)
    tol = Draw::Atof(a[4]);

  if (!GBs.IsNull())
  {
    if (!GBs->RemoveKnot(index, mult, tol))
      di << "Remove knots failed\n";
  }
  else
  {
    if (!GBs2d->RemoveKnot(index, mult, tol))
      di << "Remove knots failed\n";
  }

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int increasedegree(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;

  int Deg = Draw::Atoi(a[2]);

  occ::handle<Geom_BezierCurve>    GBz   = DrawTrSurf::GetBezierCurve(a[1]);
  occ::handle<Geom_BSplineCurve>   GBs   = DrawTrSurf::GetBSplineCurve(a[1]);
  occ::handle<Geom2d_BezierCurve>  GBz2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  occ::handle<Geom2d_BSplineCurve> GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (!GBz.IsNull())
    GBz->Increase(Deg);
  else if (!GBs.IsNull())
    GBs->IncreaseDegree(Deg);
  else if (!GBz2d.IsNull())
    GBz2d->Increase(Deg);
  else if (!GBs2d.IsNull())
    GBs2d->IncreaseDegree(Deg);
  else
    return 1;

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int removepole(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  int Index = Draw::Atoi(a[2]);

  occ::handle<Geom_BezierCurve>   GBZ   = DrawTrSurf::GetBezierCurve(a[1]);
  occ::handle<Geom2d_BezierCurve> GBZ2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  if (!GBZ.IsNull())
  {
    GBZ->RemovePole(Index);
  }
  else if (!GBZ2d.IsNull())
  {
    GBZ2d->RemovePole(Index);
  }
  else
  {
    di << "rempole needs a bezier curve";
    return 1;
  }

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int insertpole(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 6)
    return 1;

  int Index = Draw::Atoi(a[2]);

  occ::handle<Geom_BezierCurve>   GBZ   = DrawTrSurf::GetBezierCurve(a[1]);
  occ::handle<Geom2d_BezierCurve> GBZ2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  if (!GBZ.IsNull())
  {
    gp_Pnt P(Draw::Atof(a[3]), Draw::Atof(a[4]), Draw::Atof(a[5]));
    if (n == 7)
      GBZ->InsertPoleAfter(Index, P, Draw::Atof(a[6]));
    else
      GBZ->InsertPoleAfter(Index, P);
  }
  else if (!GBZ2d.IsNull())
  {
    gp_Pnt2d P(Draw::Atof(a[3]), Draw::Atof(a[4]));
    if (n == 6)
      GBZ2d->InsertPoleAfter(Index, P, Draw::Atof(a[5]));
    else
      GBZ2d->InsertPoleAfter(Index, P);
  }
  else
  {
    di << "insertpole needs a bezier curve";
    return 1;
  }

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int cfindp(Draw_Interpretor&, int n, const char** a)
{
  if (n < 6)
    return 1;

  int    Index = 0;
  int    view  = Draw::Atoi(a[2]);
  double x     = Draw::Atof(a[3]);
  double y     = Draw::Atof(a[4]);

  Draw_Display d = dout.MakeDisplay(view);

  occ::handle<Draw_Drawable3D> D = Draw::Get(a[1]);

  occ::handle<DrawTrSurf_BezierCurve> DBz = occ::down_cast<DrawTrSurf_BezierCurve>(D);
  if (!DBz.IsNull())
    DBz->FindPole(x, y, d, 5, Index);
  else
  {
    occ::handle<DrawTrSurf_BSplineCurve> DBs = occ::down_cast<DrawTrSurf_BSplineCurve>(D);
    if (!DBs.IsNull())
      DBs->FindPole(x, y, d, 5, Index);
    else
    {
      occ::handle<DrawTrSurf_BezierCurve2d> DBz2d = occ::down_cast<DrawTrSurf_BezierCurve2d>(D);
      if (!DBz2d.IsNull())
        DBz2d->FindPole(x, y, d, 5, Index);
      else
      {
        occ::handle<DrawTrSurf_BSplineCurve2d> DBs2d = occ::down_cast<DrawTrSurf_BSplineCurve2d>(D);
        if (!DBs2d.IsNull())
          DBs2d->FindPole(x, y, d, 5, Index);
        else
          return 1;
      }
    }
  }

  Draw::Set(a[5], Index);

  return 0;
}

//=================================================================================================

static int csetperiodic(Draw_Interpretor&, int n, const char** a)
{
  if (n < 2)
    return 1;

  occ::handle<Geom_BSplineCurve>   GBs   = DrawTrSurf::GetBSplineCurve(a[1]);
  occ::handle<Geom2d_BSplineCurve> GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (GBs.IsNull() && GBs2d.IsNull())
    return 1;

  if (!strcmp(a[0], "setperiodic"))
  {
    if (!GBs.IsNull())
      GBs->SetPeriodic();
    else
      GBs2d->SetPeriodic();
  }
  else if (!strcmp(a[0], "setnotperiodic"))
  {
    if (!GBs.IsNull())
      GBs->SetNotPeriodic();
    else
      GBs2d->SetNotPeriodic();
  }

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int value(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom_Curve> GC = DrawTrSurf::GetCurve(a[1]);
  if (GC.IsNull())
    return 1;

  double U = Draw::Atof(a[2]);

  bool DrawPoint = (n % 3 == 1);
  if (DrawPoint)
    n--;

  gp_Pnt P;
  if (n > 6)
  {
    if (n < 9)
      return 1;
    gp_Vec D1;
    if (n > 9)
    {
      if (n < 12)
        return 1;
      gp_Vec D2;
      GC->D2(U, P, D1, D2);
      Draw::Set(a[9], D2.X());
      Draw::Set(a[10], D2.Y());
      Draw::Set(a[11], D2.Z());
    }
    else
      GC->D1(U, P, D1);
    Draw::Set(a[6], D1.X());
    Draw::Set(a[7], D1.Y());
    Draw::Set(a[8], D1.Z());
  }
  else
    GC->D0(U, P);

  if (n > 3)
  {
    Draw::Set(a[3], P.X());
    Draw::Set(a[4], P.Y());
    Draw::Set(a[5], P.Z());
  }
  if (DrawPoint)
  {
    DrawTrSurf::Set(a[n], P);
  }

  return 0;
}

//=================================================================================================

static int value2d(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom2d_Curve> GC = DrawTrSurf::GetCurve2d(a[1]);
  if (GC.IsNull())
    return 1;

  double U = Draw::Atof(a[2]);

  bool DrawPoint = (n % 2 == 0);
  if (DrawPoint)
    n--;

  gp_Pnt2d P;
  if (n > 5)
  {
    if (n < 7)
      return 1;
    gp_Vec2d D1;
    if (n > 7)
    {
      if (n < 9)
        return 1;
      gp_Vec2d D2;
      GC->D2(U, P, D1, D2);
      Draw::Set(a[7], D2.X());
      Draw::Set(a[8], D2.Y());
    }
    else
      GC->D1(U, P, D1);
    Draw::Set(a[5], D1.X());
    Draw::Set(a[6], D1.Y());
  }
  else
    GC->D0(U, P);

  if (n > 3)
  {
    Draw::Set(a[3], P.X());
    Draw::Set(a[4], P.Y());
  }
  if (DrawPoint)
  {
    DrawTrSurf::Set(a[n], P);
  }

  return 0;
}

//=================================================================================================

static int segment(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4 || n > 5)
    return 1;

  occ::handle<Geom_BezierCurve>    GBz   = DrawTrSurf::GetBezierCurve(a[1]);
  occ::handle<Geom_BSplineCurve>   GBs   = DrawTrSurf::GetBSplineCurve(a[1]);
  occ::handle<Geom2d_BezierCurve>  GBz2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  occ::handle<Geom2d_BSplineCurve> GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  double f = Draw::Atof(a[2]), l = Draw::Atof(a[3]);

  double aTolerance = Precision::PConfusion();
  if (n == 5)
    aTolerance = Draw::Atof(a[4]);

  if (!GBz.IsNull())
    GBz->Segment(f, l);
  else if (!GBs.IsNull())
    GBs->Segment(f, l, aTolerance);
  else if (!GBz2d.IsNull())
    GBz2d->Segment(f, l);
  else if (!GBs2d.IsNull())
    GBs2d->Segment(f, l, aTolerance);
  else
    return 1;

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int setorigin(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;

  occ::handle<Geom_BSplineCurve>   GBs   = DrawTrSurf::GetBSplineCurve(a[1]);
  occ::handle<Geom2d_BSplineCurve> GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (!GBs.IsNull())
    GBs->SetOrigin(Draw::Atoi(a[2]));
  if (!GBs2d.IsNull())
    GBs2d->SetOrigin(Draw::Atoi(a[2]));
  else
    return 1;

  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int point(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;
  if (n >= 5)
  {
    gp_Pnt P(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));
    DrawTrSurf::Set(a[1], P);
  }
  else
  {
    gp_Pnt2d P(Draw::Atof(a[2]), Draw::Atof(a[3]));
    DrawTrSurf::Set(a[1], P);
  }

  return 0;
}

//=================================================================================================

static int coord(Draw_Interpretor&, int n, const char** a)
{
  if (n == 4)
  {
    gp_Pnt2d P;
    if (!DrawTrSurf::GetPoint2d(a[1], P))
      return 1;
    Draw::Set(a[2], P.X());
    Draw::Set(a[3], P.Y());
  }
  else if (n == 5)
  {
    gp_Pnt P;
    if (!DrawTrSurf::GetPoint(a[1], P))
      return 1;
    Draw::Set(a[2], P.X());
    Draw::Set(a[3], P.Y());
    Draw::Set(a[4], P.Z());
  }
  else
    return 1;

  return 0;
}

//=================================================================================================

static int minmaxcurandinf(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
    return 1;

  occ::handle<Geom2d_Curve> C1 = DrawTrSurf::GetCurve2d(argv[1]);
  if (C1.IsNull())
    return 1;

  Draw_Color              Couleur;
  Geom2dLProp_CurAndInf2d Sommets;

  Sommets.PerformCurExt(C1);
  if (Sommets.IsDone() && !Sommets.IsEmpty())
  {
    for (int i = 1; i <= Sommets.NbPoints(); i++)
    {
      Couleur = Draw_vert;
      if (Sommets.Type(i) == LProp_MinCur)
      {
        Couleur = Draw_orange;
        di << "  Maximum of curvature at U =" << Sommets.Parameter(i) << "\n";
      }
      else
      {
        di << "  Minimum of curvature at U =" << Sommets.Parameter(i) << "\n";
      }
      gp_Pnt2d                   P  = C1->Value(Sommets.Parameter(i));
      occ::handle<Draw_Marker2D> dr = new Draw_Marker2D(P, Draw_Plus, Couleur);
      dout << dr;
    }
    dout.Flush();
  }

  Geom2dLProp_CurAndInf2d Sommets2;
  Sommets2.PerformInf(C1);

  if (Sommets2.IsDone() && !Sommets2.IsEmpty())
  {
    for (int i = 1; i <= Sommets2.NbPoints(); i++)
    {
      gp_Pnt2d                   P  = C1->Value(Sommets2.Parameter(i));
      occ::handle<Draw_Marker2D> dr = new Draw_Marker2D(P, Draw_Plus, Draw_bleu);
      dout << dr;
      di << "  Inflexion at U =" << Sommets2.Parameter(i) << "\n";
    }
    dout.Flush();
  }
  return 0;
}

//=======================================================================
// function :  shcurvature
// purpose  :  affiche le peigne de courbure
//=======================================================================
static int shcurvature(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 2)
    return 1;

  occ::handle<DrawTrSurf_Curve2d> C2d = occ::down_cast<DrawTrSurf_Curve2d>(Draw::Get(argv[1]));
  occ::handle<DrawTrSurf_Curve>   C3d = occ::down_cast<DrawTrSurf_Curve>(Draw::Get(argv[1]));

  if (C2d.IsNull())
  {
    if (C3d.IsNull())
      return 1;
    C3d->ShowCurvature();
  }
  else
  {
    C2d->ShowCurvature();
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
// function :  clcurvature
// purpose  :  efface le peigne de courbure
//=======================================================================
static int clcurvature(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 2)
    return 1;
  occ::handle<DrawTrSurf_Curve2d> C2d = occ::down_cast<DrawTrSurf_Curve2d>(Draw::Get(argv[1]));
  occ::handle<DrawTrSurf_Curve>   C3d = occ::down_cast<DrawTrSurf_Curve>(Draw::Get(argv[1]));

  if (C2d.IsNull())
  {
    if (C3d.IsNull())
      return 1;
    C3d->ClearCurvature();
  }
  else
  {
    C2d->ClearCurvature();
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
// function :  radiusmax
// purpose  :  definit le rayon de courbure maximum a afficher
//=======================================================================
static int radiusmax(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 3)
    return 1;
  double                          Radius = Draw::Atof(argv[2]);
  occ::handle<DrawTrSurf_Curve2d> C2d    = occ::down_cast<DrawTrSurf_Curve2d>(Draw::Get(argv[1]));
  occ::handle<DrawTrSurf_Curve>   C3d    = occ::down_cast<DrawTrSurf_Curve>(Draw::Get(argv[1]));

  if (C2d.IsNull())
  {
    if (C3d.IsNull())
      return 1;
    C3d->SetRadiusMax(Radius);
  }
  else
  {
    C2d->SetRadiusMax(Radius);
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
// function :  radiusratio
// purpose  :  definit le ratio du rayon de courbure a afficher
//=======================================================================
static int radiusratio(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 3)
    return 1;
  double                          Ratio = Draw::Atof(argv[2]);
  occ::handle<DrawTrSurf_Curve2d> C2d   = occ::down_cast<DrawTrSurf_Curve2d>(Draw::Get(argv[1]));
  occ::handle<DrawTrSurf_Curve>   C3d   = occ::down_cast<DrawTrSurf_Curve>(Draw::Get(argv[1]));

  if (C2d.IsNull())
  {
    if (C3d.IsNull())
      return 1;
    C3d->SetRadiusRatio(Ratio);
  }
  else
  {
    C2d->SetRadiusRatio(Ratio);
  }
  Draw::Repaint();
  return 0;
}

//=================================================================================================

static int localprop(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
    return 1;

  double U = Draw::Atof(argv[2]);

  occ::handle<Geom2d_Curve> C2d = DrawTrSurf::GetCurve2d(argv[1]);
  occ::handle<Geom_Curve>   C3d;

  if (C2d.IsNull())
  {
    C3d = DrawTrSurf::GetCurve(argv[1]);
    if (C3d.IsNull())
      return 1;
    GeomLProp_CLProps Prop(C3d, 2, Precision::Confusion());
    Prop.SetParameter(U);
    occ::handle<Draw_Marker3D> drp = new Draw_Marker3D(Prop.Value(), Draw_Plus, Draw_vert);
    dout << drp;
    if (Prop.IsTangentDefined())
    {
      double K = Prop.Curvature();
      di << " Curvature : " << K << "\n";

      if (std::abs(K) > Precision::Confusion())
      {
        double R = 1 / std::abs(K);
        gp_Pnt Center;
        Prop.CentreOfCurvature(Center);
        gp_Dir Tang;
        gp_Dir Nor;
        Prop.Tangent(Tang);
        Prop.Normal(Nor);
        gp_Dir                        AxC = Nor ^ Tang;
        gp_Ax2                        Axe(Center, AxC, Nor);
        occ::handle<Geom_Circle>      Cir3d = new Geom_Circle(Axe, R);
        occ::handle<DrawTrSurf_Curve> dr;
        dr = new DrawTrSurf_Curve(Cir3d);
        dout << dr;
        dout.Flush();
      }
    }
    else
      di << "Tangent undefined.\n";
  }
  else
  {
    Geom2dLProp_CLProps2d Prop(C2d, 2, Precision::Confusion());
    Prop.SetParameter(U);
    occ::handle<Draw_Marker2D> drp = new Draw_Marker2D(Prop.Value(), Draw_Plus, Draw_vert);
    dout << drp;
    if (Prop.IsTangentDefined())
    {
      double   K = Prop.Curvature();
      gp_Pnt2d Center;

      di << " Curvature : " << K << "\n";

      if (std::abs(K) > Precision::Confusion())
      {
        double R = 1 / std::abs(K);
        Prop.CentreOfCurvature(Center);
        gp_Ax2d                         Axe(Center, gp::DX2d());
        occ::handle<Geom2d_Circle>      Cir2d = new Geom2d_Circle(Axe, R);
        occ::handle<DrawTrSurf_Curve2d> dr;
        dr = new DrawTrSurf_Curve2d(Cir2d, Draw_rouge, 30, false);
        dout << dr;
        dout.Flush();
      }
    }
    else
      di << "Tangent undefined.\n";
  }
  return 0;
}

//=================================================================================================

static int rawcont(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
    return 1;

  occ::handle<Geom_Curve> GC1;
  GC1 = DrawTrSurf::GetCurve(a[1]);
  occ::handle<Geom_Curve> GC2;
  GC2           = DrawTrSurf::GetCurve(a[2]);
  double param1 = Draw::Atof(a[3]);
  double param2 = Draw::Atof(a[4]);
  if (GC1.IsNull() || GC2.IsNull())
    return 1;
  gp_Pnt a_point1, a_point2;
  GC1->D0(param1, a_point1);
  GC2->D0(param2, a_point2);
  if (a_point2.SquareDistance(a_point1) < Precision::Confusion())
  {
    GeomAbs_Shape cont = GeomLProp::Continuity(GC1,
                                               GC2,
                                               param1,
                                               param2,
                                               true,
                                               true,
                                               Precision::Confusion(),
                                               Precision::Angular());
    switch (cont)
    {
      case GeomAbs_C0:
        di << " C0 Continuity \n";
        break;
      case GeomAbs_G1:
        di << " G1 Continuity \n";
        break;
      case GeomAbs_C1:
        di << " C1 Continuity \n";
        break;
      case GeomAbs_G2:
        di << " G2 Continuity \n";
        break;
      case GeomAbs_C2:
        di << " C2 Continuity \n";
        break;
      case GeomAbs_C3:
        di << " C3 Continuity \n";
        break;
      case GeomAbs_CN:
        di << " CN Continuity \n";
        break;
      default:
        break;
    }
  }
  else
  {
    di << " not C0 continuity \n";
  }
  return 0;
}

//=================================================================================================

static int approxcurveonsurf(Draw_Interpretor& di, int n, const char** a)
{
  double        Tol        = 1.e-7;      // Tolerance (default 0.1mm)
  GeomAbs_Shape Continuity = GeomAbs_C1; // Continuity order : 0, 1 or 2 (default 1)
  int           MaxDeg     = 14;         // Maximum degree
  int           MaxSeg     = 16; /*1*/   // Maximum number of segments

  if (n > 8 || n < 4)
    return 1;

  if (n > 4)
    Tol = std::max(Draw::Atof(a[4]), 1.e-10);

  if (n > 5)
  {
    if (Draw::Atoi(a[5]) == 0)
      Continuity = GeomAbs_C0;
    if (Draw::Atoi(a[5]) == 2)
      Continuity = GeomAbs_C2;
  }

  if (n > 6)
  {
    MaxDeg = Draw::Atoi(a[6]);
    if (MaxDeg < 1 || MaxDeg > 14)
      MaxDeg = 14;
  }

  if (n > 7)
    MaxSeg = Draw::Atoi(a[7]);
  occ::handle<Geom2d_Curve> curve2d = DrawTrSurf::GetCurve2d(a[2]);
  occ::handle<Geom_Surface> Surf    = DrawTrSurf::GetSurface(a[3]);

  occ::handle<Geom2dAdaptor_Curve> A2d = new (Geom2dAdaptor_Curve)(curve2d);
  occ::handle<GeomAdaptor_Surface> AS  = new (GeomAdaptor_Surface)(Surf);

  Approx_CurveOnSurface App(A2d, AS, A2d->FirstParameter(), A2d->LastParameter(), Tol);
  App.Perform(MaxSeg, MaxDeg, Continuity, true, false);

  if (App.HasResult())
  {
    occ::handle<Geom_BSplineCurve> BSCurve = App.Curve3d();
    DrawTrSurf::Set(a[1], BSCurve);
    return 0;
  }

  di << "Approximation failed !\n";
  return 1;
}

//=================================================================================================

static int approxcurve(Draw_Interpretor& di, int n, const char** a)
{
  double        Tol        = 1.e-7;      // Tolerance (default 0.1mm)
  GeomAbs_Shape Continuity = GeomAbs_C1; // Continuity order : 0, 1 or 2 (default 1)
  int           MaxDeg     = 14;         // Maximum degree
  int           MaxSeg     = 16;         // Maximum number of segments

  int Case, shift;
  // Case == 1 : 3d approximation without reparametrization
  // Case == 2 : 2d approximation without reparametrization
  // Case == 3 : 3d approximation with reparametrization
  // Case == 4 : curve_on_surface approximation with reparametrization
  // Case == 5 : 2 curves_on_surfaces approximation with reparametrization

  occ::handle<Geom_Curve>   curve;
  occ::handle<Geom2d_Curve> curve2d, curve2d2;
  occ::handle<Geom_Surface> surface, surface2;

  if (n < 2)
    return 1;

  if (!strcmp(a[1], "-L"))
  {
    // approximation with curvilinear abscissa reparametrization
    if (n > 11 || n < 4)
      return 1;
    Tol   = 1.e-4;
    curve = DrawTrSurf::GetCurve(a[3]);
    if (!curve.IsNull())
    {
      shift = 4;
      Case  = 3;
    }
    else
    {
      // approx curve_on_surface
      if (n < 5)
        return 1;
      curve2d = DrawTrSurf::GetCurve2d(a[3]);
      surface = DrawTrSurf::GetSurface(a[4]);
      if (curve2d.IsNull() || surface.IsNull())
      {
        return 1;
      }
      if (n >= 7)
      {
        curve2d2 = DrawTrSurf::GetCurve2d(a[5]);
        surface2 = DrawTrSurf::GetSurface(a[6]);
        if (curve2d2.IsNull() || surface2.IsNull())
        {
          shift = 5;
          Case  = 4;
        }
        else
        {
          // approx 2 curves_on_surfaces
          shift = 7;
          Case  = 5;
        }
      }
      else
      {
        shift = 5;
        Case  = 4;
      }
    }
  }
  else
  {
    // approximation without reparamitrization
    if (n > 7 || n < 3)
      return 1;
    shift = 3;
    curve = DrawTrSurf::GetCurve(a[2]);
    if (curve.IsNull())
    {
      curve2d = DrawTrSurf::GetCurve2d(a[2]);
      if (curve2d.IsNull())
      {
        return 1;
      }
      Case = 2;
    }
    else
      Case = 1;
  }

  if (n > shift)
    Tol = std::max(Draw::Atof(a[shift]), 1.e-10);

  if (n > shift + 1)
  {
    if (Draw::Atoi(a[shift + 1]) == 0)
      Continuity = GeomAbs_C0;
    if (Draw::Atoi(a[shift + 1]) == 2)
      Continuity = GeomAbs_C2;
  }

  if (n > shift + 2)
  {
    MaxDeg = Draw::Atoi(a[shift + 2]);
    if (MaxDeg < 1 || MaxDeg > 14)
      MaxDeg = 14;
  }

  if (n > shift + 3)
    MaxSeg = Draw::Atoi(a[shift + 3]);

  if (Case == 1)
  {
    GeomConvert_ApproxCurve appr(curve, Tol, Continuity, MaxSeg, MaxDeg);
    if (appr.HasResult())
    {
      // appr.Dump(std::cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      occ::handle<Geom_BSplineCurve> BSCurve = appr.Curve();
      DrawTrSurf::Set(a[1], BSCurve);
    }
  }

  else if (Case == 2)
  {
    Geom2dConvert_ApproxCurve appr(curve2d, Tol, Continuity, MaxSeg, MaxDeg);
    if (appr.HasResult())
    {
      // appr.Dump(std::cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      occ::handle<Geom2d_BSplineCurve> BSCurve = appr.Curve();
      DrawTrSurf::Set(a[1], BSCurve);
    }
  }

  else if (Case == 3)
  {
    occ::handle<Adaptor3d_Curve> HACur = new GeomAdaptor_Curve(curve);
    Approx_CurvilinearParameter  appr(HACur, Tol, Continuity, MaxDeg, MaxSeg);
    if (appr.HasResult())
    {
      // appr.Dump(std::cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      occ::handle<Geom_BSplineCurve> BSCurve = appr.Curve3d();
      DrawTrSurf::Set(a[2], BSCurve);
    }
  }
  else if (Case == 4)
  {
    occ::handle<Adaptor2d_Curve2d> HACur2d = new Geom2dAdaptor_Curve(curve2d);
    occ::handle<Adaptor3d_Surface> HASur   = new GeomAdaptor_Surface(surface);
    Approx_CurvilinearParameter    appr(HACur2d, HASur, Tol, Continuity, MaxDeg, MaxSeg);
    if (appr.HasResult())
    {
      // appr.Dump(std::cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      occ::handle<Geom_BSplineCurve> BSCurve = appr.Curve3d();
      DrawTrSurf::Set(a[2], BSCurve);
    }
  }

  else if (Case == 5)
  {
    occ::handle<Adaptor2d_Curve2d> HACur2d  = new Geom2dAdaptor_Curve(curve2d);
    occ::handle<Adaptor3d_Surface> HASur    = new GeomAdaptor_Surface(surface);
    occ::handle<Adaptor2d_Curve2d> HACur2d2 = new Geom2dAdaptor_Curve(curve2d2);
    occ::handle<Adaptor3d_Surface> HASur2   = new GeomAdaptor_Surface(surface2);
    Approx_CurvilinearParameter
      appr(HACur2d, HASur, HACur2d2, HASur2, Tol, Continuity, MaxDeg, MaxSeg);
    if (appr.HasResult())
    {
      // appr.Dump(std::cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      occ::handle<Geom_BSplineCurve> BSCurve = appr.Curve3d();
      DrawTrSurf::Set(a[2], BSCurve);
    }
  }

  return 0;
}

//=================================================================================================

static int fitcurve(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  occ::handle<Geom_Curve> GC;
  GC = DrawTrSurf::GetCurve(a[2]);
  if (GC.IsNull())
    return 1;

  int    Dmin    = 3;
  int    Dmax    = 14;
  double Tol3d   = 1.e-5;
  bool   inverse = true;

  if (n > 3)
  {
    Tol3d = Atof(a[3]);
  }

  if (n > 4)
  {
    Dmax = atoi(a[4]);
  }

  if (n > 5)
  {
    int inv = atoi(a[5]);
    if (inv > 0)
    {
      inverse = true;
    }
    else
    {
      inverse = false;
    }
  }

  occ::handle<GeomAdaptor_Curve> aGAC = new GeomAdaptor_Curve(GC);

  CurveEvaluator aCE(aGAC);

  Approx_FitAndDivide anAppro(Dmin, Dmax, Tol3d, 0., true);
  anAppro.SetInvOrder(inverse);
  anAppro.Perform(aCE);

  if (!anAppro.IsAllApproximated())
  {
    di << "Approximation failed \n";
    return 1;
  }
  int i;
  int NbCurves = anAppro.NbMultiCurves();

  Convert_CompBezierCurvesToBSplineCurve Conv;

  double tol3d, tol2d, tolreached = 0.;
  for (i = 1; i <= NbCurves; i++)
  {
    anAppro.Error(i, tol3d, tol2d);
    tolreached                    = std::max(tolreached, tol3d);
    AppParCurves_MultiCurve    MC = anAppro.Value(i);
    NCollection_Array1<gp_Pnt> Poles(1, MC.Degree() + 1);
    MC.Curve(1, Poles);
    Conv.AddCurve(Poles);
  }
  Conv.Perform();
  int NbPoles = Conv.NbPoles();
  int NbKnots = Conv.NbKnots();

  NCollection_Array1<gp_Pnt> NewPoles(1, NbPoles);
  NCollection_Array1<double> NewKnots(1, NbKnots);
  NCollection_Array1<int>    NewMults(1, NbKnots);

  Conv.KnotsAndMults(NewKnots, NewMults);
  Conv.Poles(NewPoles);

  BSplCLib::Reparametrize(GC->FirstParameter(), GC->LastParameter(), NewKnots);
  occ::handle<Geom_BSplineCurve> TheCurve =
    new Geom_BSplineCurve(NewPoles, NewKnots, NewMults, Conv.Degree());

  DrawTrSurf::Set(a[1], TheCurve);
  di << a[1] << ": tolreached = " << tolreached << "\n";

  return 0;
}

//=======================================================================
// function : newbspline
// purpose  : reduce the multiplicity of the knots to their minimum
//           compared to the degree of the curve
//=======================================================================

static int splitc1(Draw_Interpretor& di, int n, const char** c)

{
  double tolerance = 1.0e-5, angular_tolerance = 1.0e-4;
  int    optiontab, i;
  char   name[100];

  if (n < 3)
    return 1;
  optiontab = Draw::Atoi(c[2]);
  if (n >= 4)
    tolerance = Draw::Atof(c[3]);
  if (n >= 5)
  {
    angular_tolerance = Draw::Atof(c[4]);
  }
  occ::handle<Geom_Curve> ACurve = occ::down_cast<Geom_Curve>(DrawTrSurf::Get(c[1]));

  double f = ACurve->FirstParameter();
  double l = ACurve->LastParameter();

  if (Precision::IsInfinite(f) || Precision::IsInfinite(l))
  {
    di << " Error: Infinite curves\n";
    return 1;
  }

  occ::handle<Geom_BSplineCurve> BS = GeomConvert::CurveToBSplineCurve(ACurve);

  if (BS.IsNull())
    return 1;

  if (optiontab)
  {
    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>> tabBS;
    GeomConvert::C0BSplineToArrayOfC1BSplineCurve(BS, tabBS, angular_tolerance, tolerance);
    for (i = 0; i <= (tabBS->Length() - 1); i++)
    {
      Sprintf(name, "%s_%d", c[1], i + 1);
      const char* new_name = name;
      DrawTrSurf::Set(new_name, tabBS->Value(i));
      di.AppendElement(name);
    }
  }
  else
  {
    GeomConvert::C0BSplineToC1BSplineCurve(BS, tolerance, angular_tolerance);

    DrawTrSurf::Set(c[1], BS);
  }
  return 0;
}

//=======================================================================
// function : splitc12d
// purpose  : reduce the multiplicity of the knots to their minimum
//           compared to the degree of the curve
//=======================================================================

static int splitc12d(Draw_Interpretor& di, int n, const char** c)

{
  double tolerance = 1.0e-5, angular_tolerance = 1.0e-4;
  int    optiontab, i;
  char   name[100];

  if (n < 3)
    return 1;
  optiontab = Draw::Atoi(c[2]);
  if (n == 4)
    tolerance = Draw::Atof(c[3]);
  if (n == 5)
    angular_tolerance = Draw::Atof(c[4]);
  occ::handle<Geom2d_Curve> ACurve = DrawTrSurf::GetCurve2d(c[1]);

  double f = ACurve->FirstParameter();
  double l = ACurve->LastParameter();

  if (Precision::IsInfinite(f) || Precision::IsInfinite(l))
  {
    di << " Error: Infinite curves\n";
    return 1;
  }

  occ::handle<Geom2d_BSplineCurve> BS = Geom2dConvert::CurveToBSplineCurve(ACurve);

  if (BS.IsNull())
    return 1;

  if (optiontab)
  {
    occ::handle<NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>> tabBS;
    Geom2dConvert::C0BSplineToArrayOfC1BSplineCurve(BS, tabBS, angular_tolerance, tolerance);
    for (i = 0; i <= (tabBS->Length() - 1); i++)
    {
      Sprintf(name, "%s_%d", c[1], i + 1);
      const char* new_name = name;
      DrawTrSurf::Set(new_name, tabBS->Value(i));
      di.AppendElement(name);
    }
  }
  else
  {
    Geom2dConvert::C0BSplineToC1BSplineCurve(BS, tolerance);
    DrawTrSurf::Set(c[1], BS);
  }
  return 0;
}

//=======================================================================
// function : canceldenom
// purpose  : set the value of the denominator cancel its first
//           derivative on the boundaries of the surface if possible
//=======================================================================

static int canceldenom(Draw_Interpretor&, int n, const char** c)

{
  int  uoption, voption;
  bool udirection = false;
  bool vdirection = false;
  if (n < 4)
    return 1;
  uoption = Draw::Atoi(c[2]);
  voption = Draw::Atoi(c[3]);
  if (uoption)
    udirection = true;
  if (voption)
    vdirection = true;
  occ::handle<Geom_BSplineSurface> BSurf = DrawTrSurf::GetBSplineSurface(c[1]);
  GeomLib::CancelDenominatorDerivative(BSurf, udirection, vdirection);
  DrawTrSurf::Set(c[1], BSurf);
  return 0;
}

//=======================================================================
// function : length
// purpose  : eval curve's length
//=======================================================================

static int length(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;
  occ::handle<Geom_Curve>   GC   = DrawTrSurf::GetCurve(a[1]);
  occ::handle<Geom2d_Curve> GC2d = DrawTrSurf::GetCurve2d(a[1]);
  double                    Tol  = Precision::Confusion(), L;
  if (n == 3)
    Tol = Draw::Atof(a[2]);

  if (!GC.IsNull())
  {
    GeomAdaptor_Curve AC(GC);
    L = GCPnts_AbscissaPoint::Length(AC, Tol);
  }
  else if (!GC2d.IsNull())
  {
    Geom2dAdaptor_Curve AC(GC2d);
    L = GCPnts_AbscissaPoint::Length(AC, Tol);
  }
  else
  {
    di << a[1] << "is not a curve\n";
    return 1;
  }

  di << "The length " << a[1] << " is " << L << "\n";
  return 0;
}

//=================================================================================================

void GeomliteTest::CurveCommands(Draw_Interpretor& theCommands)
{

  static bool loaded = false;
  if (loaded)
    return;
  loaded = true;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;

  // analytic curves
  g = "GEOMETRY curves creation";

  theCommands.Add("point", "point name x y [z]", __FILE__, point, g);

  theCommands.Add("line", "line name pos dir", __FILE__, anacurve, g);

  theCommands.Add("circle",
                  "circle name x y [z [dx dy dz]] [ux uy [uz]] radius",
                  __FILE__,
                  anacurve,
                  g);

  theCommands.Add("ellipse",
                  "ellipse name x y [z [dx dy dz]] [ux uy [uz]] major minor",
                  __FILE__,
                  anacurve,
                  g);
  theCommands.Add("parabola",
                  "parabola name x y [z [dx dy dz]] [ux uy [uz]] focal",
                  __FILE__,
                  anacurve,
                  g);
  theCommands.Add("hyperbola",
                  "hyperbola name x y [z [dx dy dz]] [ux uy [uz]] major minor",
                  __FILE__,
                  anacurve,
                  g);

  theCommands.Add("beziercurve", "beziercurve name nbpole pole, [weight]", __FILE__, polecurve, g);

  theCommands.Add("bsplinecurve",
                  "bsplinecurve name degree nbknots  knot, umult  pole, weight",
                  __FILE__,
                  polecurve,
                  g);

  theCommands.Add("pbsplinecurve",
                  "pbsplinecurve name degree nbknots  knot, umult  pole, weight (periodic)",
                  __FILE__,
                  polecurve,
                  g);

  theCommands.Add("2dbeziercurve",
                  "2dbeziercurve name nbpole pole, [weight]",
                  __FILE__,
                  polecurve2d,
                  g);

  theCommands.Add("2dbsplinecurve",
                  "2dbsplinecurve name degree nbknots  knot, umult  pole, weight",
                  __FILE__,
                  polecurve2d,
                  g);

  theCommands.Add("2dpbsplinecurve",
                  "2dpbsplinecurve name degree nbknots  knot, umult  pole, weight (periodic)",
                  __FILE__,
                  polecurve2d,
                  g);

  g = "GEOMETRY Curves and Surfaces modification";

  theCommands.Add("reverse", "reverse name ... ", __FILE__, reverse, g);

  theCommands.Add("cmovep", "cmovep name index dx dy dz", __FILE__, cmovepole, g);

  theCommands.Add("cmovepoint",
                  "cmovepoint name u dx dy [dz index1 index2]",
                  __FILE__,
                  cmovepoint,
                  g);

  theCommands.Add("cmovetangent",
                  "cmovetangent name u  x y [z] tx ty [tz constraint = 0]",
                  __FILE__,
                  cmovetangent,
                  g);

  theCommands.Add("insertknot",
                  "insertknot name knot [mult = 1] [knot mult ...]",
                  __FILE__,
                  cinsertknot,
                  g);

  theCommands.Add("setknot", "setknot name index knot [mult]", __FILE__, csetknot, g);

  theCommands.Add("remknot", "remknot name index [mult] [tol]", __FILE__, cremknot, g);

  theCommands.Add("incdeg", "incdeg name degree", __FILE__, increasedegree, g);

  theCommands.Add("rempole", "rempole name index", __FILE__, removepole, g);

  theCommands.Add("insertpole", "insertpole name index x y [z] [weight]", __FILE__, insertpole, g);

  theCommands.Add("cfindp", "cfindp name view x y index", __FILE__, cfindp, g);

  theCommands.Add("setperiodic", "setperiodic name ...", __FILE__, csetperiodic, g);

  theCommands.Add("setnotperiodic", "setnotperiodic name", __FILE__, csetperiodic, g);

  theCommands.Add("segment", "segment name Ufirst Ulast [tol]", __FILE__, segment, g);

  theCommands.Add("setorigin", "setorigin name knotindex", __FILE__, setorigin, g);

  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("cvalue",
                  "cvalue curvename U  X Y Z [D1X D1Y D1Z D2X D2Y D2Z]",
                  __FILE__,
                  value,
                  g);

  theCommands.Add("2dcvalue", "2dcvalue curvename U  X Y [D1X D1Y D2X D2Y]", __FILE__, value2d, g);

  theCommands.Add("coord",
                  "coord P x y [z]: set in x y [z] the coordinates of P",
                  __FILE__,
                  coord,
                  g);

  theCommands.Add("minmaxcurandinf", "minmaxcurandinf curve", __FILE__, minmaxcurandinf, g);

  theCommands.Add("shcurvature", "shcurvature curvename", __FILE__, shcurvature, g);

  theCommands.Add("clcurvature", "clcurvature curvename", __FILE__, clcurvature, g);

  theCommands.Add("radiusmax", "radiusmax curvename  radius", __FILE__, radiusmax, g);

  theCommands.Add("radiusratio", "radiusratio curvename ratio", __FILE__, radiusratio, g);
  theCommands.Add("localprop", "localprop curvename U", __FILE__, localprop, g);
  theCommands.Add("rawcont", "rawcont curve1 curve2 u1 u2", __FILE__, rawcont, g);
  theCommands.Add(
    "approxcurve",
    "approxcurve [-L] name curve1 [Surf1] [curve2d2 Surf2] [Tol [cont [maxdeg [maxseg]]]] ",
    __FILE__,
    approxcurve,
    g);

  theCommands.Add("approxcurveonsurf",
                  "approxcurveonsurf name curve2d surface [Tol [cont [maxdeg [maxseg]]]] ",
                  __FILE__,
                  approxcurveonsurf,
                  g);

  theCommands.Add("fitcurve",
                  "fitcurve result  curve [tol [maxdeg [inverse]]]",
                  __FILE__,
                  fitcurve,
                  g);

  theCommands.Add("length", "length curve [Tol]", __FILE__, length, g);

  theCommands.Add("splitc1",
                  "splitc1 bspline resultinarray(0/1) [tol] [angtol] ",
                  __FILE__,
                  splitc1,
                  g);

  theCommands.Add("splitc12d",
                  "splitc12d bspline2d resultinarray(0/1) [tol] [angtol] ",
                  __FILE__,
                  splitc12d,
                  g);
  theCommands.Add("canceldenom",
                  "canceldenom BSpline-Surface UDirection(0/1) VDirection(0/1)",
                  __FILE__,
                  canceldenom,
                  g);
}
