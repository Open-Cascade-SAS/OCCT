// File:	DsgPrs_Chamf2dPresentation.cxx
// Created:	Tue Mar 19 18:45:13 1996
// Author:	Flore Lantheaume
//		<fla@filax>


#include <DsgPrs_Chamf2dPresentation.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Text.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <DsgPrs.hxx>

void DsgPrs_Chamf2dPresentation::Add(
			   const Handle(Prs3d_Presentation)& aPresentation,
			   const Handle(Prs3d_Drawer)& aDrawer,
			   const gp_Pnt& aPntAttach,
			   const gp_Pnt& aPntEnd,
			   const TCollection_ExtendedString& aText)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  
  Prs3d_Root::CurrentGroup(aPresentation)
    ->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Graphic3d_Array1OfVertex V(1,2);
  V(1).SetCoord(aPntAttach.X(),aPntAttach.Y(),aPntAttach.Z());

  V(2).SetCoord(aPntEnd.X(),aPntEnd.Y(),aPntEnd.Z());

  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  gp_Dir ArrowDir(aPntAttach.XYZ()-aPntEnd.XYZ());
  Prs3d_Arrow::Draw(aPresentation,
		    aPntAttach,
		    ArrowDir,
		    LA->Arrow1Aspect()->Angle(),
		    LA->Arrow1Aspect()->Length());
		     
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntEnd);

}


//==========================================================================
// function : DsgPrs_Chamf2dPresentation::Add
// purpose  : ODL 4-fevrier-97 
//  on peut choisir le symbol des extremites de la cote (fleche, point ...)
//==========================================================================

void DsgPrs_Chamf2dPresentation::Add(
			   const Handle(Prs3d_Presentation)& aPresentation,
			   const Handle(Prs3d_Drawer)& aDrawer,
			   const gp_Pnt& aPntAttach,
			   const gp_Pnt& aPntEnd,
			   const TCollection_ExtendedString& aText,
			   const DsgPrs_ArrowSide ArrowPrs) 
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  
  Prs3d_Root::CurrentGroup(aPresentation)
    ->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Graphic3d_Array1OfVertex V(1,2);
  V(1).SetCoord(aPntAttach.X(),aPntAttach.Y(),aPntAttach.Z());

  V(2).SetCoord(aPntEnd.X(),aPntEnd.Y(),aPntEnd.Z());

  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntEnd);

  gp_Dir ArrowDir(aPntAttach.XYZ()-aPntEnd.XYZ());

  gp_Dir ArrowDir1 = ArrowDir;
  ArrowDir1.Reverse();

  DsgPrs::ComputeSymbol(aPresentation,LA,aPntEnd,aPntAttach,ArrowDir1,ArrowDir,ArrowPrs);


}
