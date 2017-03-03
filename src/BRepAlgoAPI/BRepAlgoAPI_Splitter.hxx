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

#ifndef _BRepAlgoAPI_Splitter_HeaderFile
#define _BRepAlgoAPI_Splitter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAlgoAPI_BuilderAlgo.hxx>

//! The class contains API level of the Splitter algorithm.<br>
//! It provides means to split an arbitrary number of shapes of arbitrary
//! dimensions by other arbitrary shapes.<br>
//! The arguments of the operation are divided on two groups - Objects
//! (shapes that will be split) and Tools (shapes by which the objects will be split).<br>
//! The result of the operation contains only the split parts of the Objects.<br>
//! The split parts of the Tools are excluded from the result.<br>
//! If there are no Tool shapes, the operation will be equivalent to General Fuse.<br>
//!
//! The algorithm returns the following Error statuses:<br>
//! - 0 - in case of success;<br>
//! - 1 - in case there is no enough arguments for the operation;<br>
//! - 2 - in case the Intersection of the arguments has failed;<br>
//! - 3 - in case the Building of the result has failed.
class BRepAlgoAPI_Splitter : public BRepAlgoAPI_BuilderAlgo
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_Splitter();
  Standard_EXPORT virtual ~BRepAlgoAPI_Splitter();

  //! Constructor with already filled PaveFiller
  Standard_EXPORT BRepAlgoAPI_Splitter(const BOPAlgo_PaveFiller& thePF);

  //! Performs the algorithm.<br>
  //! Performs the intersection of the objects with tools and build the result of the operation.
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;

  //! Sets the tools
  Standard_EXPORT void SetTools (const TopTools_ListOfShape& theLS);
  
  //! Gets the tools
  Standard_EXPORT const TopTools_ListOfShape& Tools() const;

protected:

  //! Tools arguments of the operation
  TopTools_ListOfShape myTools;

private:

};

#endif // _BRepAlgoAPI_BuilderAlgo_HeaderFile
