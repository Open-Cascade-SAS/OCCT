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

#ifndef _DrawDim_Distance_HeaderFile
#define _DrawDim_Distance_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <DrawDim_Dimension.hxx>
class Draw_Display;


class DrawDim_Distance;
DEFINE_STANDARD_HANDLE(DrawDim_Distance, DrawDim_Dimension)


class DrawDim_Distance : public DrawDim_Dimension
{

public:

  
  Standard_EXPORT DrawDim_Distance(const TopoDS_Face& plane1, const TopoDS_Face& plane2);
  
  Standard_EXPORT DrawDim_Distance(const TopoDS_Face& plane1);
  
  Standard_EXPORT const TopoDS_Face& Plane1() const;
  
  Standard_EXPORT void Plane1 (const TopoDS_Face& face);
  
  Standard_EXPORT const TopoDS_Face& Plane2() const;
  
  Standard_EXPORT void Plane2 (const TopoDS_Face& face);
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawDim_Distance,DrawDim_Dimension)

protected:




private:


  TopoDS_Face myPlane1;
  TopoDS_Face myPlane2;


};







#endif // _DrawDim_Distance_HeaderFile
