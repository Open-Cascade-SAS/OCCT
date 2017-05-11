// Created on: 1995-08-09
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1995-1999 Matra Datavision
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

#include <AIS_Point.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Geom_Point.hxx>
#include <Geom_Transformation.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_Point.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Point,AIS_InteractiveObject)

//=======================================================================
//function : AIS_Point
//purpose  : 
//=======================================================================
AIS_Point::AIS_Point(const Handle(Geom_Point)& aComponent):
myComponent(aComponent),
myHasTOM(Standard_False),
myTOM(Aspect_TOM_PLUS)
{
  myHilightDrawer = new Prs3d_Drawer();
  myHilightDrawer->SetDisplayMode (-99);
  myHilightDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_GRAY80, 3.0));
  myHilightDrawer->SetColor (Quantity_NOC_GRAY80);
  myDynHilightDrawer = new Prs3d_Drawer();
  myDynHilightDrawer->SetDisplayMode (-99);
  myDynHilightDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_CYAN1, 3.0));
  myDynHilightDrawer->SetColor (Quantity_NOC_CYAN1);
}

//=======================================================================
//function : Component
//purpose  : 
//=======================================================================

Handle(Geom_Point) AIS_Point::Component()
{
  return myComponent;
}

//=======================================================================
//function : SetComponent
//purpose  : 
//=======================================================================

 void AIS_Point::SetComponent(const Handle(Geom_Point)& aComponent)
{
  myComponent = aComponent;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Point::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                        const Handle(Prs3d_Presentation)& aPresentation, 
                        const Standard_Integer aMode)
{
  aPresentation->SetInfiniteState(myInfiniteState);

  if (aMode==0)
    StdPrs_Point::Add(aPresentation,myComponent,myDrawer);
  else if (aMode== -99)
    {
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
      TheGroup->SetPrimitivesAspect (myHilightDrawer->PointAspect()->Aspect());
      Handle(Graphic3d_ArrayOfPoints) aPoint = new Graphic3d_ArrayOfPoints (1);
      aPoint->AddVertex (myComponent->X(),myComponent->Y(),myComponent->Z());
      TheGroup->AddPrimitiveArray (aPoint);
    }
    
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_Point::Compute(const Handle(Prs3d_Projector)& aProjector,
                        const Handle(Geom_Transformation)& aTransformation,
                        const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_Point::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Point::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                 const Standard_Integer /*aMode*/)
{
  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this,10);
//  eown -> SelectBasics_EntityOwner::Set(5);
  Handle(Select3D_SensitivePoint) sp = new Select3D_SensitivePoint(eown,
								   myComponent->Pnt());
  aSelection->Add(sp);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_Point::SetColor (const Quantity_Color& theCol)
{
  hasOwnColor=Standard_True;
  myDrawer->SetColor (theCol);
  UpdatePointValues();
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void AIS_Point::UnsetColor()
{
  hasOwnColor=Standard_False;
  UpdatePointValues();
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
TopoDS_Vertex AIS_Point::Vertex() const
{
  gp_Pnt P = myComponent->Pnt();
  return BRepBuilderAPI_MakeVertex(P);
}


//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Point::Compute(const Handle(Prs3d_Projector)&, 
			   const Handle(Prs3d_Presentation)&)
{
}

//=======================================================================
//function : SetMarker
//purpose  : 
//=======================================================================

void AIS_Point::SetMarker(const Aspect_TypeOfMarker aTOM)
{
  myTOM = aTOM;
  myHasTOM = Standard_True;
  UpdatePointValues();
}

//=======================================================================
//function : UnsetMarker
//purpose  : 
//=======================================================================
void AIS_Point::UnsetMarker()
{
  myHasTOM = Standard_False;
  UpdatePointValues();
}

//=======================================================================
//function : AcceptDisplayMode
//purpose  : 
//=======================================================================

 Standard_Boolean AIS_Point::AcceptDisplayMode (const Standard_Integer theMode) const
{
  return theMode == 0
      || theMode == -99;
}

//=======================================================================
//function : UpdatePointValues
//purpose  : 
//=======================================================================

void AIS_Point::UpdatePointValues()
{

  if(!hasOwnColor && myOwnWidth==0.0 && !myHasTOM)
  {
    myDrawer->SetPointAspect (Handle(Prs3d_PointAspect)());
    return;
  }
  Quantity_Color      aCol (Quantity_NOC_YELLOW);
  Aspect_TypeOfMarker aTOM = Aspect_TOM_PLUS;
  Standard_Real       aScale = 1.0;
  if (myDrawer->HasLink())
  {
    aCol   = myDrawer->Link()->PointAspect()->Aspect()->Color();
    aTOM   = myDrawer->Link()->PointAspect()->Aspect()->Type();
    aScale = myDrawer->Link()->PointAspect()->Aspect()->Scale();
  }

  if(hasOwnColor) aCol = myDrawer->Color();
  if(myOwnWidth!=0.0) aScale = myOwnWidth;
  if(myHasTOM) aTOM = myTOM;
  
  
  if(myDrawer->HasOwnPointAspect()){
    // CLE
    // const Handle(Prs3d_PointAspect) PA =  myDrawer->PointAspect();
    Handle(Prs3d_PointAspect) PA =  myDrawer->PointAspect();
    // ENDCLE
    PA->SetColor(aCol);
    PA->SetTypeOfMarker(aTOM);
    PA->SetScale(aScale);
  }
  else
    myDrawer->SetPointAspect(new Prs3d_PointAspect(aTOM,aCol,aScale));
}

