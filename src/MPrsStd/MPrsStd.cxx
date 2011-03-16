// File:	MPrsStd.cxx
// Created:	Tue Aug 26 17:34:14 1997
// Author:	Guest Design
//		<g_design@hankox.paris1.matra-dtv.fr>


#include <MPrsStd.ixx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_ARDriverHSequence.hxx>
#include <MPrsStd_AISPresentationStorageDriver.hxx>
#include <MPrsStd_AISPresentationRetrievalDriver.hxx>
#include <MPrsStd_AISPresentationRetrievalDriver_1.hxx>
//#include <MPrsStd_AISViewerStorageDriver.hxx>
//#include <MPrsStd_AISViewerRetrievalDriver.hxx> 
#include <MPrsStd_PositionStorageDriver.hxx>
#include <MPrsStd_PositionRetrievalDriver.hxx>

//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MPrsStd::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MPrsStd_AISPresentationStorageDriver(theMsgDriver));
//  aDriverSeq->Append(new MPrsStd_AISViewerStorageDriver());
  aDriverSeq->Append(new MPrsStd_PositionStorageDriver(theMsgDriver));
}


//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MPrsStd::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MPrsStd_AISPresentationRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MPrsStd_AISPresentationRetrievalDriver_1(theMsgDriver));
//  aDriverSeq->Append(new MPrsStd_AISViewerRetrievalDriver());
  aDriverSeq->Append(new MPrsStd_PositionRetrievalDriver(theMsgDriver));
}


