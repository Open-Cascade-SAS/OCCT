// Created on: 1992-04-07
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_NameEntity_HeaderFile
#define _IGESData_NameEntity_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

class IGESData_NameEntity;
DEFINE_STANDARD_HANDLE(IGESData_NameEntity, IGESData_IGESEntity)

//! a NameEntity is a kind of IGESEntity which can provide a Name
//! under alphanumeric (String) form, from Properties list
//! an effective Name entity must inherit it
class IGESData_NameEntity : public IGESData_IGESEntity
{

public:
  //! Retyrns the alphanumeric value of the Name, to be defined
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) Value() const = 0;

  DEFINE_STANDARD_RTTIEXT(IGESData_NameEntity, IGESData_IGESEntity)

protected:
private:
};

#endif // _IGESData_NameEntity_HeaderFile
