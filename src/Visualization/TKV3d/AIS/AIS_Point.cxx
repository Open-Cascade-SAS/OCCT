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
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_Point.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Point, AIS_InteractiveObject)

//=================================================================================================

AIS_Point::AIS_Point(const occ::handle<Geom_Point>& aComponent)
    : myComponent(aComponent),
      myHasTOM(false),
      myTOM(Aspect_TOM_PLUS)
{
  myHilightDrawer = new Prs3d_Drawer();
  myHilightDrawer->SetDisplayMode(-99);
  myHilightDrawer->SetPointAspect(new Prs3d_PointAspect(Aspect_TOM_PLUS, Quantity_NOC_GRAY80, 3.0));
  myHilightDrawer->SetColor(Quantity_NOC_GRAY80);
  myHilightDrawer->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
  myDynHilightDrawer = new Prs3d_Drawer();
  myDynHilightDrawer->SetDisplayMode(-99);
  myDynHilightDrawer->SetPointAspect(
    new Prs3d_PointAspect(Aspect_TOM_PLUS, Quantity_NOC_CYAN1, 3.0));
  myDynHilightDrawer->SetColor(Quantity_NOC_CYAN1);
  myDynHilightDrawer->SetZLayer(Graphic3d_ZLayerId_Top);
}

//=================================================================================================

occ::handle<Geom_Point> AIS_Point::Component()
{
  return myComponent;
}

//=================================================================================================

void AIS_Point::SetComponent(const occ::handle<Geom_Point>& aComponent)
{
  myComponent = aComponent;
}

//=================================================================================================

void AIS_Point::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                        const occ::handle<Prs3d_Presentation>& thePrs,
                        const int            theMode)
{
  thePrs->SetInfiniteState(myInfiniteState);
  if (theMode == 0)
  {
    StdPrs_Point::Add(thePrs, myComponent, myDrawer);
  }
  else if (theMode == -99)
  {
    occ::handle<Graphic3d_Group> aGroup = thePrs->CurrentGroup();
    aGroup->SetPrimitivesAspect(myHilightDrawer->PointAspect()->Aspect());
    occ::handle<Graphic3d_ArrayOfPoints> aPoint = new Graphic3d_ArrayOfPoints(1);
    aPoint->AddVertex(myComponent->X(), myComponent->Y(), myComponent->Z());
    aGroup->AddPrimitiveArray(aPoint);
  }
}

//=================================================================================================

void AIS_Point::ComputeSelection(const occ::handle<SelectMgr_Selection>& aSelection,
                                 const int /*aMode*/)
{
  occ::handle<SelectMgr_EntityOwner>   eown = new SelectMgr_EntityOwner(this, 10);
  occ::handle<Select3D_SensitivePoint> sp   = new Select3D_SensitivePoint(eown, myComponent->Pnt());
  aSelection->Add(sp);
}

//=================================================================================================

void AIS_Point::SetColor(const Quantity_Color& theCol)
{
  hasOwnColor = true;
  myDrawer->SetColor(theCol);
  UpdatePointValues();
}

//=================================================================================================

void AIS_Point::UnsetColor()
{
  hasOwnColor = false;
  UpdatePointValues();
}

//=================================================================================================

TopoDS_Vertex AIS_Point::Vertex() const
{
  gp_Pnt P = myComponent->Pnt();
  return BRepBuilderAPI_MakeVertex(P);
}

//=================================================================================================

void AIS_Point::SetMarker(const Aspect_TypeOfMarker aTOM)
{
  myTOM    = aTOM;
  myHasTOM = true;
  UpdatePointValues();
}

//=================================================================================================

void AIS_Point::UnsetMarker()
{
  myHasTOM = false;
  UpdatePointValues();
}

//=================================================================================================

bool AIS_Point::AcceptDisplayMode(const int theMode) const
{
  return theMode == 0 || theMode == -99;
}

//=================================================================================================

void AIS_Point::replaceWithNewPointAspect(const occ::handle<Prs3d_PointAspect>& theAspect)
{
  if (!myDrawer->HasLink())
  {
    myDrawer->SetPointAspect(theAspect);
    return;
  }

  const occ::handle<Graphic3d_AspectMarker3d> anAspectOld = myDrawer->PointAspect()->Aspect();
  const occ::handle<Graphic3d_AspectMarker3d> anAspectNew =
    !theAspect.IsNull() ? theAspect->Aspect() : myDrawer->Link()->PointAspect()->Aspect();
  if (anAspectNew != anAspectOld)
  {
    myDrawer->SetPointAspect(theAspect);
    NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>> aReplaceMap;
    aReplaceMap.Bind(anAspectOld, anAspectNew);
    replaceAspects(aReplaceMap);
  }
}

//=================================================================================================

void AIS_Point::UpdatePointValues()
{
  if (!hasOwnColor && myOwnWidth == 0.0f && !myHasTOM)
  {
    replaceWithNewPointAspect(occ::handle<Prs3d_PointAspect>());
    return;
  }

  Quantity_Color      aCol(Quantity_NOC_YELLOW);
  Aspect_TypeOfMarker aTOM   = Aspect_TOM_PLUS;
  double       aScale = 1.0;
  if (myDrawer->HasLink())
  {
    aCol   = myDrawer->Link()->PointAspect()->Aspect()->Color();
    aTOM   = myDrawer->Link()->PointAspect()->Aspect()->Type();
    aScale = myDrawer->Link()->PointAspect()->Aspect()->Scale();
  }

  if (hasOwnColor)
    aCol = myDrawer->Color();
  if (myOwnWidth != 0.0f)
    aScale = myOwnWidth;
  if (myHasTOM)
    aTOM = myTOM;

  if (myDrawer->HasOwnPointAspect())
  {
    occ::handle<Prs3d_PointAspect> PA = myDrawer->PointAspect();
    PA->SetColor(aCol);
    PA->SetTypeOfMarker(aTOM);
    PA->SetScale(aScale);
    SynchronizeAspects();
  }
  else
  {
    replaceWithNewPointAspect(new Prs3d_PointAspect(aTOM, aCol, aScale));
  }
}
