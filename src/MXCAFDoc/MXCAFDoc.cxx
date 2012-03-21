// Created on: 2000-08-16
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <MXCAFDoc.ixx>
#include <MXCAFDoc_ColorToolRetrievalDriver.hxx>
#include <MXCAFDoc_ColorToolStorageDriver.hxx>
#include <MXCAFDoc_DocumentToolRetrievalDriver.hxx>
#include <MXCAFDoc_DocumentToolStorageDriver.hxx>
#include <MXCAFDoc_LocationRetrievalDriver.hxx>
#include <MXCAFDoc_LocationStorageDriver.hxx>
#include <MXCAFDoc_ColorRetrievalDriver.hxx>
#include <MXCAFDoc_ColorStorageDriver.hxx>
#include <MXCAFDoc_ShapeToolRetrievalDriver.hxx>
#include <MXCAFDoc_ShapeToolStorageDriver.hxx>
#include <MXCAFDoc_VolumeStorageDriver.hxx>
#include <MXCAFDoc_AreaStorageDriver.hxx>
#include <MXCAFDoc_CentroidStorageDriver.hxx>
#include <MXCAFDoc_VolumeRetrievalDriver.hxx>
#include <MXCAFDoc_AreaRetrievalDriver.hxx>
#include <MXCAFDoc_CentroidRetrievalDriver.hxx>
#include <MXCAFDoc_GraphNodeStorageDriver.hxx>
#include <MXCAFDoc_GraphNodeRetrievalDriver.hxx>
#include <MXCAFDoc_LayerToolRetrievalDriver.hxx>
#include <MXCAFDoc_LayerToolStorageDriver.hxx>
#include <MXCAFDoc_DatumRetrievalDriver.hxx>
#include <MXCAFDoc_DatumStorageDriver.hxx>
#include <MXCAFDoc_DimTolRetrievalDriver.hxx>
#include <MXCAFDoc_DimTolStorageDriver.hxx>
#include <MXCAFDoc_DimTolToolRetrievalDriver.hxx>
#include <MXCAFDoc_DimTolToolStorageDriver.hxx>
#include <MXCAFDoc_MaterialRetrievalDriver.hxx>
#include <MXCAFDoc_MaterialStorageDriver.hxx>
#include <MXCAFDoc_MaterialToolRetrievalDriver.hxx>
#include <MXCAFDoc_MaterialToolStorageDriver.hxx>


//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MXCAFDoc::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& aDriverSeq,
 const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MXCAFDoc_LocationStorageDriver    (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_ColorStorageDriver       (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_VolumeStorageDriver      (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_AreaStorageDriver        (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_CentroidStorageDriver    (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_ColorToolStorageDriver   (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DocumentToolStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_ShapeToolStorageDriver   (theMsgDriver));

  aDriverSeq->Append(new MXCAFDoc_LayerToolStorageDriver   (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_GraphNodeStorageDriver   (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DatumStorageDriver       (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DimTolStorageDriver      (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DimTolToolStorageDriver  (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_MaterialStorageDriver    (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_MaterialToolStorageDriver(theMsgDriver));
}

//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MXCAFDoc::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& aDriverSeq,
 const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MXCAFDoc_LocationRetrievalDriver    (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_ColorRetrievalDriver       (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_VolumeRetrievalDriver      (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_AreaRetrievalDriver        (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_CentroidRetrievalDriver    (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_ColorToolRetrievalDriver   (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DocumentToolRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_ShapeToolRetrievalDriver   (theMsgDriver));
  
  aDriverSeq->Append(new MXCAFDoc_LayerToolRetrievalDriver   (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_GraphNodeRetrievalDriver   (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DatumRetrievalDriver       (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DimTolRetrievalDriver      (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_DimTolToolRetrievalDriver  (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_MaterialRetrievalDriver    (theMsgDriver));
  aDriverSeq->Append(new MXCAFDoc_MaterialToolRetrievalDriver(theMsgDriver));
}
