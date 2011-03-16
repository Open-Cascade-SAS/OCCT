// File:	CDF_MetaDataDriver.cxx
// Created:	Mon Nov 17 16:53:25 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
