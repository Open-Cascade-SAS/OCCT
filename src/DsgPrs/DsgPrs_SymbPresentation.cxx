// File:	DsgPrs_SymbPresentation.cxx
// Created:	Fri Dec  8 16:45:28 1995
// Author:	Jean-Pierre COMBE
//		<jpi>


#include <DsgPrs_SymbPresentation.ixx>

#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Geom_CartesianPoint.hxx>
#include <StdPrs_Point.hxx>
#include <TCollection_AsciiString.hxx>
#include <Prs3d_Text.hxx>
#include <Aspect_TypeOfMarker.hxx>

void DsgPrs_SymbPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				   const Handle(Prs3d_Drawer)& aDrawer,
				   const TCollection_ExtendedString& aText,
				   const gp_Pnt& OffsetPoint) 
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Handle(Prs3d_TextAspect) TA = LA->TextAspect();
  TA->SetColor(Quantity_NOC_GREEN);
  Prs3d_Text::Draw(aPresentation,TA,aText, OffsetPoint);
  
  // 2eme groupe : marker
  Handle(Geom_CartesianPoint) theP = new Geom_CartesianPoint(OffsetPoint);
  Handle(Prs3d_PointAspect) PA = aDrawer->PointAspect();
  PA->SetTypeOfMarker(Aspect_TOM_RING2);
  StdPrs_Point::Add(aPresentation,theP,aDrawer);
}


