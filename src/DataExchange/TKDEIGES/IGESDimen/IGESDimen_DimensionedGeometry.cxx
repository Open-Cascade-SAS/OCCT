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

#include <IGESDimen_DimensionedGeometry.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_DimensionedGeometry, IGESData_IGESEntity)

IGESDimen_DimensionedGeometry::IGESDimen_DimensionedGeometry() {}

void IGESDimen_DimensionedGeometry::Init(const int                      nbDims,
                                         const occ::handle<IGESData_IGESEntity>&          aDimension,
                                         const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& entities)
{
  if (entities->Lower() != 1)
    throw Standard_DimensionMismatch("IGESDimen_DimensionedGeometry : Init");
  theNbDimensions     = nbDims;
  theDimension        = aDimension;
  theGeometryEntities = entities;
  InitTypeAndForm(402, 13);
}

int IGESDimen_DimensionedGeometry::NbDimensions() const
{
  return theNbDimensions;
}

int IGESDimen_DimensionedGeometry::NbGeometryEntities() const
{
  return theGeometryEntities->Length();
}

occ::handle<IGESData_IGESEntity> IGESDimen_DimensionedGeometry::DimensionEntity() const
{
  return theDimension;
}

occ::handle<IGESData_IGESEntity> IGESDimen_DimensionedGeometry::GeometryEntity(
  const int num) const
{
  return theGeometryEntities->Value(num);
}
