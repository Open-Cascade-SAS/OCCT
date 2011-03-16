// File:	Geom_AxisPlacement.cxx
// Created:	Tue Mar  9 19:29:44 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_AxisPlacement.cxx, JCV 17/01/91

#include <Geom_AxisPlacement.ixx>

typedef Handle(Geom_AxisPlacement) Handle(AxisPlacement);
typedef gp_Ax1 Ax1;
typedef gp_Dir Dir;
typedef gp_Pnt Pnt;
typedef gp_Vec Vec;



const gp_Ax1& Geom_AxisPlacement::Axis () const { return axis; }

Dir Geom_AxisPlacement::Direction () const { return axis.Direction(); }

Pnt Geom_AxisPlacement::Location () const  { return axis.Location(); }

void Geom_AxisPlacement::SetAxis (const Ax1& A1) { axis = A1; }

void Geom_AxisPlacement::SetLocation (const Pnt& P) {axis.SetLocation (P);}

Standard_Real Geom_AxisPlacement::Angle (const Handle(AxisPlacement)& Other) const {
 return axis.Angle (Other->Axis());
}


