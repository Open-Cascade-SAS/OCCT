// File:	MDataStd_IntegerArrayStorageDriver.cxx
// Created:	FriJun 11 16:29:04 1999
// Author:	Sergey RUIN


#include <MDataStd_IntegerArrayStorageDriver.ixx>
#include <PDataStd_IntegerArray.hxx>
#include <PDataStd_IntegerArray_1.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_IntegerArrayStorageDriver
//purpose  : 
//=======================================================================

MDataStd_IntegerArrayStorageDriver::MDataStd_IntegerArrayStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_IntegerArrayStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_IntegerArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_IntegerArray);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_IntegerArrayStorageDriver::NewEmpty () const {

  return new PDataStd_IntegerArray_1 ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_IntegerArrayStorageDriver::Paste (
  const Handle(TDF_Attribute)&  Source,
  const Handle(PDF_Attribute)&        Target,
  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_IntegerArray) S = Handle(TDataStd_IntegerArray)::DownCast (Source);
  Handle(PDataStd_IntegerArray_1) T = Handle(PDataStd_IntegerArray_1)::DownCast (Target);
  
  Standard_Integer i, lower =S->Lower() ,upper = S->Upper() ;
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );  
  T->SetDelta(S->GetDelta());
}

