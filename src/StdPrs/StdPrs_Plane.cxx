// Created on: 1995-07-24
// Created by: Modelistation
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



#include <StdPrs_Plane.ixx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d.hxx>
#include <Geom_Plane.hxx>


void StdPrs_Plane::Add (const Handle (Prs3d_Presentation)& aPresentation,
		        const Adaptor3d_Surface&             aPlane,
		        const Handle (Prs3d_Drawer)&       aDrawer)
{
//  Prs3d_Root::NewGroup(aPresentation);    
  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
  if (aPlane.GetType() != GeomAbs_Plane) return;
  Handle(Geom_Plane) thegeom = new Geom_Plane(aPlane.Plane());

  Handle(Prs3d_PlaneAspect) theaspect = aDrawer->PlaneAspect();

  gp_Pnt p1;
  Standard_Real Xmax,Ymax;
  Xmax = Standard_Real(theaspect->PlaneXLength())/2.;
  Ymax = Standard_Real(theaspect->PlaneYLength())/2.;
  if (theaspect->DisplayEdges()) {
    static Graphic3d_Array1OfVertex EdgesArray(1,5);
    TheGroup->SetPrimitivesAspect(theaspect->EdgesAspect()->Aspect());
    thegeom->D0(-Xmax,Ymax,p1);
    EdgesArray(1).SetCoord(p1.X(),p1.Y(),p1.Z());
    EdgesArray(5).SetCoord(p1.X(),p1.Y(),p1.Z());
    thegeom->D0(Xmax,Ymax,p1);
    EdgesArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
    thegeom->D0(Xmax,-Ymax,p1);
    EdgesArray(3).SetCoord(p1.X(),p1.Y(),p1.Z());
    thegeom->D0(-Xmax,-Ymax,p1);
    EdgesArray(4).SetCoord(p1.X(),p1.Y(),p1.Z());
    TheGroup->Polyline(EdgesArray);
  }

  if (theaspect->DisplayIso()) {
    static Graphic3d_Array1OfVertex IsoArray(1,2);
    TheGroup->SetPrimitivesAspect(theaspect->IsoAspect()->Aspect());
    Standard_Real dist = theaspect->IsoDistance();
    Standard_Real cur = -Xmax+dist;
    while (cur+dist/2. <= Xmax) {
      thegeom->D0(cur,Ymax,p1);
      IsoArray(1).SetCoord(p1.X(),p1.Y(),p1.Z());
      thegeom->D0(cur,-Ymax,p1);
      IsoArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
      TheGroup->Polyline(IsoArray);
      cur += dist;
    }
    cur = -Ymax+dist;
    while (cur+dist/2. < Ymax) {
      thegeom->D0(Xmax,cur,p1);
      IsoArray(1).SetCoord(p1.X(),p1.Y(),p1.Z());
      thegeom->D0(-Xmax,cur,p1);
      IsoArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
      TheGroup->Polyline(IsoArray);
      cur += dist;
    }
  }

  gp_Dir norm = thegeom->Pln().Axis().Direction();
  gp_Pnt loc;
  Quantity_Length siz = theaspect->ArrowsSize();
  Quantity_Length len = theaspect->ArrowsLength();
  Quantity_PlaneAngle ang = theaspect->ArrowsAngle();
  gp_Vec trans(norm);
  trans.Scale(Standard_Real(siz));
  TheGroup->SetPrimitivesAspect(theaspect->ArrowAspect()->Aspect());
  Graphic3d_Array1OfVertex ArrowArray(1,2);
  if (theaspect->DisplayCenterArrow()) {
    loc = thegeom->Location();
    p1 = loc.Translated(trans);
    ArrowArray(1).SetCoord(loc.X(),loc.Y(),loc.Z());
    ArrowArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
    TheGroup->Polyline(ArrowArray);
    Prs3d_Arrow::Draw(aPresentation,
		      p1,
		      norm,
		      ang,
		      len);
  }
  if (theaspect->DisplayEdgesArrows()) {
    thegeom->D0(-Xmax,-Ymax,loc);
    p1 = loc.Translated(trans);
    ArrowArray(1).SetCoord(loc.X(),loc.Y(),loc.Z());
    ArrowArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
    TheGroup->Polyline(ArrowArray);
    Prs3d_Arrow::Draw(aPresentation,
		      p1,
		      norm,
		      ang,
		      len);
    thegeom->D0(-Xmax,Ymax,loc);
    p1 = loc.Translated(trans);
    ArrowArray(1).SetCoord(loc.X(),loc.Y(),loc.Z());
    ArrowArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
    TheGroup->Polyline(ArrowArray);
    Prs3d_Arrow::Draw(aPresentation,
		      p1,
		      norm,
		      ang,
		      len);
    thegeom->D0(Xmax,Ymax,loc);
    p1 = loc.Translated(trans);
    ArrowArray(1).SetCoord(loc.X(),loc.Y(),loc.Z());
    ArrowArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
    TheGroup->Polyline(ArrowArray);
    Prs3d_Arrow::Draw(aPresentation,
		      p1,
		      norm,
		      ang,
		      len);
    thegeom->D0(Xmax,-Ymax,loc);
    p1 = loc.Translated(trans);
    ArrowArray(1).SetCoord(loc.X(),loc.Y(),loc.Z());
    ArrowArray(2).SetCoord(p1.X(),p1.Y(),p1.Z());
    TheGroup->Polyline(ArrowArray);
    Prs3d_Arrow::Draw(aPresentation,
		      p1,
		      norm,
		      ang,
		      len);
  }
  
}

Standard_Boolean StdPrs_Plane::Match
  (const Quantity_Length X,
   const Quantity_Length Y,
   const Quantity_Length Z,
   const Quantity_Length aDistance,
   const Adaptor3d_Surface& aPlane,
   const Handle (Prs3d_Drawer)&)
{
  if (aPlane.GetType() == GeomAbs_Plane) {  
    gp_Pln theplane = aPlane.Plane();
    gp_Pnt thepoint (X,Y,Z);
    
    return (Abs(theplane.Distance(thepoint)) <= aDistance);
  }
  else return Standard_False;
}
