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

#ifndef _IGESBasic_ExternalRefLibName_HeaderFile
#define _IGESBasic_ExternalRefLibName_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

class IGESBasic_ExternalRefLibName;
DEFINE_STANDARD_HANDLE(IGESBasic_ExternalRefLibName, IGESData_IGESEntity)

//! defines ExternalRefLibName, Type <416> Form <4>
//! in package IGESBasic
//! Used when it is assumed that a copy of the subfigure
//! exists in native form in a library on the receiving
//! system
class IGESBasic_ExternalRefLibName : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESBasic_ExternalRefLibName();

  //! This method is used to set the fields of the class
  //! ExternalRefLibName
  //! - aLibName  : Name of library in which ExtName resides
  //! - anExtName : External Reference Entity Symbolic Name
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aLibName,
                            const Handle(TCollection_HAsciiString)& anExtName);

  //! returns name of library in which External Reference Entity
  //! Symbolic Name resides
  Standard_EXPORT Handle(TCollection_HAsciiString) LibraryName() const;

  //! returns External Reference Entity Symbolic Name
  Standard_EXPORT Handle(TCollection_HAsciiString) ReferenceName() const;

  DEFINE_STANDARD_RTTIEXT(IGESBasic_ExternalRefLibName, IGESData_IGESEntity)

protected:
private:
  Handle(TCollection_HAsciiString) theLibName;
  Handle(TCollection_HAsciiString) theExtRefEntitySymbName;
};

#endif // _IGESBasic_ExternalRefLibName_HeaderFile
