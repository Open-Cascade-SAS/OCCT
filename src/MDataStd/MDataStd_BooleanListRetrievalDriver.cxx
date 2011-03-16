// File:        MDataStd_BooleanListRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_BooleanListRetrievalDriver.ixx>
#include <PDataStd_BooleanList.hxx>
#include <TDataStd_BooleanList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_BooleanListRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_BooleanListRetrievalDriver::MDataStd_BooleanListRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_BooleanListRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_BooleanListRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_BooleanList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_BooleanListRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_BooleanList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_BooleanListRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						const Handle(TDF_Attribute)& Target,
						const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_BooleanList) S = Handle(PDataStd_BooleanList)::DownCast (Source);
  Handle(TDataStd_BooleanList) T = Handle(TDataStd_BooleanList)::DownCast (Target);

  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
  for (i = lower; i <= upper; i++)
  {
    T->Append(S->Value(i));
  }
}
