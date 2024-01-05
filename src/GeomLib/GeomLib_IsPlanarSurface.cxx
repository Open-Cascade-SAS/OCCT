// Created on: 1998-11-23
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Pln.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>


static Standard_Boolean Controle(const TColgp_Array1OfPnt& Poles,
				 const Standard_Real Tol,
				 const Handle(Geom_Surface)& S,
				 gp_Pln& Plan) 
{
  Standard_Boolean IsPlan = Standard_False;
  Standard_Real gx,gy,gz;
  gp_Pnt Bary;
  gp_Dir DX, DY;
  Standard_Real aTolSingular = Precision::Confusion();

    
  GeomLib::Inertia(Poles, Bary, DX, DY, gx, gy, gz);
  if (gz < Tol && gy > aTolSingular) {
    gp_Pnt P;
    gp_Vec DU, DV;
    Standard_Real umin, umax, vmin, vmax;
    S->Bounds(umin, umax, vmin, vmax);
    S->D1((umin + umax) / 2, (vmin + vmax) / 2, P, DU, DV);
    
    if (DU.SquareMagnitude() > gp::Resolution() &&
        DV.SquareMagnitude() > gp::Resolution())
    {
      // On prend DX le plus proche possible de DU
      gp_Dir du(DU);
      Standard_Real Angle1 = du.Angle(DX);
      Standard_Real Angle2 = du.Angle(DY);
      if (Angle1 > M_PI / 2) Angle1 = M_PI - Angle1;
      if (Angle2 > M_PI / 2) Angle2 = M_PI - Angle2;
      if (Angle2 < Angle1) {
        du = DY; DY = DX; DX = du;
      }
      if (DX.Angle(DU) > M_PI / 2) DX.Reverse();
      if (DY.Angle(DV) > M_PI / 2) DY.Reverse();

      gp_Ax3 axe(Bary, DX^DY, DX);
      Plan.SetPosition(axe);
      Plan.SetLocation(Bary);
      IsPlan = Standard_True;
    }
  }
  return IsPlan;
}

static Standard_Boolean Controle(const Handle(Geom_Curve)& C,
			  const gp_Pln& Plan,
			  const Standard_Real Tol) 
{
  Standard_Boolean B = Standard_True;
  Standard_Integer ii, Nb;
  GeomAbs_CurveType Type;
  GeomAdaptor_Curve AC(C);
  Type = AC.GetType();

  switch (Type) {
  case GeomAbs_Line :
    {
      Nb = 2;
      break;
    }
  case GeomAbs_Circle:
    {
      Nb = 3;
      break; 
    }
    
  case GeomAbs_Ellipse:
  case GeomAbs_Hyperbola:
  case GeomAbs_Parabola:
    {
      Nb = 5;
      break; 
    }
  case GeomAbs_BezierCurve:
    {
      Nb =  AC.NbPoles();
      break; 
    }
  case GeomAbs_BSplineCurve:
    {
      Nb =  AC.NbPoles();
      break; 
    }
  default :
    {
	 Nb = 8 + 3*AC.NbIntervals(GeomAbs_CN);
    }
  }
 
  Standard_Real u, du, f, l, d;
  f = AC.FirstParameter();
  l = AC.LastParameter();
  du = (l - f) / (Nb - 1);
  for (ii = 1; ii <= Nb && B; ii++) {
    u = (ii - 1)*du + f;
    d = Plan.Distance(C->Value(u));
    B = d < Tol;
  }

  return B;
}



GeomLib_IsPlanarSurface::GeomLib_IsPlanarSurface(const Handle(Geom_Surface)& S,
						 const Standard_Real Tol)
						
{
  GeomAdaptor_Surface AS(S);
  GeomAbs_SurfaceType Type;

  Type = AS.GetType();

  switch (Type) {
  case GeomAbs_Plane :
    {
      IsPlan = Standard_True;
      myPlan = AS.Plane();
      break;
    }
  case GeomAbs_Cylinder :
  case GeomAbs_Cone :
  case GeomAbs_Sphere :
  case GeomAbs_Torus :
    {
     IsPlan = Standard_False;
     break;
    }

  case GeomAbs_SurfaceOfRevolution :
    {
      Standard_Boolean Essai = Standard_True;
      gp_Pnt P;
      gp_Vec DU, DV, Dn;
      gp_Dir Dir = AS.AxeOfRevolution().Direction();
      Standard_Real Umin, Umax, Vmin, Vmax;
      S->Bounds(Umin, Umax, Vmin, Vmax);
      S->D1((Umin+Umax)/2, (Vmin+Vmax)/2, P, DU, DV);
      if (DU.Magnitude() <= gp::Resolution() ||
          DV.Magnitude() <= gp::Resolution())
      {
        Standard_Real NewU = (Umin+Umax)/2 + (Umax-Umin)*0.1;
        Standard_Real NewV = (Vmin+Vmax)/2 + (Vmax-Vmin)*0.1;
        S->D1( NewU, NewV, P, DU, DV );
      }
      Dn = DU^DV;
      if (Dn.Magnitude() > 1.e-7) {
	Standard_Real angle = Dir.Angle(Dn);
	if (angle > M_PI/2) {
	  angle = M_PI - angle;
	  Dir.Reverse();
	}
	Essai = (angle < 0.1);
      }

      if (Essai) {
	gp_Ax3 axe(P, Dir);
	axe.SetXDirection(DU);
	myPlan.SetPosition(axe);
	myPlan.SetLocation(P);
	Handle(Geom_Curve) C;
	C = S->UIso(Umin);
	IsPlan = Controle(C,  myPlan, Tol);
      }
      else 
	IsPlan = Standard_False;

      break;
    }
  case GeomAbs_SurfaceOfExtrusion :
    {
      Standard_Boolean Essai = Standard_False;
      Standard_Real Umin, Umax, Vmin, Vmax;
      Standard_Real norm;
      gp_Vec Du, Dv, Dn;
      gp_Pnt P;

      S->Bounds(Umin, Umax, Vmin, Vmax);
      S->D1((Umin+Umax)/2, (Vmin+Vmax)/2, P, Du, Dv);
      if (Du.Magnitude() <= gp::Resolution() ||
          Dv.Magnitude() <= gp::Resolution())
      {
        Standard_Real NewU = (Umin+Umax)/2 + (Umax-Umin)*0.1;
        Standard_Real NewV = (Vmin+Vmax)/2 + (Vmax-Vmin)*0.1;
        S->D1( NewU, NewV, P, Du, Dv );
      }
      Dn = Du^Dv;
      norm = Dn.Magnitude();
      if (norm > 1.e-15) {
	Dn /= norm;
	Standard_Real angmax = Tol / (Vmax-Vmin);
	gp_Dir D(Dn);
	Essai = (D.IsNormal(AS.Direction(), angmax));
      }
      if (Essai) {
	gp_Ax3 axe(P, Dn, Du);
	myPlan.SetPosition(axe);
	myPlan.SetLocation(P);
	Handle(Geom_Curve) C;
	C = S->VIso((Vmin+Vmax)/2);
	IsPlan = Controle(C,  myPlan, Tol);
      }
      else 
	IsPlan = Standard_False;
      break;
    }

  default :
      {
	Standard_Integer NbU,NbV, ii, jj, kk; 
	NbU = 8 + 3*AS.NbUIntervals(GeomAbs_CN);
	NbV = 8 + 3*AS.NbVIntervals(GeomAbs_CN);
        Standard_Real Umin, Umax, Vmin, Vmax, du, dv, U, V;
	S->Bounds(Umin, Umax, Vmin, Vmax);
	du = (Umax-Umin)/(NbU-1);
	dv = (Vmax-Vmin)/(NbV-1);
	TColgp_Array1OfPnt Pnts(1, NbU*NbV);
	for(ii=0, kk=1; ii<NbU; ii++) {
	  U = Umin + du*ii;
	  for(jj=0; jj<NbV; jj++,kk++) {
	    V = Vmin + dv*jj;
	    S->D0(U,V, Pnts(kk));
	  }
	}

	IsPlan =  Controle(Pnts, Tol, S, myPlan);
      }
    }
}

 Standard_Boolean GeomLib_IsPlanarSurface::IsPlanar() const
{
  return IsPlan;
}

 const gp_Pln& GeomLib_IsPlanarSurface::Plan() const
{
  if (!IsPlan) throw StdFail_NotDone(" GeomLib_IsPlanarSurface");
  return myPlan;
}
