// File:        MDataStd_BooleanArrayStorageDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_BooleanArrayStorageDriver.ixx>
#include <PDataStd_BooleanArray.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : MDataStd_BooleanArrayStorageDriver
//purpose  : 
//=======================================================================
MDataStd_BooleanArrayStorageDriver::MDataStd_BooleanArrayStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_BooleanArrayStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_BooleanArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_BooleanArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_BooleanArrayStorageDriver::NewEmpty() const 
{
  return new PDataStd_BooleanArray();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_BooleanArrayStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
					       const Handle(PDF_Attribute)&   Target,
					       const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_BooleanArray) S = Handle(TDataStd_BooleanArray)::DownCast (Source);
  Handle(PDataStd_BooleanArray) T = Handle(PDataStd_BooleanArray)::DownCast (Target);
  
  if (S->Upper() >= S->Lower())
  {
    T->SetLower(S->Lower());
    T->SetUpper(S->Upper());
    const Handle(TColStd_HArray1OfByte)& tvalues = S->InternalArray();
    Handle(PColStd_HArray1OfInteger) pvalues = new PColStd_HArray1OfInteger(tvalues->Lower(), tvalues->Upper());
    Standard_Integer i = tvalues->Lower(), upper = tvalues->Upper();
    for (; i <= upper; i++)
    {
      pvalues->SetValue(i, tvalues->Value(i));
    }
    T->Set(pvalues);
  }
}
