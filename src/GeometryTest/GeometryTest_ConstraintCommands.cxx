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

#include <Standard_Stream.hxx>

#include <GeometryTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_Circ2d3Tan.hxx>
#include <Geom2dGcc_Circ2d2TanOn.hxx>
#include <Geom2dGcc_Circ2dTanOnRad.hxx>
#include <Geom2dGcc_Circ2dTanCen.hxx>
#include <Geom2dGcc_Lin2d2Tan.hxx>
#include <Geom2dGcc_Lin2dTanObl.hxx>
#include <Geom2dGcc.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Precision.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Draw_Marker3D.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Draw_Marker2D.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <GC_MakeSegment.hxx>
#include <GC_MakeArcOfCircle.hxx>

#include <stdio.h>
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif
Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color);


static Standard_Integer solutions(Draw_Interpretor& di,
                                  Geom2dGcc_Circ2d2TanRad& ct3, const char* name) 
{
  char solname[200];

  Draw_Color col = DrawTrSurf_CurveColor(Draw_Color(Draw_vert));
  DrawTrSurf_CurveColor(col);

  if (ct3.IsDone()) {
    for (Standard_Integer i = 1 ; i <= ct3.NbSolutions() ; i++) {
      Handle(Geom2d_Circle) C = new Geom2d_Circle(ct3.ThisSolution(i));
      Sprintf(solname,"%s_%d",name,i);
      char* temp = solname; // pour portage WNT
      DrawTrSurf::Set(temp,C);
      di << solname << " ";
    }
    return 0;
  }
  else {
    di << "Circ2d2TanRad Not done";
    return 1;
  }
}

static Standard_Integer solutions(Draw_Interpretor& di,
                                  Geom2dGcc_Circ2d3Tan& ct3, const char* name) 
{
  char solname[200];

  Draw_Color col = DrawTrSurf_CurveColor(Draw_Color(Draw_vert));
  DrawTrSurf_CurveColor(col);

  if (ct3.IsDone()) {
    for (Standard_Integer i = 1 ; i <= ct3.NbSolutions() ; i++) {
      Handle(Geom2d_Circle) C = new Geom2d_Circle(ct3.ThisSolution(i));
      Sprintf(solname,"%s_%d",name,i);
      char* temp = solname; // pour portage WNT
      DrawTrSurf::Set(temp,C);
      di << solname << " ";
    }
    return 0;
  }
  else {
    di << "Circ2d3Tan Not done";
    return 1;
  }
}

//=======================================================================
//function : cirtang
//purpose  : 
//=======================================================================

static Standard_Integer cirtang (Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Handle(Geom2d_Curve) C1 = DrawTrSurf::GetCurve2d(a[2]);
  Handle(Geom2d_Curve) C2 = DrawTrSurf::GetCurve2d(a[3]);
  Handle(Geom2d_Curve) C3 = DrawTrSurf::GetCurve2d(a[4]);
  gp_Pnt2d P1,P2,P3;
  Standard_Boolean ip1 = DrawTrSurf::GetPoint2d(a[2],P1);
  Standard_Boolean ip2 = DrawTrSurf::GetPoint2d(a[3],P2);
  Standard_Boolean ip3 = DrawTrSurf::GetPoint2d(a[4],P3);

  Standard_Real tol = Precision::Confusion();
  if (n > 5) tol = Draw::Atof(a[5]);


  if (!C1.IsNull()) {
    // C-...
    if (!C2.IsNull()) {
      // C-C-...
      if (!C3.IsNull()) {
        // C-C-C
        Geom2dGcc_Circ2d3Tan ct3(Geom2dGcc::Unqualified(C1),
          Geom2dGcc::Unqualified(C2),
          Geom2dGcc::Unqualified(C3),
          tol,0,0,0);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) {
        // C-C-P
        Geom2dGcc_Circ2d3Tan ct3(Geom2dGcc::Unqualified(C1),
          Geom2dGcc::Unqualified(C2),
          new Geom2d_CartesianPoint(P3),
          tol,0,0);
        return solutions(di,ct3,a[1]);
      }

      else {
        // C-C-R
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C1),
          Geom2dGcc::Unqualified(C2),
          Draw::Atof(a[4]),tol);
        return solutions(di,ct3,a[1]);
      }
    }
    else if (ip2) {
      // C-P-..
      if (!C3.IsNull()) {
        // C-P-C
        Geom2dGcc_Circ2d3Tan ct3(Geom2dGcc::Unqualified(C1),
          Geom2dGcc::Unqualified(C3),
          new Geom2d_CartesianPoint(P2),
          tol,0,0);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) {
        // C-P-P
        Geom2dGcc_Circ2d3Tan ct3(Geom2dGcc::Unqualified(C1),
          new Geom2d_CartesianPoint(P2),
          new Geom2d_CartesianPoint(P3),
          tol,0);
        return solutions(di,ct3,a[1]);
      }

      else {
        // C-P-R
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C1),
          new Geom2d_CartesianPoint(P2),
          Draw::Atof(a[4]),tol);
        return solutions(di,ct3,a[1]);
      }
    }

    else {
      // C-R-..
      if (!C3.IsNull()) {
        // C-R-C
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C1),
          Geom2dGcc::Unqualified(C3),
          Draw::Atof(a[3]),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) {
        // C-R-P
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C1),
          new Geom2d_CartesianPoint(P3),
          Draw::Atof(a[3]),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else {
        // C-R-R
        di << "Curve, radius, radius ???\n";
        return 1;
      }
    }
  }

  else if (ip1) {
    // P-...
    if (!C2.IsNull()) {
      // P-C-...
      if (!C3.IsNull()) {
        // P-C-C
        Geom2dGcc_Circ2d3Tan ct3(Geom2dGcc::Unqualified(C2),
          Geom2dGcc::Unqualified(C3),
          new Geom2d_CartesianPoint(P1),
          tol,0,0);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) {
        // P-C-P
        Geom2dGcc_Circ2d3Tan ct3(Geom2dGcc::Unqualified(C2),
          new Geom2d_CartesianPoint(P1),
          new Geom2d_CartesianPoint(P3),
          tol,0);
        return solutions(di,ct3,a[1]);
      }

      else {
        // P-C-R
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C2),
          new Geom2d_CartesianPoint(P1),
          Draw::Atof(a[4]),tol);
        return solutions(di,ct3,a[1]);
      }
    }
    else if (ip2) {
      // P-P-..
      if (!C3.IsNull()) {
        // P-P-C
        Geom2dGcc_Circ2d3Tan ct3(Geom2dGcc::Unqualified(C3),
          new Geom2d_CartesianPoint(P1),
          new Geom2d_CartesianPoint(P2),
          tol,0);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) {
        // P-P-P
        Geom2dGcc_Circ2d3Tan ct3(new Geom2d_CartesianPoint(P1),
          new Geom2d_CartesianPoint(P2),
          new Geom2d_CartesianPoint(P3),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else {
        // P-P-R
        Geom2dGcc_Circ2d2TanRad ct3(new Geom2d_CartesianPoint(P1),
          new Geom2d_CartesianPoint(P2),
          Draw::Atof(a[4]),tol);
        return solutions(di,ct3,a[1]);
      }
    }

    else {
      // P-R-..
      if (!C3.IsNull()) {
        // P-R-C
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C3),
          new Geom2d_CartesianPoint(P1),
          Draw::Atof(a[3]),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) {
        // P-R-P
        Geom2dGcc_Circ2d2TanRad ct3(new Geom2d_CartesianPoint(P1),
          new Geom2d_CartesianPoint(P3),
          Draw::Atof(a[3]),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else {
        // P-R-R
        di << "Point, radius, radius ???\n";
        return 1;
      }
    }
  }

  else {
    // R-...
    if (!C2.IsNull()) {
      // R-C-...
      if (!C3.IsNull()) {
        // R-C-C
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C2),
          Geom2dGcc::Unqualified(C3),
          Draw::Atof(a[2]),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) {
        // R-C-P
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C2),
          new Geom2d_CartesianPoint(P3),
          Draw::Atof(a[2]),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else {
        // R-C-R
        di << "Radius - Curve - Radius ??\n";
        return 1;
      }
    }
    else if (ip2) {
      // R-P-..
      if (!C3.IsNull()) {
        // R-P-C
        Geom2dGcc_Circ2d2TanRad ct3(Geom2dGcc::Unqualified(C3),
          new Geom2d_CartesianPoint(P2),
          Draw::Atof(a[2]),
          tol);
        return solutions(di,ct3,a[1]);
      }

      else if (ip3) 
      {
        // R-P-P
        Geom2dGcc_Circ2d2TanRad ct3(new Geom2d_CartesianPoint(P2),
          new Geom2d_CartesianPoint(P3),
          Draw::Atof(a[2]),
          tol);
        return solutions(di,ct3,a[1]);
      }      
      else {
        // R-P-R
        di << "Radius - Point - Radius ??\n";
        return 1;
      }
    }
    else {
      // R-R-..
      di << "radius, radius ???\n";
      return 1;
    }
  }
}


//=======================================================================
//function : lintang
//purpose  : 
//=======================================================================

static Standard_Integer lintang (Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  Handle(Geom2d_Curve) C1 = DrawTrSurf::GetCurve2d(a[2]);
  Handle(Geom2d_Curve) C2 = DrawTrSurf::GetCurve2d(a[3]);

  char solname[200];

  if (C1.IsNull() || C2.IsNull())
    return 1;

  Draw_Color col = DrawTrSurf_CurveColor(Draw_Color(Draw_vert));

  if (n >= 5) {
    Handle(Geom2d_Line) L = Handle(Geom2d_Line)::DownCast(C2);
    if (L.IsNull()) {
      di << "Second argument must be a line";
      return 1;
    }
    Standard_Real ang = Draw::Atof(a[4]) * (M_PI / 180.0);
    Geom2dGcc_Lin2dTanObl ct3(Geom2dGcc::Unqualified(C1),
      L->Lin2d(),
      Precision::Angular(),
      (C1->FirstParameter()+C1->LastParameter())/2.,
      ang);
    if (ct3.IsDone()) {
      for (Standard_Integer i = 1 ; i <= ct3.NbSolutions() ; i++) {
        Handle(Geom2d_Line) LS = new Geom2d_Line(ct3.ThisSolution(i));
        Sprintf(solname,"%s_%d",a[1],i);
        char* temp = solname; // pour portage WNT
        DrawTrSurf::Set(temp,LS);
        di << solname << " ";
      }
    }
    else
      di << "Lin2dTanObl Not done\n";
  }
  else {
    Geom2dGcc_Lin2d2Tan ct3(Geom2dGcc::Unqualified(C1),
      Geom2dGcc::Unqualified(C2),
      Precision::Angular(),
      (C1->FirstParameter()+C1->LastParameter())/2.,
      (C2->FirstParameter()+C2->LastParameter())/2.);
    if (ct3.IsDone()) {
      for (Standard_Integer i = 1 ; i <= ct3.NbSolutions() ; i++) {
        Handle(Geom2d_Line) LS = new Geom2d_Line(ct3.ThisSolution(i));
        Sprintf(solname,"%s_%d",a[1],i);
        char* temp = solname; // pour portage WNT
        DrawTrSurf::Set(temp,LS);
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
static Standard_Integer interpol (Draw_Interpretor& di,Standard_Integer n, const char** a)
//==================================================================================
{
  if (n == 1) {
    di <<"give a name to your curve !\n";
    return 0;
  }
  if (n == 2) {
    Standard_Integer id,XX,YY,b, i, j;
    di << "Pick points \n";
    dout.Select(id, XX, YY, b);
    Standard_Real zoom = dout.Zoom(id);
    if (b != 1) return 0;
    if (id < 0) return 0;
    gp_Pnt P;
    gp_Pnt2d P2d;
    Standard_Boolean newcurve;

    if (dout.Is3D(id)) {
      Handle(Draw_Marker3D) mark;
      Handle(TColgp_HArray1OfPnt) Points = new TColgp_HArray1OfPnt(1, 1);
      P.SetCoord((Standard_Real)XX/zoom,(Standard_Real)YY/zoom, 0.0);
      Points->SetValue(1 , P);
      Handle(TColgp_HArray1OfPnt) ThePoints = new TColgp_HArray1OfPnt(1, 2);
      ThePoints->SetValue(1 , P);
      mark = new Draw_Marker3D(Points->Value(1), Draw_X, Draw_vert);
      dout << mark;
      dout.Flush();
      Handle(Geom_BSplineCurve) C;
      i = 1;

      while (b != 3) {
        dout.Select(id,XX,YY,b, Standard_False);
        P.SetCoord((Standard_Real)XX/zoom,(Standard_Real)YY/zoom, 0.0);
        ThePoints->SetValue(i+1, P);
        newcurve = Standard_False;
        if (!(ThePoints->Value(i)).IsEqual(ThePoints->Value(i+1), 1.e-06)) {
          if (b == 1) { 
            i++;
            mark = new Draw_Marker3D(ThePoints->Value(i), Draw_X, Draw_vert);
            dout << mark;
            dout.Flush();
            Points = 
              new TColgp_HArray1OfPnt(ThePoints->Lower(),ThePoints->Upper());
            Points->ChangeArray1() = ThePoints->Array1();
            newcurve = Standard_True;
          }
          GeomAPI_Interpolate anInterpolator(ThePoints,
            Standard_False,
            1.0e-5);
          anInterpolator.Perform() ;
          if (anInterpolator.IsDone()) {
            C = anInterpolator.Curve() ;
            Handle(DrawTrSurf_BSplineCurve) 
              DC = new DrawTrSurf_BSplineCurve(C);
            DC->ClearPoles();
            DC->ClearKnots();
            Draw::Set(a[1], DC);
            dout.RepaintView(id);
          }
          if (newcurve) {
            ThePoints = new TColgp_HArray1OfPnt(1, i+1);
            for (j = 1; j <= i; j++) ThePoints->SetValue(j, Points->Value(j));
          }
        }
      }
      GeomAPI_Interpolate anInterpolator(ThePoints,
        Standard_False,
        1.0e-5);
      anInterpolator.Perform() ;
      if (anInterpolator.IsDone()) {
        C = anInterpolator.Curve() ;
        DrawTrSurf::Set(a[1], C);
        dout.RepaintView(id);
      }      
    }
    else {
      Handle(Draw_Marker2D) mark;
      Handle(TColgp_HArray1OfPnt2d) Points = new TColgp_HArray1OfPnt2d(1, 1);
      P2d.SetCoord((Standard_Real)XX/zoom,(Standard_Real)YY/zoom);
      Points->SetValue(1 , P2d);
      Handle(TColgp_HArray1OfPnt2d) ThePoints = new TColgp_HArray1OfPnt2d(1, 2);
      ThePoints->SetValue(1, P2d);
      mark = new Draw_Marker2D(P2d, Draw_X, Draw_vert);
      dout << mark;
      dout.Flush();
      Handle(Geom2d_BSplineCurve) C;
      i = 1;

      while (b != 3) {
        dout.Select(id,XX,YY,b, Standard_False);
        P2d.SetCoord((Standard_Real)XX/zoom,(Standard_Real)YY/zoom);
        ThePoints->SetValue(i+1, P2d);
        newcurve = Standard_False;
        if (!(ThePoints->Value(i)).IsEqual(ThePoints->Value(i+1), 1.e-06)) {
          if (b == 1) { 
            i++;
            mark = new Draw_Marker2D(P2d, Draw_X, Draw_vert);
            dout << mark;
            dout.Flush();
            Points = 
              new TColgp_HArray1OfPnt2d(ThePoints->Lower(),ThePoints->Upper());
            Points->ChangeArray1() = ThePoints->Array1();
            newcurve = Standard_True;
          }
          Geom2dAPI_Interpolate    a2dInterpolator(ThePoints,
            Standard_False,
            1.0e-5) ;
          a2dInterpolator.Perform() ;
          if (a2dInterpolator.IsDone()) { 
            C = a2dInterpolator.Curve() ;

            Handle(DrawTrSurf_BSplineCurve2d) 
              DC = new DrawTrSurf_BSplineCurve2d(C);
            DC->ClearPoles();
            DC->ClearKnots();
            Draw::Set(a[1], DC);
            dout.RepaintView(id);
          }

          if (newcurve) {
            ThePoints = new TColgp_HArray1OfPnt2d(1, i+1);
            for (j = 1; j <= i; j++) ThePoints->SetValue(j, Points->Value(j));
          }
        }
      }
      Geom2dAPI_Interpolate    a2dInterpolator(Points,
        Standard_False,
        1.0e-5) ;
      a2dInterpolator.Perform() ;
      if (a2dInterpolator.IsDone()) { 
        C = a2dInterpolator.Curve() ;

        DrawTrSurf::Set(a[1], C);
        dout.RepaintView(id); 
      }

    }
  }
  else if (n == 3) {
    // lecture du fichier.
    // nbpoints, 2d ou 3d, puis valeurs.
    const char* nomfic = a[2];
    ifstream iFile(nomfic, ios::in);
    if (!iFile) return 1;
    Standard_Integer nbp, i;
    Standard_Real x, y, z;
    iFile >> nbp;
    char dimen[3];
    iFile >> dimen;
    if (!strcmp(dimen,"3d")) {
      Handle(TColgp_HArray1OfPnt) Point =
        new TColgp_HArray1OfPnt(1, nbp);
      for (i = 1; i <= nbp; i++) {
        iFile >> x >> y >> z;
        Point->SetValue(i, gp_Pnt(x, y, z));
      }
      GeomAPI_Interpolate  anInterpolator(Point,
        Standard_False,
        1.0e-5) ;
      anInterpolator.Perform() ;
      if (anInterpolator.IsDone()) { 
        Handle(Geom_BSplineCurve) C = 
          anInterpolator.Curve();
        DrawTrSurf::Set(a[1], C);
      }
    }
    else if (!strcmp(dimen,"2d")) {
      Handle(TColgp_HArray1OfPnt2d)  PointPtr = 
        new TColgp_HArray1OfPnt2d(1, nbp);
      for (i = 1; i <= nbp; i++) {
        iFile >> x >> y;
        PointPtr->SetValue(i, gp_Pnt2d(x, y));
      }
      Geom2dAPI_Interpolate   a2dInterpolator(PointPtr,
        Standard_False,
        1.0e-5);
      a2dInterpolator.Perform() ;
      if (a2dInterpolator.IsDone()) {
        Handle(Geom2d_BSplineCurve) C = a2dInterpolator.Curve() ;
        DrawTrSurf::Set(a[1], C);
      }
    }
  }
  return 0;
}

static Standard_Integer tanginterpol (Draw_Interpretor& di,
                                      Standard_Integer n, 
                                      const char** a)
{


  if (n < 4)
    return 1;

  Standard_Integer 
    ii,
    jj,
    //    num_knots,
    //    degree,
    num_tangents,
    num_read,
    num_start,
    num_parameters ;


  Standard_Real 
    //    delta,
    tolerance;
  //    parameter ;

  Standard_Boolean periodic_flag = Standard_False ;
  gp_Pnt a_point ;
  gp_Vec a_vector ;
  tolerance = 1.0e-5 ;




  Handle(Geom_BSplineCurve) NewCurvePtr ;




  num_read = 2 ;
  if (strcmp(a[num_read],"p") == 0) {
    periodic_flag = Standard_True ;
    num_read += 1 ;
  }
  num_parameters = Draw::Atoi(a[num_read]) ;

  if (num_parameters < 2) {
    num_parameters = 2 ;
  }
  if ( n <  num_parameters * 3 + num_read) {
    return 1 ;
  }
  Handle(TColgp_HArray1OfPnt)   PointsArrayPtr=
    new TColgp_HArray1OfPnt(1,num_parameters) ;

  num_tangents = ((n - num_read) / 3)  - num_parameters ;
  num_tangents = Max (0,num_tangents) ; 
  num_tangents = Min (num_parameters, num_tangents) ;
  ii = 1 ;
  num_start = num_read ;
  num_read += 1 ;
  while (num_read <= num_parameters * 3 + num_start ) {
    for (jj = 1 ; jj <= 3 ; jj++) {
      a_point.SetCoord(jj,Draw::Atof(a[num_read])) ;
      num_read += 1 ;
    }
    PointsArrayPtr->SetValue(ii,a_point) ;
    ii += 1 ;
  }
  GeomAPI_Interpolate anInterpolator(PointsArrayPtr,
    periodic_flag,
    tolerance) ; 

  if (num_tangents > 0) {
    TColgp_Array1OfVec TangentsArray(1,num_parameters) ;
    Handle(TColStd_HArray1OfBoolean) 
      TangentFlagsPtr =
      new TColStd_HArray1OfBoolean(1,num_parameters) ;

    for (ii = 1 ; ii <= num_tangents ; ii++) {
      TangentFlagsPtr->SetValue(ii,Standard_True) ;
    }
    for (ii = num_tangents + 1 ; ii <= num_parameters ; ii++) {
      TangentFlagsPtr->SetValue(ii,Standard_False) ;
    }
    ii = 1 ;
    while (ii <= num_tangents) {
      for (jj = 1 ; jj <= 3 ; jj++) {
        a_vector.SetCoord(jj,Draw::Atof(a[num_read])) ;
        num_read += 1 ;
      }
      TangentsArray.SetValue(ii,a_vector) ;
      ii += 1 ;
    }


    anInterpolator.Load(TangentsArray,
      TangentFlagsPtr) ;
  }
  anInterpolator.Perform() ;
  if (anInterpolator.IsDone()) {
    NewCurvePtr =
      anInterpolator.Curve() ;

    DrawTrSurf::Set(a[1],
      NewCurvePtr) ;
    di << a[2] << " " ;

  }
  return 0 ;
}

//==================================================================================
static Standard_Integer gcarc (Draw_Interpretor& di,Standard_Integer n, const char** a)
//==================================================================================
{
  if (n >= 5) {
    gp_Pnt P1,P2,P3,P4;
    if (!strcmp(a[2], "seg")) {
      if (DrawTrSurf::GetPoint(a[3], P1)) {
        if (DrawTrSurf::GetPoint(a[4], P2)) {
          Handle(Geom_Curve) theline (GC_MakeSegment(P1,P2).Value());
          DrawTrSurf::Set(a[1], theline);
          return 1;
        }
      }
    }
    else if (!strcmp(a[2], "cir")) {
      if (DrawTrSurf::GetPoint(a[3], P1)) {
        if (DrawTrSurf::GetPoint(a[4], P2)) {
          if (DrawTrSurf::GetPoint(a[5], P3)) {
            //	    if (DrawTrSurf::GetPoint(a[6], P4)) {
            if (n>6) {
              DrawTrSurf::GetPoint(a[6], P4);
              gp_Vec V1 = gp_Vec(P2,P3);                                                    
              Handle(Geom_Curve)thearc (GC_MakeArcOfCircle(P1,V1,P4).Value());
              DrawTrSurf::Set(a[1], thearc);
              return 1;
            }
            else {
              Handle(Geom_Curve)thearc (GC_MakeArcOfCircle(P1,P2,P3).Value());
              DrawTrSurf::Set(a[1], thearc);
              return 1;
            }
          }
        }
      }
    }
  }
  di <<"give a name for arc and the type seg or cir then\n";
  di <<"give passing points p1 p2 for seg    p1 p2 p3 or p1 p2 p3 p4 for cir (p2 p3 is a tgtvec)!\n";
  return 0;
}

//=======================================================================
//function : ConstraintCommands
//purpose  : 
//=======================================================================


void  GeometryTest::ConstraintCommands(Draw_Interpretor& theCommands)
{

  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;
  // constrained constructs
  g = "GEOMETRY Constraints";

  theCommands.Add("cirtang",
    "cirtang cname curve/point/radius curve/point/radius curve/point/radius",
    __FILE__,
    cirtang,g);

  theCommands.Add("lintan",
    "lintan lname curve1 curve2 [angle]",
    __FILE__,
    lintang,g);


  theCommands.Add("interpol",
    "interpol cname [fic]", 
    __FILE__,
    interpol, g);
  theCommands.Add("tanginterpol",
    "tanginterpol curve [p] num_points points [tangents] modifier  p = periodic",
    __FILE__,
    tanginterpol,g);

  theCommands.Add("gcarc",
    "gcarc name seg/cir p1 p2 p3 p4",
    __FILE__,
    gcarc,g);
}
