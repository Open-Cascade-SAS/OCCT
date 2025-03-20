// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESBasic_ExternalRefName_HeaderFile
#define _IGESBasic_ExternalRefName_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

class IGESBasic_ExternalRefName;
DEFINE_STANDARD_HANDLE(IGESBasic_ExternalRefName, IGESData_IGESEntity)

//! defines ExternalRefName, Type <416> Form <3>
//! in package IGESBasic
//! Used when it is assumed that a copy of the subfigure
//! exists in native form on the receiving system
class IGESBasic_ExternalRefName : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESBasic_ExternalRefName();

  //! This method is used to set the fields of the class
  //! ExternalRefName
  //! - anExtName : External Reference Entity Symbolic Name
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& anExtName);

  //! returns External Reference Entity Symbolic Name
  Standard_EXPORT Handle(TCollection_HAsciiString) ReferenceName() const;

  DEFINE_STANDARD_RTTIEXT(IGESBasic_ExternalRefName, IGESData_IGESEntity)

protected:
private:
  Handle(TCollection_HAsciiString) theExtRefEntitySymbName;
};

#endif // _IGESBasic_ExternalRefName_HeaderFile
