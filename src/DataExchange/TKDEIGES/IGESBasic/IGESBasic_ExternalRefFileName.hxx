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

#ifndef _IGESBasic_ExternalRefFileName_HeaderFile
#define _IGESBasic_ExternalRefFileName_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

class IGESBasic_ExternalRefFileName;
DEFINE_STANDARD_HANDLE(IGESBasic_ExternalRefFileName, IGESData_IGESEntity)

//! defines ExternalRefFileName, Type <416> Form <0-2>
//! in package IGESBasic
//! Used when single definition from the reference file is
//! required or for external logical references where an
//! entity in one file relates to an entity in another file
class IGESBasic_ExternalRefFileName : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESBasic_ExternalRefFileName();

  //! This method is used to set the fields of the class
  //! ExternalRefFileName
  //! - aFileIdent : External Reference File Identifier
  //! - anExtName  : External Reference Entity Symbolic Name
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aFileIdent,
                            const Handle(TCollection_HAsciiString)& anExtName);

  //! Changes FormNumber to be 2 if <mode> is True (For Entity)
  //! or 0 if <mode> is False (For Definition)
  Standard_EXPORT void SetForEntity(const Standard_Boolean mode);

  //! returns External Reference File Identifier
  Standard_EXPORT Handle(TCollection_HAsciiString) FileId() const;

  //! returns External Reference Entity Symbolic Name
  Standard_EXPORT Handle(TCollection_HAsciiString) ReferenceName() const;

  DEFINE_STANDARD_RTTIEXT(IGESBasic_ExternalRefFileName, IGESData_IGESEntity)

protected:
private:
  Handle(TCollection_HAsciiString) theExtRefFileIdentifier;
  Handle(TCollection_HAsciiString) theExtRefEntitySymbName;
};

#endif // _IGESBasic_ExternalRefFileName_HeaderFile
