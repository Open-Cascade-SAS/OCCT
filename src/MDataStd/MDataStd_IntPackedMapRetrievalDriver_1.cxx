// Created on: 2008-03-28
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
