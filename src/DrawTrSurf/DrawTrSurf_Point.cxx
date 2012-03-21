// Created on: 1994-03-28
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_IOS
# include <ios>
#elif defined(HAVE_IOS_H)
# include <ios.h>
#endif

#include <DrawTrSurf_Point.ixx>
#include <Standard_Stream.hxx>

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
#if defined(HAVE_IOS) && !defined(__sgi) && !defined(IRIX)
  ios::fmtflags F = S.flags();
  S.setf(ios::scientific,ios::floatfield);
  S.precision(15);
#else
  long form = S.setf(ios::scientific);
  int  prec = S.precision(15);
#endif
  if (is3D)
    S << "Point : " << myPoint.X() << ", " << myPoint.Y() << ", " << myPoint.Z() <<endl;
  else
    S << "Point 2d : " << myPoint.X() << ", " << myPoint.Y() <<endl;
#if defined(HAVE_IOS) && !defined(__sgi) && !defined(IRIX)
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
