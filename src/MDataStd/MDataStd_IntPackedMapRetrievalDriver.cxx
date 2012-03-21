// Created on: 2007-08-27
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
