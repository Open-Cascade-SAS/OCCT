// File:        MDataStd_ByteArrayRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_ByteArrayRetrievalDriver.ixx>
#include <PDataStd_ByteArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : MDataStd_ByteArrayRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_ByteArrayRetrievalDriver::MDataStd_ByteArrayRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ByteArrayRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ByteArrayRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_ByteArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_ByteArrayRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_ByteArray();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ByteArrayRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
					      const Handle(TDF_Attribute)& Target,
					      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_ByteArray) S = Handle(PDataStd_ByteArray)::DownCast (Source);
  Handle(TDataStd_ByteArray) T = Handle(TDataStd_ByteArray)::DownCast (Target);

  if (!S->Get().IsNull())
  {
    const Handle(PColStd_HArray1OfInteger)& pvalues = S->Get();
    Handle(TColStd_HArray1OfByte) tvalues = new TColStd_HArray1OfByte(pvalues->Lower(), pvalues->Upper());
    Standard_Integer i = pvalues->Lower(), upper = pvalues->Upper();
    for (; i <= upper; i++)
    {
      Standard_Byte pvalue = (Standard_Byte) pvalues->Value(i);
      tvalues->SetValue(i, pvalue);
    }
    T->ChangeArray(tvalues, Standard_False);
  }
}
