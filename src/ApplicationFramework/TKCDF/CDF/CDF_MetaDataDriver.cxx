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
#include <CDM_MetaData.hxx>
#include <PCDM_ReferenceIterator.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_Thread.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_MetaDataDriver, Standard_Transient)

//=================================================================================================

CDF_MetaDataDriver::CDF_MetaDataDriver() {}

//=================================================================================================

bool CDF_MetaDataDriver::HasVersion(const TCollection_ExtendedString&,
                                    const TCollection_ExtendedString&)
{
  return true;
}

//=================================================================================================

//=================================================================================================

bool CDF_MetaDataDriver::HasVersionCapability()
{
  return false;
}

//=================================================================================================

void CDF_MetaDataDriver::CreateDependsOn(const occ::handle<CDM_MetaData>&,
                                         const occ::handle<CDM_MetaData>&)
{
}

//=================================================================================================

void CDF_MetaDataDriver::CreateReference(const occ::handle<CDM_MetaData>&,
                                         const occ::handle<CDM_MetaData>&,
                                         const int,
                                         const int)
{
}

//=================================================================================================

occ::handle<PCDM_ReferenceIterator> CDF_MetaDataDriver::ReferenceIterator(
  const occ::handle<Message_Messenger>& theMessageDriver)
{
  return new PCDM_ReferenceIterator(theMessageDriver);
}

//=================================================================================================

bool CDF_MetaDataDriver::Find(const TCollection_ExtendedString& aFolder,
                              const TCollection_ExtendedString& aName)
{
  TCollection_ExtendedString aVersion;
  return Find(aFolder, aName, aVersion);
}

//=================================================================================================

occ::handle<CDM_MetaData> CDF_MetaDataDriver::MetaData(const TCollection_ExtendedString& aFolder,
                                                       const TCollection_ExtendedString& aName)
{
  TCollection_ExtendedString aVersion;
  return MetaData(aFolder, aName, aVersion);
}

//=================================================================================================

occ::handle<CDM_MetaData> CDF_MetaDataDriver::LastVersion(
  const occ::handle<CDM_MetaData>& aMetaData)
{
  return aMetaData;
}

//=================================================================================================

TCollection_ExtendedString CDF_MetaDataDriver::SetName(const occ::handle<CDM_Document>&,
                                                       const TCollection_ExtendedString& aName)
{
  return aName;
}
