// Created on: 1996-12-03
// Created by: Christophe MARION
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _HLRAlgo_PolyInternalSegment_HeaderFile
#define _HLRAlgo_PolyInternalSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Address.hxx>


//! to Update OutLines.
class HLRAlgo_PolyInternalSegment 
{
public:

  DEFINE_STANDARD_ALLOC

  
    HLRAlgo_PolyInternalSegment();
  
    Standard_Address Indices() const;




protected:





private:



  Standard_Integer myIndices[6];


};


#include <HLRAlgo_PolyInternalSegment.lxx>





#endif // _HLRAlgo_PolyInternalSegment_HeaderFile
