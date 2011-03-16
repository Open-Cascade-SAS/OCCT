// File:	Geom_Geometry.cxx
// Created:	Wed Mar 10 09:45:02 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


//File Geom_Geometry.cxx, JCV 15/01/91
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


