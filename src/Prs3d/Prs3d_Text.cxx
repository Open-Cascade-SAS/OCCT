// File:	Prs3d_Text.cxx
// Created:	Tue Sep 14 14:52:34 1993
// Author:	Jean-Louis FRENKEL
//		<jlf@stylox>


#include <Prs3d_Text.ixx>
#include <Graphic3d_Vertex.hxx>
#include <Prs3d_TextAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_Group.hxx>

void Prs3d_Text::Draw (
		       const Handle(Prs3d_Presentation)& aPresentation,
		       const Handle(Prs3d_TextAspect)& anAspect,
		       const TCollection_ExtendedString& aText,
		       const gp_Pnt& AttachmentPoint) {


  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(anAspect->Aspect());
  Standard_Real x,y,z;
  AttachmentPoint.Coord(x,y,z);

  
// POP Graphic3d_Grup accepte de l'extended
  Prs3d_Root::CurrentGroup(aPresentation)->Text(
//                     TCollection_AsciiString(aText).ToCString(),
		      aText,
	             Graphic3d_Vertex(x,y,z),
                     anAspect->Height(),
                     anAspect->Angle(),
                     anAspect->Orientation(),
                     anAspect->HorizontalJustification(),
                     anAspect->VerticalJustification());
}


void Prs3d_Text::Draw (
		       const Handle(Prs3d_Presentation)& aPresentation,
		       const Handle(Prs3d_Drawer)& aDrawer,
		       const TCollection_ExtendedString& aText,
		       const gp_Pnt& AttachmentPoint) {

  
  Prs3d_Text::Draw(aPresentation,aDrawer->TextAspect(),aText,AttachmentPoint);
  }
