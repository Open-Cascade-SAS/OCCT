// File:        MDataStd_ReferenceArrayRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_ReferenceArrayRetrievalDriver.ixx>
#include <PDataStd_ReferenceArray.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <PCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>

//=======================================================================
//function : MDataStd_ReferenceArrayRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_ReferenceArrayRetrievalDriver::MDataStd_ReferenceArrayRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ReferenceArrayRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ReferenceArrayRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_ReferenceArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_ReferenceArrayRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_ReferenceArray();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ReferenceArrayRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						   const Handle(TDF_Attribute)& Target,
						   const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_ReferenceArray) S = Handle(PDataStd_ReferenceArray)::DownCast (Source);
  Handle(TDataStd_ReferenceArray) T = Handle(TDataStd_ReferenceArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
  T->Init(lower, upper);
  for (i = lower; i <= upper; i++)
  {
    const Handle(PCollection_HExtendedString)& pvalue = S->Value(i);
    if (!pvalue.IsNull()) 
    {
      TDF_Label L;
      TCollection_AsciiString tvalue = pvalue->Convert();
      TDF_Tool::Label(T->Label().Data(), tvalue, L, Standard_True);
      if (!L.IsNull())
      {
	T->SetValue(i, L);
      }
    }
  }
}
