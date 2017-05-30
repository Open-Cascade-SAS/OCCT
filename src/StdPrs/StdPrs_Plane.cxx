// Created on: 1995-07-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor3d_Surface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_Plane.hxx>

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
  Standard_Real siz = theaspect->ArrowsSize();
  Standard_Real len = theaspect->ArrowsLength();
  Standard_Real ang = theaspect->ArrowsAngle();
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
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), p1, norm, ang, len);
  }
  if (theaspect->DisplayEdgesArrows()) {
    Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(8);
    //
    thegeom->D0(-Xmax,-Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), p1, norm, ang, len);
    //
    thegeom->D0(-Xmax,Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), p1, norm, ang, len);
    //
    thegeom->D0(Xmax,Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), p1, norm, ang, len);
    //
    thegeom->D0(Xmax,-Ymax,loc);
    p1 = loc.Translated(trans);
    aPrims->AddVertex(loc);
    aPrims->AddVertex(p1);
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), p1, norm, ang, len);
    //
    TheGroup->AddPrimitiveArray(aPrims);
  }
}

Standard_Boolean StdPrs_Plane::Match
  (const Standard_Real X,
   const Standard_Real Y,
   const Standard_Real Z,
   const Standard_Real aDistance,
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
