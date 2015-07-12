// Created on: 2001-07-25
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlLDrivers_HeaderFile
#define _XmlLDrivers_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Standard_Transient;
class Standard_GUID;
class TCollection_AsciiString;
class XmlMDF_ADriverTable;
class CDM_MessageDriver;
class XmlLDrivers_DocumentStorageDriver;
class XmlLDrivers_DocumentRetrievalDriver;
class XmlLDrivers_NamespaceDef;



class XmlLDrivers 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static const Handle(Standard_Transient)& Factory (const Standard_GUID& theGUID);
  
  Standard_EXPORT static TCollection_AsciiString CreationDate();
  
  Standard_EXPORT static Handle(XmlMDF_ADriverTable) AttributeDrivers (const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT static TCollection_AsciiString StorageVersion();




protected:





private:




friend class XmlLDrivers_DocumentStorageDriver;
friend class XmlLDrivers_DocumentRetrievalDriver;
friend class XmlLDrivers_NamespaceDef;

};







#endif // _XmlLDrivers_HeaderFile
