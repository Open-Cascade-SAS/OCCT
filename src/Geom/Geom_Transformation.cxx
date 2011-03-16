// File:	Geom_Transformation.cxx
// Created:	Wed Mar 10 10:59:58 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Transformation.cxx, JCV 14/01/91
#include <Geom_Transformation.ixx>

typedef Handle(Geom_Transformation) Handle(Transformation);
typedef Geom_Transformation         Transformation;
typedef gp_Ax1      Ax1;
typedef gp_Ax2      Ax2;
typedef gp_Ax3      Ax3;
typedef gp_Pnt      Pnt;
typedef gp_TrsfForm TrsfForm;
typedef gp_Vec      Vec;




Geom_Transformation::Geom_Transformation () { }


Geom_Transformation::Geom_Transformation (const gp_Trsf& T) 
: gpTrsf (T) { }


Handle(Geom_Transformation) Geom_Transformation::Copy() const {

  Handle(Transformation) T;
  T = new Transformation (gpTrsf);
  return T; 
}


void Geom_Transformation::SetMirror (const gp_Pnt& P) { gpTrsf.SetMirror (P); }

void Geom_Transformation::SetMirror (const gp_Ax1& A1) { gpTrsf.SetMirror (A1); }

void Geom_Transformation::SetMirror (const gp_Ax2& A2) { gpTrsf.SetMirror (A2);}

void Geom_Transformation::SetRotation (const gp_Ax1& A1, const Standard_Real Ang) {

  gpTrsf.SetRotation (A1, Ang);
}

void Geom_Transformation::SetScale (const gp_Pnt& P, const Standard_Real S) {

   gpTrsf.SetScale (P, S);
}


void Geom_Transformation::SetTransformation (const gp_Ax3& ToAxis) {

  gpTrsf.SetTransformation (ToAxis);
}


void Geom_Transformation::SetTransformation (
const gp_Ax3& FromAxis1, const gp_Ax3& ToAxis2) {

  gpTrsf.SetTransformation (FromAxis1, ToAxis2);
}


void Geom_Transformation::SetTranslation (const gp_Vec& V) {

   gpTrsf.SetTranslation (V);
}


void Geom_Transformation::SetTranslation (const gp_Pnt& P1, const gp_Pnt& P2) {

  gpTrsf.SetTranslation (P1, P2);
}


void Geom_Transformation::SetTrsf (const gp_Trsf& T) { gpTrsf = T; }

Standard_Boolean Geom_Transformation::IsNegative () const {return gpTrsf.IsNegative();}

TrsfForm Geom_Transformation::Form () const { return gpTrsf.Form(); }

Standard_Real Geom_Transformation::ScaleFactor () const { return gpTrsf.ScaleFactor(); }

const gp_Trsf& Geom_Transformation::Trsf () const { return gpTrsf; }

Standard_Real Geom_Transformation::Value (const Standard_Integer Row, const Standard_Integer Col) const {

   return gpTrsf.Value (Row, Col);
}


void Geom_Transformation::Invert () { gpTrsf.Invert(); }


Handle(Transformation) Geom_Transformation::Inverted () const {

   return new Transformation (gpTrsf.Inverted());
}


Handle(Transformation) Geom_Transformation::Multiplied (
const Handle(Geom_Transformation)& Other) const {

   return new Transformation (gpTrsf.Multiplied (Other->Trsf()));
}


void Geom_Transformation::Multiply (const Handle(Geom_Transformation)& Other) {

   gpTrsf.Multiply (Other->Trsf());
}


void Geom_Transformation::Power (const Standard_Integer N) { gpTrsf.Power (N); }


Handle(Transformation) Geom_Transformation::Powered (const Standard_Integer N) const {

  gp_Trsf T = gpTrsf;
  T.Power (N);  
  return new Transformation (T);
}


void Geom_Transformation::PreMultiply (const Handle(Geom_Transformation)& Other){

   gpTrsf.PreMultiply (Other->Trsf());
}


void Geom_Transformation::Transforms (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {

  gpTrsf.Transforms (X, Y, Z);
}

