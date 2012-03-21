// Created by: DAUTRY Philippe
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

//      	---------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Mar 13 1997	Creation



#include <MDF.ixx>

#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

#include <MDF_RRelocationTable.hxx>
#include <MDF_SRelocationTable.hxx>
#include <MDF_Tool.hxx>
#include <MDF_TypeARDriverMap.hxx>
#include <MDF_TypeASDriverMap.hxx>

#include <PDF_Attribute.hxx>
#include <PDF_HAttributeArray1.hxx>

#include <PColStd_HArray1OfInteger.hxx>

#include <PTColStd_TransientPersistentMap.hxx>

#include <MDF_TagSourceStorageDriver.hxx>
#include <MDF_TagSourceRetrievalDriver.hxx>
#include <MDF_ReferenceStorageDriver.hxx>
#include <MDF_ReferenceRetrievalDriver.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : FromTo
//purpose  : From TRANSIENT to PERSISTENT.
//=======================================================================

void MDF::FromTo
(const Handle(TDF_Data)& aSource,
 Handle(PDF_Data)& aTarget,
 const Handle(MDF_ASDriverTable)& aDriverTable,
 const Handle(MDF_SRelocationTable)& aReloc,
 const Standard_Integer aVersion) 
{
  // Into Data.
  aTarget = new PDF_Data(aVersion);

  // Extraction of the driver subset.
  const MDF_TypeASDriverMap& driverMap =
    aDriverTable->GetDrivers(aVersion);

  // Translation.
  MDF_Tool::WriteLabels(aSource, aTarget, driverMap, aReloc);
  MDF_Tool::WriteAttributes(driverMap, aReloc);
}


//=======================================================================
//function : FromTo
//purpose  : From PERSISTENT to TRANSIENT.
//=======================================================================

void MDF::FromTo
(const Handle(PDF_Data)& aSource,
 Handle(TDF_Data)& aTarget,
 const Handle(MDF_ARDriverTable)& aDriverTable,
 const Handle(MDF_RRelocationTable)& aReloc) 
{
  // Version number.
  Standard_Integer version = aSource->VersionNumber();

  // Extraction of the driver subset.
  const MDF_TypeARDriverMap& driverMap =
    aDriverTable->GetDrivers(version);

  // Translation.
  MDF_Tool::ReadLabels(aSource, aTarget, driverMap, aReloc);
  MDF_Tool::ReadAttributes(driverMap, aReloc);
}

//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MDF::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& theDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  theDriverSeq->Append(new MDF_TagSourceStorageDriver(theMsgDriver)); 
  theDriverSeq->Append(new MDF_ReferenceStorageDriver(theMsgDriver));
}


//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MDF::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& theDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{
  theDriverSeq->Append(new MDF_TagSourceRetrievalDriver(theMsgDriver)); 
  theDriverSeq->Append(new MDF_ReferenceRetrievalDriver(theMsgDriver));
}

