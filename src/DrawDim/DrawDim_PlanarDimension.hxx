// Created on: 1996-01-09
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

#ifndef _DrawDim_PlanarDimension_HeaderFile
#define _DrawDim_PlanarDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <DrawDim_Dimension.hxx>


class DrawDim_PlanarDimension;
DEFINE_STANDARD_HANDLE(DrawDim_PlanarDimension, DrawDim_Dimension)

//! Dimensions between point, line and circle ON a plane
class DrawDim_PlanarDimension : public DrawDim_Dimension
{

public:

  
  Standard_EXPORT void SetPlane (const TopoDS_Face& plane);
  
  Standard_EXPORT TopoDS_Face GetPlane() const;




  DEFINE_STANDARD_RTTIEXT(DrawDim_PlanarDimension,DrawDim_Dimension)

protected:


  TopoDS_Face myPlane;


private:




};







#endif // _DrawDim_PlanarDimension_HeaderFile
