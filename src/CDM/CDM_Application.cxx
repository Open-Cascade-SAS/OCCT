// File:	CDM_Application.cxx
// Created:	Wed Oct 22 16:58:01 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
