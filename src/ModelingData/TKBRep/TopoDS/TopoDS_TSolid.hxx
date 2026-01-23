// Created on: 1990-12-17
// Created by: Remi Lequette
// Copyright (c) 1990-1999 Matra Datavision
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

#ifndef _TopoDS_TSolid_HeaderFile
#define _TopoDS_TSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_TShape.hxx>

//! A Topological part of 3D space, bounded by shells,
//! edges and vertices.
class TopoDS_TSolid : public TopoDS_TShape
{
public:
  //! Creates an empty TSolid.
  TopoDS_TSolid()
      : TopoDS_TShape(TopAbs_SOLID)
  {
    Orientable(false);
  }

  //! Returns an empty TSolid.
  Standard_EXPORT occ::handle<TopoDS_TShape> EmptyCopy() const override;

  DEFINE_STANDARD_RTTIEXT(TopoDS_TSolid, TopoDS_TShape)
};

#endif // _TopoDS_TSolid_HeaderFile
