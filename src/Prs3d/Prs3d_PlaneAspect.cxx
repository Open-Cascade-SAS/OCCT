// Copyright (c) 1995-1999 Matra Datavision
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

#include <Prs3d_PlaneAspect.ixx>

Prs3d_PlaneAspect::Prs3d_PlaneAspect()
{
  myEdgesAspect = new Prs3d_LineAspect(Quantity_NOC_GREEN,Aspect_TOL_SOLID,1.);
  myIsoAspect = new Prs3d_LineAspect(Quantity_NOC_GRAY75,Aspect_TOL_SOLID,0.5);
  myArrowAspect = new Prs3d_LineAspect(Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID,1.);
  myDrawCenterArrow = Standard_False;
  myDrawEdgesArrows = Standard_False;
  myDrawEdges = Standard_True;
  myDrawIso = Standard_False;
  myIsoDistance = 0.5;
  myPlaneXLength= 1.;
  myPlaneYLength= 1.;
  myArrowsLength= 0.02;
  myArrowsSize=0.1;
  myArrowsAngle=M_PI/8.;
}

Handle(Prs3d_LineAspect) Prs3d_PlaneAspect::EdgesAspect() const
{
  return myEdgesAspect;
}

Handle(Prs3d_LineAspect) Prs3d_PlaneAspect::IsoAspect() const
{
  return myIsoAspect;
}

Handle(Prs3d_LineAspect) Prs3d_PlaneAspect::ArrowAspect() const
{
  return myArrowAspect;
}

void Prs3d_PlaneAspect::SetDisplayCenterArrow(const Standard_Boolean draw)
{
  myDrawCenterArrow = draw;
}

void Prs3d_PlaneAspect::SetDisplayEdgesArrows(const Standard_Boolean draw)
{
  myDrawEdgesArrows = draw;
}

void Prs3d_PlaneAspect::SetDisplayEdges(const Standard_Boolean draw)
{
  myDrawEdges = draw;
}

void Prs3d_PlaneAspect::SetDisplayIso(const Standard_Boolean draw)
{
  myDrawIso = draw;
}

Standard_Boolean Prs3d_PlaneAspect::DisplayCenterArrow() const
{
  return myDrawCenterArrow;
}

Standard_Boolean Prs3d_PlaneAspect::DisplayEdgesArrows() const
{
  return myDrawEdgesArrows;
}

Standard_Boolean Prs3d_PlaneAspect::DisplayEdges() const
{
  return myDrawEdges;
}

Standard_Boolean Prs3d_PlaneAspect::DisplayIso() const
{
  return myDrawIso;
}

void Prs3d_PlaneAspect::SetPlaneLength(const Quantity_Length lX,
				       const Quantity_Length lY)
{
  myPlaneXLength = lX;
  myPlaneYLength = lY;
}

Quantity_Length Prs3d_PlaneAspect::PlaneXLength() const
{
  return myPlaneXLength;
}

Quantity_Length Prs3d_PlaneAspect::PlaneYLength() const
{
  return myPlaneYLength;
}

void Prs3d_PlaneAspect::SetIsoDistance(const Quantity_Length l) 
{
  myIsoDistance = l;
}

Quantity_Length Prs3d_PlaneAspect::IsoDistance() const
{
  return myIsoDistance;
}

void Prs3d_PlaneAspect::SetArrowsLength(const Quantity_Length L)
{
  myArrowsLength = L;
}

Quantity_Length Prs3d_PlaneAspect::ArrowsLength() const
{
  return myArrowsLength;
}

void Prs3d_PlaneAspect::SetArrowsSize(const Quantity_Length L)
{
  myArrowsSize = L;
}

Quantity_Length Prs3d_PlaneAspect::ArrowsSize() const
{
  return myArrowsSize;
}

void Prs3d_PlaneAspect::SetArrowsAngle(const Quantity_PlaneAngle ang)
{
  myArrowsAngle = ang;
}

Quantity_Length Prs3d_PlaneAspect::ArrowsAngle() const
{
  return myArrowsAngle;
}












