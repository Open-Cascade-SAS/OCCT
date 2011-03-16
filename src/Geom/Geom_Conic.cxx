// File:	Geom_Conic.cxx
// Created:	Wed Mar 10 09:31:40 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Conic.cx, JCV 17/01/91

#include <Geom_Conic.ixx>

typedef Geom_Conic         Conic;
typedef Handle(Geom_Conic) Handle(Conic);
typedef gp_Ax1 Ax1;
typedef gp_Ax2 Ax2;
typedef gp_Pnt Pnt;
typedef gp_Vec Vec;



void  Geom_Conic::Reverse () {

  gp_Dir Vz = pos.Direction ();
  Vz.Reverse();
  pos.SetDirection (Vz);
}

void Geom_Conic::SetAxis (const Ax1& A1) {  pos.SetAxis (A1); }

void Geom_Conic::SetLocation (const Pnt& O) { pos.SetLocation (O); }

void Geom_Conic::SetPosition (const Ax2& A2) { pos = A2; }

Ax1 Geom_Conic::Axis () const { return pos.Axis(); }

GeomAbs_Shape Geom_Conic::Continuity () const { return GeomAbs_CN; }

Pnt Geom_Conic::Location () const { return pos.Location(); }

const gp_Ax2& Geom_Conic::Position () const { return pos; }

Ax1 Geom_Conic::XAxis () const {return Ax1(pos.Location(), pos.XDirection());}

Ax1 Geom_Conic::YAxis () const {return Ax1(pos.Location(), pos.YDirection());}

Standard_Boolean Geom_Conic::IsCN (const Standard_Integer ) const { return Standard_True; }

