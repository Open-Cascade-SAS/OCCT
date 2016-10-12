// Created on: 2000-08-21
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

#ifndef _ShapeProcess_HeaderFile
#define _ShapeProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class ShapeProcess_Operator;
class ShapeProcess_Context;
class ShapeProcess_Context;
class ShapeProcess_ShapeContext;
class ShapeProcess_Operator;
class ShapeProcess_UOperator;
class ShapeProcess_OperLibrary;

//! Shape Processing module
//! allows to define and apply general Shape Processing as a
//! customizable sequence of Shape Healing operators. The
//! customization is implemented via user-editable resource
//! file which defines sequence of operators to be executed
//! and their parameters.
class ShapeProcess 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Registers operator to make it visible for Performer
  Standard_EXPORT static Standard_Boolean RegisterOperator (const Standard_CString name, const Handle(ShapeProcess_Operator)& op);
  
  //! Finds operator by its name
  Standard_EXPORT static Standard_Boolean FindOperator (const Standard_CString name, Handle(ShapeProcess_Operator)& op);
  
  //! Performs a specified sequence of operators on Context
  //! Resource file and other data should be already loaded
  //! to Context (including description of sequence seq)
  Standard_EXPORT static Standard_Boolean Perform (const Handle(ShapeProcess_Context)& context, const Standard_CString seq);




protected:





private:




friend class ShapeProcess_Context;
friend class ShapeProcess_ShapeContext;
friend class ShapeProcess_Operator;
friend class ShapeProcess_UOperator;
friend class ShapeProcess_OperLibrary;

};







#endif // _ShapeProcess_HeaderFile
