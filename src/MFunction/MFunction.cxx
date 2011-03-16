// File:	MFunction.cxx
// Created:	Thu Jun 24 09:34:40 1999
// Author:	Vladislav ROMASHKO
//		<vro@flox.nnov.matra-dtv.fr>


#include <MFunction.ixx>

#include <MDF_ARDriverHSequence.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <CDM_MessageDriver.hxx>
#include <MFunction_FunctionStorageDriver.hxx>
#include <MFunction_FunctionRetrievalDriver.hxx>


//=======================================================================
//function : AddStorageDrivers
//purpose  : 
//=======================================================================

void MFunction::AddStorageDrivers(const Handle(MDF_ASDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{ 
  aDriverSeq->Append(new MFunction_FunctionStorageDriver(theMsgDriver));
}

//=======================================================================
//function : AddRetrievalDrivers
//purpose  : 
//=======================================================================

void MFunction::AddRetrievalDrivers(const Handle(MDF_ARDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MFunction_FunctionRetrievalDriver(theMsgDriver));
}
