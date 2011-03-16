// File:	Geom_ElementarySurface.cxx
// Created:	Wed Mar 10 09:42:08 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_ElementarySurface.cxx, JCV 17/01/91

#include <Geom_ElementarySurface.ixx>



typedef Geom_ElementarySurface         ElementarySurface;
typedef Handle(Geom_ElementarySurface) Handle(ElementarySurface);
typedef Handle(Geom_Surface)           Handle(Surface);

typedef gp_Ax1 Ax1;
typedef gp_Ax2 Ax2;
typedef gp_Ax3 Ax3;
typedef gp_Dir Dir;
typedef gp_Pnt Pnt;
typedef gp_Vec Vec;




//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_ElementarySurface::Continuity () const {

  return GeomAbs_CN; 
}

//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ElementarySurface::IsCNu (const Standard_Integer ) const {

  return Standard_True; 
}

//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ElementarySurface::IsCNv (const Standard_Integer ) const {

  return Standard_True; 
}

//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

Ax1 Geom_ElementarySurface::Axis () const {

  return pos.Axis(); 
}

//=======================================================================
//function : SetAxis
//purpose  : 
//=======================================================================

void Geom_ElementarySurface::SetAxis (const Ax1& A1) { 

  pos.SetAxis (A1); 
}

//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

Pnt Geom_ElementarySurface::Location () const {

  return pos.Location(); 
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

const gp_Ax3& Geom_ElementarySurface::Position () const {

  return pos; 
}

//=======================================================================
//function : SetPosition
//purpose  : 
//=======================================================================

void Geom_ElementarySurface::SetPosition (const Ax3& A3) {

  pos = A3; 
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Geom_ElementarySurface::SetLocation (const Pnt& Loc) { 

  pos.SetLocation (Loc);
}


//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_ElementarySurface::UReverse () {

  pos.YReverse();
}



//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_ElementarySurface::VReverse () {

  pos.ZReverse();
}


