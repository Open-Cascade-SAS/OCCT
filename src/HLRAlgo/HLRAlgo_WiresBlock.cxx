// Created on: 1992-03-27
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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
