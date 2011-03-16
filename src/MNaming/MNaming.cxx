// File:	MNaming.cxx
//      	-----------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Aug  4 1997	Creation



#include <MNaming.ixx>

#include <MNaming_NamedShapeStorageDriver.hxx>
#include <MNaming_NamedShapeRetrievalDriver.hxx>
#include <MNaming_NamingStorageDriver.hxx>
#include <MNaming_NamingRetrievalDriver.hxx>
#include <MNaming_NamingRetrievalDriver_1.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MNaming::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MNaming_NamedShapeStorageDriver(theMsgDriver)); 
  aDriverSeq->Append(new MNaming_NamingStorageDriver(theMsgDriver));
}


//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MNaming::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MNaming_NamedShapeRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MNaming_NamingRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MNaming_NamingRetrievalDriver_1(theMsgDriver));
}
