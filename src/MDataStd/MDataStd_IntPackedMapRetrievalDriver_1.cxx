// File:	MDataStd_IntPackedMapRetrievalDriver_1.cxx
// Created:	Fri Mar 28 11:41:25 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2008


#include <MDataStd_IntPackedMapRetrievalDriver_1.ixx>
#include <PDataStd_IntPackedMap.hxx>
#include <PDataStd_IntPackedMap_1.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : MDataStd_IntPackedMapRetrievalDriver_1
//purpose  : Constructor
//=======================================================================
MDataStd_IntPackedMapRetrievalDriver_1::MDataStd_IntPackedMapRetrievalDriver_1
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}
//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntPackedMapRetrievalDriver_1::VersionNumber() const
{ return 1; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntPackedMapRetrievalDriver_1::SourceType() const
{ return STANDARD_TYPE(PDataStd_IntPackedMap_1); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_IntPackedMapRetrievalDriver_1::NewEmpty() const
{ return new TDataStd_IntPackedMap (); }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntPackedMapRetrievalDriver_1::Paste(
			      const Handle(PDF_Attribute)& Source,
			      const Handle(TDF_Attribute)& Target,
			      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_IntPackedMap_1) aS = 
    Handle(PDataStd_IntPackedMap_1)::DownCast (Source);
  Handle(TDataStd_IntPackedMap) aT = 
    Handle(TDataStd_IntPackedMap)::DownCast (Target);
  if(!aS.IsNull() && !aT.IsNull()) {
    if(!aS->IsEmpty()) {
      Handle(TColStd_HPackedMapOfInteger) aHMap = new TColStd_HPackedMapOfInteger ();
      Standard_Integer aKey;
      for(Standard_Integer i = aS->Lower(); i<= aS->Upper() ; i++) {
	aKey = aS->GetValue(i);
	if(!aHMap->ChangeMap().Add( aKey )) {
	  WriteMessage(
	  TCollection_ExtendedString("error retrieving integer mamaber of the attribute TDataStd_IntPackedMap"));
	  return;
	}
      }
      aT->ChangeMap(aHMap);
    }
    aT->SetDelta(aS->GetDelta()); 
  } else 
    WriteMessage(TCollection_ExtendedString("error retrieving attribute TDataStd_IntPackedMap"));
}
