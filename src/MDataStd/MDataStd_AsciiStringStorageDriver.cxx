// File:	MDataStd_AsciiStringStorageDriver.cxx
// Created:	Wed Aug 22 18:37:00 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007

#include <MDataStd_AsciiStringStorageDriver.ixx>
#include <PDataStd_AsciiString.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>
//=======================================================================
//function : MDataStd_AsciiStringStorageDriver
//purpose  : Constructor
//=======================================================================
MDataStd_AsciiStringStorageDriver::MDataStd_AsciiStringStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_AsciiStringStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_AsciiStringStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_AsciiString);}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_AsciiStringStorageDriver::NewEmpty() const
{ return new PDataStd_AsciiString; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_AsciiStringStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
  const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_AsciiString) S = Handle(TDataStd_AsciiString)::DownCast (Source);
  Handle(PDataStd_AsciiString) T = Handle(PDataStd_AsciiString)::DownCast (Target);
  if(!S.IsNull() && !T.IsNull()) {
    if(S->Get().Length() != 0) {
      Handle(PCollection_HAsciiString) aString = new PCollection_HAsciiString (S->Get());
      T->Set (aString);
    }
  }
}
