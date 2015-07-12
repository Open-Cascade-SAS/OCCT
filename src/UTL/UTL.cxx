// Created on: 1997-11-24
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


#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_FileIterator.hxx>
#include <OSD_Host.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_SingleProtection.hxx>
#include <Resource_Manager.hxx>
#include <Resource_Unicode.hxx>
#include <Standard_GUID.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_Data.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <UTL.hxx>

TCollection_ExtendedString UTL::xgetenv(const Standard_CString aCString) {
  TCollection_ExtendedString x;
  OSD_Environment theEnv(aCString);
  TCollection_AsciiString theValue=theEnv.Value();
  if( ! theValue.IsEmpty())
    x = TCollection_ExtendedString(theValue);
  return x;
}

TCollection_ExtendedString UTL::Extension(const TCollection_ExtendedString& aFileName) 
{
  TCollection_AsciiString aFileNameU(aFileName);
  OSD_Path p = OSD_Path(aFileNameU);
  TCollection_AsciiString theExtension = p.Extension();
  if (theExtension.Value(1) == '.')
    theExtension.Remove(1, 1);
  return TCollection_ExtendedString(theExtension);
}

Storage_Error UTL::OpenFile(Storage_BaseDriver& aDriver, 
                            const TCollection_ExtendedString& aFileName, 
                            const Storage_OpenMode aMode) 
{
  return aDriver.Open(TCollection_AsciiString(aFileName),aMode);
}

void UTL::AddToUserInfo(const Handle(Storage_Data)& aData, 
                        const TCollection_ExtendedString& anInfo) 
{
  aData->AddToUserInfo(TCollection_AsciiString(anInfo));
}

OSD_Path UTL::Path(const TCollection_ExtendedString& aFileName) 
{
  OSD_Path p = OSD_Path(TCollection_AsciiString(aFileName));
  return p;
}

TCollection_ExtendedString UTL::Disk(const OSD_Path& aPath)
{
  return TCollection_ExtendedString(aPath.Disk());
}

TCollection_ExtendedString UTL::Trek(const OSD_Path& aPath)
{
  return TCollection_ExtendedString(aPath.Trek());
}

TCollection_ExtendedString UTL::Name(const OSD_Path& aPath)
{
  return TCollection_ExtendedString(aPath.Name());
}

TCollection_ExtendedString UTL::Extension(const OSD_Path& aPath)
{
  return TCollection_ExtendedString(aPath.Extension());
}

OSD_FileIterator UTL::FileIterator(const OSD_Path& aPath, const TCollection_ExtendedString& aMask)
{
  OSD_FileIterator it = OSD_FileIterator(aPath,TCollection_AsciiString(aMask));
  return it;
}

TCollection_ExtendedString UTL::LocalHost()
{
  OSD_Host h;
  return TCollection_ExtendedString(h.HostName());
}

TCollection_ExtendedString UTL::ExtendedString(const TCollection_AsciiString& anAsciiString)
{
  return TCollection_ExtendedString(anAsciiString);
}

Standard_GUID UTL::GUID(const TCollection_ExtendedString& anXString)
{
  return Standard_GUID(TCollection_AsciiString(anXString,'?').ToCString());
}

Standard_Boolean UTL::Find(const Handle(Resource_Manager)& aResourceManager,
                           const TCollection_ExtendedString& aResourceName)
{
  return aResourceManager->Find(TCollection_AsciiString(aResourceName).ToCString());
}

TCollection_ExtendedString UTL::Value(const Handle(Resource_Manager)& aResourceManager,
                                      const TCollection_ExtendedString& aResourceName)
{
  TCollection_AsciiString aResourceNameU(aResourceName);
  return TCollection_ExtendedString(aResourceManager->Value(aResourceNameU.ToCString()),
                                    Standard_True);
}  

Standard_Integer UTL::IntegerValue(const TCollection_ExtendedString& anExtendedString)
{
  TCollection_AsciiString a(anExtendedString);
  return a.IntegerValue();
}

Standard_CString UTL::CString(const TCollection_ExtendedString& anExtendedString)
{
  static TCollection_AsciiString theValue;
  theValue = TCollection_AsciiString(anExtendedString);
  return theValue.ToCString();
}

Standard_Boolean UTL::IsReadOnly(const TCollection_ExtendedString& aFileName)
{
  switch (OSD_File(UTL::Path(aFileName)).Protection().User()) {
  case OSD_W:
  case OSD_RW:
  case OSD_WX:
  case OSD_RWX:
  case OSD_RWD:
  case OSD_WXD:
  case OSD_RWXD:
    return Standard_False;
  default:
    return Standard_True;
  }
}
