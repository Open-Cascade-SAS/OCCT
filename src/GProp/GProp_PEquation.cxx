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


#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GProp_PEquation.hxx>
#include <GProp_PGProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <Standard_NoSuchObject.hxx>

GProp_PEquation::GProp_PEquation(const TColgp_Array1OfPnt& Pnts, 
				       const Standard_Real Tol) 
{
  GProp_PGProps Pmat(Pnts);
  g = Pmat.CentreOfMass(); 
  Standard_Real Xg,Yg,Zg;
  g.Coord(Xg,Yg,Zg);
  GProp_PrincipalProps Pp = Pmat.PrincipalProperties();
  gp_Vec V1 = Pp.FirstAxisOfInertia();
  Standard_Real Xv1,Yv1,Zv1;
  V1.Coord(Xv1,Yv1,Zv1); 
  gp_Vec V2 = Pp.SecondAxisOfInertia(); 
  Standard_Real Xv2,Yv2,Zv2;
  V2.Coord(Xv2,Yv2,Zv2);
  gp_Vec V3 = Pp.ThirdAxisOfInertia(); 
  Standard_Real Xv3,Yv3,Zv3;
  V3.Coord(Xv3,Yv3,Zv3);
  Standard_Real D,X,Y,Z;
  Standard_Real Dmx1 = RealFirst();
  Standard_Real Dmn1 = RealLast();
  Standard_Real Dmx2 = RealFirst();
  Standard_Real Dmn2 = RealLast();
  Standard_Real Dmx3 = RealFirst();
  Standard_Real Dmn3 = RealLast();

  for (Standard_Integer i = Pnts.Lower(); i <= Pnts.Upper();i++){
    Pnts(i).Coord(X,Y,Z);
    D = (X-Xg)*Xv1 +(Y-Yg)*Yv1 + (Z-Zg)*Zv1;
    if (D > Dmx1) Dmx1 = D;
    if (D < Dmn1) Dmn1 = D;
    D = (X-Xg)*Xv2 +(Y-Yg)*Yv2 + (Z-Zg)*Zv2;
    if (D > Dmx2) Dmx2 = D;
    if (D < Dmn2) Dmn2 = D;
    D = (X-Xg)*Xv3 +(Y-Yg)*Yv3 + (Z-Zg)*Zv3;
    if (D > Dmx3) Dmx3 = D;
    if (D < Dmn3) Dmn3 = D;
  }
  Standard_Integer dimension= 3 ;
  Standard_Integer It = 0;
  if (Abs(Dmx1-Dmn1) <= Tol)  {
    dimension =dimension-1;
    It =1;
  }
  if (Abs(Dmx2-Dmn2) <= Tol)  {
    dimension =dimension-1;
    It =2*(It+1);
  }
  if (Abs(Dmx3-Dmn3) <= Tol)  {
    dimension =dimension-1;
    It = 3*(It+1);
  }
  switch (dimension)  {
  case 0:
    {
      type = GProp_Point;
      break;
    }
  case 1:
    {
      type = GProp_Line;
      if (It == 4) v1 = V3;
      else if (It == 6) v1 = V2;
      else v1 = V1; 
      break;
    }
  case 2:
    {
      type = GProp_Plane;
      if (It == 1) v1 = V1;
      else if (It == 2) v1 =V2;
      else v1 = V3;
      break;
    }
  case 3:
    {
      type = GProp_Space;
      g.SetXYZ(g.XYZ() + Dmn1*V1.XYZ() + Dmn2*V2.XYZ() + Dmn3*V3.XYZ());
      v1 = (Dmx1-Dmn1)*V1;      
      v2 = (Dmx2-Dmn2)*V2;
      v3 = (Dmx3-Dmn3)*V3;
      break;
    }
  }
}
Standard_Boolean  GProp_PEquation::IsPlanar() const {

  if (type == GProp_Plane) return Standard_True;
  else  return Standard_False;
}

Standard_Boolean  GProp_PEquation::IsLinear() const {

  if (type == GProp_Line) return Standard_True;
  else  return Standard_False;
}

Standard_Boolean  GProp_PEquation::IsPoint() const {

  if (type == GProp_Point) return Standard_True;
  else  return Standard_False;
}

Standard_Boolean GProp_PEquation::IsSpace() const {
  if (type == GProp_Space) return Standard_True;
  else  return Standard_False;
}

gp_Pln  GProp_PEquation::Plane() const {
  if (!IsPlanar()) throw Standard_NoSuchObject();
  return gp_Pln(g,v1);
}
gp_Lin  GProp_PEquation::Line() const {
  if (!IsLinear()) throw Standard_NoSuchObject();
  return gp_Lin(g,gp_Dir(v1));  
}

gp_Pnt  GProp_PEquation::Point() const {
  if (!IsPoint()) throw Standard_NoSuchObject();
  return g;
}

void GProp_PEquation::Box(gp_Pnt& P  , gp_Vec& V1,
			     gp_Vec& V2 , gp_Vec& V3) const {
  if (!IsSpace()) throw Standard_NoSuchObject();
  P = g;
  V1 = v1;
  V2 = v2;
  V3 = v3;
}
