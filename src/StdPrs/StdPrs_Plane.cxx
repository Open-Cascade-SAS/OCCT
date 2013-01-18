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
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
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
                        const Adaptor3d_Surface&           aPlane,
                        const Handle (Prs3d_Drawer)&       aDrawer)
{
//  Prs3d_Root::NewGroup(aPresentation);    
  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
  if (aPlane.GetType() != GeomAbs_Plane) return;
  Handle(Geom_Plane) thegeom = new Geom_Plane(aPlane.Plane());

  Handle(Prs3d_PlaneAspect) theaspect = aDrawer->PlaneAspect();

  gp_Pnt p1;
  Standard_Real Xmax,Ymax;
  Xmax = 0.5*Standard_Real(theaspect->PlaneXLength());
  Ymax = 0.5*Standard_Real(theaspect->PlaneYLength());
  if (theaspect->DisplayEdges()) {
    TheGroup->SetPrimitivesAspect(theaspect->EdgesAspect()->Aspect());
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(5);
    p1 = thegeom->Value(-Xmax,Ymax);
    aPrims->AddVertex(p1);
    aPrims->AddVertex(thegeom->Value( Xmax, Ymax));
    aPrims->AddVertex(thegeom->Value( Xmax,-Ymax));
    aPrims->AddVertex(thegeom->Value(-Xmax,-Ymax));
    aPrims->AddVertex(p1);
    TheGroup->AddPrimitiveArray(aPrims);
  }

  if (theaspect->DisplayIso()) {
    TheGroup->SetPrimitivesAspect(theaspect->IsoAspect()->Aspect());
    const Standard_Real dist = theaspect->IsoDistance();
    const Standard_Integer nbx = Standard_Integer(Abs(2.*Xmax) / dist) - 1;
    const Standard_Integer nby = Standard_Integer(Abs(2.*Ymax) / dist) - 1;
    Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2*(nbx+nby));
    Standard_Integer i;
    Standard_Real cur = -Xmax+dist;
    for (i = 0; i < nbx; i++, cur += dist) {
      aPrims->AddVertex(thegeom->Value(cur, Ymax));
      aPrims->AddVertex(thegeom->Value(cur,-Ymax));
    }
    cur = -Ymax+dist;
    for (i = 0; i < nby; i++, cur += dist) {
      aPrims->AddVertex(thegeom->Value( Xmax,cur));
      aPrims->AddVertex(thegeom->Value(-Xmax,cur));
    }
    TheGroup->AddPrimitiveArray(aPrims);
  }

  gp_Dir norm = thegeom->Pln().Axis().Direction();
  gp_Pnt loc;
  Quantity_Length siz = theaspect->ArrowsSize();
  Quantity_Length len = theaspect->ArrowsLength();
  Quantity_PlaneAngle ang = theaspect->ArrowsAngle();
  gp_Vec trans(norm);
  trans.Scale(Standard_Real(siz));

  TheGroup->SetPrimitivesAspect(theaspect->ArrowAspect()->Aspect());
  if (theaspect->DisplayCenterArrow()) {
    loc = thegeom->Location();
    p1 = loc.Translated(trans);
    Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    TheGroup->AddPrimitiveArray(aPrims);
    Prs3d_Arrow::Draw(aPresentation,p1,norm,ang,len);
  }
  if (theaspect->DisplayEdgesArrows()) {
    Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(8);
    //
    thegeom->D0(-Xmax,-Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw(aPresentation,p1,norm,ang,len);
    //
    thegeom->D0(-Xmax,Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw(aPresentation,p1,norm,ang,len);
    //
    thegeom->D0(Xmax,Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw(aPresentation,p1,norm,ang,len);
    //
    thegeom->D0(Xmax,-Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw(aPresentation,p1,norm,ang,len);
    //
    TheGroup->AddPrimitiveArray(aPrims);
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
