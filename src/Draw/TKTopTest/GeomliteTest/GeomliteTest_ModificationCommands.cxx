// Created on: 1997-04-15
// Created by: Joelle CHAUVET
// Copyright (c) 1997-1999 Matra Datavision
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

#include <GeomliteTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Display.hxx>

#include <GeomLib.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Message.hxx>

#ifdef _MSC_VER
  #include <stdio.h>
#endif

//=================================================================================================

static int extendcurve(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom_BoundedCurve> GB = occ::down_cast<Geom_BoundedCurve>(DrawTrSurf::GetCurve(a[1]));
  if (GB.IsNull())
  {
    di << "extendcurve needs a Bounded curve";
    return 1;
  }

  gp_Pnt P;
  if (!DrawTrSurf::GetPoint(a[2], P))
    return 1;
  bool apres = true;
  if (n == 5)
  {
    if (strcmp(a[4], "B") == 0)
    {
      apres = false;
    }
  }
  int cont = Draw::Atoi(a[3]);
  GeomLib::ExtendCurveToPoint(GB, P, cont, apres);
  DrawTrSurf::Set(a[1], GB);
  return 0;
}

//=================================================================================================

static int extendsurf(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom_BoundedSurface> GB =
    occ::down_cast<Geom_BoundedSurface>(DrawTrSurf::GetSurface(a[1]));
  if (GB.IsNull())
  {
    di << "extendsurf needs a Bounded surface";
    return 1;
  }
  double    chord = Draw::Atof(a[2]);
  int cont  = Draw::Atoi(a[3]);
  bool enU = true, apres = true;
  if (n >= 5)
  {
    if (strcmp(a[4], "V") == 0)
    {
      enU = false;
    }
    if (strcmp(a[4], "B") == 0)
    {
      apres = false;
    }
  }
  if (n == 6)
  {
    if (strcmp(a[5], "B") == 0)
    {
      apres = false;
    }
  }

  GeomLib::ExtendSurfByLength(GB, chord, cont, enU, apres);
  DrawTrSurf::Set(a[1], GB);

  return 0;
}

//=================================================================================================

static int samerange(Draw_Interpretor& /*di*/, int n, const char** a)
{
  if (n < 6)
    return 1;
  occ::handle<Geom2d_Curve> C = DrawTrSurf::GetCurve2d(a[2]);
  occ::handle<Geom2d_Curve> Res;
  double        f, l, rf, rl;
  f  = Draw::Atof(a[3]);
  l  = Draw::Atof(a[4]);
  rf = Draw::Atof(a[5]);
  rl = Draw::Atof(a[6]);

  GeomLib::SameRange(Precision::PConfusion(), C, f, l, rf, rl, Res);

  DrawTrSurf::Set(a[1], Res);

  return 0;
}

//=======================================================================
// function : setweight
// purpose  : Changes a weight of a pole on B-spline curve/surface
//=======================================================================

static int setweight(Draw_Interpretor& /*di*/, int n, const char** a)
{
  if (n < 4 || n > 5)
  {
    Message::SendFail() << "Syntax error: Wrong parameters";
    return 1;
  }

  int anIndex1 = Draw::Atoi(a[2]);
  int anIndex2 = n == 5 ? Draw::Atoi(a[3]) : 0;
  double    aWeight  = Draw::Atof(a[n - 1]);

  occ::handle<Geom_BSplineCurve> aBSplCurve = DrawTrSurf::GetBSplineCurve(a[1]);
  if (!aBSplCurve.IsNull())
  {
    aBSplCurve->SetWeight(anIndex1, aWeight);
    return 0;
  }

  occ::handle<Geom_BezierCurve> aBezCurve = DrawTrSurf::GetBezierCurve(a[1]);
  if (!aBezCurve.IsNull())
  {
    aBezCurve->SetWeight(anIndex1, aWeight);
    return 0;
  }

  occ::handle<Geom2d_BSplineCurve> aBSplCurve2d = DrawTrSurf::GetBSplineCurve2d(a[1]);
  if (!aBSplCurve2d.IsNull())
  {
    aBSplCurve2d->SetWeight(anIndex1, aWeight);
    return 0;
  }

  occ::handle<Geom2d_BezierCurve> aBezCurve2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  if (!aBezCurve2d.IsNull())
  {
    aBezCurve2d->SetWeight(anIndex1, aWeight);
    return 0;
  }

  occ::handle<Geom_BSplineSurface> aBSplSurf = DrawTrSurf::GetBSplineSurface(a[1]);
  occ::handle<Geom_BezierSurface>  aBezSurf  = DrawTrSurf::GetBezierSurface(a[1]);
  if (n != 5 && (!aBSplSurf.IsNull() || !aBezSurf.IsNull()))
  {
    Message::SendFail() << "Syntax error: Incorrect parameters";
    return 1;
  }

  if (!aBSplSurf.IsNull())
  {
    aBSplSurf->SetWeight(anIndex1, anIndex2, aWeight);
    return 0;
  }

  if (!aBezSurf.IsNull())
  {
    aBezSurf->SetWeight(anIndex1, anIndex2, aWeight);
    return 0;
  }

  Message::SendFail() << a[1] << " is not a B-spline nor a Bezier curve/surface";
  return 1;
}

//=================================================================================================

void GeomliteTest::ModificationCommands(Draw_Interpretor& theCommands)
{
  static bool loaded = false;
  if (loaded)
    return;
  loaded = true;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;

  g = "GEOMETRY Curves and Surfaces modification";

  theCommands.Add("extendcurve",
                  "extendcurve name point cont [A(fter)/B(efore)]",
                  __FILE__,
                  extendcurve,
                  g);

  theCommands.Add("extendsurf",
                  "extendsurf name length cont [U/V] [A(fter)/B(efore)]",
                  __FILE__,
                  extendsurf,
                  g);

  theCommands.Add("chgrange",
                  "chgrange newname curve2d first last  RequestedFirst RequestedLast ]",
                  __FILE__,
                  samerange,
                  g);

  theCommands.Add("setweight",
                  "setweight curve/surf index1 [index2] weight"
                  "\n\t\tchanges a weight of a pole of B-spline curve/surface (index2 is useful "
                  "for surfaces only)",
                  __FILE__,
                  setweight,
                  g);
}
