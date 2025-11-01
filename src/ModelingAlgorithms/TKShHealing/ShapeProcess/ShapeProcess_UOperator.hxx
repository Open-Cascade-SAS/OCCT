// Created on: 2000-08-22
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _ShapeProcess_UOperator_HeaderFile
#define _ShapeProcess_UOperator_HeaderFile

#include <Standard.hxx>

#include <ShapeProcess_OperFunc.hxx>
#include <ShapeProcess_Operator.hxx>
class ShapeProcess_Context;

class ShapeProcess_UOperator;
DEFINE_STANDARD_HANDLE(ShapeProcess_UOperator, ShapeProcess_Operator)

//! Defines operator as container for static function
//! OperFunc. This allows user to create new operators
//! without creation of new classes
class ShapeProcess_UOperator : public ShapeProcess_Operator
{

public:
  //! Creates operator with implementation defined as
  //! OperFunc (static function)
  Standard_EXPORT ShapeProcess_UOperator(const ShapeProcess_OperFunc func);

  //! Performs operation and records changes in the context
  Standard_EXPORT virtual Standard_Boolean Perform(
    const Handle(ShapeProcess_Context)& context,
    const Message_ProgressRange&        theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(ShapeProcess_UOperator, ShapeProcess_Operator)

protected:
private:
  ShapeProcess_OperFunc myFunc;
};

#endif // _ShapeProcess_UOperator_HeaderFile
