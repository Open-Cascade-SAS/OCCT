// File:      HLRAlgo_Coincidence.cxx
// Created:   Wed Feb 19 14:31:45 1992
// Author:    Christophe MARION
// Copyright: OPEN CASCADE 2000

#include <HLRAlgo_Coincidence.ixx>

//=======================================================================
//function : HLRAlgo_Coincidence
//purpose  : 
//=======================================================================

HLRAlgo_Coincidence::HLRAlgo_Coincidence()
 : myFE(0),
   myParam(0.),
   myStBef(TopAbs_IN),
   myStAft(TopAbs_IN)
{
}
