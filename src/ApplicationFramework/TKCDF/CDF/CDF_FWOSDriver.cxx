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

#include <CDF_FWOSDriver.hxx>
#include <CDM_MetaData.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_FWOSDriver, CDF_MetaDataDriver)

#ifdef _MSC_VER
  #include <tchar.h>
#endif // _MSC_VER

//=================================================================================================

static void PutSlash(TCollection_ExtendedString& anXSTRING)
{
#ifdef _WIN32
  anXSTRING += "\\";
#else
  anXSTRING += "/";
#endif // _WIN32
}

//=================================================================================================

CDF_FWOSDriver::CDF_FWOSDriver(
  NCollection_DataMap<TCollection_ExtendedString, occ::handle<CDM_MetaData>>& theLookUpTable)
    : myLookUpTable(&theLookUpTable)
{
}

//=================================================================================================

bool CDF_FWOSDriver::Find(const TCollection_ExtendedString& aFolder,
                          const TCollection_ExtendedString& aName,
                          const TCollection_ExtendedString& /*aVersion*/)
{

  OSD_Path      thePath(aFolder);
  OSD_Directory theDirectory(thePath);
  if (theDirectory.Exists())
  {
    TCollection_ExtendedString f(aFolder);
    PutSlash(f);
    f += aName;
    OSD_Path p2(f);
    OSD_File theFile(p2);
    return theFile.Exists();
  }
  return false;
}

//=================================================================================================

bool CDF_FWOSDriver::HasReadPermission(const TCollection_ExtendedString& aFolder,
                                       const TCollection_ExtendedString& aName,
                                       const TCollection_ExtendedString& /*aVersion*/)
{
  return OSD_File(OSD_Path(Concatenate(aFolder, aName))).IsReadable();
}

//=================================================================================================

occ::handle<CDM_MetaData> CDF_FWOSDriver::MetaData(const TCollection_ExtendedString& aFolder,
                                                   const TCollection_ExtendedString& aName,
                                                   const TCollection_ExtendedString& /*aVersion*/)
{
  TCollection_ExtendedString p = Concatenate(aFolder, aName);
  return CDM_MetaData::LookUp(*myLookUpTable,
                              aFolder,
                              aName,
                              p,
                              p,
                              !OSD_File(OSD_Path(p)).IsWriteable());
}

//=================================================================================================

occ::handle<CDM_MetaData> CDF_FWOSDriver::CreateMetaData(
  const occ::handle<CDM_Document>&  aDocument,
  const TCollection_ExtendedString& aFileName)
{
  return CDM_MetaData::LookUp(*myLookUpTable,
                              aDocument->RequestedFolder(),
                              aDocument->RequestedName(),
                              Concatenate(aDocument->RequestedFolder(), aDocument->RequestedName()),
                              aFileName,
                              !OSD_File(OSD_Path(aFileName)).IsWriteable());
}

//=================================================================================================

TCollection_ExtendedString CDF_FWOSDriver::BuildFileName(const occ::handle<CDM_Document>& aDocument)
{

  TCollection_ExtendedString retstr = TCollection_ExtendedString(aDocument->RequestedFolder());
  PutSlash(retstr);
  retstr += aDocument->RequestedName();
  return retstr;
}

//=================================================================================================

bool CDF_FWOSDriver::FindFolder(const TCollection_ExtendedString& aFolder)
{

  OSD_Path      thePath(aFolder);
  OSD_Directory theDirectory(thePath);
  return theDirectory.Exists();
}

//=================================================================================================

TCollection_ExtendedString CDF_FWOSDriver::Concatenate(const TCollection_ExtendedString& aFolder,
                                                       const TCollection_ExtendedString& aName)
{
  TCollection_ExtendedString ff(aFolder);
  PutSlash(ff);
  ff += aName;
  return ff;
}

//=================================================================================================

TCollection_ExtendedString CDF_FWOSDriver::DefaultFolder()
{
  TCollection_ExtendedString theDefaultFolder;
  if (theDefaultFolder.Length() == 0)
  {

#ifdef _WIN32
    TCollection_ExtendedString hd(OSD_Environment("HOMEDRIVE").Value());
    if (hd.Length() != 0)
    {
      theDefaultFolder = hd;
      theDefaultFolder += TCollection_ExtendedString(OSD_Environment("HOMEPATH").Value());
    }
    else
    {
      theDefaultFolder = TCollection_ExtendedString(OSD_Environment("TEMP").Value());
      if (theDefaultFolder.Length() == 0)
        theDefaultFolder = ".";
    }
#else
    TCollection_ExtendedString home(OSD_Environment("HOME").Value());
    if (home.Length() != 0)
    {
      theDefaultFolder = home;
    }
    else
    {
      theDefaultFolder = TCollection_ExtendedString("/tmp");
    }
#endif
  }
  return theDefaultFolder;
}

//=================================================================================================

TCollection_ExtendedString CDF_FWOSDriver::SetName(const occ::handle<CDM_Document>&  aDocument,
                                                   const TCollection_ExtendedString& aName)
{

  TCollection_ExtendedString xn(aName), n(aName);

#ifdef _WIN32
  // windows is not case sensitive
  // make the extension lower case
  for (int i = 1; i <= xn.Length(); i++)
  {
    char16_t echar = xn.Value(i);
    echar          = towlower(echar);
    xn.SetValue(i, echar);
  }
#endif

  TCollection_ExtendedString e(aDocument->FileExtension());
  TCollection_ExtendedString xe(e);
  if (e.Length() > 0)
  {
#ifdef _WIN32
    // windows is not case sensitive
    // make the extension lower case
    for (int i = 1; i <= xe.Length(); i++)
    {
      char16_t echar = xe.Value(i);
      echar          = towlower(echar);
      xe.SetValue(i, echar);
    }
#endif
    xe.Insert(1, '.');
    e.Insert(1, '.');
    int  ln                      = xn.Length();
    int  le                      = xe.Length();
    bool ExtensionIsAlreadyThere = false;
    if (ln >= le)
    {
      int ind                 = xn.SearchFromEnd(xe);
      ExtensionIsAlreadyThere = ind + le - 1 == ln;
    }
    if (!ExtensionIsAlreadyThere)
    {
      n += e;
    }
  }
  return n;
}
