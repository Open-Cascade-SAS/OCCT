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


