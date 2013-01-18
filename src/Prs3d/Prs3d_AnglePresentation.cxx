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

#include <Prs3d_AnglePresentation.ixx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_AngleAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Prs3d_Text.hxx>

void Prs3d_AnglePresentation::Draw (
			       const Handle(Prs3d_Presentation)& aPresentation,
			       const Handle(Prs3d_Drawer)& aDrawer,
			       const TCollection_ExtendedString& aText,
			       const gp_Pnt& AttachmentPoint1,
			       const gp_Pnt& AttachmentPoint2,
			       const gp_Pnt& AttachmentPoint3,
			       const gp_Pnt& OffsetPoint)
{
  Handle(Prs3d_AngleAspect) AA = aDrawer->AngleAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(AA->LineAspect()->Aspect());

  gp_Vec V1 (AttachmentPoint1,AttachmentPoint2);
  gp_Vec V2 (AttachmentPoint1,AttachmentPoint3);
  const Standard_Real alpha = V1.Angle(V2);
  const Standard_Real ray = AttachmentPoint1.Distance(OffsetPoint);

  gp_Dir I (V1);
  gp_Dir K = I.Crossed(gp_Dir(V2));
  gp_Dir J = K.Crossed(I);

  const Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / M_PI));
  const Standard_Real dteta = alpha/(nbp-1);
  gp_Vec u;
  gp_Pnt p1, p2;

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(nbp+2);
  aPrims->AddVertex(AttachmentPoint2);
  for (Standard_Integer i = 1; i<=nbp; i++)
  {
    u = (gp_Vec(I) * Cos ( (i-1) * dteta) 
      +  gp_Vec(J) * Sin ( (i-1) * dteta)) * ray ;
    p2 = AttachmentPoint1.Translated(u);
    aPrims->AddVertex(p2);
    if( i == nbp ) {
      gp_Dir dir( gp_Vec(p1,p2) );
      Prs3d_Arrow::Draw(aPresentation,p2,dir,AA->ArrowAspect()->Angle(),AA->ArrowAspect()->Length());
    }
    p1 = p2;
  }
  aPrims->AddVertex(AttachmentPoint3);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  u =  (gp_Vec(I) * Cos (0.5*alpha) 
      + gp_Vec(J) * Sin (0.5*alpha)) * ray ;
  Prs3d_Text::Draw(aPresentation,AA->TextAspect(),aText,AttachmentPoint1.Translated(u));
}
