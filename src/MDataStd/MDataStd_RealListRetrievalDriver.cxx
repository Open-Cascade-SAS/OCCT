// File:        MDataStd_RealListRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_RealListRetrievalDriver.ixx>
#include <PDataStd_RealList.hxx>
#include <TDataStd_RealList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_RealListRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_RealListRetrievalDriver::MDataStd_RealListRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_RealListRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_RealListRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_RealList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_RealListRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_RealList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_RealListRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
					     const Handle(TDF_Attribute)& Target,
					     const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_RealList) S = Handle(PDataStd_RealList)::DownCast (Source);
  Handle(TDataStd_RealList) T = Handle(TDataStd_RealList)::DownCast (Target);

  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
  for (i = lower; i <= upper; i++)
  {
    T->Append(S->Value(i));
  }
}
