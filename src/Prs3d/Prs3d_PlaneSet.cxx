// Created on: 1993-10-20
// Created by: Isabelle VERDURON
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



#include <Prs3d_PlaneSet.ixx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>

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

