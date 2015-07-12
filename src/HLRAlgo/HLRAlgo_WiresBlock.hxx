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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_Array1OfTransient.hxx>
#include <Standard_Integer.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Address.hxx>
class HLRAlgo_EdgesBlock;


class HLRAlgo_WiresBlock;
DEFINE_STANDARD_HANDLE(HLRAlgo_WiresBlock, MMgt_TShared)

//! A WiresBlock is a set of Blocks. It is used by the
//! DataStructure to structure the Edges.
//!
//! A WiresBlock contains :
//!
//! * An Array  of Blocks.
class HLRAlgo_WiresBlock : public MMgt_TShared
{

public:

  
  //! Create a Block of Blocks.
  Standard_EXPORT HLRAlgo_WiresBlock(const Standard_Integer NbWires);
  
  Standard_EXPORT Standard_Integer NbWires() const;
  
  Standard_EXPORT void Set (const Standard_Integer I, const Handle(HLRAlgo_EdgesBlock)& W);
  
  Standard_EXPORT Handle(HLRAlgo_EdgesBlock)& Wire (const Standard_Integer I);
  
  Standard_EXPORT void UpdateMinMax (const Standard_Address TotMinMax);
  
    Standard_Address MinMax() const;




  DEFINE_STANDARD_RTTI(HLRAlgo_WiresBlock,MMgt_TShared)

protected:




private:


  TColStd_Array1OfTransient myWires;
  Standard_Integer myMinMax[16];


};


#include <HLRAlgo_WiresBlock.lxx>





#endif // _HLRAlgo_WiresBlock_HeaderFile
