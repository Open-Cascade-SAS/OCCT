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

#include <IGESSolid_SolidAssembly.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_SolidAssembly, IGESData_IGESEntity)

IGESSolid_SolidAssembly::IGESSolid_SolidAssembly() {}

void IGESSolid_SolidAssembly::Init(const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&           Items,
                                   const occ::handle<NCollection_HArray1<occ::handle<IGESGeom_TransformationMatrix>>>& Matrices)
{
  if (Items->Lower() != 1 || Matrices->Lower() != 1 || Items->Length() != Matrices->Length())
    throw Standard_DimensionError("IGESSolid_SolidAssembly : Init");

  theItems    = Items;
  theMatrices = Matrices;
  InitTypeAndForm(184, 0);
}

bool IGESSolid_SolidAssembly::HasBrep() const
{
  return (FormNumber() == 1);
}

void IGESSolid_SolidAssembly::SetBrep(const bool hasbrep)
{
  InitTypeAndForm(184, (hasbrep ? 1 : 0));
}

int IGESSolid_SolidAssembly::NbItems() const
{
  return theItems->Length();
}

occ::handle<IGESData_IGESEntity> IGESSolid_SolidAssembly::Item(const int Index) const
{
  return theItems->Value(Index);
}

occ::handle<IGESGeom_TransformationMatrix> IGESSolid_SolidAssembly::TransfMatrix(
  const int Index) const
{
  return theMatrices->Value(Index);
}
