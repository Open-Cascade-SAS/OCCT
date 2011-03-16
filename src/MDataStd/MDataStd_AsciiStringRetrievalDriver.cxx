// File:	MDataStd_AsciiStringRetrievalDriver.cxx
// Created:	Thu Aug 23 09:59:19 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007

#include <MDataStd_AsciiStringRetrievalDriver.ixx>
#include <PDataStd_AsciiString.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_AsciiStringRetrievalDriver
//purpose  : Constructor
//=======================================================================
MDataStd_AsciiStringRetrievalDriver::MDataStd_AsciiStringRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}
//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_AsciiStringRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_AsciiStringRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_AsciiString); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_AsciiStringRetrievalDriver::NewEmpty() const
{ return new TDataStd_AsciiString (); }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_AsciiStringRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_AsciiString) S = Handle(PDataStd_AsciiString)::DownCast (Source);
  Handle(TDataStd_AsciiString) T = Handle(TDataStd_AsciiString)::DownCast (Target);
  if(!S.IsNull() && !T.IsNull()) {
    if(!S->Get().IsNull()) {
      TCollection_AsciiString aString((S->Get())->Convert ());
      T->Set (aString);
    }
  }
}

