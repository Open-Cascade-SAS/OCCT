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



//JCV 09/07/92 portage sur C1


#include <Geom_Geometry.ixx>
#include <Standard_ConstructionError.hxx>

typedef Handle(Geom_Geometry) Handle(Geometry);
typedef Geom_Geometry         Geometry;
typedef gp_Pnt                Pnt;
typedef gp_Vec                Vec;
typedef gp_Ax1                Ax1;
typedef gp_Ax2                Ax2;
typedef gp_Trsf               Trsf;



Handle(Geom_Geometry) Geom_Geometry::Copy() const {

   Handle(Geom_Geometry) G;
   Standard_ConstructionError::Raise();
   return G;
}


void Geom_Geometry::Mirror (const gp_Pnt& P) {
   
  Trsf T;
  T.SetMirror (P);
  Transform (T);
}



void Geom_Geometry::Mirror (const gp_Ax1& A1) {

  Trsf T;
  T.SetMirror (A1);
  Transform (T);
}


void Geom_Geometry::Mirror (const gp_Ax2& A2) {

  Trsf T;
  T.SetMirror (A2);
  Transform (T);
}


void Geom_Geometry::Rotate (const gp_Ax1& A1, const Standard_Real Ang) {

  Trsf T;
  T.SetRotation (A1, Ang);
  Transform (T);
}


void Geom_Geometry::Scale (const gp_Pnt& P, const Standard_Real S) {

  Trsf T;
  T.SetScale (P, S);
  Transform (T);
}


void Geom_Geometry::Translate (const gp_Vec& V) {

  Trsf T;
  T.SetTranslation (V);
  Transform (T);
}


void Geom_Geometry::Translate (const gp_Pnt& P1, const gp_Pnt& P2) {

  Vec V (P1, P2);
  Translate (V);
}


Handle(Geometry) Geom_Geometry::Mirrored (const gp_Pnt& P) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G  = me->Copy();
  G->Mirror (P);
  return G;
}


Handle(Geometry) Geom_Geometry::Mirrored (const gp_Ax1& A1) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Mirror (A1);
  return G;
}


Handle(Geometry) Geom_Geometry::Mirrored (const gp_Ax2& A2) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G = me->Copy();
  G->Mirror (A2);
  return G;
}



Handle(Geometry) Geom_Geometry::Rotated (

const gp_Ax1& A1, 
const Standard_Real Ang
) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G  = me->Copy();
  G->Rotate (A1, Ang);
  return G;
}



Handle(Geometry) Geom_Geometry::Scaled (const gp_Pnt& P, const Standard_Real S) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G  = me->Copy();
  G->Scale (P, S);
  return G;
}



Handle(Geometry) Geom_Geometry::Transformed (const gp_Trsf& T) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G  = me->Copy();
  G->Transform (T);
  return G;
}



Handle(Geometry) Geom_Geometry::Translated (const gp_Vec& V) const {

  Handle(Geometry) me = this;
  Handle(Geometry) G  = me->Copy();
  G->Translate (V);
  return G;
}


Handle(Geometry) Geom_Geometry::Translated (

const gp_Pnt& P1,
const gp_Pnt& P2
) const {

   Handle(Geometry) me = this;
   Handle(Geometry) G  = me->Copy();
   G->Translate (P1, P2);
   return G;
}


