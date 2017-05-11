// Created on: 1997-01-21
// Created by: Prestataire Christiane ARMAND
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <AIS_Line.hxx>

#include <AIS_GraphicTool.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_Line.hxx>
#include <Geom_Point.hxx>
#include <Geom_Transformation.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_Curve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Line,AIS_InteractiveObject)

//==================================================================
// function: FindLimits
// purpose:
//==================================================================
//unused
/*#ifdef OCCT_DEBUG
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
  aPresentation->SetDisplayPriority(5);

  if (!myLineIsSegment) ComputeInfiniteLine(aPresentation);
  else ComputeSegmentLine(aPresentation);

}

void AIS_Line::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTransformation, const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_Line::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_Line::ComputeSelection(const Handle(SelectMgr_Selection)& theSelection,
                                const Standard_Integer             theMode)
{
  // Do not support selection modes different from 0 currently
  if (theMode)
    return;

  if (!myLineIsSegment)
  {
    ComputeInfiniteLineSelection(theSelection);
  }
  else
  {
    ComputeSegmentLineSelection(theSelection);
  }
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_Line::SetColor(const Quantity_Color &aCol)
{
  hasOwnColor=Standard_True;
  myDrawer->SetColor (aCol);

  Standard_Real WW = HasWidth()? myOwnWidth:
                                 myDrawer->HasLink() ?
                                 AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_Line) : 1.;

  if (!myDrawer->HasOwnLineAspect ())
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
    Quantity_Color CC = Quantity_NOC_YELLOW;
    if( HasColor() ) CC = myDrawer->Color();
    else if (myDrawer->HasLink()) AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_Line, CC);
    myDrawer->LineAspect()->SetColor(CC);
    myDrawer->SetColor (CC);
 }
}

//=======================================================================
//function : SetWidth 
//purpose  : 
//=======================================================================
void AIS_Line::SetWidth(const Standard_Real aValue)
{
  myOwnWidth=aValue;

  if (!myDrawer->HasOwnLineAspect ()) {
    Quantity_Color CC = Quantity_NOC_YELLOW;
    if( HasColor() ) CC = myDrawer->Color();
    else if(myDrawer->HasLink()) AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_Line, CC);
    myDrawer->SetLineAspect (new Prs3d_LineAspect (CC, Aspect_TOL_SOLID, aValue));
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
   Standard_Real WW = myDrawer->HasLink() ?
     AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_Line) : 1.;
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
  StdPrs_Curve::Add(aPresentation,curv,myDrawer);

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
  StdPrs_Curve::Add(aPresentation,curv,myDrawer);
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
void AIS_Line::Compute(const Handle(Prs3d_Projector)&, 
			  const Handle(Prs3d_Presentation)&)
{
}




