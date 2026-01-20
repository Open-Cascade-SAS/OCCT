// Created on: 1995-01-17
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

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <GeometryTest.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ExtremaCurveSurface.hxx>
#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <Message.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <stdio.h>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=================================================================================================

static void showProjSolution(Draw_Interpretor&      di,
                             const int i,
                             const gp_Pnt&          P,
                             const gp_Pnt&          P1,
                             const double    U,
                             const double    V,
                             const bool isSurface)
{
  char name[100];
  Sprintf(name, "%s%d", "ext_", i);
  di << name << " ";
  char* temp = name; // portage WNT
  if (P.Distance(P1) > Precision::Confusion())
  {
    occ::handle<Geom_Line>         L  = new Geom_Line(P, gp_Vec(P, P1));
    occ::handle<Geom_TrimmedCurve> CT = new Geom_TrimmedCurve(L, 0., P.Distance(P1));
    DrawTrSurf::Set(temp, CT);
  }
  else
  {
    DrawTrSurf::Set(temp, P1);
    if (isSurface)
      di << " Point on surface ";
    else
      di << " Point on curve ";
  }
  if (isSurface)
    di << " Parameters: " << U << " " << V << "\n";
  else
    di << " parameter " << i << " = " << U << "\n";
}

//=================================================================================================

static int proj(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
  {
    Message::SendFail() << " Use proj curve/surf x y z [{extrema algo: g(grad)/t(tree)}|{u v}]";
    return 1;
  }

  gp_Pnt P(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));

  occ::handle<Geom_Curve>   GC = DrawTrSurf::GetCurve(a[1]);
  occ::handle<Geom_Surface> GS;
  Extrema_ExtAlgo      aProjAlgo = Extrema_ExtAlgo_Grad;

  if (n == 6 && a[5][0] == 't')
    aProjAlgo = Extrema_ExtAlgo_Tree;

  if (GC.IsNull())
  {
    GS = DrawTrSurf::GetSurface(a[1]);

    if (GS.IsNull())
      return 1;

    if (n <= 6)
    {
      double U1, U2, V1, V2;
      GS->Bounds(U1, U2, V1, V2);

      GeomAPI_ProjectPointOnSurf proj(P, GS, U1, U2, V1, V2, aProjAlgo);
      if (!proj.IsDone())
      {
        di << "projection failed.";
        return 0;
      }

      double UU, VV;
      for (int i = 1; i <= proj.NbPoints(); i++)
      {
        gp_Pnt P1 = proj.Point(i);
        proj.Parameters(i, UU, VV);
        showProjSolution(di, i, P, P1, UU, VV, true);
      }
    }
    else if (n == 7)
    {
      const gp_XY            aP2d(Draw::Atof(a[5]), Draw::Atof(a[6]));
      GeomAdaptor_Surface    aGAS(GS);
      Extrema_GenLocateExtPS aProjector(aGAS, Precision::PConfusion(), Precision::PConfusion());
      aProjector.Perform(P, aP2d.X(), aP2d.Y(), false);
      if (!aProjector.IsDone())
      {
        di << "projection failed.";
        return 0;
      }

      const Extrema_POnSurf& aP = aProjector.Point();
      double          UU, VV;
      aP.Parameter(UU, VV);
      showProjSolution(di, 1, P, aP.Value(), UU, VV, true);
    }
  }
  else
  {
    GeomAPI_ProjectPointOnCurve proj(P, GC, GC->FirstParameter(), GC->LastParameter());

    if (proj.NbPoints() == 0)
    {
      std::cout << "No project point was found." << std::endl;
      return 0;
    }

    for (int i = 1; i <= proj.NbPoints(); i++)
    {
      gp_Pnt        P1 = proj.Point(i);
      double UU = proj.Parameter(i);
      showProjSolution(di, i, P, P1, UU, UU, false);
    }
  }

  return 0;
}

//=================================================================================================

static int appro(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  occ::handle<Geom_Curve> GC;
  int   Nb = Draw::Atoi(a[2]);

  NCollection_Array1<gp_Pnt> Points(1, Nb);

  occ::handle<Draw_Marker3D> mark;

  if (n == 4)
  {
    GC = DrawTrSurf::GetCurve(a[3]);
    if (GC.IsNull())
      return 1;

    double U, U1, U2;
    U1                  = GC->FirstParameter();
    U2                  = GC->LastParameter();
    double Delta = (U2 - U1) / (Nb - 1);
    for (int i = 1; i <= Nb; i++)
    {
      U         = U1 + (i - 1) * Delta;
      Points(i) = GC->Value(U);
      mark      = new Draw_Marker3D(Points(i), Draw_X, Draw_vert);
      dout << mark;
    }
  }
  else
  {
    int id, XX, YY, b;
    dout.Select(id, XX, YY, b);
    double zoom = dout.Zoom(id);

    Points(1) = gp_Pnt(((double)XX) / zoom, ((double)YY) / zoom, 0.);

    mark = new Draw_Marker3D(Points(1), Draw_X, Draw_vert);

    dout << mark;

    for (int i = 2; i <= Nb; i++)
    {
      dout.Select(id, XX, YY, b);
      Points(i) = gp_Pnt(((double)XX) / zoom, ((double)YY) / zoom, 0.);
      mark      = new Draw_Marker3D(Points(i), Draw_X, Draw_vert);
      dout << mark;
    }
  }
  dout.Flush();
  int Dmin  = 3;
  int Dmax  = 8;
  double    Tol3d = 1.e-3;

  occ::handle<Geom_BSplineCurve> TheCurve;
  GeomAPI_PointsToBSpline   aPointToBSpline(Points, Dmin, Dmax, GeomAbs_C2, Tol3d);
  TheCurve = aPointToBSpline.Curve();

  DrawTrSurf::Set(a[1], TheCurve);
  di << a[1];

  return 0;
}

//=================================================================================================

static int grilapp(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 12)
    return 1;

  int     i, j;
  int     Nu = Draw::Atoi(a[2]);
  int     Nv = Draw::Atoi(a[3]);
  NCollection_Array2<double> ZPoints(1, Nu, 1, Nv);

  double X0 = Draw::Atof(a[4]);
  double dX = Draw::Atof(a[5]);
  double Y0 = Draw::Atof(a[6]);
  double dY = Draw::Atof(a[7]);

  int Count = 8;
  for (j = 1; j <= Nv; j++)
  {
    for (i = 1; i <= Nu; i++)
    {
      if (Count > n)
        return 1;
      ZPoints(i, j) = Draw::Atof(a[Count]);
      Count++;
    }
  }

  occ::handle<Geom_BSplineSurface> S = GeomAPI_PointsToBSplineSurface(ZPoints, X0, dX, Y0, dY);
  DrawTrSurf::Set(a[1], S);

  di << a[1];

  return 0;
}

//=================================================================================================

static int surfapp(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
    return 1;

  int   i, j;
  int   Nu = Draw::Atoi(a[2]);
  int   Nv = Draw::Atoi(a[3]);
  NCollection_Array2<gp_Pnt> Points(1, Nu, 1, Nv);
  bool   IsPeriodic = false;
  bool   RemoveLast = false;

  if (n >= 5 && n <= 6)
  {
    occ::handle<Geom_Surface> Surf = DrawTrSurf::GetSurface(a[4]);
    if (Surf.IsNull())
      return 1;

    double U, V, U1, V1, U2, V2;
    Surf->Bounds(U1, U2, V1, V2);
    for (j = 1; j <= Nv; j++)
    {
      V = V1 + (j - 1) * (V2 - V1) / (Nv - 1);
      for (i = 1; i <= Nu; i++)
      {
        U            = U1 + (i - 1) * (U2 - U1) / (Nu - 1);
        Points(i, j) = Surf->Value(U, V);
      }
    }
    if (n == 6)
    {
      int ip = Draw::Atoi(a[5]);
      if (ip > 0)
        IsPeriodic = true;
    }
    if (IsPeriodic)
    {
      for (j = 1; j <= Nv; j++)
      {
        double d = Points(1, j).Distance(Points(Nu, j));
        if (d <= Precision::Confusion())
        {
          RemoveLast = true;
          break;
        }
      }
    }
  }
  else if (n >= 16)
  {
    int Count = 4;
    for (j = 1; j <= Nv; j++)
    {
      for (i = 1; i <= Nu; i++)
      {
        if (Count > n)
          return 1;
        Points(i, j) =
          gp_Pnt(Draw::Atof(a[Count]), Draw::Atof(a[Count + 1]), Draw::Atof(a[Count + 2]));
        Count += 3;
      }
    }
  }
  char             name[100];
  int Count = 1;
  for (j = 1; j <= Nv; j++)
  {
    for (i = 1; i <= Nu; i++)
    {
      Sprintf(name, "point_%d", Count++);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, Points(i, j));
    }
  }

  GeomAPI_PointsToBSplineSurface anApprox;
  if (RemoveLast)
  {
    NCollection_Array2<gp_Pnt> Points1(1, Nu - 1, 1, Nv);
    for (j = 1; j <= Nv; j++)
    {
      for (i = 1; i <= Nu - 1; i++)
      {
        Points1(i, j) = Points(i, j);
      }
    }
    anApprox.Init(Points1, Approx_ChordLength, 3, 8, GeomAbs_C2, 1.e-3, IsPeriodic);
  }
  else
  {
    anApprox.Init(Points, Approx_ChordLength, 3, 8, GeomAbs_C2, 1.e-3, IsPeriodic);
  }

  if (anApprox.IsDone())
  {
    const occ::handle<Geom_BSplineSurface>& S = anApprox.Surface();
    DrawTrSurf::Set(a[1], S);
    di << a[1];
  }

  return 0;
}

//=================================================================================================

static int surfint(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
    return 1;

  occ::handle<Geom_Surface> Surf = DrawTrSurf::GetSurface(a[2]);
  if (Surf.IsNull())
    return 1;
  int   i, j;
  int   Nu = Draw::Atoi(a[3]);
  int   Nv = Draw::Atoi(a[4]);
  NCollection_Array2<gp_Pnt> Points(1, Nu, 1, Nv);

  double U, V, U1, V1, U2, V2;
  Surf->Bounds(U1, U2, V1, V2);
  for (j = 1; j <= Nv; j++)
  {
    V = V1 + (j - 1) * (V2 - V1) / (Nv - 1);
    for (i = 1; i <= Nu; i++)
    {
      U            = U1 + (i - 1) * (U2 - U1) / (Nu - 1);
      Points(i, j) = Surf->Value(U, V);
    }
  }

  char             name[100];
  int Count = 1;
  for (j = 1; j <= Nv; j++)
  {
    for (i = 1; i <= Nu; i++)
    {
      Sprintf(name, "point_%d", Count++);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, Points(i, j));
    }
  }

  bool IsPeriodic = false;
  if (n > 5)
  {
    int ip = Draw::Atoi(a[5]);
    if (ip > 0)
      IsPeriodic = true;
  }
  bool RemoveLast = false;
  if (IsPeriodic)
  {
    for (j = 1; j <= Nv; j++)
    {
      double d = Points(1, j).Distance(Points(Nu, j));
      if (d <= Precision::Confusion())
      {
        RemoveLast = true;
        break;
      }
    }
  }
  const Approx_ParametrizationType ParType = Approx_ChordLength;
  GeomAPI_PointsToBSplineSurface   anApprox;
  if (RemoveLast)
  {
    NCollection_Array2<gp_Pnt> Points1(1, Nu - 1, 1, Nv);
    for (j = 1; j <= Nv; j++)
    {
      for (i = 1; i <= Nu - 1; i++)
      {
        Points1(i, j) = Points(i, j);
      }
    }
    anApprox.Interpolate(Points1, ParType, IsPeriodic);
  }
  else
  {
    anApprox.Interpolate(Points, ParType, IsPeriodic);
  }
  if (anApprox.IsDone())
  {
    const occ::handle<Geom_BSplineSurface>& S = anApprox.Surface();
    DrawTrSurf::Set(a[1], S);
    di << a[1];
  }
  else
  {
    di << "Interpolation not done \n";
  }

  return 0;
}

//=================================================================================================

static int extrema(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
  {
    return 1;
  }

  occ::handle<Geom_Curve>   GC1, GC2;
  occ::handle<Geom_Surface> GS1, GS2;

  bool isInfinitySolutions = false;
  double    aMinDist            = RealLast();

  double U1f, U1l, U2f, U2l, V1f = 0., V1l = 0., V2f = 0., V2l = 0.;

  GC1 = DrawTrSurf::GetCurve(a[1]);
  if (GC1.IsNull())
  {
    GS1 = DrawTrSurf::GetSurface(a[1]);
    if (GS1.IsNull())
      return 1;

    GS1->Bounds(U1f, U1l, V1f, V1l);
  }
  else
  {
    U1f = GC1->FirstParameter();
    U1l = GC1->LastParameter();
  }

  GC2 = DrawTrSurf::GetCurve(a[2]);
  if (GC2.IsNull())
  {
    GS2 = DrawTrSurf::GetSurface(a[2]);
    if (GS2.IsNull())
      return 1;
    GS2->Bounds(U2f, U2l, V2f, V2l);
  }
  else
  {
    U2f = GC2->FirstParameter();
    U2l = GC2->LastParameter();
  }

  NCollection_Vector<gp_Pnt>        aPnts1, aPnts2;
  NCollection_Vector<double> aPrms[4];
  if (!GC1.IsNull() && !GC2.IsNull())
  {
    GeomAPI_ExtremaCurveCurve Ex(GC1, GC2, U1f, U1l, U2f, U2l);

    // Since GeomAPI cannot provide access to flag directly.
    isInfinitySolutions = Ex.Extrema().IsParallel();
    if (isInfinitySolutions)
    {
      aMinDist = Ex.LowerDistance();
    }
    else
    {
      for (int aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
      {
        gp_Pnt aP1, aP2;
        Ex.Points(aJ, aP1, aP2);
        aPnts1.Append(aP1);
        aPnts2.Append(aP2);

        double aU1, aU2;
        Ex.Parameters(aJ, aU1, aU2);
        aPrms[0].Append(aU1);
        aPrms[2].Append(aU2);
      }
    }
  }
  else if (!GC1.IsNull() && !GS2.IsNull())
  {
    GeomAPI_ExtremaCurveSurface Ex(GC1, GS2, U1f, U1l, U2f, U2l, V2f, V2l);

    isInfinitySolutions = Ex.Extrema().IsParallel();
    if (isInfinitySolutions)
    {
      aMinDist = Ex.LowerDistance();
    }
    else
    {
      for (int aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
      {
        gp_Pnt aP1, aP2;
        Ex.Points(aJ, aP1, aP2);
        aPnts1.Append(aP1);
        aPnts2.Append(aP2);

        double aU1, aU2, aV2;
        Ex.Parameters(aJ, aU1, aU2, aV2);
        aPrms[0].Append(aU1);
        aPrms[2].Append(aU2);
        aPrms[3].Append(aV2);
      }
    }
  }
  else if (!GS1.IsNull() && !GC2.IsNull())
  {
    GeomAPI_ExtremaCurveSurface Ex(GC2, GS1, U2f, U2l, U1f, U1l, V1f, V1l);

    isInfinitySolutions = Ex.Extrema().IsParallel();
    if (isInfinitySolutions)
    {
      aMinDist = Ex.LowerDistance();
    }
    else
    {
      for (int aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
      {
        gp_Pnt aP2, aP1;
        Ex.Points(aJ, aP2, aP1);
        aPnts1.Append(aP1);
        aPnts2.Append(aP2);

        double aU1, aV1, aU2;
        Ex.Parameters(aJ, aU2, aU1, aV1);
        aPrms[0].Append(aU1);
        aPrms[1].Append(aV1);
        aPrms[2].Append(aU2);
      }
    }
  }
  else if (!GS1.IsNull() && !GS2.IsNull())
  {
    GeomAPI_ExtremaSurfaceSurface Ex(GS1, GS2, U1f, U1l, V1f, V1l, U2f, U2l, V2f, V2l);
    // Since GeomAPI cannot provide access to flag directly.
    isInfinitySolutions = Ex.Extrema().IsParallel();
    if (isInfinitySolutions)
    {
      aMinDist = Ex.LowerDistance();
    }
    else
    {
      for (int aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
      {
        gp_Pnt aP1, aP2;
        Ex.Points(aJ, aP1, aP2);
        aPnts1.Append(aP1);
        aPnts2.Append(aP2);

        double aU1, aV1, aU2, aV2;
        Ex.Parameters(aJ, aU1, aV1, aU2, aV2);
        aPrms[0].Append(aU1);
        aPrms[1].Append(aV1);
        aPrms[2].Append(aU2);
        aPrms[3].Append(aV2);
      }
    }
  }

  char  aName[100];
  char* aName2 = aName; // portage WNT

  // Output points.
  const int aPntCount = aPnts1.Size();
  if (aPntCount == 0 || isInfinitySolutions)
  {
    // Infinity solutions flag may be set with 0 number of
    // solutions in analytic extrema Curve/Curve.
    if (isInfinitySolutions)
      di << "Infinite number of extremas, distance = " << aMinDist << "\n";
    else
      di << "No solutions!\n";
  }
  for (int aJ = 1; aJ <= aPntCount; aJ++)
  {
    gp_Pnt aP1 = aPnts1(aJ - 1), aP2 = aPnts2(aJ - 1);

    if (aP1.Distance(aP2) < 1.e-16)
    {
      di << "Extrema " << aJ << " is point : " << aP1.X() << " " << aP1.Y() << " " << aP1.Z()
         << "\n";
      continue;
    }

    occ::handle<Geom_Line>         aL  = new Geom_Line(aP1, gp_Vec(aP1, aP2));
    occ::handle<Geom_TrimmedCurve> aCT = new Geom_TrimmedCurve(aL, 0., aP1.Distance(aP2));
    Sprintf(aName, "%s%d", "ext_", aJ);
    DrawTrSurf::Set(aName2, aCT);
    di << aName << " ";
  }

  if (n >= 4)
  {
    // Output points.
    for (int aJ = 1; aJ <= aPntCount; aJ++)
    {
      gp_Pnt aP1 = aPnts1(aJ - 1), aP2 = aPnts2(aJ - 1);
      Sprintf(aName, "%s%d%s", "ext_", aJ, "_2");
      DrawTrSurf::Set(aName2, aP1);
      di << aName << " ";
      Sprintf(aName, "%s%d%s", "ext_", aJ, "_3");
      DrawTrSurf::Set(aName2, aP2);
      di << aName << " ";
    }

    // Output parameters.
    for (int aJ = 0; aJ < 4; ++aJ)
    {
      for (int aPrmCount = aPrms[aJ].Size(), aK = 0; aK < aPrmCount; ++aK)
      {
        double aP = aPrms[aJ](aK);
        Sprintf(aName, "%s%d%s%d", "prm_", aJ + 1, "_", aK + 1);
        Draw::Set(aName2, aP);
        di << aName << " ";
      }
    }
  }

  return 0;
}

//=================================================================================================

static int totalextcc(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  occ::handle<Geom_Curve> GC1, GC2;

  double U1f, U1l, U2f, U2l;

  GC1 = DrawTrSurf::GetCurve(a[1]);
  if (GC1.IsNull())
  {
    return 1;
  }
  else
  {
    U1f = GC1->FirstParameter();
    U1l = GC1->LastParameter();
  }

  GC2 = DrawTrSurf::GetCurve(a[2]);
  if (GC2.IsNull())
  {
    return 1;
  }
  else
  {
    U2f = GC2->FirstParameter();
    U2l = GC2->LastParameter();
  }

  char                      name[100];
  GeomAPI_ExtremaCurveCurve Ex(GC1, GC2, U1f, U1l, U2f, U2l);
  gp_Pnt                    P1, P2;
  if (Ex.TotalNearestPoints(P1, P2))
  {
    if (P1.Distance(P2) < 1.e-16)
    {
      di << "Extrema is point : " << P1.X() << " " << P1.Y() << " " << P1.Z() << "\n";
    }
    else
    {
      di << "Extrema is segment of line\n";
      occ::handle<Geom_Line>         L  = new Geom_Line(P1, gp_Vec(P1, P2));
      occ::handle<Geom_TrimmedCurve> CT = new Geom_TrimmedCurve(L, 0., P1.Distance(P2));
      Sprintf(name, "%s%d", "ext_", 1);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << " ";
    }

    double u1, u2;
    Ex.TotalLowerDistanceParameters(u1, u2);

    di << "Parameters on curves : " << u1 << " " << u2 << "\n";
  }
  else
  {
    di << "Curves are infinite and parallel\n";
  }

  di << "Minimal distance : " << Ex.TotalLowerDistance() << "\n";

  return 0;
}

void GeometryTest::APICommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;

  done = true;

  theCommands.Add("proj",
                  "proj curve/surf x y z [{extrema algo: g(grad)/t(tree)}|{u v}]\n"
                  "\t\tOptional parameters are relevant to surf only.\n"
                  "\t\tIf initial {u v} are given then local extrema is called",
                  __FILE__,
                  proj);

  theCommands.Add("appro", "appro result nbpoint [curve]", __FILE__, appro);
  theCommands.Add("surfapp", "surfapp result nbupoint nbvpoint x y z ....", __FILE__, surfapp);

  theCommands.Add("surfint",
                  "surfint result surf nbupoint nbvpoint [uperiodic]",
                  __FILE__,
                  surfint);

  theCommands.Add("grilapp",
                  "grilapp result nbupoint nbvpoint X0 dX Y0 dY z11 z12 .. z1nu ....  ",
                  __FILE__,
                  grilapp);

  theCommands.Add("extrema",
                  "extrema curve/surface curve/surface [extended_output = 0|1]",
                  __FILE__,
                  extrema);
  theCommands.Add("totalextcc", "totalextcc curve curve", __FILE__, totalextcc);
}
