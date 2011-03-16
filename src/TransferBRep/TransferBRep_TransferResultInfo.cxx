// File:	TransferBRep_TransferResultInfo.cxx
// Created:	Wed Aug 11 10:03:01 1999
// Author:	Roman LYGIN
//		<rln@kinox>


#include <TransferBRep_TransferResultInfo.ixx>

//=======================================================================
//function : TransferBRep_TransferResultInfo
//purpose  : 
//=======================================================================

TransferBRep_TransferResultInfo::TransferBRep_TransferResultInfo()
{
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

 void TransferBRep_TransferResultInfo::Clear ()
{
    myNR = myNRW = myNRF = myNRWF =
    myR  = myRW  = myRF  = myRWF  = 0;
}
