// Created on: 1995-01-17
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <GeometryTest.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
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
#ifdef WNT
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : proj
//purpose  : 
//=======================================================================

static Standard_Integer proj (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n < 5) return 1;

  gp_Pnt P(atof(a[2]),atof(a[3]),atof(a[4]));

  char name[100];

  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[1]);
  Handle(Geom_Surface) GS;

  if (GC.IsNull())  {
    GS = DrawTrSurf::GetSurface(a[1]);
    if (GS.IsNull())
      return 1;
    Standard_Real U1, U2, V1, V2;
    GS->Bounds(U1,U2,V1,V2);

    GeomAPI_ProjectPointOnSurf proj(P,GS,U1,U2,V1,V2);
    Standard_Real UU,VV;
    for ( Standard_Integer i = 1; i <= proj.NbPoints(); i++) {
      gp_Pnt P1 = proj.Point(i);
      if ( P.Distance(P1) > Precision::Confusion()) {
	Handle(Geom_Line) L = new Geom_Line(P,gp_Vec(P,P1));
	Handle(Geom_TrimmedCurve) CT = 
	  new Geom_TrimmedCurve(L, 0., P.Distance(P1));
	sprintf(name,"%s%d","ext_",i);
	char* temp = name; // portage WNT
	DrawTrSurf::Set(temp, CT);
	di << name << " ";
      }
      else {
	sprintf(name,"%s%d","ext_",i);
	di << name << " ";
	char* temp = name; // portage WNT
	DrawTrSurf::Set(temp, P1);
	proj.Parameters(i,UU,VV);
	di << " Le point est sur la surface." << "\n";
	di << " Ses parametres sont:  UU = " << UU << "\n";
	di << "                       VV = " << VV << "\n";
      }
    }

  }
  else {
    GeomAPI_ProjectPointOnCurve proj(P,GC,GC->FirstParameter(),
				          GC->LastParameter());
//    Standard_Real UU;
    for ( Standard_Integer i = 1; i <= proj.NbPoints(); i++) {
      gp_Pnt P1 = proj.Point(i);
      Standard_Real UU = proj.Parameter(i);
      di << " parameter " << i << " = " << UU << "\n";
      if ( P.Distance(P1) > Precision::Confusion()) {
	Handle(Geom_Line) L = new Geom_Line(P,gp_Vec(P,P1));
	Handle(Geom_TrimmedCurve) CT = 
	  new Geom_TrimmedCurve(L, 0., P.Distance(P1));
	sprintf(name,"%s%d","ext_",i);
	char* temp = name; // portage WNT
	DrawTrSurf::Set(temp, CT);
	di << name << " ";
      }
      else {
	sprintf(name,"%s%d","ext_",i);
	char* temp = name; // portage WNT
	DrawTrSurf::Set(temp, P1);
	di << name << " ";
	UU = proj.Parameter(i);
	di << " Le point est sur la courbe." << "\n";
	di << " Son parametre est U = " << UU << "\n";
      }
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
  Standard_Integer Nb = atoi(a[2]);

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
  Standard_Integer Nu = atoi(a[2]);
  Standard_Integer Nv = atoi(a[3]);
  TColStd_Array2OfReal ZPoints (1, Nu, 1, Nv);

  Standard_Real X0 = atof(a[4]);
  Standard_Real dX = atof(a[5]);
  Standard_Real Y0 = atof(a[6]);
  Standard_Real dY = atof(a[7]);

  Standard_Integer Count = 8;
  for ( j = 1; j <= Nv; j++) {
    for ( i = 1; i <= Nu; i++) {
      if ( Count > n) return 1;
      ZPoints(i,j) = atof(a[Count]);
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
  Standard_Integer Nu = atoi(a[2]);
  Standard_Integer Nv = atoi(a[3]);
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
	Points(i,j) = gp_Pnt(atof(a[Count]),atof(a[Count+1]),atof(a[Count+2]));
	Count += 3;
      }
    }
  }
  char name[100];
  Standard_Integer Count = 1;
  for ( j = 1; j <= Nv; j++) {
    for ( i = 1; i <= Nu; i++) {
      sprintf(name,"point_%d",Count++);
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
  if ( n<3) return 1;

  Handle(Geom_Curve)   GC1, GC2;
  Handle(Geom_Surface) GS1, GS2;

  Standard_Boolean C1 = Standard_False;
  Standard_Boolean C2 = Standard_False;
  Standard_Boolean S1 = Standard_False;
  Standard_Boolean S2 = Standard_False;

  Standard_Real U1f,U1l,U2f,U2l,V1f,V1l,V2f,V2l;

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

  char name[100];
  if ( C1 && C2) {
    GeomAPI_ExtremaCurveCurve Ex(GC1,GC2,U1f,U1l,U2f,U2l);
    if(!Ex.Extrema().IsParallel()) {
      for ( Standard_Integer i = 1; i <= Ex.NbExtrema(); i++) {
	gp_Pnt P1,P2;
	Ex.Points(i,P1,P2);
	if (P1.Distance(P2) < 1.e-16) {
	  di << "Extrema " << i << " is point : " << P1.X() << " " << P1.Y() << " " << P1.Z() << "\n";
	  continue;
	}
	Handle(Geom_Line) L = new Geom_Line(P1,gp_Vec(P1,P2));
	Handle(Geom_TrimmedCurve) CT = 
	  new Geom_TrimmedCurve(L, 0., P1.Distance(P2));
	sprintf(name,"%s%d","ext_",i);
	char* temp = name; // portage WNT
	DrawTrSurf::Set(temp, CT);
	di << name << " ";
      }
    }
    else {
      di << "Infinite number of extremas, distance = " << Ex.LowerDistance() << "\n";
    }
  }
  else if ( C1 & S2) {
    GeomAPI_ExtremaCurveSurface Ex(GC1,GS2,U1f,U1l,U2f,U2l,V2f,V2l);
    for ( Standard_Integer i = 1; i <= Ex.NbExtrema(); i++) {
      gp_Pnt P1,P2;
      Ex.Points(i,P1,P2);
      if (P1.Distance(P2) < 1.e-16) continue;
      Handle(Geom_Line) L = new Geom_Line(P1,gp_Vec(P1,P2));
      Handle(Geom_TrimmedCurve) CT = 
	new Geom_TrimmedCurve(L, 0., P1.Distance(P2));
      sprintf(name,"%s%d","ext_",i);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << " ";
    }
  }
  else if ( S1 & C2) {
    GeomAPI_ExtremaCurveSurface Ex(GC2,GS1,U2f,U2l,U1f,U1l,V1f,V1l);
    for ( Standard_Integer i = 1; i <= Ex.NbExtrema(); i++) {
      gp_Pnt P1,P2;
      Ex.Points(i,P1,P2);
      if (P1.Distance(P2) < 1.e-16) continue;
      Handle(Geom_Line) L = new Geom_Line(P1,gp_Vec(P1,P2));
      Handle(Geom_TrimmedCurve) CT = 
	new Geom_TrimmedCurve(L, 0., P1.Distance(P2));
      sprintf(name,"%s%d","ext_",i);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << " ";
    }
  }
  else if ( S1 & S2) {
    GeomAPI_ExtremaSurfaceSurface Ex(GS1,GS2,U1f,U1l,V1f,V1l,U2f,U2l,V2f,V2l);
    for ( Standard_Integer i = 1; i <= Ex.NbExtrema(); i++) {
      gp_Pnt P1,P2;
      Ex.Points(i,P1,P2);
      if (P1.Distance(P2) < 1.e-16) continue;
      Handle(Geom_Line) L = new Geom_Line(P1,gp_Vec(P1,P2));
      Handle(Geom_TrimmedCurve) CT = 
	new Geom_TrimmedCurve(L, 0., P1.Distance(P2));
      sprintf(name,"%s%d","ext_",i);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << " ";
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
      di << "Extrema is segment of line" << "\n"; 
      Handle(Geom_Line) L = new Geom_Line(P1,gp_Vec(P1,P2));
      Handle(Geom_TrimmedCurve) CT = 
	new Geom_TrimmedCurve(L, 0., P1.Distance(P2));
      sprintf(name,"%s%d","ext_",1);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << " ";
    }

    Standard_Real u1, u2;
    Ex.TotalLowerDistanceParameters(u1, u2);

    di << "Parameters on curves : " << u1 << " " << u2 << "\n";

  }
  else {
    di << "Curves are infinite and parallel" << "\n";
  }
  
  di << "Minimal distance : " << Ex.TotalLowerDistance() << "\n";

  return 0;

}


void GeometryTest::APICommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;

  done = Standard_True;
  const char* g;

  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("proj", "proj curve/surf x y z",__FILE__, proj);

  g = "GEOMETRY approximations";

  theCommands.Add("appro", "appro result nbpoint [curve]",__FILE__, appro);
  theCommands.Add("surfapp","surfapp result nbupoint nbvpoint x y z ....",
		  __FILE__,
		  surfapp);
  theCommands.Add("grilapp",
       "grilapp result nbupoint nbvpoint X0 dX Y0 dY z11 z12 .. z1nu ....  ",
        __FILE__,grilapp);

  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("extrema", "extrema curve/surface curve/surface",__FILE__,extrema);
  theCommands.Add("totalextcc", "totalextcc curve curve",__FILE__,totalextcc);
}
