// Created on: 1998-11-25
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _DrawDim_PlanarDiameter_HeaderFile
#define _DrawDim_PlanarDiameter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <DrawDim_PlanarDimension.hxx>
class TopoDS_Face;
class Draw_Display;

class DrawDim_PlanarDiameter;
DEFINE_STANDARD_HANDLE(DrawDim_PlanarDiameter, DrawDim_PlanarDimension)

class DrawDim_PlanarDiameter : public DrawDim_PlanarDimension
{

public:
  Standard_EXPORT DrawDim_PlanarDiameter(const TopoDS_Face& plane, const TopoDS_Shape& circle);

  Standard_EXPORT DrawDim_PlanarDiameter(const TopoDS_Shape& circle);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(DrawDim_PlanarDiameter, DrawDim_PlanarDimension)

protected:
private:
  TopoDS_Shape myCircle;
};

#endif // _DrawDim_PlanarDiameter_HeaderFile
