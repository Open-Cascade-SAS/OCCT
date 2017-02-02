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
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_FileNode.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_SingleProtection.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <UTL.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_FWOSDriver,CDF_MetaDataDriver)

#ifdef _MSC_VER
#include <tchar.h>
#endif  // _MSC_VER


//==============================================================================
//function : PutSlash
//purpose  :
//==============================================================================
static void PutSlash (TCollection_ExtendedString& anXSTRING) {
#ifdef _WIN32
  anXSTRING+="\\";
#else
  anXSTRING+="/";
#endif  // _WIN32
}

//==============================================================================
//function : CDF_FWOSDriver
//purpose  :
//==============================================================================
CDF_FWOSDriver::CDF_FWOSDriver() {}

//==============================================================================
//function : Find
//purpose  :
//==============================================================================
Standard_Boolean CDF_FWOSDriver::Find(const TCollection_ExtendedString& aFolder,
                                         const TCollection_ExtendedString& aName,
                                         const TCollection_ExtendedString& /*aVersion*/)
{

  OSD_Path thePath=UTL::Path(aFolder);
  OSD_Directory theDirectory(thePath);
  if(theDirectory.Exists()) {
    TCollection_ExtendedString f(aFolder);
    PutSlash(f);
    f+=aName;
    OSD_Path p2 = UTL::Path(f);
    OSD_File theFile(p2);
    return theFile.Exists();
  }
  return Standard_False;
}

//==============================================================================
//function : HasReadPermission
//purpose  :
//==============================================================================
Standard_Boolean CDF_FWOSDriver::HasReadPermission(const TCollection_ExtendedString& aFolder,
                                                      const TCollection_ExtendedString& aName,
                                                      const TCollection_ExtendedString& /*aVersion*/)
{
  OSD_SingleProtection theProtection=OSD_File(UTL::Path(Concatenate(aFolder,aName))).Protection().User();
  switch (theProtection) {
    case OSD_None:
    case OSD_R:
    case OSD_RW:
    case OSD_RX:
    case OSD_WX:
    case OSD_RWX:
    case OSD_RD:
    case OSD_RWD:
    case OSD_RXD:
    case OSD_RWXD:
      return Standard_True;
    default:
      return Standard_False;
    }
}

//==============================================================================
//function : MetaData
//purpose  :
//==============================================================================
Handle(CDM_MetaData) CDF_FWOSDriver::MetaData(const TCollection_ExtendedString& aFolder,
                                                 const TCollection_ExtendedString& aName,
                                                 const TCollection_ExtendedString& /*aVersion*/)
{
  TCollection_ExtendedString p = Concatenate(aFolder,aName);
  return CDM_MetaData::LookUp(aFolder,aName,p,p,UTL::IsReadOnly(p));
}

//==============================================================================
//function : CreateMetaData
//purpose  :
//==============================================================================
Handle(CDM_MetaData) CDF_FWOSDriver::CreateMetaData(const Handle(CDM_Document)& aDocument,
                                                       const TCollection_ExtendedString& aFileName)
{
  return CDM_MetaData::LookUp(aDocument->RequestedFolder(),aDocument->RequestedName(),
                              Concatenate(aDocument->RequestedFolder(),aDocument->RequestedName()),
                              aFileName,UTL::IsReadOnly(aFileName));
}

//==============================================================================
//function : BuildFileName
//purpose  :
//==============================================================================
TCollection_ExtendedString CDF_FWOSDriver::BuildFileName(const Handle(CDM_Document)& aDocument)
{

  TCollection_ExtendedString retstr = TCollection_ExtendedString(aDocument->RequestedFolder());
  PutSlash(retstr);
  retstr += aDocument->RequestedName();
  return retstr;
}

//==============================================================================
//function : FindFolder
//purpose  :
//==============================================================================
Standard_Boolean CDF_FWOSDriver::FindFolder(const TCollection_ExtendedString& aFolder)
{
  
  OSD_Path thePath=UTL::Path(aFolder);
  OSD_Directory theDirectory(thePath);
  return theDirectory.Exists();
}

//==============================================================================
//function : Concatenate
//purpose  :
//==============================================================================
TCollection_ExtendedString CDF_FWOSDriver::Concatenate(const TCollection_ExtendedString& aFolder,
                                                          const TCollection_ExtendedString& aName)
{
  TCollection_ExtendedString ff(aFolder);
  ff = "";
  ff += aFolder;
  PutSlash(ff);
  ff+=aName;
  return ff;
}

//==============================================================================
//function : DefaultFolder
//purpose  :
//==============================================================================
TCollection_ExtendedString CDF_FWOSDriver::DefaultFolder()
{
  TCollection_ExtendedString theDefaultFolder;
  if (theDefaultFolder.Length() == 0) {
    
#ifdef _WIN32
    TCollection_ExtendedString hd=UTL::xgetenv("HOMEDRIVE");
    if(hd.Length() != 0) {
      theDefaultFolder=hd;
      theDefaultFolder+=UTL::xgetenv("HOMEPATH");
    }
    else {
      theDefaultFolder=UTL::xgetenv("TEMP");
      if(theDefaultFolder.Length()==0)
        throw Standard_Failure("cannot determine default folder; HOMEDRIVE and TEMP are undefined");
    }
#else
    TCollection_ExtendedString home=UTL::xgetenv("HOME");
    if(home.Length() !=0)
      theDefaultFolder =  home;
    else
      theDefaultFolder= TCollection_ExtendedString("/tmp");
#endif
  }
  return theDefaultFolder;
}

//==============================================================================
//function : BuildMetaData
//purpose  :
//==============================================================================
Handle(CDM_MetaData) CDF_FWOSDriver::BuildMetaData(const TCollection_ExtendedString& aFileName)
{

  OSD_Path p = UTL::Path(aFileName);
  
  TCollection_ExtendedString f = UTL::Trek(p);
  TCollection_ExtendedString n = UTL::Name(p);
  n +=".";
  n += UTL::Extension(p);

  return CDM_MetaData::LookUp(f,n,aFileName,aFileName,UTL::IsReadOnly(aFileName));
}

//==============================================================================
//function : SetName
//purpose  :
//==============================================================================
TCollection_ExtendedString CDF_FWOSDriver::SetName(const Handle(CDM_Document)& aDocument,
                                                      const TCollection_ExtendedString& aName)
{
  
  TCollection_ExtendedString xn(aName), n(aName);

#ifdef _WIN32
  //windows is not case sensitive
  //make the extension lower case
  for(int i = 1; i <= xn.Length(); i++)
  {
	Standard_ExtCharacter echar = xn.Value(i);
	echar = towlower(echar);
	xn.SetValue(i, echar);
  }
#endif
  
  TCollection_ExtendedString e (aDocument->FileExtension());
  TCollection_ExtendedString xe(e);
  if (e.Length() > 0) {
#ifdef _WIN32
    //windows is not case sensitive
    //make the extension lower case
    for(int i = 1; i <= xe.Length(); i++)
    {
	  Standard_ExtCharacter echar = xe.Value(i);
	  echar = towlower(echar);
	  xe.SetValue(i, echar);
    }
#endif
    xe.Insert(1, '.');
    e.Insert(1, '.');
    Standard_Integer ln = xn.Length();
    Standard_Integer le = xe.Length();
    Standard_Boolean ExtensionIsAlreadyThere = Standard_False;
    if(ln>=le) {
      Standard_Integer ind=xn.SearchFromEnd(xe);
      ExtensionIsAlreadyThere = ind+le-1==ln;
    }
    if(!ExtensionIsAlreadyThere) n+=e;
  }
  return n;
}
