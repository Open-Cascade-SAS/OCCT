// File:	Geom_Point.cxx
// Created:	Wed Mar 10 10:02:09 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Point.cxx, JCV 15/01/91

#include <Geom_Point.ixx>


typedef Handle(Geom_Point) Handle(Point);
typedef Geom_Point         Point;


Standard_Real Geom_Point::Distance (const Handle(Point)& Other) const {

  gp_Pnt P1 = this->Pnt ();
  gp_Pnt P2 = Other->Pnt ();
  return P1.Distance (P2);
}


Standard_Real Geom_Point::SquareDistance (const Handle(Point)& Other) const {

  gp_Pnt P1 = this->Pnt ();
  gp_Pnt P2 = Other->Pnt ();
  return P1.SquareDistance (P2);
}
