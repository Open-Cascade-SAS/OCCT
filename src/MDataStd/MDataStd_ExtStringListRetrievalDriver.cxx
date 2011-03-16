// File:        MDataStd_ExtStringListRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_ExtStringListRetrievalDriver.ixx>
#include <PDataStd_ExtStringList.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
//function : MDataStd_ExtStringListRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_ExtStringListRetrievalDriver::MDataStd_ExtStringListRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ExtStringListRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ExtStringListRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_ExtStringList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_ExtStringListRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_ExtStringList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ExtStringListRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
					     const Handle(TDF_Attribute)& Target,
					     const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_ExtStringList) S = Handle(PDataStd_ExtStringList)::DownCast (Source);
  Handle(TDataStd_ExtStringList) T = Handle(TDataStd_ExtStringList)::DownCast (Target);

  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
  for (i = lower; i <= upper; i++)
  {
    const Handle(PCollection_HExtendedString)& pvalue = S->Value(i);
    TCollection_ExtendedString tvalue = pvalue->Convert();
    T->Append(tvalue);
  }
}
