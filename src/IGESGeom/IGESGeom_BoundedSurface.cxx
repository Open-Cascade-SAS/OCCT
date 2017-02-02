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

#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_BoundedSurface,IGESData_IGESEntity)

IGESGeom_BoundedSurface::IGESGeom_BoundedSurface ()    {  }


    void  IGESGeom_BoundedSurface::Init
  (const Standard_Integer aType,
   const Handle(IGESData_IGESEntity)& aSurface,
   const Handle(IGESGeom_HArray1OfBoundary)& aBoundary)
{
  if (aBoundary->Lower() != 1)
    throw Standard_DimensionMismatch("IGESGeom_BoundedSurface : Init");
  theType       = aType;
  theSurface    = aSurface;
  theBoundaries = aBoundary;
  InitTypeAndForm(143,0);
}

    Standard_Integer  IGESGeom_BoundedSurface::RepresentationType () const
{
  return theType;
}

    Handle(IGESData_IGESEntity)  IGESGeom_BoundedSurface::Surface () const
{
  return theSurface;
}

    Standard_Integer  IGESGeom_BoundedSurface::NbBoundaries () const
{
  
  return ( theBoundaries.IsNull() ? 0 : theBoundaries->Length());
}

    Handle(IGESGeom_Boundary)  IGESGeom_BoundedSurface::Boundary
  (const Standard_Integer Index) const
{
  return theBoundaries->Value(Index);
}
