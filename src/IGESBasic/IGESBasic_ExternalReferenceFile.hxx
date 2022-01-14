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

#ifndef _IGESBasic_ExternalReferenceFile_HeaderFile
#define _IGESBasic_ExternalReferenceFile_HeaderFile

#include <Standard.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class IGESBasic_ExternalReferenceFile;
DEFINE_STANDARD_HANDLE(IGESBasic_ExternalReferenceFile, IGESData_IGESEntity)

//! defines ExternalReferenceFile, Type <406> Form <12>
//! in package IGESBasic
//! References definitions residing in another file
class IGESBasic_ExternalReferenceFile : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESBasic_ExternalReferenceFile();
  
  //! This method is used to set the fields of the class
  //! ExternalReferenceFile
  //! - aNameArray : External Reference File Names
  Standard_EXPORT void Init (const Handle(Interface_HArray1OfHAsciiString)& aNameArray);
  
  //! returns number of External Reference File Names
  Standard_EXPORT Standard_Integer NbListEntries() const;
  
  //! returns External Reference File Name
  //! raises exception if Index <= 0 or Index > NbListEntries()
  Standard_EXPORT Handle(TCollection_HAsciiString) Name (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESBasic_ExternalReferenceFile,IGESData_IGESEntity)

protected:




private:


  Handle(Interface_HArray1OfHAsciiString) theNames;


};







#endif // _IGESBasic_ExternalReferenceFile_HeaderFile
