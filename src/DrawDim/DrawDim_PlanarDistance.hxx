// Created on: 1996-01-10
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

#ifndef _DrawDim_PlanarDistance_HeaderFile
#define _DrawDim_PlanarDistance_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <DrawDim_PlanarDimension.hxx>
class TopoDS_Face;
class Draw_Display;
class gp_Pnt;
class TopoDS_Edge;


class DrawDim_PlanarDistance;
DEFINE_STANDARD_HANDLE(DrawDim_PlanarDistance, DrawDim_PlanarDimension)

//! PlanarDistance point/point
//! PlanarDistance point/line
//! PlanarDistance line/line
class DrawDim_PlanarDistance : public DrawDim_PlanarDimension
{

public:

  
  Standard_EXPORT DrawDim_PlanarDistance(const TopoDS_Face& plane, const TopoDS_Shape& point1, const TopoDS_Shape& point2);
  
  Standard_EXPORT DrawDim_PlanarDistance(const TopoDS_Shape& geom1, const TopoDS_Shape& geom2);
  
  //! private
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawDim_PlanarDistance,DrawDim_PlanarDimension)

protected:




private:

  
  Standard_EXPORT void Draw (const gp_Pnt& p, const TopoDS_Edge& e, Draw_Display& d) const;

  TopoDS_Shape myGeom1;
  TopoDS_Shape myGeom2;


};







#endif // _DrawDim_PlanarDistance_HeaderFile
