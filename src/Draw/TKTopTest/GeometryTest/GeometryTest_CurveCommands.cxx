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

// 09/06/97 : JPI : suppression des commandes redondantes suite a la creation de GeomliteTest

#include <GeometryTest.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Color.hxx>

#include <GeomAPI.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAPI_IntSS.hxx>

// #include <GeomLProp.hxx>
#include <GeomProjLib.hxx>
#include <BSplCLib.hxx>

#include <gp.hxx>
#include <gp_Pln.hxx>

#include <Geom_Line.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>

#include <GccAna_Lin2dBisec.hxx>
#include <GccAna_Circ2dBisec.hxx>
#include <GccAna_CircLin2dBisec.hxx>
#include <GccAna_CircPnt2dBisec.hxx>
#include <GccAna_LinPnt2dBisec.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_IType.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Curve.hxx>

#include <Law_BSpline.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Integer.hxx>

#include <Adaptor3d_Curve.hxx>

#include <GeomAdaptor_Surface.hxx>

#include <ProjLib_HCompProjectedCurve.hxx>
#include <Precision.hxx>
#include <Message.hxx>

#include <Geom_Surface.hxx>
#include <cstdio>
#include <Geom_BSplineCurve.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GCPnts_DistFunction.hxx>
#include <gce_MakeLin.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <NCollection_Array2.hxx>
#include <Geom_BSplineSurface.hxx>
#include <DrawTrSurf_BSplineSurface.hxx>

// epa test
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <DBRep.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=================================================================================================

static int polelaw(Draw_Interpretor&, int n, const char** a)
{
  int k, jj, qq, i;

  if (n < 3)
    return 1;
  bool periodic = false;
  int  deg      = Draw::Atoi(a[2]);
  int  nbk      = Draw::Atoi(a[3]);

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

  int np;
  np = Sigma - deg - 1;
  NCollection_Array1<double> flat_knots(1, Sigma);
  jj = 1;
  for (i = 1; i <= nbk; i++)
  {
    for (qq = 1; qq <= mults(i); qq++)
    {
      flat_knots(jj) = knots(i);
      jj++;
    }
  }

  NCollection_Array1<gp_Pnt2d> poles(1, np);
  NCollection_Array1<double>   schoenberg_points(1, np);
  BSplCLib::BuildSchoenbergPoints(deg, flat_knots, schoenberg_points);
  for (i = 1; i <= np; i++)
  {
    poles(i).SetCoord(schoenberg_points(i), Draw::Atof(a[k]));
    k++;
  }

  occ::handle<Geom2d_BSplineCurve> result =
    new Geom2d_BSplineCurve(poles, knots, mults, deg, periodic);
  DrawTrSurf::Set(a[1], result);

  return 0;
}

//=================================================================================================

static int to2d(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;

  // get the curve
  occ::handle<Geom_Curve> C = DrawTrSurf::GetCurve(a[2]);
  if (C.IsNull())
    return 1;

  occ::handle<Geom_Surface> S;
  if (n >= 4)
  {
    S = DrawTrSurf::GetSurface(a[3]);
    if (S.IsNull())
      return 1;
  }
  else
    S = new Geom_Plane(gp::XOY());

  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(S);
  if (P.IsNull())
    return 1;
  occ::handle<Geom2d_Curve> r = GeomAPI::To2d(C, P->Pln());
  DrawTrSurf::Set(a[1], r);
  return 0;
}

//=================================================================================================

static int to3d(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;

  occ::handle<Geom2d_Curve> C = DrawTrSurf::GetCurve2d(a[2]);
  if (C.IsNull())
    return 1;

  occ::handle<Geom_Surface> S;
  if (n >= 4)
  {
    S = DrawTrSurf::GetSurface(a[3]);
    if (S.IsNull())
      return 1;
  }
  else
    S = new Geom_Plane(gp::XOY());

  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(S);
  if (P.IsNull())
    return 1;
  occ::handle<Geom_Curve> r = GeomAPI::To3d(C, P->Pln());

  DrawTrSurf::Set(a[1], r);
  return 0;
}

//=================================================================================================

static int gproject(Draw_Interpretor& di, int n, const char** a)
{
  TCollection_AsciiString newname;
  TCollection_AsciiString newname1;

  if (n < 4)
  {
    di << "gproject waits 3 or more arguments\n";
    return 1;
  }

  TCollection_AsciiString name = a[1];

  occ::handle<Geom_Curve>   Cur = DrawTrSurf::GetCurve(a[2]);
  occ::handle<Geom_Surface> Sur = DrawTrSurf::GetSurface(a[3]);
  if (Cur.IsNull() || Sur.IsNull())
    return 1;

  occ::handle<GeomAdaptor_Curve>   hcur = new GeomAdaptor_Curve(Cur);
  occ::handle<GeomAdaptor_Surface> hsur = new GeomAdaptor_Surface(Sur);

  int    index    = 4;
  double aTol3d   = 1.e-6;
  double aMaxDist = -1.0;

  if (n > 4 && a[4][0] != '-')
  {
    aTol3d = Draw::Atof(a[4]);
    index  = 5;

    if (n > 5 && a[5][0] != '-')
    {
      aMaxDist = Draw::Atof(a[5]);
      index    = 6;
    }
  }

  occ::handle<ProjLib_HCompProjectedCurve> HProjector =
    new ProjLib_HCompProjectedCurve(aTol3d, hsur, hcur, aMaxDist);
  ProjLib_CompProjectedCurve& Projector = *HProjector;

  GeomAbs_Shape aContinuity = GeomAbs_C2;
  int           aMaxDegree, aMaxSeg;
  bool          aProj2d;
  bool          aProj3d;

  while (index + 1 < n)
  {
    if (a[index][0] != '-')
      return 1;

    if (a[index][1] == 'c')
    {
      const char* aContinuityName = a[index + 1];
      if (!strcmp(aContinuityName, "C0"))
      {
        aContinuity = GeomAbs_C0;
      }
      else if (!strcmp(aContinuityName, "C1"))
      {
        aContinuity = GeomAbs_C1;
      }
      else if (!strcmp(aContinuityName, "C2"))
      {
        aContinuity = GeomAbs_C2;
      }

      Projector.SetContinuity(aContinuity);
    }
    else if (a[index][1] == 'd')
    {
      aMaxDegree = Draw::Atoi(a[index + 1]);
      aMaxDegree = aMaxDegree > 25 ? 25 : aMaxDegree;
      Projector.SetMaxDegree(aMaxDegree);
    }
    else if (a[index][1] == 's')
    {
      aMaxSeg = Draw::Atoi(a[index + 1]);
      Projector.SetMaxSeg(aMaxSeg);
    }
    else if (!strcmp(a[index], "-2d"))
    {
      aProj2d = Draw::Atoi(a[index + 1]) > 0;
      Projector.SetProj2d(aProj2d);
    }
    else if (!strcmp(a[index], "-3d"))
    {
      aProj3d = Draw::Atoi(a[index + 1]) > 0;
      Projector.SetProj3d(aProj3d);
    }

    index += 2;
  }

  Projector.Perform();

  for (int k = 1; k <= Projector.NbCurves(); k++)
  {
    newname  = name + "_" + TCollection_AsciiString(k);
    newname1 = name + "2d_" + TCollection_AsciiString(k);

    if (Projector.ResultIsPoint(k))
    {
      if (Projector.GetProj2d())
      {
        DrawTrSurf::Set(newname1.ToCString(), Projector.GetResult2dP(k));
        di << newname1 << " is pcurve\n";
      }
      if (Projector.GetProj3d())
      {
        DrawTrSurf::Set(newname.ToCString(), Projector.GetResult3dP(k));
        di << newname << " is 3d projected curve\n";
      }
    }
    else
    {
      if (Projector.GetProj2d())
      {
        DrawTrSurf::Set(newname1.ToCString(), Projector.GetResult2dC(k));

        di << newname1 << " is pcurve\n";
        di << " Tolerance reached in 2d is " << Projector.GetResult2dUApproxError(k) << ";  "
           << Projector.GetResult2dVApproxError(k) << "\n";
      }
      if (Projector.GetProj3d())
      {
        DrawTrSurf::Set(newname.ToCString(), Projector.GetResult3dC(k));

        di << newname << " is 3d projected curve\n";
        di << " Tolerance reached in 3d is " << Projector.GetResult3dApproxError(k) << "\n";
      }
    }
  }
  return 0;
}

//=================================================================================================

static int project(Draw_Interpretor& di, int n, const char** a)
{
  if (n == 1)
  {

    di << "project result2d c3d surf [-e p] [-v n] [-t tol]\n";
    di << "   -e p   : extent the surface of <p>%\n";
    di << "   -v n   : verify the projection at <n> points.\n";
    di << "   -t tol : set the tolerance for approximation\n";
    return 0;
  }

  if (n < 4)
    return 1;
  occ::handle<Geom_Surface> GS = DrawTrSurf::GetSurface(a[3]);
  if (GS.IsNull())
    return 1;

  occ::handle<Geom_Curve> GC = DrawTrSurf::GetCurve(a[2]);
  if (GC.IsNull())
    return 1;

  double tolerance = Precision::Confusion();

  double U1, U2, V1, V2;
  GS->Bounds(U1, U2, V1, V2);

  bool Verif    = false;
  int  NbPoints = 0;

  int index = 4;
  while (index + 1 < n)
  {
    if (a[index][0] != '-')
      return 1;

    if (a[index][1] == 'e')
    {
      double p  = Draw::Atof(a[index + 1]);
      double dU = p * (U2 - U1) / 100.;
      double dV = p * (V2 - V1) / 100.;
      U1 -= dU;
      U2 += dU;
      V1 -= dV;
      V2 += dV;
    }
    else if (a[index][1] == 'v')
    {
      Verif    = true;
      NbPoints = Draw::Atoi(a[index + 1]);
    }
    else if (a[index][1] == 't')
    {
      tolerance = Draw::Atof(a[index + 1]);
    }
    index += 2;
  }

  occ::handle<Geom2d_Curve> G2d = GeomProjLib::Curve2d(GC, GS, U1, U2, V1, V2, tolerance);

  if (G2d.IsNull())
  {
    di << "\nProjection Failed\n";
    return 1;
  }
  else
  {
    DrawTrSurf::Set(a[1], G2d);
  }
  if (Verif)
  { // verify the projection on n points
    if (NbPoints <= 0)
    {
      di << " n must be positive\n";
      return 0;
    }
    gp_Pnt   P1, P2;
    gp_Pnt2d P2d;

    double U, dU;
    double Dist, DistMax = -1.;
    U1 = GC->FirstParameter();
    U2 = GC->LastParameter();
    dU = (U2 - U1) / (NbPoints + 1);
    for (int i = 0; i <= NbPoints + 1; i++)
    {
      U    = U1 + i * dU;
      P1   = GC->Value(U);
      P2d  = G2d->Value(U);
      P2   = GS->Value(P2d.X(), P2d.Y());
      Dist = P1.Distance(P2);
      di << " Parameter = " << U << "\tDistance = " << Dist << "\n";
      if (Dist > DistMax)
        DistMax = Dist;
    }
    di << " **** Distance Maximale : " << DistMax << "\n";
  }

  return 0;
}

//=================================================================================================

int projonplane(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom_Surface> S = DrawTrSurf::GetSurface(a[3]);
  if (S.IsNull())
    return 1;

  occ::handle<Geom_Plane> Pl = occ::down_cast<Geom_Plane>(S);
  if (Pl.IsNull())
  {
    di << " The surface must be a plane\n";
    return 1;
  }

  occ::handle<Geom_Curve> C = DrawTrSurf::GetCurve(a[2]);
  if (C.IsNull())
    return 1;

  bool Param = true;
  if ((n == 5 && Draw::Atoi(a[4]) == 0) || (n == 8 && Draw::Atoi(a[7]) == 0))
    Param = false;

  gp_Dir D;

  if (n == 8)
  {
    D = gp_Dir(Draw::Atof(a[4]), Draw::Atof(a[5]), Draw::Atof(a[6]));
  }
  else
  {
    D = Pl->Pln().Position().Direction();
  }

  occ::handle<Geom_Curve> Res = GeomProjLib::ProjectOnPlane(C, Pl, D, Param);

  DrawTrSurf::Set(a[1], Res);
  return 0;
}

//=================================================================================================

static void solution(const occ::handle<GccInt_Bisec>& Bis, const char* name, const int i)
{
  char solname[200];
  if (i == 0)
    Sprintf(solname, "%s", name);
  else
    Sprintf(solname, "%s_%d", name, i);
  const char* temp = solname; // pour portage WNT

  switch (Bis->ArcType())
  {
    case GccInt_Lin:
      DrawTrSurf::Set(temp, new Geom2d_Line(Bis->Line()));
      break;
    case GccInt_Cir:
      DrawTrSurf::Set(temp, new Geom2d_Circle(Bis->Circle()));
      break;
    case GccInt_Ell:
      DrawTrSurf::Set(temp, new Geom2d_Ellipse(Bis->Ellipse()));
      break;
    case GccInt_Par:
      DrawTrSurf::Set(temp, new Geom2d_Parabola(Bis->Parabola()));
      break;
    case GccInt_Hpr:
      DrawTrSurf::Set(temp, new Geom2d_Hyperbola(Bis->Hyperbola()));
      break;
    case GccInt_Pnt:
      DrawTrSurf::Set(temp, Bis->Point());
      break;
  }
}

static int bisec(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom2d_Curve> C1 = DrawTrSurf::GetCurve2d(a[2]);
  occ::handle<Geom2d_Curve> C2 = DrawTrSurf::GetCurve2d(a[3]);
  gp_Pnt2d                  P1, P2;
  bool                      ip1 = DrawTrSurf::GetPoint2d(a[2], P1);
  bool                      ip2 = DrawTrSurf::GetPoint2d(a[3], P2);
  int                       i, Compt = 0;
  int                       NbSol = 0;

  if (!C1.IsNull())
  {
    occ::handle<Standard_Type> Type1 = C1->DynamicType();
    if (!C2.IsNull())
    {
      occ::handle<Standard_Type> Type2 = C2->DynamicType();
      if (Type1 == STANDARD_TYPE(Geom2d_Line) && Type2 == STANDARD_TYPE(Geom2d_Line))
      {
        GccAna_Lin2dBisec Bis(occ::down_cast<Geom2d_Line>(C1)->Lin2d(),
                              occ::down_cast<Geom2d_Line>(C2)->Lin2d());
        if (Bis.IsDone())
        {
          char solname[200];
          NbSol = Bis.NbSolutions();
          for (i = 1; i <= NbSol; i++)
          {
            Sprintf(solname, "%s_%d", a[1], i);
            const char* temp = solname; // pour portage WNT
            DrawTrSurf::Set(temp, new Geom2d_Line(Bis.ThisSolution(i)));
          }
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
      else if (Type1 == STANDARD_TYPE(Geom2d_Line) && Type2 == STANDARD_TYPE(Geom2d_Circle))
      {
        GccAna_CircLin2dBisec Bis(occ::down_cast<Geom2d_Circle>(C2)->Circ2d(),
                                  occ::down_cast<Geom2d_Line>(C1)->Lin2d());
        if (Bis.IsDone())
        {
          NbSol = Bis.NbSolutions();
          if (NbSol >= 2)
            Compt = 1;
          for (i = 1; i <= NbSol; i++)
          {
            solution(Bis.ThisSolution(i), a[1], Compt);
            Compt++;
          }
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
      else if (Type2 == STANDARD_TYPE(Geom2d_Line) && Type1 == STANDARD_TYPE(Geom2d_Circle))
      {
        GccAna_CircLin2dBisec Bis(occ::down_cast<Geom2d_Circle>(C1)->Circ2d(),
                                  occ::down_cast<Geom2d_Line>(C2)->Lin2d());
        if (Bis.IsDone())
        {
          //	  char solname[200];
          NbSol = Bis.NbSolutions();
          if (NbSol >= 2)
            Compt = 1;
          for (i = 1; i <= NbSol; i++)
          {
            solution(Bis.ThisSolution(i), a[1], Compt);
            Compt++;
          }
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
      else if (Type2 == STANDARD_TYPE(Geom2d_Circle) && Type1 == STANDARD_TYPE(Geom2d_Circle))
      {
        GccAna_Circ2dBisec Bis(occ::down_cast<Geom2d_Circle>(C1)->Circ2d(),
                               occ::down_cast<Geom2d_Circle>(C2)->Circ2d());
        if (Bis.IsDone())
        {
          //	  char solname[200];
          NbSol = Bis.NbSolutions();
          if (NbSol >= 2)
            Compt = 1;
          for (i = 1; i <= NbSol; i++)
          {
            solution(Bis.ThisSolution(i), a[1], Compt);
            Compt++;
          }
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
      else
      {
        di << " args must be line/circle/point line/circle/point\n";
        return 1;
      }
    }
    else if (ip2)
    {
      if (Type1 == STANDARD_TYPE(Geom2d_Circle))
      {
        GccAna_CircPnt2dBisec Bis(occ::down_cast<Geom2d_Circle>(C1)->Circ2d(), P2);
        if (Bis.IsDone())
        {
          NbSol = Bis.NbSolutions();
          if (NbSol >= 2)
            Compt = 1;
          for (i = 1; i <= NbSol; i++)
          {
            solution(Bis.ThisSolution(i), a[1], Compt);
            Compt++;
          }
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
      else if (Type1 == STANDARD_TYPE(Geom2d_Line))
      {
        GccAna_LinPnt2dBisec Bis(occ::down_cast<Geom2d_Line>(C1)->Lin2d(), P2);
        if (Bis.IsDone())
        {
          NbSol = 1;
          solution(Bis.ThisSolution(), a[1], 0);
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
    }
    else
    {
      di << " the second arg must be line/circle/point \n";
    }
  }
  else if (ip1)
  {
    if (!C2.IsNull())
    {
      occ::handle<Standard_Type> Type2 = C2->DynamicType();
      if (Type2 == STANDARD_TYPE(Geom2d_Circle))
      {
        GccAna_CircPnt2dBisec Bis(occ::down_cast<Geom2d_Circle>(C2)->Circ2d(), P1);
        if (Bis.IsDone())
        {
          NbSol = Bis.NbSolutions();
          if (NbSol >= 2)
            Compt = 1;
          for (i = 1; i <= Bis.NbSolutions(); i++)
          {
            solution(Bis.ThisSolution(i), a[1], Compt);
            Compt++;
          }
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
      else if (Type2 == STANDARD_TYPE(Geom2d_Line))
      {
        GccAna_LinPnt2dBisec Bis(occ::down_cast<Geom2d_Line>(C2)->Lin2d(), P1);
        if (Bis.IsDone())
        {
          NbSol = 1;
          solution(Bis.ThisSolution(), a[1], 0);
        }
        else
        {
          di << " Bisec has failed !!\n";
          return 1;
        }
      }
    }
    else if (ip2)
    {
      GccAna_Pnt2dBisec Bis(P1, P2);
      if (Bis.HasSolution())
      {
        NbSol = 1;
        DrawTrSurf::Set(a[1], new Geom2d_Line(Bis.ThisSolution()));
      }
      else
      {
        di << " Bisec has failed !!\n";
        return 1;
      }
    }
    else
    {
      di << " the second arg must be line/circle/point \n";
      return 1;
    }
  }
  else
  {
    di << " args must be line/circle/point line/circle/point\n";
    return 1;
  }

  if (NbSol >= 2)
  {
    di << "There are " << NbSol << " Solutions.\n";
  }
  else
  {
    di << "There is " << NbSol << " Solution.\n";
  }

  return 0;
}

//=================================================================================================

static int movelaw(Draw_Interpretor& di, int n, const char** a)
{
  int    ii, condition = 0, error_status;
  double u, x, tolerance, tx;

  u         = Draw::Atof(a[2]);
  x         = Draw::Atof(a[3]);
  tolerance = 1.0e-5;
  if (n < 5)
  {
    return 1;
  }
  occ::handle<Geom2d_BSplineCurve> G2 = DrawTrSurf::GetBSplineCurve2d(a[1]);
  if (!G2.IsNull())
  {
    tx = Draw::Atof(a[4]);
    if (n == 6)
    {
      condition = std::max(Draw::Atoi(a[5]), -1);
      condition = std::min(condition, G2->Degree() - 1);
    }
    NCollection_Array1<gp_Pnt2d>      curve_poles(G2->Poles());
    NCollection_Array1<double>        law_poles(1, G2->NbPoles());
    const NCollection_Array1<double>& law_knots = G2->Knots();
    const NCollection_Array1<int>&    law_mults = G2->Multiplicities();
    for (ii = 1; ii <= G2->NbPoles(); ii++)
    {
      law_poles(ii) = curve_poles(ii).Coord(2);
    }

    Law_BSpline a_law(law_poles, law_knots, law_mults, G2->Degree(), false);

    a_law.MovePointAndTangent(u, x, tx, tolerance, condition, condition, error_status);

    for (ii = 1; ii <= G2->NbPoles(); ii++)
    {
      curve_poles(ii).SetCoord(2, a_law.Pole(ii));
      G2->SetPole(ii, curve_poles(ii));
    }

    if (!error_status)
    {
      Draw::Repaint();
    }
    else
    {
      di << "Not enough degree of freedom increase degree please\n";
    }
  }
  return 0;
}

// Static method computing deviation of curve and polyline
#include <math_PSO.hxx>
#include <math_PSOParticlesPool.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_BrentMinimum.hxx>

static double CompLocalDev(const Adaptor3d_Curve& theCurve, const double u1, const double u2);

static void ComputeDeviation(const Adaptor3d_Curve&                theCurve,
                             const occ::handle<Geom_BSplineCurve>& thePnts,
                             double&                               theDmax,
                             double&                               theUfMax,
                             double&                               theUlMax,
                             int&                                  theImax)
{
  theDmax  = 0.;
  theUfMax = 0.;
  theUlMax = 0.;
  theImax  = 0;

  // take knots
  const NCollection_Array1<double>& aKnots = thePnts->Knots();
  int                               nbp    = aKnots.Length();

  int i;
  for (i = 1; i < nbp; ++i)
  {
    double u1 = aKnots(i), u2 = aKnots(i + 1);
    double d = CompLocalDev(theCurve, u1, u2);
    if (d > theDmax)
    {
      theDmax  = d;
      theImax  = i;
      theUfMax = u1;
      theUlMax = u2;
    }
  }
}

double CompLocalDev(const Adaptor3d_Curve& theCurve, const double u1, const double u2)
{
  math_Vector aLowBorder(1, 1);
  math_Vector aUppBorder(1, 1);
  math_Vector aSteps(1, 1);
  //
  aLowBorder(1) = u1;
  aUppBorder(1) = u2;
  // clang-format off
  aSteps(1) =(aUppBorder(1) - aLowBorder(1)) * 0.01; // Run PSO on even distribution with 100 points.
  //
  GCPnts_DistFunction aFunc1(theCurve,  u1, u2);
  //
  double aValue;
  math_Vector aT(1,1);
  GCPnts_DistFunctionMV aFunc(aFunc1);

  math_PSO aFinder(&aFunc, aLowBorder, aUppBorder, aSteps); // Choose 32 best points from 100 above.
  // clang-format on
  aFinder.Perform(aSteps, aValue, aT);
  double d = 0.;

  double d1, d2;
  double x1 = std::max(u1, aT(1) - aSteps(1));
  bool   Ok = aFunc1.Value(x1, d1);
  if (!Ok)
  {
    return std::sqrt(-aValue);
  }
  double x2 = std::min(u2, aT(1) + aSteps(1));
  Ok        = aFunc1.Value(x2, d2);
  if (!Ok)
  {
    return std::sqrt(-aValue);
  }
  if (d1 <= aValue || d2 <= aValue)
  {
    double dmin = std::min(d1, std::min(aValue, d2));
    return std::sqrt(-dmin);
  }

  math_BrentMinimum anOptLoc(Precision::PConfusion());
  anOptLoc.Perform(aFunc1, x1, aT(1), x2);

  if (anOptLoc.IsDone())
  {
    d = -anOptLoc.Minimum();
  }
  else
  {
    d = -aValue;
  }
  return std::sqrt(d);
}

//=================================================================================================

static int crvpoints(Draw_Interpretor& di, int /*n*/, const char** a)
{
  int    i, nbp;
  double defl;

  occ::handle<Adaptor3d_Curve> aHCurve;
  occ::handle<Geom_Curve>      C = DrawTrSurf::GetCurve(a[2]);
  if (C.IsNull())
  {
    // try getting a wire
    TopoDS_Wire aWire = TopoDS::Wire(DBRep::Get(a[2], TopAbs_WIRE));
    if (aWire.IsNull())
    {
      Message::SendFail() << "cannot evaluate the argument " << a[2] << " as a curve";
      return 1;
    }
    BRepAdaptor_CompCurve aCompCurve(aWire);
    aHCurve = new BRepAdaptor_CompCurve(aCompCurve);
  }
  else
  {
    aHCurve = new GeomAdaptor_Curve(C);
  }

  defl = Draw::Atof(a[3]);

  GCPnts_QuasiUniformDeflection PntGen(*aHCurve, defl);

  if (!PntGen.IsDone())
  {
    di << "Points generation failed\n";
    return 1;
  }

  nbp = PntGen.NbPoints();
  di << "Nb points : " << nbp << "\n";

  NCollection_Array1<gp_Pnt> aPoles(1, nbp);
  NCollection_Array1<double> aKnots(1, nbp);
  NCollection_Array1<int>    aMults(1, nbp);

  for (i = 1; i <= nbp; ++i)
  {
    aPoles(i) = PntGen.Value(i);
    aKnots(i) = PntGen.Parameter(i);
    aMults(i) = 1;
  }

  aMults(1)   = 2;
  aMults(nbp) = 2;

  occ::handle<Geom_BSplineCurve>       aPnts  = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  occ::handle<DrawTrSurf_BSplineCurve> aDrCrv = new DrawTrSurf_BSplineCurve(aPnts);

  aDrCrv->ClearPoles();
  Draw_Color aKnColor(Draw_or);
  aDrCrv->SetKnotsColor(aKnColor);
  aDrCrv->SetKnotsShape(Draw_Plus);

  Draw::Set(a[1], aDrCrv);

  double dmax = 0., ufmax = 0., ulmax = 0.;
  int    imax = 0;

  // check deviation
  ComputeDeviation(*aHCurve, aPnts, dmax, ufmax, ulmax, imax);
  di << "Max defl: " << dmax << " " << ufmax << " " << ulmax << " " << imax << "\n";

  return 0;
}

//=================================================================================================

static int crvtpoints(Draw_Interpretor& di, int n, const char** a)
{
  int    i, nbp, aMinPntsNb = 2;
  double defl, angle = Precision::Angular();

  occ::handle<Adaptor3d_Curve> aHCurve;
  occ::handle<Geom_Curve>      C = DrawTrSurf::GetCurve(a[2]);
  if (C.IsNull())
  {
    // try getting a wire
    TopoDS_Wire aWire = TopoDS::Wire(DBRep::Get(a[2], TopAbs_WIRE));
    if (aWire.IsNull())
    {
      Message::SendFail() << "cannot evaluate the argument " << a[2] << " as a curve";
      return 1;
    }
    BRepAdaptor_CompCurve aCompCurve(aWire);
    aHCurve = new BRepAdaptor_CompCurve(aCompCurve);
  }
  else
  {
    aHCurve = new GeomAdaptor_Curve(C);
  }
  defl = Draw::Atof(a[3]);

  if (n > 4)
    angle = Draw::Atof(a[4]);

  if (n > 5)
    aMinPntsNb = Draw::Atoi(a[5]);

  GCPnts_TangentialDeflection PntGen(*aHCurve, angle, defl, aMinPntsNb);

  nbp = PntGen.NbPoints();
  di << "Nb points : " << nbp << "\n";

  NCollection_Array1<gp_Pnt> aPoles(1, nbp);
  NCollection_Array1<double> aKnots(1, nbp);
  NCollection_Array1<int>    aMults(1, nbp);

  for (i = 1; i <= nbp; ++i)
  {
    aPoles(i) = PntGen.Value(i);
    aKnots(i) = PntGen.Parameter(i);
    aMults(i) = 1;
  }

  aMults(1)   = 2;
  aMults(nbp) = 2;

  occ::handle<Geom_BSplineCurve>       aPnts  = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  occ::handle<DrawTrSurf_BSplineCurve> aDrCrv = new DrawTrSurf_BSplineCurve(aPnts);

  aDrCrv->ClearPoles();
  Draw_Color aKnColor(Draw_or);
  aDrCrv->SetKnotsColor(aKnColor);
  aDrCrv->SetKnotsShape(Draw_Plus);

  Draw::Set(a[1], aDrCrv);

  double dmax = 0., ufmax = 0., ulmax = 0.;
  int    imax = 0;

  // check deviation
  ComputeDeviation(*aHCurve, aPnts, dmax, ufmax, ulmax, imax);
  //
  di << "Max defl: " << dmax << " " << ufmax << " " << ulmax << " " << imax << "\n";

  return 0;
}

//=======================================================================
// function : uniformAbscissa
// purpose  : epa test (TATA-06-002 (Problem with GCPnts_UniformAbscissa class)
//=======================================================================
static int uniformAbscissa(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
    return 1;

  /*occ::handle<Geom_BSplineCurve> ellip;
  ellip = DrawTrSurf::GetBSplineCurve(a[1]);
  if (ellip.IsNull())
  {
    di << " BSpline is NULL  \n";
    return 1;
  }*/

  occ::handle<Geom_Curve> ellip;
  ellip = DrawTrSurf::GetCurve(a[1]);
  if (ellip.IsNull())
  {
    di << " Curve is NULL  \n";
    return 1;
  }

  int nocp;
  nocp = Draw::Atoi(a[2]);
  if (nocp < 2)
    return 1;

  // test nbPoints for Geom_Ellipse

  try
  {
    GeomLProp_CLProps Prop(ellip, 2, Precision::Intersection());
    Prop.SetCurve(ellip);

    GeomAdaptor_Curve GAC(ellip);
    di << "Type Of curve: " << GAC.GetType() << "\n";
    constexpr double Tol = Precision::Confusion();
    double           L;

    L = GCPnts_AbscissaPoint::Length(GAC, GAC.FirstParameter(), GAC.LastParameter(), Tol);
    di << "Ellipse length = " << L << "\n";
    double Abscissa = L / (nocp - 1);
    di << " CUR : Abscissa " << Abscissa << "\n";

    GCPnts_UniformAbscissa myAlgo(GAC, Abscissa, ellip->FirstParameter(), ellip->LastParameter());
    if (myAlgo.IsDone())
    {
      di << " CasCurve  - nbpoints " << myAlgo.NbPoints() << "\n";
      for (int i = 1; i <= myAlgo.NbPoints(); i++)
        di << i << " points = " << myAlgo.Parameter(i) << "\n";
    }
  }

  catch (Standard_Failure const&)
  {
    di << " Standard Failure  \n";
  }
  return 0;
}

//=======================================================================
// function : EllipsUniformAbscissa
// purpose  : epa test (TATA-06-002 (Problem with GCPnts_UniformAbscissa class)
//=======================================================================
static int EllipsUniformAbscissa(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 4)
    return 1;

  double R1;
  R1 = Draw::Atof(a[1]);
  double R2;
  R2 = Draw::Atof(a[2]);

  int nocp;
  nocp = Draw::Atoi(a[3]);
  if (nocp < 2)
    return 1;

  // test nbPoints for Geom_Ellipse
  occ::handle<Geom_Ellipse> ellip;

  try
  {
    gp_Pnt location;
    location = gp_Pnt(0.0, 0.0, 0.0);
    gp_Dir main_direction(gp_Dir::D::Z);

    gp_Dir x_direction(gp_Dir::D::X);
    gp_Ax2 mainaxis(location, main_direction);

    mainaxis.SetXDirection(x_direction);
    ellip = new Geom_Ellipse(mainaxis, R1, R2);

    BRepBuilderAPI_MakeEdge curve_edge(ellip);
    TopoDS_Edge             edge_curve = curve_edge.Edge();

    DBRep::Set("Ellipse", edge_curve);
  }

  catch (Standard_Failure const&)
  {
    di << " Standard Failure  \n";
  }

  try
  {
    GeomLProp_CLProps Prop(ellip, 2, Precision::Intersection());
    Prop.SetCurve(ellip);

    GeomAdaptor_Curve GAC(ellip);
    di << "Type Of curve: " << GAC.GetType() << "\n";
    constexpr double Tol = Precision::Confusion();
    double           L;

    L = GCPnts_AbscissaPoint::Length(GAC, GAC.FirstParameter(), GAC.LastParameter(), Tol);
    di << "Ellipse length = " << L << "\n";
    double Abscissa = L / (nocp - 1);
    di << " CUR : Abscissa " << Abscissa << "\n";

    GCPnts_UniformAbscissa myAlgo(GAC, Abscissa, ellip->FirstParameter(), ellip->LastParameter());
    if (myAlgo.IsDone())
    {
      di << " CasCurve  - nbpoints " << myAlgo.NbPoints() << "\n";
      for (int i = 1; i <= myAlgo.NbPoints(); i++)
        di << i << " points = " << myAlgo.Parameter(i) << "\n";
    }
  }

  catch (Standard_Failure const&)
  {
    di << " Standard Failure  \n";
  }
  return 0;
}

//=================================================================================================

static int discrCurve(Draw_Interpretor& di, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 3)
  {
    di << "Invalid number of parameters.\n";
    return 1;
  }

  occ::handle<Geom_Curve> aCurve = DrawTrSurf::GetCurve(theArgVec[2]);
  if (aCurve.IsNull())
  {
    di << "Curve is NULL.\n";
    return 1;
  }

  int  aSrcNbPnts = 0;
  bool isUniform  = false;
  for (int anArgIter = 3; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    TCollection_AsciiString anArgCase(anArg);
    anArgCase.LowerCase();
    if (anArgCase == "nbpnts")
    {
      if (++anArgIter >= theArgNb)
      {
        di << "Value for argument '" << anArg << "' is absent.\n";
        return 1;
      }

      aSrcNbPnts = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArgCase == "uniform")
    {
      if (++anArgIter >= theArgNb)
      {
        di << "Value for argument '" << anArg << "' is absent.\n";
        return 1;
      }

      isUniform = (Draw::Atoi(theArgVec[anArgIter]) == 1);
    }
    else
    {
      di << "Invalid argument '" << anArg << "'.\n";
      return 1;
    }
  }

  if (aSrcNbPnts < 2)
  {
    di << "Invalid count of points.\n";
    return 1;
  }

  if (!isUniform)
  {
    di << "Invalid type of discretization.\n";
    return 1;
  }

  GeomAdaptor_Curve      aCurveAdaptor(aCurve);
  GCPnts_UniformAbscissa aSplitter(aCurveAdaptor, aSrcNbPnts, Precision::Confusion());
  if (!aSplitter.IsDone())
  {
    di << "Error: Invalid result.\n";
    return 0;
  }

  const int aDstNbPnts = aSplitter.NbPoints();

  if (aDstNbPnts < 2)
  {
    di << "Error: Invalid result.\n";
    return 0;
  }

  NCollection_Array1<gp_Pnt> aPoles(1, aDstNbPnts);
  NCollection_Array1<double> aKnots(1, aDstNbPnts);
  NCollection_Array1<int>    aMultiplicities(1, aDstNbPnts);

  for (int aPntIter = 1; aPntIter <= aDstNbPnts; ++aPntIter)
  {
    aPoles.ChangeValue(aPntIter)          = aCurveAdaptor.Value(aSplitter.Parameter(aPntIter));
    aKnots.ChangeValue(aPntIter)          = (aPntIter - 1) / (aDstNbPnts - 1.0);
    aMultiplicities.ChangeValue(aPntIter) = 1;
  }
  aMultiplicities.ChangeValue(1)          = 2;
  aMultiplicities.ChangeValue(aDstNbPnts) = 2;

  occ::handle<Geom_BSplineCurve> aPolyline =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
  DrawTrSurf::Set(theArgVec[1], aPolyline);

  return 0;
}

//=================================================================================================

static int mypoints(Draw_Interpretor& di, int /*n*/, const char** a)
{
  int    i, nbp;
  double defl;

  occ::handle<Geom_Curve> C = DrawTrSurf::GetCurve(a[2]);
  defl                      = Draw::Atof(a[3]);
  occ::handle<Geom_BSplineCurve> aBS(occ::down_cast<Geom_BSplineCurve>(C));

  if (aBS.IsNull())
    return 1;

  int ui1 = aBS->FirstUKnotIndex();
  int ui2 = aBS->LastUKnotIndex();

  int nbsu = ui2 - ui1 + 1;
  nbsu += (nbsu - 1) * (aBS->Degree() - 1);

  NCollection_Array1<double> anUPars(1, nbsu);
  NCollection_Array1<bool>   anUFlg(1, nbsu);

  int    j, k, nbi;
  double t1, t2, dt;

  // Filling of sample parameters
  nbi = aBS->Degree();
  k   = 0;
  t1  = aBS->Knot(ui1);
  for (i = ui1 + 1; i <= ui2; ++i)
  {
    t2 = aBS->Knot(i);
    dt = (t2 - t1) / nbi;
    j  = 1;
    do
    {
      ++k;
      anUPars(k) = t1;
      anUFlg(k)  = false;
      t1 += dt;
    } while (++j <= nbi);
    t1 = t2;
  }
  ++k;
  anUPars(k) = t1;

  int l;
  defl *= defl;

  j            = 1;
  anUFlg(1)    = true;
  anUFlg(nbsu) = true;
  bool bCont   = true;
  while (j < nbsu - 1 && bCont)
  {
    t2        = anUPars(j);
    gp_Pnt p1 = aBS->Value(t2);
    for (k = j + 2; k <= nbsu; ++k)
    {
      t2               = anUPars(k);
      gp_Pnt        p2 = aBS->Value(t2);
      gce_MakeLin   MkLin(p1, p2);
      const gp_Lin& lin = MkLin.Value();
      bool          ok  = true;
      for (l = j + 1; l < k; ++l)
      {
        if (anUFlg(l))
          continue;
        gp_Pnt pp = aBS->Value(anUPars(l));
        double d  = lin.SquareDistance(pp);

        if (d <= defl)
          continue;

        ok = false;
        break;
      }

      if (!ok)
      {
        j         = k - 1;
        anUFlg(j) = true;
        break;
      }
    }

    if (k >= nbsu)
      bCont = false;
  }

  nbp = 0;
  for (i = 1; i <= nbsu; ++i)
  {
    if (anUFlg(i))
      nbp++;
  }

  NCollection_Array1<gp_Pnt> aPoles(1, nbp);
  NCollection_Array1<double> aKnots(1, nbp);
  NCollection_Array1<int>    aMults(1, nbp);
  j = 0;
  for (i = 1; i <= nbsu; ++i)
  {
    if (anUFlg(i))
    {
      ++j;
      aKnots(j) = anUPars(i);
      aMults(j) = 1;
      aPoles(j) = aBS->Value(aKnots(j));
    }
  }

  aMults(1)   = 2;
  aMults(nbp) = 2;

  occ::handle<Geom_BSplineCurve>       aPnts  = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  occ::handle<DrawTrSurf_BSplineCurve> aDrCrv = new DrawTrSurf_BSplineCurve(aPnts);

  aDrCrv->ClearPoles();
  Draw_Color aKnColor(Draw_or);
  aDrCrv->SetKnotsColor(aKnColor);
  aDrCrv->SetKnotsShape(Draw_Plus);

  Draw::Set(a[1], aDrCrv);

  double dmax = 0., ufmax = 0., ulmax = 0.;
  int    imax = 0;

  ComputeDeviation(GeomAdaptor_Curve(C), aPnts, dmax, ufmax, ulmax, imax);
  di << "Max defl: " << dmax << " " << ufmax << " " << ulmax << " " << imax << "\n";

  return 0;
}

//=================================================================================================

static int surfpoints(Draw_Interpretor& /*di*/, int /*n*/, const char** a)
{
  int    i;
  double defl;

  occ::handle<Geom_Surface> S = DrawTrSurf::GetSurface(a[2]);
  defl                        = Draw::Atof(a[3]);

  occ::handle<GeomAdaptor_Surface> AS = new GeomAdaptor_Surface(S);

  occ::handle<Adaptor3d_TopolTool> aTopTool = new Adaptor3d_TopolTool(AS);

  aTopTool->SamplePnts(defl, 10, 10);

  int                        nbpu = aTopTool->NbSamplesU();
  int                        nbpv = aTopTool->NbSamplesV();
  NCollection_Array1<double> Upars(1, nbpu), Vpars(1, nbpv);
  aTopTool->UParameters(Upars);
  aTopTool->VParameters(Vpars);

  NCollection_Array2<gp_Pnt> aPoles(1, nbpu, 1, nbpv);
  NCollection_Array1<double> anUKnots(1, nbpu);
  NCollection_Array1<double> aVKnots(1, nbpv);
  NCollection_Array1<int>    anUMults(1, nbpu);
  NCollection_Array1<int>    aVMults(1, nbpv);

  int j;
  for (i = 1; i <= nbpu; ++i)
  {
    anUKnots(i) = Upars(i);
    anUMults(i) = 1;
    for (j = 1; j <= nbpv; ++j)
    {
      aVKnots(j)   = Vpars(j);
      aVMults(j)   = 1;
      aPoles(i, j) = S->Value(anUKnots(i), aVKnots(j));
    }
  }

  anUMults(1)    = 2;
  anUMults(nbpu) = 2;
  aVMults(1)     = 2;
  aVMults(nbpv)  = 2;

  occ::handle<Geom_BSplineSurface> aPnts =
    new Geom_BSplineSurface(aPoles, anUKnots, aVKnots, anUMults, aVMults, 1, 1);
  occ::handle<DrawTrSurf_BSplineSurface> aDrSurf = new DrawTrSurf_BSplineSurface(aPnts);

  aDrSurf->ClearPoles();
  Draw_Color aKnColor(Draw_or);
  aDrSurf->SetKnotsColor(aKnColor);
  aDrSurf->SetKnotsShape(Draw_Plus);

  Draw::Set(a[1], aDrSurf);

  return 0;
}

//=================================================================================================

static int intersection(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
    return 1;

  //
  occ::handle<Geom_Curve>   GC1;
  occ::handle<Geom_Surface> GS1 = DrawTrSurf::GetSurface(a[2]);
  if (GS1.IsNull())
  {
    GC1 = DrawTrSurf::GetCurve(a[2]);
    if (GC1.IsNull())
      return 1;
  }

  //
  occ::handle<Geom_Surface> GS2 = DrawTrSurf::GetSurface(a[3]);
  if (GS2.IsNull())
    return 1;

  //
  double tol = Precision::Confusion();
  if (n == 5 || n == 9 || n == 13 || n == 17)
    tol = Draw::Atof(a[n - 1]);

  //
  occ::handle<Geom_Curve> Result;
  gp_Pnt                  Point;

  //
  if (GC1.IsNull())
  {
    GeomInt_IntSS Inters;
    //
    // Surface Surface
    if (n <= 5)
    {
      // General case
      Inters.Perform(GS1, GS2, tol, true);
    }
    else if (n == 8 || n == 9 || n == 12 || n == 13 || n == 16 || n == 17)
    {
      bool                             useStart = true, useBnd = true;
      int                              ista1 = 0, ista2 = 0, ibnd1 = 0, ibnd2 = 0;
      double                           UVsta[4];
      occ::handle<GeomAdaptor_Surface> AS1, AS2;

      //
      if (n <= 9) // user starting point
      {
        useBnd = false;
        ista1  = 4;
        ista2  = 7;
      }
      else if (n <= 13) // user bounding
      {
        useStart = false;
        ibnd1    = 4;
        ibnd2    = 11;
      }
      else // both user starting point and bounding
      {
        ista1 = 4;
        ista2 = 7;
        ibnd1 = 8;
        ibnd2 = 15;
      }

      if (useStart)
      {
        for (int i = ista1; i <= ista2; i++)
        {
          UVsta[i - ista1] = Draw::Atof(a[i]);
        }
      }

      if (useBnd)
      {
        double UVbnd[8];
        for (int i = ibnd1; i <= ibnd2; i++)
          UVbnd[i - ibnd1] = Draw::Atof(a[i]);

        AS1 = new GeomAdaptor_Surface(GS1, UVbnd[0], UVbnd[1], UVbnd[2], UVbnd[3]);
        AS2 = new GeomAdaptor_Surface(GS2, UVbnd[4], UVbnd[5], UVbnd[6], UVbnd[7]);
      }

      //
      if (useStart && !useBnd)
      {
        Inters.Perform(GS1, GS2, tol, UVsta[0], UVsta[1], UVsta[2], UVsta[3]);
      }
      else if (!useStart && useBnd)
      {
        Inters.Perform(AS1, AS2, tol);
      }
      else
      {
        Inters.Perform(AS1, AS2, tol, UVsta[0], UVsta[1], UVsta[2], UVsta[3]);
      }
    } // else if (n == 8 || n == 9 || n == 12 || n == 13 || n == 16 || n == 17)
    else
    {
      di << "incorrect number of arguments\n";
      return 1;
    }

    //
    if (!Inters.IsDone())
    {
      di << "No intersections found!\n";

      return 1;
    }

    //
    char buf[1024];
    int  i, aNbLines, aNbPoints;

    //
    aNbLines = Inters.NbLines();
    if (aNbLines >= 2)
    {
      for (i = 1; i <= aNbLines; ++i)
      {
        Sprintf(buf, "%s_%d", a[1], i);
        di << buf << " ";
        Result           = Inters.Line(i);
        const char* temp = buf;
        DrawTrSurf::Set(temp, Result);
      }
    }
    else if (aNbLines == 1)
    {
      Result = Inters.Line(1);
      Sprintf(buf, "%s", a[1]);
      di << buf << " ";
      DrawTrSurf::Set(a[1], Result);
    }

    //
    aNbPoints = Inters.NbPoints();
    for (i = 1; i <= aNbPoints; ++i)
    {
      Point = Inters.Point(i);
      Sprintf(buf, "%s_p_%d", a[1], i);
      di << buf << " ";
      const char* temp = buf;
      DrawTrSurf::Set(temp, Point);
    }
  } // if (GC1.IsNull())
  else
  {
    // Curve Surface
    GeomAPI_IntCS Inters(GC1, GS2);

    //
    if (!Inters.IsDone())
    {
      di << "No intersections found!\n";
      return 1;
    }

    int nblines  = Inters.NbSegments();
    int nbpoints = Inters.NbPoints();

    char newname[1024];

    if ((nblines + nbpoints) >= 2)
    {
      int i;
      int Compt = 1;

      if (nblines >= 1)
        std::cout << "   Lines: " << std::endl;

      for (i = 1; i <= nblines; i++, Compt++)
      {
        Sprintf(newname, "%s_%d", a[1], Compt);
        di << newname << " ";
        Result           = Inters.Segment(i);
        const char* temp = newname; // pour portage WNT
        DrawTrSurf::Set(temp, Result);
      }

      if (nbpoints >= 1)
        std::cout << "   Points: " << std::endl;

      const int imax = nblines + nbpoints;

      for (/*i = 1*/; i <= imax; i++, Compt++)
      {
        Sprintf(newname, "%s_%d", a[1], i);
        di << newname << " ";
        Point            = Inters.Point(i);
        const char* temp = newname; // pour portage WNT
        DrawTrSurf::Set(temp, Point);
      }
    }
    else if (nblines == 1)
    {
      Result = Inters.Segment(1);
      Sprintf(newname, "%s", a[1]);
      di << newname << " ";
      DrawTrSurf::Set(a[1], Result);
    }
    else if (nbpoints == 1)
    {
      Point = Inters.Point(1);
      Sprintf(newname, "%s", a[1]);
      di << newname << " ";
      DrawTrSurf::Set(a[1], Point);
    }
  }

  dout.Flush();
  return 0;
}

//=======================================================================
// function : GetCurveContinuity
// purpose  : Returns the continuity of the given curve
//=======================================================================
static int GetCurveContinuity(Draw_Interpretor& theDI, int theNArg, const char** theArgv)
{
  if (theNArg != 2)
  {
    theDI << "Use: getcurvcontinuity {curve or 2dcurve} \n";
    return 1;
  }

  char aContName[7][3] = {"C0",  // 0
                          "G1",  // 1
                          "C1",  // 2
                          "G2",  // 3
                          "C2",  // 4
                          "C3",  // 5
                          "CN"}; // 6

  occ::handle<Geom2d_Curve> GC2d;
  occ::handle<Geom_Curve>   GC3d = DrawTrSurf::GetCurve(theArgv[1]);
  if (GC3d.IsNull())
  {
    GC2d = DrawTrSurf::GetCurve2d(theArgv[1]);
    if (GC2d.IsNull())
    {
      theDI << "Argument is not a 2D or 3D curve!\n";
      return 1;
    }
    else
    {
      theDI << theArgv[1] << " has " << aContName[GC2d->Continuity()] << " continuity.\n";
    }
  }
  else
  {
    theDI << theArgv[1] << " has " << aContName[GC3d->Continuity()] << " continuity.\n";
  }

  return 0;
}

//=================================================================================================

void GeometryTest::CurveCommands(Draw_Interpretor& theCommands)
{

  static bool loaded = false;
  if (loaded)
    return;
  loaded = true;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;

  g = "GEOMETRY curves creation";

  theCommands.Add("law", "law  name degree nbknots  knot, umult  value", __FILE__, polelaw, g);

  theCommands.Add("to2d", "to2d c2dname c3d [plane (XOY)]", __FILE__, to2d, g);

  theCommands.Add("to3d", "to3d c3dname c2d [plane (XOY)]", __FILE__, to3d, g);

  theCommands.Add("gproject",
                  "gproject projectname curve surface [tolerance [maxdist]]\n"
                  "\t\t[-c continuity][-d maxdegree][-s maxsegments][-2d proj2d][-3d proj3d]\n"
                  "\t\t-c continuity  : set curve continuity (C0, C1, C2) for approximation\n"
                  "\t\t-d maxdegree   : set max possible degree of result for approximation\n"
                  "\t\t-s maxsegments : set max value of parametric intervals the projected curve "
                  "for approximation\n"
                  "\t\t-2d proj2d     : set necessity of 2d results (0 or 1)\n"
                  "\t\t-3d proj3d     : set necessity of 3d results (0 or 1)",
                  __FILE__,
                  gproject,
                  g);

  theCommands.Add("project", "project : no args to have help", __FILE__, project, g);

  theCommands.Add("projonplane",
                  "projonplane r C3d Plane [dx dy dz] [0/1]",
                  __FILE__,
                  projonplane,
                  g);

  theCommands.Add("bisec", "bisec result line/circle/point line/circle/point", __FILE__, bisec, g);

  g = "GEOMETRY Curves and Surfaces modification";

  theCommands.Add("movelaw", "movelaw name u  x  tx [ constraint = 0]", __FILE__, movelaw, g);

  g = "GEOMETRY intersections";

  theCommands.Add(
    "intersect",
    "intersect result surf1/curv1 surf2 [tolerance]\n\t\t  "
    "intersect result surf1 surf2 [u1 v1 u2 v2] [U1F U1L V1F V1L U2F U2L V2F V2L] [tolerance]",
    __FILE__,
    intersection,
    g);

  theCommands.Add("crvpoints",
                  "crvpoints result <curve or wire> deflection",
                  __FILE__,
                  crvpoints,
                  g);

  theCommands.Add("crvtpoints",
                  "crvtpoints result <curve or wire> deflection angular deflection - tangential "
                  "deflection points",
                  __FILE__,
                  crvtpoints,
                  g);

  theCommands.Add("uniformAbscissa", "uniformAbscissa Curve nbPnt", __FILE__, uniformAbscissa, g);

  theCommands.Add("uniformAbscissaEl",
                  "uniformAbscissaEl maxR minR nbPnt",
                  __FILE__,
                  EllipsUniformAbscissa,
                  g);

  theCommands.Add("discrCurve",
                  "discrCurve polyline curve params\n"
                  "Approximates a curve by a polyline (first degree B-spline).\n"
                  "nbPnts number - creates polylines with the number points\n"
                  "uniform 0 | 1 - creates polyline with equal length segments",
                  __FILE__,
                  discrCurve,
                  g);

  theCommands.Add("mypoints", "mypoints result curv deflection", __FILE__, mypoints, g);
  theCommands.Add("surfpoints", "surfoints result surf deflection", __FILE__, surfpoints, g);

  theCommands.Add(
    "getcurvcontinuity",
    "getcurvcontinuity {curve or 2dcurve}: \n\tReturns the continuity of the given curve",
    __FILE__,
    GetCurveContinuity,
    g);
}
