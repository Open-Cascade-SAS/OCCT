// Created on: 1991-07-15
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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
#include <ElCLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

extern Standard_Boolean Draw_Bounds;


//=======================================================================
//function : DrawMarker
//purpose  : 
//=======================================================================

void Draw_Display::DrawMarker (const gp_Pnt& pt, 
			       const Draw_MarkerShape S, 
			       const Standard_Integer Size)
{
  gp_Pnt2d p;
  Project(pt,p);
  DrawMarker(p,S,Size);
}

//=======================================================================
//function : DrawMarker
//purpose  : 
//=======================================================================

void Draw_Display::DrawMarker (const gp_Pnt2d& pt, 
			       const Draw_MarkerShape S, 
			       const Standard_Integer ISize)
{
  Draw_Bounds = Standard_False;

  gp_Pnt2d p1 = pt;
  gp_Pnt2d p2 = p1;
  gp_Circ2d C;
  Standard_Real Size = ((Standard_Real) ISize) / Zoom();
  
  switch (S) {
    
  case Draw_Square :
    p1.Translate(gp_Vec2d(-Size,-Size));
    p2.Translate(gp_Vec2d( Size,-Size));
    Draw(p1,p2);
    p1.Translate(gp_Vec2d(2*Size,2*Size));
    Draw(p1,p2);
    p2.Translate(gp_Vec2d(-2*Size,2*Size));
    Draw(p1,p2);
    p1.Translate(gp_Vec2d(-2*Size,-2*Size));
    Draw(p1,p2);
    break;

  case Draw_Losange :
    p1.Translate(gp_Vec2d(-Size,0));
    p2.Translate(gp_Vec2d( 0,Size));
    Draw(p1,p2);
    p1.Translate(gp_Vec2d(2*Size,0));
    Draw(p1,p2);
    p2.Translate(gp_Vec2d(0,-2*Size));
    Draw(p1,p2);
    p1.Translate(gp_Vec2d(-2*Size,0));
    Draw(p1,p2);
    break;

  case Draw_X :
    p1.Translate(gp_Vec2d(-Size,-Size));
    p2.Translate(gp_Vec2d( Size,Size));
    Draw(p1,p2);
    p1.Translate(gp_Vec2d(2*Size,0));
    p2.Translate(gp_Vec2d(-2*Size,0));
    Draw(p1,p2);
    break;

  case Draw_Plus :
    p1.Translate(gp_Vec2d(-Size,0));
    p2.Translate(gp_Vec2d( Size,0));
    Draw(p1,p2);
    p1.Translate(gp_Vec2d(Size,Size));
    p2.Translate(gp_Vec2d(-Size,-Size));
    Draw(p1,p2);
    break;
    
  case Draw_Circle :
//    gp_Circ2d C;
    C.SetRadius(ISize);
    C.SetLocation(pt);
    Draw(C, 0, 2*M_PI, Standard_False);
    break;
  default:
    break;
    
  }
  Draw_Bounds = Standard_True;
  MoveTo(pt);
}

//=======================================================================
//function : DrawMarker
//purpose  : 
//=======================================================================

void Draw_Display::DrawMarker (const gp_Pnt& pt, 
			       const Draw_MarkerShape S, 
			       const Standard_Real Size)
{
  gp_Pnt2d p;
  Project(pt,p);
  DrawMarker(p,S,Size);
}

//=======================================================================
//function : DrawMarker
//purpose  : 
//=======================================================================

void Draw_Display::DrawMarker (const gp_Pnt2d& pt, 
			       const Draw_MarkerShape S, 
			       const Standard_Real R)
{
  switch (S) {
  case Draw_Square :
  case Draw_Losange :
  case Draw_X :
  case Draw_Plus :
  case Draw_Circle :
    {
      Standard_Integer I = (Standard_Integer ) R;
      if(!I) return;
      DrawMarker(pt, S, I);
      break;
    }
  case Draw_CircleZoom :
    if(R == 0.0) return;
    gp_Circ2d C;
    C.SetRadius(R);
    C.SetLocation(pt);
    // if the circus is too small, a "plus" is drawn to mark the point 
    Standard_Boolean b = (R * Zoom()) > 2;
    if(b)
      Draw(C, 0, 2*M_PI);
    else
      DrawMarker(pt, Draw_Plus);
  }
  Draw_Bounds = Standard_True;
  MoveTo(pt);
}

#define MAXPNT 200
#define DEFLECTION 5

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void Draw_Display::Draw(const gp_Circ& C, const Standard_Real A1,const Standard_Real A3,
			const Standard_Boolean ModifyWithZoom)
{
  Standard_Real A2 = A3;
  while (A2 < A1) A2 += 2*M_PI;
  
  Standard_Real angle = DEFLECTION / (C.Radius() * Zoom());
  Standard_Integer n = (Standard_Integer )( (A2 - A1) / angle);
  if (n > MAXPNT) {
    angle = (A2 - A1) / MAXPNT;
    n = MAXPNT;
  }
  if (n <= 6) {
    angle = (A2 - A1) / 6;
    n = 6;
  }
  Standard_Real c = 2*Cos(angle);
  
  gp_Circ Cloc(C);
  if(!ModifyWithZoom) {
    Standard_Integer ISize = (Standard_Integer )( Cloc.Radius() / Zoom());
    Cloc.SetRadius(ISize);
  }
  
  gp_Pnt PC = Cloc.Location();
  gp_Pnt P = ElCLib::Value(A1,Cloc);
  MoveTo(P);
  gp_Vec V1(PC,P);
  P = ElCLib::Value(A1+angle,Cloc);
  gp_Vec V2(PC,P);
  DrawTo(P);
  gp_Vec V;

  for (Standard_Integer i = 2; i < n; i++) {
    V = c * V2 - V1;
    V1 = V2;
    V2 = V;
    DrawTo(PC.Translated(V));
  }
  
  P = ElCLib::Value(A2,Cloc);
  DrawTo(P);
}

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void Draw_Display::Draw(const gp_Circ2d& C, const Standard_Real A1, const Standard_Real A3,
			const Standard_Boolean ModifyWithZoom)
{
  Standard_Real A2 = A3;
  while (A2 < A1) A2 += 2*M_PI;
  
  Standard_Real angle = DEFLECTION / (C.Radius() * Zoom());
  Standard_Integer n = (Standard_Integer )( (A2 - A1) / angle);
  if (n > MAXPNT) {
    angle = (A2 - A1) / MAXPNT;
    n = MAXPNT;
  }
  else if (n <= 6) {
    angle = (A2 - A1) / 6;
    n = 6;
  }
  Standard_Real c = 2*Cos(angle);
  
  gp_Circ2d Cloc(C);
  if(!ModifyWithZoom) {// the effet of zoom is cancelled to follow
    Standard_Real Size = Cloc.Radius() / Zoom();
    Cloc.SetRadius(Size);
  }
  
  gp_Pnt2d PC = Cloc.Location();
  gp_Pnt2d P = ElCLib::Value(A1,Cloc);
  MoveTo(P);
  gp_Vec2d V1(PC,P);
  P = ElCLib::Value(A1+angle,Cloc);
  gp_Vec2d V2(PC,P);
  DrawTo(P);
  gp_Vec2d V;

  for (Standard_Integer i = 2; i < n; i++) {
    V = c * V2 - V1;
    V1 = V2;
    V2 = V;
    DrawTo(PC.Translated(V));
  }
  
  P = ElCLib::Value(A2,Cloc);
  DrawTo(P);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Pnt2d Draw_Display::Project(const gp_Pnt& p) const
{
  gp_Pnt2d pt;
  Project(p,pt);
  return pt;
}
