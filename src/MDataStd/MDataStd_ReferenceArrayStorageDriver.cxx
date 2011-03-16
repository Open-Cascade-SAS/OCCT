// File:        MDataStd_ReferenceArrayStorageDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_ReferenceArrayStorageDriver.ixx>
#include <PDataStd_ReferenceArray.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
//function : MDataStd_ReferenceArrayStorageDriver
//purpose  : 
//=======================================================================
MDataStd_ReferenceArrayStorageDriver::MDataStd_ReferenceArrayStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ReferenceArrayStorageDriver::VersionNumber() const
{ 
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ReferenceArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_ReferenceArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_ReferenceArrayStorageDriver::NewEmpty() const 
{
  return new PDataStd_ReferenceArray();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ReferenceArrayStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						 const Handle(PDF_Attribute)& Target,
						 const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_ReferenceArray) S = Handle(TDataStd_ReferenceArray)::DownCast (Source);
  Handle(PDataStd_ReferenceArray) T = Handle(PDataStd_ReferenceArray)::DownCast (Target);
  
  Standard_Integer lower = S->Lower(), upper = S->Upper(), i = lower;
  if (upper >= lower)
  {
    T->Init(lower, upper);
    for (; i <= upper; i++) 
    {
      TDF_Label L = S->Value(i);
      if (!L.IsNull())
      {
	TCollection_AsciiString tvalue;
	TDF_Tool::Entry(L, tvalue);
	Handle(PCollection_HExtendedString) pvalue = new PCollection_HExtendedString(tvalue);
	T->SetValue(i, pvalue);
      }
    }
  }
}
