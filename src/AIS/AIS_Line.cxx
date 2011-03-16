// File:	AIS_Line.cxx
// Created:	Tue Jan 21 11:45:01 1997
// Author:	Prestataire Christiane ARMAND
//		<car@chamalox.paris1.matra-dtv.fr>
// Copyright:	 Matra Datavision 1997

#define GER61351	//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <AIS_Line.ixx>
#include <Aspect_TypeOfLine.hxx>
#include <Prs3d_Drawer.hxx>
#include <Precision.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <StdPrs_Curve.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <AIS_Drawer.hxx>
#include <GC_MakeSegment.hxx>
#include <Handle_Geom_Line.hxx>
#include <Quantity_Color.hxx>
#include <AIS_GraphicTool.hxx>
#include <UnitsAPI.hxx>
//==================================================================
// function: FindLimits
// purpose:
//==================================================================
//unused
/*#ifdef DEB
static void FindLimits(const Adaptor3d_Curve& aCurve,
		       const Standard_Real  aLimit,
		       gp_Pnt& P1,
		       gp_Pnt& P2)
{
  Standard_Real First = aCurve.FirstParameter();
  Standard_Real Last  = aCurve.LastParameter();
  Standard_Boolean firstInf = Precision::IsNegativeInfinite(First);
  Standard_Boolean lastInf  = Precision::IsPositiveInfinite(Last);
  

  if (firstInf || lastInf) {
    Standard_Real delta = 1;
    if (firstInf && lastInf) {
      do {
	delta *= 2;
	First = - delta;
	Last  =   delta;
	aCurve.D0(First,P1);
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (firstInf) {
      aCurve.D0(Last,P2);
      do {
	delta *= 2;
	First = Last - delta;
	aCurve.D0(First,P1);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (lastInf) {
      aCurve.D0(First,P1);
      do {
	delta *= 2;
	Last = First + delta;
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }    
}
#endif
*/

//=======================================================================
//function : AIS_Line
//purpose  : 
//=======================================================================
AIS_Line::AIS_Line(const Handle(Geom_Line)& aComponent):
myComponent (aComponent),
myLineIsSegment(Standard_False)
{
  SetInfiniteState();
}

//=======================================================================
//function : AIS_Line
//purpose  : 
//=======================================================================
AIS_Line::AIS_Line(const Handle(Geom_Point)& aStartPoint,
		   const Handle(Geom_Point)& aEndPoint):
myStartPoint(aStartPoint),
myEndPoint(aEndPoint),
myLineIsSegment(Standard_True)
{}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Line::Compute(const Handle(PrsMgr_PresentationManager3d)&,
		       const Handle(Prs3d_Presentation)& aPresentation, 
		       const Standard_Integer)
{
  aPresentation->Clear();

  aPresentation->SetDisplayPriority(5);

  if (!myLineIsSegment) ComputeInfiniteLine(aPresentation);
  else ComputeSegmentLine(aPresentation);

}

void AIS_Line::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_Line::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_Line::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				const Standard_Integer)
{

  if (!myLineIsSegment) ComputeInfiniteLineSelection(aSelection);
  else ComputeSegmentLineSelection(aSelection);

}


//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_Line::SetColor(const Quantity_NameOfColor aCol)
#ifdef GER61351
{
  SetColor(Quantity_Color(aCol));
}

void AIS_Line::SetColor(const Quantity_Color &aCol)
#endif
{
  hasOwnColor=Standard_True;
  myOwnColor=aCol;

  Standard_Real WW = HasWidth()? myOwnWidth:
                                 AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Line);

  if (!myDrawer->HasLineAspect ())
    myDrawer->SetLineAspect (new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));
  else
    myDrawer->LineAspect()->SetColor(aCol);
}


//=======================================================================
//function : UnsetColor 
//purpose  : 
//=======================================================================
void AIS_Line::UnsetColor()
{
  hasOwnColor = Standard_False;

  Handle(Prs3d_LineAspect) NullAsp;

  if (!HasWidth()) myDrawer->SetLineAspect(NullAsp);
  else{
#ifdef GER61351
    Quantity_Color CC;
    if( HasColor() ) CC = myOwnColor;
    else AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line,CC);
#else
    Quantity_NameOfColor CC =
		AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line);
#endif 

    myDrawer->LineAspect()->SetColor(CC);
    myOwnColor = CC;
 }
}

//=======================================================================
//function : SetWidth 
//purpose  : 
//=======================================================================
void AIS_Line::SetWidth(const Standard_Real aValue)
{
  myOwnWidth=aValue;

#ifndef GER61351
  Quantity_NameOfColor CC =
		 HasColor()? myOwnColor : AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line);
#endif

  if (!myDrawer->HasLineAspect ()) {
#ifdef GER61351
    Quantity_Color CC;
    if( HasColor() ) CC = myOwnColor;
    else AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line,CC);
#endif
    myDrawer->SetLineAspect (new Prs3d_LineAspect(CC,Aspect_TOL_SOLID,aValue));
  } else
    myDrawer->LineAspect()->SetWidth(aValue);
}


//=======================================================================
//function : UnsetWidth 
//purpose  : 
//=======================================================================
void AIS_Line::UnsetWidth()
{
  Handle(Prs3d_LineAspect) NullAsp;

  if (!HasColor()) myDrawer->SetLineAspect(NullAsp);
  else{
   Standard_Real WW = AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Line);
   myDrawer->LineAspect()->SetWidth(WW);
   myOwnWidth = WW;
  }
}

//=======================================================================
//function : ComputeInfiniteLine
//purpose  : 
//=======================================================================
void AIS_Line::ComputeInfiniteLine( const Handle(Prs3d_Presentation)& aPresentation)
{

  GeomAdaptor_Curve curv(myComponent);
  Standard_Boolean isPrimitiveArraysEnabled = Graphic3d_ArrayOfPrimitives::IsEnable();
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Disable();
  StdPrs_Curve::Add(aPresentation,curv,myDrawer);
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Enable();

  //pas de prise en compte lors du FITALL
  aPresentation->SetInfiniteState (Standard_True);

}

//=======================================================================
//function : ComputeSegmentLine
//purpose  : 
//=======================================================================
void AIS_Line::ComputeSegmentLine( const Handle(Prs3d_Presentation)& aPresentation)
{

  gp_Pnt P1 = myStartPoint->Pnt();
  gp_Pnt P2 = myEndPoint->Pnt();
  
  myComponent = new Geom_Line(P1,gp_Dir(P2.XYZ()-P1.XYZ()));

  Standard_Real dist = P1.Distance(P2);
  GeomAdaptor_Curve curv(myComponent,0.,dist);
  Standard_Boolean isPrimitiveArraysEnabled = Graphic3d_ArrayOfPrimitives::IsEnable();
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Disable();
  StdPrs_Curve::Add(aPresentation,curv,myDrawer);
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Enable();

}


//=======================================================================
//function : ComputeInfiniteLineSelection
//purpose  : 
//=======================================================================

void AIS_Line::ComputeInfiniteLineSelection(const Handle(SelectMgr_Selection)& aSelection)
{

/*  // on calcule les points min max a partir desquels on cree un segment sensible...
  GeomAdaptor_Curve curv(myComponent);
  gp_Pnt P1,P2;
  FindLimits(curv,myDrawer->MaximalParameterValue(),P1,P2);
*/   
  const gp_Dir& thedir = myComponent->Position().Direction();
  const gp_Pnt& loc = myComponent->Position().Location();
  const gp_XYZ& dir_xyz = thedir.XYZ();
  const gp_XYZ& loc_xyz = loc.XYZ();
//POP  Standard_Real aLength = UnitsAPI::CurrentToLS (250000. ,"LENGTH");
  Standard_Real aLength = UnitsAPI::AnyToLS (250000. ,"mm");
  gp_Pnt P1 = loc_xyz + aLength*dir_xyz;
  gp_Pnt P2 = loc_xyz - aLength*dir_xyz;
  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this,5);
  Handle(Select3D_SensitiveSegment) seg = new Select3D_SensitiveSegment(eown,P1,P2);
  aSelection->Add(seg);
}
//=======================================================================
//function : ComputeSegmentLineSelection
//purpose  : 
//=======================================================================

void AIS_Line::ComputeSegmentLineSelection(const Handle(SelectMgr_Selection)& aSelection)
{


  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this,5);
  Handle(Select3D_SensitiveSegment) seg = new Select3D_SensitiveSegment(eown,
								        myStartPoint->Pnt(),
									myEndPoint->Pnt());
  aSelection->Add(seg);
}
//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Line::Compute(const Handle(PrsMgr_PresentationManager2d)&, 
			  const Handle(Graphic2d_GraphicObject)&,
			  const Standard_Integer)
{
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Line::Compute(const Handle(Prs3d_Projector)&, 
			  const Handle(Prs3d_Presentation)&)
{
}




