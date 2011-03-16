// File:	DsgPrs_XYZAxisPresentation.cdl
// Created:	Mon Feb 10 14:50:11 1997
// Author:	Odile Olivier
//		<odl@sacadox.paris1.matra-dtv.fr>
//Copyright:	 Matra Datavision 1997

#define OCC218	// SAV Enable to compute the triedhron color texts and arrows.


#include <DsgPrs_XYZAxisPresentation.ixx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Arrow.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>


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

 Quantity_Length xo,yo,zo,x,y,z;
  
 aPfirst.Coord(xo,yo,zo);
 aPlast.Coord(x,y,z);
 
 Graphic3d_Array1OfVertex A(1,2);
 A(1).SetCoord(xo,yo,zo);
 A(2).SetCoord(x,y,z);
 
 G->SetPrimitivesAspect(aLineAspect->Aspect());
 G->Polyline(A);
 Prs3d_Arrow::Draw(aPresentation,gp_Pnt(x,y,z),aDir,PI/180.*10.,aVal/10.);
 Prs3d_Root::CurrentGroup(aPresentation)->Text(aText,A(2),1./81.);

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
#ifdef OCC218
  Handle(Graphic3d_Group) G = Prs3d_Root::CurrentGroup(aPresentation);

  Quantity_Length xo,yo,zo,x,y,z;
  
  aPfirst.Coord(xo,yo,zo);
  aPlast.Coord(x,y,z);
 
  Graphic3d_Array1OfVertex A(1,2);
  A(1).SetCoord(xo,yo,zo);
  A(2).SetCoord(x,y,z);
 
#ifdef DEB
  Quantity_Length arrowAngle = anArrowAspect->Angle();
  Quantity_Length textHeight = aTextAspect->Height();
#endif

  G->SetPrimitivesAspect(aLineAspect->Aspect());
  G->Polyline(A);
  G->SetPrimitivesAspect( anArrowAspect->Aspect() );
  Prs3d_Arrow::Draw(aPresentation,gp_Pnt(x,y,z),aDir,PI/180.*10.,aVal/10.);
  G->SetPrimitivesAspect(aTextAspect->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Text(aText,A(2),1./81.);
#endif
}
