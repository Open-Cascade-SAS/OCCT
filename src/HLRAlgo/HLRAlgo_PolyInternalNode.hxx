// Created on: 1997-01-10
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _HLRAlgo_PolyInternalNode_HeaderFile
#define _HLRAlgo_PolyInternalNode_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Address.hxx>


class HLRAlgo_PolyInternalNode;
DEFINE_STANDARD_HANDLE(HLRAlgo_PolyInternalNode, MMgt_TShared)

//! to Update OutLines.
class HLRAlgo_PolyInternalNode : public MMgt_TShared
{

public:

  
    HLRAlgo_PolyInternalNode();
  
    Standard_Address Indices() const;
  
    Standard_Address RValues() const;




  DEFINE_STANDARD_RTTI(HLRAlgo_PolyInternalNode,MMgt_TShared)

protected:




private:


  Standard_Integer myIndices[4];
  Standard_Real myRValues[11];


};


#include <HLRAlgo_PolyInternalNode.lxx>





#endif // _HLRAlgo_PolyInternalNode_HeaderFile
