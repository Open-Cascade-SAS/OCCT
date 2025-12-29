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

#include <IGESBasic_SubfigureDef.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_SubfigureDef, IGESData_IGESEntity)

IGESBasic_SubfigureDef::IGESBasic_SubfigureDef() = default;

void IGESBasic_SubfigureDef::Init(
  const int                                                                 aDepth,
  const occ::handle<TCollection_HAsciiString>&                              aName,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allAssocEntities)
{
  if (!allAssocEntities.IsNull() && allAssocEntities->Lower() != 1)
    throw Standard_DimensionMismatch("IGESBasic_SubfigureDef : Init");
  theDepth         = aDepth;
  theName          = aName;
  theAssocEntities = allAssocEntities;
  InitTypeAndForm(308, 0);
}

int IGESBasic_SubfigureDef::Depth() const
{
  return theDepth;
}

occ::handle<TCollection_HAsciiString> IGESBasic_SubfigureDef::Name() const
{
  return theName;
}

int IGESBasic_SubfigureDef::NbEntities() const
{
  return (theAssocEntities.IsNull() ? 0 : theAssocEntities->Length());
}

occ::handle<IGESData_IGESEntity> IGESBasic_SubfigureDef::AssociatedEntity(const int Index) const
{
  return theAssocEntities->Value(Index);
}

occ::handle<Standard_Transient> IGESBasic_SubfigureDef::Value(const int Index) const
{
  return occ::handle<Standard_Transient>(theAssocEntities->Value(Index));
}
