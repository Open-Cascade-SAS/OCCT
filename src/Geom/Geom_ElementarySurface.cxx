// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Geom_ElementarySurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_ElementarySurface,Geom_Surface)

typedef Geom_ElementarySurface         ElementarySurface;
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


