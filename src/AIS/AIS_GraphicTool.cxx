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
#include <Aspect_AspectLine.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Quantity_Color.hxx>

static Handle(Prs3d_LineAspect) GetLineAspect(const Handle(Prs3d_Drawer)& Dr,
					      const AIS_TypeOfAttribute Att)
{
  switch(Att){
  case AIS_TOA_Line:
    return Dr->LineAspect();
    break;
  case AIS_TOA_Dimension:
    return Dr->DimensionAspect()->LineAspect();
    break;
  case AIS_TOA_Wire:
    return Dr->WireAspect();
    break;
  case AIS_TOA_Plane:
    return Dr->PlaneAspect()->EdgesAspect();
    break;
  case AIS_TOA_Vector:
    return Dr->VectorAspect();
    break;
  case AIS_TOA_UIso:
    return Handle(Prs3d_LineAspect) (Dr->UIsoAspect());
    break;
  case AIS_TOA_VIso:
    return Handle(Prs3d_LineAspect) (Dr->VIsoAspect());
    break;
  case AIS_TOA_Free:
    return Dr->FreeBoundaryAspect();
    break;
  case AIS_TOA_UnFree:
    return Dr->UnFreeBoundaryAspect();
    break;
  case AIS_TOA_Section:
    return Dr->SectionAspect();
    break;
  case AIS_TOA_Hidden:
    return Dr->HiddenLineAspect();
    break;
  case AIS_TOA_Seen:
    return Dr->SeenLineAspect();
    break;
  case AIS_TOA_FirstAxis:
    return Dr->DatumAspect()->FirstAxisAspect();
  case AIS_TOA_SecondAxis:
    return Dr->DatumAspect()->SecondAxisAspect();
  case AIS_TOA_ThirdAxis:
    return Dr->DatumAspect()->SecondAxisAspect();
  }
  Handle(Prs3d_LineAspect) bid;
  return bid;
}

Quantity_NameOfColor AIS_GraphicTool::GetLineColor (const Handle(Prs3d_Drawer)& Dr, const AIS_TypeOfAttribute Att)
{
  Quantity_Color color;
  GetLineColor(Dr,Att,color);
  return color.Name(); 
}

void AIS_GraphicTool::GetLineColor (const Handle(Prs3d_Drawer)& Dr, const AIS_TypeOfAttribute Att, Quantity_Color &aColor)
{
  Standard_Real W;
  Aspect_TypeOfLine TYP;
  GetLineAspect(Dr,Att)->Aspect()->Values(aColor,TYP,W);
}

Standard_Real AIS_GraphicTool::GetLineWidth (const Handle(Prs3d_Drawer)& Dr,
				  const AIS_TypeOfAttribute Att)
{
//  Quantity_NameOfColor Col;
  Standard_Real W;
  Aspect_TypeOfLine TYP;
  
  Handle(Prs3d_LineAspect) LA = GetLineAspect(Dr,Att);
  
  Quantity_Color QCol;
  LA->Aspect()->Values(QCol,TYP,W);
  return W;
}
Aspect_TypeOfLine AIS_GraphicTool::GetLineType (const Handle(Prs3d_Drawer)& Dr,
				 const AIS_TypeOfAttribute Att)
{
//  Quantity_NameOfColor Col;
  Standard_Real W;
  Aspect_TypeOfLine TYP;
  
  Handle(Prs3d_LineAspect) LA = GetLineAspect(Dr,Att);
  
  Quantity_Color QCol;
  LA->Aspect()->Values(QCol,TYP,W);
  return TYP;
}


void AIS_GraphicTool::GetLineAtt(const Handle(Prs3d_Drawer)& Dr,
			       const AIS_TypeOfAttribute Att,
			       Quantity_NameOfColor& Col,
			       Standard_Real& W,
			       Aspect_TypeOfLine& TYP)
{
  
  Handle(Prs3d_LineAspect) LA = GetLineAspect(Dr,Att);
  
  Quantity_Color QCol;
  LA->Aspect()->Values(QCol,TYP,W);
  Col = QCol.Name();
}

Quantity_NameOfColor AIS_GraphicTool::GetInteriorColor(const Handle(Prs3d_Drawer)& Dr)
{
  Quantity_Color color;
  GetInteriorColor(Dr,color);
  return color.Name();
}

void AIS_GraphicTool::GetInteriorColor(const Handle(Prs3d_Drawer)& Dr, Quantity_Color &aColor)
{
  Handle(Graphic3d_AspectFillArea3d) AFA = Dr->ShadingAspect()->Aspect();
  Aspect_InteriorStyle IS;
  Aspect_TypeOfLine T;
  Standard_Real W;
  Quantity_Color EC;
  AFA->Values(IS,aColor,EC,T,W);
}

Graphic3d_MaterialAspect AIS_GraphicTool::GetMaterial(const Handle(Prs3d_Drawer)& Dr)
{
  return Dr->ShadingAspect()->Aspect()->BackMaterial();
}
