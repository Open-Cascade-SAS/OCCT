// Created on: 1996-01-12
// Created by: Denis PASCAL
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _DrawDim_PlanarAngle_HeaderFile
#define _DrawDim_PlanarAngle_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <DrawDim_PlanarDimension.hxx>
class TopoDS_Face;
class Draw_Display;

class DrawDim_PlanarAngle;
DEFINE_STANDARD_HANDLE(DrawDim_PlanarAngle, DrawDim_PlanarDimension)

class DrawDim_PlanarAngle : public DrawDim_PlanarDimension
{

public:
  Standard_EXPORT DrawDim_PlanarAngle(const TopoDS_Face&  plane,
                                      const TopoDS_Shape& line1,
                                      const TopoDS_Shape& line2);

  Standard_EXPORT DrawDim_PlanarAngle(const TopoDS_Shape& line1, const TopoDS_Shape& line2);

  Standard_EXPORT void Sector(const Standard_Boolean inverted, const Standard_Boolean reversed);

  Standard_EXPORT void Position(const Standard_Real value);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(DrawDim_PlanarAngle, DrawDim_PlanarDimension)

protected:
private:
  TopoDS_Shape     myLine1;
  TopoDS_Shape     myLine2;
  Standard_Boolean myIsReversed;
  Standard_Boolean myIsInverted;
  Standard_Real    myPosition;
};

#endif // _DrawDim_PlanarAngle_HeaderFile
