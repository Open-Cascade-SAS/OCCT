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

#include <IGESSolid_BooleanTree.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_BooleanTree, IGESData_IGESEntity)

IGESSolid_BooleanTree::IGESSolid_BooleanTree() = default;

void IGESSolid_BooleanTree::Init(
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& operands,
  const occ::handle<NCollection_HArray1<int>>&                              operations)
{
  if (operands->Lower() != 1 || operations->Lower() != 1
      || operands->Length() != operations->Length())
    throw Standard_DimensionError("IGESSolid_BooleanTree : Init");

  theOperations = operations;
  theOperands   = operands;
  InitTypeAndForm(180, 0);
}

int IGESSolid_BooleanTree::Length() const
{
  return theOperands->Length();
}

bool IGESSolid_BooleanTree::IsOperand(const int Index) const
{
  return (!theOperands->Value(Index).IsNull());
}

occ::handle<IGESData_IGESEntity> IGESSolid_BooleanTree::Operand(const int Index) const
{
  return theOperands->Value(Index);
}

int IGESSolid_BooleanTree::Operation(const int Index) const
{
  if (theOperands->Value(Index).IsNull())
    return theOperations->Value(Index);
  else
    return 0; // It is not an operation. (operations can be : 1-2-3)
}
