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

// Modified by rmi, Thu Dec  4 14:24:24 1997

#include <CDF_Application.hxx>
#include <CDF_MetaDataDriver.hxx>
#include <CDF_Store.hxx>
#include <CDF_StoreList.hxx>
#include <CDF_StoreSetNameStatus.hxx>
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <PCDM_StoreStatus.hxx>
#include <Standard_ProgramError.hxx>

#define theMetaDataDriver                                                                          \
  Handle(CDF_Application)::DownCast((myCurrentDocument->Application()))->MetaDataDriver()

static const Handle(TCollection_HExtendedString) blank = new TCollection_HExtendedString("");

CDF_Store::CDF_Store()
    : myHasSubComponents(Standard_False),
      myIsMainDocument(Standard_False),
      myStatus(PCDM_SS_No_Obj)
{
}

CDF_Store::CDF_Store(const Handle(CDM_Document)& aDocument)
    : myHasSubComponents(Standard_False)
{

  myMainDocument = aDocument;
  Init();
}

void CDF_Store::Init()
{

  myCurrentDocument = myMainDocument;
  myList            = new CDF_StoreList(myCurrentDocument);

  // getting the subcomponents.
  //
  myIsMainDocument = Standard_False;
  myList->Init();
  for (myList->Init(); myList->More(); myList->Next())
  {
    myCurrentDocument = myList->Value();
    if (myCurrentDocument != myMainDocument)
    {
      myHasSubComponents = Standard_True;
      FindDefault();
    }
  }
  myIsMainDocument  = Standard_True;
  myCurrentDocument = myMainDocument;
}

Handle(TCollection_HExtendedString) CDF_Store::Folder() const
{
  if (myCurrentDocument->HasRequestedFolder())
    return new TCollection_HExtendedString(myCurrentDocument->RequestedFolder());
  return blank;
}

Handle(TCollection_HExtendedString) CDF_Store::Name() const
{
  return new TCollection_HExtendedString(myCurrentDocument->RequestedName());
}

Standard_Boolean CDF_Store::SetFolder(const Standard_ExtString aFolder)
{
  TCollection_ExtendedString f(aFolder);
  return SetFolder(f);
}

Standard_Boolean CDF_Store::SetFolder(const TCollection_ExtendedString& aFolder)
{

  TCollection_ExtendedString theFolder(aFolder);
  Standard_Integer           aLen = theFolder.Length();

  // if the last character is the folder separator, remove it.
  if (aLen > 1 && (theFolder.Value(aLen) == '/' || theFolder.Value(aLen) == '\\'))
    theFolder.Trunc(aLen - 1);

  if (theMetaDataDriver->FindFolder(theFolder))
  {
    myCurrentDocument->SetRequestedFolder(theFolder);
    return Standard_True;
  }
  return Standard_False;
}

CDF_StoreSetNameStatus CDF_Store::RecheckName()
{
  return SetName(myCurrentDocument->RequestedName());
}

CDF_StoreSetNameStatus CDF_Store::SetName(const TCollection_ExtendedString& aName)
{
  TCollection_ExtendedString theName = theMetaDataDriver->SetName(myCurrentDocument, aName);

  if (myCurrentDocument->IsStored())
  {
    Handle(CDM_MetaData) E = myCurrentDocument->MetaData();
    if (E->Folder() == myCurrentDocument->RequestedFolder() && E->Name() == theName)
      return CDF_SSNS_OK;
  }

  if (myCurrentDocument->HasRequestedFolder())
  {
    if (theMetaDataDriver->Find(myCurrentDocument->RequestedFolder(), theName))
    {
      if (theMetaDataDriver->MetaData(myCurrentDocument->RequestedFolder(), theName)->IsRetrieved())
        return CDF_SSNS_OpenDocument;
      else
      {
        myCurrentDocument->SetRequestedName(theName);
        return CDF_SSNS_ReplacingAnExistentDocument;
      }
    }
  }
  myCurrentDocument->SetRequestedName(theName);
  return CDF_SSNS_OK;
}

CDF_StoreSetNameStatus CDF_Store::SetName(const Standard_ExtString aName)
{
  TCollection_ExtendedString theName(aName);
  return SetName(theName);
}

void CDF_Store::Realize(const Message_ProgressRange& theRange)
{
  Standard_ProgramError_Raise_if(!myList->IsConsistent(), "information are missing");
  Handle(CDM_MetaData) m;
  myText   = "";
  myStatus = myList->Store(m, myText, theRange);
  if (myStatus == PCDM_SS_OK)
    myPath = m->Path();
}

Standard_ExtString CDF_Store::Path() const
{
  return myPath.ToExtString();
}

Handle(TCollection_HExtendedString) CDF_Store::MetaDataPath() const
{
  if (myCurrentDocument->IsStored())
    return new TCollection_HExtendedString(myCurrentDocument->MetaData()->Path());
  return blank;
}

Handle(TCollection_HExtendedString) CDF_Store::Description() const
{
  if (myMainDocument->FindDescription())
    return new TCollection_HExtendedString(myMainDocument->Description());
  return blank;
}

Standard_Boolean CDF_Store::IsStored() const
{
  return myCurrentDocument->IsStored();
}

Standard_Boolean CDF_Store::IsModified() const
{
  return myCurrentDocument->IsModified();
}

Standard_Boolean CDF_Store::CurrentIsConsistent() const
{
  if (!myCurrentDocument->IsStored())
    return myCurrentDocument->HasRequestedFolder();
  return Standard_True;
}

Standard_Boolean CDF_Store::IsConsistent() const
{
  return myList->IsConsistent();
}

Standard_Boolean CDF_Store::HasAPreviousVersion() const
{
  return myCurrentDocument->HasRequestedPreviousVersion();
}

Handle(TCollection_HExtendedString) CDF_Store::PreviousVersion() const
{
  if (myCurrentDocument->HasRequestedPreviousVersion())
    return new TCollection_HExtendedString(myCurrentDocument->RequestedPreviousVersion());
  return blank;
}

Standard_Boolean CDF_Store::SetPreviousVersion(const Standard_ExtString aPreviousVersion)
{
  if (theMetaDataDriver->HasVersionCapability())
  {
    if (myCurrentDocument->HasRequestedFolder())
    {
      if (theMetaDataDriver->Find(myCurrentDocument->RequestedFolder(),
                                  myCurrentDocument->RequestedName(),
                                  aPreviousVersion))
      {

        myCurrentDocument->SetRequestedPreviousVersion(aPreviousVersion);
        return Standard_True;
      }
      else
        return Standard_False;
    }
    else
      return Standard_False;
  }
  return Standard_True;
}

void CDF_Store::SetCurrent(const Standard_ExtString /*aPresentation*/)
{
  myIsMainDocument = myCurrentDocument == myMainDocument;
}

void CDF_Store::SetMain()
{
  myCurrentDocument = myMainDocument;
  myIsMainDocument  = Standard_True;
}

Standard_Boolean CDF_Store::IsMainDocument() const
{
  return myIsMainDocument;
}

PCDM_StoreStatus CDF_Store::StoreStatus() const
{
  return myStatus;
}

Standard_ExtString CDF_Store::AssociatedStatusText() const
{
  return myText.ToExtString();
}

void CDF_Store::FindDefault()
{
  if (!myCurrentDocument->IsStored())
  {
    myCurrentDocument->SetRequestedFolder(
      Handle(CDF_Application)::DownCast((myCurrentDocument->Application()))->DefaultFolder());
    myCurrentDocument->SetRequestedName(
      theMetaDataDriver->SetName(myCurrentDocument, myCurrentDocument->RequestedName()));
  }
}

void CDF_Store::SetComment(const Standard_ExtString aComment)
{
  myCurrentDocument->SetRequestedComment(aComment);
}

Handle(TCollection_HExtendedString) CDF_Store::Comment() const
{
  return new TCollection_HExtendedString(myCurrentDocument->RequestedComment());
}
