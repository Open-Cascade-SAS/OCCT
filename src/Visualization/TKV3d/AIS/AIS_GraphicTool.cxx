// Created on: 1997-02-10
// Created by: Robert COUBLANC
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

#include <AIS_GraphicTool.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Quantity_Color.hxx>

static occ::handle<Prs3d_LineAspect> GetLineAspect(const occ::handle<Prs3d_Drawer>& Dr,
                                                   const AIS_TypeOfAttribute        Att)
{
  switch (Att)
  {
    case AIS_TOA_Line:
      return Dr->LineAspect();
    case AIS_TOA_Dimension:
      return Dr->DimensionAspect()->LineAspect();
    case AIS_TOA_Wire:
      return Dr->WireAspect();
    case AIS_TOA_Plane:
      return Dr->PlaneAspect()->EdgesAspect();
    case AIS_TOA_Vector:
      return Dr->VectorAspect();
    case AIS_TOA_UIso:
      return occ::handle<Prs3d_LineAspect>(Dr->UIsoAspect());
    case AIS_TOA_VIso:
      return occ::handle<Prs3d_LineAspect>(Dr->VIsoAspect());
    case AIS_TOA_Free:
      return Dr->FreeBoundaryAspect();
    case AIS_TOA_UnFree:
      return Dr->UnFreeBoundaryAspect();
    case AIS_TOA_Section:
      return Dr->SectionAspect();
    case AIS_TOA_Hidden:
      return Dr->HiddenLineAspect();
    case AIS_TOA_Seen:
      return Dr->SeenLineAspect();
    case AIS_TOA_FaceBoundary:
      return Dr->FaceBoundaryAspect();
    case AIS_TOA_FirstAxis:
      return Dr->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis);
    case AIS_TOA_SecondAxis:
      return Dr->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis);
    case AIS_TOA_ThirdAxis:
      return Dr->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis);
  }
  occ::handle<Prs3d_LineAspect> bid;
  return bid;
}

Quantity_NameOfColor AIS_GraphicTool::GetLineColor(const occ::handle<Prs3d_Drawer>& Dr,
                                                   const AIS_TypeOfAttribute        Att)
{
  Quantity_Color color;
  GetLineColor(Dr, Att, color);
  return color.Name();
}

void AIS_GraphicTool::GetLineColor(const occ::handle<Prs3d_Drawer>& Dr,
                                   const AIS_TypeOfAttribute        Att,
                                   Quantity_Color&                  aColor)
{
  aColor = GetLineAspect(Dr, Att)->Aspect()->Color();
}

double AIS_GraphicTool::GetLineWidth(const occ::handle<Prs3d_Drawer>& Dr,
                                     const AIS_TypeOfAttribute        Att)
{
  occ::handle<Prs3d_LineAspect> LA = GetLineAspect(Dr, Att);
  return LA->Aspect()->Width();
}

Aspect_TypeOfLine AIS_GraphicTool::GetLineType(const occ::handle<Prs3d_Drawer>& Dr,
                                               const AIS_TypeOfAttribute        Att)
{
  occ::handle<Prs3d_LineAspect> LA = GetLineAspect(Dr, Att);
  return LA->Aspect()->Type();
}

void AIS_GraphicTool::GetLineAtt(const occ::handle<Prs3d_Drawer>& Dr,
                                 const AIS_TypeOfAttribute        Att,
                                 Quantity_NameOfColor&            Col,
                                 double&                          W,
                                 Aspect_TypeOfLine&               TYP)
{
  occ::handle<Prs3d_LineAspect> LA = GetLineAspect(Dr, Att);
  Col                              = LA->Aspect()->Color().Name();
  W                                = LA->Aspect()->Width();
  TYP                              = LA->Aspect()->Type();
}

Quantity_NameOfColor AIS_GraphicTool::GetInteriorColor(const occ::handle<Prs3d_Drawer>& Dr)
{
  Quantity_Color color;
  GetInteriorColor(Dr, color);
  return color.Name();
}

void AIS_GraphicTool::GetInteriorColor(const occ::handle<Prs3d_Drawer>& Dr, Quantity_Color& aColor)
{
  occ::handle<Graphic3d_AspectFillArea3d> AFA = Dr->ShadingAspect()->Aspect();
  aColor                                      = AFA->InteriorColor();
}

Graphic3d_MaterialAspect AIS_GraphicTool::GetMaterial(const occ::handle<Prs3d_Drawer>& Dr)
{
  return Dr->ShadingAspect()->Aspect()->BackMaterial();
}
