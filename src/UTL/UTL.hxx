// Created on: 1997-11-21
// Created by: Mister rmi
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

#ifndef _UTL_HeaderFile
#define _UTL_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <Storage_Error.hxx>
#include <Storage_OpenMode.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TCollection_ExtendedString;
class Storage_BaseDriver;
class Storage_Data;
class OSD_Path;
class OSD_FileIterator;
class TCollection_AsciiString;
class Standard_GUID;
class Resource_Manager;



class UTL 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static TCollection_ExtendedString xgetenv (const Standard_CString aCString);
  
  Standard_EXPORT static Storage_Error OpenFile (Storage_BaseDriver& aFile, const TCollection_ExtendedString& aName, const Storage_OpenMode aMode);
  
  Standard_EXPORT static void AddToUserInfo (const Handle(Storage_Data)& aData, const TCollection_ExtendedString& anInfo);
  
  Standard_EXPORT static OSD_Path Path (const TCollection_ExtendedString& aFileName);
  
  Standard_EXPORT static TCollection_ExtendedString Disk (const OSD_Path& aPath);
  
  Standard_EXPORT static TCollection_ExtendedString Trek (const OSD_Path& aPath);
  
  Standard_EXPORT static TCollection_ExtendedString Name (const OSD_Path& aPath);
  
  Standard_EXPORT static TCollection_ExtendedString Extension (const OSD_Path& aPath);
  
  Standard_EXPORT static OSD_FileIterator FileIterator (const OSD_Path& aPath, const TCollection_ExtendedString& aMask);
  
  Standard_EXPORT static TCollection_ExtendedString Extension (const TCollection_ExtendedString& aFileName);
  
  Standard_EXPORT static TCollection_ExtendedString LocalHost();
  
  Standard_EXPORT static TCollection_ExtendedString ExtendedString (const TCollection_AsciiString& anAsciiString);
  
  Standard_EXPORT static Standard_GUID GUID (const TCollection_ExtendedString& anXString);
  
  Standard_EXPORT static Standard_Boolean Find (const Handle(Resource_Manager)& aResourceManager, const TCollection_ExtendedString& aResourceName);
  
  Standard_EXPORT static TCollection_ExtendedString Value (const Handle(Resource_Manager)& aResourceManager, const TCollection_ExtendedString& aResourceName);
  
  Standard_EXPORT static Standard_Integer IntegerValue (const TCollection_ExtendedString& anExtendedString);
  
  Standard_EXPORT static Standard_CString CString (const TCollection_ExtendedString& anExtendedString);
  
  Standard_EXPORT static Standard_Boolean IsReadOnly (const TCollection_ExtendedString& aFileName);




protected:





private:





};







#endif // _UTL_HeaderFile
