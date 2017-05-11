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

#include <AIS_Circle.hxx>

#include <AIS_GraphicTool.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Transformation.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Circle,AIS_InteractiveObject)

//=======================================================================
//function : AIS_Circle
//purpose  : 
//=======================================================================
AIS_Circle::AIS_Circle(const Handle(Geom_Circle)& aComponent):
AIS_InteractiveObject(PrsMgr_TOP_AllView),
myComponent(aComponent),
myUStart(0.),
myUEnd(2*M_PI),
myCircleIsArc(Standard_False),
myIsFilledCircleSens (Standard_False)
{
}

//=======================================================================
//function : AIS_Circle
//purpose  : 
//=======================================================================
AIS_Circle::AIS_Circle(const Handle(Geom_Circle)& theComponent,
                       const Standard_Real theUStart,
                       const Standard_Real theUEnd,
                       const Standard_Boolean theIsFilledCircleSens)
: AIS_InteractiveObject(PrsMgr_TOP_AllView),
  myComponent (theComponent),
  myUStart (theUStart),
  myUEnd (theUEnd),
  myCircleIsArc (Standard_True),
  myIsFilledCircleSens (theIsFilledCircleSens)
{
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Circle::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                         const Handle(Prs3d_Presentation)& aPresentation, 
                         const Standard_Integer /*aMode*/)
{
  aPresentation->SetDisplayPriority(5);

  if (myCircleIsArc) ComputeArc(aPresentation);
  else ComputeCircle(aPresentation);

}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_Circle::Compute(const Handle(Prs3d_Projector)& aProjector,
                         const Handle(Geom_Transformation)& aTransformation,
                         const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_Circle::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
  PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_Circle::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                  const Standard_Integer /*aMode*/)
{

  if (myCircleIsArc) ComputeArcSelection(aSelection);
  else ComputeCircleSelection(aSelection);

}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_Circle::SetColor(const Quantity_Color &aCol)
{
  hasOwnColor=Standard_True;
  myDrawer->SetColor (aCol);

  Standard_Real WW = HasWidth() ? myOwnWidth :
                                  myDrawer->HasLink() ?
                                  AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_Line) :
                                  1.;

  if (!myDrawer->HasOwnLineAspect ())
    myDrawer->SetLineAspect (new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));
  else
    myDrawer->LineAspect()->SetColor(aCol);
}



//=======================================================================
//function : SetWidth 
//purpose  : 
//=======================================================================
void AIS_Circle::SetWidth(const Standard_Real aValue)
{
  myOwnWidth=aValue;

  if (!myDrawer->HasOwnLineAspect ()) {
    Quantity_Color CC = Quantity_NOC_YELLOW;
    if( HasColor() ) CC = myDrawer->Color();
    else if(myDrawer->HasLink()) AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_Line, CC);
    myDrawer->SetLineAspect (new Prs3d_LineAspect(CC,Aspect_TOL_SOLID,aValue));
  } else
    myDrawer->LineAspect()->SetWidth(aValue);
}


//=======================================================================
//function : UnsetColor 
//purpose  : 
//=======================================================================
void AIS_Circle::UnsetColor()
{
  hasOwnColor = Standard_False;

  Handle(Prs3d_LineAspect) NullAsp;

  if (!HasWidth()) myDrawer->SetLineAspect(NullAsp);
  else{
    Quantity_Color CC = Quantity_NOC_YELLOW;;
    if( HasColor() ) CC = myDrawer->Color();
    else if (myDrawer->HasLink()) AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line,CC);
    myDrawer->LineAspect()->SetColor(CC);
    myDrawer->SetColor (CC);
  }
}

//=======================================================================
//function : UnsetWidth 
//purpose  : 
//=======================================================================
void AIS_Circle::UnsetWidth()
{
  Handle(Prs3d_LineAspect) NullAsp;

  if (!HasColor()) myDrawer->SetLineAspect(NullAsp);
  else{
   Standard_Real WW = myDrawer->HasLink() ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Line) : 1.;
   myDrawer->LineAspect()->SetWidth(WW);
   myOwnWidth = WW;
  }
}

//=======================================================================
//function : ComputeCircle
//purpose  : 
//=======================================================================
void AIS_Circle::ComputeCircle( const Handle(Prs3d_Presentation)& aPresentation)
{

  GeomAdaptor_Curve curv(myComponent);
  Standard_Real prevdev = myDrawer->DeviationCoefficient();
  myDrawer->SetDeviationCoefficient(1.e-5);
  StdPrs_DeflectionCurve::Add(aPresentation,curv,myDrawer);
  myDrawer->SetDeviationCoefficient(prevdev);

}

//=======================================================================
//function : ComputeArc

//purpose  : 
//=======================================================================
void AIS_Circle::ComputeArc( const Handle(Prs3d_Presentation)& aPresentation)
{
  GeomAdaptor_Curve curv(myComponent,myUStart,myUEnd);
  Standard_Real prevdev = myDrawer->DeviationCoefficient();
  myDrawer->SetDeviationCoefficient(1.e-5);
  StdPrs_DeflectionCurve::Add(aPresentation,curv,myDrawer);
  myDrawer->SetDeviationCoefficient(prevdev);
}

//=======================================================================
//function : ComputeCircleSelection
//purpose  : 
//=======================================================================

void AIS_Circle::ComputeCircleSelection(const Handle(SelectMgr_Selection)& aSelection)
{
  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveCircle) seg = new Select3D_SensitiveCircle (eown,
                                                                       myComponent,
                                                                       myIsFilledCircleSens);
  aSelection->Add(seg);
}
//=======================================================================
//function : ComputeArcSelection
//purpose  : 
//=======================================================================

void AIS_Circle::ComputeArcSelection(const Handle(SelectMgr_Selection)& aSelection)
{


  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveCircle) seg = new Select3D_SensitiveCircle (eown,
                                                                       myComponent,
                                                                       myUStart, myUEnd,
                                                                       myIsFilledCircleSens);
  aSelection->Add(seg);
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Circle::Compute(const Handle(Prs3d_Projector)&, 
                         const Handle(Prs3d_Presentation)&)
{
}
