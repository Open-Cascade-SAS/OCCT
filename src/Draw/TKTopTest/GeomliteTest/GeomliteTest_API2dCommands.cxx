// Created on: 1995-01-11
// Created by: Remi LEQUETTE
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

// modified : pmn 11/04/97 : mis dans GeomliteTest

#include <GeomliteTest.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomLib_Tool.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>
#include <Geom2d_Circle.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Message.hxx>
#include <NCollection_Shared.hxx>

#include <memory>
#include <cstdio>
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=================================================================================================

static int proj(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 4)
  {
    di << "Syntax error: wrong number of arguments";
    return 1;
  }

  occ::handle<Geom2d_Curve> GC = DrawTrSurf::GetCurve2d(a[1]);
  if (GC.IsNull())
  {
    di << "Syntax error: '" << a[1] << "' is NULL";
    return 1;
  }

  const gp_Pnt2d                P(Draw::Atof(a[2]), Draw::Atof(a[3]));
  Geom2dAPI_ProjectPointOnCurve proj(P, GC, GC->FirstParameter(), GC->LastParameter());
  for (int i = 1; i <= proj.NbPoints(); i++)
  {
    gp_Pnt2d                      aP1   = proj.Point(i);
    const double                  aDist = P.Distance(aP1);
    const TCollection_AsciiString aName = TCollection_AsciiString("ext_") + i;
    if (aDist > Precision::PConfusion())
    {
      occ::handle<Geom2d_Line>         L  = new Geom2d_Line(P, gp_Dir2d(aP1.XY() - P.XY()));
      occ::handle<Geom2d_TrimmedCurve> CT = new Geom2d_TrimmedCurve(L, 0., aDist);
      DrawTrSurf::Set(aName.ToCString(), CT);
    }
    else
    {
      DrawTrSurf::Set(aName.ToCString(), aP1);
    }

    di << aName << " ";
  }

  return 0;
}

//=================================================================================================

static int appro(Draw_Interpretor& di, int n, const char** a)
{
  // Approximation et interpolation 2d

  // 2dappro
  //     - affiche la tolerance
  // 2dappro tol
  //     - change la tolerance
  // 2dappro result nbpoint
  //     - saisie interactive
  // 2dappro result nbpoint curve
  //     - calcule des points sur la courbe
  // 2dappro result nbpoint x1 y1 x2 y2 ..
  //     - tableau de points
  // 2dappro result nbpoint x1 dx y1 y2 ..
  //     - tableau de points (x1,y1) (x1+dx,y2) ... avec x = t

  static double Tol2d = 1.e-6;
  if (n < 3)
  {
    if (n == 2)
    {
      Tol2d = Draw::Atof(a[1]);
    }

    di << "Tolerance for 2d approx : " << Tol2d << "\n";
    return 0;
  }

  int i, Nb = Draw::Atoi(a[2]);

  bool                         hasPoints = true;
  NCollection_Array1<gp_Pnt2d> Points(1, Nb);
  NCollection_Array1<double>   YValues(1, Nb);
  double                       X0 = 0, DX = 0;

  occ::handle<Draw_Marker2D> mark;

  if (n == 3)
  {
    // saisie interactive
    int id, XX, YY, b;
    dout.Select(id, XX, YY, b);
    double zoom = dout.Zoom(id);

    Points(1) = gp_Pnt2d(((double)XX) / zoom, ((double)YY) / zoom);

    mark = new Draw_Marker2D(Points(1), Draw_X, Draw_vert);

    dout << mark;

    for (i = 2; i <= Nb; i++)
    {
      dout.Select(id, XX, YY, b);
      Points(i) = gp_Pnt2d(((double)XX) / zoom, ((double)YY) / zoom);
      mark      = new Draw_Marker2D(Points(i), Draw_X, Draw_vert);
      dout << mark;
    }
  }
  else
  {
    if (n == 4)
    {
      // points sur courbe
      occ::handle<Geom2d_Curve> GC = DrawTrSurf::GetCurve2d(a[3]);
      if (GC.IsNull())
        return 1;

      double U, U1, U2;
      U1           = GC->FirstParameter();
      U2           = GC->LastParameter();
      double Delta = (U2 - U1) / (Nb - 1);
      for (i = 1; i <= Nb; i++)
      {
        U         = U1 + (i - 1) * Delta;
        Points(i) = GC->Value(U);
      }
    }

    else
    {
      // test points ou ordonnees
      int nc = n - 3;
      if (nc == 2 * Nb)
      {
        // points
        nc = 3;
        for (i = 1; i <= Nb; i++)
        {
          Points(i).SetCoord(Draw::Atof(a[nc]), Draw::Atof(a[nc + 1]));
          nc += 2;
        }
      }
      else if (nc - 2 == Nb)
      {
        // YValues
        hasPoints = false;
        nc        = 5;
        X0        = Draw::Atof(a[3]);
        DX        = Draw::Atof(a[4]);
        for (i = 1; i <= Nb; i++)
        {
          YValues(i) = Draw::Atof(a[nc]);
          Points(i).SetCoord(X0 + (i - 1) * DX, YValues(i));
          nc++;
        }
      }
      else
        return 1;
    }
    // display the points
    for (i = 1; i <= Nb; i++)
    {
      mark = new Draw_Marker2D(Points(i), Draw_X, Draw_vert);
      dout << mark;
    }
  }
  dout.Flush();
  int Dmin = 3;
  int Dmax = 8;

  occ::handle<Geom2d_BSplineCurve> TheCurve;
  if (hasPoints)
  {
    if (!strcmp(a[0], "2dinterpole"))
    {
      Geom2dAPI_Interpolate anInterpol(new NCollection_HArray1<gp_Pnt2d>(Points), false, Tol2d);
      anInterpol.Perform();
      if (!anInterpol.IsDone())
      {
        di << "not done";
        return 1;
      }
      TheCurve = anInterpol.Curve();
    }
    else
    {
      Geom2dAPI_PointsToBSpline anApprox(Points, Dmin, Dmax, GeomAbs_C2, Tol2d);
      if (!anApprox.IsDone())
      {
        di << "not done";
        return 1;
      }
      TheCurve = anApprox.Curve();
    }
  }
  else
  {
    if (!strcmp(a[0], "2dinterpole"))
    {
      di << "incorrect usage";
      return 1;
    }
    Geom2dAPI_PointsToBSpline anApprox(YValues, X0, DX, Dmin, Dmax, GeomAbs_C2, Tol2d);
    if (!anApprox.IsDone())
    {
      di << "not done";
      return 1;
    }
    TheCurve = anApprox.Curve();
  }

  DrawTrSurf::Set(a[1], TheCurve);
  di << a[1];
  return 0;
}

//=================================================================================================

static int extrema(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
  {
    di << "Syntax error: wrong number of arguments";
    return 1;
  }

  occ::handle<Geom2d_Curve> GC1 = DrawTrSurf::GetCurve2d(a[1]);
  occ::handle<Geom2d_Curve> GC2 = DrawTrSurf::GetCurve2d(a[2]);
  if (GC1.IsNull())
  {
    di << "Syntax error: '" << a[1] << "' is NULL";
    return 1;
  }
  if (GC2.IsNull())
  {
    di << "Syntax error: '" << a[2] << "' is NULL";
    return 1;
  }

  const double U1f = GC1->FirstParameter();
  const double U1l = GC1->LastParameter();
  const double U2f = GC2->FirstParameter();
  const double U2l = GC2->LastParameter();

  Geom2dAPI_ExtremaCurveCurve Ex(GC1, GC2, U1f, U1l, U2f, U2l);
  bool                        isInfinitySolutions = Ex.Extrema().IsParallel();
  const int                   aNExtr              = Ex.NbExtrema();

  if (aNExtr == 0 || isInfinitySolutions)
  {
    // Infinity solutions flag may be set with 0 number of
    // solutions in analytic extrema Curve/Curve.
    if (isInfinitySolutions)
      di << "Infinite number of extremas, distance = " << Ex.LowerDistance() << "\n";
    else
      di << "No solutions!\n";
  }

  for (int i = 1; i <= aNExtr; i++)
  {
    gp_Pnt2d P1, P2;
    Ex.Points(i, P1, P2);
    di << "dist " << i << ": " << Ex.Distance(i) << "  ";
    const TCollection_AsciiString aName = TCollection_AsciiString("ext_") + i;
    if (Ex.Distance(i) <= Precision::PConfusion())
    {
      occ::handle<Draw_Marker2D> mark = new Draw_Marker2D(P1, Draw_X, Draw_vert);
      dout << mark;
      dout.Flush();
      const char* temp = aName.ToCString();
      DrawTrSurf::Set(temp, P1);
    }
    else
    {
      occ::handle<Geom2d_Line>         L    = new Geom2d_Line(P1, gp_Vec2d(P1, P2));
      occ::handle<Geom2d_TrimmedCurve> CT   = new Geom2d_TrimmedCurve(L, 0., P1.Distance(P2));
      const char*                      temp = aName.ToCString();
      DrawTrSurf::Set(temp, CT);
    }
    di << aName << "\n";
  }

  return 0;
}

//=================================================================================================

static int intersect(Draw_Interpretor& di, int n, const char** a)
{
  occ::handle<Geom2d_Curve> C1, C2;
  double                    Tol         = 0.001;
  bool                      bPrintState = false;

  // Retrieve other parameters if any
  for (int i = 1; i < n; ++i)
  {
    if (!strcmp(a[i], "-tol"))
    {
      Tol = Draw::Atof(a[++i]);
    }
    else if (!strcmp(a[i], "-state"))
    {
      bPrintState = true;
    }
    else if (C1.IsNull())
    {
      C1 = DrawTrSurf::GetCurve2d(a[i]);
      if (C1.IsNull())
      {
        di << "Syntax error: curve '" << a[i] << "' is null";
        return 1;
      }
    }
    else if (C2.IsNull())
    {
      C2 = DrawTrSurf::GetCurve2d(a[i]);
      if (C2.IsNull())
      {
        di << "Syntax error: curve '" << a[i] << "' is null";
        return 1;
      }
    }
    else
    {
      di << "Syntax error at '" << a[i] << "'";
      return 1;
    }
  }
  if (C1.IsNull())
  {
    di << "Syntax error: wrong number of arguments";
    return 1;
  }

  Geom2dAPI_InterCurveCurve Intersector;
  if (!C2.IsNull())
  {
    // Curves intersection
    Intersector.Init(C1, C2, Tol);
  }
  else
  {
    // Self-intersection of the curve
    Intersector.Init(C1, Tol);
  }

  const Geom2dInt_GInter& anIntTool = Intersector.Intersector();
  if (!anIntTool.IsDone())
  {
    di << "Intersection failed\n";
    return 0;
  }

  if (anIntTool.IsEmpty())
  {
    return 0;
  }

  int aNbPoints = Intersector.NbPoints();
  for (int i = 1; i <= aNbPoints; i++)
  {
    // API simplified result
    gp_Pnt2d P = Intersector.Point(i);
    di << "Intersection point " << i << " : " << P.X() << " " << P.Y() << "\n";
    // Intersection extended results from intersection tool
    const IntRes2d_IntersectionPoint& aPInt = anIntTool.Point(i);
    di << "parameter on the fist: " << aPInt.ParamOnFirst();
    di << " parameter on the second: " << aPInt.ParamOnSecond() << "\n";
    if (bPrintState)
    {
      di << "Intersection type: "
         << (aPInt.TransitionOfFirst().IsTangent() ? "TOUCH" : "INTERSECTION") << "\n";
    }
    occ::handle<Draw_Marker2D> mark = new Draw_Marker2D(P, Draw_X, Draw_vert);
    dout << mark;
  }
  dout.Flush();

  occ::handle<Geom2d_Curve>       S1, S2;
  occ::handle<DrawTrSurf_Curve2d> CD;
  int                             aNbSegments = Intersector.NbSegments();
  for (int i = 1; i <= aNbSegments; i++)
  {
    di << "Segment #" << i << " found.\n";
    Intersector.Segment(i, S1, S2);
    CD = new DrawTrSurf_Curve2d(S1, Draw_bleu, 30);
    dout << CD;
    CD = new DrawTrSurf_Curve2d(S2, Draw_violet, 30);
    dout << CD;
  }

  dout.Flush();
  return 0;
}

//=================================================================================================

static int intersect_ana(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
  {
    di << "Syntax error: wrong number of arguments";
    return 1;
  }

  occ::handle<Geom2d_Curve>  C1    = DrawTrSurf::GetCurve2d(a[1]);
  occ::handle<Geom2d_Curve>  C2    = DrawTrSurf::GetCurve2d(a[2]);
  occ::handle<Geom2d_Circle> aCir1 = occ::down_cast<Geom2d_Circle>(C1);
  occ::handle<Geom2d_Circle> aCir2 = occ::down_cast<Geom2d_Circle>(C2);
  if (aCir1.IsNull() || aCir2.IsNull())
  {
    di << "Syntax error: '" << a[aCir1.IsNull() ? 1 : 2] << "' is not a circle";
    return 1;
  }

  IntAna2d_AnaIntersection Intersector(aCir1->Circ2d(), aCir2->Circ2d());
  for (int i = 1; i <= Intersector.NbPoints(); i++)
  {
    gp_Pnt2d P = Intersector.Point(i).Value();
    di << "Intersection point " << i << " : " << P.X() << " " << P.Y() << "\n";
    di << "parameter on the fist: " << Intersector.Point(i).ParamOnFirst();
    di << " parameter on the second: " << Intersector.Point(i).ParamOnSecond() << "\n";
    occ::handle<Draw_Marker2D> mark = new Draw_Marker2D(P, Draw_X, Draw_vert);
    dout << mark;
  }
  dout.Flush();
  return 0;
}

//=================================================================================================

static int intconcon(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
  {
    di << "Syntax error: wrong number of arguments";
    return 1;
  }

  occ::handle<Geom2d_Curve> C1 = DrawTrSurf::GetCurve2d(a[1]);
  if (C1.IsNull())
  {
    di << "Syntax error: '" << a[1] << "' is Null";
    return 1;
  }

  occ::handle<Geom2d_Curve> C2 = DrawTrSurf::GetCurve2d(a[2]);
  if (C2.IsNull())
  {
    di << "Syntax error: '" << a[2] << "' is Null";
    return 1;
  }

  Geom2dAdaptor_Curve                        AC1(C1), AC2(C2);
  GeomAbs_CurveType                          T1 = AC1.GetType(), T2 = AC2.GetType();
  Handle(NCollection_Shared<IntAna2d_Conic>) pCon;
  switch (T2)
  {
    case GeomAbs_Line: {
      pCon.reset(new NCollection_Shared<IntAna2d_Conic>(AC2.Line()));
      break;
    }
    case GeomAbs_Circle: {
      pCon.reset(new NCollection_Shared<IntAna2d_Conic>(AC2.Circle()));
      break;
    }
    case GeomAbs_Ellipse: {
      pCon.reset(new NCollection_Shared<IntAna2d_Conic>(AC2.Ellipse()));
      break;
    }
    case GeomAbs_Hyperbola: {
      pCon.reset(new NCollection_Shared<IntAna2d_Conic>(AC2.Hyperbola()));
      break;
    }
    case GeomAbs_Parabola: {
      pCon.reset(new NCollection_Shared<IntAna2d_Conic>(AC2.Parabola()));
      break;
    }
    default: {
      di << "Syntax error: '" << a[2] << "' is not conic";
      return 1;
    }
  }

  IntAna2d_AnaIntersection Intersector;
  switch (T1)
  {
    case GeomAbs_Line:
      Intersector.Perform(AC1.Line(), *pCon);
      break;
    case GeomAbs_Circle:
      Intersector.Perform(AC1.Circle(), *pCon);
      break;
    case GeomAbs_Ellipse:
      Intersector.Perform(AC1.Ellipse(), *pCon);
      break;
    case GeomAbs_Hyperbola:
      Intersector.Perform(AC1.Hyperbola(), *pCon);
      break;
    case GeomAbs_Parabola:
      Intersector.Perform(AC1.Parabola(), *pCon);
      break;
    default:
      di << "Syntax error: '" << a[1] << "' is not conic";
      return 1;
  }

  for (int i = 1; i <= Intersector.NbPoints(); i++)
  {
    gp_Pnt2d P = Intersector.Point(i).Value();
    di << "Intersection point " << i << " : " << P.X() << " " << P.Y() << "\n";
    di << "parameter on the fist: " << Intersector.Point(i).ParamOnFirst();
    if (!Intersector.Point(i).SecondIsImplicit())
    {
      di << " parameter on the second: " << Intersector.Point(i).ParamOnSecond() << "\n";
    }
    else
    {
      di << "\n";
    }
    occ::handle<Draw_Marker2D> mark = new Draw_Marker2D(P, Draw_X, Draw_vert);
    dout << mark;
  }
  dout.Flush();
  return 0;
}

//=================================================================================================

static int deviation(Draw_Interpretor& theDI, int theNArg, const char** theArgv)
{
  if (theNArg < 3)
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  const occ::handle<Geom2d_Curve> aC = DrawTrSurf::GetCurve2d(theArgv[2]);

  if (aC.IsNull())
  {
    theDI << "Error: " << theArgv[2] << " is not a 2D-curve.\n";
    return 1;
  }

  Geom2dAdaptor_Curve anAC(aC);

  int    aNbInterv      = 2;
  double aU0            = RealLast();
  int    aNbApprox      = 10;
  int    aNbExact       = 100;
  bool   anIsApproxOnly = false;

  for (int aCurrArg = 3; aCurrArg < theNArg; aCurrArg++)
  {
    TCollection_AsciiString anArg(theArgv[aCurrArg]);
    anArg.LowerCase();
    if (anArg == "-i")
    {
      aU0 = Draw::Atof(theArgv[++aCurrArg]);
    }
    else if (anArg == "-d")
    {
      aNbInterv = Draw::Atoi(theArgv[++aCurrArg]);
    }
    else if (anArg == "-napprox")
    {
      aNbApprox = Draw::Atoi(theArgv[++aCurrArg]);
    }
    else if (anArg == "-nexact")
    {
      aNbExact = Draw::Atoi(theArgv[++aCurrArg]);
    }
    else if (anArg == "-approxonly")
    {
      anIsApproxOnly = true;
      ++aCurrArg;
    }
    else
    {
      theDI << "Error: Wrong option " << theArgv[aCurrArg] << "\n";
      return 1;
    }
  }

  const double aU1 = anAC.FirstParameter();
  const double aU2 = anAC.LastParameter();

  double   aRetCurvParam = aU0;
  gp_Pnt2d aPtOnCurv;
  gp_Vec2d aRetVec;
  gp_Lin2d aLinSegm;

  double aDefl = RealLast();

  if (aU0 == RealLast() || anIsApproxOnly)
  {
    aDefl = GeomLib_Tool::ComputeDeviation(anAC, aU1, aU2, aNbInterv, aNbApprox, &aU0);

    if (aDefl < 0.0)
    {
      theDI << "Error: Cannot compute deviation on interval.\n";
      return 0;
    }
  }
  if (!anIsApproxOnly)
  {
    aDefl = GeomLib_Tool::ComputeDeviation(anAC,
                                           aU1,
                                           aU2,
                                           aU0,
                                           aNbExact,
                                           &aRetCurvParam,
                                           &aPtOnCurv,
                                           &aRetVec,
                                           &aLinSegm);
  }
  if (aDefl < 0.0)
  {
    theDI << "Error: Cannot compute a deviation!\n";
    return 0;
  }
  theDI << "Computed value is: " << aDefl << "\n";
  TCollection_AsciiString anArgString = theArgv[1];
  TCollection_AsciiString aPntString  = anArgString + "_pnt";
  DrawTrSurf::Set(aPntString.ToCString(), aPtOnCurv);
  theDI << "From point " << aPntString << " (with parameter " << aRetCurvParam << ") to ";

  occ::handle<Geom2d_Curve> aLine      = new Geom2d_Line(aLinSegm);
  TCollection_AsciiString   aLinString = anArgString + "_lin";
  DrawTrSurf::Set(aLinString.ToCString(), aLine);
  theDI << "the line " << aLinString << ".\n";

  aLine                               = new Geom2d_Line(aPtOnCurv, aRetVec);
  aLine                               = new Geom2d_TrimmedCurve(aLine, 0.0, aDefl);
  TCollection_AsciiString aNormString = anArgString + "_norm";
  DrawTrSurf::Set(aNormString.ToCString(), aLine);
  theDI << "The deflection is measured along the line " << aNormString << ".\n";

  return 0;
}

void GeomliteTest::API2dCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;

  const char* g;

  done = true;
  g    = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("2dproj", "proj curve x y", __FILE__, proj, g);

  g = "GEOMETRY approximations";

  theCommands.Add("2dapprox",
                  "2dapprox result nbpoint [curve] [[x] y [x] y...]",
                  __FILE__,
                  appro,
                  g);
  theCommands.Add("2dinterpole",
                  "2dinterpole result nbpoint [curve] [[x] y [x] y ...]",
                  __FILE__,
                  appro,
                  g);

  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("2dextrema", "extrema curve curve", __FILE__, extrema, g);

  g = "GEOMETRY intersections";

  theCommands.Add("2dintersect",
                  "2dintersect curve1 [curve2] [-tol tol] [-state]\n"
                  "Intersects the given 2d curve(s)."
                  "If only one curve is given, it will be checked on self-intersection.\n"
                  "Options:\n"
                  " -tol - allows changing the intersection tolerance (default value is 1.e-3);\n"
                  " -state - allows printing the intersection state for each point.",
                  __FILE__,
                  intersect,
                  g);

  theCommands.Add("2dintanalytical",
                  "2dintanalytical circle1 circle2"
                  "Intersect circle1 and circle2 using IntAna2d_AnaIntersection.",
                  __FILE__,
                  intersect_ana,
                  g);
  theCommands.Add("intconcon",
                  "intconcon curve1 curve2"
                  "Intersect conic curve1 and conic curve2 using IntAna2d_AnaIntersection",
                  __FILE__,
                  intconcon,
                  g);

  theCommands.Add("2ddeviation",
                  "2ddeviation result curve [-i U0] [-d N] [-Napprox N] [-Nexact N] [-approxOnly]\n"
                  "-i - sets an initial parameter for computation by iterative method;\n"
                  "-d - sets number of sub-intervals for searching. Default value is 2.\n"
                  "-Napprox - sets number of iteration for approx deviation computing,\n"
                  "           defauilt value is 10"
                  "-Nexact - sets number of iteration for exact deviation computing,\n"
                  "          defauilt value is 100"
                  "-approxOnly - to find deviation with approx method only,\n"
                  "              the exact method is used if this parameter is not specified",
                  __FILE__,
                  deviation,
                  g);
}
