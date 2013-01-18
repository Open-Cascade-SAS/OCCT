// Created on: 1997-02-10
// Created by: Odile Olivier
// Copyright (c) 1997-1999 Matra Datavision
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

#include <DsgPrs_XYZAxisPresentation.ixx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Arrow.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_XYZAxisPresentation::Add(
		       const Handle(Prs3d_Presentation)& aPresentation,
		       const Handle(Prs3d_LineAspect)& aLineAspect,	     
		       const gp_Dir & aDir, 
		       const Standard_Real aVal,
		       const Standard_CString aText,
		       const gp_Pnt& aPfirst,
		       const gp_Pnt& aPlast)
{
  Handle(Graphic3d_Group) G = Prs3d_Root::CurrentGroup(aPresentation);
  G->SetPrimitivesAspect(aLineAspect->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPfirst);
  aPrims->AddVertex(aPlast);
  G->AddPrimitiveArray(aPrims);
 
  Prs3d_Arrow::Draw(aPresentation,aPlast,aDir,M_PI/180.*10.,aVal/10.);

  Graphic3d_Vertex a2(aPlast.X(),aPlast.Y(),aPlast.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Text(aText,a2,1./81.);
}


void DsgPrs_XYZAxisPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_LineAspect)& aLineAspect,
				     const Handle(Prs3d_ArrowAspect)& anArrowAspect,
				     const Handle(Prs3d_TextAspect)& aTextAspect,
				     const gp_Dir & aDir, 
				     const Standard_Real aVal,
				     const Standard_CString aText,
				     const gp_Pnt& aPfirst,
				     const gp_Pnt& aPlast)
{
  Handle(Graphic3d_Group) G = Prs3d_Root::CurrentGroup(aPresentation);
  G->SetPrimitivesAspect(aLineAspect->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPfirst);
  aPrims->AddVertex(aPlast);
  G->AddPrimitiveArray(aPrims);

  G->SetPrimitivesAspect( anArrowAspect->Aspect() );
  Prs3d_Arrow::Draw(aPresentation,aPlast,aDir,M_PI/180.*10.,aVal/10.);

  G->SetPrimitivesAspect(aTextAspect->Aspect());
  Graphic3d_Vertex a2(aPlast.X(),aPlast.Y(),aPlast.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Text(aText,a2,1./81.);
}
