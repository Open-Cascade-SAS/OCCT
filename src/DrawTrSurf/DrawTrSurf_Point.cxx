// Created on: 1994-03-28
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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
#include <Draw_Drawable3D.hxx>
#include <DrawTrSurf_Point.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_Point,Draw_Drawable3D)

//=======================================================================
//function : DrawTrSurf_Point
//purpose  : 
//=======================================================================
DrawTrSurf_Point::DrawTrSurf_Point(const gp_Pnt& P, 
				   const Draw_MarkerShape Shape,
				   const Draw_Color& Col) :
       myPoint(P),
       is3D(Standard_True),
       myShape(Shape),
       myColor(Col)
{
}

//=======================================================================
//function : DrawTrSurf_Point
//purpose  : 
//=======================================================================

DrawTrSurf_Point::DrawTrSurf_Point(const gp_Pnt2d& P, 
				   const Draw_MarkerShape Shape,
				   const Draw_Color& Col) :
       myPoint(P.X(),P.Y(),0.),
       is3D(Standard_False),
       myShape(Shape),
       myColor(Col)
{
}

//=======================================================================
//function : Is3D
//purpose  : 
//=======================================================================

Standard_Boolean DrawTrSurf_Point::Is3D() const
{
  return is3D;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Point::DrawOn(Draw_Display& dis) const 
{
  dis.SetColor(myColor);
  if (is3D)
    dis.DrawMarker(myPoint,myShape);
  else
    dis.DrawMarker(Point2d(),myShape);
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_Pnt DrawTrSurf_Point::Point() const 
{
  return myPoint;
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

void DrawTrSurf_Point::Point(const gp_Pnt& P)
{
  myPoint = P;
  is3D = Standard_True;
}

//=======================================================================
//function : Point2d
//purpose  : 
//=======================================================================

gp_Pnt2d DrawTrSurf_Point::Point2d() const 
{
  return gp_Pnt2d(myPoint.X(),myPoint.Y());
}

//=======================================================================
//function : Point2d
//purpose  : 
//=======================================================================

void DrawTrSurf_Point::Point2d(const gp_Pnt2d& P)
{
  myPoint.SetCoord(P.X(),P.Y(),0);
  is3D = Standard_False;
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================

void DrawTrSurf_Point::Color(const Draw_Color& aColor)
{
  myColor = aColor;
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================

Draw_Color DrawTrSurf_Point::Color() const 
{
  return myColor;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

void DrawTrSurf_Point::Shape(const Draw_MarkerShape S)
{
  myShape = S;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

Draw_MarkerShape DrawTrSurf_Point::Shape() const 
{
  return myShape;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DrawTrSurf_Point::Copy() const 
{
  Handle(DrawTrSurf_Point) P;
  if (is3D)
    P = new DrawTrSurf_Point(myPoint,myShape,myColor);
  else
    P = new DrawTrSurf_Point(Point2d(),myShape,myColor);
    
  return P;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DrawTrSurf_Point::Dump(Standard_OStream& S) const 
{
#if !defined(_MSC_VER) && !defined(__sgi) && !defined(IRIX)
  ios::fmtflags F = S.flags();
  S.setf(ios::scientific,ios::floatfield);
  S.precision(15);
#else
  long form = S.setf(ios::scientific);
  std::streamsize prec = S.precision(15);
#endif
  if (is3D)
    S << "Point : " << myPoint.X() << ", " << myPoint.Y() << ", " << myPoint.Z() <<endl;
  else
    S << "Point 2d : " << myPoint.X() << ", " << myPoint.Y() <<endl;
#if !defined(_MSC_VER) && !defined(__sgi) && !defined(IRIX)
  S.setf(F);
#else
  S.setf(form);
  S.precision(prec);
#endif
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DrawTrSurf_Point::Whatis(Draw_Interpretor& S) const 
{
    S << "point";
}
