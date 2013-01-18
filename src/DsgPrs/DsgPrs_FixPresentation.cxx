// Created on: 1996-04-01
// Created by: Flore Lantheaume
// Copyright (c) 1996-1999 Matra Datavision
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

#include <DsgPrs_FixPresentation.ixx>

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Vertex.hxx>

#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Root.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_AspectMarker.hxx>

#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>

#include <Quantity_Color.hxx>


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
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
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

  // On ajoute un rond au point d'attache
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Handle(Graphic3d_AspectMarker3d) MarkerAsp = new Graphic3d_AspectMarker3d();
  MarkerAsp->SetType(Aspect_TOM_BALL);
  MarkerAsp->SetScale(0.8);
  Quantity_Color acolor;
  Aspect_TypeOfLine atype;
  Standard_Real awidth;
  LA->LineAspect()->Aspect()->Values(acolor, atype, awidth);
  MarkerAsp->SetColor(acolor);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAsp);
  Graphic3d_Vertex V3d(aPntAttach.X(), aPntAttach.Y(), aPntAttach.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Marker(V3d);
}
