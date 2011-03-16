// File:	MDataStd_IntPackedMapStorageDriver.cxx
// Created:	Thu Aug 23 10:40:06 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007

#include <MDataStd_IntPackedMapStorageDriver.ixx>
#include <PDataStd_IntPackedMap.hxx>
#include <PDataStd_IntPackedMap_1.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <PColStd_HArray1OfInteger.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>
//=======================================================================
//function : MDataStd_IntPackedMapStorageDriver
//purpose  : Constructor
//=======================================================================
MDataStd_IntPackedMapStorageDriver::MDataStd_IntPackedMapStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntPackedMapStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntPackedMapStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_IntPackedMap);}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_IntPackedMapStorageDriver::NewEmpty() const
{ return new PDataStd_IntPackedMap_1; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntPackedMapStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
  const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_IntPackedMap)   aS = 
    Handle(TDataStd_IntPackedMap)::DownCast (Source);
  Handle(PDataStd_IntPackedMap_1) aT = 
    Handle(PDataStd_IntPackedMap_1)::DownCast (Target);
  if(!aS.IsNull() && !aT.IsNull()) {
    Standard_Integer aSize = (aS->IsEmpty()) ? 0 : aS->Extent();
    if(aSize) {
      aT->Init (1, aSize);
      TColStd_MapIteratorOfPackedMapOfInteger anIt(aS->GetMap());
      for(Standard_Integer i = 1;anIt.More();anIt.Next(),i++)
	aT->SetValue(i, anIt.Key());
    }
    aT->SetDelta(aS->GetDelta());
  } else 
    WriteMessage(TCollection_ExtendedString ("error storing attribute TDataStd_IntPackedMap"));
}
