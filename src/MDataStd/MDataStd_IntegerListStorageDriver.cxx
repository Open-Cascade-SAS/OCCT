// File:        MDataStd_IntegerListStorageDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_IntegerListStorageDriver.ixx>
#include <PDataStd_IntegerList.hxx>
#include <TDataStd_IntegerList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

//=======================================================================
//function : MDataStd_IntegerListStorageDriver
//purpose  : 
//=======================================================================
MDataStd_IntegerListStorageDriver::MDataStd_IntegerListStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntegerListStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntegerListStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_IntegerList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_IntegerListStorageDriver::NewEmpty() const 
{
  return new PDataStd_IntegerList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntegerListStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
					      const Handle(PDF_Attribute)&        Target,
					      const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_IntegerList) S = Handle(TDataStd_IntegerList)::DownCast (Source);
  Handle(PDataStd_IntegerList) T = Handle(PDataStd_IntegerList)::DownCast (Target);
  
  Standard_Integer lower = 1, upper = S->Extent(), i = lower;
  if (upper >= lower)
  {
    T->Init(lower, upper);
    TColStd_ListIteratorOfListOfInteger itr(S->List());
    for (; itr.More(); itr.Next(), i++) 
    {
      T->SetValue(i, itr.Value());
    }
  }
}
