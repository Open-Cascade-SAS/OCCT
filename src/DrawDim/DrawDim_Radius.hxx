// Created on: 1997-04-21
// Created by: Denis PASCAL
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

#ifndef _DrawDim_Radius_HeaderFile
#define _DrawDim_Radius_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <DrawDim_Dimension.hxx>
class TopoDS_Face;
class Draw_Display;


class DrawDim_Radius;
DEFINE_STANDARD_HANDLE(DrawDim_Radius, DrawDim_Dimension)


class DrawDim_Radius : public DrawDim_Dimension
{

public:

  
  Standard_EXPORT DrawDim_Radius(const TopoDS_Face& cylinder);
  
  Standard_EXPORT const TopoDS_Face& Cylinder() const;
  
  Standard_EXPORT void Cylinder (const TopoDS_Face& face);
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawDim_Radius,DrawDim_Dimension)

protected:




private:


  TopoDS_Face myCylinder;


};







#endif // _DrawDim_Radius_HeaderFile
