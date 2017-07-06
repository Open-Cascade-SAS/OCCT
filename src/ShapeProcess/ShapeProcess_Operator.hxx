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

#ifndef _ShapeProcess_Operator_HeaderFile
#define _ShapeProcess_Operator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
class ShapeProcess_Context;


class ShapeProcess_Operator;
DEFINE_STANDARD_HANDLE(ShapeProcess_Operator, Standard_Transient)

//! Abstract Operator class providing a tool to
//! perform an operation on Context
class ShapeProcess_Operator : public Standard_Transient
{

public:

  
  //! Performs operation and eventually records
  //! changes in the context
  Standard_EXPORT virtual Standard_Boolean Perform (const Handle(ShapeProcess_Context)& context) = 0;




  DEFINE_STANDARD_RTTIEXT(ShapeProcess_Operator,Standard_Transient)

protected:




private:




};







#endif // _ShapeProcess_Operator_HeaderFile
