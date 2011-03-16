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
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_Text.hxx>

void Prs3d_LengthPresentation::Draw (
			       const Handle(Prs3d_Presentation)& aPresentation,
			       const Handle(Prs3d_Drawer)& aDrawer,
			       const TCollection_ExtendedString& aText,
			       const gp_Pnt& AttachmentPoint1,
			       const gp_Pnt& AttachmentPoint2,
			       const gp_Pnt& OffsetPoint) {


  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
 Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  gp_Dir D (gp_Vec(AttachmentPoint1,AttachmentPoint2));
  gp_Lin L (OffsetPoint,D);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L,AttachmentPoint1),L);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L,AttachmentPoint2),L);
  Graphic3d_Array1OfVertex V(1,2);

  Quantity_Length X,Y,Z;

  Proj1.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);

  AttachmentPoint1.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  
  Proj2.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  AttachmentPoint2.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  Proj1.Coord(X,Y,Z);
  if (LA->DrawFirstArrow()) {
    Prs3d_Arrow::Draw(aPresentation,Proj1,D.Reversed(),
                      LA->Arrow1Aspect()->Angle(),
		      LA->Arrow1Aspect()->Length());
    }
  Quantity_Length X2,Y2,Z2;
  Proj2.Coord(X2,Y2,Z2);
  if (LA->DrawSecondArrow()) {
    Prs3d_Arrow::Draw(aPresentation,Proj2,D,
                      LA->Arrow2Aspect()->Angle(),
		      LA->Arrow2Aspect()->Length());
    }

  gp_Pnt p;
  p.SetCoord( (X+X2)/2. , (Y+Y2)/2. , (Z+Z2)/2.);

  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,p);
}


