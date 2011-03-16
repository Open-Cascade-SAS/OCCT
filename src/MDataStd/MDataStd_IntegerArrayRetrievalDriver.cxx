// File:	MDataStd_UObjectRetrievalDriver.cxx
// Created:	Tue Jun 15 16:29:04 1999
// Author:	Sergey RUIN


#include <MDataStd_IntegerArrayRetrievalDriver.ixx>
#include <PDataStd_IntegerArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_IntegerArrayRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_IntegerArrayRetrievalDriver::MDataStd_IntegerArrayRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_IntegerArrayRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_IntegerArrayRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_IntegerArray);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_IntegerArrayRetrievalDriver::NewEmpty () const {

  return new TDataStd_IntegerArray ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_IntegerArrayRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
      const Handle(TDF_Attribute)& Target,
//      const Handle(MDF_RRelocationTable)& RelocTable) const
      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_IntegerArray) S = Handle(PDataStd_IntegerArray)::DownCast (Source);
  Handle(TDataStd_IntegerArray) T = Handle(TDataStd_IntegerArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower() , upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );
}

