// File:        MDataStd_BooleanArrayRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_BooleanArrayRetrievalDriver.ixx>
#include <PDataStd_BooleanArray.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : MDataStd_BooleanArrayRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_BooleanArrayRetrievalDriver::MDataStd_BooleanArrayRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_BooleanArrayRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_BooleanArrayRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_BooleanArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_BooleanArrayRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_BooleanArray();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_BooleanArrayRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						const Handle(TDF_Attribute)& Target,
						const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_BooleanArray) S = Handle(PDataStd_BooleanArray)::DownCast (Source);
  Handle(TDataStd_BooleanArray) T = Handle(TDataStd_BooleanArray)::DownCast (Target);

  if (S->Upper() >= S->Lower())
  {
    T->Init(S->Lower(), S->Upper());
    const Handle(PColStd_HArray1OfInteger)& pvalues = S->Get();
    Handle(TColStd_HArray1OfByte) tvalues = new TColStd_HArray1OfByte(pvalues->Lower(), pvalues->Upper());
    Standard_Integer i = pvalues->Lower(), upper = pvalues->Upper();
    for (; i <= upper; i++)
    {
      Standard_Byte pvalue = (Standard_Byte) pvalues->Value(i);
      tvalues->SetValue(i, pvalue);
    }
    T->SetInternalArray(tvalues);
  }
}
