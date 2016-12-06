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
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
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
#include <Draw_Segment3D.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Precision.hxx>
#include <stdio.h>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : proj
//purpose  : 
//=======================================================================

static void showProjSolution(Draw_Interpretor& di,
                             const Standard_Integer i,
                             const gp_Pnt& P, const gp_Pnt& P1,
                             const Standard_Real U, const Standard_Real V,
                             const Standard_Boolean isSurface)
{
  char name[100];
  Sprintf(name, "%s%d", "ext_", i);
  di << name << " ";
  char* temp = name; // portage WNT
  if (P.Distance(P1) > Precision::Confusion())
  {
    Handle(Geom_Line) L = new Geom_Line(P, gp_Vec(P, P1));
    Handle(Geom_TrimmedCurve) CT =
      new Geom_TrimmedCurve(L, 0., P.Distance(P1));
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

//=======================================================================
//function : proj
//purpose  : 
//=======================================================================

static Standard_Integer proj (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n < 5)
  {
    cout << " Use proj curve/surf x y z [{extrema algo: g(grad)/t(tree)}|{u v}]" << endl;
    return 1;
  }

  gp_Pnt P(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]));

  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[1]);
  Handle(Geom_Surface) GS;
  Extrema_ExtAlgo aProjAlgo = Extrema_ExtAlgo_Grad;

  if (n == 6 && a[5][0] == 't')
    aProjAlgo = Extrema_ExtAlgo_Tree;

  if (GC.IsNull())
  {
    GS = DrawTrSurf::GetSurface(a[1]);

    if (GS.IsNull())
      return 1;

    if (n <= 6)
    {
      Standard_Real U1, U2, V1, V2;
      GS->Bounds(U1,U2,V1,V2);

      GeomAPI_ProjectPointOnSurf proj(P,GS,U1,U2,V1,V2,aProjAlgo);
      if (!proj.IsDone())
      {
        di << "projection failed.";
        return 0;
      }

      Standard_Real UU,VV;
      for ( Standard_Integer i = 1; i <= proj.NbPoints(); i++)
      {
        gp_Pnt P1 = proj.Point(i);
        proj.Parameters(i, UU, VV);
        showProjSolution(di, i, P, P1, UU, VV, Standard_True);
      }
    }
    else if (n == 7)
    {
      const gp_XY aP2d(Draw::Atof(a[5]), Draw::Atof(a[6]));
      GeomAdaptor_Surface aGAS(GS);
      Extrema_GenLocateExtPS aProjector(aGAS, Precision::PConfusion(), Precision::PConfusion());
      aProjector.Perform(P, aP2d.X(), aP2d.Y());
      if (!aProjector.IsDone())
      {
        di << "projection failed.";
        return 0;
      }

      const Extrema_POnSurf& aP = aProjector.Point();
      Standard_Real UU, VV;
      aP.Parameter(UU, VV);
      showProjSolution(di, 1, P, aP.Value(), UU, VV, Standard_True);
    }
  }
  else
  {
    GeomAPI_ProjectPointOnCurve proj(P,GC,GC->FirstParameter(),
      GC->LastParameter());

    if(proj.NbPoints() == 0)
    {
      cout << "No project point was found." << endl;
      return 0;
    }

    for ( Standard_Integer i = 1; i <= proj.NbPoints(); i++)
    {
      gp_Pnt P1 = proj.Point(i);
      Standard_Real UU = proj.Parameter(i);
      showProjSolution(di, i, P, P1, UU, UU, Standard_False);
    }
  }

  return 0;
}

//=======================================================================
//function : appro
//purpose  : 
//=======================================================================

static Standard_Integer appro(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n<3) return 1;

  Handle(Geom_Curve) GC;
  Standard_Integer Nb = Draw::Atoi(a[2]);

  TColgp_Array1OfPnt Points(1, Nb);

  Handle(Draw_Marker3D) mark;

  if ( n == 4) {
    GC = DrawTrSurf::GetCurve(a[3]);
    if ( GC.IsNull()) 
      return 1;

    Standard_Real U, U1, U2;
    U1 = GC->FirstParameter();
    U2 = GC->LastParameter();
    Standard_Real Delta = ( U2 - U1) / (Nb-1);
    for ( Standard_Integer i = 1 ; i <= Nb; i++) {
      U = U1 + (i-1) * Delta;
      Points(i) = GC->Value(U);
      mark = new Draw_Marker3D( Points(i), Draw_X, Draw_vert); 
      dout << mark;
    }
  }
  else {
    Standard_Integer id,XX,YY,b;
    dout.Select(id,XX,YY,b);
    Standard_Real zoom = dout.Zoom(id);

    Points(1) = gp_Pnt( ((Standard_Real)XX)/zoom, 
		        ((Standard_Real)YY)/zoom, 
		        0.);
    
    mark = new Draw_Marker3D( Points(1), Draw_X, Draw_vert); 
    
    dout << mark;
    
    for (Standard_Integer i = 2; i<=Nb; i++) {
      dout.Select(id,XX,YY,b);
      Points(i) = gp_Pnt( ((Standard_Real)XX)/zoom, 
			 ((Standard_Real)YY)/zoom, 
			 0.);
      mark = new Draw_Marker3D( Points(i), Draw_X, Draw_vert); 
      dout << mark;
    }
  }    
  dout.Flush();
  Standard_Integer Dmin = 3;
  Standard_Integer Dmax = 8;
  Standard_Real Tol3d = 1.e-3;
  
  Handle(Geom_BSplineCurve) TheCurve;
  GeomAPI_PointsToBSpline aPointToBSpline(Points,Dmin,Dmax,GeomAbs_C2,Tol3d);
  TheCurve = aPointToBSpline.Curve();

  
  DrawTrSurf::Set(a[1], TheCurve);
  di << a[1];

  return 0;

}


//=======================================================================
//function : grilapp
//purpose  : 
//=======================================================================

static Standard_Integer grilapp(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n < 12) return 1;

  Standard_Integer i,j;
  Standard_Integer Nu = Draw::Atoi(a[2]);
  Standard_Integer Nv = Draw::Atoi(a[3]);
  TColStd_Array2OfReal ZPoints (1, Nu, 1, Nv);

  Standard_Real X0 = Draw::Atof(a[4]);
  Standard_Real dX = Draw::Atof(a[5]);
  Standard_Real Y0 = Draw::Atof(a[6]);
  Standard_Real dY = Draw::Atof(a[7]);

  Standard_Integer Count = 8;
  for ( j = 1; j <= Nv; j++) {
    for ( i = 1; i <= Nu; i++) {
      if ( Count > n) return 1;
      ZPoints(i,j) = Draw::Atof(a[Count]);
      Count++;
    }
  }
  
  Handle(Geom_BSplineSurface) S 
    = GeomAPI_PointsToBSplineSurface(ZPoints,X0,dX,Y0,dY);
  DrawTrSurf::Set(a[1],S);

  di << a[1];
  
  return 0;
}

//=======================================================================
//function : surfapp
//purpose  : 
//=======================================================================

static Standard_Integer surfapp(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n < 5 ) return 1;

  Standard_Integer i,j;
  Standard_Integer Nu = Draw::Atoi(a[2]);
  Standard_Integer Nv = Draw::Atoi(a[3]);
  TColgp_Array2OfPnt Points (1, Nu, 1, Nv);

  if ( n == 5) {
    Handle(Geom_Surface) Surf = DrawTrSurf::GetSurface(a[4]);
    if ( Surf.IsNull()) return 1;

    Standard_Real U, V, U1, V1, U2, V2;
    Surf->Bounds( U1, U2, V1, V2);
    for ( j = 1; j <= Nv; j++) {
      V = V1 + (j-1) * (V2-V1) / (Nv-1);
      for ( i = 1; i <= Nu; i++) {
	U = U1 + (i-1) * (U2-U1) / (Nu-1);
	Points(i,j) = Surf->Value(U,V);
      }
    } 
  }
  else if ( n >= 16) {
    Standard_Integer Count = 4;
    for ( j = 1; j <= Nv; j++) {
      for ( i = 1; i <= Nu; i++) {
	if ( Count > n) return 1;
	Points(i,j) = gp_Pnt(Draw::Atof(a[Count]),Draw::Atof(a[Count+1]),Draw::Atof(a[Count+2]));
	Count += 3;
      }
    }
  }
  char name[100];
  Standard_Integer Count = 1;
  for ( j = 1; j <= Nv; j++) {
    for ( i = 1; i <= Nu; i++) {
      Sprintf(name,"point_%d",Count++);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp,Points(i,j));
    }
  } 

  Handle(Geom_BSplineSurface) S = GeomAPI_PointsToBSplineSurface(Points);
  DrawTrSurf::Set(a[1],S);
  di << a[1];

  return 0;
}


//=======================================================================
//function : extrema
//purpose  : 
//=======================================================================

static Standard_Integer extrema(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3)
  {
    return 1;
  }

  Handle(Geom_Curve)   GC1, GC2;
  Handle(Geom_Surface) GS1, GS2;

  Standard_Boolean C1 = Standard_False;
  Standard_Boolean C2 = Standard_False;
  Standard_Boolean S1 = Standard_False;
  Standard_Boolean S2 = Standard_False;
  Standard_Boolean isInfinitySolutions = Standard_False;
  Standard_Real aMinDist = RealLast();

  Standard_Real U1f, U1l, U2f, U2l, V1f = 0., V1l = 0., V2f = 0., V2l = 0.;

  GC1 = DrawTrSurf::GetCurve(a[1]);
  if ( GC1.IsNull()) {
    GS1 = DrawTrSurf::GetSurface(a[1]);
    if ( GS1.IsNull())
      return 1;
    S1 = Standard_True;
    GS1->Bounds(U1f,U1l,V1f,V1l);
  }
  else {
    C1 = Standard_True;
    U1f = GC1->FirstParameter();
    U1l = GC1->LastParameter();
  }

  GC2 = DrawTrSurf::GetCurve(a[2]);
  if ( GC2.IsNull()) {
    GS2 = DrawTrSurf::GetSurface(a[2]);
    if ( GS2.IsNull())
      return 1;
    S2 = Standard_True;
    GS2->Bounds(U2f,U2l,V2f,V2l);
  }
  else {
    C2 = Standard_True;
    U2f = GC2->FirstParameter();
    U2l = GC2->LastParameter();
  }

  NCollection_Vector<gp_Pnt> aPnts1, aPnts2;
  NCollection_Vector<Standard_Real> aPrms[4];
  if (C1 && C2)
  {
    GeomAPI_ExtremaCurveCurve Ex(GC1, GC2, U1f, U1l, U2f, U2l);

    for (Standard_Integer aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
    {
        gp_Pnt aP1, aP2;
        Ex.Points(aJ, aP1, aP2);
        aPnts1.Append(aP1);
        aPnts2.Append(aP2);

        Standard_Real aU1, aU2;
        Ex.Parameters(aJ, aU1, aU2);
        aPrms[0].Append(aU1);
        aPrms[2].Append(aU2);
    }
    // Since GeomAPI cannot provide access to flag directly.
    isInfinitySolutions = Ex.Extrema().IsParallel();
    aMinDist = Ex.LowerDistance();
  }
  else if (C1 && S2)
  {
    GeomAPI_ExtremaCurveSurface Ex(GC1, GS2, U1f, U1l, U2f, U2l, V2f, V2l);
    for (Standard_Integer aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
    {
      gp_Pnt aP1, aP2;
      Ex.Points(aJ, aP1, aP2);
      aPnts1.Append(aP1);
      aPnts2.Append(aP2);

      Standard_Real aU1, aU2, aV2;
      Ex.Parameters(aJ, aU1, aU2, aV2);
      aPrms[0].Append(aU1);
      aPrms[2].Append(aU2);
      aPrms[3].Append(aV2);
    }
  }
  else if (S1 && C2)
  {
    GeomAPI_ExtremaCurveSurface Ex(GC2, GS1, U2f, U2l, U1f, U1l, V1f, V1l);
    for (Standard_Integer aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
    {
      gp_Pnt aP2, aP1;
      Ex.Points(aJ, aP2, aP1);
      aPnts1.Append(aP1);
      aPnts2.Append(aP2);

      Standard_Real aU1, aV1, aU2;
      Ex.Parameters(aJ, aU2, aU1, aV1);
      aPrms[0].Append(aU1);
      aPrms[1].Append(aV1);
      aPrms[2].Append(aU2);
    }
  }
  else if (S1 && S2)
  {
    GeomAPI_ExtremaSurfaceSurface Ex(
      GS1, GS2, U1f, U1l, V1f, V1l, U2f, U2l, V2f, V2l);
    for (Standard_Integer aJ = 1; aJ <= Ex.NbExtrema(); ++aJ)
    {
      gp_Pnt aP1, aP2;
      Ex.Points(aJ, aP1, aP2);
      aPnts1.Append(aP1);
      aPnts2.Append(aP2);

      Standard_Real aU1, aV1, aU2, aV2;
      Ex.Parameters(aJ, aU1, aV1, aU2, aV2);
      aPrms[0].Append(aU1);
      aPrms[1].Append(aV1);
      aPrms[2].Append(aU2);
      aPrms[3].Append(aV2);
    }
  }

  char aName[100];
  char* aName2 = aName; // portage WNT

  // Output points.
  const Standard_Integer aPntCount = aPnts1.Size();
  if (aPntCount == 0 || isInfinitySolutions)
  {
    // Infinity solutions flag may be set with 0 number of 
    // solutions in analytic extrema Curve/Curve.
    if (isInfinitySolutions) 
      di << "Infinite number of extremas, distance = " << aMinDist << "\n";
    else
      di << "No solutions!\n";
  }
  for (Standard_Integer aJ = 1; aJ <= aPntCount; aJ++)
  {
    gp_Pnt aP1 = aPnts1(aJ - 1), aP2 = aPnts2(aJ - 1);

    if (aP1.Distance(aP2) < 1.e-16)
    {
      di << "Extrema " << aJ << " is point : " <<
        aP1.X() << " " << aP1.Y() << " " << aP1.Z() << "\n";
      continue;
    }

    Handle(Geom_Line) aL = new Geom_Line(aP1, gp_Vec(aP1, aP2));
    Handle(Geom_TrimmedCurve) aCT = 
      new Geom_TrimmedCurve(aL, 0., aP1.Distance(aP2));
    Sprintf(aName, "%s%d", "ext_", aJ);
    DrawTrSurf::Set(aName2, aCT);
    di << aName << " ";
  }

  if (n >= 4)
  {
    // Output points.
    for (Standard_Integer aJ = 1; aJ <= aPntCount; aJ++)
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
    for (Standard_Integer aJ = 0; aJ < 4; ++aJ)
    {
      for (Standard_Integer aPrmCount = aPrms[aJ].Size(), aK = 0;
        aK < aPrmCount; ++aK)
      {
        Standard_Real aP = aPrms[aJ](aK);
        Sprintf(aName, "%s%d%s%d", "prm_", aJ + 1, "_", aK + 1);
        Draw::Set(aName2, aP);
        di << aName << " ";
      }
    }
  }

  return 0;
}

//=======================================================================
//function : totalextcc
//purpose  : 
//=======================================================================

static Standard_Integer totalextcc(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n<3) return 1;

  Handle(Geom_Curve)   GC1, GC2;


  Standard_Real U1f,U1l,U2f,U2l;

  GC1 = DrawTrSurf::GetCurve(a[1]);
  if ( GC1.IsNull()) {
      return 1;
  }
  else {
    U1f = GC1->FirstParameter();
    U1l = GC1->LastParameter();
  }

  GC2 = DrawTrSurf::GetCurve(a[2]);
  if ( GC2.IsNull()) {
      return 1;
  }
  else {
    U2f = GC2->FirstParameter();
    U2l = GC2->LastParameter();
  }

  char name[100];
    GeomAPI_ExtremaCurveCurve Ex(GC1,GC2,U1f,U1l,U2f,U2l);
  gp_Pnt P1,P2;
  if(Ex.TotalNearestPoints(P1,P2)) {
    if (P1.Distance(P2) < 1.e-16) {
      di << "Extrema is point : " << P1.X() << " " << P1.Y() << " " << P1.Z() << "\n";
    }
    else {
      di << "Extrema is segment of line\n"; 
      Handle(Geom_Line) L = new Geom_Line(P1,gp_Vec(P1,P2));
      Handle(Geom_TrimmedCurve) CT = 
	new Geom_TrimmedCurve(L, 0., P1.Distance(P2));
      Sprintf(name,"%s%d","ext_",1);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << " ";
    }

    Standard_Real u1, u2;
    Ex.TotalLowerDistanceParameters(u1, u2);

    di << "Parameters on curves : " << u1 << " " << u2 << "\n";

  }
  else {
    di << "Curves are infinite and parallel\n";
  }
  
  di << "Minimal distance : " << Ex.TotalLowerDistance() << "\n";

  return 0;

}


void GeometryTest::APICommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;

  done = Standard_True;

  theCommands.Add("proj", "proj curve/surf x y z [{extrema algo: g(grad)/t(tree)}|{u v}]\n"
                  "\t\tOptional parameters are relevant to surf only.\n"
                  "\t\tIf initial {u v} are given then local extrema is called",__FILE__, proj);

  theCommands.Add("appro", "appro result nbpoint [curve]",__FILE__, appro);
  theCommands.Add("surfapp","surfapp result nbupoint nbvpoint x y z ....",
		  __FILE__,
		  surfapp);
  theCommands.Add("grilapp",
       "grilapp result nbupoint nbvpoint X0 dX Y0 dY z11 z12 .. z1nu ....  ",
        __FILE__,grilapp);

  theCommands.Add("extrema", "extrema curve/surface curve/surface [extended_output = 0|1]",__FILE__,extrema);
  theCommands.Add("totalextcc", "totalextcc curve curve",__FILE__,totalextcc);
}
