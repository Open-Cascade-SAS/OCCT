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
#include <DrawTrSurf_Curve.hxx>
#include <DrawTrSurf_Curve2d.hxx>
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

#include <BSplCLib.hxx>

#include <gp.hxx>
#include <gp_Pln.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>

#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>

#include <GeomLib.hxx>
#include <GeomConvert.hxx>
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

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>

#include <Precision.hxx>

#include <stdio.h>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <TColGeom2d_HArray1OfBSplineCurve.hxx>
#include <Convert_CompPolynomialToPoles.hxx>
#include <CPnts_AbscissaPoint.hxx>
#include <GCPnts_AbscissaPoint.hxx>

#include <PLib.hxx> 
#include <GeomAbs_Shape.hxx>
#include <Geom_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2d_Curve.hxx>

#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Approx_CurvilinearParameter.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : anacurve
//purpose  : 
//=======================================================================

static Standard_Integer anacurve (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Handle(Geom_Geometry) result;
  Handle(Geom2d_Curve)  result2d;

  if (!strcmp(a[0],"line")) {
    if (n == 6)
      result2d = new Geom2d_Line(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				 gp_Dir2d(Draw::Atof(a[4]),Draw::Atof(a[5])));
    else if (n == 8)
      result = new Geom_Line(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
			     gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])));
    else
      return 1;
  }

  else if (!strcmp(a[0],"circle")) {
    if (n == 5)
      result2d = 
	new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				   gp_Dir2d(1,0)),
			  Draw::Atof(a[4]));
    else if (n == 6)
      result = 
	new Geom_Circle(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
			       gp_Dir(0,0,1)),
			Draw::Atof(a[5]));
    else if (n == 7)
      result2d =
	new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				   gp_Dir2d(Draw::Atof(a[4]),Draw::Atof(a[5]))),
			  Draw::Atof(a[6]));
    else if (n == 9)
      result = 
	new Geom_Circle(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
			       gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]))),
			Draw::Atof(a[8]));
    else if (n == 12)
      result = 
	new Geom_Circle(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
			       gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])),
			       gp_Dir(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10]))),
			Draw::Atof(a[11]));
    else
      return 1;
  }
  
  else if (!strcmp(a[0],"parabola")) {
    if (n == 5)
      result2d = 
	new Geom2d_Parabola(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				     gp_Dir2d(1,0)),
			    Draw::Atof(a[4]));
    else if (n == 6)
      result = 
	new Geom_Parabola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				 gp_Dir(0,0,1)),
			  Draw::Atof(a[5]));
    else if (n == 7)
      result2d = 
	new Geom2d_Parabola(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				     gp_Dir2d(Draw::Atof(a[4]),Draw::Atof(a[5]))),
			    Draw::Atof(a[6]));
    else if (n == 9)
      result = 
	new Geom_Parabola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				 gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]))),
			  Draw::Atof(a[8]));
    else if (n == 12)
      result = 
	new Geom_Parabola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				 gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])),
				 gp_Dir(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10]))),
			  Draw::Atof(a[11]));
    else
      return 1;
  }
  
  else if (!strcmp(a[0],"ellipse")) {
    if (n == 6)
      result2d = 
	new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				    gp_Dir2d(1,0)),
			   Draw::Atof(a[4]),Draw::Atof(a[5]));
    else if (n == 7)
      result = 
	new Geom_Ellipse(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				gp_Dir(0,0,1)),
			 Draw::Atof(a[5]),Draw::Atof(a[6]));
    else if (n == 8)
      result2d = 
	new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				    gp_Dir2d(Draw::Atof(a[4]),Draw::Atof(a[5]))),
			   Draw::Atof(a[6]), Draw::Atof(a[7]));
    else if (n == 10)
      result = 
	new Geom_Ellipse(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]))),
			 Draw::Atof(a[8]), Draw::Atof(a[9]));
    else if (n == 13)
      result = 
	new Geom_Ellipse(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])),
				gp_Dir(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10]))),
			 Draw::Atof(a[11]), Draw::Atof(a[12]));
    else
      return 1;
  }
  
  else if (!strcmp(a[0],"hyperbola")) {
    if (n == 6)
      result2d = 
	new Geom2d_Hyperbola(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				      gp_Dir2d(1,0)),
			     Draw::Atof(a[4]),Draw::Atof(a[5]));
    else if (n == 7)
      result = 
	new Geom_Hyperbola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				  gp_Dir(0,0,1)),
			   Draw::Atof(a[5]),Draw::Atof(a[6]));
    else if (n == 8)
      result2d = 
	new Geom2d_Hyperbola(gp_Ax22d(gp_Pnt2d(Draw::Atof(a[2]),Draw::Atof(a[3])),
				      gp_Dir2d(Draw::Atof(a[4]),Draw::Atof(a[5]))),
			     Draw::Atof(a[6]), Draw::Atof(a[7]));
    else if (n == 10)
      result = 
	new Geom_Hyperbola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				  gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]))),
			   Draw::Atof(a[8]), Draw::Atof(a[9]));
    else if (n == 13)
      result = 
	new Geom_Hyperbola(gp_Ax2(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
				  gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])),
				  gp_Dir(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10]))),
			   Draw::Atof(a[11]), Draw::Atof(a[12]));
    else
      return 1;
  }
  
  if (!result.IsNull())
    DrawTrSurf::Set(a[1],result);
  else if (!result2d.IsNull())
    DrawTrSurf::Set(a[1],result2d);
  else
    return 1;

  return 0;
}

//=======================================================================
//function : polecurve
//purpose  : 
//=======================================================================

static Standard_Integer polecurve (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  Standard_Integer k,i;


  if (n < 3) return 1;

  if (!strcmp(a[0],"beziercurve")) {
    
    Standard_Integer np = Draw::Atoi(a[2]);
    if (np == 0) return 1;
    
    i = (n - 3) / (np);
    if (i < 3 || i > 4) return 1;
    Standard_Boolean hasw = i == 4;
    
    TColgp_Array1OfPnt poles(1,np);
    TColStd_Array1OfReal weights(1,np);
    
    k = 3;
    for (i = 1; i <= np; i++) {
      poles(i).SetCoord(Draw::Atof(a[k]),Draw::Atof(a[k+1]),Draw::Atof(a[k+2]));
      k += 3;
      if (hasw) {
	weights(i) = Draw::Atof(a[k]);
	k++;
      }
    }
    
    Handle(Geom_BezierCurve) result;
    if (hasw)
      result = new Geom_BezierCurve(poles,weights);
    else
      result = new Geom_BezierCurve(poles);
    
    DrawTrSurf::Set(a[1],result);
  }
  
  else if (!strcmp((*a[0] == 'p') ? a[0]+1 : a[0],"bsplinecurve")) {
    Standard_Integer deg = Draw::Atoi(a[2]);
    Standard_Integer nbk = Draw::Atoi(a[3]);

    TColStd_Array1OfReal    knots(1, nbk);
    TColStd_Array1OfInteger mults(1, nbk);
    k = 4;
    Standard_Integer Sigma = 0;
    for (i = 1; i<=nbk; i++) {
      knots( i) = Draw::Atof(a[k]);
      k++;
      mults( i) = Draw::Atoi(a[k]);
      Sigma += mults(i);
      k++;
    }

    Standard_Boolean periodic = *a[0] == 'p';
    Standard_Integer np;
    if (periodic)
      np = Sigma - mults(nbk);
    else
      np = Sigma - deg  -1;

    TColgp_Array1OfPnt   poles  (1, np);
    TColStd_Array1OfReal weights(1, np);
    
    for (i = 1; i <= np; i++) {
      poles(i).SetCoord(Draw::Atof(a[k]),Draw::Atof(a[k+1]),Draw::Atof(a[k+2]));
      k += 3;
      weights(i) = Draw::Atof(a[k]);
      k++;
    }
    
    Handle(Geom_BSplineCurve) result =
      new Geom_BSplineCurve(poles, weights, knots, mults, deg, periodic);
    DrawTrSurf::Set(a[1],result);
  }
  
  return 0;
}

//=======================================================================
//function : polecurve2d
//purpose  : 
//=======================================================================

static Standard_Integer polecurve2d (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  Standard_Integer k,i;


  if (n < 3) return 1;

  if (!strcmp(a[0],"2dbeziercurve")) {
    
    Standard_Integer np = Draw::Atoi(a[2]);
    if (np == 0) return 1;
    
    i = (n - 2) / (np);
    if (i < 2 || i > 3) return 1;
    Standard_Boolean hasw = i == 3;
    
    TColgp_Array1OfPnt2d poles(1,np);
    TColStd_Array1OfReal weights(1,np);
    
    k = 3;
    for (i = 1; i <= np; i++) {
      poles(i).SetCoord(Draw::Atof(a[k]),Draw::Atof(a[k+1]));
      k += 2;
      if (hasw) {
	weights(i) = Draw::Atof(a[k]);
	k++;
      }
    }
    
    Handle(Geom2d_BezierCurve) result;
    if (hasw)
      result = new Geom2d_BezierCurve(poles,weights);
    else
      result = new Geom2d_BezierCurve(poles);
    
    DrawTrSurf::Set(a[1],result);
  }
  
  else if (!strcmp((*(a[0]+2) == 'p') ? a[0]+3 : a[0]+2,"bsplinecurve")) {
    Standard_Integer deg = Draw::Atoi(a[2]);
    Standard_Integer nbk = Draw::Atoi(a[3]);

    TColStd_Array1OfReal    knots(1, nbk);
    TColStd_Array1OfInteger mults(1, nbk);
    k = 4;
    Standard_Integer Sigma = 0;
    for (i = 1; i<=nbk; i++) {
      knots( i) = Draw::Atof(a[k]);
      k++;
      mults( i) = Draw::Atoi(a[k]);
      Sigma += mults(i);
      k++;
    }

    Standard_Boolean periodic = *(a[0]+2) == 'p';
    Standard_Integer np;
    if (periodic)
      np = Sigma - mults(nbk);
    else
      np = Sigma - deg  -1;

    TColgp_Array1OfPnt2d poles  (1, np);
    TColStd_Array1OfReal weights(1, np);
    
    for (i = 1; i <= np; i++) {
      poles(i).SetCoord(Draw::Atof(a[k]),Draw::Atof(a[k+1]));
      k += 2;
      weights(i) = Draw::Atof(a[k]);
      k++;
    }
    
    Handle(Geom2d_BSplineCurve) result =
      new Geom2d_BSplineCurve(poles, weights, knots, mults, deg, periodic);
    DrawTrSurf::Set(a[1],result);
  }
  
  return 0;
}

//=======================================================================
//function : reverse
//purpose  : 
//=======================================================================

static Standard_Integer reverse (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;
  for (i = 1; i < n; i++) {

    Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[i]);
    if (!GC.IsNull()) {
      GC->Reverse();
      Draw::Repaint();
    }
    Handle(Geom2d_Curve) GC2d = DrawTrSurf::GetCurve2d(a[i]);
    if (!GC2d.IsNull()) {
      GC2d->Reverse();
      Draw::Repaint();
    }
  }
  return 0;
}

//=======================================================================
//function : cmovepole
//purpose  : 
//=======================================================================

static Standard_Integer cmovepole (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Standard_Real dx = Draw::Atof(a[3]);
  Standard_Real dy = Draw::Atof(a[4]);
  Standard_Real dz=0;
  if (n >= 6) dz = Draw::Atof(a[5]);
  Standard_Integer Index = Draw::Atoi(a[2]);

  Handle(Geom_BezierCurve) G1 = DrawTrSurf::GetBezierCurve(a[1]);
  if (!G1.IsNull()) {
    gp_Pnt P = G1->Pole(Index);
    P.SetCoord(P.X()+dx, P.Y()+dy, P.Z()+dz);
    G1->SetPole(Index,P);
    Draw::Repaint();
    return 0;
  }


  Handle(Geom_BSplineCurve) G2 = DrawTrSurf::GetBSplineCurve(a[1]);
  if (!G2.IsNull()) {
    gp_Pnt P = G2->Pole(Index);
    P.SetCoord(P.X()+dx, P.Y()+dy, P.Z()+dz);
    G2->SetPole(Index,P);
    Draw::Repaint();
    return 0;
  }

  Handle(Geom2d_BezierCurve) G3 = DrawTrSurf::GetBezierCurve2d(a[1]);
  if (!G3.IsNull()) {
    gp_Pnt2d P = G3->Pole(Index);
    P.SetCoord(P.X()+dx, P.Y()+dy);
    G3->SetPole(Index,P);
    Draw::Repaint();
    return 0;
  }


  Handle(Geom2d_BSplineCurve) G4 = DrawTrSurf::GetBSplineCurve2d(a[1]);
  if (!G4.IsNull()) {
    gp_Pnt2d P = G4->Pole(Index);
    P.SetCoord(P.X()+dx, P.Y()+dy);
    G4->SetPole(Index,P);
    Draw::Repaint();
    return 0;
  }



  return 0;
}

//=======================================================================
//function : cmovetangent
//purpose  : 
//=======================================================================

static Standard_Integer cmovetangent (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  Standard_Integer dimension,
  condition,
  error_status ;
  Standard_Real u,
  x,
  y,
  z,
  tolerance,
  tx,
  ty,
  tz ;
  u = Draw::Atof(a[2]);
  x = Draw::Atof(a[3]);
  y = Draw::Atof(a[4]);
  z = 0.0e0,
  tolerance = 1.0e-5 ;
  dimension = 3 ;
  if (n <= 8) {
    dimension = 2 ;
    if (n < 7) {
      return 1 ;
    }
  }
  else {
    dimension = 3 ;
    if (n < 9) {
      return 1 ;
    }
  }
  condition = 0 ;
  error_status  = 0 ;
  if (dimension == 3) {
    Handle(Geom_BSplineCurve) G2 = DrawTrSurf::GetBSplineCurve(a[1]);
    if (!G2.IsNull()) {
      z  = Draw::Atof(a[5]) ;
      tx = Draw::Atof(a[6]) ;
      ty = Draw::Atof(a[7]) ;
      tz = Draw::Atof(a[8]) ;
      if (n == 10) {
	condition = Max(Draw::Atoi(a[9]), -1)  ;
	condition = Min(condition, G2->Degree()-1) ;
      }
      gp_Pnt p;
      gp_Vec tangent ;
      p.SetCoord(x,y,z);
      tangent.SetCoord(tx,ty,tz) ;
      
      G2->MovePointAndTangent(u, 
			      p, 
			      tangent,
			      tolerance,
			      condition,
			      condition,
			      error_status) ;
      if (! error_status) {
	Draw::Repaint();
	}
      else {
	di << "Not enought degree of freedom increase degree please\n";
      }
      
      return 0;
    }
  }
  else {
    Handle(Geom2d_BSplineCurve) G2 = DrawTrSurf::GetBSplineCurve2d(a[1]);
    if (!G2.IsNull()) {
      tx = Draw::Atof(a[5]) ;
      ty = Draw::Atof(a[6]) ;
      if (n == 8) {
	condition = Max(Draw::Atoi(a[7]), -1)  ;
	condition = Min(condition, G2->Degree()-1) ;
      }
      gp_Pnt2d p;
      gp_Vec2d tangent ;
      p.SetCoord(x,y);
      tangent.SetCoord(tx,ty) ;
      
      G2->MovePointAndTangent(u, 
			      p, 
			      tangent,
			      tolerance,
			      condition,
			      condition,
			      error_status) ;
      if (! error_status) {
	Draw::Repaint();
	}
      else {
	di << "Not enought degree of freedom increase degree please\n";
      }
      
      return 0;
    }
   } 
  return 0;
}


//=======================================================================
//function : cmovepoint
//purpose  : 
//=======================================================================

static Standard_Integer cmovepoint (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Standard_Real dx = Draw::Atof(a[3]);
  Standard_Real dy = Draw::Atof(a[4]);
  Standard_Real dz=0;
  if (n >= 6 && n != 7) dz = Draw::Atof(a[5]);
  Standard_Real u = Draw::Atof(a[2]);
  Standard_Integer index1 = 0;
  Standard_Integer index2 = 0;
  Standard_Integer fmodif, lmodif;
  if (n == 7) {
    index1 = Draw::Atoi(a[5]);
    index2 = Draw::Atoi(a[6]);
  }
  else if (n == 8) {
    index1 = Draw::Atoi(a[6]);
    index2 = Draw::Atoi(a[7]);
  }

  Handle(Geom_BSplineCurve) G2 = DrawTrSurf::GetBSplineCurve(a[1]);
  if (!G2.IsNull()) {
    if (index1 == 0) {
      index1 = 2;
      index2 = G2->NbPoles()-1;
    }
    gp_Pnt p;
    G2->D0(u, p);
    p.SetCoord(p.X()+dx, p.Y()+dy, p.Z()+dz);
    G2->MovePoint(u, p, index1, index2, fmodif, lmodif);
    Draw::Repaint();
    return 0;
  }

  Handle(Geom2d_BSplineCurve) G4 = DrawTrSurf::GetBSplineCurve2d(a[1]);
  if (!G4.IsNull()) {
    if (index1 == 0) {
      index1 = 2;
      index2 = G4->NbPoles()-1;
    }
    gp_Pnt2d p;
    G4->D0(u, p);
    p.SetCoord(p.X()+dx, p.Y()+dy);
    G4->MovePoint(u, p, index1, index2, fmodif, lmodif);
    Draw::Repaint();
    return 0;
  }
  return 0;
}

//=======================================================================
//function : cinsertknot
//purpose  : 
//=======================================================================

static Standard_Integer cinsertknot (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  Handle(Geom_BSplineCurve) GBs = DrawTrSurf::GetBSplineCurve(a[1]);
  Handle(Geom2d_BSplineCurve) GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (GBs.IsNull() && GBs2d.IsNull()) return 1;

  if (n <= 4) {
    Standard_Real    knot = Draw::Atof(a[2]);
    Standard_Integer mult = 1;
    if (n == 4) mult = Draw::Atoi(a[3]);
    if (!GBs.IsNull())
      GBs->InsertKnot(knot,mult,Precision::PConfusion());
    else
      GBs2d->InsertKnot(knot,mult,Precision::PConfusion());
  }

  else {
    // multiple insertion
    if (n % 2 != 0) return 1;
    Standard_Integer i,nbk = (n-2) / 2;
    TColStd_Array1OfReal    knots(1,nbk);
    TColStd_Array1OfInteger mults(1,nbk);
    for (i = 2; i < n; i += 2) {
      knots(i/2) = Draw::Atof(a[i]);
      mults(i/2) = Draw::Atoi(a[i+1]);
    }

    if (!GBs.IsNull())
      GBs->InsertKnots(knots,mults,Precision::PConfusion());
    else
      GBs2d->InsertKnots(knots,mults,Precision::PConfusion());

  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : csetknot
//purpose  : 
//=======================================================================

static Standard_Integer csetknot (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  Handle(Geom_BSplineCurve) GBs = DrawTrSurf::GetBSplineCurve(a[1]);
  Handle(Geom2d_BSplineCurve) GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (GBs.IsNull() && GBs2d.IsNull()) return 1;

  Standard_Integer index = Draw::Atoi(a[2]);
  Standard_Real    knot  = Draw::Atof(a[3]);

  if ( n == 4) {
    if (!GBs.IsNull())
      GBs->SetKnot(index,knot);
    else
      GBs2d->SetKnot(index,knot);
  }
  else {
    Standard_Integer mult  = Draw::Atoi(a[4]);
    if (!GBs.IsNull())
      GBs->SetKnot(index,knot,mult);
    else
      GBs2d->SetKnot(index,knot,mult);
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : cremknot
//purpose  : 
//=======================================================================

static Standard_Integer cremknot (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(Geom_BSplineCurve) GBs = DrawTrSurf::GetBSplineCurve(a[1]);
  Handle(Geom2d_BSplineCurve) GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (GBs.IsNull() && GBs2d.IsNull()) return 1;

  Standard_Integer index = Draw::Atoi(a[2]);
  Standard_Integer mult  = 0;
  if (n >= 4) mult = Draw::Atoi(a[3]);

  Standard_Real tol = RealLast();
  if (n >= 5) tol = Draw::Atof(a[4]);

  if (!GBs.IsNull()) {
    if (!GBs->RemoveKnot(index,mult,tol))
      di << "Remove knots failed\n";
  }
  else {
    if (!GBs2d->RemoveKnot(index,mult,tol))
      di << "Remove knots failed\n";
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : increasedegree
//purpose  : 
//=======================================================================

static Standard_Integer increasedegree (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Standard_Integer Deg = Draw::Atoi(a[2]);

  Handle(Geom_BezierCurve) GBz = DrawTrSurf::GetBezierCurve(a[1]);
  Handle(Geom_BSplineCurve) GBs = DrawTrSurf::GetBSplineCurve(a[1]);
  Handle(Geom2d_BezierCurve) GBz2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  Handle(Geom2d_BSplineCurve) GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

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

//=======================================================================
//function : removepole
//purpose  : 
//=======================================================================

static Standard_Integer removepole (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Standard_Integer Index = Draw::Atoi(a[2]);

  Handle(Geom_BezierCurve)   GBZ   = DrawTrSurf::GetBezierCurve(a[1]);
  Handle(Geom2d_BezierCurve) GBZ2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  if (!GBZ.IsNull()) {
    GBZ->RemovePole(Index);
  }
  else if (!GBZ2d.IsNull()) {
    GBZ2d->RemovePole(Index);
  }
  else {
    di << "rempole needs a bezier curve";
    return 1;
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : insertpole
//purpose  : 
//=======================================================================

static Standard_Integer insertpole (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 6) return 1;

  Standard_Integer Index = Draw::Atoi(a[2]);

  Handle(Geom_BezierCurve)   GBZ   = DrawTrSurf::GetBezierCurve(a[1]);
  Handle(Geom2d_BezierCurve) GBZ2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  if (!GBZ.IsNull()) {
    gp_Pnt P (Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
    if ( n == 7)
      GBZ->InsertPoleAfter(Index,P,Draw::Atof(a[6]));
    else
      GBZ->InsertPoleAfter(Index,P);
  }
  else if (!GBZ2d.IsNull()) {
    gp_Pnt2d P (Draw::Atof(a[3]),Draw::Atof(a[4]));
    if ( n == 6)
      GBZ2d->InsertPoleAfter(Index,P,Draw::Atof(a[5]));
    else
      GBZ2d->InsertPoleAfter(Index,P);
  }
  else {
    di << "insertpole needs a bezier curve";
    return 1;
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : cfindp
//purpose  : 
//=======================================================================

static Standard_Integer cfindp (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 6) return 1;

  Standard_Integer Index = 0;
  Standard_Integer view = Draw::Atoi(a[2]);
  Standard_Real x = Draw::Atof(a[3]);
  Standard_Real y = Draw::Atof(a[4]);

  Draw_Display d = dout.MakeDisplay(view);

  Handle(Draw_Drawable3D) D = Draw::Get(a[1]);

  Handle(DrawTrSurf_BezierCurve) DBz = 
    Handle(DrawTrSurf_BezierCurve)::DownCast(D);
  if( !DBz.IsNull())
    DBz->FindPole( x, y, d, 5, Index);
  else {
    Handle(DrawTrSurf_BSplineCurve) DBs = 
      Handle(DrawTrSurf_BSplineCurve)::DownCast(D);
    if (!DBs.IsNull())
      DBs->FindPole( x, y, d, 5, Index);
    else {
      Handle(DrawTrSurf_BezierCurve2d) DBz2d = 
	Handle(DrawTrSurf_BezierCurve2d)::DownCast(D);
      if( !DBz2d.IsNull())
	DBz2d->FindPole( x, y, d, 5, Index);
      else {
	Handle(DrawTrSurf_BSplineCurve2d) DBs2d = 
	  Handle(DrawTrSurf_BSplineCurve2d)::DownCast(D);
	if (!DBs2d.IsNull())
	  DBs2d->FindPole( x, y, d, 5, Index);
	else 
	  return 1;
      }
    }
  }
  
  Draw::Set(a[5],Index);
  
  return 0;
}


//=======================================================================
//function : csetperiodic
//purpose  : 
//=======================================================================

static Standard_Integer csetperiodic (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Handle(Geom_BSplineCurve) GBs = DrawTrSurf::GetBSplineCurve(a[1]);
  Handle(Geom2d_BSplineCurve) GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);
  
  if (GBs.IsNull() && GBs2d.IsNull()) return 1;
  
  if (!strcmp(a[0],"setperiodic")) {
    if (!GBs.IsNull()) 
      GBs->SetPeriodic();
    else
      GBs2d->SetPeriodic();
  }
  else if (!strcmp(a[0],"setnotperiodic")) {
    if (!GBs.IsNull()) 
      GBs->SetNotPeriodic();
    else
      GBs2d->SetNotPeriodic();
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : value
//purpose  : 
//=======================================================================

static Standard_Integer value (Draw_Interpretor& ,
			       Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[1]);
  if (GC.IsNull()) return 1;

  Standard_Real U = Draw::Atof(a[2]);

  Standard_Boolean DrawPoint = ( n%3 == 1);
  if ( DrawPoint) n--;

  gp_Pnt P;
  if (n > 6) {
    if (n < 9) return 1;
    gp_Vec D1;
    if (n > 9) {
      if (n < 12) return 1;
      gp_Vec D2;
      GC->D2(U,P,D1,D2);
      Draw::Set(a[9] ,D2.X());
      Draw::Set(a[10],D2.Y());
      Draw::Set(a[11],D2.Z());
    }
    else 
      GC->D1(U,P,D1);
    Draw::Set(a[6],D1.X());
    Draw::Set(a[7],D1.Y());
    Draw::Set(a[8],D1.Z());
  }
  else 
    GC->D0(U,P);

  if ( n > 3) {
    Draw::Set(a[3],P.X());
    Draw::Set(a[4],P.Y());
    Draw::Set(a[5],P.Z());
  }
  if ( DrawPoint) {
    DrawTrSurf::Set(a[n],P);
  }
  
  return 0;
}


//=======================================================================
//function : value2d
//purpose  : 
//=======================================================================

static Standard_Integer value2d (Draw_Interpretor& , 
				 Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(a[1]);
  if (GC.IsNull()) return 1;

  Standard_Real U = Draw::Atof(a[2]);

  Standard_Boolean DrawPoint = ( n%2 == 0);
  if ( DrawPoint ) n--;

  gp_Pnt2d P;
  if (n > 5) {
    if (n < 7) return 1;
    gp_Vec2d D1;
    if (n > 7) {
      if (n < 9) return 1;
      gp_Vec2d D2;
      GC->D2(U,P,D1,D2);
      Draw::Set(a[7] ,D2.X());
      Draw::Set(a[8],D2.Y());
    }
    else 
      GC->D1(U,P,D1);
    Draw::Set(a[5],D1.X());
    Draw::Set(a[6],D1.Y());
  }
  else 
    GC->D0(U,P);

  if ( n > 3 ) {
    Draw::Set(a[3],P.X());
    Draw::Set(a[4],P.Y());
  }
  if ( DrawPoint ) {
    DrawTrSurf::Set(a[n], P);
  }
  
  return 0;
}

//=======================================================================
//function : segment
//purpose  : 
//=======================================================================

static Standard_Integer segment (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  Handle(Geom_BezierCurve) GBz = DrawTrSurf::GetBezierCurve(a[1]);
  Handle(Geom_BSplineCurve) GBs = DrawTrSurf::GetBSplineCurve(a[1]);
  Handle(Geom2d_BezierCurve) GBz2d = DrawTrSurf::GetBezierCurve2d(a[1]);
  Handle(Geom2d_BSplineCurve) GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  Standard_Real f = Draw::Atof(a[2]), l = Draw::Atof(a[3]);

  if (!GBz.IsNull()) 
    GBz->Segment(f,l);
  else if (!GBs.IsNull())
    GBs->Segment(f,l);
  else if (!GBz2d.IsNull()) 
    GBz2d->Segment(f,l);
  else if (!GBs2d.IsNull())
    GBs2d->Segment(f,l);
  else
    return 1;

  Draw::Repaint();
  return 0;
}


//=======================================================================
//function : setorigin
//purpose  : 
//=======================================================================

static Standard_Integer setorigin (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(Geom_BSplineCurve) GBs = DrawTrSurf::GetBSplineCurve(a[1]);
  Handle(Geom2d_BSplineCurve) GBs2d = DrawTrSurf::GetBSplineCurve2d(a[1]);

  if (!GBs.IsNull())
    GBs->SetOrigin(Draw::Atoi(a[2])); 
  if (!GBs2d.IsNull())
    GBs2d->SetOrigin(Draw::Atoi(a[2])); 
  else
    return 1;

  Draw::Repaint();
  return 0;
}


//=======================================================================
//function : point
//purpose  : 
//=======================================================================

static Standard_Integer point(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 4) return 1;
  if (n >= 5) {
    gp_Pnt P(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]));
    DrawTrSurf::Set(a[1],P);
  }
  else {
    gp_Pnt2d P(Draw::Atof(a[2]),Draw::Atof(a[3]));
    DrawTrSurf::Set(a[1],P);
  }

  return 0;
}

//=======================================================================
//function : coord
//purpose  : 
//=======================================================================

static Standard_Integer coord(Draw_Interpretor&,
			      Standard_Integer n, const char** a) 
{
  if ( n == 4) {
    gp_Pnt2d P;
    if ( !DrawTrSurf::GetPoint2d(a[1],P)) return 1;
    Draw::Set(a[2],P.X());
    Draw::Set(a[3],P.Y());
  }
  else if ( n == 5) {
    gp_Pnt  P;
    if ( !DrawTrSurf::GetPoint(a[1],P)) return 1;
    Draw::Set(a[2],P.X());
    Draw::Set(a[3],P.Y());
    Draw::Set(a[4],P.Z());
  }
  else 
    return 1;

  return 0;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
static Standard_Integer minmaxcurandinf(Draw_Interpretor& di, 
				   Standard_Integer argc, const char** argv)
{
  if (argc < 2) return 1;

  Handle(Geom2d_Curve)  C1 = DrawTrSurf::GetCurve2d(argv[1]);
  if (C1.IsNull()) return 1;

  Draw_Color              Couleur;
  Geom2dLProp_CurAndInf2d Sommets;  

  Sommets.PerformCurExt (C1);
  if (Sommets.IsDone() && !Sommets.IsEmpty()) {
    for (Standard_Integer i = 1; i <= Sommets.NbPoints(); i++){      
      Couleur = Draw_vert;
      if (Sommets.Type(i) == LProp_MinCur) {
	Couleur = Draw_orange;
	di << "  Maximum of curvature at U ="<<Sommets.Parameter(i)<<"\n";
      }
      else {
	di << "  Minimum of curvature at U ="<<Sommets.Parameter(i)<<"\n";
      }
      gp_Pnt2d P = C1->Value(Sommets.Parameter(i));
      Handle(Draw_Marker2D) dr = new Draw_Marker2D(P,Draw_Plus,Couleur); 
      dout << dr;
    }
    dout.Flush();
  }
   
  Geom2dLProp_CurAndInf2d Sommets2;
  Sommets2.PerformInf (C1);
  
  if (Sommets2.IsDone() && !Sommets2.IsEmpty()) {
    for (Standard_Integer i = 1; i <= Sommets2.NbPoints(); i++){
      gp_Pnt2d P = C1->Value(Sommets2.Parameter(i));
      Handle(Draw_Marker2D) dr = new Draw_Marker2D(P,Draw_Plus,Draw_bleu); 
      dout << dr;
      di << "  Inflexion at U ="<<Sommets2.Parameter(i)<<"\n";
    }
    dout.Flush();
  }
  return 0;
}
//=======================================================================
//function :  shcurvature
//purpose  :  affiche le peigne de courbure
//=======================================================================
static Standard_Integer shcurvature(Draw_Interpretor&, 
				   Standard_Integer argc, const char** argv)
{
  if (argc < 2) return 1;

   Handle(DrawTrSurf_Curve2d) C2d =  Handle(DrawTrSurf_Curve2d)
                                  ::DownCast(Draw::Get(argv[1]));
   Handle(DrawTrSurf_Curve) C3d =  Handle(DrawTrSurf_Curve)
                                  ::DownCast(Draw::Get(argv[1]));
   
  if (C2d.IsNull()) {
    if (C3d.IsNull()) return 1;
    C3d->ShowCurvature();
  }
  else {
    C2d->ShowCurvature();
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
//function :  clcurvature
//purpose  :  efface le peigne de courbure
//=======================================================================
static Standard_Integer clcurvature(Draw_Interpretor&, 
				   Standard_Integer argc, const char** argv)
{
  if (argc < 2) return 1; 
   Handle(DrawTrSurf_Curve2d) C2d =  Handle(DrawTrSurf_Curve2d)
                                  ::DownCast(Draw::Get(argv[1]));
   Handle(DrawTrSurf_Curve) C3d =  Handle(DrawTrSurf_Curve)
                                  ::DownCast(Draw::Get(argv[1]));

  if (C2d.IsNull()) {
    if (C3d.IsNull()) return 1;
    C3d->ClearCurvature();
  }
  else {
    C2d->ClearCurvature();
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
//function :  radiusmax
//purpose  :  definit le rayon de courbure maximum a afficher
//=======================================================================
static Standard_Integer radiusmax(Draw_Interpretor&, 
				  Standard_Integer argc, const char** argv)
{
  if (argc < 3) return 1; 
  Standard_Real Radius = Draw::Atof(argv[2]);
   Handle(DrawTrSurf_Curve2d) C2d =  Handle(DrawTrSurf_Curve2d)
                                  ::DownCast(Draw::Get(argv[1]));
   Handle(DrawTrSurf_Curve) C3d =  Handle(DrawTrSurf_Curve)
                                  ::DownCast(Draw::Get(argv[1]));

  if (C2d.IsNull()) {
    if (C3d.IsNull()) return 1;
    C3d->SetRadiusMax(Radius);
  }
  else {
    C2d->SetRadiusMax(Radius);
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
//function :  radiusratio
//purpose  :  definit le ratio du rayon de courbure a afficher
//=======================================================================
static Standard_Integer radiusratio(Draw_Interpretor&, 
				  Standard_Integer argc, const char** argv)
{
  if (argc < 3) return 1;
  Standard_Real Ratio = Draw::Atof(argv[2]);
  Handle(DrawTrSurf_Curve2d) C2d =  Handle(DrawTrSurf_Curve2d)
                                  ::DownCast(Draw::Get(argv[1]));
  Handle(DrawTrSurf_Curve) C3d =  Handle(DrawTrSurf_Curve)
                                  ::DownCast(Draw::Get(argv[1]));

  if (C2d.IsNull()) { 
    if (C3d.IsNull()) return 1;
    C3d->SetRadiusRatio(Ratio);
  }
  else {
    C2d->SetRadiusRatio(Ratio);
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
static Standard_Integer localprop(Draw_Interpretor& di, 
				  Standard_Integer argc, const char** argv)
{
  if (argc < 3) return 1;

  Standard_Real U =  Draw::Atof(argv[2]);

  Handle(Geom2d_Curve)  C2d = DrawTrSurf::GetCurve2d(argv[1]);
  Handle(Geom_Curve)    C3d;


  if (C2d.IsNull()) {
    C3d = DrawTrSurf::GetCurve(argv[1]);
    if (C3d.IsNull()) return 1;
    GeomLProp_CLProps Prop (C3d,2,Precision::Confusion());
    Prop.SetParameter(U);
    Handle(Draw_Marker3D)drp = new Draw_Marker3D(Prop.Value(),
						 Draw_Plus,
						 Draw_vert );
    dout << drp;
    if (Prop.IsTangentDefined()) {
      Standard_Real K = Prop.Curvature();
      di <<" Curvature : "<<K<<"\n";

      if (Abs(K) > Precision::Confusion()) {
	Standard_Real R = 1/Abs(K);
	gp_Pnt        Center;
	Prop.CentreOfCurvature(Center);
	gp_Dir Tang;
	gp_Dir Nor;
	Prop.Tangent(Tang);
	Prop.Normal(Nor);
	gp_Dir AxC = Nor^Tang;
	gp_Ax2 Axe(Center,AxC,Nor);
	Handle(Geom_Circle) Cir3d = new Geom_Circle(Axe,R);
	Handle(DrawTrSurf_Curve) dr;
	dr = new DrawTrSurf_Curve(Cir3d);
	dout << dr;
	dout.Flush();
      }
    }
    else 
      di <<"Tangent undefined.\n";  
  }
  else {
    Geom2dLProp_CLProps2d Prop (C2d,2,Precision::Confusion());
    Prop.SetParameter(U);
    Handle(Draw_Marker2D) drp = new Draw_Marker2D(Prop.Value(),
						  Draw_Plus,
						  Draw_vert);
    dout << drp;
    if (Prop.IsTangentDefined()) {
      Standard_Real K = Prop.Curvature();
      gp_Pnt2d      Center;

      di <<" Curvature : "<<K<<"\n";

      if (Abs(K) > Precision::Confusion()) {
	Standard_Real R = 1/Abs(K);
	Prop.CentreOfCurvature(Center);
	gp_Ax2d Axe(Center,gp::DX2d());
	Handle(Geom2d_Circle) Cir2d = new Geom2d_Circle(Axe,R);
	Handle(DrawTrSurf_Curve2d) dr;
	dr = new DrawTrSurf_Curve2d(Cir2d,Draw_rouge,30,Standard_False);
	dout << dr;
	dout.Flush();
      }
    }
    else 
      di <<"Tangent undefined.\n";
  }
  return 0;
}  
//=======================================================================
//function : rawcont
//purpose  : 
//=======================================================================

static Standard_Integer rawcont(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Handle(Geom_Curve) GC1;
  GC1 = DrawTrSurf::GetCurve(a[1]);
  Handle(Geom_Curve) GC2;
  GC2 = DrawTrSurf::GetCurve(a[2]);
  Standard_Real param1 =
    Draw::Atof(a[3]) ;
  Standard_Real param2 =
    Draw::Atof(a[4]) ;
  if (GC1.IsNull() || GC2.IsNull())
    return 1;
  gp_Pnt a_point1,
    a_point2 ;
  GC1->D0(param1,
	  a_point1) ;
  GC2->D0(param2,
	  a_point2) ;
  if (a_point2.SquareDistance(a_point1) < Precision::Confusion()) {
    GeomAbs_Shape cont =
      GeomLProp::Continuity(GC1,
			    GC2,
			    param1,
			    param2,
			    Standard_True,
			    Standard_True,
			    Precision::Confusion(),
			    Precision::Angular()) ;
    switch (cont) {
    case GeomAbs_C0: 
      di << " C0 Continuity \n" ;
      break ;
    case GeomAbs_G1:
      di << " G1 Continuity \n" ;
      break ;
    case GeomAbs_C1 :
      di << " C1 Continuity \n" ;
      break ;
    case GeomAbs_G2 :
      di << " G2 Continuity \n" ;
      break ; 
    case GeomAbs_C2 :
      di << " C2 Continuity \n" ;
      break ; 
    case GeomAbs_C3 :
      di << " C3 Continuity \n" ;
      break ; 
    case GeomAbs_CN :
      di << " CN Continuity \n" ;
      break ; 
    default:
      break ; 
    }
  }
  else {
    di << " not C0 continuity \n" ;
  }
  return 0 ;
}
//=======================================================================
//function : approxcurveonsurf
//purpose  : 
//=======================================================================
static Standard_Integer approxcurveonsurf(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  Standard_Real Tol = 1.e-7;              // Tolerance (default 0.1mm) 
  GeomAbs_Shape Continuity = GeomAbs_C1;  // Continuity order : 0, 1 or 2 (default 1)
  Standard_Integer MaxDeg = 14;           // Maximum degree
  Standard_Integer MaxSeg = 16; /*1*/          // Maximum number of segments

  if ( n>8 || n<4) return 1;

  if (n>4) Tol = Max(Draw::Atof(a[4]),1.e-10);

  if (n>5) {
    if (Draw::Atoi(a[5]) == 0) Continuity = GeomAbs_C0;
    if (Draw::Atoi(a[5]) == 2) Continuity = GeomAbs_C2;
  }

  if (n>6) {
    MaxDeg = Draw::Atoi(a[6]);
    if (MaxDeg<1 || MaxDeg>14) MaxDeg = 14;
  }

  if (n>7) MaxSeg = Draw::Atoi(a[7]);
  Handle(Geom2d_Curve) curve2d = DrawTrSurf::GetCurve2d(a[2]);
  Handle(Geom_Surface) Surf = DrawTrSurf::GetSurface(a[3]);

  Handle(Geom2dAdaptor_HCurve) A2d = new (Geom2dAdaptor_HCurve)(curve2d);
  Handle(GeomAdaptor_HSurface) AS = new (GeomAdaptor_HSurface)(Surf);

  Approx_CurveOnSurface App(A2d, AS, A2d->FirstParameter(), A2d->LastParameter(),
			    Tol,  Continuity,  MaxDeg, MaxSeg,
			    Standard_True, Standard_False);

  if(App.HasResult()) {
    Handle(Geom_BSplineCurve) BSCurve = App.Curve3d(); 
    DrawTrSurf::Set(a[1], BSCurve);
    return 0;
  }

  di << "Approximation failed !\n";
  return 1;
			    
}

//=======================================================================
//function : approxcurve
//purpose  : 
//=======================================================================
static Standard_Integer approxcurve(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ 
  Standard_Real Tol = 1.e-7;              // Tolerance (default 0.1mm) 
  GeomAbs_Shape Continuity = GeomAbs_C1;  // Continuity order : 0, 1 or 2 (default 1)
  Standard_Integer MaxDeg = 14;           // Maximum degree
  Standard_Integer MaxSeg = 16;           // Maximum number of segments
  
  Standard_Integer Case, shift;
// Case == 1 : 3d approximation without reparametrization
// Case == 2 : 2d approximation without reparametrization
// Case == 3 : 3d approximation with reparametrization
// Case == 4 : curve_on_surface approximation with reparametrization
// Case == 5 : 2 curves_on_surfaces approximation with reparametrization

  Handle(Geom_Curve) curve;
  Handle(Geom2d_Curve) curve2d, curve2d2;
  Handle(Geom_Surface) surface, surface2;

  if(n < 2) return 1;

  if (!strcmp(a[1],"-L")) {
// aproximation with curvilinear abscissa reparametrization
    if (n > 11 || n < 4) return 1;
    Tol = 1.e-4;
    curve = DrawTrSurf::GetCurve(a[3]);
    if (!curve.IsNull()) {
      shift = 4;
      Case = 3;
    }
    else {
// approx curve_on_surface
      if (n < 5) return 1;
      curve2d = DrawTrSurf::GetCurve2d(a[3]); 
      surface = DrawTrSurf::GetSurface(a[4]);
      if (curve2d.IsNull() || surface.IsNull()) {
	return 1;
      }
      if (n >= 7) {
	curve2d2 = DrawTrSurf::GetCurve2d(a[5]); 
	surface2 = DrawTrSurf::GetSurface(a[6]);
	if (curve2d2.IsNull() || surface2.IsNull()) {
	  shift = 5;
	  Case = 4;
	}
	else {
// approx 2 curves_on_surfaces
	  shift = 7;
	  Case = 5;
	}
      }
      else {
	shift = 5;
	Case = 4;
      }
    }
  }
  else {
// aproximation without reparamitrization
    if ( n>7 || n<3) return 1;
    shift = 3;
    curve = DrawTrSurf::GetCurve(a[2]);
    if (curve.IsNull()) {
      curve2d = DrawTrSurf::GetCurve2d(a[2]); 
      if (curve2d.IsNull()) {
	return 1;
      }
      Case = 2;
    }
    else
      Case = 1;
  }
  
  if (n>shift) Tol = Max(Draw::Atof(a[shift]),1.e-10);
  
  if (n>shift+1) {
    if (Draw::Atoi(a[shift+1]) == 0) Continuity = GeomAbs_C0;
    if (Draw::Atoi(a[shift+1]) == 2) Continuity = GeomAbs_C2;
  }
  
  if (n>shift+2) {
      MaxDeg = Draw::Atoi(a[shift+2]);
      if (MaxDeg<1 || MaxDeg>14) MaxDeg = 14;
    }
  
  if (n>shift+3) MaxSeg = Draw::Atoi(a[shift+3]);
    
  if (Case == 1) {
    GeomConvert_ApproxCurve appr(curve, Tol, Continuity, MaxSeg, MaxDeg);
    if(appr.HasResult()) {
      //appr.Dump(cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      Handle(Geom_BSplineCurve) BSCurve = appr.Curve(); 
      DrawTrSurf::Set(a[1], BSCurve);
    }
 }   

  else if (Case == 2) {
    Geom2dConvert_ApproxCurve appr(curve2d, Tol, Continuity, MaxSeg, MaxDeg);
    if(appr.HasResult()) {
      //appr.Dump(cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      Handle(Geom2d_BSplineCurve) BSCurve = appr.Curve(); 
      DrawTrSurf::Set(a[1], BSCurve);
    }
  }    

  else if (Case == 3) {
    Handle(Adaptor3d_HCurve) HACur = new GeomAdaptor_HCurve(curve);
    Approx_CurvilinearParameter appr(HACur, Tol, Continuity, MaxDeg, MaxSeg);
    if(appr.HasResult()) {
      //appr.Dump(cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      Handle(Geom_BSplineCurve) BSCurve = appr.Curve3d(); 
      DrawTrSurf::Set(a[2], BSCurve);
    }
}    
  else if (Case == 4) {
    Handle(Adaptor2d_HCurve2d) HACur2d = new Geom2dAdaptor_HCurve(curve2d);
    Handle(Adaptor3d_HSurface) HASur = new GeomAdaptor_HSurface(surface);
    Approx_CurvilinearParameter appr(HACur2d, HASur, Tol, Continuity, MaxDeg, MaxSeg);
    if(appr.HasResult()) {
      //appr.Dump(cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      Handle(Geom_BSplineCurve) BSCurve = appr.Curve3d(); 
      DrawTrSurf::Set(a[2], BSCurve);
    }
  }

  else if (Case == 5) {
    Handle(Adaptor2d_HCurve2d) HACur2d = new Geom2dAdaptor_HCurve(curve2d);
    Handle(Adaptor3d_HSurface) HASur = new GeomAdaptor_HSurface(surface);
    Handle(Adaptor2d_HCurve2d) HACur2d2 = new Geom2dAdaptor_HCurve(curve2d2);
    Handle(Adaptor3d_HSurface) HASur2 = new GeomAdaptor_HSurface(surface2);
    Approx_CurvilinearParameter appr(HACur2d, HASur, HACur2d2, HASur2, Tol, Continuity, MaxDeg, MaxSeg);
    if(appr.HasResult()) {
      //appr.Dump(cout);
      Standard_SStream aSStream;
      appr.Dump(aSStream);
      di << aSStream;
      Handle(Geom_BSplineCurve) BSCurve = appr.Curve3d(); 
      DrawTrSurf::Set(a[2], BSCurve);
    }
  }
  

  return 0;
}

//=======================================================================
//function : newbspline
//purpose  : reduce the multiplicity of the knots to their minimum 
//           compared to the degree of the curve
//=======================================================================

static Standard_Integer splitc1(Draw_Interpretor& di,
				   Standard_Integer n, const char** c)

{Standard_Real      tolerance=1.0e-5,
   angular_tolerance = 1.0e-4 ;
 Standard_Integer   optiontab,i;
 char name[100];

 if (n<3) return 1;
 optiontab=Draw::Atoi(c[2]);
 if (n >= 4 )
   tolerance=Draw::Atof(c[3]);
 if (n >= 5) {
   angular_tolerance = Draw::Atof(c[4]) ;
 }
 Handle(Geom_Curve) ACurve = Handle(Geom_Curve)::DownCast(DrawTrSurf::Get(c[1])) ;
 
 Standard_Real f = ACurve->FirstParameter();
 Standard_Real l = ACurve->LastParameter();

 if ( Precision::IsInfinite(f) || Precision::IsInfinite(l)) {
   di << " Error: Infinite curves\n";
   return 1;
 }

 Handle(Geom_BSplineCurve) BS = GeomConvert::CurveToBSplineCurve(ACurve);
 
 if ( BS.IsNull()) return 1;

 if (optiontab){
   Handle(TColGeom_HArray1OfBSplineCurve)  tabBS;
   GeomConvert::C0BSplineToArrayOfC1BSplineCurve(BS,
						 tabBS,
						 angular_tolerance,
						 tolerance);
   for (i=0;i<=(tabBS->Length()-1);i++){
     Sprintf(name,"%s_%d",c[1],i+1);
     Standard_CString new_name = name ;
     DrawTrSurf::Set(new_name,
                     tabBS->Value(i));
     di.AppendElement(name);
   }
 }
 else{
   GeomConvert::C0BSplineToC1BSplineCurve(BS,
					  tolerance,
					  angular_tolerance) ;

   DrawTrSurf::Set(c[1],BS);
 }
 return 0;
}

//=======================================================================
//function : splitc12d
//purpose  : reduce the multiplicity of the knots to their minimum 
//           compared to the degree of the curve
//=======================================================================

static Standard_Integer splitc12d(Draw_Interpretor& di,
				     Standard_Integer n, const char** c)

{Standard_Real      tolerance=1.0e-5,
   angular_tolerance = 1.0e-4 ;
 Standard_Integer   optiontab,i;
 char name[100];

 if (n<3) return 1;
 optiontab=Draw::Atoi(c[2]);
 if (n==4)
   tolerance=Draw::Atof(c[3]);
 if (n==5) 
   angular_tolerance = Draw::Atof(c[4]) ;
 Handle(Geom2d_Curve) ACurve = DrawTrSurf::GetCurve2d(c[1]);
 
 Standard_Real f = ACurve->FirstParameter();
 Standard_Real l = ACurve->LastParameter();

 if ( Precision::IsInfinite(f) || Precision::IsInfinite(l)) {
   di << " Error: Infinite curves\n";
   return 1;
 }

 Handle(Geom2d_BSplineCurve) BS = Geom2dConvert::CurveToBSplineCurve(ACurve);
 
 if ( BS.IsNull()) return 1;

 if (optiontab){
   Handle(TColGeom2d_HArray1OfBSplineCurve)  tabBS;
   Geom2dConvert::C0BSplineToArrayOfC1BSplineCurve(BS,
						   tabBS,
						   angular_tolerance,
						   tolerance);
   for (i=0;i<=(tabBS->Length()-1);i++){
     Sprintf(name,"%s_%d",c[1],i+1);
     Standard_CString new_name = name ;
     DrawTrSurf::Set(new_name,
		     tabBS->Value(i));
     di.AppendElement(name);
   }
 }
 else{
   Geom2dConvert::C0BSplineToC1BSplineCurve(BS,tolerance);
   DrawTrSurf::Set(c[1],BS);
 }
 return 0;
}

//=======================================================================
//function : canceldenom
//purpose  : set the value of the denominator cancel its first 
//           derivative on the boundaries of the surface if possible
//=======================================================================

static Standard_Integer canceldenom(Draw_Interpretor& ,
				     Standard_Integer n, const char** c)

{Standard_Integer    uoption,voption;
 Standard_Boolean    udirection=Standard_False;
 Standard_Boolean    vdirection=Standard_False;
 if (n<4) return 1;
 uoption=Draw::Atoi(c[2]);
 voption=Draw::Atoi(c[3]);
 if (uoption)
   udirection=Standard_True;
 if (voption)
   vdirection=Standard_True;
 Handle(Geom_BSplineSurface) BSurf = DrawTrSurf::GetBSplineSurface(c[1]);
 GeomLib::CancelDenominatorDerivative(BSurf,udirection,vdirection);
 DrawTrSurf::Set(c[1],BSurf);
 return 0;
}

//=======================================================================
//function : length
//purpose  : eval curve's length
//=======================================================================

static Standard_Integer length(Draw_Interpretor& di,
			       Standard_Integer n, const char** a)
{
  if (n<2) return 1;
  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[1]);
  Handle(Geom2d_Curve) GC2d = DrawTrSurf::GetCurve2d(a[1]);
  Standard_Real Tol = Precision::Confusion(), L;
  if (n==3) Tol = Draw::Atof(a[2]);

  if (!GC.IsNull()) {
    GeomAdaptor_Curve AC(GC);
    L = GCPnts_AbscissaPoint::Length(AC, Tol);
  }  
  else if (!GC2d.IsNull()) {
    Geom2dAdaptor_Curve AC(GC2d);
    L = GCPnts_AbscissaPoint::Length(AC, Tol);
  }
  else {
    di << a[1] << "is not a curve\n";
    return 1;
  }

  di << "The length " << a[1] << " is " << L << "\n";
  return 0;
}



//=======================================================================
//function : CurveCommands
//purpose  : 
//=======================================================================

void  GeomliteTest::CurveCommands(Draw_Interpretor& theCommands)
{
  
  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;
  
  DrawTrSurf::BasicCommands(theCommands);
  
  const char* g;
  
  
  // analytic curves
  g = "GEOMETRY curves creation";


  theCommands.Add("point",
		  "point name x y [z]",
		  __FILE__,
		  point ,g);
  
  theCommands.Add("line",
		  "line name pos dir",
		  __FILE__,
		  anacurve,g);
  
  theCommands.Add("circle",
		  "circle name x y [z [dx dy dz]] [ux uy [uz]] radius",
		  __FILE__,
		  anacurve,g);
  
  theCommands.Add("ellipse",
		  "ellipse name x y [z [dx dy dz]] [ux uy [uz]] major minor",
		  __FILE__,
		  anacurve,g);
  theCommands.Add("parabola",
		  "parabola name x y [z [dx dy dz]] [ux uy [uz]] focal",
		  __FILE__,
		  anacurve,g);
  theCommands.Add("hyperbola",
		  "hyperbola name x y [z [dx dy dz]] [ux uy [uz]] major minor",
		  __FILE__,
		  anacurve,g);

  theCommands.Add("beziercurve",
		  "beziercurve name nbpole pole, [weight]",
		  __FILE__,
		  polecurve,g);

  theCommands.Add("bsplinecurve",
		  "bsplinecurve name degree nbknots  knot, umult  pole, weight",
		  __FILE__,
		  polecurve,g);

  theCommands.Add("pbsplinecurve",
		  "pbsplinecurve name degree nbknots  knot, umult  pole, weight (periodic)",
		  __FILE__,
		  polecurve,g);

  theCommands.Add("2dbeziercurve",
		  "2dbeziercurve name nbpole pole, [weight]",
		  __FILE__,
		  polecurve2d,g);

  theCommands.Add("2dbsplinecurve",
		  "2dbsplinecurve name degree nbknots  knot, umult  pole, weight",
		  __FILE__,
		  polecurve2d,g);

  theCommands.Add("2dpbsplinecurve",
		  "2dpbsplinecurve name degree nbknots  knot, umult  pole, weight (periodic)",
		  __FILE__,
		  polecurve2d,g);

  g = "GEOMETRY Curves and Surfaces modification";

  theCommands.Add("reverse",
		  "reverse name ... ",
		  __FILE__,
		  reverse,g);

  theCommands.Add("cmovep",
		  "cmovep name index dx dy dz",
		  __FILE__,
		  cmovepole,g);

  theCommands.Add("cmovepoint",
		  "cmovepoint name u dx dy [dz index1 index2]",
		  __FILE__,
		  cmovepoint,g);

  theCommands.Add("cmovetangent",
                  "cmovetangent name u  x y [z] tx ty [tz constraint = 0]",
		  __FILE__,
		  cmovetangent,g) ;

  theCommands.Add("insertknot",
		  "insertknot name knot [mult = 1] [knot mult ...]",
		  __FILE__,
		  cinsertknot,g);

  theCommands.Add("setknot",
		  "setknot name index knot [mult]",
		  __FILE__,
		  csetknot,g);

  theCommands.Add("remknot",
		  "remknot name index [mult] [tol]",
		  __FILE__,
		  cremknot,g);

  theCommands.Add("incdeg",
		  "incdeg name degree",
		  __FILE__,
		  increasedegree,g);

  theCommands.Add("rempole",
		  "rempole name index",
		  __FILE__,
		  removepole,g);

  theCommands.Add("insertpole",
		  "insertpole name index x y [z] [weight]",
		  __FILE__,
		  insertpole,g);

  theCommands.Add("cfindp",
		  "cfindp name view x y index",
		  __FILE__,
		  cfindp,g);

  theCommands.Add("setperiodic",
		  "setperiodic name ...",
		  __FILE__,
		  csetperiodic,g);

  theCommands.Add("setnotperiodic",
		  "setnotperiodic name",
		  __FILE__,
		  csetperiodic,g);

  theCommands.Add("segment",
		  "segment name Ufirst Ulast",
		   __FILE__,
		  segment , g);

  theCommands.Add("setorigin",
		  "setorigin name knotindex",
		   __FILE__,
		  setorigin , g);

  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("cvalue",
		  "cvalue curvename U  X Y Z [D1X D1Y D1Z D2X D2Y D2Z]",
		  __FILE__,
		  value,g);

  theCommands.Add("2dcvalue",
		  "2dcvalue curvename U  X Y [D1X D1Y D2X D2Y]",
		  __FILE__,
		  value2d,g);

  theCommands.Add("coord",
		  "coord P x y [z]: set in x y [z] the coordinates of P",
		  __FILE__,
		  coord,g);

  theCommands.Add("minmaxcurandinf",
		  "minmaxcurandinf curve",
		  __FILE__,
		  minmaxcurandinf,g);
  
  theCommands.Add("shcurvature",
		  "shcurvature curvename",
		  __FILE__,
		  shcurvature,g);

  theCommands.Add("clcurvature",
		  "clcurvature curvename",
		  __FILE__,
		  clcurvature,g);
  

 theCommands.Add("radiusmax",
		  "radiusmax curvename  radius",
		  __FILE__,
		  radiusmax,g);

 theCommands.Add("radiusratio",
		  "radiusratio curvename ratio",
		  __FILE__,
		  radiusratio,g); 
  theCommands.Add("localprop",
		  "localprop curvename U",
		  __FILE__,
		  localprop,g);
  theCommands.Add("rawcont",
		  "rawcont curve1 curve2 u1 u2",
		  __FILE__,
		  rawcont,g) ;
  theCommands.Add("approxcurve",
                  "approxcurve [-L] name curve1 [Surf1] [curve2d2 Surf2] [Tol [cont [maxdeg [maxseg]]]] ",
		  __FILE__,
		  approxcurve,g);

  theCommands.Add("approxcurveonsurf",
                  "approxcurveonsurf name curve2d surface [Tol [cont [maxdeg [maxseg]]]] ",
		  __FILE__,
		  approxcurveonsurf,g);

 theCommands.Add("length", "length curve [Tol]", 
		  __FILE__,
		  length, g);


  theCommands.Add("splitc1",
		  "splitc1 bspline resultinarray(0/1) [tol] [angtol] ",
		  __FILE__,
		  splitc1,g);

  theCommands.Add("splitc12d",
		  "splitc12d bspline2d resultinarray(0/1) [tol] [angtol] ",
		  __FILE__,
		  splitc12d,g);
  theCommands.Add("canceldenom",
		  "canceldenom BSpline-Surface UDirection(0/1) VDirection(0/1)",
		  __FILE__,
		  canceldenom,g);

 

}
