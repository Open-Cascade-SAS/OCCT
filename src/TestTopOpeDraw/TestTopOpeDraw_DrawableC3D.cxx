// Created on: 1995-12-01
// Created by: Jean Yves LEBEY
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


#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Text3D.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TestTopOpeDraw_DrawableC3D.hxx>
#include <TopOpeBRepTool_define.hxx>

//=======================================================================
//function : TestTopOpeDraw_DrawableC3D
//purpose  : 
//=======================================================================
TestTopOpeDraw_DrawableC3D::TestTopOpeDraw_DrawableC3D
(const Handle(Geom_Curve)& C, const Draw_Color& CColor, 
 const Standard_CString Text, const Draw_Color& TextColor,
 const Standard_Integer Discret, const Standard_Real Deflect,const Standard_Integer DrawMode, const Standard_Boolean DispOrigin) :
 DrawTrSurf_Curve(C,CColor,Discret,Deflect,DrawMode,DispOrigin),
 myText(Text),myTextColor(TextColor)
{
  myText3D = new Draw_Text3D(Pnt(),Text,TextColor);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableC3D
//purpose : 
//=======================================================================
TestTopOpeDraw_DrawableC3D::TestTopOpeDraw_DrawableC3D
(const Handle(Geom_Curve)& C, const Draw_Color& CColor) :
 DrawTrSurf_Curve(C,CColor,16,0.01,1), //Discret,Deflect,DrawMode
 myText(""),
 myTextColor(CColor)
{
  DrawTrSurf_Curve::DisplayOrigin(Standard_True);
  myText3D = new Draw_Text3D(Pnt(),"",CColor);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableC3D
//purpose : 
//=======================================================================
TestTopOpeDraw_DrawableC3D::TestTopOpeDraw_DrawableC3D
(const Handle(Geom_Curve)& C, const Draw_Color& CColor,
 const Standard_CString Text, const Draw_Color& TextColor) :
 DrawTrSurf_Curve(C,CColor,16,0.01,1), //Discret,Deflect,DrawMode
 myText(Text),
 myTextColor(TextColor)
{
  DrawTrSurf_Curve::DisplayOrigin(Standard_True);
  myText3D = new Draw_Text3D(Pnt(),Text,TextColor);
}

//=======================================================================
//function : Pnt
//purpose : 
//=======================================================================
gp_Pnt TestTopOpeDraw_DrawableC3D::Pnt() const
{
  const Handle(Geom_Curve)& GC = GetCurve();
  Standard_Real f = GC->FirstParameter();
  Standard_Real l = GC->LastParameter();
  Standard_Real t = 1/2.;
  Standard_Real p = t*f + (1-t)*l;
  gp_Pnt P = GC->Value(p);
  return P;
}

//=======================================================================
//function : ChangePnt
//purpose  : 
//=======================================================================
void TestTopOpeDraw_DrawableC3D::ChangePnt(const gp_Pnt&)
{
  myText3D = new Draw_Text3D(Pnt(),myText,myTextColor);
}

//=======================================================================
//function : ChangeCurve
//purpose  : 
//=======================================================================
void TestTopOpeDraw_DrawableC3D::ChangeCurve(const Handle(Geom_Curve)& GC)
{
  curv = GC;
}

//=======================================================================
//function : ChangeText
//purpose  : 
//=======================================================================
void TestTopOpeDraw_DrawableC3D::ChangeText(const Standard_CString T)
{
  myText = T;
  myText3D = new Draw_Text3D(Pnt(),T,myTextColor);
}

//=======================================================================
//function : Whatis
//purpose : 
//=======================================================================
void  TestTopOpeDraw_DrawableC3D::Whatis(Draw_Interpretor& s)const 
{
  if (!curv.IsNull()) s << "DrawableC3D";
}

//=======================================================================
//function : DrawOn
//purpose : 
//=======================================================================
void TestTopOpeDraw_DrawableC3D::DrawOn(Draw_Display& dis) const
{
  DrawTrSurf_Curve::DrawOn(dis);
  myText3D->DrawOn(dis);
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================
void TestTopOpeDraw_DrawableC3D::Name(const Standard_CString N) 
{
  char* temp = new char[2];
  temp[0] = ' ' ;
  temp[1] = 0;  
  strcat(temp, N);
  Draw_Drawable3D::Name(temp);
  ChangeText(temp);
}
