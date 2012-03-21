// Created on: 1995-01-11
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

// modified : pmn 11/04/97 : mis dans GeomliteTest


#include <GeomliteTest.hxx>
#include <Geom2d_Curve.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
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
  if ( n < 4) return 1;

  gp_Pnt2d P(atof(a[2]),atof(a[3]));

  char name[100];

  Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(a[1]);

  if (GC.IsNull())
    return 1;

  Geom2dAPI_ProjectPointOnCurve proj(P,GC,GC->FirstParameter(),
				          GC->LastParameter());
  
  for ( Standard_Integer i = 1; i <= proj.NbPoints(); i++) {
    gp_Pnt2d P1 = proj.Point(i);
    Handle(Geom2d_Line) L = new Geom2d_Line(P,gp_Vec2d(P,P1));
    Handle(Geom2d_TrimmedCurve) CT = 
      new Geom2d_TrimmedCurve(L, 0., P.Distance(P1));
    sprintf(name,"%s%d","ext_",i);
    char* temp = name; // portage WNT
    DrawTrSurf::Set(temp, CT);
    di << name << " ";
  }

  return 0;
}

//=======================================================================
//function : appro
//purpose  : 
//=======================================================================

static Standard_Integer appro(Draw_Interpretor& di, Standard_Integer n, const char** a)
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
  

  static Standard_Real Tol2d = 1.e-6;

  if (n < 3) {
    if (n == 2) 
      Tol2d = atof(a[1]);
    
    di << "Tolerance for 2d approx : "<< Tol2d << "\n";
    return 0;
  }


  Standard_Integer i, Nb = atoi(a[2]);
  
  Standard_Boolean hasPoints = Standard_True;
  TColgp_Array1OfPnt2d Points(1, Nb);
  TColStd_Array1OfReal YValues(1,Nb);
  Standard_Real X0=0,DX=0;
  
  Handle(Draw_Marker2D) mark;
  
  if (n == 3)  {
    // saisie interactive
    Standard_Integer id,XX,YY,b;
    dout.Select(id,XX,YY,b);
    Standard_Real zoom = dout.Zoom(id);

    Points(1) = gp_Pnt2d( ((Standard_Real)XX)/zoom, 
		          ((Standard_Real)YY)/zoom );
    
    mark = new Draw_Marker2D( Points(1), Draw_X, Draw_vert); 
    
    dout << mark;
    
    for (i = 2; i<=Nb; i++) {
      dout.Select(id,XX,YY,b);
      Points(i) = gp_Pnt2d( ((Standard_Real)XX)/zoom, 
			    ((Standard_Real)YY)/zoom );
      mark = new Draw_Marker2D( Points(i), Draw_X, Draw_vert); 
      dout << mark;
    }
  }    
  else {
    if ( n == 4) {
    // points sur courbe
      Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(a[3]);
      if ( GC.IsNull()) 
	return 1;

      Standard_Real U, U1, U2;
      U1 = GC->FirstParameter();
      U2 = GC->LastParameter();
      Standard_Real Delta = ( U2 - U1) / (Nb-1);
      for ( i = 1 ; i <= Nb; i++) {
	U = U1 + (i-1) * Delta;
	Points(i) = GC->Value(U);
      }
    }

    else {
      // test points ou ordonnees
      hasPoints = Standard_False;
      Standard_Integer nc = n - 3;
      if (nc == 2 * Nb) {
	// points
	nc = 3;
	for (i = 1; i <= Nb; i++) {
	  Points(i).SetCoord(atof(a[nc]),atof(a[nc+1]));
	  nc += 2;
	}
      }
      else if (nc - 2 == Nb) {
	// YValues
	nc = 5;
	X0 = atof(a[3]);
	DX = atof(a[4]);
      	for (i = 1; i <= Nb; i++) {
	  YValues(i) = atof(a[nc]);
	  Points(i).SetCoord(X0+(i-1)*DX,YValues(i));
	  nc++;
	}
      }
      else
	return 1;
    }
    // display the points
    for ( i = 1 ; i <= Nb; i++) {
      mark = new Draw_Marker2D( Points(i), Draw_X, Draw_vert); 
      dout << mark;
    }
  }
  dout.Flush();
  Standard_Integer Dmin = 3;
  Standard_Integer Dmax = 8;
  
  Handle(Geom2d_BSplineCurve) TheCurve;
  if (hasPoints)
    TheCurve = Geom2dAPI_PointsToBSpline(Points,Dmin,Dmax,GeomAbs_C2,Tol2d);
  else
    TheCurve = Geom2dAPI_PointsToBSpline(YValues,X0,DX,Dmin,Dmax,GeomAbs_C2,Tol2d);
  
  DrawTrSurf::Set(a[1], TheCurve);
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

  Handle(Geom2d_Curve)   GC1, GC2;

  Standard_Real U1f,U1l,U2f,U2l;

  GC1 = DrawTrSurf::GetCurve2d(a[1]);
  if ( GC1.IsNull())
    return 1;
  U1f = GC1->FirstParameter();
  U1l = GC1->LastParameter();

  GC2 = DrawTrSurf::GetCurve2d(a[2]);
  if ( GC2.IsNull())
    return 1;
  U2f = GC2->FirstParameter();
  U2l = GC2->LastParameter();

  char name[100];

  Geom2dAPI_ExtremaCurveCurve Ex(GC1,GC2,U1f,U1l,U2f,U2l);

// modified by APV (compilation error - LINUX)
//  for ( Standard_Integer i = 1; i <= Ex.NbExtrema(); i++) {
  Standard_Integer i;
  for ( i = 1; i <= Ex.NbExtrema(); i++) {
// modified by APV (compilation error - LINUX)

    gp_Pnt2d P1,P2;
    Ex.Points(i,P1,P2);
    di << "dist " << i << ": " << Ex.Distance(i) << " \n";
    if (Ex.Distance(i) <= Precision::PConfusion()) {
      Handle(Draw_Marker2D) mark = new Draw_Marker2D( P1, Draw_X, Draw_vert); 
      dout << mark;
      dout.Flush();
    }
    else {
      Handle(Geom2d_Line) L = new Geom2d_Line(P1,gp_Vec2d(P1,P2));
      Handle(Geom2d_TrimmedCurve) CT = 
	new Geom2d_TrimmedCurve(L, 0., P1.Distance(P2));
      sprintf(name,"%s%d","ext_",i);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << " ";
    }
  }
  if (i==1)
    di << "No decisions ";

  return 0;
}


//=======================================================================
//function : intersect
//purpose  : 
//=======================================================================

static Standard_Integer intersect(Draw_Interpretor& /*di*/, Standard_Integer n, const char** a)
{
  if( n < 2) 
    return 1;

  Handle(Geom2d_Curve) C1 = DrawTrSurf::GetCurve2d(a[1]);
  if ( C1.IsNull()) 
    return 1;

  Standard_Real Tol = 0.001;
  Geom2dAPI_InterCurveCurve Intersector;

  Handle(Geom2d_Curve) C2;
  if ( n == 3) {
    C2 = DrawTrSurf::GetCurve2d(a[2]);
    if ( C2.IsNull())
      return 1;
    Intersector.Init(C1,C2,Tol);
  }
  else {
    Intersector.Init(C1, Tol);
  }

  Standard_Integer i;

  for ( i = 1; i <= Intersector.NbPoints(); i++) {
    gp_Pnt2d P = Intersector.Point(i);
    Handle(Draw_Marker2D) mark = new Draw_Marker2D( P, Draw_X, Draw_vert); 
    dout << mark;
  }
  dout.Flush();

  Handle(Geom2d_Curve) S1,S2;
  Handle(DrawTrSurf_Curve2d) CD;
  if ( n == 3) {
    for ( i = 1; i <= Intersector.NbSegments(); i++) {
      Intersector.Segment(i,S1,S2);
      CD = new DrawTrSurf_Curve2d(S1, Draw_bleu, 30);
      dout << CD;
      CD = new DrawTrSurf_Curve2d(S2, Draw_violet, 30);
      dout << CD;
    }
  }
  dout.Flush();

  return 0;
}


void GeomliteTest::API2dCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;

  const char *g;

  done = Standard_True;
  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("2dproj", "proj curve x y",__FILE__, proj,g);

  g = "GEOMETRY approximations";

  theCommands.Add("2dapprox", "2dapprox result nbpoint [curve] [[x] y [x] y...]",__FILE__, 
		  appro,g);
  theCommands.Add("2dinterpole", "2dinterpole result nbpoint [curve] [[x] y [x] y ...]",__FILE__, 
		  appro,g);

  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("2dextrema", "extrema curve curve",__FILE__,
		  extrema,g);

  g = "GEOMETRY intersections";

  theCommands.Add("2dintersect", "intersect curve curve",__FILE__,
		  intersect,g);
}
