// File:	Geom_VectorWithMagnitude.cxx
// Created:	Wed Mar 10 11:03:38 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_VectorWithMagnitude.cxx, JCV 16/01/91

#include <Geom_VectorWithMagnitude.ixx>

typedef Geom_VectorWithMagnitude         VectorWithMagnitude;
typedef Handle(Geom_VectorWithMagnitude) Handle(VectorWithMagnitude);
typedef Geom_Vector                      Vector;
typedef Handle(Geom_Vector)              Handle(Vector);
typedef Handle(Geom_Geometry)            Handle(Geometry);
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;




//=======================================================================
//function : Geom_VectorWithMagnitude
//purpose  : 
//=======================================================================

Geom_VectorWithMagnitude::Geom_VectorWithMagnitude (const gp_Vec& V) 
{ gpVec = V; }


//=======================================================================
//function : Geom_VectorWithMagnitude
//purpose  : 
//=======================================================================

Geom_VectorWithMagnitude::Geom_VectorWithMagnitude (
const Standard_Real X,  const Standard_Real Y,  const Standard_Real Z)  { gpVec = gp_Vec (X, Y, Z); }


//=======================================================================
//function : Geom_VectorWithMagnitude
//purpose  : 
//=======================================================================

Geom_VectorWithMagnitude::Geom_VectorWithMagnitude (
const Pnt& P1, const Pnt& P2) { gpVec = gp_Vec (P1, P2); }


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_VectorWithMagnitude::Copy() const {

  Handle(VectorWithMagnitude) V;
  V = new VectorWithMagnitude (gpVec);
  return V; 
}


//=======================================================================
//function : SetCoord
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::SetCoord (
const Standard_Real X,  const Standard_Real Y,  const Standard_Real Z) { gpVec = gp_Vec (X, Y ,Z); }

//=======================================================================
//function : SetVec
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::SetVec (const gp_Vec& V) { gpVec = V; }

//=======================================================================
//function : SetX
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::SetX (const Standard_Real X) { gpVec.SetX (X); }

//=======================================================================
//function : SetY
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::SetY (const Standard_Real Y) { gpVec.SetY (Y); }

//=======================================================================
//function : SetZ
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::SetZ (const Standard_Real Z) {  gpVec.SetZ (Z); }

//=======================================================================
//function : Magnitude
//purpose  : 
//=======================================================================

Standard_Real Geom_VectorWithMagnitude::Magnitude () const {return gpVec.Magnitude ();}

//=======================================================================
//function : SquareMagnitude
//purpose  : 
//=======================================================================

Standard_Real Geom_VectorWithMagnitude::SquareMagnitude () const { 

  return gpVec.SquareMagnitude ();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::Add (const Handle(Vector)& Other) { 

  gpVec.Add (Other->Vec());
}


//=======================================================================
//function : Added
//purpose  : 
//=======================================================================

Handle(VectorWithMagnitude) Geom_VectorWithMagnitude::Added (
const Handle(Vector)& Other) const { 

   gp_Vec V1 = gpVec;
   V1.Add (Other->Vec());
   return new VectorWithMagnitude (V1);
}


//=======================================================================
//function : Cross
//purpose  : 
//=======================================================================

void  Geom_VectorWithMagnitude::Cross (const Handle(Vector)& Other) { 

  gpVec.Cross (Other->Vec());
}



//=======================================================================
//function : Crossed
//purpose  : 
//=======================================================================

Handle(Vector) Geom_VectorWithMagnitude::Crossed (
const Handle(Vector)& Other) const { 

  gp_Vec V (gpVec);
  V.Cross (Other->Vec());
  return new VectorWithMagnitude (V);
}


//=======================================================================
//function : CrossCross
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::CrossCross (
const Handle(Vector)& V1, const Handle(Vector)& V2) {

  gpVec.CrossCross (V1->Vec(), V2->Vec());
}


//=======================================================================
//function : CrossCrossed
//purpose  : 
//=======================================================================

Handle(Vector) Geom_VectorWithMagnitude::CrossCrossed (
const Handle(Vector)& V1, const Handle(Vector)& V2) const { 

  gp_Vec V (gpVec);
  V.CrossCross (V1->Vec(), V2->Vec());
  return new VectorWithMagnitude (V);
}


//=======================================================================
//function : Divide
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::Divide (const Standard_Real Scalar) { 

  gpVec.Divide (Scalar);
}


//=======================================================================
//function : Divided
//purpose  : 
//=======================================================================

Handle(VectorWithMagnitude) Geom_VectorWithMagnitude::Divided (
const Standard_Real Scalar) const { 

  gp_Vec V (gpVec);
  V.Divide (Scalar);
  return new VectorWithMagnitude (V);
}


//=======================================================================
//function : Multiplied
//purpose  : 
//=======================================================================

Handle(VectorWithMagnitude) Geom_VectorWithMagnitude::Multiplied (
const Standard_Real Scalar) const { 

  gp_Vec V (gpVec);
  V.Multiply (Scalar);
  return new VectorWithMagnitude (V);
}


//=======================================================================
//function : Multiply
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::Multiply (const Standard_Real Scalar) { 

  gpVec.Multiply (Scalar);
}


//=======================================================================
//function : Normalize
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::Normalize () { gpVec.Normalize (); }


//=======================================================================
//function : Normalized
//purpose  : 
//=======================================================================

Handle(VectorWithMagnitude) Geom_VectorWithMagnitude::Normalized () const { 

  gp_Vec V (gpVec);
  V.Normalize ();
  return new VectorWithMagnitude (V);
}


//=======================================================================
//function : Subtract
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::Subtract (const Handle(Vector)& Other) { 

  gpVec.Subtract (Other->Vec());
}


//=======================================================================
//function : Subtracted
//purpose  : 
//=======================================================================

Handle(VectorWithMagnitude) Geom_VectorWithMagnitude::Subtracted (
const Handle(Vector)& Other) const { 

  gp_Vec V (gpVec);
  V.Subtract (Other->Vec());
  return new VectorWithMagnitude (V);
}



//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_VectorWithMagnitude::Transform (const Trsf& T) { 

  gpVec.Transform (T);
}
