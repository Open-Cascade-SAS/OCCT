// File:        MDataStd_ExtStringListStorageDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_ExtStringListStorageDriver.ixx>
#include <PDataStd_ExtStringList.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
//function : MDataStd_ExtStringListStorageDriver
//purpose  : 
//=======================================================================
MDataStd_ExtStringListStorageDriver::MDataStd_ExtStringListStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ExtStringListStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ExtStringListStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_ExtStringList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_ExtStringListStorageDriver::NewEmpty() const 
{
  return new PDataStd_ExtStringList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ExtStringListStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						const Handle(PDF_Attribute)& Target,
						const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_ExtStringList) S = Handle(TDataStd_ExtStringList)::DownCast (Source);
  Handle(PDataStd_ExtStringList) T = Handle(PDataStd_ExtStringList)::DownCast (Target);
  
  Standard_Integer lower = 1, upper = S->Extent(), i = lower;
  if (upper >= lower)
  {
    T->Init(lower, upper);
    TDataStd_ListIteratorOfListOfExtendedString itr(S->List());
    for (; itr.More(); itr.Next(), i++) 
    {
      const TCollection_ExtendedString& tvalue = itr.Value();
      Handle(PCollection_HExtendedString) pvalue = new PCollection_HExtendedString(tvalue);
      T->SetValue(i, pvalue);
    }
  }
}
