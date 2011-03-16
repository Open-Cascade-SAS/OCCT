// File:	Geom2d_Geometry.cxx
// Created:	Wed Mar 24 19:22:26 1993
// Author:	JCV
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom2d_Geometry.cxx, JCV 23/06/91

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
