// Created on: 1997-11-17
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <CDF_Application.hxx>
#include <CDF_MetaDataDriver.hxx>
#include <CDF_Session.hxx>
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <PCDM_ReferenceIterator.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_MetaDataDriver,Standard_Transient)

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
