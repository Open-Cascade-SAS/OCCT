// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinLDrivers_HeaderFile
#define _BinLDrivers_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Standard_Transient;
class Standard_GUID;
class BinMDF_ADriverTable;
class CDM_MessageDriver;
class TCollection_AsciiString;
class BinLDrivers_DocumentStorageDriver;
class BinLDrivers_DocumentRetrievalDriver;
class BinLDrivers_DocumentSection;



class BinLDrivers 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static const Handle(Standard_Transient)& Factory (const Standard_GUID& theGUID);
  
  //! Creates a table of the supported drivers' types
  Standard_EXPORT static Handle(BinMDF_ADriverTable) AttributeDrivers (const Handle(CDM_MessageDriver)& MsgDrv);
  
  //! returns last storage version
  Standard_EXPORT static TCollection_AsciiString StorageVersion();




protected:





private:




friend class BinLDrivers_DocumentStorageDriver;
friend class BinLDrivers_DocumentRetrievalDriver;
friend class BinLDrivers_DocumentSection;

};







#endif // _BinLDrivers_HeaderFile
