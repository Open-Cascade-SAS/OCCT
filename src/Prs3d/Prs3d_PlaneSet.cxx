// Created on: 1993-10-20
// Created by: Isabelle VERDURON
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


#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_PlaneSet.hxx>
#include <Standard_Type.hxx>

Prs3d_PlaneSet::Prs3d_PlaneSet(const Standard_Real Xdir,
			       const Standard_Real Ydir,
			       const Standard_Real Zdir,
			       const Standard_Real Xloc,
			       const Standard_Real Yloc,
			       const Standard_Real Zloc,
			       const Quantity_Length anOffset)

     : myPlane(gp_Pln(gp_Pnt(Xloc,Yloc,Zloc),gp_Dir(Xdir,Ydir,Zdir))),
       myOffset(anOffset) {
}


void Prs3d_PlaneSet::SetDirection(const Standard_Real X,
			      const Standard_Real Y,
			      const Standard_Real Z) {

  myPlane = gp_Pln(myPlane.Location(),gp_Dir(X,Y,Z));
}

void Prs3d_PlaneSet::SetLocation(const Standard_Real X,
			      const Standard_Real Y,
			      const Standard_Real Z) {

  myPlane.SetLocation(gp_Pnt(X,Y,Z));
}

void Prs3d_PlaneSet::SetOffset(const Quantity_Length anOffset) {

  myOffset = anOffset;
}

gp_Pln Prs3d_PlaneSet::Plane() const {

  return myPlane;

}
Quantity_Length Prs3d_PlaneSet::Offset () const {

  return myOffset;

}

void Prs3d_PlaneSet::Location(Quantity_Length& X,Quantity_Length& Y,Quantity_Length& Z) const {

  myPlane.Location().Coord(X,Y,Z);
}

void Prs3d_PlaneSet::Direction(Quantity_Length& X,Quantity_Length& Y,Quantity_Length& Z) const {

  myPlane.Axis().Direction().Coord(X,Y,Z);
}

