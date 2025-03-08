// Created on: 1998-06-10
// Created by: data exchange team
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

#include <ShapeBuild.hxx>

#include <Geom_Plane.hxx>

//=================================================================================================

Handle(Geom_Plane) ShapeBuild::PlaneXOY()
{
  static Handle(Geom_Plane) xoy;
  if (xoy.IsNull())
    xoy = new Geom_Plane(0, 0, 1, 0);
  return xoy;
}
