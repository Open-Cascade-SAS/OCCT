// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_Splitter_HeaderFile
#define _BOPAlgo_Splitter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Builder.hxx>

//! The class is a General Fuse (GF) based algorithm.<br>
//! It provides means to split an arbitrary number of shapes of arbitrary
//! dimensions by other arbitrary shapes.<br>
//! The arguments of the operation are divided on two groups - Objects
//! (shapes that will be split) and Tools (shapes by which the objects will be split).<br>
//! The result of the operation contains only the split parts of the Objects.<br>
//! The split parts of the Tools are excluded from the result.<br>
//! If there are no Tool shapes, the operation will be equivalent to General Fuse.<br>
//!
//! All options of the General Fuse algorithm, such as Fuzzy mode, safe mode, parallel
//! mode, gluing mode and history support are also available in this algorithm.<br>
//!
//! The implementation of the algorithm is minimal - only the methods
//! CheckData() and Perform() have been overridden.<br>
//! The method BOPAlgo_Builder::BuildResult(), which adds the split parts of the arguments
//! into result, does not have to be overridden, because its native implementation
//! performs the necessary actions for the Splitter algorithm - it adds
//! the split parts of only Objects into result, avoiding the split parts of Tools.
class BOPAlgo_Splitter : public BOPAlgo_Builder
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BOPAlgo_Splitter();
  Standard_EXPORT virtual ~BOPAlgo_Splitter();

  Standard_EXPORT BOPAlgo_Splitter(const BOPCol_BaseAllocator& theAllocator);

  //! Clears internal fields and arguments
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Adds Tool argument of the operation
  Standard_EXPORT virtual void AddTool(const TopoDS_Shape& theShape);

  //! Adds the Tool arguments of the operation
  Standard_EXPORT virtual void SetTools(const BOPCol_ListOfShape& theShapes);

  //! Returns the Tool arguments of the operation
  const BOPCol_ListOfShape& Tools() const
  {
    return myTools;
  }

  //! Performs the operation
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;

protected:

  //! Checks the input data
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

  BOPCol_ListOfShape myTools;
  BOPCol_MapOfShape myMapTools;

private:

};

#endif // _BOPAlgo_Splitter_HeaderFile
