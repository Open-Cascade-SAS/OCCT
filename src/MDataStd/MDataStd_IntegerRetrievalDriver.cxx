// File:	MDataStd_IntegerRetrievalDriver.cxx
// Created:	Thu Apr 10 16:32:26 1997
// Author:	VAUTHIER Jean-Claude


#include <MDataStd_IntegerRetrievalDriver.ixx>
#include <TDataStd_Integer.hxx>
#include <PDataStd_Integer.hxx>
#include <TDF_Label.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : MDataStd_IntegerRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_IntegerRetrievalDriver::MDataStd_IntegerRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_IntegerRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_IntegerRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_Integer);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_IntegerRetrievalDriver::NewEmpty () const {

  return new TDataStd_Integer ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_IntegerRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
//  const Handle(MDF_RRelocationTable)&       RelocTable) const
  const Handle(MDF_RRelocationTable)&       ) const
{
  Handle(PDataStd_Integer) S = Handle(PDataStd_Integer)::DownCast (Source);
  Handle(TDataStd_Integer) T = Handle(TDataStd_Integer)::DownCast (Target);
  T->Set (S->Get ());
}

