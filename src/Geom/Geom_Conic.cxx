// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

