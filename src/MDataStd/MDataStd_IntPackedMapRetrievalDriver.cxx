// File:	MDataStd_IntPackedMapRetrievalDriver.cxx
// Created:	Mon Aug 27 11:12:02 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007


#include <MDataStd_IntPackedMapRetrievalDriver.ixx>
#include <PDataStd_IntPackedMap.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : MDataStd_IntPackedMapRetrievalDriver
//purpose  : Constructor
//=======================================================================
MDataStd_IntPackedMapRetrievalDriver::MDataStd_IntPackedMapRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}
//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntPackedMapRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntPackedMapRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_IntPackedMap); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_IntPackedMapRetrievalDriver::NewEmpty() const
{ return new TDataStd_IntPackedMap (); }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntPackedMapRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_IntPackedMap) aS = Handle(PDataStd_IntPackedMap)::DownCast (Source);
  Handle(TDataStd_IntPackedMap) aT = Handle(TDataStd_IntPackedMap)::DownCast (Target);
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
  } else 
    WriteMessage(TCollection_ExtendedString("error retrieving attribute TDataStd_IntPackedMap"));
}
