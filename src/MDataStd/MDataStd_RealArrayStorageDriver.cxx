// File:	MDataStd_RealArrayStorageDriver.cxx
// Created:	FriJun 11 16:29:04 1999
// Author:	Sergey RUIN


#include <MDataStd_RealArrayStorageDriver.ixx>
#include <PDataStd_RealArray.hxx>
#include <PDataStd_RealArray_1.hxx>
#include <TDataStd_RealArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_RealArrayStorageDriver
//purpose  : 
//=======================================================================

MDataStd_RealArrayStorageDriver::MDataStd_RealArrayStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RealArrayStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RealArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_RealArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_RealArrayStorageDriver::NewEmpty () const {

  return new PDataStd_RealArray_1 ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RealArrayStorageDriver::Paste (
			    const Handle(TDF_Attribute)&  Source,
			    const Handle(PDF_Attribute)&  Target,
			    const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_RealArray) S   = Handle(TDataStd_RealArray)::DownCast (Source);
  Handle(PDataStd_RealArray_1) T = Handle(PDataStd_RealArray_1)::DownCast (Target);
  
  Standard_Integer i, lower = S->Lower(), upper = S->Upper() ;
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );  
  T->SetDelta(S->GetDelta());
}

