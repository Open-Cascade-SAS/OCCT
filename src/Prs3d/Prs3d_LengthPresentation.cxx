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

#include <Prs3d_LengthPresentation.ixx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Prs3d_Text.hxx>

void Prs3d_LengthPresentation::Draw (
			       const Handle(Prs3d_Presentation)& aPresentation,
			       const Handle(Prs3d_Drawer)& aDrawer,
			       const TCollection_ExtendedString& aText,
			       const gp_Pnt& AttachmentPoint1,
			       const gp_Pnt& AttachmentPoint2,
			       const gp_Pnt& OffsetPoint)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Dir D (gp_Vec(AttachmentPoint1,AttachmentPoint2));
  gp_Lin L (OffsetPoint,D);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L,AttachmentPoint1),L);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L,AttachmentPoint2),L);

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(4);
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(Proj1);
  aPrims->AddVertex(Proj2);
  aPrims->AddVertex(AttachmentPoint2);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Quantity_Length X,Y,Z;
  Proj1.Coord(X,Y,Z);
  if (LA->DrawFirstArrow())
    Prs3d_Arrow::Draw(aPresentation,Proj1,D.Reversed(),LA->Arrow1Aspect()->Angle(),LA->Arrow1Aspect()->Length());

  Quantity_Length X2,Y2,Z2;
  Proj2.Coord(X2,Y2,Z2);
  if (LA->DrawSecondArrow())
    Prs3d_Arrow::Draw(aPresentation,Proj2,D,LA->Arrow2Aspect()->Angle(),LA->Arrow2Aspect()->Length());

  gp_Pnt p( .5*(X+X2), .5*(Y+Y2), .5*(Z+Z2) );
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,p);
}
