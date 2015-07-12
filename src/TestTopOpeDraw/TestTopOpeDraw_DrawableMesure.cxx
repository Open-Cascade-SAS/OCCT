// Created on: 1997-03-13
// Created by: Prestataire Mary FABIEN
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


#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Interpretor.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TestTopOpeDraw_DrawableC3D.hxx>
#include <TestTopOpeDraw_DrawableMesure.hxx>
#include <TestTopOpeDraw_DrawableP3D.hxx>
#include <TestTopOpeTools_Mesure.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>

#include <string.h>
static gp_Pnt PScale(const gp_Pnt& P,
		     const Standard_Real ScaleX,
		     const Standard_Real ScaleY)
{
  gp_Pnt Pnt;
  Pnt.SetX(P.X() * ScaleX);
  Pnt.SetY(P.Y() * ScaleY);
  Pnt.SetZ(0.);
  return Pnt;
}

static TColgp_Array1OfPnt& Scale(const TColgp_Array1OfPnt& AP,
				 const Standard_Real ScaleX,
				 const Standard_Real ScaleY)
{
  Standard_Integer i,up = AP.Upper();
  TColgp_Array1OfPnt *aAP = new TColgp_Array1OfPnt(1, up);
  for (i = 1; i <= up;i++) { 
    aAP->SetValue(i, PScale(AP.Value(i), ScaleX, ScaleY));
  }
  return *aAP;
}

Handle(TColgp_HArray1OfPnt) makepnt(const TestTopOpeTools_Mesure& M,
				    Handle(TColgp_HArray1OfPnt) T)

{
  Standard_Integer up = T->Upper();
  TColgp_Array1OfPnt& AOP = T->ChangeArray1();
  const TColgp_Array1OfPnt& MAOP = M.Pnts();
  for(Standard_Integer i= 1; i <= up; i++) {
    AOP.SetValue(i,MAOP.Value(i));
  }
  return T;
}

Handle(TColgp_HArray1OfPnt) makepnt(const TestTopOpeTools_Mesure& M)

{
  Standard_Integer up = M.NPnts();
  Handle(TColgp_HArray1OfPnt) T = new TColgp_HArray1OfPnt(1,up);
  TColgp_Array1OfPnt& AOP = T->ChangeArray1();
  const TColgp_Array1OfPnt& MAOP = M.Pnts();
  for(Standard_Integer i= 1; i <= up; i++) {
    AOP.SetValue(i,MAOP.Value(i));
  }
  return T;
}

Handle(Geom_Curve) makecurvescale (const Handle(TColgp_HArray1OfPnt)& HP,
				   const Standard_Real ScaleX,
				   const Standard_Real ScaleY) 
{
  Handle(Geom_Curve) C;
  C = TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt(Scale(HP->Array1(),ScaleX,ScaleY));
  return C;
}

Handle(Geom_Curve) makecurve(const Handle(TColgp_HArray1OfPnt)& HP)
{
  Handle(Geom_Curve) C;
  C = TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt(HP->Array1());
  return C;
}


static Standard_CString makename(const TCollection_AsciiString C)
{
  char * temp = new char[C.Length()+2];
  temp[0] = ' ' ;
  temp[C.Length()+1] = 0;
  strncpy(&temp[1], C.ToCString(), C.Length());
  return temp;
}

static Standard_CString makename1(const gp_Pnt& P)
{
  TCollection_AsciiString C((Standard_Integer)P.Coord(1));

  char * temp = new char[C.Length()+2];
  temp[0]=' ';
  temp[C.Length()+1]=0;
  strncpy(&temp[1], C.ToCString(), C.Length());
  return temp;
}

static Standard_CString makename2(const gp_Pnt& P)
{
  TCollection_AsciiString C;

  C = "";C = C + P.Coord(2);
  TCollection_AsciiString S1 = C.Token(".",1);
  S1.RightJustify(15, ' ');
  TCollection_AsciiString S2 = C.Token(".",2);
  S2.Trunc(2);
  C = " ";C = C + S1 + "." + S2;

  char * temp = new char[C.Length()+1];
  temp[C.Length()]=0;
  strncpy(&temp[0], C.ToCString(), C.Length());
  return temp;
}

static Handle(Geom_Curve) DrawAxe1(const TColgp_Array1OfPnt& T,
				   const Standard_Real ScaleX)
{
  Standard_Integer up = T.Upper();
  TColgp_Array1OfPnt *AOP = new TColgp_Array1OfPnt(1, up);
  for(Standard_Integer i= 1; i <= up; i++) {
    gp_Pnt pnt(T.Value(i).Coord(1) * ScaleX, 0, 0);
    (*AOP).SetValue(i, pnt);
  }
  Handle(Geom_Curve) C;
  C = TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt(*AOP);
  return C;
}

static Handle(Geom_Curve) DrawAxe2(const TColgp_Array1OfPnt& T,
				   const Standard_Real ScaleY)
{
  Standard_Integer up = T.Upper();
  TColgp_Array1OfPnt *AOP = new TColgp_Array1OfPnt(1, up);
  for(Standard_Integer i= 1; i <= up; i++) {
    gp_Pnt pnt(0, T.Value(i).Coord(2) * ScaleY, 0);
    (*AOP).SetValue(i, pnt);
  }
  Handle(Geom_Curve) C;
  C = TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt(*AOP);
  return C;
}

//=======================================================================
//function : TestTopOpeDraw_DrawableMesure
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableMesure::TestTopOpeDraw_DrawableMesure
(const TestTopOpeTools_Mesure& M,
 const Draw_Color& CurveColor,
 const Draw_Color& TextColor,
 const Standard_Real ScaleX,
 const Standard_Real ScaleY) :
     TestTopOpeDraw_DrawableC3D(::makecurvescale(makepnt(M),ScaleX,ScaleY),
				  CurveColor, ::makename(M.Name())
				  , TextColor)
     ,myP(makepnt(M))
     ,myAXE1(new TestTopOpeDraw_DrawableC3D(DrawAxe1(myP->Array1(), ScaleX),
					      Draw_saumon))
     ,myAXE2(new TestTopOpeDraw_DrawableC3D(DrawAxe2(myP->Array1(), ScaleY),
					      Draw_saumon))
     ,myScaleX(ScaleX)
     ,myScaleY(ScaleY)
{
  Standard_Integer up = myP->Upper(), i;
  myHDP   = new TestTopOpeDraw_HArray1OfDrawableP3D(1,up);
  myHADP1 = new TestTopOpeDraw_HArray1OfDrawableP3D(1,up);
  myHADP2 = new TestTopOpeDraw_HArray1OfDrawableP3D(1,up);
  for(i = 1; i <= up; i++) {
    Handle(TestTopOpeDraw_DrawableP3D) DP
      = new TestTopOpeDraw_DrawableP3D(PScale(myP->Value(i), ScaleX, ScaleY),
					 Draw_rouge);
    myHDP->SetValue(i,DP);
    gp_Pnt PA1(myP->Value(i).Coord(1), 0, 0);
    gp_Pnt PA1Sc(myP->Value(i).Coord(1) * ScaleX, 0, 0);
    Handle(TestTopOpeDraw_DrawableP3D) DPA1
      = new TestTopOpeDraw_DrawableP3D(PA1Sc,Draw_rose,
					 makename1(PA1),Draw_rose,-7,15);
    myHADP1->SetValue(i,DPA1);
    gp_Pnt PA2(0,myP->Value(i).Coord(2), 0);
    gp_Pnt PA2Sc(0,myP->Value(i).Coord(2) * myScaleY, 0);
    Handle(TestTopOpeDraw_DrawableP3D) DPA2
      = new TestTopOpeDraw_DrawableP3D(PA2Sc,Draw_rose,
					 makename2(PA2),Draw_rose,-120,5);
    myHADP2->SetValue(i,DPA2);
  }
}

//=======================================================================
//function : SetScaleX
//purpose : 
//=======================================================================

void TestTopOpeDraw_DrawableMesure::SetScaleX(const Standard_Real ScaleX)
{ 
  SetScale(ScaleX, myScaleY);
}

//=======================================================================
//function : SetScaleY
//purpose : 
//=======================================================================

void TestTopOpeDraw_DrawableMesure::SetScaleY(const Standard_Real ScaleY)
{
  SetScale(myScaleX, ScaleY);
}

//=======================================================================
//function : SetScale
//purpose : 
//=======================================================================

void TestTopOpeDraw_DrawableMesure::SetScale(const Standard_Real ScaleX,
					     const Standard_Real ScaleY)
{
  myScaleX = ScaleX;
  myScaleY = ScaleY;
  Handle(Geom_Curve) GC = ::makecurvescale(myP, myScaleX, myScaleY);
  ChangeCurve(GC);
  ChangePnt(Pnt());
  GC = DrawAxe1(myP->Array1(), myScaleX);
  myAXE1->ChangeCurve(GC);
  myAXE1->ChangePnt(Pnt());
  GC = DrawAxe2(myP->Array1(), myScaleY);
  myAXE2->ChangeCurve(GC);
  myAXE2->ChangePnt(Pnt());
  Standard_Integer up = myP->Upper(), i;
  for(i = 1; i <= up; i++) {
    Handle(TestTopOpeDraw_DrawableP3D) DP = myHDP->Value(i);
    gp_Pnt P = PScale(myP->Value(i), myScaleX, myScaleY);
    DP->ChangePnt(P);
    myHDP->SetValue(i,DP);

    Handle(TestTopOpeDraw_DrawableP3D) DPA1 = myHADP1->Value(i);
    gp_Pnt PA1(myP->Value(i).Coord(1) * myScaleX, 0, 0);
    DPA1->ChangePnt(PA1);
    myHADP1->SetValue(i,DPA1);

    Handle(TestTopOpeDraw_DrawableP3D) DPA2 = myHADP2->Value(i);
    gp_Pnt PA2(0, myP->Value(i).Coord(2) * myScaleY, 0);
    DPA2->ChangePnt(PA2);
    myHADP2->SetValue(i,DPA2);
  }
}

//=======================================================================
//function : Pnt
//purpose : 
//=======================================================================

gp_Pnt TestTopOpeDraw_DrawableMesure::Pnt() const
{
  const Handle(Geom_Curve)& GC = DrawTrSurf_Curve::GetCurve();
  Standard_Real f = GC->FirstParameter();
  Standard_Real l = GC->LastParameter();
  Standard_Real t = 0.;
  Standard_Real p = t*f + (1-t)*l;
  gp_Pnt P = GC->Value(p);
  return P;
}

//=======================================================================
//function : Pnts
//purpose  : 
//=======================================================================

const Handle(TColgp_HArray1OfPnt)& TestTopOpeDraw_DrawableMesure::Pnts() const
{
  return myP;
}

//=======================================================================
//function : SetName
//purpose : 
//=======================================================================

void TestTopOpeDraw_DrawableMesure::SetName(const TCollection_AsciiString& Name)
{
  Standard_CString Str = makename(Name);
  ChangeText(Str);
}

//=======================================================================
//function : Clear
//purpose : 
//=======================================================================

void TestTopOpeDraw_DrawableMesure::Clear()
{
  myScaleX = 1;
  myScaleY = 1;
  
}

//=======================================================================
//function : Whatis
//purpose : 
//=======================================================================

void  TestTopOpeDraw_DrawableMesure::Whatis(Draw_Interpretor& s)const 
{
  if (!myP.IsNull())
    s << "DrawableMesure";
}

//=======================================================================
//function : DrawOn
//purpose : 
//=======================================================================

void TestTopOpeDraw_DrawableMesure::DrawOn(Draw_Display& dis) const
{
  Standard_Integer i,n;

  // les axes
  n = myHADP1->Upper();
  myAXE1->DrawOn(dis);
  for(i = 1; i <= n; i++) {
    myHADP1->Value(i)->DrawOn(dis);
  }

  myAXE2->DrawOn(dis);
  n = myHADP2->Upper();
  for(i = 1; i <= n; i++) {
    myHADP2->Value(i)->DrawOn(dis);
  }

  // la courbe et ses points
  TestTopOpeDraw_DrawableC3D::DrawOn(dis);
  n = myHDP->Upper();
  for(i = 1; i <= n; i++) {
    myHDP->Value(i)->DrawOn(dis);
    myHADP1->Value(i)->DrawOn(dis);
    myHADP2->Value(i)->DrawOn(dis);
  }
}
