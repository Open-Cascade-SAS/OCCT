// File:        MDataStd_ReferenceListRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_ReferenceListRetrievalDriver.ixx>
#include <PDataStd_ReferenceList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <PCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>

//=======================================================================
//function : MDataStd_ReferenceListRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_ReferenceListRetrievalDriver::MDataStd_ReferenceListRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ReferenceListRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ReferenceListRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_ReferenceList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_ReferenceListRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_ReferenceList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ReferenceListRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						  const Handle(TDF_Attribute)& Target,
						  const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_ReferenceList) S = Handle(PDataStd_ReferenceList)::DownCast (Source);
  Handle(TDataStd_ReferenceList) T = Handle(TDataStd_ReferenceList)::DownCast (Target);

  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
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
	T->Append(L);
      }
    }
  }
}
