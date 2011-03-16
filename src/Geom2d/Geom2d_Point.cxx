// File:	Geom2d_Point.cxx
// Created:	Wed Mar 24 19:27:37 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom2d_Point.cxx, JCV 23/06/91

#include <Geom2d_Point.ixx>

typedef Geom2d_Point Point;
typedef Handle(Geom2d_Point) Handle(Point);


Standard_Real Geom2d_Point::Distance (const Handle(Point)& Other) const {

  gp_Pnt2d P1 = this-> Pnt2d ();
  gp_Pnt2d P2 = Other->Pnt2d ();
  return P1.Distance (P2);
}


Standard_Real Geom2d_Point::SquareDistance (const Handle(Point)& Other) const {

  gp_Pnt2d P1 = this-> Pnt2d ();
  gp_Pnt2d P2 = Other->Pnt2d ();
  return P1.SquareDistance (P2);
}
