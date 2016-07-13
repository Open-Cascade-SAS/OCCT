// Created on: 1996-04-01
// Created by: Flore Lantheaume
// Copyright (c) 1996-1999 Matra Datavision
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

#include <DsgPrs_FixPresentation.hxx>

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void DsgPrs_FixPresentation::Add(
		       const Handle(Prs3d_Presentation)& aPresentation,
		       const Handle(Prs3d_Drawer)& aDrawer,
		       const gp_Pnt& aPntAttach,
		       const gp_Pnt& aPntEnd,
		       const gp_Dir& aNormPln,
		       const Standard_Real symbsize)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(10);

  //Trace du segment de raccordement
  aPrims->AddVertex(aPntAttach);
  aPrims->AddVertex(aPntEnd);

  // trace du symbole 'Fix'
  gp_Vec dirac(aPntAttach, aPntEnd); // vecteur directeur du seg. de raccord
  dirac.Normalize();
  gp_Vec norac = dirac.Crossed(gp_Vec(aNormPln));
  gp_Ax1 ax(aPntEnd, aNormPln);
  norac.Rotate(ax, M_PI/8); // vecteur normal au seg. de raccord
  norac*=(symbsize/2);
  gp_Pnt P1 = aPntEnd.Translated(norac);
  gp_Pnt P2 = aPntEnd.Translated(-norac);

  aPrims->AddVertex(P1);
  aPrims->AddVertex(P2);

  // trace des 'dents'
  norac*=0.8;
  P1 = aPntEnd.Translated(norac);
  P2 = aPntEnd.Translated(-norac);
  dirac*=(symbsize/2);
  gp_Pnt PF = P1;
  gp_Pnt PL = PF.Translated(dirac);
  PL.Translate(norac);

  aPrims->AddVertex(PF);
  aPrims->AddVertex(PL);

  PF = P2;
  PL = PF.Translated(dirac);
  PL.Translate(norac);

  aPrims->AddVertex(PF);
  aPrims->AddVertex(PL);

  PF.SetXYZ(0.5*(P1.XYZ() + P2.XYZ()));
  PL = PF.Translated(dirac);
  PL.Translate(norac);

  aPrims->AddVertex(PF);
  aPrims->AddVertex(PL);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Quantity_Color aColor = LA->LineAspect()->Aspect()->Color();
  Handle(Graphic3d_AspectMarker3d) aMarkerAsp = new Graphic3d_AspectMarker3d (Aspect_TOM_O, aColor, 1.0);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect (aMarkerAsp);
  Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
  anArrayOfPoints->AddVertex (aPntAttach.X(), aPntAttach.Y(), aPntAttach.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);
}
