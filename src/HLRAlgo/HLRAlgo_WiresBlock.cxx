// Created on: 1992-03-27
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef No_Exception
#define No_Exception
#endif


#include <HLRAlgo_EdgesBlock.hxx>
#include <HLRAlgo_WiresBlock.hxx>
#include <Standard_Type.hxx>

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
