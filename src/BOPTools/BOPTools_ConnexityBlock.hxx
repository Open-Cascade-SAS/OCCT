// Created by: Peter KURNEV
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


#ifndef BOPTools_ConnexityBlock_HeaderFile
#define BOPTools_ConnexityBlock_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#include <NCollection_BaseAllocator.hxx>
#include <BOPCol_ListOfShape.hxx>

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
    myRegular(Standard_True),
    myAllocator(theAllocator),
    myShapes(myAllocator),
    myLoops(myAllocator)  {
  };
  //
  const BOPCol_ListOfShape& Shapes()const {
    return myShapes;
  };
  //
  BOPCol_ListOfShape& ChangeShapes() {
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
  const BOPCol_ListOfShape& Loops()const {
    return myLoops;
  };
  //
  BOPCol_ListOfShape& ChangeLoops() {
    return myLoops;
  };
  //
 protected:
  Standard_Boolean myRegular;
  Handle(NCollection_BaseAllocator) myAllocator;
  BOPCol_ListOfShape myShapes;
  BOPCol_ListOfShape myLoops;
};


#endif
