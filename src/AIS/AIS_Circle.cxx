// File:	AIS_Circle.cxx
// Created:	Tue Jan 21 11:45:01 1997
// Author:	Prestataire Christiane ARMAND
//		<car@chamalox.paris1.matra-dtv.fr>
// Copyright:	 Matra Datavision 1997

#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <AIS_Circle.ixx>
#include <Aspect_TypeOfLine.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <TopoDS.hxx>
#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <AIS_Drawer.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Quantity_Color.hxx>
#include <AIS_GraphicTool.hxx>

//=======================================================================
//function : AIS_Circle
//purpose  : 
//=======================================================================
AIS_Circle::AIS_Circle(const Handle(Geom_Circle)& aComponent):
AIS_InteractiveObject(PrsMgr_TOP_AllView),
myComponent(aComponent),
myUStart(0.),
myUEnd(2*PI),
myCircleIsArc(Standard_False)
{
}

//=======================================================================
//function : AIS_Circle
//purpose  : 
//=======================================================================
AIS_Circle::AIS_Circle(const Handle(Geom_Circle)& aComponent,
                       const Standard_Real aUStart,
                       const Standard_Real aUEnd,
                       const Standard_Boolean aSens):
       AIS_InteractiveObject(PrsMgr_TOP_AllView)
{
  myComponent = aComponent;
  myUStart    = aUStart;
  myUEnd      = aUEnd;
  mySens      = aSens;
  myCircleIsArc = Standard_True;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Circle::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                         const Handle(Prs3d_Presentation)& aPresentation, 
                         const Standard_Integer /*aMode*/)
{
  aPresentation->Clear();

  aPresentation->SetDisplayPriority(5);

  if (myCircleIsArc) ComputeArc(aPresentation);
  else ComputeCircle(aPresentation);

}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_Circle::Compute(const Handle_Prs3d_Projector& aProjector,
                         const Handle_Geom_Transformation& aTransformation,
                         const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_Circle::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
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

void AIS_Circle::SetColor(const Quantity_NameOfColor aCol)
#ifdef GER61351
{
  SetColor(Quantity_Color(aCol));
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_Circle::SetColor(const Quantity_Color &aCol)
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
//function : SetWidth 
//purpose  : 
//=======================================================================
void AIS_Circle::SetWidth(const Standard_Real aValue)
{
  myOwnWidth=aValue;

#ifndef GER61351
  Quantity_NameOfColor CC = 
	HasColor() ? myOwnColor : AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line);
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
//function : UnsetColor 
//purpose  : 
//=======================================================================
void AIS_Circle::UnsetColor()
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
#endif
		AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line);
    myDrawer->LineAspect()->SetColor(CC);
    myOwnColor = CC;
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
   Standard_Real WW = AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Line);
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
  Standard_Boolean isPrimitiveArraysEnabled = Graphic3d_ArrayOfPrimitives::IsEnable();
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Disable();
  StdPrs_DeflectionCurve::Add(aPresentation,curv,myDrawer);
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Enable();
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
  Standard_Boolean isPrimitiveArraysEnabled = Graphic3d_ArrayOfPrimitives::IsEnable();
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Disable();
  StdPrs_DeflectionCurve::Add(aPresentation,curv,myDrawer);
  if(isPrimitiveArraysEnabled) Graphic3d_ArrayOfPrimitives::Enable();
  myDrawer->SetDeviationCoefficient(prevdev);

}

//=======================================================================
//function : ComputeCircleSelection
//purpose  : 
//=======================================================================

void AIS_Circle::ComputeCircleSelection(const Handle(SelectMgr_Selection)& aSelection)
{
  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveCircle) seg = new Select3D_SensitiveCircle(eown,
								       myComponent);
  aSelection->Add(seg);
}
//=======================================================================
//function : ComputeArcSelection
//purpose  : 
//=======================================================================

void AIS_Circle::ComputeArcSelection(const Handle(SelectMgr_Selection)& aSelection)
{


  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveCircle) seg = new Select3D_SensitiveCircle(eown,
								      myComponent,myUStart,myUEnd);
  aSelection->Add(seg);
}
//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Circle::Compute(const Handle(PrsMgr_PresentationManager2d)&, 
                         const Handle(Graphic2d_GraphicObject)&,
                         const Standard_Integer)
{
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Circle::Compute(const Handle(Prs3d_Projector)&, 
                         const Handle(Prs3d_Presentation)&)
{
}
