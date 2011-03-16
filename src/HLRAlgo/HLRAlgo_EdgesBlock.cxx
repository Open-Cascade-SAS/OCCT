// File:	HLRAlgo_EdgesBlock.cxx
// Created:	Fri Mar 27 17:08:48 1992
// Author:	Christophe MARION
//		<cma@sdsun2>
#ifndef No_Exception
#define No_Exception
#endif
#include <HLRAlgo_EdgesBlock.ixx>

//=======================================================================
//function : HLRAlgo_EdgesBlock
//purpose  : 
//=======================================================================

HLRAlgo_EdgesBlock::HLRAlgo_EdgesBlock (const Standard_Integer NbEdges) :
  myEdges(1,NbEdges), myFlags(1,NbEdges)
{}

//=======================================================================
//function : UpdateMinMax
//purpose  : 
//=======================================================================

void HLRAlgo_EdgesBlock::UpdateMinMax (const Standard_Address TotMinMax)
{
  for (Standard_Integer i = 0; i <= 15; i++)
    myMinMax[i] = ((Standard_Integer*)TotMinMax)[i];
}

