// File:	Prs3d_PlaneSet.cxx
// Created:	Wed Oct 20 13:04:21 1993
// Author:	Isabelle VERDURON
//		<isa@stylox>


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

