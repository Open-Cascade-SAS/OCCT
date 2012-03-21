// Created on: 1997-10-22
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <CDM_Application.ixx>
#include <CDM_NullMessageDriver.hxx>

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
                         const Standard_Boolean            Status,
                         const TCollection_ExtendedString& /*ErrorString*/)
{
  TCollection_ExtendedString message;
  if (Status) 
    message="Updated:";
  else
    message="Error during updating:";
      
  message+=aDocument->Presentation();
  Write(message.ToExtString());
}
