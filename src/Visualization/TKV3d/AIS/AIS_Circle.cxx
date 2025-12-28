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
#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitivePoly.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_DeflectionCurve.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Circle, AIS_InteractiveObject)

//=================================================================================================

AIS_Circle::AIS_Circle(const occ::handle<Geom_Circle>& aComponent)
    : AIS_InteractiveObject(PrsMgr_TOP_AllView),
      myComponent(aComponent),
      myUStart(0.0),
      myUEnd(2.0 * M_PI),
      myCircleIsArc(false),
      myIsFilledCircleSens(false)
{
}

//=================================================================================================

AIS_Circle::AIS_Circle(const occ::handle<Geom_Circle>& theComponent,
                       const double        theUStart,
                       const double        theUEnd,
                       const bool     theIsFilledCircleSens)
    : AIS_InteractiveObject(PrsMgr_TOP_AllView),
      myComponent(theComponent),
      myUStart(theUStart),
      myUEnd(theUEnd),
      myCircleIsArc(std::abs(std::abs(theUEnd - theUStart) - 2.0 * M_PI) > gp::Resolution()),
      myIsFilledCircleSens(theIsFilledCircleSens)
{
}

//=================================================================================================

void AIS_Circle::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                         const occ::handle<Prs3d_Presentation>& thePrs,
                         const int)
{
  if (myCircleIsArc)
  {
    ComputeArc(thePrs);
  }
  else
  {
    ComputeCircle(thePrs);
  }
}

//=================================================================================================

void AIS_Circle::ComputeSelection(const occ::handle<SelectMgr_Selection>& aSelection,
                                  const int /*aMode*/)
{

  if (myCircleIsArc)
    ComputeArcSelection(aSelection);
  else
    ComputeCircleSelection(aSelection);
}

//=================================================================================================

void AIS_Circle::replaceWithNewLineAspect(const occ::handle<Prs3d_LineAspect>& theAspect)
{
  if (!myDrawer->HasLink())
  {
    myDrawer->SetLineAspect(theAspect);
    return;
  }

  const occ::handle<Graphic3d_AspectLine3d> anAspectOld = myDrawer->LineAspect()->Aspect();
  const occ::handle<Graphic3d_AspectLine3d> anAspectNew =
    !theAspect.IsNull() ? theAspect->Aspect() : myDrawer->Link()->LineAspect()->Aspect();
  if (anAspectNew != anAspectOld)
  {
    myDrawer->SetLineAspect(theAspect);
    NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>> aReplaceMap;
    aReplaceMap.Bind(anAspectOld, anAspectNew);
    replaceAspects(aReplaceMap);
  }
}

//=================================================================================================

void AIS_Circle::SetColor(const Quantity_Color& aCol)
{
  hasOwnColor = true;
  myDrawer->SetColor(aCol);

  if (!myDrawer->HasOwnLineAspect())
  {
    double WW = HasWidth() ? myOwnWidth
                       : myDrawer->HasLink()
                         ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Line)
                         : 1.;
    replaceWithNewLineAspect(new Prs3d_LineAspect(aCol, Aspect_TOL_SOLID, WW));
  }
  else
  {
    myDrawer->LineAspect()->SetColor(aCol);
    SynchronizeAspects();
  }
}

//=================================================================================================

void AIS_Circle::SetWidth(const double aValue)
{
  myOwnWidth = (float)aValue;

  if (!myDrawer->HasOwnLineAspect())
  {
    Quantity_Color CC = Quantity_NOC_YELLOW;
    if (HasColor())
      CC = myDrawer->Color();
    else if (myDrawer->HasLink())
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_Line, CC);
    replaceWithNewLineAspect(new Prs3d_LineAspect(CC, Aspect_TOL_SOLID, aValue));
  }
  else
  {
    myDrawer->LineAspect()->SetWidth(aValue);
    SynchronizeAspects();
  }
}

//=================================================================================================

void AIS_Circle::UnsetColor()
{
  hasOwnColor = false;

  if (!HasWidth())
  {
    replaceWithNewLineAspect(occ::handle<Prs3d_LineAspect>());
  }
  else
  {
    Quantity_Color CC = Quantity_NOC_YELLOW;
    if (HasColor())
      CC = myDrawer->Color();
    else if (myDrawer->HasLink())
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_Line, CC);
    myDrawer->LineAspect()->SetColor(CC);
    myDrawer->SetColor(CC);
    SynchronizeAspects();
  }
}

//=================================================================================================

void AIS_Circle::UnsetWidth()
{
  if (!HasColor())
  {
    replaceWithNewLineAspect(occ::handle<Prs3d_LineAspect>());
  }
  else
  {
    float WW =
      myDrawer->HasLink()
        ? (float)AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Line)
        : 1.0f;
    myDrawer->LineAspect()->SetWidth(WW);
    myOwnWidth = WW;
  }
}

//=================================================================================================

void AIS_Circle::ComputeCircle(const occ::handle<Prs3d_Presentation>& thePresentation)
{

  GeomAdaptor_Curve curv(myComponent);
  double     prevdev = myDrawer->DeviationCoefficient();
  myDrawer->SetDeviationCoefficient(1.e-5);
  StdPrs_DeflectionCurve::Add(thePresentation, curv, myDrawer);
  myDrawer->SetDeviationCoefficient(prevdev);
}

//=================================================================================================

void AIS_Circle::ComputeArc(const occ::handle<Prs3d_Presentation>& thePresentation)
{
  GeomAdaptor_Curve curv(myComponent, myUStart, myUEnd);
  double     prevdev = myDrawer->DeviationCoefficient();
  myDrawer->SetDeviationCoefficient(1.e-5);
  StdPrs_DeflectionCurve::Add(thePresentation, curv, myDrawer);
  myDrawer->SetDeviationCoefficient(prevdev);
}

//=================================================================================================

void AIS_Circle::ComputeCircleSelection(const occ::handle<SelectMgr_Selection>& theSelection)
{
  occ::handle<SelectMgr_EntityOwner>    anOwner = new SelectMgr_EntityOwner(this);
  occ::handle<Select3D_SensitiveCircle> aCirc =
    new Select3D_SensitiveCircle(anOwner, myComponent->Circ(), myIsFilledCircleSens);
  theSelection->Add(aCirc);
}

//=================================================================================================

void AIS_Circle::ComputeArcSelection(const occ::handle<SelectMgr_Selection>& theSelection)
{
  occ::handle<SelectMgr_EntityOwner>  anOwner = new SelectMgr_EntityOwner(this);
  occ::handle<Select3D_SensitivePoly> aSeg    = new Select3D_SensitivePoly(anOwner,
                                                                   myComponent->Circ(),
                                                                   myUStart,
                                                                   myUEnd,
                                                                   myIsFilledCircleSens);
  theSelection->Add(aSeg);
}
