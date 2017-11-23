// Created by: Peter KURNEV
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

#ifndef BOPTools_ConnexityBlock_HeaderFile
#define BOPTools_ConnexityBlock_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//class : ConnexityBlock
//purpose  : 
//=======================================================================
class BOPTools_ConnexityBlock {
 public:
  BOPTools_ConnexityBlock()    : 
    myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
    myRegular(Standard_True),
    myShapes(myAllocator),
    myLoops(myAllocator)    {
  };
  //
  BOPTools_ConnexityBlock(const Handle(NCollection_BaseAllocator)& theAllocator):
	myAllocator(theAllocator),  
	myRegular(Standard_True),
    myShapes(myAllocator),
    myLoops(myAllocator)  {
  };
  //
  const TopTools_ListOfShape& Shapes()const {
    return myShapes;
  };
  //
  TopTools_ListOfShape& ChangeShapes() {
    return myShapes;
  };
  //
  void SetRegular(const Standard_Boolean theFlag) {
    myRegular=theFlag;
  }
  //
  Standard_Boolean IsRegular()const {
    return myRegular;
  }
  //
  const TopTools_ListOfShape& Loops()const {
    return myLoops;
  };
  //
  TopTools_ListOfShape& ChangeLoops() {
    return myLoops;
  };
  //
 protected:
  Handle(NCollection_BaseAllocator) myAllocator;
  Standard_Boolean myRegular;
  TopTools_ListOfShape myShapes;
  TopTools_ListOfShape myLoops;
};


#endif
