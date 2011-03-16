// File:	Geom2d_Transformation.cxx
// Created:	Wed Mar 24 19:30:53 1993
// Author:	JCV
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom2d_Transformation.cxx, JCV 23/06/91

#include <Geom2d_Transformation.ixx>


typedef Geom2d_Transformation         Transformation;
typedef Handle(Geom2d_Transformation) Handle(Transformation);

typedef gp_Ax2d     Ax2d;
typedef gp_Pnt2d    Pnt2d;
typedef gp_TrsfForm TrsfForm;
typedef gp_Vec2d    Vec2d;








Handle(Geom2d_Transformation) Geom2d_Transformation::Copy() const {

  Handle(Transformation) T;
  T = new Transformation (gpTrsf2d);
  return T; 
}




Geom2d_Transformation::Geom2d_Transformation () { }


Geom2d_Transformation::Geom2d_Transformation (const gp_Trsf2d& T) 
:  gpTrsf2d (T) { }


Handle(Transformation) Geom2d_Transformation::Inverted () const {

   return new Transformation (gpTrsf2d.Inverted());
}


Handle(Transformation) Geom2d_Transformation::Multiplied (

const Handle(Transformation)& Other) const {

   return new Transformation (gpTrsf2d.Multiplied (Other->Trsf2d()));
}


Handle(Transformation) Geom2d_Transformation::Powered (const Standard_Integer N) const{

  gp_Trsf2d Temp = gpTrsf2d;
  Temp.Power (N);
  return new Transformation (Temp);
}



void Geom2d_Transformation::SetMirror (const gp_Pnt2d& P) {

  gpTrsf2d.SetMirror (P);
}


void Geom2d_Transformation::SetMirror (const gp_Ax2d& A) {

  gpTrsf2d.SetMirror (A);
}


void Geom2d_Transformation::SetRotation (const gp_Pnt2d& P, const Standard_Real Ang) { 

  gpTrsf2d.SetRotation (P, Ang); 
}


void Geom2d_Transformation::SetScale (const gp_Pnt2d& P, const Standard_Real S) { 

  gpTrsf2d.SetScale (P, S);
}


void Geom2d_Transformation::SetTransformation (const gp_Ax2d& ToAxis) { 

  gpTrsf2d.SetTransformation (ToAxis);
}


void Geom2d_Transformation::SetTransformation (
const gp_Ax2d& FromAxis1, const gp_Ax2d& ToAxis2) {

  gpTrsf2d.SetTransformation (FromAxis1, ToAxis2);
}


void Geom2d_Transformation::SetTranslation (const gp_Vec2d& V) {

  gpTrsf2d.SetTranslation (V);
}


void Geom2d_Transformation::SetTranslation (const gp_Pnt2d& P1, const gp_Pnt2d& P2) {

  gpTrsf2d.SetTranslation (P1, P2); 
}


void Geom2d_Transformation::SetTrsf2d (const gp_Trsf2d& T) { gpTrsf2d = T; }


Standard_Boolean Geom2d_Transformation::IsNegative () const {

  return gpTrsf2d.IsNegative();
}


TrsfForm Geom2d_Transformation::Form () const { return gpTrsf2d.Form(); }


Standard_Real Geom2d_Transformation::ScaleFactor () const {

   return gpTrsf2d.ScaleFactor();
}


gp_Trsf2d Geom2d_Transformation::Trsf2d () const {  return gpTrsf2d; }



Standard_Real Geom2d_Transformation::Value (const Standard_Integer Row, const Standard_Integer Col) const{

  return gpTrsf2d.Value (Row, Col);
}


void Geom2d_Transformation::Invert () { gpTrsf2d.Invert(); }


void Geom2d_Transformation::Transforms (Standard_Real& X, Standard_Real& Y) const {  

  gpTrsf2d.Transforms (X, Y); 
}


void Geom2d_Transformation::Multiply (const Handle(Geom2d_Transformation)& Other) {

  gpTrsf2d.Multiply (Other->Trsf2d());
}


void Geom2d_Transformation::Power (const Standard_Integer N) { gpTrsf2d.Power (N); }


void Geom2d_Transformation::PreMultiply (
const Handle(Transformation)& Other) {

  gpTrsf2d.PreMultiply (Other->Trsf2d());
}




