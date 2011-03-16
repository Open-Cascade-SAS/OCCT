// File:	MDataStd_UObjectRetrievalDriver.cxx
// Created:	Tue Jun 15 16:29:04 1999
// Author:	Sergey RUIN


#include <MDataStd_RealArrayRetrievalDriver.ixx>
#include <PDataStd_RealArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_RealArrayRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_RealArrayRetrievalDriver::MDataStd_RealArrayRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RealArrayRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RealArrayRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_RealArray);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_RealArrayRetrievalDriver::NewEmpty () const {

  return new TDataStd_RealArray ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RealArrayRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
      const Handle(TDF_Attribute)& Target,
//      const Handle(MDF_RRelocationTable)& RelocTable) const
      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_RealArray) S = Handle(PDataStd_RealArray)::DownCast (Source);
  Handle(TDataStd_RealArray) T = Handle(TDataStd_RealArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower() , upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );
}

