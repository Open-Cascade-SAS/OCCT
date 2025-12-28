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
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

//=================================================================================================

class BOPTools_ConnexityBlock
{
public:
  BOPTools_ConnexityBlock()
      : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
        myRegular(true),
        myShapes(myAllocator),
        myLoops(myAllocator) {};
  //
  BOPTools_ConnexityBlock(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myAllocator(theAllocator),
        myRegular(true),
        myShapes(myAllocator),
        myLoops(myAllocator) {};

  //
  const NCollection_List<TopoDS_Shape>& Shapes() const { return myShapes; };

  //
  NCollection_List<TopoDS_Shape>& ChangeShapes() { return myShapes; };

  //
  void SetRegular(const bool theFlag) { myRegular = theFlag; }

  //
  bool IsRegular() const { return myRegular; }

  //
  const NCollection_List<TopoDS_Shape>& Loops() const { return myLoops; };

  //
  NCollection_List<TopoDS_Shape>& ChangeLoops() { return myLoops; };

  //
protected:
  occ::handle<NCollection_BaseAllocator> myAllocator;
  bool                  myRegular;
  NCollection_List<TopoDS_Shape>              myShapes;
  NCollection_List<TopoDS_Shape>              myLoops;
};

#endif
