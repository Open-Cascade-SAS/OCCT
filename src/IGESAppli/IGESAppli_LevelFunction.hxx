// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_LevelFunction_HeaderFile
#define _IGESAppli_LevelFunction_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_LevelFunction;
DEFINE_STANDARD_HANDLE(IGESAppli_LevelFunction, IGESData_IGESEntity)

//! defines LevelFunction, Type <406> Form <3>
//! in package IGESAppli
//! Used to transfer the meaning or intended use of a level
//! in the sending system
class IGESAppli_LevelFunction : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_LevelFunction();
  
  //! This method is used to set the fields of the class
  //! LevelFunction
  //! - nbPropVal    : Number of Properties, always = 2
  //! - aCode        : Function Description code
  //! default = 0
  //! - aFuncDescrip : Function Description
  //! default = null string
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Standard_Integer aCode, const Handle(TCollection_HAsciiString)& aFuncDescrip);
  
  //! is always 2
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the function description code . Default = 0
  Standard_EXPORT Standard_Integer FuncDescriptionCode() const;
  
  //! returns the function description
  //! Default = null string
  Standard_EXPORT Handle(TCollection_HAsciiString) FuncDescription() const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_LevelFunction,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Integer theFuncDescripCode;
  Handle(TCollection_HAsciiString) theFuncDescrip;


};







#endif // _IGESAppli_LevelFunction_HeaderFile
