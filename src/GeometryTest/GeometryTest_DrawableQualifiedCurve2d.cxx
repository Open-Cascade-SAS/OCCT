// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <GeometryTest_DrawableQualifiedCurve2d.hxx>

#include <GccEnt.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeometryTest_DrawableQualifiedCurve2d, DrawTrSurf_Curve2d)

//=======================================================================
//function : GeometryTest_DrawableQualifiedCurve2d
//purpose  : 
//=======================================================================

GeometryTest_DrawableQualifiedCurve2d::GeometryTest_DrawableQualifiedCurve2d (const Handle(Geom2d_Curve)& theCurve,
                                                                              const GccEnt_Position thePosition,
                                                                              const Standard_Boolean theDispOrigin)
: DrawTrSurf_Curve2d (theCurve, theDispOrigin), myPosition (thePosition)
{
  look = Draw_orange;
}

//=======================================================================
//function : GeometryTest_DrawableQualifiedCurve2d
//purpose  : 
//=======================================================================

GeometryTest_DrawableQualifiedCurve2d::GeometryTest_DrawableQualifiedCurve2d (const Handle(Geom2d_Curve)& theCurve,
                                                                              const Draw_Color& theColor,
                                                                              const Standard_Integer theDiscret,
                                                                              const GccEnt_Position thePosition,
                                                                              const Standard_Boolean theDispOrigin,
                                                                              const Standard_Boolean theDispCurvRadius,
                                                                              const Standard_Real theRadiusMax,
                                                                              const Standard_Real theRatioOfRadius)
: DrawTrSurf_Curve2d (theCurve, theColor, theDiscret, theDispOrigin, theDispCurvRadius, theRadiusMax, theRatioOfRadius),
  myPosition (thePosition)
{
  look = Draw_orange;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void GeometryTest_DrawableQualifiedCurve2d::DrawOn (Draw_Display& theDisplay) const
{
  DrawTrSurf_Curve2d::DrawOn (theDisplay);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  GeometryTest_DrawableQualifiedCurve2d::Dump (Standard_OStream& theStream) const 
{
  theStream << "Qualified curve 2D: \n";
  theStream << "Position :" << GccEnt::PositionToString (myPosition) << "\n";
  DrawTrSurf_Curve2d::Dump (theStream);
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  GeometryTest_DrawableQualifiedCurve2d::Whatis (Draw_Interpretor& theDI)const 
{
  Handle(Standard_Type) aType = GetCurve()->DynamicType();

  if (aType == STANDARD_TYPE (Geom2d_Circle))
  {
    theDI << "qualified 2d Circle";
  }
  else if (aType == STANDARD_TYPE (Geom2d_Line))
  {
    theDI << "qualified 2d Line";
  }
}
