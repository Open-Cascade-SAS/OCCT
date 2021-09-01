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

//! The class contains API level of the **Splitter** algorithm,
//! which allows splitting a group of arbitrary shapes by the
//! other group of arbitrary shapes.<br>
//! The arguments of the operation are divided on two groups:<br>
//! *Objects* - shapes that will be split;<br>
//! *Tools*   - shapes by which the *Objects* will be split.<br>
//! The result of the operation contains only the split parts
//! of the shapes from the group of *Objects*.<br>
//! The split parts of the shapes from the group of *Tools* are excluded
//! from the result.<br>
//! The shapes can be split by the other shapes from the same group
//! (in case these shapes are interfering).
//!
//! The class is a General Fuse based algorithm. Thus, all options
//! of the General Fuse algorithm such as Fuzzy mode, safe processing mode,
//! parallel processing mode, gluing mode and history support are also
//! available in this algorithm.<br>
//! There is no requirement on the existence of the *Tools* shapes.
//! And if there are no *Tools* shapes, the result of the splitting
//! operation will be equivalent to the General Fuse result.
//!
//! The algorithm returns the following Error statuses:<br>
//! - 0 - in case of success;<br>
//! - *BOPAlgo_AlertTooFewArguments*    - in case there is no enough arguments for the operation;<br>
//! - *BOPAlgo_AlertIntersectionFailed* - in case the Intersection of the arguments has failed;<br>
//! - *BOPAlgo_AlertBuilderFailed*      - in case the Building of the result has failed.
class BRepAlgoAPI_Splitter : public BRepAlgoAPI_BuilderAlgo
{
public:

  DEFINE_STANDARD_ALLOC

public: //! @name Constructors

  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_Splitter();

  //! Constructor with already prepared intersection tool - PaveFiller
  Standard_EXPORT BRepAlgoAPI_Splitter(const BOPAlgo_PaveFiller& thePF);


public: //! @name Setters/Getters for the Tools

  //! Sets the Tool arguments
  void SetTools (const TopTools_ListOfShape& theLS)
  {
    myTools = theLS;
  }

  //! Returns the Tool arguments
  const TopTools_ListOfShape& Tools() const
  {
    return myTools;
  }


public: //! @name Performing the operation

  //! Performs the Split operation.
  //! Performs the intersection of the argument shapes (both objects and tools)
  //! and splits objects by the tools.
  Standard_EXPORT virtual void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;


protected: //! @name Fields

  TopTools_ListOfShape myTools; //!< Tool arguments of the operation

};

#endif // _BRepAlgoAPI_Splitter_HeaderFile
