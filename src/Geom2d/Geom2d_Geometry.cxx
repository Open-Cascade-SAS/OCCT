// Created on: 1993-03-24
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



#include <Geom2d_Geometry.ixx>
#include <Standard_ConstructionError.hxx>

typedef Handle(Geom2d_Geometry) Handle(Geometry);
typedef Geom2d_Geometry Geometry;
typedef gp_Ax2d   Ax2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Vec2d  Vec2d;
typedef gp_Trsf2d Trsf2d;



void Geom2d_Geometry::Mirror (const gp_Pnt2d& P) {
   
  Trsf2d T;
  T.SetMirror (P);
  Transform (T);
}



void Geom2d_Geometry::Mirror (const gp_Ax2d& A) {

  Trsf2d T;
  T.SetMirror (A);
  Transform (T);
}


void Geom2d_Geometry::Rotate (const gp_Pnt2d& P, const Standard_Real Ang) {

  Trsf2d T;
  T.SetRotation (P, Ang);
  Transform (T);
}


void Geom2d_Geometry::Scale (const gp_Pnt2d& P, const Standard_Real S) {

  Trsf2d T;
  T.SetScale (P, S);
  Transform (T);
}


void Geom2d_Geometry::Translate (const gp_Vec2d& V) {

  Trsf2d T;
  T.SetTranslation (V);
  Transform (T);
}


void Geom2d_Geometry::Translate (const gp_Pnt2d& P1, const gp_Pnt2d& P2) {

  Vec2d V (P1, P2);
  Translate (V);
}


 Handle(Geometry) Geom2d_Geometry::Mirrored (const gp_Pnt2d& P) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Mirror (P);
  return G;
}


Handle(Geometry) Geom2d_Geometry::Mirrored (const gp_Ax2d& A) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Mirror (A);
  return G;
}


Handle(Geometry) Geom2d_Geometry::Rotated (
const gp_Pnt2d& P, const Standard_Real Ang) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Rotate (P, Ang);
  return G;
}


Handle(Geometry) Geom2d_Geometry::Scaled (
const gp_Pnt2d& P,  const Standard_Real S) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Scale (P, S);
  return G;
}


Handle(Geometry) Geom2d_Geometry::Transformed (const gp_Trsf2d& T) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Transform (T);
  return G;
}


Handle(Geometry) Geom2d_Geometry::Translated (const gp_Vec2d& V) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Translate (V);
  return G;
}


Handle(Geometry) Geom2d_Geometry::Translated (
const gp_Pnt2d& P1, const gp_Pnt2d& P2) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Translate (P1, P2);
  return G;
}
