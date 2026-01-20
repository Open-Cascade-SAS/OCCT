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

// JPI : Commande smooth transferee dans GeomliteTest
// PMN : Ajout de la commande smooth
// JCT : Correction d'un trap dans la commande gcarc

#include <Standard_Macro.hxx>

#include <iostream>

#include <iomanip>

#include <fstream>

#include <GeometryTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_Circ2d3Tan.hxx>
#include <Geom2dGcc_Circ2dTanCen.hxx>
#include <Geom2dGcc_Lin2d2Tan.hxx>
#include <Geom2dGcc_Lin2dTanObl.hxx>
#include <Geom2dGcc.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Draw_Marker3D.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Draw_Marker2D.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <GC_MakeArcOfCircle.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif
Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color);

static int solutions(Draw_Interpretor&        di,
                                  Geom2dGcc_Circ2d2TanRad& ct3,
                                  const char*              name)
{
  char solname[200];

  Draw_Color col = DrawTrSurf_CurveColor(Draw_Color(Draw_vert));
  DrawTrSurf_CurveColor(col);

  if (ct3.IsDone())
  {
    for (int i = 1; i <= ct3.NbSolutions(); i++)
    {
      occ::handle<Geom2d_Circle> C = new Geom2d_Circle(ct3.ThisSolution(i));
      Sprintf(solname, "%s_%d", name, i);
      DrawTrSurf::Set(solname, C);
      di << solname << " ";
    }
    return 0;
  }
  else
  {
    di << "Circ2d2TanRad Not done";
    return 1;
  }
}

static int solutions(Draw_Interpretor& di, Geom2dGcc_Circ2d3Tan& ct3, const char* name)
{
  char solname[200];

  Draw_Color col = DrawTrSurf_CurveColor(Draw_Color(Draw_vert));
  DrawTrSurf_CurveColor(col);

  if (ct3.IsDone())
  {
    for (int i = 1; i <= ct3.NbSolutions(); i++)
    {
      occ::handle<Geom2d_Circle> C = new Geom2d_Circle(ct3.ThisSolution(i));
      Sprintf(solname, "%s_%d", name, i);
      DrawTrSurf::Set(solname, C);
      di << solname << " ";
    }
    return 0;
  }
  else
  {
    di << "Circ2d3Tan Not done";
    return 1;
  }
}

//=================================================================================================

static int solutions(Draw_Interpretor&       theDI,
                                  Geom2dGcc_Circ2dTanCen& theCt2,
                                  const char*             theName)
{
  char solname[200];

  Draw_Color col = DrawTrSurf_CurveColor(Draw_Color(Draw_vert));
  DrawTrSurf_CurveColor(col);

  if (theCt2.IsDone())
  {
    for (int i = 1; i <= theCt2.NbSolutions(); i++)
    {
      occ::handle<Geom2d_Circle> C = new Geom2d_Circle(theCt2.ThisSolution(i));
      Sprintf(solname, "%s_%d", theName, i);
      DrawTrSurf::Set(solname, C);
      theDI << solname << " ";
    }
    return 0;
  }
  else
  {
    theDI << "Circ2dTanCen Not done";
    return 1;
  }
}

//=================================================================================================

static int Cirtang(Draw_Interpretor& theDI,
                                int  theNArgs,
                                const char**      theArgVals)
{
  if (theNArgs < 3)
  {
    theDI << "Use: " << theArgVals[0]
          << "result [-t <Tolerance>] -c <curve> -p <point> -r <Radius>...\n";
    return 1;
  }

  double        aTol = Precision::Confusion();
  occ::handle<Geom2d_Curve> aC[3];
  gp_Pnt2d             aP[3];
  double        aRadius = -1.0;

  int aNbCurves = 0, aNbPnts = 0;

  for (int anArgID = 2; anArgID < theNArgs; anArgID++)
  {
    if (theArgVals[anArgID][0] != '-')
    {
      theDI << "Cannot interpret the argument #" << anArgID << " (" << theArgVals[anArgID] << ")\n";
      return 1;
    }
    else if (!strcmp(theArgVals[anArgID], "-c"))
    {
      if (aNbCurves >= 3)
      {
        theDI << "A lot of curves are given (not greater than 3 ones are expected)\n";
        return 1;
      }

      aC[aNbCurves] = DrawTrSurf::GetCurve2d(theArgVals[++anArgID]);
      if (aC[aNbCurves].IsNull())
      {
        theDI << "Error: " << theArgVals[anArgID] << " is not a curve\n";
        return 1;
      }

      aNbCurves++;
    }
    else if (!strcmp(theArgVals[anArgID], "-p"))
    {
      if (aNbPnts >= 3)
      {
        theDI << "A lot of points are given (not greater than 3 ones are expected)\n";
        return 1;
      }

      if (!DrawTrSurf::GetPoint2d(theArgVals[++anArgID], aP[aNbPnts]))
      {
        theDI << "Error: " << theArgVals[anArgID] << " is not a point\n";
        return 1;
      }

      aNbPnts++;
    }
    else if (!strcmp(theArgVals[anArgID], "-r"))
    {
      aRadius = Draw::Atof(theArgVals[++anArgID]);
    }
    else if (!strcmp(theArgVals[anArgID], "-t"))
    {
      aTol = Draw::Atof(theArgVals[++anArgID]);
    }
    else
    {
      theDI << "Unknown option " << theArgVals[anArgID] << "\n";
      return 1;
    }
  }

  if (aNbCurves == 3)
  {
    // C-C-C
    Geom2dGcc_Circ2d3Tan aCt3(Geom2dGcc::Unqualified(aC[0]),
                              Geom2dGcc::Unqualified(aC[1]),
                              Geom2dGcc::Unqualified(aC[2]),
                              aTol,
                              0,
                              0,
                              0);
    theDI << "Solution of type C-C-C is: ";
    return solutions(theDI, aCt3, theArgVals[1]);
  }
  else if (aNbCurves == 2)
  {
    if (aNbPnts >= 1)
    {
      // C-C-P
      Geom2dGcc_Circ2d3Tan aCt3(Geom2dGcc::Unqualified(aC[0]),
                                Geom2dGcc::Unqualified(aC[1]),
                                new Geom2d_CartesianPoint(aP[0]),
                                aTol,
                                0,
                                0);
      theDI << "Solution of type C-C-P is: ";
      return solutions(theDI, aCt3, theArgVals[1]);
    }
    else if (aRadius > 0)
    {
      // C-C-R
      Geom2dGcc_Circ2d2TanRad aCt3(Geom2dGcc::Unqualified(aC[0]),
                                   Geom2dGcc::Unqualified(aC[1]),
                                   aRadius,
                                   aTol);
      theDI << "Solution of type C-C-R is: ";
      return solutions(theDI, aCt3, theArgVals[1]);
    }

    theDI << "Error: Unsupported set of input data!\n";
    return 1;
  }
  else if (aNbCurves == 1)
  {
    if (aNbPnts == 2)
    {
      // C-P-P
      Geom2dGcc_Circ2d3Tan aCt3(Geom2dGcc::Unqualified(aC[0]),
                                new Geom2d_CartesianPoint(aP[0]),
                                new Geom2d_CartesianPoint(aP[1]),
                                aTol,
                                0);
      theDI << "Solution of type C-P-P is: ";
      return solutions(theDI, aCt3, theArgVals[1]);
    }
    else if (aNbPnts == 1)
    {
      if (aRadius > 0.0)
      {
        // C-P-R
        Geom2dGcc_Circ2d2TanRad aCt3(Geom2dGcc::Unqualified(aC[0]),
                                     new Geom2d_CartesianPoint(aP[0]),
                                     aRadius,
                                     aTol);
        theDI << "Solution of type C-P-R is: ";
        return solutions(theDI, aCt3, theArgVals[1]);
      }
      else
      {
        // C-P
        Geom2dGcc_Circ2dTanCen aCt2(Geom2dGcc::Unqualified(aC[0]),
                                    new Geom2d_CartesianPoint(aP[0]),
                                    aTol);
        theDI << "Solution of type C-P is: ";
        return solutions(theDI, aCt2, theArgVals[1]);
      }
    }

    theDI << "Error: Unsupported set of input data!\n";
    return 1;
  }
  else if (aNbPnts >= 2)
  {
    if (aNbPnts == 3)
    {
      // P-P-P
      Geom2dGcc_Circ2d3Tan aCt3(new Geom2d_CartesianPoint(aP[0]),
                                new Geom2d_CartesianPoint(aP[1]),
                                new Geom2d_CartesianPoint(aP[2]),
                                aTol);
      theDI << "Solution of type P-P-P is: ";
      return solutions(theDI, aCt3, theArgVals[1]);
    }
    else if (aRadius > 0)
    {
      // P-P-R
      Geom2dGcc_Circ2d2TanRad aCt3(new Geom2d_CartesianPoint(aP[0]),
                                   new Geom2d_CartesianPoint(aP[1]),
                                   aRadius,
                                   aTol);
      theDI << "Solution of type P-P-R is: ";
      return solutions(theDI, aCt3, theArgVals[1]);
    }

    theDI << "Error: Unsupported set of input data!\n";
    return 1;
  }

  theDI << "Error: Unsupported set of input data!\n";
  return 1;
}

//=================================================================================================

static int lintang(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<Geom2d_Curve> C1 = DrawTrSurf::GetCurve2d(a[2]);
  occ::handle<Geom2d_Curve> C2 = DrawTrSurf::GetCurve2d(a[3]);

  char solname[200];

  if (C1.IsNull() || C2.IsNull())
    return 1;

  Draw_Color col = DrawTrSurf_CurveColor(Draw_Color(Draw_vert));

  if (n >= 5)
  {
    occ::handle<Geom2d_Line> L = occ::down_cast<Geom2d_Line>(C2);
    if (L.IsNull())
    {
      di << "Second argument must be a line";
      return 1;
    }
    double         ang = Draw::Atof(a[4]) * (M_PI / 180.0);
    Geom2dGcc_Lin2dTanObl ct3(Geom2dGcc::Unqualified(C1),
                              L->Lin2d(),
                              Precision::Angular(),
                              (C1->FirstParameter() + C1->LastParameter()) / 2.,
                              ang);
    if (ct3.IsDone())
    {
      for (int i = 1; i <= ct3.NbSolutions(); i++)
      {
        occ::handle<Geom2d_Line> LS = new Geom2d_Line(ct3.ThisSolution(i));
        Sprintf(solname, "%s_%d", a[1], i);
        char* temp = solname; // pour portage WNT
        DrawTrSurf::Set(temp, LS);
        di << solname << " ";
      }
    }
    else
      di << "Lin2dTanObl Not done\n";
  }
  else
  {
    Geom2dGcc_Lin2d2Tan ct3(Geom2dGcc::Unqualified(C1),
                            Geom2dGcc::Unqualified(C2),
                            Precision::Angular(),
                            (C1->FirstParameter() + C1->LastParameter()) / 2.,
                            (C2->FirstParameter() + C2->LastParameter()) / 2.);
    if (ct3.IsDone())
    {
      for (int i = 1; i <= ct3.NbSolutions(); i++)
      {
        occ::handle<Geom2d_Line> LS = new Geom2d_Line(ct3.ThisSolution(i));
        Sprintf(solname, "%s_%d", a[1], i);
        char* temp = solname; // pour portage WNT
        DrawTrSurf::Set(temp, LS);
        di << solname << " ";
      }
    }
    else
      di << "Lin2d2Tan Not done\n";
  }

  DrawTrSurf_CurveColor(col);

  return 0;
}

//==================================================================================
static int interpol(Draw_Interpretor& di, int n, const char** a)
//==================================================================================
{
  if (n == 1)
  {
    di << "give a name to your curve !\n";
    return 0;
  }
  if (n == 2)
  {
    int id, XX, YY, b, i, j;
    di << "Pick points \n";
    dout.Select(id, XX, YY, b);
    double zoom = dout.Zoom(id);
    if (b != 1)
      return 0;
    if (id < 0)
      return 0;
    gp_Pnt           P;
    gp_Pnt2d         P2d;
    bool newcurve;

    if (dout.Is3D(id))
    {
      occ::handle<Draw_Marker3D>       mark;
      occ::handle<NCollection_HArray1<gp_Pnt>> Points = new NCollection_HArray1<gp_Pnt>(1, 1);
      P.SetCoord((double)XX / zoom, (double)YY / zoom, 0.0);
      Points->SetValue(1, P);
      occ::handle<NCollection_HArray1<gp_Pnt>> ThePoints = new NCollection_HArray1<gp_Pnt>(1, 2);
      ThePoints->SetValue(1, P);
      mark = new Draw_Marker3D(Points->Value(1), Draw_X, Draw_vert);
      dout << mark;
      dout.Flush();
      occ::handle<Geom_BSplineCurve> C;
      i = 1;

      while (b != 3)
      {
        dout.Select(id, XX, YY, b, false);
        P.SetCoord((double)XX / zoom, (double)YY / zoom, 0.0);
        ThePoints->SetValue(i + 1, P);
        newcurve = false;
        if (!(ThePoints->Value(i)).IsEqual(ThePoints->Value(i + 1), 1.e-06))
        {
          if (b == 1)
          {
            i++;
            mark = new Draw_Marker3D(ThePoints->Value(i), Draw_X, Draw_vert);
            dout << mark;
            dout.Flush();
            Points = new NCollection_HArray1<gp_Pnt>(ThePoints->Lower(), ThePoints->Upper());
            Points->ChangeArray1() = ThePoints->Array1();
            newcurve               = true;
          }
          GeomAPI_Interpolate anInterpolator(ThePoints, false, 1.0e-5);
          anInterpolator.Perform();
          if (anInterpolator.IsDone())
          {
            C                                  = anInterpolator.Curve();
            occ::handle<DrawTrSurf_BSplineCurve> DC = new DrawTrSurf_BSplineCurve(C);
            DC->ClearPoles();
            DC->ClearKnots();
            Draw::Set(a[1], DC);
            dout.RepaintView(id);
          }
          if (newcurve)
          {
            ThePoints = new NCollection_HArray1<gp_Pnt>(1, i + 1);
            for (j = 1; j <= i; j++)
              ThePoints->SetValue(j, Points->Value(j));
          }
        }
      }
      GeomAPI_Interpolate anInterpolator(ThePoints, false, 1.0e-5);
      anInterpolator.Perform();
      if (anInterpolator.IsDone())
      {
        C = anInterpolator.Curve();
        DrawTrSurf::Set(a[1], C);
        dout.RepaintView(id);
      }
    }
    else
    {
      occ::handle<Draw_Marker2D>         mark;
      occ::handle<NCollection_HArray1<gp_Pnt2d>> Points = new NCollection_HArray1<gp_Pnt2d>(1, 1);
      P2d.SetCoord((double)XX / zoom, (double)YY / zoom);
      Points->SetValue(1, P2d);
      occ::handle<NCollection_HArray1<gp_Pnt2d>> ThePoints = new NCollection_HArray1<gp_Pnt2d>(1, 2);
      ThePoints->SetValue(1, P2d);
      mark = new Draw_Marker2D(P2d, Draw_X, Draw_vert);
      dout << mark;
      dout.Flush();
      occ::handle<Geom2d_BSplineCurve> C;
      i = 1;

      while (b != 3)
      {
        dout.Select(id, XX, YY, b, false);
        P2d.SetCoord((double)XX / zoom, (double)YY / zoom);
        ThePoints->SetValue(i + 1, P2d);
        newcurve = false;
        if (!(ThePoints->Value(i)).IsEqual(ThePoints->Value(i + 1), 1.e-06))
        {
          if (b == 1)
          {
            i++;
            mark = new Draw_Marker2D(P2d, Draw_X, Draw_vert);
            dout << mark;
            dout.Flush();
            Points = new NCollection_HArray1<gp_Pnt2d>(ThePoints->Lower(), ThePoints->Upper());
            Points->ChangeArray1() = ThePoints->Array1();
            newcurve               = true;
          }
          Geom2dAPI_Interpolate a2dInterpolator(ThePoints, false, 1.0e-5);
          a2dInterpolator.Perform();
          if (a2dInterpolator.IsDone())
          {
            C = a2dInterpolator.Curve();

            occ::handle<DrawTrSurf_BSplineCurve2d> DC = new DrawTrSurf_BSplineCurve2d(C);
            DC->ClearPoles();
            DC->ClearKnots();
            Draw::Set(a[1], DC);
            dout.RepaintView(id);
          }

          if (newcurve)
          {
            ThePoints = new NCollection_HArray1<gp_Pnt2d>(1, i + 1);
            for (j = 1; j <= i; j++)
              ThePoints->SetValue(j, Points->Value(j));
          }
        }
      }
      Geom2dAPI_Interpolate a2dInterpolator(Points, false, 1.0e-5);
      a2dInterpolator.Perform();
      if (a2dInterpolator.IsDone())
      {
        C = a2dInterpolator.Curve();

        DrawTrSurf::Set(a[1], C);
        dout.RepaintView(id);
      }
    }
  }
  else if (n == 3)
  {
    // lecture du fichier.
    // nbpoints, 2d ou 3d, puis valeurs.
    const char*   nomfic = a[2];
    std::ifstream iFile(nomfic, std::ios::in);
    if (!iFile)
      return 1;
    int nbp, i;
    double    x, y, z;
    iFile >> nbp;
    constexpr size_t aBufferSize = 3;
    char             dimen[aBufferSize];
    iFile.width(aBufferSize);
    iFile >> dimen;
    if (!strcmp(dimen, "3d"))
    {
      occ::handle<NCollection_HArray1<gp_Pnt>> Point = new NCollection_HArray1<gp_Pnt>(1, nbp);
      for (i = 1; i <= nbp; i++)
      {
        iFile >> x >> y >> z;
        Point->SetValue(i, gp_Pnt(x, y, z));
      }
      GeomAPI_Interpolate anInterpolator(Point, false, 1.0e-5);
      anInterpolator.Perform();
      if (anInterpolator.IsDone())
      {
        const occ::handle<Geom_BSplineCurve>& C = anInterpolator.Curve();
        DrawTrSurf::Set(a[1], C);
      }
    }
    else if (!strcmp(dimen, "2d"))
    {
      occ::handle<NCollection_HArray1<gp_Pnt2d>> PointPtr = new NCollection_HArray1<gp_Pnt2d>(1, nbp);
      for (i = 1; i <= nbp; i++)
      {
        iFile >> x >> y;
        PointPtr->SetValue(i, gp_Pnt2d(x, y));
      }
      Geom2dAPI_Interpolate a2dInterpolator(PointPtr, false, 1.0e-5);
      a2dInterpolator.Perform();
      if (a2dInterpolator.IsDone())
      {
        const occ::handle<Geom2d_BSplineCurve>& C = a2dInterpolator.Curve();
        DrawTrSurf::Set(a[1], C);
      }
    }
  }
  return 0;
}

static int tanginterpol(Draw_Interpretor& di, int n, const char** a)
{

  if (n < 4)
    return 1;

  int ii, jj,
    //    num_knots,
    //    degree,
    num_tangents, num_read, num_start, num_parameters;

  double
    //    delta,
    tolerance;
  //    parameter ;

  bool periodic_flag = false;
  gp_Pnt           a_point;
  gp_Vec           a_vector;
  tolerance = 1.0e-5;

  occ::handle<Geom_BSplineCurve> NewCurvePtr;

  num_read = 2;
  if (strcmp(a[num_read], "p") == 0)
  {
    periodic_flag = true;
    num_read += 1;
  }
  num_parameters = Draw::Atoi(a[num_read]);

  if (num_parameters < 2)
  {
    num_parameters = 2;
  }
  if (n < num_parameters * 3 + num_read)
  {
    return 1;
  }
  occ::handle<NCollection_HArray1<gp_Pnt>> PointsArrayPtr = new NCollection_HArray1<gp_Pnt>(1, num_parameters);

  num_tangents = ((n - num_read) / 3) - num_parameters;
  num_tangents = std::max(0, num_tangents);
  num_tangents = std::min(num_parameters, num_tangents);
  ii           = 1;
  num_start    = num_read;
  num_read += 1;
  while (num_read <= num_parameters * 3 + num_start)
  {
    for (jj = 1; jj <= 3; jj++)
    {
      a_point.SetCoord(jj, Draw::Atof(a[num_read]));
      num_read += 1;
    }
    PointsArrayPtr->SetValue(ii, a_point);
    ii += 1;
  }
  GeomAPI_Interpolate anInterpolator(PointsArrayPtr, periodic_flag, tolerance);

  if (num_tangents > 0)
  {
    NCollection_Array1<gp_Vec>               TangentsArray(1, num_parameters);
    occ::handle<NCollection_HArray1<bool>> TangentFlagsPtr =
      new NCollection_HArray1<bool>(1, num_parameters);

    for (ii = 1; ii <= num_tangents; ii++)
    {
      TangentFlagsPtr->SetValue(ii, true);
    }
    for (ii = num_tangents + 1; ii <= num_parameters; ii++)
    {
      TangentFlagsPtr->SetValue(ii, false);
    }
    ii = 1;
    while (ii <= num_tangents)
    {
      for (jj = 1; jj <= 3; jj++)
      {
        a_vector.SetCoord(jj, Draw::Atof(a[num_read]));
        num_read += 1;
      }
      TangentsArray.SetValue(ii, a_vector);
      ii += 1;
    }

    anInterpolator.Load(TangentsArray, TangentFlagsPtr);
  }
  anInterpolator.Perform();
  if (anInterpolator.IsDone())
  {
    NewCurvePtr = anInterpolator.Curve();

    DrawTrSurf::Set(a[1], NewCurvePtr);
    di << a[2] << " ";
  }
  return 0;
}

//==================================================================================
static int gcarc(Draw_Interpretor& di, int n, const char** a)
//==================================================================================
{
  if (n >= 5)
  {
    gp_Pnt P1, P2, P3, P4;
    if (!strcmp(a[2], "seg"))
    {
      if (DrawTrSurf::GetPoint(a[3], P1))
      {
        if (DrawTrSurf::GetPoint(a[4], P2))
        {
          occ::handle<Geom_Curve> theline(GC_MakeSegment(P1, P2).Value());
          DrawTrSurf::Set(a[1], theline);
          return 1;
        }
      }
    }
    else if (!strcmp(a[2], "cir"))
    {
      if (DrawTrSurf::GetPoint(a[3], P1))
      {
        if (DrawTrSurf::GetPoint(a[4], P2))
        {
          if (DrawTrSurf::GetPoint(a[5], P3))
          {
            //	    if (DrawTrSurf::GetPoint(a[6], P4)) {
            if (n > 6)
            {
              DrawTrSurf::GetPoint(a[6], P4);
              gp_Vec             V1 = gp_Vec(P2, P3);
              occ::handle<Geom_Curve> thearc(GC_MakeArcOfCircle(P1, V1, P4).Value());
              DrawTrSurf::Set(a[1], thearc);
              return 1;
            }
            else
            {
              occ::handle<Geom_Curve> thearc(GC_MakeArcOfCircle(P1, P2, P3).Value());
              DrawTrSurf::Set(a[1], thearc);
              return 1;
            }
          }
        }
      }
    }
  }
  di << "give a name for arc and the type seg or cir then\n";
  di << "give passing points p1 p2 for seg    p1 p2 p3 or p1 p2 p3 p4 for cir (p2 p3 is a "
        "tgtvec)!\n";
  return 0;
}

//=================================================================================================

void GeometryTest::ConstraintCommands(Draw_Interpretor& theCommands)
{

  static bool loaded = false;
  if (loaded)
    return;
  loaded = true;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;
  // constrained constructs
  g = "GEOMETRY Constraints";

  theCommands.Add("cirtang",
                  "cirtang cname [-t <Tolerance>] -c <curve> -p <point> -r <Radius>...",
                  __FILE__,
                  Cirtang,
                  g);

  theCommands.Add("lintan", "lintan lname curve1 curve2 [angle]", __FILE__, lintang, g);

  theCommands.Add("interpol", "interpol cname [fic]", __FILE__, interpol, g);
  theCommands.Add("tanginterpol",
                  "tanginterpol curve [p] num_points points [tangents] modifier  p = periodic",
                  __FILE__,
                  tanginterpol,
                  g);

  theCommands.Add("gcarc", "gcarc name seg/cir p1 p2 p3 p4", __FILE__, gcarc, g);
}
