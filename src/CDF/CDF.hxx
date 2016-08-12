// Created on: 1997-08-07
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

#ifndef _CDF_HeaderFile
#define _CDF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class CDF_Directory;
class CDF_DirectoryIterator;
class CDF_Session;
class CDF_Application;
class CDF_StoreList;
class CDF_Store;
class CDF_MetaDataDriver;
class CDF_FWOSDriver;
class CDF_MetaDataDriverFactory;

class CDF 
{
public:
  
  Standard_EXPORT static void GetLicense (const Standard_Integer anApplicationIdentifier);
  
  Standard_EXPORT static Standard_Boolean IsAvailable (const Standard_Integer anApplicationIdentifier);
};

#endif // _CDF_HeaderFile
