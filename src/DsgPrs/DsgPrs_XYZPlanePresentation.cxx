// File:	DsgPrs_XYZPlanePresentation.cdl
// Created:	Mon Feb 10 14:50:11 1997
// Author:	Odile Olivier
//		<odl@sacadox.paris1.matra-dtv.fr>
//Copyright:	 Matra Datavision 1997

#include <DsgPrs_XYZPlanePresentation.ixx>
#include <Prs3d_Root.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_XYZPlanePresentation::Add(
		       const Handle(Prs3d_Presentation)& aPresentation,
		       const Handle(Prs3d_Drawer)& aDrawer,	     
		       const gp_Pnt& aPt1, 
		       const gp_Pnt& aPt2,
		       const gp_Pnt& aPt3)
{


 Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
 TheGroup->SetPrimitivesAspect(aDrawer->PlaneAspect()->EdgesAspect()->Aspect());

 Quantity_Length x1,y1,z1,x2,y2,z2,x3,y3,z3;
  
 aPt1.Coord(x1,y1,z1);
 aPt2.Coord(x2,y2,z2);
 aPt3.Coord(x3,y3,z3);
 
 Graphic3d_Array1OfVertex A(1,4);
 A(1).SetCoord(x1,y1,z1);
 A(2).SetCoord(x2,y2,z2);
 A(3).SetCoord(x3,y3,z3);
 A(4).SetCoord(x1,y1,z1);
 
 TheGroup->Polyline(A);

}
