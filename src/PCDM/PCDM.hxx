// Created on: 1997-08-01
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _PCDM_HeaderFile
#define _PCDM_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <PCDM_TypeOfFileDriver.hxx>
#include <PCDM_BaseDriverPointer.hxx>
class CDM_Document;
class PCDM_StorageDriver;
class Storage_Schema;
class TCollection_ExtendedString;
class CDM_Application;
class TCollection_AsciiString;
class PCDM_Document;
class PCDM_Reader;
class PCDM_Writer;
class PCDM_RetrievalDriver;
class PCDM_StorageDriver;
class PCDM_ReferenceIterator;
class PCDM_Reference;
class PCDM_ReadWriter;
class PCDM_ReadWriter_1;



class PCDM 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean FindStorageDriver (const Handle(CDM_Document)& aDocument);
  
  //! gets   in the  EuclidDesktop   resource  the plugin
  //! identifier of the driver plugs the driver.
  Standard_EXPORT static Handle(PCDM_StorageDriver) StorageDriver (const Handle(CDM_Document)& aDocument);
  
  //! returns a schema to be used during a Store or Retrieve
  //! operation.
  //! Schema will plug the schema defined by
  //! the SchemaName method.
  Standard_EXPORT static Handle(Storage_Schema) Schema (const TCollection_ExtendedString& aSchemaName, const Handle(CDM_Application)& anApplication);




protected:





private:

  
  Standard_EXPORT static PCDM_TypeOfFileDriver FileDriverType (const TCollection_AsciiString& aFileName, PCDM_BaseDriverPointer& aBaseDriver);



friend class PCDM_Document;
friend class PCDM_Reader;
friend class PCDM_Writer;
friend class PCDM_RetrievalDriver;
friend class PCDM_StorageDriver;
friend class PCDM_ReferenceIterator;
friend class PCDM_Reference;
friend class PCDM_ReadWriter;
friend class PCDM_ReadWriter_1;

};







#endif // _PCDM_HeaderFile
