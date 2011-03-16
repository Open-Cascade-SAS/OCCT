// File:	MDataStd_ByteArrayRetrievalDriver_1.cxx
// Created:	Thu Mar 27 18:41:22 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2008


#include <MDataStd_ByteArrayRetrievalDriver_1.ixx>

#include <PDataStd_ByteArray.hxx>
#include <PDataStd_ByteArray_1.hxx>
#include <TDataStd_ByteArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : MDataStd_ByteArrayRetrievalDriver_1
//purpose  : 
//=======================================================================
MDataStd_ByteArrayRetrievalDriver_1::MDataStd_ByteArrayRetrievalDriver_1(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ByteArrayRetrievalDriver_1::VersionNumber() const
{ 
  return 1; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ByteArrayRetrievalDriver_1::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_ByteArray_1);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_ByteArrayRetrievalDriver_1::NewEmpty () const 
{
  return new TDataStd_ByteArray();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ByteArrayRetrievalDriver_1::Paste(const Handle(PDF_Attribute)& Source,
					      const Handle(TDF_Attribute)& Target,
					      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_ByteArray_1) S = Handle(PDataStd_ByteArray_1)::DownCast (Source);
  Handle(TDataStd_ByteArray) T = Handle(TDataStd_ByteArray)::DownCast (Target);

  if (!S->Get().IsNull()) {
    const Handle(PColStd_HArray1OfInteger)& pvalues = S->Get();
    Handle(TColStd_HArray1OfByte) tvalues = 
      new TColStd_HArray1OfByte(pvalues->Lower(), pvalues->Upper());
    Standard_Integer i = pvalues->Lower(), upper = pvalues->Upper();
    for (; i <= upper; i++) {
      Standard_Byte pvalue = (Standard_Byte) pvalues->Value(i);
      tvalues->SetValue(i, pvalue);
    }
    T->ChangeArray(tvalues, Standard_False);
    T->SetDelta(S->GetDelta());
  }
}
