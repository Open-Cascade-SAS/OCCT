// Created on: 1992-04-06
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

#ifndef _HLRAlgo_WiresBlock_HeaderFile
#define _HLRAlgo_WiresBlock_HeaderFile

#include <HLRAlgo_EdgesBlock.hxx>

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_Array1OfTransient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>


class HLRAlgo_WiresBlock;
DEFINE_STANDARD_HANDLE(HLRAlgo_WiresBlock, Standard_Transient)

//! A WiresBlock is a set of Blocks. It is used by the
//! DataStructure to structure the Edges.
//!
//! A WiresBlock contains :
//!
//! * An Array  of Blocks.
class HLRAlgo_WiresBlock : public Standard_Transient
{
public:
  //! Create a Block of Blocks.
  HLRAlgo_WiresBlock(const Standard_Integer NbWires) :
    myWires(1,NbWires)
  {
  }
  
  Standard_Integer NbWires() const
  {
    return myWires.Upper();
  }
  
  void Set (const Standard_Integer I, const Handle(HLRAlgo_EdgesBlock)& W)
  {
    myWires (I) = W;
  }
  
  Handle(HLRAlgo_EdgesBlock)& Wire (const Standard_Integer I)
  {
    return *((Handle(HLRAlgo_EdgesBlock)*) &myWires(I));
  }
  
  void UpdateMinMax (const HLRAlgo_EdgesBlock::MinMaxIndices& theMinMaxes)
  {myMinMax = theMinMaxes;}

  HLRAlgo_EdgesBlock::MinMaxIndices& MinMax()
  {
    return myMinMax;
  }

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_WiresBlock,Standard_Transient)

private:
  TColStd_Array1OfTransient myWires;
  HLRAlgo_EdgesBlock::MinMaxIndices myMinMax;
};

#endif // _HLRAlgo_WiresBlock_HeaderFile
