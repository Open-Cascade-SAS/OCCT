// File:	DsgPrs_TangentPresentation.cxx
// Created:	Tue Jan 16 14:29:33 1996
// Author:	Jean-Pierre COMBE
//		<jpi>


#include <DsgPrs_TangentPresentation.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Geom_CartesianPoint.hxx>
#include <StdPrs_Point.hxx>
#include <gp_Vec.hxx>
#include <Prs3d_Arrow.hxx>

void DsgPrs_TangentPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const gp_Pnt& OffsetPoint,
				      const gp_Dir& aDirection,
				      const Standard_Real length)
{
  gp_Vec vec(aDirection);
  gp_Vec vec1 = vec.Multiplied(length);
  gp_Vec vec2 = vec.Multiplied(-length);
  gp_Pnt p1 = OffsetPoint.Translated(vec1);
  gp_Pnt p2 = OffsetPoint.Translated(vec2);
  
  // Aspect
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  LA->LineAspect()->SetTypeOfLine(Aspect_TOL_SOLID);
  Handle(Prs3d_ArrowAspect) ARR1 = LA->Arrow1Aspect();
  Handle(Prs3d_ArrowAspect) ARR2 = LA->Arrow2Aspect();
  ARR1->SetLength(length/5);
  ARR2->SetLength(length/5);

  // Array1OfVertex
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Graphic3d_Array1OfVertex V(1,2);
  Quantity_Length X,Y,Z;
  p1.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  p2.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  // fleche 1 : 
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Arrow::Draw(aPresentation,p1,aDirection,
		    LA->Arrow1Aspect()->Angle(),
		    LA->Arrow1Aspect()->Length());

  // fleche 2
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Arrow::Draw(aPresentation,p2,aDirection.Reversed(),
		    LA->Arrow2Aspect()->Angle(),
		    LA->Arrow2Aspect()->Length());
}


