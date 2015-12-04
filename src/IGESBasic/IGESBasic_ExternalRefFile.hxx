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

#ifndef _IGESBasic_ExternalRefFile_HeaderFile
#define _IGESBasic_ExternalRefFile_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESBasic_ExternalRefFile;
DEFINE_STANDARD_HANDLE(IGESBasic_ExternalRefFile, IGESData_IGESEntity)

//! defines ExternalRefFile, Type <416> Form <1>
//! in package IGESBasic
//! Used when entire reference file is to be instanced
class IGESBasic_ExternalRefFile : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESBasic_ExternalRefFile();
  
  //! This method is used to set the field of the class
  //! ExternalRefFile
  //! - aFileIdent : External Reference File Identifier
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aFileIdent);
  
  //! returns External Reference File Identifier
  Standard_EXPORT Handle(TCollection_HAsciiString) FileId() const;




  DEFINE_STANDARD_RTTIEXT(IGESBasic_ExternalRefFile,IGESData_IGESEntity)

protected:




private:


  Handle(TCollection_HAsciiString) theExtRefFileIdentifier;


};







#endif // _IGESBasic_ExternalRefFile_HeaderFile
