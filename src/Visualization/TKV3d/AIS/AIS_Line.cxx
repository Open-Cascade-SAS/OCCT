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
#include <Geom_Line.hxx>
#include <Geom_Point.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_Curve.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Line, AIS_InteractiveObject)

//=================================================================================================

AIS_Line::AIS_Line(const Handle(Geom_Line)& aComponent)
    : myComponent(aComponent),
      myLineIsSegment(Standard_False)
{
  SetInfiniteState();
}

//=================================================================================================

AIS_Line::AIS_Line(const Handle(Geom_Point)& aStartPoint, const Handle(Geom_Point)& aEndPoint)
    : myStartPoint(aStartPoint),
      myEndPoint(aEndPoint),
      myLineIsSegment(Standard_True)
{
}

//=================================================================================================

void AIS_Line::Compute(const Handle(PrsMgr_PresentationManager)&,
                       const Handle(Prs3d_Presentation)& thePrs,
                       const Standard_Integer)
{
  if (!myLineIsSegment)
  {
    ComputeInfiniteLine(thePrs);
  }
  else
  {
    ComputeSegmentLine(thePrs);
  }
}

//=================================================================================================

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

//=================================================================================================

void AIS_Line::replaceWithNewLineAspect(const Handle(Prs3d_LineAspect)& theAspect)
{
  if (!myDrawer->HasLink())
  {
    myDrawer->SetLineAspect(theAspect);
    return;
  }

  const Handle(Graphic3d_Aspects)& anAspectOld = myDrawer->LineAspect()->Aspect();
  const Handle(Graphic3d_Aspects)& anAspectNew =
    !theAspect.IsNull() ? theAspect->Aspect() : myDrawer->Link()->LineAspect()->Aspect();
  if (anAspectNew != anAspectOld)
  {
    myDrawer->SetLineAspect(theAspect);
    Graphic3d_MapOfAspectsToAspects aReplaceMap;
    aReplaceMap.Bind(anAspectOld, anAspectNew);
    replaceAspects(aReplaceMap);
  }
}

//=================================================================================================

void AIS_Line::SetColor(const Quantity_Color& aCol)
{
  hasOwnColor = Standard_True;
  myDrawer->SetColor(aCol);

  Standard_Real WW = HasWidth() ? myOwnWidth
                     : myDrawer->HasLink()
                       ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Line)
                       : 1.;

  if (!myDrawer->HasOwnLineAspect())
  {
    replaceWithNewLineAspect(new Prs3d_LineAspect(aCol, Aspect_TOL_SOLID, WW));
  }
  else
  {
    myDrawer->LineAspect()->SetColor(aCol);
    SynchronizeAspects();
  }
}

//=================================================================================================

void AIS_Line::UnsetColor()
{
  hasOwnColor = Standard_False;

  if (!HasWidth())
  {
    replaceWithNewLineAspect(Handle(Prs3d_LineAspect)());
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

void AIS_Line::SetWidth(const Standard_Real aValue)
{
  myOwnWidth = (Standard_ShortReal)aValue;

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

void AIS_Line::UnsetWidth()
{
  if (!HasColor())
  {
    replaceWithNewLineAspect(Handle(Prs3d_LineAspect)());
  }
  else
  {
    Standard_ShortReal WW =
      myDrawer->HasLink()
        ? (Standard_ShortReal)AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Line)
        : 1.0f;
    myDrawer->LineAspect()->SetWidth(WW);
    myOwnWidth = WW;
    SynchronizeAspects();
  }
}

//=================================================================================================

void AIS_Line::ComputeInfiniteLine(const Handle(Prs3d_Presentation)& aPresentation)
{
  GeomAdaptor_Curve curv(myComponent);
  StdPrs_Curve::Add(aPresentation, curv, myDrawer);

  // pas de prise en compte lors du FITALL
  aPresentation->SetInfiniteState(Standard_True);
}

//=================================================================================================

void AIS_Line::ComputeSegmentLine(const Handle(Prs3d_Presentation)& aPresentation)
{
  gp_Pnt P1 = myStartPoint->Pnt();
  gp_Pnt P2 = myEndPoint->Pnt();

  myComponent = new Geom_Line(P1, gp_Dir(P2.XYZ() - P1.XYZ()));

  Standard_Real     dist = P1.Distance(P2);
  GeomAdaptor_Curve curv(myComponent, 0., dist);
  StdPrs_Curve::Add(aPresentation, curv, myDrawer);
}

//=================================================================================================

void AIS_Line::ComputeInfiniteLineSelection(const Handle(SelectMgr_Selection)& aSelection)
{

  /*  // on calcule les points min max a partir desquels on cree un segment sensible...
    GeomAdaptor_Curve curv(myComponent);
    gp_Pnt P1,P2;
    FindLimits(curv,myDrawer->MaximalParameterValue(),P1,P2);
  */
  const gp_Dir& thedir  = myComponent->Position().Direction();
  const gp_Pnt& loc     = myComponent->Position().Location();
  const gp_XYZ& dir_xyz = thedir.XYZ();
  const gp_XYZ& loc_xyz = loc.XYZ();
  // POP  Standard_Real aLength = UnitsAPI::CurrentToLS (250000. ,"LENGTH");
  Standard_Real                     aLength = UnitsAPI::AnyToLS(250000., "mm");
  gp_Pnt                            P1      = loc_xyz + aLength * dir_xyz;
  gp_Pnt                            P2      = loc_xyz - aLength * dir_xyz;
  Handle(SelectMgr_EntityOwner)     eown    = new SelectMgr_EntityOwner(this, 5);
  Handle(Select3D_SensitiveSegment) seg     = new Select3D_SensitiveSegment(eown, P1, P2);
  aSelection->Add(seg);
}

//=================================================================================================

void AIS_Line::ComputeSegmentLineSelection(const Handle(SelectMgr_Selection)& aSelection)
{

  Handle(SelectMgr_EntityOwner)     eown = new SelectMgr_EntityOwner(this, 5);
  Handle(Select3D_SensitiveSegment) seg =
    new Select3D_SensitiveSegment(eown, myStartPoint->Pnt(), myEndPoint->Pnt());
  aSelection->Add(seg);
}
