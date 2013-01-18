// Created on: 1997-02-10
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


//GER61351 //GG_171199     Enable to get an object RGB color instead a restricted object NameOfColor.



#include <AIS_GraphicTool.ixx>


#include <Quantity_Color.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Aspect_AspectLine.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_AngleAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <Prs3d_ShadingAspect.hxx>


static Handle(Prs3d_LineAspect) GetLineAspect(const Handle(Prs3d_Drawer)& Dr,
					      const AIS_TypeOfAttribute Att)
{
  switch(Att){
  case AIS_TOA_Line:
    return Dr->LineAspect();
    break;
  case AIS_TOA_Angle:
    return Dr->AngleAspect()->LineAspect();
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
    return Dr->UIsoAspect();
    break;
  case AIS_TOA_VIso:
    return Dr->VIsoAspect();
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
