// Created on: 1995-12-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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


#include <TestTopOpeDraw_DrawableC3D.ixx>
#include <Draw_Interpretor.hxx>
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
void TestTopOpeDraw_DrawableC3D::ChangePnt(const gp_Pnt& P)
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
