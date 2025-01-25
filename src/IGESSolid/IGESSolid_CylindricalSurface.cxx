// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_CylindricalSurface, IGESData_IGESEntity)

IGESSolid_CylindricalSurface::IGESSolid_CylindricalSurface() {}

void IGESSolid_CylindricalSurface::Init(const Handle(IGESGeom_Point)&     aLocation,
                                        const Handle(IGESGeom_Direction)& anAxis,
                                        const Standard_Real               aRadius,
                                        const Handle(IGESGeom_Direction)& aRefdir)
{
  theLocationPoint = aLocation;
  theAxis          = anAxis;
  theRadius        = aRadius;
  theRefDir        = aRefdir;
  InitTypeAndForm(192, (theRefDir.IsNull() ? 0 : 1));
}

Handle(IGESGeom_Point) IGESSolid_CylindricalSurface::LocationPoint() const
{
  return theLocationPoint;
}

Handle(IGESGeom_Direction) IGESSolid_CylindricalSurface::Axis() const
{
  return theAxis;
}

Standard_Real IGESSolid_CylindricalSurface::Radius() const
{
  return theRadius;
}

Handle(IGESGeom_Direction) IGESSolid_CylindricalSurface::ReferenceDir() const
{
  return theRefDir;
}

Standard_Boolean IGESSolid_CylindricalSurface::IsParametrised() const
{
  return (!theRefDir.IsNull());
}
