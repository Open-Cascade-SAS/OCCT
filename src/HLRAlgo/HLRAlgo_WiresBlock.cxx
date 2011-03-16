// File:	HLRAlgo_WiresBlock.cxx
// Created:	Fri Mar 27 17:08:48 1992
// Author:	Christophe MARION
//		<cma@sdsun2>
#ifndef No_Exception
#define No_Exception
#endif
#include <HLRAlgo_WiresBlock.ixx>

//=======================================================================
//function : HLRAlgo_WiresBlock
//purpose  : 
//=======================================================================

HLRAlgo_WiresBlock::HLRAlgo_WiresBlock (const Standard_Integer NbWires) :
myWires(1,NbWires)
{}

//=======================================================================
//function : NbWires
//purpose  : 
//=======================================================================

Standard_Integer HLRAlgo_WiresBlock::NbWires () const
{ return myWires.Upper(); }

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void HLRAlgo_WiresBlock::Set (const Standard_Integer I,
			      const Handle(HLRAlgo_EdgesBlock)& W)
{ myWires (I) = W; }

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

Handle(HLRAlgo_EdgesBlock) &
HLRAlgo_WiresBlock::Wire (const Standard_Integer I)
{ return *((Handle(HLRAlgo_EdgesBlock)*) &myWires(I)); }

//=======================================================================
//function : UpdateMinMax
//purpose  : 
//=======================================================================

void HLRAlgo_WiresBlock::UpdateMinMax (const Standard_Address TotMinMax)
{
  for (Standard_Integer i = 0; i <= 15; i++)
    myMinMax[i] = ((Standard_Integer*)TotMinMax)[i];
}

