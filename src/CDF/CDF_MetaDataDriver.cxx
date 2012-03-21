// Created on: 1997-11-17
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <CDF_MetaDataDriver.ixx>
#include <CDF_Application.hxx>
#include <CDF_Session.hxx>


//=======================================================================
//function : CDF_MetaDataDriver
//purpose  : 
//=======================================================================

CDF_MetaDataDriver::CDF_MetaDataDriver() {}

//=======================================================================
//function : HasVersion
//purpose  : 
//=======================================================================

Standard_Boolean CDF_MetaDataDriver::HasVersion(const TCollection_ExtendedString& ,const TCollection_ExtendedString& ) {
  return Standard_True;
}

//=======================================================================
//function : HasVersionCapability
//purpose  : 
//=======================================================================

//=======================================================================
//function : HasVersionCapability
//purpose  : 
//=======================================================================

Standard_Boolean CDF_MetaDataDriver::HasVersionCapability() {
  return Standard_False;
}
//=======================================================================
//function : CreateDependsOn
//purpose  : 
//=======================================================================

void CDF_MetaDataDriver::CreateDependsOn(const Handle(CDM_MetaData)& ,
				     const Handle(CDM_MetaData)& ) {}

//=======================================================================
//function : CreateReference
//purpose  : 
//=======================================================================

void CDF_MetaDataDriver::CreateReference(const Handle(CDM_MetaData)& ,const Handle(CDM_MetaData)& , const Standard_Integer , const Standard_Integer ) {}

//=======================================================================
//function : ReferenceIterator
//purpose  : 
//=======================================================================

Handle(PCDM_ReferenceIterator) CDF_MetaDataDriver::ReferenceIterator() {
  return new PCDM_ReferenceIterator(CDF_Session::CurrentSession()->CurrentApplication()->MessageDriver());
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Standard_Boolean CDF_MetaDataDriver::Find(const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName) {
  TCollection_ExtendedString aVersion;
  return Find(aFolder,aName,aVersion);
}
//=======================================================================
//function : MetaData
//purpose  : 
//=======================================================================

Handle(CDM_MetaData) CDF_MetaDataDriver::MetaData(const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName) {
  TCollection_ExtendedString aVersion;
  return MetaData(aFolder,aName,aVersion);
}
//=======================================================================
//function : LastVersion
//purpose  : 
//=======================================================================

Handle(CDM_MetaData) CDF_MetaDataDriver::LastVersion(const Handle(CDM_MetaData)& aMetaData) {
  return aMetaData;
}
//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDF_MetaDataDriver::SetName(const Handle(CDM_Document)& , const TCollection_ExtendedString& aName) {
  return aName;
}
