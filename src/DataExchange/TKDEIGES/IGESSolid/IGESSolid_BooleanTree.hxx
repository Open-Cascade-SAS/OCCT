// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_BooleanTree_HeaderFile
#define _IGESSolid_BooleanTree_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>

//! defines BooleanTree, Type <180> Form Number <0>
//! in package IGESSolid
//! The Boolean tree describes a binary tree structure
//! composed of regularized Boolean operations and operands,
//! in post-order notation.
class IGESSolid_BooleanTree : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESSolid_BooleanTree();

  //! This method is used to set the fields of the class
  //! BooleanTree
  //! - operands   : Array containing pointer to DE of operands
  //! - operations : Array containing integer type for operations
  Standard_EXPORT void Init(
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& operands,
    const occ::handle<NCollection_HArray1<int>>&                              operations);

  //! returns the length of the post-order list
  Standard_EXPORT int Length() const;

  //! returns True if Index'th value in the post-order list is an Operand;
  //! else returns False if it is an Integer Operations
  //! raises exception if Index < 1 or Index > Length()
  Standard_EXPORT bool IsOperand(const int Index) const;

  //! returns the Index'th value in the post-order list only if it is
  //! an operand else returns NULL
  //! raises exception if Index < 1 or Index > Length()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Operand(const int Index) const;

  //! returns the Index'th value in the post-order list only if it is
  //! an operation else returns 0
  //! raises exception if Index < 1 or Index > Length()
  Standard_EXPORT int Operation(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_BooleanTree, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> theOperands;
  occ::handle<NCollection_HArray1<int>>                              theOperations;
};

#endif // _IGESSolid_BooleanTree_HeaderFile
