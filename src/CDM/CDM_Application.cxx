// Created on: 1997-10-22
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


#include <CDM_Application.hxx>
#include <CDM_Document.hxx>
#include <CDM_MessageDriver.hxx>
#include <CDM_MetaData.hxx>
#include <CDM_NullMessageDriver.hxx>
#include <CDM_Reference.hxx>
#include <Resource_Manager.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDM_Application,Standard_Transient)

//=======================================================================
//function : SetDocumentVersion
//purpose  : 
//=======================================================================
void CDM_Application::SetDocumentVersion
                        (const Handle(CDM_Document)& aDocument,
                         const Handle(CDM_MetaData)& aMetaData) const
{
  aDocument->SetModifications(aMetaData->DocumentVersion(this));
}

//=======================================================================
//function : SetReferenceCounter
//purpose  : 
//=======================================================================

void CDM_Application::SetReferenceCounter
                        (const Handle(CDM_Document)& aDocument,
                         const Standard_Integer aReferenceCounter)
{
  aDocument->SetReferenceCounter(aReferenceCounter);
}

//=======================================================================
//function : MessageDriver
//purpose  : 
//=======================================================================

Handle(CDM_MessageDriver) CDM_Application::MessageDriver()
{
  static Handle(CDM_NullMessageDriver) theMessageDriver
    =new CDM_NullMessageDriver;
  return theMessageDriver;
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void CDM_Application::Write(const Standard_ExtString aString)
{
  MessageDriver()->Write(aString);
}

//=======================================================================
//function : BeginOfUpdate
//purpose  : 
//=======================================================================

void CDM_Application::BeginOfUpdate (const Handle(CDM_Document)& aDocument)
{
  TCollection_ExtendedString updating("Updating:");
  updating+=aDocument->Presentation();
  Write(updating.ToExtString());
}

//=======================================================================
//function : EndOfUpdate
//purpose  : 
//=======================================================================

void CDM_Application::EndOfUpdate
                        (const Handle(CDM_Document)&       aDocument,
                         const Standard_Boolean            theStatus,
                         const TCollection_ExtendedString& /*ErrorString*/)
{
  TCollection_ExtendedString message;
  if (theStatus)
    message="Updated:";
  else
    message="Error during updating:";
      
  message+=aDocument->Presentation();
  Write(message.ToExtString());
}
